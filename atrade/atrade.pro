QMAKE_CXXFLAGS += -std=c++14

TARGET = atrade
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    marketbase.cpp \
    backtester/eventbus.cpp \
    backtester/backtester.cpp \
    backtester/marketsimulator.cpp \
    utils.cpp \
    backtester/simplemarketsimulator.cpp \
    security.cpp \
    backtester/qshdatasource.cpp \
    backtester/marketdatasourcebase.cpp \
    backtester/_utils.cpp \
    globals.cpp \
    backtester/ordlogmarketsimulator.cpp \
    types/order.cpp \
#    async/strategy.cpp \
#    async/operation.cpp \
#    async/_asyncmarket.cpp \
#    async/_marketdata.cpp \
#    async/_error_code.cpp \
#    async/_history.cpp \
#    async/_portfolio.cpp \
#    async/_task.cpp \
#    async/_tradingcontext.cpp \
    async2/asyncmarket.cpp \
    async2/history.cpp \
    async2/marketdata.cpp \
    async2/portfolio.cpp \
    async2/task.cpp \
    async2/error_code.cpp \
    async2/trading_context.cpp \
    async2/_static_vars.cpp \
    async2/routines.cpp \
    types/deal.cpp \
    async2/market_service.cpp

HEADERS += \
    headers.h \
    atrade.hpp \
    types/types.h \
    imarket.h \
    marketbase.h \
    types/order.h \
    backtester/eventbus.h \
    backtester/backtester.h \
    backtester/marketsimulator.h \
    backtester/routines.hpp \
    utils.hpp \
    backtester/simplemarketsimulator.h \
    types/deal.h \
    security.h \
#    async/tradingcontext.h \
#    async/asyncmarket.h \
#    async/async.hpp \
#    async/timer.hpp \
#    async/task.h \
#    async/add_order.hpp \
#    async/subscription.hpp \
#    async/error_code.hpp \
#    async/req_securities.hpp \
#    async/make_deal.hpp \
#    async/history.h \
#    async/cancel_order.hpp \
#    async/indicator.hpp \
#    async/watcher.hpp \
#    async/portfolio.hpp \
#    async/strategy.hpp \
#    async/marketdata.hpp \
#    async/exceptions.hpp \
#    async/operation.hpp \
    backtester/imarketdatasource.h \
    backtester/qshdatasource.h \
    backtester/marketdatasourcebase.h \
    backtester/backtester.hpp \
    backtester/exceptions.h \
    types/quote.h \
    backtester/types.h \
    types/ordlogentry.h \
    backtester/utils.hpp \
    globals.h \
    backtester/ordlogmarketsimulator.h \
    async2/marketdata.hpp \
    async2/portfolio.hpp \
    async2/subscription.hpp \
    async2/exceptions.hpp \
    async2/add_order.hpp \
    async2/error_code.hpp \
    async2/asyncmarket.hpp \
    async2/history.hpp \
    async2/task.hpp \
    async2/trading_context.hpp \
    async2/timer.hpp \
    async2/watcher.hpp \
    async2/indicator.hpp \
    async2/async2.hpp \
    async2/req_securities.hpp \
    async2/wait_deal.hpp \
    async2/cancel_order.hpp \
    async2/spawner.hpp \
    async2/routines.hpp \
    async2/strategy.hpp \
    async2/market_service.hpp \
    async2/move_order.hpp

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../libs/

LIBS += -L$$OUT_PWD/../qsh/ -lqsh

LIBS += -lboost_iostreams -lboost_filesystem -lboost_system

INSTALLS += target
