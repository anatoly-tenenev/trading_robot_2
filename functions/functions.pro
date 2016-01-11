QMAKE_CXXFLAGS += -std=c++14

TARGET = functions
TEMPLATE = lib
CONFIG += staticlib

SOURCES += functions.cpp

HEADERS += functions.hpp

INSTALLS += target

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../libs

LIBS += -L$$OUT_PWD/../atrade/ -latrade

LIBS += -lboost_iostreams -lboost_coroutine -lboost_context -lboost_filesystem -lboost_system -lpthread
