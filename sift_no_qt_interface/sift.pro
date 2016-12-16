#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T14:07:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sift
TEMPLATE = app


SOURCES += main.cpp \
    xform.c \
    utils.c \
    siftmatch.cpp \
    sift.c \
    minpq.c \
    kdtree.c \
    imgfeatures.c

HEADERS  +=     xform.h \
    utils.h \
    siftmatch.h \
    sift.h \
    minpq.h \
    kdtree.h \
    imgfeatures.h

FORMS    += siftmatch.ui

INCLUDEPATH+=D:\opencv\opencv2.4.3\include\opencv\
             D:\opencv\opencv2.4.3\include\opencv2\
             D:\opencv\opencv2.4.3\include

LIBS+=D:\opencv\opencv2.4.3\lib\libopencv_calib3d243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_contrib243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_core243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_features2d243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_flann243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_gpu243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_highgui243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_imgproc243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_legacy243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_ml243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_objdetect243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_video243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_nonfree243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_photo243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_stitching243.dll.a\
        D:\opencv\opencv2.4.3\lib\libopencv_videostab243.dll.a
