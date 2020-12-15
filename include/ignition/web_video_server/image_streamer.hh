#ifndef IGNITION__WEB_VIDEO_SERVER__IMAGE_STREAMER_HH_
#define IGNITION__WEB_VIDEO_SERVER__IMAGE_STREAMER_HH_

#include "ignition/async_web_server_cpp/http_connection.hh"
#include "ignition/async_web_server_cpp/http_request.hh"

namespace ignition {
namespace web_video_server {

class ImageStreamer
{
  public: ImageStreamer(const async_web_server_cpp::HttpRequest &_request,
                        async_web_server_cpp::HttpConnectionPtr _connection);

  public: virtual ~ImageStreamer() = default;
public: virtual void Start() = 0;

  public: bool IsInactive() { return inactive; }

  protected: async_web_server_cpp::HttpConnectionPtr connection;
  protected: async_web_server_cpp::HttpRequest request;
  protected: bool inactive {false};
};

using ImageStreamerPtr = std::shared_ptr<ImageStreamer>;

class ImageStreamerType
{
  public: virtual ImageStreamerPtr create_streamer(
              const async_web_server_cpp::HttpRequest &request,
              async_web_server_cpp::HttpConnectionPtr connection) = 0;

  public: virtual std::string create_viewer(
              const async_web_server_cpp::HttpRequest &request) = 0;
};

}  // namespace web_video_server
}  // namespace ignition

#endif  // IGNITION__WEB_VIDEO_SERVER__IMAGE_STREAMER_HH_
