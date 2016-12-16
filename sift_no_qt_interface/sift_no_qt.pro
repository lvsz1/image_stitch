TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    xform.c \
    utils.c \
    sift.c \
    minpq.c \
    kdtree.c \
    imgfeatures.c \
    siftmatch.cpp

HEADERS += \
    xform.h \
    utils.h \
    sift.h \
    minpq.h \
    kdtree.h \
    imgfeatures.h \
    siftmatch.h

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_highgui.so
LIBS += /usr/local/lib/libopencv_core.so
LIBS += /usr/local/lib/libopencv_imgproc.so
LIBS += /usr/local/lib/libopencv*.so
