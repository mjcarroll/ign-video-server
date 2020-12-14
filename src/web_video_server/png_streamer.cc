#include "ignition/web_video_server/png_streamer.hh"

#include <ignition/common/Image.hh>

namespace ignition {
namespace web_video_server {

PngStreamer::PngStreamer(const async_web_server_cpp::HttpRequest &_request,
                        async_web_server_cpp::HttpConnectionPtr _connection):
  IgnTransportImageStreamer(_request, _connection),
  stream(_connection)
{
  stream.sendInitialHeader();
}

void PngStreamer::SendImage(const ignition::msgs::Image &_msg)
{
  std::vector<unsigned char> buffer;

  common::Image image;
  image.SetFromData(
      (unsigned char*) _msg.data().c_str(),
      _msg.width(), _msg.height(), common::Image::RGB_INT8);
  image.SavePNGToBuffer(buffer);

  auto now = std::chrono::system_clock::now();

  stream.sendPartAndClear(now, "image/png", buffer);
}

ImageStreamerPtr
PngStreamerType::create_streamer(const async_web_server_cpp::HttpRequest &_request,
                                 async_web_server_cpp::HttpConnectionPtr _connection)
{
  return ImageStreamerPtr(new PngStreamer(_request, _connection));
}

std::string
PngStreamerType::create_viewer(const async_web_server_cpp::HttpRequest &request)
{
  std::stringstream ss;
  ss << "<img src=\"/stream?";
  ss << request.query;
  ss << "\"></img>";
  return ss.str();
}

} // namespace web_video_server
} // namespace ignition

