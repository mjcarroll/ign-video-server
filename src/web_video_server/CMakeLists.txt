add_library(
  web_video_server
  image_streamer.cc
  ign_image_streamer.cc
  web_video_server.cc
  multipart_stream.cc
  png_streamer.cc
  libav_streamer.cc
  vp8_streamer.cc
  vp9_streamer.cc
  h264_streamer.cc
)

target_link_libraries(
  web_video_server
  PUBLIC
  async_web_server_cpp
  ignition-msgs6::ignition-msgs6
  ignition-transport9::ignition-transport9
  ignition-common3::ignition-common3
  ignition-common3::graphics
  ignition-common3::av
)

add_executable(
 video_server
 web_video_server_main.cc
)

target_link_libraries(video_server PUBLIC web_video_server)
