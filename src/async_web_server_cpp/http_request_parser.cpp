//
// http_request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "ignition/async_web_server_cpp/http_request_parser.hh"

namespace async_web_server_cpp
{
HttpRequestParser::HttpRequestParser()
  : state_(method_start)
{
}

void HttpRequestParser::reset()
{
  state_ = method_start;
}

std::optional<bool> HttpRequestParser::consume(HttpRequest &req, char input)
{
  switch (state_)
  {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      state_ = method;
      req.method.push_back(input);
      return {};
    }
  case method:
    if (input == ' ')
    {
      state_ = uri;
      return {};
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.method.push_back(input);
      return {};
    }
  case uri:
    if (input == ' ')
    {
      state_ = http_version_h;
      return {};
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      req.uri.push_back(input);
      return {};
    }
  case http_version_h:
    if (input == 'H')
    {
      state_ = http_version_t_1;
      return {};
    }
    else
    {
      return false;
    }
  case http_version_t_1:
    if (input == 'T')
    {
      state_ = http_version_t_2;
      return {};
    }
    else
    {
      return false;
    }
  case http_version_t_2:
    if (input == 'T')
    {
      state_ = http_version_p;
      return {};
    }
    else
    {
      return false;
    }
  case http_version_p:
    if (input == 'P')
    {
      state_ = http_version_slash;
      return {};
    }
    else
    {
      return false;
    }
  case http_version_slash:
    if (input == '/')
    {
      req.http_version_major = 0;
      req.http_version_minor = 0;
      state_ = http_version_major_start;
      return {};
    }
    else
    {
      return false;
    }
  case http_version_major_start:
    if (is_digit(input))
    {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      state_ = http_version_major;
      return {};
    }
    else
    {
      return false;
    }
  case http_version_major:
    if (input == '.')
    {
      state_ = http_version_minor_start;
      return {};
    }
    else if (is_digit(input))
    {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      return {};
    }
    else
    {
      return false;
    }
  case http_version_minor_start:
    if (is_digit(input))
    {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      state_ = http_version_minor;
      return {};
    }
    else
    {
      return false;
    }
  case http_version_minor:
    if (input == '\r')
    {
      state_ = expecting_newline_1;
      return {};
    }
    else if (is_digit(input))
    {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      return {};
    }
    else
    {
      return false;
    }
  case expecting_newline_1:
    if (input == '\n')
    {
      state_ = header_line_start;
      return {};
    }
    else
    {
      return false;
    }
  case header_line_start:
    if (input == '\r')
    {
      state_ = expecting_newline_3;
      return {};
    }
    else if (!req.headers.empty() && (input == ' ' || input == '\t'))
    {
      state_ = header_lws;
      return {};
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.headers.push_back(HttpHeader());
      req.headers.back().name.push_back(input);
      state_ = header_name;
      return {};
    }
  case header_lws:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return {};
    }
    else if (input == ' ' || input == '\t')
    {
      return {};
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      state_ = header_value;
      req.headers.back().value.push_back(input);
      return {};
    }
  case header_name:
    if (input == ':')
    {
      state_ = space_before_header_value;
      return {};
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
    else
    {
      req.headers.back().name.push_back(input);
      return {};
    }
  case space_before_header_value:
    if (input == ' ')
    {
      state_ = header_value;
      return {};
    }
    else
    {
      return false;
    }
  case header_value:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return {};
    }
    else if (is_ctl(input))
    {
      return false;
    }
    else
    {
      req.headers.back().value.push_back(input);
      return {};
    }
  case expecting_newline_2:
    if (input == '\n')
    {
      state_ = header_line_start;
      return {};
    }
    else
    {
      return false;
    }
  case expecting_newline_3:
    return (input == '\n');
  default:
    return false;
  }
}

bool HttpRequestParser::is_char(int c)
{
  return c >= 0 && c <= 127;
}

bool HttpRequestParser::is_ctl(int c)
{
  return (c >= 0 && c <= 31) || (c == 127);
}

bool HttpRequestParser::is_tspecial(int c)
{
  switch (c)
  {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}

bool HttpRequestParser::is_digit(int c)
{
  return c >= '0' && c <= '9';
}

}
