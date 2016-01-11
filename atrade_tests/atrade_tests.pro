QMAKE_CXXFLAGS += -std=c++14

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    simple_market_simulator_tests.cpp

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../libs
INCLUDEPATH += $$PWD/../libs/googletest

LIBS += -L$$OUT_PWD/../atrade/ -latrade

LIBS += -L$$OUT_PWD/../libs/googletest -lgoogletest

LIBS += -lboost_iostreams -lboost_coroutine -lboost_context -lboost_filesystem -lboost_system -lpthread

HEADERS += \
    fake_market_data_source.h

