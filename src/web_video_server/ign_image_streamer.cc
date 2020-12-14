#include "ignition/web_video_server/ign_image_streamer.hh"

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

  inactive = true;
  if (!imageTopics.count(this->topic))
  {
    std::cerr << "Could not find topic: " << this->topic << " to stream" << std::endl;
  }

  node.Subscribe(this->topic, &IgnTransportImageStreamer::OnImageMsg, this);
}

void IgnTransportImageStreamer::Initialize()
{
  // nop
}

void IgnTransportImageStreamer::OnImageMsg(const ignition::msgs::Image &_msg)
{
  if (this->inactive)
    return;

  try
  {
    if (!this->initialized)
    {
      this->Initialize();
      this->initialized = true;
    }

    this->SendImage(_msg);
  }
  catch(std::exception &ex)
  {
    std::cerr << "Error sending image: " << ex.what() << std::endl;
    this->inactive = true;
  }
}

}  // namespace web_video_server
}  // namespace ignition

