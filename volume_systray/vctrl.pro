TARGET = volumecontrol
TEMPLATE = app

QT += widgets
CONFIG += silent
CONFIG += ALSAMixer
##CONFIG += SNDMixer

#-- Common gcc flags
QMAKE_CXXFLAGS += -Wall -W -Wunreachable-code
QMAKE_CXXFLAGS += -ffunction-sections -fdata-sections
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS   += -Wl,--gc-sections,--as-needed

#-- Debug Symbols
#QMAKE_CXXFLAGS += -g -ggdb
#QMAKE_LFLAGS   += -Wl,--print-gc-sections
#DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

HEADERS += vctrl.h	mixer.h
SOURCES += main.cpp vctrl.cpp

RESOURCES = vctrl.qrc

ALSAMixer {
	SOURCES += mixer_alsa.c
	LIBS += -lasound
}

SNDMixer {
	SOURCES += mixer_snd.c
}
