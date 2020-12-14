#include "ignition/web_video_server/image_streamer.hh"

namespace ignition {
namespace web_video_server {

ImageStreamer::ImageStreamer(
    const async_web_server_cpp::HttpRequest &_request,
    async_web_server_cpp::HttpConnectionPtr _connection):
  request(_request),
  connection(_connection),
  inactive(false)
{
}

}  // namespace web_video_server
}  // namespace ignition

