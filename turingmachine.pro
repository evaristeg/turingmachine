TEMPLATE = app
QT += widgets
CONFIG += qt
CONFIG += debug
QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
QMAKE_LFLAGS += -stdlib=libc++

# Input
HEADERS += TuringMachine.hpp

SOURCES += main.cpp
SOURCES += TuringMachine.cpp

