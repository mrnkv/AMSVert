#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T20:54:29
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AMSVert
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newamsdialog.cpp \
    amsmodel.cpp

HEADERS  += mainwindow.h \
    newamsdialog.h \
    amsmodel.h

FORMS    += mainwindow.ui \
    newamsdialog.ui
