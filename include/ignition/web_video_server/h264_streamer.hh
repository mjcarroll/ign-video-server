#ifndef IGNITION__WEB_VIDEO_SERVER__H264_STREAMER_HH_
#define IGNITION__WEB_VIDEO_SERVER__H264_STREAMER_HH_

#include "ignition/web_video_server/libav_streamer.hh"

namespace ignition
{
namespace web_video_server {

class H264Streamer: public LibavStreamer
{
  public: H264Streamer(const async_web_server_cpp::HttpRequest &_request,
                      async_web_server_cpp::HttpConnectionPtr _connection);

  public: ~H264Streamer() override = default;

  protected: void initializeEncoder() override;
};

class H264StreamerType: public LibavStreamerType
{
  public: H264StreamerType();

  public: ImageStreamerPtr create_streamer(const async_web_server_cpp::HttpRequest &request,
                                           async_web_server_cpp::HttpConnectionPtr connection) override;
};

}  // namespace web_video_server
}  // namespace ignition

#endif  // IGNITION__WEB_VIDEO_SERVER__VP9_STREAMER_HH_
