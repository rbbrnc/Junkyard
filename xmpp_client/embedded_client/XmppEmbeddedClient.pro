TARGET = eClient

QT += xml network

CONFIG += console
CONFIG += silent

## INCLUDEPATH += $$QXMPP_INCLUDEPATH
## LIBS += $$QXMPP_LIBS
DEFINES += QXMPP_STATIC

SOURCES += \
	main.cpp \
	xmppembeddedclient.cpp

HEADERS += \
	xmppembeddedclient.h

unix:contains(QT_ARCH, powerpc): {
#	DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
	QMAKE_CXXFLAGS += -Wall -W -g -ggdb

	INCLUDEPATH += /opt/eldk4.2/ppc_6xx/usr/local/qxmpp/include/qxmpp
	LIBS += /opt/eldk4.2/ppc_6xx/usr/local/qxmpp/lib/libqxmpp.a
}

unix:contains(QT_ARCH, i386): {
#	DEFINES +=
	QMAKE_CXXFLAGS += -Wall -W -Wunreachable-code -g -ggdb

#	INCLUDEPATH += /opt/eldk4.2/ppc_6xx/usr/local/qxmpp/include/qxmpp
#	LIBS += /opt/eldk4.2/ppc_6xx/usr/local/qxmpp/lib/libqxmpp.a

}

