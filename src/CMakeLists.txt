add_subdirectory(async_web_server_cpp)
add_subdirectory(web_video_server)

add_executable(
  image_publisher 
  image_publisher.cc
)

target_link_libraries(
  image_publisher
  PRIVATE
  ignition-msgs6::ignition-msgs6
  ignition-transport9::ignition-transport9
  ignition-common3::graphics
  ${OpenCV_LIBS}
)


