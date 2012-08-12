#-------------------------------------------------
#
#
#-------------------------------------------------

QT       += core gui

TARGET = Reno_term
TEMPLATE = app

CONFIG += static
CONFIG += qwt

include($${PWD}/../src/qserialdeviceenumerator/qserialdeviceenumerator.pri)
include($${PWD}/../src/qserialdevice/qserialdevice.pri)


#INCLUDEPATH += /home/granik/qserialdevice-qserialdevice-0.4.0/qserialdevice-qserialdevice/src/qserialdevice
#NCLUDEPATH += /home/granik/qserialdevice-qserialdevice-0.4.0/qserialdevice-qserialdevice/src/qserialdeviceenumerator #$${PWD}/../src

#QMAKE_LIBDIR += /home/granik/qserialdevice-qserialdevice-0.4.0/BuildLibrary0/src/build/release
#LIBS += -lqserialdevice

#DEPENDPATH  += $${PWD}/../src

#QMAKE_RPATHDIR *= $${PWD}/../qwt-6.0.0/lib

#QMAKE_CFLAGS -= -m64
#QMAKE_LFLAGS -= -m64
#QMAKE_CXXFLAGS -= -m64


#QMAKE_CFLAGS += -m32 -march=i586
#QMAKE_LFLAGS += -m32 -march=i586
#QMAKE_CXXFLAGS += -m32 -march=i586

#win32 {
#    contains(QWT_CONFIG, QwtDll) {
#        DEFINES    += QT_DLL QWT_DLL
#    }
#}

SOURCES += main.cpp\
        mainwindow.cpp \
    optionswidget.cpp \
    plot.cpp \
    infowidget.cpp \
    rec.cpp

HEADERS  += mainwindow.h \
    infowidget.h \
    plot.h \
    optionswidget.h \
    rec.h

FORMS    += mainwindow.ui \
    infowidget.ui \
    infowidget.ui \
    optionswidget.ui \
    rec.ui

win32 {
    LIBS += -lsetupapi -luuid -ladvapi32
}
unix:!macx {
    LIBS += -ludev
}

OTHER_FILES +=



