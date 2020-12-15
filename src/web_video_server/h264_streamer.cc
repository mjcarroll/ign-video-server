#include "ignition/web_video_server/h264_streamer.hh"

namespace ignition
{
namespace web_video_server
{

H264Streamer::H264Streamer(const async_web_server_cpp::HttpRequest &_request,
                         async_web_server_cpp::HttpConnectionPtr _connection) :
    LibavStreamer(_request, _connection, "mp4", "libx264", "video/mp4")
{
}

void H264Streamer::initializeEncoder()
{
  av_opt_set(codec_context->priv_data, "preset", "medium", 0);
  av_opt_set(codec_context->priv_data, "tune", "zerolatency", 0);
  av_opt_set_int(codec_context->priv_data, "crf", 20, 0);
  av_opt_set_int(codec_context->priv_data, "bufsize", 100, 0);
  av_opt_set_int(codec_context->priv_data, "keyint", 30, 0);
  av_opt_set_int(codec_context->priv_data, "g", 1, 0);

  // container format options
  if (!strcmp(format_context->oformat->name, "mp4")) {
    // set up mp4 for streaming (instead of seekable file output)
    av_dict_set(&opt, "movflags", "+frag_keyframe+empty_moov+faststart", 0);
  }}

H264StreamerType::H264StreamerType() :
    LibavStreamerType("mp4", "libx264", "video/mp4")
{
}

ImageStreamerPtr
H264StreamerType::create_streamer(const async_web_server_cpp::HttpRequest &_request,
                                 async_web_server_cpp::HttpConnectionPtr _connection)
{
  return std::shared_ptr<ImageStreamer>(new H264Streamer(_request, _connection));
}

}
}
