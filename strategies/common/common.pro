QMAKE_CXXFLAGS += -std=c++14

TARGET = common
TEMPLATE = lib
CONFIG += staticlib

SOURCES +=

HEADERS += \
#    simple_trading_guard.hpp \
#    simple_closing.hpp \
    simple_closing_2.hpp \
    simple_trading_guard_2.hpp \
    scalper_strategy.hpp \
    scalper_strategy_2.hpp \
    research_strategy.hpp \
    simple_closing_3.hpp \
    scalper_strategy_3.hpp \
    all.hpp \
    scalper_strategy_4.hpp \
    scalper_strategy_5.hpp \
    scalper_strategy_6.hpp \
    calm_strategy.hpp \
    simple_trade_strategy.hpp \
    loop_strategy.hpp \
    calm_opp_strategy.hpp \
    mo_watcher_strategy.hpp

INSTALLS += target

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../libs

LIBS += -L$$OUT_PWD/../../functions/ -lfunctions

LIBS += -L$$OUT_PWD/../../atrade/ -latrade

LIBS += -lboost_iostreams -lboost_coroutine -lboost_context -lboost_filesystem -lboost_system -lpthread
