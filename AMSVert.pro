#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T20:54:29
#
#-------------------------------------------------
QMAKE_CXXFLAGS += -std=c++11
#INCLUDEPATH += /usr/local/qwt-6.1.2/include
LIBS += -lqwt-qt5

QT       += core gui xml widgets

greaterThan(QT_MAJOR_VERSION, 4)

TARGET = AMSVert
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newamsdialog.cpp \
    amsmodel.cpp \
    amsfromfiles.cpp \
    resultdrawwidget.cpp

HEADERS  += mainwindow.h \
    newamsdialog.h \
    amsmodel.h \
    amsfromfiles.h \
    resultdrawwidget.h

FORMS    += mainwindow.ui \
    newamsdialog.ui \
    amsfromfiles.ui
