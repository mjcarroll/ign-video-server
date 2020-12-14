#include "ignition/web_video_server/web_video_server.hh"

#include <ignition/common/Console.hh>

#include <atomic>
#include <csignal>

std::atomic<bool> g_terminate{false};

void signal_handler(int _signal)
{
  if (_signal == SIGINT || _signal == SIGTERM)
    g_terminate= true;
}

int main(int argc, char** argv)
{
  ignition::common::Console::SetVerbosity(4);


  // Install a signal handler for SIGINT and SIGTERM.
  std::signal(SIGINT,  signal_handler);
  std::signal(SIGTERM, signal_handler);

  ignition::web_video_server::WebVideoServer server("localhost", 5000);
  server.spin();

  while (!g_terminate)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
