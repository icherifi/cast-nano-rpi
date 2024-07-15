# Screen Mirroring with Raspberry

Nano cast is implemented for Raspberry. This solution is an alternative of Chormecast or MiraCast. It's developped by a student in it's spare time and is still being developped.


# Technical details

## Overview

![alt text](https://github.com/icherifi/cast-nano-rpi/blob/main/diagram1.png)

## Technical Detail

![alt text](https://github.com/icherifi/cast-nano-rpi/blob/main/diagram2.png)

## UI

![alt text](https://github.com/icherifi/cast-nano-rpi/blob/main/diagram3.png)

# Build

cd media-receiver-client/deps/libdatachannel
git submodule update --init --recursive --depth 1
cd ../..
.cmake -B build
cd build
make -j2

To run locally (for test):

node ./signaling-server-nodejs/src/signaling-server.js

./media-receiver-client/build/media-receiver-client

gst-launch-1.0 udpsrc address=127.0.0.1 port=5000 caps="application/x-rtp" ! queue ! rtph264depay ! video/x-h264,stream-format=byte-stream ! queue ! avdec_h264 ! queue ! autovideosink &

web UI : http://localhost:8001/


To-do :
    - Run html by connecting to the server
    - Test on Raspberry

