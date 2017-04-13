SOURCES += \
    main.cpp \
    display/opengl.cpp \
    display/fslutil.c \
    display/fsl_egl.c \
    display/framebuffer.cpp \
    ipu/ipu.cpp \
    capture/videodevice.cpp \
    capture/thread_cap.cpp \
    capture/ThreadBase.cpp \
    match/match.cpp \
    vpu/capture.cpp \
    vpu/dec.cpp \
    vpu/display.cpp \
    vpu/enc.cpp \
    vpu/fb.cpp \
    vpu/transcode.cpp \
    vpu/utils.cpp \
    pushflow/ffmpeg.cpp \
    pushflow/librtmp_sendh264.cpp \
    pushflow/sps_decode.cpp \
    vpu/loopback.cpp \
    vpu/vpu_process.cpp \
    pushflow/buffer.cpp

HEADERS += \
    Include.h \
    FSL/fslutil.h \
    FSL/fsl_egl.h \
    display/opengl.h \
    display/mxcfb.h \
    display/framebuffer.h \
    ipu/ipu_csc.h \
    ipu/ipu.h \
    capture/v4l2.h \
    capture/thread_cap.h \
    capture/ThreadBase.h \
    capture/parameters.h \
    match/match.h \
    vpu/vpu_test.h \
    vpu/dut_probes_vts.h \
    vpu/dut_api_vts.h \
    pushflow/sps_decode.h \
    pushflow/librtmp_send264.h \
    pushflow/buffer.h

#vpu
INCLUDEPATH += /usr/local/vpu/include
LIBS += /usr/local/vpu/lib/*

#rtmp
INCLUDEPATH += /usr/local/arm/rtmp/include
LIBS += /usr/local/arm/rtmp/lib/librtmp.so

#INCLUDEPATH += /usr/local/arm/ffmpeg/include
#INCLUDEPATH += /usr/local/arm/sdl/include
#INCLUDEPATH += /usr/local/arm/lame/include/lame
#INCLUDEPATH += /usr/local/arm/xvid/include
#INCLUDEPATH += /usr/local/arm/x264/include
#LIBS += -L/usr/local/arm/ffmpeg/lib -lavfilter -lavcodec -lavdevice\
#        -lavformat -lavutil -lswresample -lswscale
#LIBS += -L/usr/local/arm/sdl/lib -lSDL
#LIBS += -L/usr/local/arm/lame/lib -lmp3lame
#LIBS += -L/usr/local/arm/x264/lib -lx264
#LIBS += -L/usr/local/arm/xvid/lib -lxvidcore -lpostproc
INCLUDEPATH += /usr/local/arm/sdl/include
INCLUDEPATH += /usr/local/arm/lame/include/lame
INCLUDEPATH += /usr/local/arm/xvid/include
LIBS += -L/usr/local/arm/sdl/lib -lSDL
LIBS += -L/usr/local/arm/lame/lib -lmp3lame
LIBS += -L/usr/local/arm/xvid/lib -lxvidcore -lpostproc

INCLUDEPATH += /usr/local/arm/1/ffmpeg/include
INCLUDEPATH += /usr/local/arm/1/h264/include
LIBS += -L/usr/local/arm/1/ffmpeg/lib -lavfilter -lavcodec -lavdevice\
        -lavformat -lavutil -lswresample -lswscale
LIBS += -L/usr/local/arm/1/h264/lib -lx264


#gpu
INCLUDEPATH += /usr/local/gpu/include
LIBS+=/usr/local/gpu/lib/libEGL.so\
/usr/local/gpu/lib/libEGL-fb.so\
/usr/local/gpu/lib/libGAL.so\
/usr/local/gpu/lib/libGLESv2.so\

#opencv
INCLUDEPATH += /usr/local/opencv-arm/include
INCLUDEPATH += /usr/local/opencv-arm/include/opencv
INCLUDEPATH += /usr/local/opencv-arm/include/opencv2

LIBS += /usr/local/opencv-arm/lib/libopencv_highgui.so
LIBS += /usr/local/opencv-arm/lib/libopencv_core.so
LIBS += /usr/local/opencv-arm/lib/libopencv_imgproc.so
LIBS += /usr/local/opencv-arm/lib/libopencv*.so



OTHER_FILES += \
    video_play_openGL.pro.user
TARGET = final
DESTDIR = /home/vincent/nfsroot
