#ifndef IGNITION__WEB_VIDEO_SERVER__VP8_STREAMER_HH_
#define IGNITION__WEB_VIDEO_SERVER__VP8_STREAMER_HH_

#include "ignition/web_video_server/libav_streamer.hh"

namespace ignition
{
namespace web_video_server {

class Vp8Streamer: public LibavStreamer
{
  public: Vp8Streamer(const async_web_server_cpp::HttpRequest &_request,
                      async_web_server_cpp::HttpConnectionPtr _connection);

  public: ~Vp8Streamer() override = default;

  protected: void initializeEncoder() override;
};

class Vp8StreamerType: public LibavStreamerType
{
  public: Vp8StreamerType();
  public: ImageStreamerPtr create_streamer(const async_web_server_cpp::HttpRequest &request,
                                           async_web_server_cpp::HttpConnectionPtr connection) override;
};

}  // namespace web_video_server
}  // namespace ignition

#endif  // IGNITION__WEB_VIDEO_SERVER__PNG_STREAMER_HH_
