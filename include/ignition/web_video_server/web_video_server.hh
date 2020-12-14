#ifndef IGNITION__WEB_VIDEO_SERVER_HH__
#define IGNITION__WEB_VIDEO_SERVER_HH__

#include "ignition/web_video_server/image_streamer.hh"

#include <ignition/async_web_server_cpp/http_connection.hh>
#include <ignition/async_web_server_cpp/http_request.hh>
#include <ignition/async_web_server_cpp/http_server.hh>

#include <ignition/transport/Node.hh>

#include <map>
#include <memory>
#include <mutex>
#include <string>


namespace ignition {
namespace web_video_server {

/**
 * @class WebVideoServer
 * @brief
 */
class WebVideoServer
{
public:
  /**
   * @brief  Constructor
   * @return
   */
  WebVideoServer(const std::string &_address, int _port);

  /**
   * @brief  Destructor - Cleans up
   */
  virtual ~WebVideoServer() = default;

  /**
   * @brief  Starts the server and spins
   */
  void spin();


  bool handle_stream(const async_web_server_cpp::HttpRequest &request,
                     async_web_server_cpp::HttpConnectionPtr connection, const char* begin, const char* end);

  bool handle_stream_viewer(const async_web_server_cpp::HttpRequest &request,
                            async_web_server_cpp::HttpConnectionPtr connection, const char* begin, const char* end);

  bool handle_snapshot(const async_web_server_cpp::HttpRequest &request,
                       async_web_server_cpp::HttpConnectionPtr connection, const char* begin, const char* end);

  bool handle_list_streams(const async_web_server_cpp::HttpRequest &request,
                           async_web_server_cpp::HttpConnectionPtr connection, const char* begin, const char* end);

private:
  void restreamFrames(double max_age);
  void cleanup_inactive_streams();

  std::string address;
  int port;

  ignition::transport::Node node;

  std::shared_ptr<async_web_server_cpp::HttpServer> server;
  async_web_server_cpp::HttpRequestHandlerGroup handler_group;

  std::vector<std::shared_ptr<ImageStreamer> > image_subscribers;
  std::map<std::string, std::shared_ptr<ImageStreamerType> > stream_types;

  std::mutex subscriber_mutex;
};

}  // namespace web_video_server
}  // ignition

#endif  // IGNITION__WEB_VIDEO_SERVER_HH__
