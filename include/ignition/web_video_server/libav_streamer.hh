#ifndef IGNITION__WEB_VIDEO_SERVER__LIBAV_STREAMER_HH_
#define IGNITION__WEB_VIDEO_SERVER__LIBAV_STREAMER_HH_

#include "ignition/web_video_server/ign_image_streamer.hh"

#include "ignition/async_web_server_cpp/http_request.hh"
#include "ignition/async_web_server_cpp/http_connection.hh"

#include <ignition/common/Console.hh>

#include <mutex>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/intreadwrite.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/imgutils.h>
}

namespace ignition {
namespace web_video_server {

class LibavStreamer: public IgnTransportImageStreamer
{
public: LibavStreamer(const async_web_server_cpp::HttpRequest &_request,
                      async_web_server_cpp::HttpConnectionPtr _connection,
                      const std::string &_formatName,
                      const std::string &_codecName,
                      const std::string &_contentType);

public: ~LibavStreamer() override;

protected: virtual void initializeEncoder();
protected: void SendImage(const ignition::msgs::Image &_msg) override;
protected: void Initialize(const ignition::msgs::Image &_msg) override;

protected: AVOutputFormat* output_format {nullptr};
protected: AVFormatContext* format_context {nullptr};
protected: AVCodec* codec {nullptr};
protected: AVCodecContext* codec_context {nullptr};
protected: AVStream* video_stream {nullptr};
protected: AVDictionary* opt {nullptr};   // container format options

private:
  AVFrame* frame {nullptr};
  struct SwsContext* sws_context {nullptr};

  bool first_set { false };
  std::chrono::time_point<std::chrono::steady_clock> first_image_timestamp;
  std::mutex encode_mutex;

  std::string format_name;
  std::string codec_name;
  std::string content_type;
  int bitrate;
  int qmin;
  int qmax;
  int gop;

  uint8_t* io_buffer {nullptr};  // custom IO buffer
};


class LibavStreamerType : public ImageStreamerType
{
public:
  LibavStreamerType(const std::string &format_name, const std::string &codec_name, const std::string &content_type);

  virtual std::shared_ptr<ImageStreamer> create_streamer(const async_web_server_cpp::HttpRequest &request,
                                                         async_web_server_cpp::HttpConnectionPtr connection);

  std::string create_viewer(const async_web_server_cpp::HttpRequest &request);

private:
  const std::string format_name;
  const std::string codec_name;
  const std::string content_type;
};


}  // namespace web_video_server
}  // namespace ignition

#endif  // IGNITION__WEB_VIDEO_SERVER__LIBAV_STREAMER_HH_
