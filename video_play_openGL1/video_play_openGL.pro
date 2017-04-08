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
    match/match.cpp

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
    match/match.h

#INCLUDEPATH+=/usr/local/ffmpegarmlib/include
#/usr/local/ffmpegarmlib/lib/libavutil.so\
#        /usr/local/ffmpegarmlib/lib/libavcodec.so\
#        /usr/local/ffmpegarmlib/lib/libavcodec.so.52\
#        /usr/local/ffmpegarmlib/lib/libavdevice.so\
#        /usr/local/ffmpegarmlib/lib/libavformat.so\
#        /usr/local/ffmpegarmlib/lib/libswscale.so
#ffmpeg 新版本
INCLUDEPATH += /usr/local/arm/ffmpeg/include
INCLUDEPATH += /usr/local/arm/sdl/include
INCLUDEPATH += /usr/local/arm/lame/include/lame
INCLUDEPATH += /usr/local/arm/xvid/include
INCLUDEPATH += /usr/local/arm/x264/include
LIBS += -L/usr/local/arm/ffmpeg/lib -lavfilter -lavcodec -lavdevice\
        -lavformat -lavutil -lswresample -lswscale
LIBS += -L/usr/local/arm/sdl/lib -lSDL
LIBS += -L/usr/local/arm/lame/lib -lmp3lame
LIBS += -L/usr/local/arm/x264/lib -lx264
LIBS += -L/usr/local/arm/xvid/lib -lxvidcore -lpostproc

INCLUDEPATH += /usr/local/gpu/include
LIBS+=/usr/local/gpu/lib/libEGL.so\
/usr/local/gpu/lib/libEGL-fb.so\
/usr/local/gpu/lib/libGAL.so\
/usr/local/gpu/lib/libGLESv2.so\


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
