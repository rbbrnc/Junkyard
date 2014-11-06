#ifndef XMPP_EMBEDDED_CLIENT_H
#define XMPP_EMBEDDED_CLIENT_H

#include <QXmppClient.h>
#include <QXmppTransferManager.h>

class XmppEmbeddedClient : public QXmppClient
{
    Q_OBJECT

	public:
	    XmppEmbeddedClient(QObject *parent = 0);
	    ~XmppEmbeddedClient();

	private slots:
	    void onMessageReceived(const QXmppMessage &message);
	    void onConnected();

        // Transfer related
        void sendFile(const QString &toJid, const QString &fileName);
        void onTransferJobError(QXmppTransferJob::Error error);
        void onTransferJobFinished();
        void slotFileReceived(QXmppTransferJob *job);

    private:
        QXmppTransferManager *m_transferManager;

};

#endif
