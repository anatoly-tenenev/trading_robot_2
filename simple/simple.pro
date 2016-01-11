QMAKE_CXXFLAGS += -std=c++14

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += $$PWD/../

LIBS += -L$$OUT_PWD/../atrade/ -latrade

LIBS += -L$$OUT_PWD/../qsh/ -lqsh

LIBS += -lboost_iostreams
