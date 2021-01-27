#include "ignition/web_video_server/libav_streamer.hh"

#include "ignition/async_web_server_cpp/http_reply.hh"


#include <cstdint>
#include <mutex>
#include <vector>

#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER

namespace ignition {
namespace web_video_server {

#if ( LIBAVCODEC_VERSION_INT  < AV_VERSION_INT(58,9,100) )
static int ffmpeg_mutex_lock_manager(void ** mutex, enum AVLockOp op)
{
  if (nullptr== mutex)
    return -1;

  switch (op)
  {
    case AV_LOCK_CREATE:
    {
      *mutex = nullptr;
      std::mutex *m = new std::mutex();
      *mutex = static_cast<void *>(m);
      break;
    }
    case AV_LOCK_OBTAIN:
    {
      std::mutex *m = static_cast<std::mutex *>(*mutex);
      m->lock();
      break;
    }
    case AV_LOCK_RELEASE:
    {
      std::mutex *m = static_cast<std::mutex *>(*mutex);
      m->unlock();
      break;
    }
    case AV_LOCK_DESTROY:
    {
      std::mutex *m = static_cast<std::mutex *>(*mutex);
      m->lock();
      m->unlock();
      delete m;
      break;
    }
    default:
      break;
  }
  return 0;
}
#endif

LibavStreamer::LibavStreamer(const async_web_server_cpp::HttpRequest &_request,
                             async_web_server_cpp::HttpConnectionPtr _connection,
                             const std::string &_formatName,
                             const std::string &_codecName,
                             const std::string &_contentType):
  IgnTransportImageStreamer(_request, _connection),
  format_name(_formatName),
  codec_name(_codecName),
  content_type(_contentType)
{

  auto get_int = [_request](const std::string& param, int defaultValue) {
    auto string = _request.get_query_param_value_or_default(param, std::to_string(defaultValue));
    return std::stoi(string);
  };

  bitrate = get_int("bitrate", 100000);
  qmin = get_int("qmin", 10);
  qmax = get_int("qmax", 42);
  gop = get_int("gop", 25);

#if ( LIBAVCODEC_VERSION_INT  < AV_VERSION_INT(58,9,100) )
  av_lockmgr_register(&ffmpeg_mutex_lock_manager);
  av_register_all();
#endif
}

LibavStreamer::~LibavStreamer()
{
  if (codec_context)
  {
    #if ( LIBAVCODEC_VERSION_INT  < AV_VERSION_INT(58,9,100) )
    avcodec_close(codec_context);
    #else
    avcodec_free_context(&codec_context);
    #endif
  }

  if (frame)
  {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
    av_free(frame);
    frame = nullptr;
#else
    av_frame_free(&frame);
#endif
  }

  if (io_buffer)
    delete io_buffer;

  if (format_context) {
    if (format_context->pb)
      av_free(format_context->pb);
    avformat_free_context(format_context);
  }

  if (sws_context)
    sws_freeContext(sws_context);
}

void LibavStreamer::initializeEncoder()
{

}

// output callback for ffmpeg IO context
static int dispatch_output_packet(void* opaque, uint8_t* buffer, int buffer_size)
{
  igndbg << "dispatch_output_packet: " << buffer_size << std::endl;
  async_web_server_cpp::HttpConnectionPtr connection = *((async_web_server_cpp::HttpConnectionPtr*) opaque);
  std::vector<uint8_t> encoded_frame;
  encoded_frame.assign(buffer, buffer + buffer_size);
  connection->write_and_clear(encoded_frame);
  return 0; // TODO: can this fail?
}

void LibavStreamer::Initialize(const ignition::msgs::Image &_msg)
{
  auto output_width = _msg.width();
  auto output_height = _msg.height();

  // Load format
  format_context = avformat_alloc_context();
  if (!format_context)
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::internal_server_error)(request,
                                                                                                         connection,
                                                                                                         NULL, NULL);
    throw std::runtime_error("Error allocating ffmpeg format context");
  }

