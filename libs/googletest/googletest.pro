QMAKE_CXXFLAGS += -std=c++14 -DGTEST_USE_OWN_TR1_TUPLE=0 -DGTEST_HAS_PTHREAD=1

TARGET = googletest
TEMPLATE = lib

DEFINES += GOOGLETEST_LIBRARY

INSTALLS += target

HEADERS += \
    src/gtest-internal-inl.h \
    gtest/gtest.h \
    gtest/gtest-death-test.h \
    gtest/gtest-message.h \
    gtest/gtest-param-test.h \
    gtest/gtest-param-test.h.pump \
    gtest/gtest_pred_impl.h \
    gtest/gtest-printers.h \
    gtest/gtest_prod.h \
    gtest/gtest-spi.h \
    gtest/gtest-test-part.h \
    gtest/gtest-typed-test.h \
    gtest/internal/gtest-death-test-internal.h \
    gtest/internal/gtest-filepath.h \
    gtest/internal/gtest-internal.h \
    gtest/internal/gtest-linked_ptr.h \
    gtest/internal/gtest-param-util.h \
    gtest/internal/gtest-param-util-generated.h \
    gtest/internal/gtest-port.h \
    gtest/internal/gtest-port-arch.h \
    gtest/internal/gtest-string.h \
    gtest/internal/gtest-tuple.h \
    gtest/internal/gtest-tuple.h.pump \
    gtest/internal/gtest-type-util.h \
    gtest/internal/custom/gtest.h \
    gtest/internal/custom/gtest-port.h \
    gtest/internal/custom/gtest-printers.h

SOURCES += \
    src/gtest.cc \
    src/gtest-all.cc \
    src/gtest-death-test.cc \
    src/gtest-filepath.cc \
    src/gtest_main.cc \
    src/gtest-port.cc \
    src/gtest-printers.cc \
    src/gtest-test-part.cc \
    src/gtest-typed-test.cc

OTHER_FILES += \
    gtest/internal/gtest-param-util-generated.h.pump \
    gtest/internal/gtest-type-util.h.pump

LIBS += -lpthread
