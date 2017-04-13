#!/bin/bash

IN1=/usr/local/opencv-arm/include
IN2=/usr/local/opencv-arm/include/opencv
IN3=/usr/local/opencv-arm/include/opencv2
IN4=/usr/local/qt-arm/include/QtCore/
IN5=/usr/local/qt-arm/include/
IN6=/usr/include/qt4/QtGui/

IN7=/usr/local/arm/ffmpeg/include
IN8=/usr/local/arm/sdl/include
IN9=/usr/local/arm/lame/include/lame
IN10=/usr/local/arm/xvid/include
IN11=/usr/local/arm/x264/include
LIB5=/usr/local/arm/ffmpeg/lib -lavfilter -lavcodec -lavdevice -lavformat -lavutil -lswresample -lswscale
LIB6=/usr/local/arm/sdl/lib -lSDL
LIB7=/usr/local/arm/lame/lib -lmp3lame
LIB8=/usr/local/arm/x264/lib -lx264
LIB9=/usr/local/arm/xvid/lib -lxvidcore -lpostproc


LIB1=/usr/local/opencv-arm/lib/libopencv_highgui.so
LIB2=/usr/local/opencv-arm/lib/libopencv_core.so
LIB3=/usr/local/opencv-arm/lib/libopencv_imgproc.so
LIB4=/usr/local/opencv-arm/lib/libopencv*.so

arm-linux-g++ EncodeSaver.cpp -fPIC -shared -o libEncodeSaver.so -I$IN1 -I$IN2 -I$IN3 -I$IN4 -I$IN5 -I$IN6 -I$IN11 -I$IN10 -I$IN9 -I$IN8 -I$IN7 -LLIB1 -LLIB2 -LLIB3 -LLIB4 -LLIB5 -LLIB6 -LLIB7 -LLIB8 -LLIB9

#arm-linux-g++ tab.cpp -fPIC -shared -o libtab.so -I$IN1 -I$IN2 -I$IN3 -I$IN4     -I$IN5 -I$IN6 -I$IN11 -I$IN10 -I$IN9 -I$IN8 -I$IN7 -LLIB1 -LLIB2 -LLIB3 -LL    IB4 -LLIB5 -LLIB6 -LLIB7 -LLIB8 -LLIB9

