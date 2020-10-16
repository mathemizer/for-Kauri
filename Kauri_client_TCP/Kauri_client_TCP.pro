TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lws2_32\

SOURCES += \
        asn1_solver.cpp \
        client_unit_cl.cpp \
        client_unit_method_linux.cpp \
        client_unit_method_win.cpp \
        main.cpp

HEADERS += \
    asn1_solver.h \
    client_unit_cl.h \
    client_unit_method_linux.h \
    client_unit_method_win.h
