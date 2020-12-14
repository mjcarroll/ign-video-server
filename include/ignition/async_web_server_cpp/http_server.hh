#ifndef IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_SERVER_HH_
#define IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_SERVER_HH_

#include "ignition/async_web_server_cpp/http_request_handler.hh"
#include "ignition/async_web_server_cpp/http_connection.hh"

#include <asio/error_code.hpp>
#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>

#include <string>
#include <thread>
#include <vector>

namespace async_web_server_cpp
{

/**
 * @class HttpServer
 * The HttpServer is an implementation of a HTTP server that serves http request from a given port
 * The server maintains a pool of threads to use to serve requests. Each request is dispatched to
 * the given request handler to be handled.
 */
class HttpServer
{
public:
  HttpServer(const std::string &address, const std::string &port,
             HttpServerRequestHandler request_handler, std::size_t thread_pool_size);
  ~HttpServer();

  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;

  HttpServer(HttpServer&&) = delete;
  HttpServer& operator=(HttpServer&&) = delete;

  void run();

  void stop();

private:
  void start_accept();

  void handle_accept(const asio::error_code &e);

  asio::io_service io_service_;
  asio::ip::tcp::acceptor acceptor_;
  std::size_t thread_pool_size_;
  std::vector<std::shared_ptr<std::thread> > threads_;
  HttpConnectionPtr new_connection_;
  HttpServerRequestHandler request_handler_;
};

}

#endif  // IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_SERVER_HH_
