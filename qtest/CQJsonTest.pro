TEMPLATE = app

QT += widgets

TARGET = CQJsonTest

DEPENDPATH += .

MOC_DIR = .moc

QMAKE_CXXFLAGS += -std=c++11

CONFIG += debug

SOURCES += \
CQJsonTest.cpp \

HEADERS += \
CQJsonTest.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

PRE_TARGETDEPS = \
$(LIB_DIR)/libCQJson.a \

INCLUDEPATH = \
. \
../include \
../qinclude \
../../CUtil/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CQJson/lib \
-L../../CStrUtil/lib \
\
-lCQJson -lCJson -lCStrUtil \
