 QT += core
 QT -= gui
 CONFIG += c++11
 TARGET = QtCvTest
 CONFIG += console
 CONFIG -= app_bundle
 TEMPLATE = app
 SOURCES += main.cpp
 DEFINES += QT_DEPRECATED_WARNINGS

win32: {
    include(c:/dev/opencv/opencv.pri)
}
