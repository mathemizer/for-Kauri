TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        asn1_resolver.cpp \
        main.cpp \
        my_semaphore.cpp \
        server_unit_cl.cpp \
        server_unit_method_linux.cpp \
        server_unit_method_win.cpp

LIBS += -lws2_32\

HEADERS += \
    asn1_resolver.h \
    my_semaphore.h \
    server_unit_cl.h \
    server_unit_method_linux.h \
    server_unit_method_win.h
