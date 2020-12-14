#ifndef IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_FWD_HH_
#define IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_FWD_HH_

#include <memory>

namespace async_web_server_cpp
{

class HttpConnection;
using HttpConnectionPtr = std::shared_ptr<HttpConnection>;
using HttpConnectionWeakPtr = std::weak_ptr<HttpConnection>;

class WebsocketConnection;
using WebsocketConnectionPtr = std::shared_ptr<WebsocketConnection>;
using WebsocketConnectionWeakPtr = std::weak_ptr<WebsocketConnection>;

}

#endif

