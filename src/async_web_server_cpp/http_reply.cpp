//
// http_reply.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include <fstream>
#include "ignition/async_web_server_cpp/http_connection.hh"
#include "ignition/async_web_server_cpp/http_reply.hh"

namespace async_web_server_cpp
{

namespace status_strings
{

const std::string switching_protocols =
  "HTTP/1.0 101 Switching Protocols\r\n";
const std::string ok =
  "HTTP/1.0 200 OK\r\n";
const std::string created =
  "HTTP/1.0 201 Created\r\n";
const std::string accepted =
  "HTTP/1.0 202 Accepted\r\n";
const std::string no_content =
  "HTTP/1.0 204 No Content\r\n";
const std::string multiple_choices =
  "HTTP/1.0 300 Multiple Choices\r\n";
const std::string moved_permanently =
  "HTTP/1.0 301 Moved Permanently\r\n";
const std::string moved_temporarily =
  "HTTP/1.0 302 Moved Temporarily\r\n";
const std::string not_modified =
  "HTTP/1.0 304 Not Modified\r\n";
const std::string bad_request =
  "HTTP/1.0 400 Bad Request\r\n";
const std::string unauthorized =
  "HTTP/1.0 401 Unauthorized\r\n";
const std::string forbidden =
  "HTTP/1.0 403 Forbidden\r\n";
const std::string not_found =
  "HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
  "HTTP/1.0 500 Internal Server Error\r\n";
const std::string not_implemented =
  "HTTP/1.0 501 Not Implemented\r\n";
const std::string bad_gateway =
  "HTTP/1.0 502 Bad Gateway\r\n";
const std::string service_unavailable =
  "HTTP/1.0 503 Service Unavailable\r\n";

asio::const_buffer to_buffer(HttpReply::status_type status)
{
  switch (status)
  {
  case HttpReply::switching_protocols:
    return asio::buffer(switching_protocols);
  case HttpReply::ok:
    return asio::buffer(ok);
  case HttpReply::created:
    return asio::buffer(created);
  case HttpReply::accepted:
    return asio::buffer(accepted);
  case HttpReply::no_content:
    return asio::buffer(no_content);
  case HttpReply::multiple_choices:
    return asio::buffer(multiple_choices);
  case HttpReply::moved_permanently:
    return asio::buffer(moved_permanently);
  case HttpReply::moved_temporarily:
    return asio::buffer(moved_temporarily);
  case HttpReply::not_modified:
    return asio::buffer(not_modified);
  case HttpReply::bad_request:
    return asio::buffer(bad_request);
  case HttpReply::unauthorized:
    return asio::buffer(unauthorized);
  case HttpReply::forbidden:
    return asio::buffer(forbidden);
  case HttpReply::not_found:
    return asio::buffer(not_found);
  case HttpReply::internal_server_error:
    return asio::buffer(internal_server_error);
  case HttpReply::not_implemented:
    return asio::buffer(not_implemented);
  case HttpReply::bad_gateway:
    return asio::buffer(bad_gateway);
  case HttpReply::service_unavailable:
    return asio::buffer(service_unavailable);
  default:
    return asio::buffer(internal_server_error);
  }
}

} // namespace status_strings

namespace misc_strings
{

const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};

} // namespace misc_strings

