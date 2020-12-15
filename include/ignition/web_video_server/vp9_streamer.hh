#ifndef IGNITION__WEB_VIDEO_SERVER__VP9_STREAMER_HH_
#define IGNITION__WEB_VIDEO_SERVER__VP9_STREAMER_HH_

#include "ignition/web_video_server/libav_streamer.hh"

namespace ignition
{
namespace web_video_server {

class Vp9Streamer: public LibavStreamer
{
  public: Vp9Streamer(const async_web_server_cpp::HttpRequest &_request,
                      async_web_server_cpp::HttpConnectionPtr _connection);

  public: ~Vp9Streamer() override = default;

  protected: void initializeEncoder() override;
};

class Vp9StreamerType: public LibavStreamerType
{
  public: Vp9StreamerType();
  public: ImageStreamerPtr create_streamer(const async_web_server_cpp::HttpRequest &request,
                                           async_web_server_cpp::HttpConnectionPtr connection) override;
};

}  // namespace web_video_server
}  // namespace ignition

#endif  // IGNITION__WEB_VIDEO_SERVER__VP9_STREAMER_HH_