  output_format = av_guess_format(format_name.c_str(), NULL, NULL);
  if (!output_format)
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::internal_server_error)(request,
                                                                                                         connection,
                                                                                                         NULL, NULL);
    throw std::runtime_error("Error looking up output format");
  }

  format_context->oformat = output_format;

  // Set up custom IO callback.
  size_t io_buffer_size = 3 * 1024;    // 3M seen elsewhere and adjudged good
  io_buffer = new unsigned char[io_buffer_size];
  AVIOContext* io_ctx = avio_alloc_context(io_buffer, io_buffer_size, AVIO_FLAG_WRITE, &connection, NULL, dispatch_output_packet, NULL);

  if (!io_ctx)
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::internal_server_error)(request,
                                                                                                         connection,
                                                                                                         NULL, NULL);
    throw std::runtime_error("Error setting up IO context");
  }
  io_ctx->seekable = 0;                       // no seeking, it's a stream
  format_context->pb = io_ctx;
  format_context->max_interleave_delta = 0;
  output_format->flags |= AVFMT_FLAG_CUSTOM_IO;
  output_format->flags |= AVFMT_NOFILE;

  // Load codec
  if (codec_name.empty()) // use default codec if none specified
    codec = avcodec_find_encoder(output_format->video_codec);
  else
    codec = avcodec_find_encoder_by_name(codec_name.c_str());

  if (!codec)
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::internal_server_error)(request,
                                                                                                         connection,
                                                                                                         NULL, NULL);
    throw std::runtime_error("Error looking up codec");
  }

  video_stream = avformat_new_stream(format_context, codec);
  if (!video_stream)
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::internal_server_error)(request,
                                                                                                         connection,
                                                                                                         NULL, NULL);
    throw std::runtime_error("Error creating video stream");
  }

  #if ( LIBAVCODEC_VERSION_INT  < AV_VERSION_INT(58,9,100) )
  codec_context = video_stream->codec;
  #else
  codec_context = avcodec_alloc_context3(codec);
  #endif

  // Set options
  avcodec_get_context_defaults3(codec_context, codec);

  codec_context->codec_id = codec->id;
  codec_context->bit_rate = bitrate;

  codec_context->width = output_width;
  codec_context->height = output_height;
  codec_context->delay = 0;

  video_stream->time_base.num = 1;
  video_stream->time_base.den = 1000;

  codec_context->time_base.num = 1;
  codec_context->time_base.den = 1;
  codec_context->gop_size = gop;
  codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
  codec_context->max_b_frames = 0;

  // Quality settings
  codec_context->qmin = qmin;
  codec_context->qmax = qmax;

  #if ( LIBAVCODEC_VERSION_INT  >= AV_VERSION_INT(58,9,100) )
  avcodec_parameters_from_context(video_stream->codecpar, codec_context);
  #endif

  initializeEncoder();

  codec_context->flags |= AV_CODEC_FLAG_LOW_DELAY;
  codec_context->max_b_frames = 0;

  // Open Codec
  if (avcodec_open2(codec_context, codec, NULL) < 0)
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::internal_server_error)(request,
                                                                                                         connection,
                                                                                                         NULL, NULL);
    throw std::runtime_error("Could not open video codec");
  }

  // Allocate frame buffers
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
  frame = avcodec_alloc_frame();
#else
  frame = av_frame_alloc();
#endif
  av_image_alloc(frame->data, frame->linesize, output_width, output_height,
          codec_context->pix_fmt, 1);

  frame->width = output_width;
  frame->height = output_height;
  frame->format = codec_context->pix_fmt;
  output_format->flags |= AVFMT_NOFILE;

  // define meta data
  av_dict_set(&format_context->metadata, "author", "ROS web_video_server", 0);
  av_dict_set(&format_context->metadata, "title", topic.c_str(), 0);

  // Send response headers
  async_web_server_cpp::HttpReply::builder(async_web_server_cpp::HttpReply::ok)
    .header("Connection", "close")
    .header("Server", "web_video_server")
    .header("Cache-Control","no-cache, no-store, must-revalidate, pre-check=0, post-check=0, max-age=0")
    .header("Pragma", "no-cache")
    .header("Expires", "0")
    .header("Max-Age", "0")
    .header("Trailer", "Expires")
    .header("Content-type", content_type)
    .header("Access-Control-Allow-Origin", "*")
    .write(connection);

  // Send video stream header
  if (avformat_write_header(format_context, &opt) < 0)
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::internal_server_error)(request,
                                                                                                         connection,
                                                                                                         NULL, NULL);
    throw std::runtime_error("Error openning dynamic buffer");
  }
}


