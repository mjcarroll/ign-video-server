# ign-video-server

Web-based streaming video server for Ignition

This is an ignition variant of the RobotWebTools web_video_server: https://github.com/RobotWebTools/web_video_server

It also includes a version of https://github.com/GT-RAIL/async_web_server_cpp which uses standalone asio.


## Building

Dependencies:
 * `libasio-dev` - Standalone version of the Boost `asio` library 
 * `OpenSSL`
 * `OpenCV` - Required for image streaming demo

## Running

* For a demo publisher, run

```
./bin/image_publisher <ignition topic>
```

* Run the web server with

```
./bin/video_server
```

Navigate a web browser to http://localhost:5000/stream?topic=/foo&type=png

Where type can be `png`, `vp8`, `vp9`, `h264`
