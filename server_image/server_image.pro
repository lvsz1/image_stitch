TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    socket/sockettools.cpp \
    sift/xform.c \
    sift/utils.c \
    sift/siftmatch.cpp \
    sift/sift.c \
    sift/minpq.c \
    sift/kdtree.c \
    sift/imgfeatures.c \
    schedule/socketselect.cpp \
    schedule/threadpool.cpp \
    schedule/socketepoll.cpp \
    schedule/socketlibevent.cpp

HEADERS += \
    socket/sockettools.h \
    sift/xform.h \
    sift/utils.h \
    sift/siftmatch.h \
    sift/sift.h \
    sift/minpq.h \
    sift/kdtree.h \
    sift/imgfeatures.h \
    schedule/socketselect.h \
    schedule/io_limit.h \
    schedule/threadpool.h \
    schedule/socketepoll.h \
    schedule/socketlibevent.h

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_highgui.so
LIBS += /usr/local/lib/libopencv_core.so
LIBS += /usr/local/lib/libopencv_imgproc.so
LIBS += /usr/local/lib/libopencv*.so

LIBS += -levent
