QMAKE_CXXFLAGS += -std=c++14

TARGET = strategies
TEMPLATE = lib
CONFIG += staticlib

SOURCES +=

HEADERS += \
#    simple_random_choise.hpp \
    all.hpp \
    simple_random_choise_2.hpp

INSTALLS += target

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../libs

LIBS += -L$$OUT_PWD/../../atrade/ -latrade

LIBS += -L$$OUT_PWD/../../functions/ -lfunctions

LIBS += -lboost_iostreams -lboost_coroutine -lboost_context -lboost_filesystem -lboost_system -lpthread
