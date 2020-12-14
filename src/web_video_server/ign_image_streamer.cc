#include "ignition/web_video_server/ign_image_streamer.hh"

#include <ignition/common/Console.hh>

namespace ignition {
namespace web_video_server {

IgnTransportImageStreamer::IgnTransportImageStreamer(
    const async_web_server_cpp::HttpRequest &_request,
    async_web_server_cpp::HttpConnectionPtr _connection):
  ImageStreamer(_request, _connection)
{
  this->topic = request.get_query_param_value_or_default("topic", "/image");
}

void IgnTransportImageStreamer::Start()
{
  std::vector<std::string> allTopics;
  std::set<std::string> imageTopics;

  this->node.TopicList(allTopics);
  for (auto queryTopic: allTopics)
  {
    std::vector<transport::MessagePublisher> publishers;
    this->node.TopicInfo(queryTopic, publishers);
    for (auto pub: publishers)
    {
      if (pub.MsgTypeName() == "ignition.msgs.Image")
      {
        imageTopics.insert(queryTopic);
        break;
      }
    }
  }

  if (!imageTopics.count(this->topic))
  {
    inactive = true;
    std::cerr << "Could not find topic: " << this->topic << " to stream" << std::endl;
  }
  else
  {
    inactive = false;
    node.Subscribe(this->topic, &IgnTransportImageStreamer::OnImageMsg, this);
  }
}

void IgnTransportImageStreamer::Initialize(const ignition::msgs::Image &_msg)
{
  (void) _msg;
}

void IgnTransportImageStreamer::OnImageMsg(const ignition::msgs::Image &_msg)
{
  if (this->inactive)
  {
    igndbg << "Received image message, but streamer inactive\n";
    return;
  }

  try
  {
    if (!this->initialized)
    {
      this->Initialize(_msg);
      this->initialized = true;
    }

    this->SendImage(_msg);
  }
  catch(std::exception &ex)
  {
    ignerr << "Error sending image: " << ex.what() << std::endl;
    this->inactive = true;
  }
}

}  // namespace web_video_server
}  // namespace ignition

