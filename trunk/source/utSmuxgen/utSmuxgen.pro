#-------------------------------------------------
#
# Project created by QtCreator 2013-08-18T10:16:45
#
#-------------------------------------------------

QT       += core
QT       += testlib
QT       += phonon


TARGET = utSmuxgen
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    testglobalsmuxgentools.cpp \
    ../globalsmuxgentools.cpp \
    ../cglobaltracer.cpp

HEADERS += \
    testglobalsmuxgentools.h \
    ../globalsmuxgentools.h \
    ../cglobaltracer.h
