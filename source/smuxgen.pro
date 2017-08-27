# -------------------------------------------------
# Project created by QtCreator 2009-11-18T13:51:28
# -------------------------------------------------

QT += multimedia
QT += sql
QT += xml
QT += widgets

# CONFIG - = app_bundle
# TEMPLATE = app
TARGET = smuxgen
SOURCES += main.cpp \
 gui.cpp \
 coursetemplate.cpp \
 coursetemplateoptions.cpp \
 globalsmuxgentools.cpp \
 globaltracer.cpp \
 coursegenerator.cpp \
 imagedownloader.cpp \
 sapi.cpp \
 smuxgenwidgets.cpp \
 supermemosql.cpp \
    codeeditor.cpp
HEADERS += gui.h \
 coursetemplate.h \
 globalsmuxgentools.h \
 courseoptions.h \
 coursegenerator.h \
 globaltracer.h \
 imagedownloader.h \
 sapi.h \
 smuxgenwidgets.h \
 supermemosql.h \
 codeeditor.h
RESOURCES += gui.qrc
FORMS += 