void LibavStreamer::SendImage(const ignition::msgs::Image &_msg)
{
  igndbg << "LibavStreamer::SendImage" << std::endl;
  auto output_width = _msg.width();
  auto output_height = _msg.height();
  std::vector<uint8_t> encoded_frame(6 * output_width * output_height);

  auto stamp = ignition::msgs::Convert(_msg.header().stamp()) +
    std::chrono::time_point<std::chrono::steady_clock>();

  if (!first_set)
  {
    first_set = true;
    first_image_timestamp = stamp;
  }

  std::scoped_lock lock(encode_mutex);
#if (LIBAVUTIL_VERSION_MAJOR < 53)
  PixelFormat input_coding_format = PIX_FMT_RGB24;
#else
  AVPixelFormat input_coding_format = AV_PIX_FMT_RGB24;
#endif

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
  AVPicture *raw_frame = new AVPicture;
  avpicture_fill(raw_frame, img.data, input_coding_format, output_width, output_height);
#else
  AVFrame *raw_frame = av_frame_alloc();
  av_image_fill_arrays(raw_frame->data, raw_frame->linesize,
                       (uint8_t*)&_msg.data()[0], input_coding_format, output_width, output_height, 1);
#endif

  if (!sws_context)
  {
    static int sws_flags = SWS_BICUBIC;
    sws_context = sws_getContext(
        output_width, output_height, input_coding_format,
        output_width, output_height, codec_context->pix_fmt,
        sws_flags, nullptr, nullptr, nullptr);

    if (!sws_context)
    {
      throw std::runtime_error("Could not initialize the conversion context");
    }
  }

  int ret = sws_scale(sws_context,
          (const uint8_t * const *)raw_frame->data,
          raw_frame->linesize,
          0, output_height,
          frame->data, frame->linesize);


#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
  delete raw_frame;
#else
  av_frame_free(&raw_frame);
#endif

  // Encode the frame
  AVPacket pkt;
  int got_packet;
  av_init_packet(&pkt);

#if (LIBAVCODEC_VERSION_MAJOR < 54)
  int buf_size = 6 * output_width * output_height;
  pkt.data = (uint8_t*)av_malloc(buf_size);
  pkt.size = avcodec_encode_video(codec_context, pkt.data, buf_size, frame);
  got_packet = pkt.size > 0;
#elif (LIBAVCODEC_VERSION_MAJOR < 57)
  pkt.data = NULL; // packet data will be allocated by the encoder
  pkt.size = 0;
  if (avcodec_encode_video2(codec_context, &pkt, frame, &got_packet) < 0)
  {
     throw std::runtime_error("Error encoding video frame");
  }
#else
  ret = avcodec_send_frame(codec_context, frame);
  if (ret == AVERROR_EOF)
  {
    igndbg << "avcodec_send_frame() encoder flushed\n";
  }
  else if (ret == AVERROR(EAGAIN))
  {
    igndbg << "avcodec_send_frame() need output read out\n";
  }
  if (ret < 0)
  {
    throw std::runtime_error("Error encoding video frame");
  }

  ret = avcodec_receive_packet(codec_context, &pkt);
  got_packet = pkt.size > 0;
  if (ret == AVERROR_EOF)
  {
    igndbg << "avcodec_recieve_packet() encoder flushed\n";
  }
  else if (ret == AVERROR(EAGAIN))
  {
    igndbg << "avcodec_recieve_packet() need more input\n";
    got_packet = 0;
  }
#endif

  if (got_packet)
  {
    std::chrono::duration<double, std::ratio<1,1>> diff = (stamp - first_image_timestamp);
    double seconds = diff.count();

    igndbg << "Diff: " << seconds << std::endl;

    // Encode video at 1/0.95 to minimize delay
    pkt.pts = (int64_t)(seconds / av_q2d(video_stream->time_base) * 0.95);
    if (pkt.pts <= 0)
      pkt.pts = 1;
    pkt.dts = AV_NOPTS_VALUE;

    if (pkt.flags&AV_PKT_FLAG_KEY)
      pkt.flags |= AV_PKT_FLAG_KEY;

    pkt.stream_index = video_stream->index;

    if (av_write_frame(format_context, &pkt))
    {
      throw std::runtime_error("Error when writing frame");
    }
  }
  else
  {
    igndbg << "encodedframe.clear()\n";
    encoded_frame.clear();
  }
#if LIBAVCODEC_VERSION_INT < 54
  av_free(pkt.data);
#endif

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
  av_free_packet(&pkt);
#else
  av_packet_unref(&pkt);
#endif

  connection->write_and_clear(encoded_frame);
}

LibavStreamerType::LibavStreamerType(const std::string &_format_name, const std::string &_codec_name,
                                     const std::string &_content_type) :
    format_name(_format_name), codec_name(_codec_name), content_type(_content_type)
{
}

std::shared_ptr<ImageStreamer>
LibavStreamerType::create_streamer(const async_web_server_cpp::HttpRequest &request,
                                   async_web_server_cpp::HttpConnectionPtr connection)
{
  return std::shared_ptr<ImageStreamer>(
      new LibavStreamer(request, connection, format_name, codec_name, content_type));
}

std::string LibavStreamerType::create_viewer(const async_web_server_cpp::HttpRequest &request)
{
  std::stringstream ss;
  ss << R"(<video width="320" height="240" autoplay>)";
  ss << R"(  <source src="/stream?)" << request.query << R"(">)";
  ss << R"(</video>)";
  return ss.str();
}

}  // namespace web_video_server
}  // namespace ignition
