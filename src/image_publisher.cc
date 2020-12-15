#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

#include <ignition/msgs.hh>
#include <ignition/transport.hh>
#include <ignition/common/Image.hh>
#include <ignition/math/Helpers.hh>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " /ign/topic" << std::endl;
    return 1;
  }

  // Install a signal handler for SIGINT and SIGTERM.
  std::signal(SIGINT,  signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Create a transport node and advertise a topic.
  ignition::transport::Node node;
  std::string topic = argv[1];

  auto pub = node.Advertise<ignition::msgs::Image>(topic);
  if (!pub)
  {
    std::cerr << "Error advertising topic [" << topic << "]" << std::endl;
    return -1;
  }


  int width = 320;
  int height = 240;

  cv::VideoCapture cap;
  cap.open(0);
  cap.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(width));
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(height));

  if (!cap.isOpened())
  {
    std::cerr << "Could not open video stream" << std::endl;
  }

  // Prepare the message.
  ignition::msgs::Image msg;

  // Publish messages at 1Hz.
  while (!g_terminatePub)
  {
    cv::Mat frame;
    cap >> frame;

    if (frame.empty()) break;

    cv::imshow("image_publisher", frame);
    cv::waitKey(1);

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    msg.set_pixel_format_type(ignition::msgs::PixelFormatType::RGB_INT8);
    msg.set_width(width);
    msg.set_height(height);
    msg.set_data(frame.data, 3 * width * height);

    auto now = std::chrono::steady_clock::now();
    auto [sec, nsec] = ignition::math::timePointToSecNsec(now);
    msg.mutable_header()->mutable_stamp()->set_sec(sec);
    msg.mutable_header()->mutable_stamp()->set_nsec(nsec);

    if (!pub.Publish(msg))
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
