#include "ignition/web_video_server/web_video_server.hh"
#include "ignition/web_video_server/png_streamer.hh"

#include <ignition/async_web_server_cpp/http_reply.hh>
#include <ignition/common/Console.hh>

#include <iostream>

using namespace std::placeholders;

namespace ignition {
namespace web_video_server {

WebVideoServer::WebVideoServer(const std::string &_address, int _port):
  address(_address),
  port(_port),
  handler_group(
      async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::not_found))
{

  handler_group.addHandlerForPath("/", std::bind(&WebVideoServer::handle_list_streams, this, _1, _2, _3, _4));
  handler_group.addHandlerForPath("/stream", std::bind(&WebVideoServer::handle_stream, this, _1, _2, _3, _4));
  handler_group.addHandlerForPath("/stream_viewer",
                                   std::bind(&WebVideoServer::handle_stream_viewer, this, _1, _2, _3, _4));
  /*
  handler_group.addHandlerForPath("/snapshot", std::bind(&WebVideoServer::handle_snapshot, this, _1, _2, _3, _4));
  */

  stream_types["png"] = std::shared_ptr<ImageStreamerType>(new PngStreamerType());

  try
  {
    server = std::make_shared<async_web_server_cpp::HttpServer>(address, std::to_string(port), handler_group, 1);
  }
  catch (const std::exception& ex)
  {
    ignerr << "Error creating server: " << ex.what() << std::endl;
  }
}

void WebVideoServer::spin()
{
  igndbg << "WebVideoServer::spin" << std::endl;
  server->run();
}

bool WebVideoServer::handle_list_streams(const async_web_server_cpp::HttpRequest &request,
                                   async_web_server_cpp::HttpConnectionPtr connection, const char* begin,
                                   const char* end)
{
  (void) begin;
  (void) end;
  (void) request;

  igndbg << "WebVideoServer::handle_list_streams()\n";

  std::vector<std::string> allTopics;
  std::vector<std::string> imageTopics;

  this->node.TopicList(allTopics);
  for (auto topic: allTopics)
  {
    std::vector<transport::MessagePublisher> publishers;
    this->node.TopicInfo(topic, publishers);
    for (auto pub: publishers)
    {
      if (pub.MsgTypeName() == "ignition.msgs.Image")
      {
        imageTopics.push_back(topic);
        break;
      }
    }
  }

  async_web_server_cpp::HttpReply::builder(async_web_server_cpp::HttpReply::ok)
    .header("Connection", "close")
    .header("Server", "web_video_server")
    .header("Cache-Control", "no-cache, no-store, must-revalidate, pre-check=0, post-check=0, max-age=0")
    .header("Pragma", "no-cache")
    .header("Content-type", "text/html;")
    .write(connection);

  connection->write(
R"(<html>
<head>
  <title>Ignition Image Topic List</title>
</head>
<body>
<h1>Available Image Topics:</h1>
<ul>
)");
  for (auto topic: imageTopics)
  {
    connection->write("<li>" + topic + "</li>");
  }
  connection->write("</ul>");
  connection->write("</body></html>");
  return true;
}

bool WebVideoServer::handle_stream(const async_web_server_cpp::HttpRequest &request,
                                   async_web_server_cpp::HttpConnectionPtr connection,
                                   const char* begin, const char* end)
{
  std::string type = request.get_query_param_value_or_default("type", "");
  igndbg << "WebVideoServer::handle_stream(): type=" << type << "\n";

  if (stream_types.find(type) != stream_types.end())
  {
    std::shared_ptr<ImageStreamer> streamer = stream_types[type]->create_streamer(request, connection);
    streamer->Start();
    std::scoped_lock lock(subscriber_mutex);
    image_subscribers.push_back(streamer);
  }
  else
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::not_found)
      (request, connection, begin, end);
  }
  return true;
}

bool WebVideoServer::handle_stream_viewer(const async_web_server_cpp::HttpRequest &request,
                                          async_web_server_cpp::HttpConnectionPtr connection,
                                          const char* begin, const char* end)
{
  std::string type = request.get_query_param_value_or_default("type", "");
  std::string topic = request.get_query_param_value_or_default("topic", "");
  igndbg << "WebVideoServer::handle_stream_viewer(): type=" << type << " topic=" << topic << "\n";

  if (stream_types.find(type) != stream_types.end())
  {
    std::shared_ptr<ImageStreamer> streamer = stream_types[type]->create_streamer(request, connection);
    streamer->Start();
    std::scoped_lock lock(subscriber_mutex);
    image_subscribers.push_back(streamer);

    async_web_server_cpp::HttpReply::builder(async_web_server_cpp::HttpReply::ok)
      .header("Connection", "close")
      .header("Server", "web_video_server")
      .header("Content-type", "text/html;")
      .write(connection);

    std::stringstream ss;
    ss << "<html><head><title>" << topic << "</title></head><body>";
    ss << "<h1>" << topic << "</h1>";
    ss << stream_types[type]->create_viewer(request);
    ss << "</body></html>";

    std::cout << ss.str() << std::endl;
    connection->write(ss.str());
  }
  else
  {
    async_web_server_cpp::HttpReply::stock_reply(async_web_server_cpp::HttpReply::not_found)
      (request, connection, begin, end);
  }
  return true;
}

}  // namespace web_video_server
}  // namespace ignition
