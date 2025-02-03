QT += core gui widgets

CONFIG += c++11

#QGoodWindow------------------------------------------------------------

INCLUDEPATH += D:/cppLibs/QGoodWindow/QGoodWindow/build-library/build/build-shared-with-central-widget/include
LIBS += -LD:/cppLibs/QGoodWindow/QGoodWindow/build-library/build/build-shared-with-central-widget/lib -lQGoodWindow

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    centralwidget.ui