namespace stock_replies
{

const char ok[] = "";
const char created[] =
  "<html>"
  "<head><title>Created</title></head>"
  "<body><h1>201 Created</h1></body>"
  "</html>";
const char accepted[] =
  "<html>"
  "<head><title>Accepted</title></head>"
  "<body><h1>202 Accepted</h1></body>"
  "</html>";
const char no_content[] =
  "<html>"
  "<head><title>No Content</title></head>"
  "<body><h1>204 Content</h1></body>"
  "</html>";
const char multiple_choices[] =
  "<html>"
  "<head><title>Multiple Choices</title></head>"
  "<body><h1>300 Multiple Choices</h1></body>"
  "</html>";
const char moved_permanently[] =
  "<html>"
  "<head><title>Moved Permanently</title></head>"
  "<body><h1>301 Moved Permanently</h1></body>"
  "</html>";
const char moved_temporarily[] =
  "<html>"
  "<head><title>Moved Temporarily</title></head>"
  "<body><h1>302 Moved Temporarily</h1></body>"
  "</html>";
const char not_modified[] =
  "<html>"
  "<head><title>Not Modified</title></head>"
  "<body><h1>304 Not Modified</h1></body>"
  "</html>";
const char bad_request[] =
  "<html>"
  "<head><title>Bad Request</title></head>"
  "<body><h1>400 Bad Request</h1></body>"
  "</html>";
const char unauthorized[] =
  "<html>"
  "<head><title>Unauthorized</title></head>"
  "<body><h1>401 Unauthorized</h1></body>"
  "</html>";
const char forbidden[] =
  "<html>"
  "<head><title>Forbidden</title></head>"
  "<body><h1>403 Forbidden</h1></body>"
  "</html>";
const char not_found[] =
  "<html>"
  "<head><title>Not Found</title></head>"
  "<body><h1>404 Not Found</h1></body>"
  "</html>";
const char internal_server_error[] =
  "<html>"
  "<head><title>Internal Server Error</title></head>"
  "<body><h1>500 Internal Server Error</h1></body>"
  "</html>";
const char not_implemented[] =
  "<html>"
  "<head><title>Not Implemented</title></head>"
  "<body><h1>501 Not Implemented</h1></body>"
  "</html>";
const char bad_gateway[] =
  "<html>"
  "<head><title>Bad Gateway</title></head>"
  "<body><h1>502 Bad Gateway</h1></body>"
  "</html>";
const char service_unavailable[] =
  "<html>"
  "<head><title>Service Unavailable</title></head>"
  "<body><h1>503 Service Unavailable</h1></body>"
  "</html>";

std::string to_string(HttpReply::status_type status)
{
  switch (status)
  {
  case HttpReply::ok:
    return ok;
  case HttpReply::created:
    return created;
  case HttpReply::accepted:
    return accepted;
  case HttpReply::no_content:
    return no_content;
  case HttpReply::multiple_choices:
    return multiple_choices;
  case HttpReply::moved_permanently:
    return moved_permanently;
  case HttpReply::moved_temporarily:
    return moved_temporarily;
  case HttpReply::not_modified:
    return not_modified;
  case HttpReply::bad_request:
    return bad_request;
  case HttpReply::unauthorized:
    return unauthorized;
  case HttpReply::forbidden:
    return forbidden;
  case HttpReply::not_found:
    return not_found;
  case HttpReply::internal_server_error:
    return internal_server_error;
  case HttpReply::not_implemented:
    return not_implemented;
  case HttpReply::bad_gateway:
    return bad_gateway;
  case HttpReply::service_unavailable:
    return service_unavailable;
  default:
    return internal_server_error;
  }
}

} // namespace stock_replies

std::vector<asio::const_buffer> HttpReply::to_buffers(const std::vector<HttpHeader> &headers)
{
  std::vector<asio::const_buffer> buffers;
  for (std::size_t i = 0; i < headers.size(); ++i)
  {
    const HttpHeader &h = headers[i];
    buffers.push_back(asio::buffer(h.name));
    buffers.push_back(asio::buffer(misc_strings::name_value_separator));
    buffers.push_back(asio::buffer(h.value));
    buffers.push_back(asio::buffer(misc_strings::crlf));
  }
  buffers.push_back(asio::buffer(misc_strings::crlf));
  return buffers;
}


HttpServerRequestHandler HttpReply::from_file(HttpReply::status_type status,
    const std::string& content_type,
    const std::string& filename,
    const std::vector<HttpHeader>& additional_headers)
{
  std::vector<HttpHeader> headers;
  headers.push_back(HttpHeader("Content-Type", content_type));
  std::copy(additional_headers.begin(), additional_headers.end(), headers.begin());

  return FileHttpRequestHandler(status, filename, headers);
}
FileHttpRequestHandler::FileHttpRequestHandler(HttpReply::status_type status,
					       const std::string& filename,
					       const std::vector<HttpHeader>& headers)
  : status_(status), headers_(headers), filename_(filename)
{
}

static bool serveFromFile(HttpReply::status_type status, const std::string& filename, const std::vector<HttpHeader>& headers, HttpConnectionPtr connection) {
  std::ifstream file_stream(filename.c_str());
  std::stringstream file_buffer;
  file_buffer << file_stream.rdbuf();
  std::string content = file_buffer.str();

  ReplyBuilder reply_builder_(status);
  reply_builder_.headers(headers);
  reply_builder_.header("Content-Length", std::to_string(content.size()));
  reply_builder_.write(connection);
  connection->write(content);
  return true;
}
bool FileHttpRequestHandler::operator()(const HttpRequest &request, HttpConnectionPtr connection, const char* begin, const char* end)
{
  (void) request;
  (void) begin;
  (void) end;
  return serveFromFile(status_, filename_, headers_, connection);
}


