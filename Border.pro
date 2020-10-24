#-------------------------------------------------
#
# Project created by QtCreator 2020-10-23T15:49:11
#
#-------------------------------------------------

QT       += gui

TARGET = Border
TEMPLATE = lib

DEFINES += _USE_MATH_DEFINES

DEFINES += BORDER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        border.cpp \
    BorderFinder.cpp \
    slicerutils.cpp

HEADERS += \
        border.h \
    BorderFinder.h \
    borderfinderConstants.h \
    slicerutils.h \
    border_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


    INCLUDEPATH += $$PWD/include

    LIBS += -L$$PWD/lib -lOpenMeshCore
    LIBS += -L$$PWD/lib -lOpenMeshCored
    LIBS += -L$$PWD/lib -OpenMeshTools
    LIBS += -L$$PWD/lib -OpenMeshToolsd
