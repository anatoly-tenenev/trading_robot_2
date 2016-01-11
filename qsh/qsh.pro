QMAKE_CXXFLAGS += -std=c++14

TARGET = qsh
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    exception.cpp \
    security.cpp \
    utils.cpp \
    exceptions/endofstreamexception.cpp \
    reader/iqshreader.cpp \
    reader/iqshstream.cpp \
    reader/qshreader.cpp \
    reader/v4/datareader.cpp \
    reader/v4/ordlogstream.cpp \
    reader/v4/qshreaderimpl.cpp \
    reader/v4/qshstream.cpp \
    types/deal.cpp \
    types/flags.cpp \
    types/leb128.cpp \
    types/ordlogentry.cpp \
    types/quote.cpp \
    types/rawquotes.cpp \
    types/uleb128.cpp

HEADERS += \
    exception.h \
    headers.h \
    qsh.hpp \
    security.hpp \
    utils.h \
    exceptions/endofstreamexception.h \
    reader/iqshreader.hpp \
    reader/iqshstream.hpp \
    reader/qshreader.hpp \
    reader/v4/datareader.hpp \
    reader/v4/ordlogstream.hpp \
    reader/v4/qshreaderimpl.hpp \
    reader/v4/qshstream.hpp \
    types/deal.h \
    types/flags.h \
    types/leb128.h \
    types/ordlogentry.h \
    types/quote.h \
    types/rawquotes.h \
    types/streamtype.h \
    types/types.h \
    types/uleb128.h

INCLUDEPATH += $$PWD/../

LIBS += -lboost_iostreams

INSTALLS += target