HttpServerRequestHandler HttpReply::from_filesystem(HttpReply::status_type status,
    const std::string& path_root,
    const std::string& filesystem_root,
    bool list_directories,
    const std::vector<HttpHeader>& additional_headers)
{
  return FilesystemHttpRequestHandler(status, path_root, filesystem_root, list_directories, additional_headers);
}
FilesystemHttpRequestHandler::FilesystemHttpRequestHandler(HttpReply::status_type status,
							   const std::string& path_root,
							   const std::string& filesystem_root,
							   bool list_directories,
							   const std::vector<HttpHeader>& headers)
  : status_(status), headers_(headers), path_root_(path_root), filesystem_root_(filesystem_root), list_directories_(list_directories)
{
}

bool FilesystemHttpRequestHandler::operator()(const HttpRequest &request, HttpConnectionPtr connection, const char* begin, const char* end)
{
  if(request.path.find(path_root_) == 0) { // request.path startswith path_root
    std::string rel_path = request.path.substr(path_root_.length());
    if(rel_path.find_first_of('/') == 0) { // remove leading slash to make path relative
      rel_path = rel_path.substr(1);
    }

    std::filesystem::path requested_path = filesystem_root_ / rel_path;

    if(std::filesystem::exists(requested_path)) {
      if(std::filesystem::is_directory(requested_path)) {
        if(list_directories_) {
          std::stringstream content;
          content << "<html><body>";
          content << "<h1> Directory Listing: " << request.path << "</h1>";
          std::filesystem::directory_iterator end_itr;
          for (std::filesystem::directory_iterator itr(requested_path); itr != end_itr; ++itr) {
            if(std::filesystem::is_directory(itr->status())) {
              content << "<a href=\"" << itr->path().filename() << "/\">";
              content << itr->path().filename() << "/";
              content << "</a>";
            } else if(std::filesystem::is_regular_file(itr->status())) {
              content << "<a href=\"" << itr->path().filename() << "\">";
              content << itr->path().filename();
              content << "</a>";
            }
            content << "<br>";
          }
          content << "</body></html>";
          HttpReply::static_reply(HttpReply::ok, "text/html", content.str(), headers_)(request, connection, begin, end);
        } else {
          HttpReply::stock_reply(HttpReply::forbidden)(request, connection, begin, end);
        }
	      return true;
      } else if(std::filesystem::is_regular_file(requested_path)) {
        serveFromFile(status_, requested_path.generic_string(), headers_, connection);
        return true;
      } else {
        // It's not a directory or file?
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
  return false;
}


HttpServerRequestHandler HttpReply::stock_reply(HttpReply::status_type status)
{
  return static_reply(status, "text/html", stock_replies::to_string(status));
}

HttpServerRequestHandler HttpReply::static_reply(HttpReply::status_type status,
    const std::string& content_type,
    const std::string& content,
    const std::vector<HttpHeader>& additional_headers)
{
  std::vector<HttpHeader> headers;
  headers.push_back(HttpHeader("Content-Length", std::to_string(content.size())));
  headers.push_back(HttpHeader("Content-Type", content_type));
  std::copy(additional_headers.begin(), additional_headers.end(), headers.begin());
  return StaticHttpRequestHandler(status, headers, content);
}


StaticHttpRequestHandler::StaticHttpRequestHandler(HttpReply::status_type status,
    const std::vector<HttpHeader> &headers,
    const std::string &content)
  : reply_builder_(status), content_string_(content)
{
  reply_builder_.headers(headers);
}

bool StaticHttpRequestHandler::operator()(const HttpRequest &request, HttpConnectionPtr connection, const char* begin, const char* end)
{
  (void) request;
  (void) begin;
  (void) end;
  reply_builder_.write(connection);
  connection->write(content_string_);
  return true;
}


ReplyBuilder HttpReply::builder(HttpReply::status_type status)
{
  return ReplyBuilder(status);
}

ReplyBuilder::ReplyBuilder(HttpReply::status_type status)
  : status_(status), headers_(new std::vector<HttpHeader>())
{
}

ReplyBuilder &ReplyBuilder::header(const std::string &name, const std::string &value)
{
  return header(HttpHeader(name, value));
}

ReplyBuilder &ReplyBuilder::header(const HttpHeader &header)
{
  headers_->push_back(header);
  return *this;
}

ReplyBuilder &ReplyBuilder::headers(const std::vector<HttpHeader> &headers)
{
  headers_->insert(headers_->end(), headers.begin(), headers.end());
  return *this;
}

void ReplyBuilder::write(HttpConnectionPtr connection)
{
  connection->write(status_strings::to_buffer(status_), HttpConnection::ResourcePtr());
  connection->write(HttpReply::to_buffers(*headers_), headers_);
}

}
