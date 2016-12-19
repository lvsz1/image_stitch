TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    threadpool.cpp

HEADERS += \
    threadpool.h

LIBS += -lpthread
