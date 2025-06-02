QT += core gui widgets serialbus svg network

CONFIG += c++17

RC_FILE = blackbox.rc

#qwt------------------------------------------------------------------------------------------------------

INCLUDEPATH += C:\Qt\addons/qwt-6.2.0/include
LIBS += -LC:\Qt\addons/qwt-6.2.0/lib -lqwt

CONFIG(debug, debug|release) {
    LIBS += -LC:\Qt\addons/qwt-6.2.0/lib/debug -lqwtd
}
CONFIG(release, debug|release) {
    LIBS += -LC:\Qt\addons/qwt-6.2.0/lib/release -lqwtd
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

INCLUDEPATH += A:\MainWork\DIPLOM\zlib-1.3.1
LIBS += -LA:\MainWork\DIPLOM\zlib-1.3.1 -lz

#quazip---------------------------------------------------------

INCLUDEPATH += A:\MainWork\DIPLOM\quazip\quazip
LIBS += -LA:\MainWork\DIPLOM\quazip\quazip\release -lquazip

#unrar---------------------------------------------------------

INCLUDEPATH += A:\MainWork\DIPLOM\unrar
LIBS += -LA:\MainWork\DIPLOM\unrar\x64 -lunrar64

#QGoodWindow------------------------------------------------------------------------------------------------------

INCLUDEPATH += C:\Qt\addons\QGoodWindow\QGoodWindow\build-library\build\build-shared-with-central-widget\include
LIBS += -LC:\Qt\addons\QGoodWindow\QGoodWindow\build-library\build\build-shared-with-central-widget\lib -lQGoodWindow

#FFTW---------------------------------------------------------------------------------------------------

INCLUDEPATH += A:\MainWork\DIPLOM\vcpkg\installed\x64-windows\include
LIBS += -LA:\MainWork\DIPLOM\vcpkg\installed\x64-windows\lib -lfftw3

----------------------------------------------------------

TARGET = BlackBox
TEMPLATE = app

SOURCES += \
    analysisagent.cpp \
    filteragent.cpp \
    main.cpp \
    mainwindow.cpp \
    XYCurveData.cpp \
    mtab.cpp \
    mpower.cpp \
    signalanalysiswidget.cpp \
    testseq.cpp \
    zipper.cpp\
    ../common/can-trace.cpp \
    ../common/dbc-types.cpp \
    ../common/qwt_plot_advanced.cpp \
    ../common/utils.cpp \
    ../common/common.cpp \

HEADERS += \
    ../include/zmq.h \
    ../include/zmq_utils.h \
    analysisagent.h \
    filteragent.h \
    mainwindow.h \
    XYCurveData.h \
    mtab.h \
    mpower.h \
    signalanalysiswidget.h \
    testseq.h \
    zipper.h\
    ../common/can-trace.h \
    ../common/dbc-types.h \
    ../common/key-map.h \
    ../common/qwt_plot_advanced.h \
    ../common/series_data.h \
    ../common/utils.h \
    ../common/common.h \
    zmq.h \
    zmq_utils.h

FORMS += \
    centralwidget.ui \
    mpower.ui \
    testseq.ui\

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./ -llibzmq-v143-mt-4_3_6
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./ -llibzmq-v143-mt-4_3_6d
#else:unix: LIBS += -L$$PWD/./ -llibzmq-v143-mt-4_3_6

#INCLUDEPATH += $$PWD/.
#DEPENDPATH += $$PWD/.

#INCLUDEPATH += A:/MainWork/DIPLOM/BlackBox/BlackBox/include
#LIBS += -LA:/MainWork/DIPLOM/BlackBox/BlackBox -lzmq-v143-mt-4_3_6d

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./liblibzmq-v143-mt-4_3_6.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./liblibzmq-v143-mt-4_3_6d.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./libzmq-v143-mt-4_3_6.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./libzmq-v143-mt-4_3_6d.lib
#else:unix: PRE_TARGETDEPS += $$PWD/./liblibzmq-v143-mt-4_3_6.a
