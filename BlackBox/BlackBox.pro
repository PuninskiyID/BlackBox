QT += core gui widgets svg network

CONFIG += c++17

RC_FILE = blackbox.rc

PROJECT_ROOT = $$clean_path($$_PRO_FILE_PWD_/..)
isEmpty(THIRDPARTY): THIRDPARTY = $$clean_path($$_PRO_FILE_PWD_/../../thirdparty)
THIRDPARTY = $$clean_path($${THIRDPARTY})

message("Project root: $${PROJECT_ROOT}")
message("Third-party libraries: $${THIRDPARTY}")

#qwt------------------------------------------------------------

INCLUDEPATH += $${THIRDPARTY}/qwt-6.2.0/include
LIBS += -L$${THIRDPARTY}/qwt-6.2.0/lib -lqwt

CONFIG(debug, debug|release) {
    LIBS += -L$${THIRDPARTY}/qwt-6.2.0/lib/debug -lqwtd
}
CONFIG(release, debug|release) {
    LIBS += -L$${THIRDPARTY}/qwt-6.2.0/lib/release -lqwtd
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

ZLIB_DIR = $$clean_path($${THIRDPARTY}/zlib-1.3.1)
!exists($${ZLIB_DIR}/zlib.h): error("Zlib header not found: $${ZLIB_DIR}/zlib.h")
win32:!exists($${ZLIB_DIR}/libz.a): error("Zlib MinGW library not found: $${ZLIB_DIR}/libz.a")
unix:!exists($${ZLIB_DIR}/libz.so):!exists($${ZLIB_DIR}/libz.a): error("Zlib library not found in: $${ZLIB_DIR}")

INCLUDEPATH += $${ZLIB_DIR}
LIBS += -L$${ZLIB_DIR} -lz

#quazip---------------------------------------------------------

QUAZIP_DIR = $$clean_path($${THIRDPARTY}/quazip)
QUAZIP_INCLUDE = $$clean_path($${QUAZIP_DIR}/quazip)
QUAZIP_LIB_DIR = $$clean_path($${QUAZIP_DIR}/quazip/release)
!exists($${QUAZIP_INCLUDE}/quazip.h): error("QuaZip header not found: $${QUAZIP_INCLUDE}/quazip.h")
win32:!exists($${QUAZIP_LIB_DIR}/libquazip.a): error("QuaZip MinGW library not found: $${QUAZIP_LIB_DIR}/libquazip.a")
unix:!exists($${QUAZIP_LIB_DIR}/libquazip.so):!exists($${QUAZIP_LIB_DIR}/libquazip.a): error("QuaZip library not found in: $${QUAZIP_LIB_DIR}")

INCLUDEPATH += $${QUAZIP_INCLUDE}
LIBS += -L$${QUAZIP_LIB_DIR} -lquazip

#unrar---------------------------------------------------------

UNRAR_DIR = $$clean_path($${THIRDPARTY}/unrar)
UNRAR_LIB_DIR = $$clean_path($${UNRAR_DIR}/x64)
!exists($${UNRAR_DIR}/UnRAR.h): error("UnRAR header not found: $${UNRAR_DIR}/UnRAR.h")
win32:!exists($${UNRAR_LIB_DIR}/UnRAR64.lib): error("UnRAR MinGW library not found: $${UNRAR_LIB_DIR}/UnRAR64.lib")
unix:!exists($${UNRAR_LIB_DIR}/UnRAR64.so):!exists($${UNRAR_LIB_DIR}/libunrar64.a): error("UnRAR library not found in: $${UNRAR_LIB_DIR}")

INCLUDEPATH += $${UNRAR_DIR}
LIBS += -L$${UNRAR_LIB_DIR} -lunrar64

#QGoodWindow------------------------------------------------------------

QGOODWINDOW_DIR = $$clean_path($${THIRDPARTY}/QGoodWindow/QGoodWindow/build-library/build/build-shared-with-central-widget)
QGOODWINDOW_INCLUDE = $$clean_path($${QGOODWINDOW_DIR}/include)
QGOODWINDOW_LIB = $$clean_path($${QGOODWINDOW_DIR}/lib)

!exists($${QGOODWINDOW_INCLUDE}/QGoodWindow):!exists($${QGOODWINDOW_INCLUDE}/QGoodWindow.h): error("QGoodWindow header not found in: $${QGOODWINDOW_INCLUDE}")
win32:!exists($${QGOODWINDOW_LIB}/libQGoodWindow.a): error("QGoodWindow MinGW library not found: $${QGOODWINDOW_LIB}/libQGoodWindow.a")
unix:!exists($${QGOODWINDOW_LIB}/libQGoodWindow.so):!exists($${QGOODWINDOW_LIB}/libQGoodWindow.a): error("QGoodWindow library not found in: $${QGOODWINDOW_LIB}")

INCLUDEPATH += $${QGOODWINDOW_INCLUDE}
LIBS += -L$${QGOODWINDOW_LIB} -lQGoodWindow

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
