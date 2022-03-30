QT       += core gui bluetooth serialport positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -lwiringPi

SOURCES += \
    accelerometer.cpp \
    autopilot.cpp \
    bluetoothserver.cpp \
    customlabel.cpp \
    drawchart.cpp \
    drawtrack.cpp \
    filtrkalmana.cpp \
    genalg.cpp \
    genalg2.cpp \
    gps.cpp \
    gyroscope.cpp \
    main.cpp \
    mainwindow.cpp \
    neuron.cpp \
    neuronet.cpp \
    sensorreader.cpp \
    slavecontroller.cpp

HEADERS += \
    accelerometer.h \
    autopilot.h \
    bluetoothserver.h \
    customlabel.h \
    drawchart.h \
    drawtrack.h \
    filtrkalmana.h \
    genalg.h \
    genalg2.h \
    gps.h \
    gyroscope.h \
    mainwindow.h \
    neuron.h \
    neuronet.h \
    sensorreader.h \
    slavecontroller.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
