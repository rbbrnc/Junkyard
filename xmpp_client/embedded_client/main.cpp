#include <QCoreApplication>
#include <QDebug>

#include "xmppembeddedclient.h"

#include <QXmppConfiguration.h>
#include <QXmppLogger.h>

int main(int argc, char *argv[])
{
	if (argc < 3) {
		qDebug() << "Usage echoclient <host> <user> <password> [resource]";
		return -1;
	}

	QCoreApplication app(argc, argv);

	XmppEmbeddedClient client;


//	client.logger()->setLoggingType(QXmppLogger::StdoutLogging);
	client.logger()->setLoggingType(QXmppLogger::NoLogging);

	QXmppConfiguration cfg;
	cfg.setHost(argv[1]);
	cfg.setUser(argv[2]);
	cfg.setPassword(argv[3]);

	if (!QString(argv[4]).isEmpty()) {
		cfg.setResource(argv[4]);
	}
	cfg.setDomain("localhost");
	cfg.setSaslAuthMechanism("DIGEST-MD5");

	qDebug() << "Try to connect to:"
			 << cfg.host() << cfg.port()
			 << cfg.domain() << cfg.resource()
			 << cfg.user() << cfg.password() << cfg.jid();


	client.connectToServer(cfg, QXmppPresence());

	int rc = app.exec();

	client.disconnect();

	return rc;
}
