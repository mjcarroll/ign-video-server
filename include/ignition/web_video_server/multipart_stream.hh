#ifndef IGNITION__WEB_VIDEO_SERVER__MULTIPART_STREAM_HH_
#define IGNITION__WEB_VIDEO_SERVER__MULTIPART_STREAM_HH_

#include <ignition/async_web_server_cpp/http_connection.hh>

#include <chrono>
#include <queue>

namespace ignition {
namespace web_video_server
{
using Time = std::chrono::time_point<std::chrono::system_clock>;

struct PendingFooter {
  Time timestamp;
  std::weak_ptr<std::string> contents;
};

class MultipartStream {
public:
  MultipartStream(async_web_server_cpp::HttpConnectionPtr& connection,
                  const std::string& boundry="boundarydonotcross",
                  std::size_t max_queue_size=1);

  void sendInitialHeader();
  void sendPartHeader(const Time &time, const std::string& type, size_t payload_size);
  void sendPartFooter(const Time &time);
  void sendPartAndClear(const Time &time, const std::string& type, std::vector<unsigned char> &data);
  void sendPart(const Time &time, const std::string& type, const asio::const_buffer &buffer,
		async_web_server_cpp::HttpConnection::ResourcePtr resource);

private:
  bool isBusy();

private:
  const std::size_t max_queue_size_;
  async_web_server_cpp::HttpConnectionPtr connection_;
  std::string boundry_;
  std::queue<PendingFooter> pending_footers_;
};

}
}

#endif  // IGNITION__WEB_VIDEO_SERVER__MULTIPART_STREAM_HH_

