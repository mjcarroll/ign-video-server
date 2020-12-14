#ifndef IGNITION__WEB_VIDEO_SERVER__PNG_STREAMER_HH_
#define IGNITION__WEB_VIDEO_SERVER__PNG_STREAMER_HH_

#include "ignition/web_video_server/ign_image_streamer.hh"
#include "ignition/web_video_server/multipart_stream.hh"

namespace ignition
{
namespace web_video_server {

class PngStreamer: public IgnTransportImageStreamer
{
  public: PngStreamer(const async_web_server_cpp::HttpRequest &_request,
                      async_web_server_cpp::HttpConnectionPtr _connection);

  public: ~PngStreamer() override = default;

  protected: void SendImage(const ignition::msgs::Image &_msg) override;

  private: MultipartStream stream;
};

class PngSnapshotStreamer: public IgnTransportImageStreamer
{
  public: PngSnapshotStreamer(const async_web_server_cpp::HttpRequest &_request,
                              async_web_server_cpp::HttpConnectionPtr _connection);

  public: ~PngSnapshotStreamer() override = default;

  protected: void SendImage(const ignition::msgs::Image &_msg) override;
};

class PngStreamerType: public ImageStreamerType
{
  public: ImageStreamerPtr create_streamer(const async_web_server_cpp::HttpRequest &request,
                                           async_web_server_cpp::HttpConnectionPtr connection);

  public: std::string create_viewer(const async_web_server_cpp::HttpRequest &request);
};

}  // namespace web_video_server
}  // namespace ignition



#endif  // IGNITION__WEB_VIDEO_SERVER__PNG_STREAMER_HH_

