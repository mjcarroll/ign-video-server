#include "ignition/web_video_server/multipart_stream.hh"
#include "ignition/async_web_server_cpp/http_reply.hh"

namespace ignition {
namespace web_video_server
{

MultipartStream::MultipartStream(
    async_web_server_cpp::HttpConnectionPtr& connection,
    const std::string& boundry,
    std::size_t max_queue_size)
  : connection_(connection), boundry_(boundry), max_queue_size_(max_queue_size)
{}

void MultipartStream::sendInitialHeader() {
  async_web_server_cpp::HttpReply::builder(async_web_server_cpp::HttpReply::ok)
    .header("Connection", "close")
    .header("Server", "web_video_server")
    .header("Cache-Control", "no-cache, no-store, must-revalidate, pre-check=0, post-check=0, max-age=0")
    .header("Pragma", "no-cache")
    .header("Content-type", "multipart/x-mixed-replace;boundary="+boundry_)
    .header("Access-Control-Allow-Origin", "*")
    .header("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,HEAD,OPTIONS")
    .header("Access-Control-Allow-Headers", "Origin, Authorization, Accept, Content-Type")
    .header("Access-Control-Max-Age", "3600").write(connection_);
  connection_->write("--"+boundry_+"\r\n");
}

void MultipartStream::sendPartHeader(const Time &time, const std::string& type, size_t payload_size) {
  char stamp[20];

  sprintf(stamp, "%.06lf", static_cast<double>(time.time_since_epoch().count()));
  auto headers = std::make_shared<std::vector<async_web_server_cpp::HttpHeader>>();

  headers->push_back(async_web_server_cpp::HttpHeader("Content-type", type));
  headers->push_back(async_web_server_cpp::HttpHeader("X-Timestamp", stamp));
  headers->push_back(async_web_server_cpp::HttpHeader("Access-Control-Allow-Origin", "*"));
  headers->push_back(async_web_server_cpp::HttpHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,HEAD,OPTIONS"));
  headers->push_back(async_web_server_cpp::HttpHeader("Access-Control-Allow-Headers", "Origin, Authorization, Accept, Content-Type"));
  headers->push_back(async_web_server_cpp::HttpHeader("Access-Control-Max-Age", "3600"));
  headers->push_back(
      async_web_server_cpp::HttpHeader("Content-Length", std::to_string(payload_size)));
  connection_->write(async_web_server_cpp::HttpReply::to_buffers(*headers), headers);
}

void MultipartStream::sendPartFooter(const Time &time) {
  auto str = std::make_shared<std::string>("\r\n--"+boundry_+"\r\n");
  PendingFooter pf;
  pf.timestamp = time;
  pf.contents = str;
  connection_->write(asio::buffer(*str), str);
  if (max_queue_size_ > 0) pending_footers_.push(pf);
}

void MultipartStream::sendPartAndClear(const Time &time, const std::string& type,
				       std::vector<unsigned char> &data) {
  if (!isBusy())
  {
    sendPartHeader(time, type, data.size());
    connection_->write_and_clear(data);
    sendPartFooter(time);
  }
}

void MultipartStream::sendPart(const Time &time, const std::string& type,
			       const asio::const_buffer &buffer,
			       async_web_server_cpp::HttpConnection::ResourcePtr resource) {
  if (!isBusy())
  {
    sendPartHeader(time, type, asio::buffer_size(buffer));
    connection_->write(buffer, resource);
    sendPartFooter(time);
  }
}

bool MultipartStream::isBusy() {
  Time currentTime = std::chrono::system_clock::now();
  while (!pending_footers_.empty())
  {
    if (pending_footers_.front().contents.expired()) {
      pending_footers_.pop();
    } else {
      Time footerTime = pending_footers_.front().timestamp;
      if ((currentTime - footerTime).count() > 0.5) {
        pending_footers_.pop();
      } else {
        break;
      }
    }
  }
  return !(max_queue_size_ == 0 || pending_footers_.size() < max_queue_size_);
}

}
}
