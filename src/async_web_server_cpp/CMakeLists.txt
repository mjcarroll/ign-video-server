add_library(
  async_web_server_cpp
  http_request.cpp
  http_request_parser.cpp
  http_connection.cpp 
  http_reply.cpp 
  http_request_handler.cpp
  http_server.cpp
  websocket_connection.cpp 
  websocket_request_handler.cpp
  websocket_message.cpp 
)

target_compile_definitions(
  async_web_server_cpp
  PUBLIC
  "-DASIO_STANDALONE=1"
)

target_link_libraries(
  async_web_server_cpp
  ${OPENSSL_LIBRARIES}
  pthread
)
