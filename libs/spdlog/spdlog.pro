QMAKE_CXXFLAGS += -std=c++14

TARGET = spdlog
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    details/format.cc

HEADERS += \
    async_logger.h \
    common.h \
    formatter.h \
    logger.h \
    spdlog.h \
    tweakme.h \
    details/async_logger_impl.h \
    details/async_log_helper.h \
    details/file_helper.h \
    details/format.h \
    details/line_logger.h \
    details/logger_impl.h \
    details/log_msg.h \
    details/mpmc_bounded_q.h \
    details/null_mutex.h \
    details/os.h \
    details/pattern_formatter_impl.h \
    details/registry.h \
    details/spdlog_impl.h \
    sinks/android_sink.h \
    sinks/base_sink.h \
    sinks/dist_sink.h \
    sinks/file_sinks.h \
    sinks/null_sink.h \
    sinks/ostream_sink.h \
    sinks/sink.h \
    sinks/stdout_sinks.h \
    sinks/syslog_sink.h

INSTALLS += target
