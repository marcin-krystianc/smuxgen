# -------------------------------------------------
# Project created by QtCreator 2009-11-18T13:51:28
# -------------------------------------------------
QT += sql
QT += xml
QT += phonon

# CONFIG -= app_bundle
# TEMPLATE = app
TARGET = smuxgen
SOURCES += main.cpp \
    gui.cpp \
    coursetemplate.cpp \
    coursetemplateoptions.cpp \
    csmuxgenwidgets.cpp \
    cglobaltracer.cpp \
    csupermemosql.cpp \
    ccoursegenerator.cpp
HEADERS += gui.h \
    coursetemplate.h \
    coursetemplateoptions.h \
    csmuxgenwidgets.h \
    cglobaltracer.h \
    csupermemosql.h \
    ccoursegenerator.h
RESOURCES += gui.qrc
