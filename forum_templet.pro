#-------------------------------------------------
#
# Project created by QtCreator 2014-08-11T11:29:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = forum_templet
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    columnmappingdialog.cpp

HEADERS  += mainwindow.hpp \
    columnmappingdialog.hpp \
    csvparser.hpp

FORMS    += mainwindow.ui \
    columnmappingdialog.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    resources.qrc
