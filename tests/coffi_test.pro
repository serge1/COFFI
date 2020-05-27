TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += $$files(../coffi/*.hpp)

SOURCES += coffi_test.cpp

INCLUDEPATH += ..

LIBS += -lboost_unit_test_framework-mt

LD_LIBRARY_PATH += C:\msys64\mingw32\lib
