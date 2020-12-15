#include "ignition/web_video_server/vp8_streamer.hh"

namespace ignition
{
namespace web_video_server
{

Vp8Streamer::Vp8Streamer(const async_web_server_cpp::HttpRequest &_request,
                         async_web_server_cpp::HttpConnectionPtr _connection) :
    LibavStreamer(_request, _connection, "webm", "libvpx", "video/webm")
{
  //quality_ = request.get_query_param_value_or_default("quality", "realtime");
}

void Vp8Streamer::initializeEncoder()
{
  typedef std::map<std::string, std::string> AvOptMap;
  AvOptMap av_opt_map;
  av_opt_map["quality"] = "realtime";
  av_opt_map["deadline"] = "1";
  av_opt_map["auto-alt-ref"] = "0";
  av_opt_map["lag-in-frames"] = "1";
  av_opt_map["rc_lookahead"] = "1";
  av_opt_map["drop_frame"] = "1";
  av_opt_map["error-resilient"] = "1";

  for (AvOptMap::iterator itr = av_opt_map.begin(); itr != av_opt_map.end(); ++itr)
  {
    av_opt_set(codec_context->priv_data, itr->first.c_str(), itr->second.c_str(), 0);
  }

  // Buffering settings
  int bufsize = 10;
  codec_context->rc_buffer_size = bufsize;
  codec_context->rc_initial_buffer_occupancy = bufsize; //bitrate/3;
  av_opt_set_int(codec_context->priv_data, "bufsize", bufsize, 0);
  av_opt_set_int(codec_context->priv_data, "buf-initial", bufsize, 0);
  av_opt_set_int(codec_context->priv_data, "buf-optimal", bufsize, 0);
  av_opt_set_int(codec_context->priv_data, "skip_threshold", 10, 0);
}

Vp8StreamerType::Vp8StreamerType() :
    LibavStreamerType("webm", "libvpx", "video/webm")
{
}

ImageStreamerPtr
Vp8StreamerType::create_streamer(const async_web_server_cpp::HttpRequest &_request,
                                 async_web_server_cpp::HttpConnectionPtr _connection)
{
  return std::shared_ptr<ImageStreamer>(new Vp8Streamer(_request, _connection));
}

}
}
