QMAKE_CXXFLAGS += -std=c++14

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../libs

LIBS += -L$$OUT_PWD/../../atrade/ -latrade

LIBS += -L$$OUT_PWD/../../qsh/ -lqsh

LIBS += -lboost_iostreams -lboost_coroutine -lboost_context -lboost_filesystem -lboost_system -lpthread

