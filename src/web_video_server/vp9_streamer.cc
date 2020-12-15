#include "ignition/web_video_server/vp9_streamer.hh"

namespace ignition
{
namespace web_video_server
{

Vp9Streamer::Vp9Streamer(const async_web_server_cpp::HttpRequest &_request,
                         async_web_server_cpp::HttpConnectionPtr _connection) :
    LibavStreamer(_request, _connection, "webm", "libvpx-vp9", "video/webm")
{
  //quality_ = request.get_query_param_value_or_default("quality", "realtime");
}

void Vp9Streamer::initializeEncoder()
{
  av_opt_set(codec_context->priv_data, "quality", "realtime", 0);
  av_opt_set(codec_context->priv_data, "deadline", "realtime", 0);
  av_opt_set_int(codec_context->priv_data, "speed", 8, 0);
  av_opt_set_int(codec_context->priv_data, "lag-in-frames", 0, 0);


}

Vp9StreamerType::Vp9StreamerType() :
    LibavStreamerType("webm", "libvpx-vp9", "video/webm")
{
}

ImageStreamerPtr
Vp9StreamerType::create_streamer(const async_web_server_cpp::HttpRequest &_request,
                                 async_web_server_cpp::HttpConnectionPtr _connection)
{
  return std::shared_ptr<ImageStreamer>(new Vp9Streamer(_request, _connection));
}

}
}
