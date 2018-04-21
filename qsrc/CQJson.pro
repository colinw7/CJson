TEMPLATE = lib

TARGET = CQJson

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++14

MOC_DIR = .moc

SOURCES += \
CQJsonModel.cpp \
CQJsonTable.cpp \
CQJsonTree.cpp \
CQHeaderView.cpp \

HEADERS += \
../qinclude/CQJsonModel.h \
../qinclude/CQJsonTable.h \
../qinclude/CQJsonTree.h \
CQHeaderView.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../qinclude \
../include \
../../CUtil/include \
