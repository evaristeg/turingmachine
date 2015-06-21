TEMPLATE = app
QT += widgets
CONFIG += qt
#CONFIG += debug
QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
QMAKE_LFLAGS += -stdlib=libc++

# Input
HEADERS += Machine.hpp
HEADERS += MainWidget.hpp
HEADERS += ResetDialog.hpp
HEADERS += TuringMachine.hpp

SOURCES += main.cpp
SOURCES += MainWidget.cpp
SOURCES += MergeSort.cpp
SOURCES += ResetDialog.cpp
SOURCES += TuringMachine.cpp

