#ifndef IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_HEADER_HH_
#define IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_HEADER_HH_

#include <string>

namespace async_web_server_cpp
{

/**
 * Represents a HTTP header in a request or response
 */
struct HttpHeader
{
  HttpHeader()
  {
  }

  HttpHeader(std::string _name, std::string _value) : name(_name), value(_value)
  {
  }

  std::string name;
  std::string value;
};

}

#endif  // IGNITION__ASYNC_WEB_SERVER_CPP__HTTP_HEADER_HH_
