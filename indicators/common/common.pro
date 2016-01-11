QMAKE_CXXFLAGS += -std=c++14

TARGET = common
TEMPLATE = lib
CONFIG += staticlib

SOURCES +=

HEADERS += \
#    moex_indicator.hpp \
    moex_indicator_2.hpp \
    price_indicator.hpp \
    market_order_indicator.hpp \
    step_indicator.hpp \
    volume_indicator.hpp \
    price_indicator_2.hpp \
    cancel_indicator.hpp

INSTALLS += target

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../libs

LIBS += -L$$OUT_PWD/../../atrade/ -latrade

LIBS += -lboost_iostreams -lboost_coroutine -lboost_context -lboost_filesystem -lboost_system -lpthread
