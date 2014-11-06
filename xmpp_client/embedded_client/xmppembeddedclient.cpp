#include <QDebug>
#include <QBuffer>

#include <QXmppConfiguration.h>
#include <QXmppMessage.h>

#include "xmppembeddedclient.h"

XmppEmbeddedClient::XmppEmbeddedClient(QObject *parent)
    : QXmppClient(parent)
{
	connect(this, SIGNAL(messageReceived(QXmppMessage)), this, SLOT(onMessageReceived(QXmppMessage)));
	connect(this, SIGNAL(connected()), this, SLOT(onConnected()));

    // Add a transfer manager
    m_transferManager = new QXmppTransferManager;

    // CAPIRE PROXY?!?!?!?
    //m_transferManager->setProxy("localhost");
    //m_transferManager->setProxy("proxy.qxmpp.org");
    addExtension(m_transferManager);

    connect(m_transferManager, SIGNAL(fileReceived(QXmppTransferJob *)), this, SLOT(slotFileReceived(QXmppTransferJob *)));
}

XmppEmbeddedClient::~XmppEmbeddedClient()
{
}

void XmppEmbeddedClient::onConnected()
{
	QXmppConfiguration cfg = configuration();

	qDebug() << "Connected on:"
		 << cfg.host()
		 << cfg.port()
		 << cfg.domain()
		 << cfg.saslAuthMechanism()
		 << cfg.resource()
		 << cfg.user() << cfg.password() << cfg.jid();
}

void XmppEmbeddedClient::onMessageReceived(const QXmppMessage& message)
{
	QString from = message.from();
	QString msg = message.body();

	qDebug() << __func__
			 << "[" << configuration().jid() << "]"
			 << "To:" << message.to()
			 << "From:" << from
			 << "Message:" << msg;

    // Se la risorsa nn e' la mia rispondi picche!
    // TODO: parse message

    //sendFile(from, "/tmp/testfile.txt");
    sendPacket(QXmppMessage("", from, "OK!"));
}

/// A file transfer request was received.
void XmppEmbeddedClient::slotFileReceived(QXmppTransferJob *job)
{
    qDebug() << "Got transfer request from:" << job->jid();

    // Abort transfers from unknown users.
    // TODO.!!

    connect(job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(onTransferJobError(QXmppTransferJob::Error)));
    connect(job, SIGNAL(finished()), this, SLOT(onTransferJobFinished()));
    //connect(job, SIGNAL(progress(qint64,qint64)), this, SLOT(slotProgress(qint64,qint64)));

    //QXmppTransferFileInfo 	fileInfo () const

    qDebug() << "QUrl:" << job->localFileUrl()
             << "Info name:" << job->fileInfo().name()
             << "Info Size:" << job->fileInfo().size()
             << "Description:" << job->fileInfo().description();


    QString fileName = job->fileInfo().name();
    if (fileName.isEmpty()) {
        fileName = "/tmp/aaa";
    } else {
        fileName.prepend("/tmp/");
    }
    job->accept(fileName);
}

void XmppEmbeddedClient::sendFile(const QString &toJid, const QString &fileName)
{
    // if we don't have a recipient,  or
    // if we don't have a valid file, or
    // if the recipient is not available,
    // do nothing
    if (toJid.isEmpty() || fileName.isEmpty()) {

        // OR not available
        /*
        QXmppUtils::jidToBareJid(presence.from()) != toJism || presence.type() != QXmppPresence::Available) */
        return;
    }

    qDebug() << __func__ << "To:" << toJid << "File:" << fileName;

    // send the file and connect to the job's signals
    QXmppTransferJob *job = m_transferManager->sendFile(toJid, fileName, "transfer example");

    connect(job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(onTransferJobError(QXmppTransferJob::Error)));
    connect(job, SIGNAL(finished()), this, SLOT(onTransferJobFinished()));
    //connect(job, SIGNAL(progress(qint64,qint64)), this, SLOT(slotProgress(qint64,qint64)));
}

void XmppEmbeddedClient::onTransferJobError(QXmppTransferJob::Error error)
{
    qDebug() << "Transmission failed:";

    switch (error) {
    case QXmppTransferJob::NoError:
        qDebug() << "No error";
        break;
    case QXmppTransferJob::AbortError:
        qDebug() << "The file transfer was aborted.";
        break;
    case QXmppTransferJob::FileAccessError:
        qDebug() << "An error was encountered trying to access a local file.";
        break;
    case QXmppTransferJob::FileCorruptError:
        qDebug() << "The file is corrupt: the file size or hash do not match.";
        break;
    case QXmppTransferJob::ProtocolError:
        qDebug() << "An error was encountered in the file transfer protocol.";
        break;
    default:
        qDebug() << "[?]";
        break;
    }
}

void XmppEmbeddedClient::onTransferJobFinished()
{
    QXmppTransferJob *job = qobject_cast<QXmppTransferJob *>(sender());

    if (QXmppTransferJob::NoError != job->error()) {
        return;
    }

    if (QXmppTransferJob::OutgoingDirection == job->direction()) {
        qDebug() << __PRETTY_FUNCTION__ << "Sent:" << "[" << job->localFileUrl().toString() << "]";;
    } else {
        // Here we have: QXmppTransferJob::IncomingDirection == job->direction()
        qDebug() << __PRETTY_FUNCTION__ << "Received:" << "[" << job->localFileUrl().toString() << "]";;
    }
}

