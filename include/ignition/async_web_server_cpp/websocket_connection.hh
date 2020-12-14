#ifndef IGNITION__ASYNC_WEB_SERVER_CPP__WEBSOCKET_CONNECTION_HH_
#define IGNITION__ASYNC_WEB_SERVER_CPP__WEBSOCKET_CONNECTION_HH_

#include "ignition/async_web_server_cpp/http_connection.hh"
#include "ignition/async_web_server_cpp/websocket_message.hh"

namespace async_web_server_cpp
{

class WebsocketHttpRequestHandler;

/**
 *  Represents a websocket connection. Similar to an HttpConnection, to keep the
 * connection alive keep a shared pointer to this object.
 */
class WebsocketConnection : std::enable_shared_from_this<WebsocketConnection>
{
public:
  explicit WebsocketConnection(HttpConnectionPtr connection);

  WebsocketConnection(const WebsocketConnection&) = delete;
  WebsocketConnection& operator=(const WebsocketConnection&) = delete;

  WebsocketConnection(WebsocketConnection&&) = delete;
  WebsocketConnection& operator=(WebsocketConnection&&) = delete;

  using MessageHandler = std::function<void(const WebsocketMessage& message)>;

  bool sendTextMessage(const std::string& content);
  bool sendPingMessage(const std::string& content = "");

  bool sendMessage(const WebsocketMessage& message);
  bool sendFrame(WebsocketFrame& frame);

private:
  static void static_handle_read(WebsocketConnection* _this, const char* begin, const char* end);
  void handle_read(const char* begin, const char* end);
  HttpConnectionPtr connection_;

  void set_message_handler(MessageHandler& handler);
  MessageHandler handler_;

  WebsocketFrame frame_;
  WebsocketMessage message_;
  WebsocketFrameParser frame_parser_;
  WebsocketFrameBuffer frame_buffer_;

  friend class WebsocketHttpRequestHandler;
};

}

#endif  // IGNITION__ASYNC_WEB_SERVER_CPP__WEBSOCKET_CONNECTION_HH_
