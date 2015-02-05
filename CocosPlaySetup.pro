#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T11:34:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CocosPlaySetup
TEMPLATE = app


SOURCES += main.cpp\
        dlgsetup.cpp \
    cctrldialog.cpp \
    cscale9image.cpp \
    pythontoolthread.cpp \
    pythreadstatelock.cpp \
    noticetip.cpp

HEADERS  += dlgsetup.h \
    cctrldialog.h \
    cscale9image.h \
    pythontoolthread.h \
    pythreadstatelock.h \
    noticetip.h

FORMS    += dlgsetup.ui

RESOURCES += \
    data.qrc

win32:{
INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

INCLUDEPATH += $$PWD/libs
DEPENDPATH += $$PWD/libs

LIBS += -L$$PWD/libs/ -lpython27
}
