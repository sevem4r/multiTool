#-------------------------------------------------
#
# Project created by QtCreator 2018-09-05T12:41:01
#
#-------------------------------------------------

QT       += core gui widgets serialport quickwidgets printsupport

TARGET = multiTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    bootloader.cpp \
    expression.cpp \
        main.cpp \
        mainwindow.cpp \
    qcustomplot.cpp \
    consoleinput.cpp \
    consoleoutput.cpp \
    serialport.cpp \
    graph.cpp \
    hexloader.cpp \
    datalogger.cpp \
    tinyexpr.c

HEADERS += \
    bootloader.h \
    expression.h \
        mainwindow.h \
    qcustomplot.h \
    consoleinput.h \
    consoleoutput.h \
    serialport.h \
    graph.h \
    hexloader.h \
    datalogger.h \
    tinyexpr.h

FORMS += \
        mainwindow.ui

RC_FILE = icon.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
