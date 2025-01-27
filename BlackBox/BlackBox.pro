#-------------------------------------------------
#
# Project created by QtCreator 2012-02-17T16:52:20
#
#-------------------------------------------------

QT       += core gui svg
QT       += network
CONFIG += c++17
#CONFIG += console

#INCLUDEPATH += C:/qwt-6.2.0/include


#LIBS += -LC:/qwt-6.2.0/lib -lqwtd

RC_FILE = blackbox.rc

#qwt------------------------------------------------------------

INCLUDEPATH += C:/qwt/qwt-6.2.0/include
LIBS += -LC:/qwt/qwt-6.2.0/lib -lqwt

CONFIG(debug, debug|release) {
    LIBS += -LC:/qwt/qwt-6.2.0/lib/debug -lqwtd
}
CONFIG(release, debug|release) {
    LIBS += -LC:/qwt/qwt-6.2.0/lib/release -lqwtd
}

INCLUDEPATH += ../common
unix {
    QWT_ROOT=/usr/local/qwt-6.2.0/
    include ( $${QWT_ROOT}/features/qwt.prf )
}

win32 {
    CONFIG += qwt
}

#zlib-----------------------------------------------------------

INCLUDEPATH += D:/c++bibles/zlib-1.3.1
LIBS += -LD:/c++bibles/zlib-1.3.1 -lz

#quazip---------------------------------------------------------

INCLUDEPATH += D:\c++bibles\quazip\quazip
LIBS += -LD:\c++bibles\quazip\quazip\release -lquazip

#unrar---------------------------------------------------------

INCLUDEPATH += D:\c++bibles\unrar
LIBS += -LD:/c++bibles/unrar/x64 -lunrar64

#---------------------------------------------------------------

TARGET = BlackBox
TEMPLATE = app



SOURCES += main.cpp\
    ../common/can-trace.cpp \
    ../common/dbc-types.cpp \
    ../common/qwt_plot_advanced.cpp \
    ../common/utils.cpp \
    ../common/common.cpp \
    XYCurveData.cpp \
    mainwindow.cpp \
    mtab.cpp \
    mpower.cpp \
    testseq.cpp \
    zipper.cpp\

HEADERS  += mainwindow.h \
    ../common/can-trace.h \
    ../common/dbc-types.h \
    ../common/key-map.h \
    ../common/qwt_plot_advanced.h \
    ../common/series_data.h \
    ../common/utils.h \
    ../common/common.h \
    XYCurveData.h \
    mtab.h \
    mpower.h \
    testseq.h \
    zipper.h\

FORMS    += mainwindow.ui \
    mpower.ui \
    testseq.ui



