#ifndef IGNITION__WEB_VIDEO_SERVER__IGN_IMAGE_STREAMER_HH_
#define IGNITION__WEB_VIDEO_SERVER__IGN_IMAGE_STREAMER_HH_

#include "ignition/web_video_server/image_streamer.hh"

#include <ignition/msgs/image.pb.h>
#include <ignition/transport/Node.hh>

namespace ignition {
namespace web_video_server {

class IgnTransportImageStreamer: public ImageStreamer
{
  public: IgnTransportImageStreamer(const async_web_server_cpp::HttpRequest &_request,
                                    async_web_server_cpp::HttpConnectionPtr _connection);

  public: virtual ~IgnTransportImageStreamer() = default;

  public: void Start() override;

  private: void OnImageMsg(const ignition::msgs::Image &_msg);

  protected: virtual void SendImage(const ignition::msgs::Image &_msg) = 0;

  protected: virtual void Initialize();

  protected: std::string topic;

  private: bool initialized { false };

  private: ignition::transport::Node node;
};

}  // namespace web_video_server
}  // namespace ignition

#endif // IGNITION__WEB_VIDEO_SERVER__IMAGE_STREAMER_HH_

