#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Usage: $0 ip port"
    exit 1
fi
# gst-launch v4l2src device=/dev/video0 ! videoscale! video/x-raw-yuv,width=320,height=240 ! ffmpegcolorspace ! jpegenc ! tcpserversink port=$2
#gst-launch tcpclientsrc host=$1 port=$2  ! jpegdec ! autovideosink
gst-launch udpsrc port=$2 ! smokedec ! autovideosink