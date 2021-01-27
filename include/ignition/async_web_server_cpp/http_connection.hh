#ifndef IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_CONNECTION_HH_
#define IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_CONNECTION_HH_

#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/version.hpp>

#if ASIO_VERSION < 101200
  #include <asio/strand.hpp>
#else
  #include <asio/io_context_strand.hpp>
#endif

#include "ignition/async_web_server_cpp/http_fwd.hh"
#include "ignition/async_web_server_cpp/http_request_handler.hh"
#include "ignition/async_web_server_cpp/http_request.hh"
#include "ignition/async_web_server_cpp/http_request_parser.hh"

#include <functional>
#include <memory>
#include <mutex>

namespace async_web_server_cpp
{

/**
 *  Represents a connection to a client
 * The connection to the client is maintained as long as there is a shared
 * pointer to the connection object. If supplying the response is simple then
 * it can be done in the request handler callback from the server. However,
 * if the response will take time to generate or must be supplied over a long
 * period of time then a shared_ptr to the connection can be held and used
 * later. While reading and writing from multiple threads is supported, care
 * should be taken to ensure that proper external synchronization is done as
 * needed. For example, while write can be called from two threads, if two
 * calls to write need to be unseperated then calls to write should be locked
 * to prevent interleaving of different write calls.
 */
class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
  using ReadHandler = std::function<void(const char* begin, const char* end)>;
  using ResourcePtr = std::shared_ptr<const void>;

  explicit HttpConnection(asio::io_service &io_service,
                          HttpServerRequestHandler request_handler);

  HttpConnection(const HttpConnection&) = delete;
  HttpConnection& operator=(const HttpConnection&) = delete;

  HttpConnection(HttpConnection&&) = delete;
  HttpConnection& operator=(HttpConnection&&) = delete;

  asio::ip::tcp::socket &socket();

  /**
   * Start async operation to read request (normally called by server)
   */
  void start();

  /**
   * Perform an async read
   */
  void async_read(ReadHandler callback);

  /**
   * Write the given bytes to the socket and clear the vector
   */
  void write_and_clear(std::vector<unsigned char> &data);

  void write(const std::string &);

  void write(const asio::const_buffer &buffer,
             ResourcePtr resource);

  void write(const std::vector<asio::const_buffer> &buffer,
             ResourcePtr resource);

private:
  void handle_read(const char* begin, const char* end);
  void handle_read_raw(ReadHandler callback,
                       const asio::error_code &e,
                       std::size_t bytes_transferred);

  // Must be called while holding write lock
  void write_pending();

  void handle_write(const asio::error_code &e,
                    std::vector<ResourcePtr> resources);

  asio::io_service::strand strand_;
  asio::ip::tcp::socket socket_;
  HttpServerRequestHandler request_handler_;
  std::array<char, 8192> buffer_;
  HttpRequest request_;
  HttpRequestParser request_parser_;

  std::mutex write_mutex_;
  bool write_in_progress_;
  std::vector<asio::const_buffer> pending_write_buffers_;
  std::vector<ResourcePtr> pending_write_resources_;
  asio::error_code last_error_;
  ReadHandler read_handler_;
};

}

#endif  // IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_CONNECTION_HH_
