QT       += core gui printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KU-Control-Tuner
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/chart/chart_select_dialog.cpp \
    src/chart/chart_view_form.cpp \
    src/driver/tcp_driver_interface.cpp \
    src/driver/driver.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/tuner/parameter_tuner_form.cpp

HEADERS += \
    src/chart/chart_select_dialog.h \
    src/chart/chart_view_form.h \
    src/define.h \
    src/driver/tcp_driver_interface.hpp \
    src/driver/driver.hpp \
    src/mainwindow.h \
    src/tuner/parameter_tuner_form.h

FORMS += \
    ui/chart/chart_select_dialog.ui \
    ui/chart/chart_view_form.ui \
    ui/mainwindow.ui \
    ui/tuner/parameter_tuner_form.ui

SOURCES += \
        lib/QCustomPlot/qcustomplot.cpp


HEADERS += \
        lib/QCustomPlot/qcustomplot.h

RESOURCES += \
    resources/icon.qrc \
    resources/parameter.qrc
