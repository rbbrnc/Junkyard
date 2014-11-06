#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QXmppClient.h>
#include <QXmppMessage.h>
#include <QXmppTransferManager.h>


class QListWidgetItem;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

    private:
        //void debugRosters(const QStringList &bareJids);

        QString xmppStanzaErrorTypeToString(QXmppStanza::Error::Type type);
        QString xmppStanzaErrorConditionToString(QXmppStanza::Error::Condition cond);
        QString xmppStanzaErrorToString(const QXmppStanza::Error &error);

    private slots:
        void onPresenceReceived(const QXmppPresence &presence);
        void onMessageReceived(const QXmppMessage &message);
        void onConnected();
        void onDisconnected();
        void onError(QXmppClient::Error err);

        void onRosterReceived();
        void onPresenceChanged(const QString &bareJid, const QString &resource);

        void on_sendButton_clicked();
        void on_connectButton_clicked();

        void on_rosterListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

        void on_sendFileButton_clicked();

        // Transfer related
        void sendFile(const QString &toJid, const QString &fileName);
        void onTransferJobError(QXmppTransferJob::Error error);
        void onTransferJobFinished();
        void onFileReceived(QXmppTransferJob *job);


    private:
		Ui::MainWindow *ui;

        QXmppClient *m_xmppClient;
        QXmppTransferManager *m_transferManager;
};

#endif // MAINWINDOW_H
