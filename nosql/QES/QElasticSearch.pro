TEMPLATE = app
TARGET = qes
DEPENDPATH += .
INCLUDEPATH += .

QT += core gui widgets network webkit

include ($$PWD/jsontree/JsonTree.pri)

CONFIG += silent

# Input
HEADERS += \
    mainwindow.h \
	elasticsearch.h \
    jsoneditorform.h \
    optionform.h \
    searchform.h \
    netservices.h \
    basepage.h \
    pages.h

SOURCES += \
	main.cpp \
    mainwindow.cpp \
	elasticsearch.cpp \
    jsoneditorform.cpp \
    optionform.cpp \
    searchform.cpp \
    netservices.cpp \
    basepage.cpp

FORMS += \
    mainwindow.ui \
    jsoneditorform.ui \
    optionform.ui \
    searchform.ui

#-- common gcc flags
QMAKE_CXXFLAGS += -Wall -W -Wunreachable-code
QMAKE_CXXFLAGS += -ffunction-sections -fdata-sections
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS   += -Wl,--gc-sections,--as-needed

#-- DEBUG Symbols
QMAKE_CXXFLAGS += -g -ggdb
#QMAKE_LFLAGS   += -Wl,--print-gc-sections
#DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

RESOURCES += \
    application.qrc


