#-------------------------------------------------
#
# Project created by QtCreator 2012-06-04T17:21:19
#
#-------------------------------------------------

QT       += core gui network sql webkit

TARGET = downloadmanager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    downloadmanager.cpp \
    downloaddao.cpp \
    download.cpp \
    dbconnection.cpp \
    contact.cpp \
    downloadthread.cpp \
    downloadmanagerimpl.cpp

HEADERS  += mainwindow.h \
    downloadmanager.h \
    downloaddao.h \
    download.h \
    dbconnection.h \
    contact.h \
    common.h \
    downloadthread.h \
    downloadmanagerimpl.h

MOC_DIR += build/moc
OBJECTS_DIR += build/obj
DESTDIR += bin

FORMS    += mainwindow.ui






