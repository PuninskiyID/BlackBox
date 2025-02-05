QT += core gui widgets svg network

CONFIG += c++17

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

INCLUDEPATH += D:/cppLibs/zlib-1.3.1
LIBS += -LD:/cppLibs/zlib-1.3.1 -lz

#quazip---------------------------------------------------------

INCLUDEPATH += D:/cppLibs/quazip/quazip
LIBS += -LD:/cppLibs/quazip/quazip/release -lquazip

#unrar---------------------------------------------------------

INCLUDEPATH += D:/cppLibs/unrar
LIBS += -LD:/cppLibs/unrar/x64 -lunrar64

#QGoodWindow------------------------------------------------------------

INCLUDEPATH += D:/cppLibs/QGoodWindow/QGoodWindow/build-library/build/build-shared-with-central-widget/include
LIBS += -LD:/cppLibs/QGoodWindow/QGoodWindow/build-library/build/build-shared-with-central-widget/lib -lQGoodWindow

#---------------------------------------------------------------

TARGET = BlackBox
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    XYCurveData.cpp \
    mtab.cpp \
    mpower.cpp \
    testseq.cpp \
    zipper.cpp\
    ../common/can-trace.cpp \
    ../common/dbc-types.cpp \
    ../common/qwt_plot_advanced.cpp \
    ../common/utils.cpp \
    ../common/common.cpp \

HEADERS += \
    mainwindow.h \
    XYCurveData.h \
    mtab.h \
    mpower.h \
    testseq.h \
    zipper.h\
    ../common/can-trace.h \
    ../common/dbc-types.h \
    ../common/key-map.h \
    ../common/qwt_plot_advanced.h \
    ../common/series_data.h \
    ../common/utils.h \
    ../common/common.h \

FORMS += \
    centralwidget.ui \
    mpower.ui \
    testseq.ui\
