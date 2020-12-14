#ifndef IGNITION__ASYNC_WEB_SERVER_CPP__WEBSOCKET_REQUEST_HANDLER_HH_
#define IGNITION__ASYNC_WEB_SERVER_CPP__WEBSOCKET_REQUEST_HANDLER_HH_

#include "ignition/async_web_server_cpp/http_request_handler.hh"
#include "ignition/async_web_server_cpp/websocket_connection.hh"

namespace async_web_server_cpp
{

class WebsocketConnection;

typedef std::function<WebsocketConnection::MessageHandler(const HttpRequest &, WebsocketConnectionPtr)> WebsocketRequestHandler;

/**
 * A HTTP request handler that upgrades a HttpConnection to a WebsocketConnection.
 */
class WebsocketHttpRequestHandler
{
public:
  WebsocketHttpRequestHandler(WebsocketRequestHandler handler);
  bool operator()(const HttpRequest &request, HttpConnectionPtr connection, const char* begin, const char* end);

private:
  WebsocketRequestHandler handler_;
};

}

#endif  // IGNITION__ASYNC_WEB_SERVER_CPP__WEBSOCKET_REQUEST_HANDLER_HH_
