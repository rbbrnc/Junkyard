TARGET   = sad
TEMPLATE = app

CONFIG += silent

#-- Places for generated files
#DESTDIR = ../..
OBJECTS_DIR = out/objs
MOC_DIR     = out/mocs
UI_DIR      = out/ui
RCC_DIR     = out/resources


SOURCES += main.cpp \
	   searchpage.cpp \
	   mainwindow.cpp \
	   cmp_md5.cpp \
	   cmp_imagedata.cpp \
	   cmp_histogram.cpp

HEADERS += \
	   compare_functions.h \
	   searchpage.h \
	   mainwindow.h

FORMS   += \
	mainwindow.ui \
	searchpage.ui

SOURCES += histogram.cpp
HEADERS += histogram.h

#RESOURCES += $$PWD/libQExiv2/QExiv2.qrc

#-- Try clang
#QMAKE_CC  = clang
#QMAKE_CXX = clang++

#-- common gcc flags
QMAKE_CXXFLAGS += -Wall -W -Wunreachable-code
QMAKE_CXXFLAGS += -ffunction-sections -fdata-sections
QMAKE_LFLAGS   += -Wl,--gc-sections,--as-needed

#-- DEBUG Symbols
QMAKE_CXXFLAGS += -g -ggdb
#QMAKE_LFLAGS   += -Wl,--print-gc-sections
#DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

