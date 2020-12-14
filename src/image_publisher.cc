#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

#include <ignition/msgs.hh>
#include <ignition/transport.hh>
#include <ignition/common/Image.hh>

/// \brief Flag used to break the publisher loop and terminate the program.
static std::atomic<bool> g_terminatePub(false);

//////////////////////////////////////////////////
/// \brief Function callback executed when a SIGINT or SIGTERM signals are
/// captured. This is used to break the infinite loop that publishes messages
/// and exit the program smoothly.
void signal_handler(int _signal)
{
  if (_signal == SIGINT || _signal == SIGTERM)
    g_terminatePub = true;
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " /ign/topic image_filename.jpg" << std::endl;
    return 1;
  }

  // Install a signal handler for SIGINT and SIGTERM.
  std::signal(SIGINT,  signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Create a transport node and advertise a topic.
  ignition::transport::Node node;
  std::string topic = argv[1];
  std::string image_fname = argv[2];

  auto pub = node.Advertise<ignition::msgs::Image>(topic);
  if (!pub)
  {
    std::cerr << "Error advertising topic [" << topic << "]" << std::endl;
    return -1;
  }

  // Prepare the message.
  ignition::msgs::Image msg;

  ignition::common::Image image(image_fname);

  msg.set_width(image.Width());
  msg.set_height(image.Height());
  msg.set_step(image.Pitch());

  unsigned char *data = nullptr;
  unsigned int size = 0;


  switch(image.PixelFormat())
  {
    case ignition::common::Image::RGB_INT8:
      msg.set_pixel_format_type(ignition::msgs::PixelFormatType::RGB_INT8);
      break;
    default:
      std::cerr << "Unhandled pixel format" << std::endl;
      break;
  }

  image.Data(&data, size);
  msg.set_data(data, size);

  // Publish messages at 1Hz.
  while (!g_terminatePub)
  {
    if (!pub.Publish(msg))
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
