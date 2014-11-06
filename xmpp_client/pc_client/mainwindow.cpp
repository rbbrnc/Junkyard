#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QBuffer>

#include <QXmppConfiguration.h>
#include <QXmppLogger.h>
#include <QXmppRosterManager.h>
#include <QXmppUtils.h>
#include <QXmppStanza.h>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    ui->errorLabel->setText("");

    ui->hostLlineEdit->setText("localhost");
    ui->portSpinBox->setValue(5222);
    ui->domainLineEdit->setText("localhost");

    ui->userLineEdit->setText("test");
    ui->passwordLineEdit->setText("test");

    m_xmppClient = new QXmppClient(this);

//    m_xmppClient->logger()->setLoggingType(QXmppLogger::StdoutLogging);
    m_xmppClient->logger()->setLoggingType(QXmppLogger::NoLogging);

    connect(m_xmppClient, SIGNAL(presenceReceived(QXmppPresence)), this, SLOT(onPresenceReceived(QXmppPresence)));
    connect(m_xmppClient, SIGNAL(messageReceived(QXmppMessage)), this, SLOT(onMessageReceived(QXmppMessage)));
    connect(m_xmppClient, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_xmppClient, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_xmppClient, SIGNAL(error(QXmppClient::Error)), this, SLOT(onError(QXmppClient::Error)));

    QXmppRosterManager &rmgr = m_xmppClient->rosterManager();
    connect(&rmgr, SIGNAL(rosterReceived()), this, SLOT(onRosterReceived()));
    connect(&rmgr, SIGNAL(presenceChanged(QString, QString)), this, SLOT(onPresenceChanged(QString, QString)));

    // Add a transfer manager
    m_transferManager = new QXmppTransferManager;

    // CAPIRE PROXY?!?!?!?
    //m_transferManager->setProxy("localhost");
    //m_transferManager->setProxy("proxy.qxmpp.org");
    m_xmppClient->addExtension(m_transferManager);
    connect(m_transferManager, SIGNAL(fileReceived(QXmppTransferJob *)), this, SLOT(onFileReceived(QXmppTransferJob *)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_sendButton_clicked()
{
    m_xmppClient->sendPacket(QXmppMessage("", ui->toLineEdit->text(), ui->messageTextEdit->toPlainText()));
}

void MainWindow::on_connectButton_clicked()
{
    if (m_xmppClient->isConnected()) {
        m_xmppClient->disconnectFromServer();
        return;
    }

    QXmppConfiguration cfg;

    cfg.setHost(ui->hostLlineEdit->text());
    cfg.setUser(ui->userLineEdit->text());
    cfg.setPassword(ui->passwordLineEdit->text());
    cfg.setResource(ui->resourceLineEdit->text());
    cfg.setDomain(ui->domainLineEdit->text());
    cfg.setSaslAuthMechanism(ui->saslAuthComboBox->currentText());

    ui->errorLabel->setText("");
/*
    qDebug() << "Try to connect to:"
             << cfg.host() << cfg.port()
             << cfg.domain() << cfg.resource()
             << cfg.user() << cfg.password() << cfg.jid();
*/
    m_xmppClient->connectToServer(cfg, QXmppPresence());
}


void MainWindow::onMessageReceived(const QXmppMessage &message)
{
    if (message.type() == QXmppMessage::Error) {
        QString errorString = xmppStanzaErrorTypeToString(message.error().type())
                + " sending message to " + message.from() + " "
                + xmppStanzaErrorConditionToString(message.error().condition())
                + message.error().text();

        ui->logText->appendPlainText(errorString);
        return;
    }

    QString jid = message.from();
    QString txt = QString("%1 [%2]: %3")
                        .arg(QXmppUtils::jidToUser(jid))
                        .arg(QXmppUtils::jidToResource(jid))
                        .arg(message.body());

    ui->logText->appendPlainText(txt);
}

void MainWindow::onConnected()
{
    ui->onlineLabel->setText("Online");
    ui->errorLabel->setText("");
    ui->onlineIconLabel->setPixmap(QPixmap(":/pixmaps/bullet_green.png"));
    ui->connectButton->setText("Disconnect");
    ui->sendButton->setEnabled(true);
}

void MainWindow::onDisconnected()
{
    ui->onlineLabel->setText("Offline");
    ui->onlineIconLabel->setPixmap(QPixmap(":/pixmaps/bullet_black.png"));
    ui->connectButton->setText("Connect");
    ui->sendButton->setEnabled(false);
}

QString MainWindow::xmppStanzaErrorTypeToString(QXmppStanza::Error::Type type)
{
    switch (type) {
    case QXmppStanza::Error::Cancel:   return "Cancel";
    case QXmppStanza::Error::Continue: return "Continue";
    case QXmppStanza::Error::Modify:   return "Modify";
    case QXmppStanza::Error::Auth:     return "Auth";
    case QXmppStanza::Error::Wait:     return "Wait";
    default:
        return "?";
        break;
    }
}

QString MainWindow::xmppStanzaErrorConditionToString(QXmppStanza::Error::Condition cond)
{
    switch (cond) {
    case QXmppStanza::Error::BadRequest:            return "Bad Request";
    case QXmppStanza::Error::Conflict:              return "Conflict";
    case QXmppStanza::Error::FeatureNotImplemented: return "Feature Not Implemented";
    case QXmppStanza::Error::Forbidden:             return "Forbidden";
    case QXmppStanza::Error::Gone:                  return "Gone";
    case QXmppStanza::Error::InternalServerError:   return "Internal Server Error";
    case QXmppStanza::Error::ItemNotFound:          return "Item Not Found";
    case QXmppStanza::Error::JidMalformed:          return "Jid Malformed";
    case QXmppStanza::Error::NotAcceptable:         return "Not Acceptable";
    case QXmppStanza::Error::NotAllowed:            return "Not Allowed";
    case QXmppStanza::Error::NotAuthorized:         return "Not Authorized";
    case QXmppStanza::Error::PaymentRequired:       return "Payment Required";
    case QXmppStanza::Error::RecipientUnavailable:  return "Recipient Unavailable";
    case QXmppStanza::Error::Redirect:              return "Redirect";
    case QXmppStanza::Error::RegistrationRequired:  return "Registration Required";
    case QXmppStanza::Error::RemoteServerNotFound:  return "Remote Server Not Found";
    case QXmppStanza::Error::RemoteServerTimeout:   return "Remote Server Timeout";
    case QXmppStanza::Error::ResourceConstraint:    return "Resource Constraint.";
    case QXmppStanza::Error::ServiceUnavailable:    return "Service Unavailable";
    case QXmppStanza::Error::SubscriptionRequired:  return "Subscription Required";
    case QXmppStanza::Error::UndefinedCondition:    return "Undefined Condition";
    case QXmppStanza::Error::UnexpectedRequest:     return "Unexpected Request";
    default:
        return "?";
    }
}

QString MainWindow::xmppStanzaErrorToString(const QXmppStanza::Error &error)
{
    QString errorString;
    QString errorType = xmppStanzaErrorTypeToString(error.type());
    QString errorCondition = xmppStanzaErrorConditionToString(error.condition());

    errorString = errorType + " " + errorCondition + " " + error.text();
    return errorString;
}

void MainWindow::onError(QXmppClient::Error err)
{
    QString errorString;

    switch (err) {
    case QXmppClient::NoError:
        break;

    case QXmppClient::SocketError:
        errorString = m_xmppClient->socketErrorString();
        break;
    case QXmppClient::KeepAliveError:
        // Error due to no response to a keep alive.
        errorString = "Error due to no response to a keep alive.";
        break;
    case QXmppClient::XmppStreamError:
        // Error due to XML stream.
        errorString = xmppStanzaErrorConditionToString(m_xmppClient->xmppStreamError());
        break;
    default:
        // Unknown error code
        errorString = QString("Unknown XmppClient Error: %1").arg(err);
        break;
    }

    //qDebug() <<__PRETTY_FUNCTION__ << errorString;
    ui->errorLabel->setText(errorString);
    if (!errorString.isNull()) {
        ui->onlineIconLabel->setPixmap(QPixmap(":/pixmaps/bullet_red.png"));
    }
}

#if 0
void MainWindow::debugRosters(const QStringList &bareJids)
{
    QXmppRosterManager &rmgr = m_xmppClient->rosterManager();

    qDebug() << "Roster List entries:" << bareJids.count();

    for (int i = 0; i < bareJids.count(); i++) {
        QMap<QString, QXmppPresence> pmap = rmgr.getAllPresencesForBareJid(bareJids.at(i));
        qDebug() << bareJids.at(i) << "PresenceMAP entries:" << pmap.count() << "Keys:" << pmap.keys();


        QStringList resList = rmgr.getResources(bareJids.at(i));
        qDebug() << "Resource List entries:" << resList.count();

        for (int j = 0; j < resList.count(); j++) {
                QXmppPresence pres = rmgr.getPresence(bareJids.at(i), resList.at(j));
                qDebug() << bareJids.at(i) << "RES:" << resList.at(j);
                onPresenceReceived(pres);
        }
    }
}
#endif

void MainWindow::onPresenceChanged(const QString &bareJid, const QString &resource)
{
    qDebug() << __func__ << "Presence changed:" << qPrintable(bareJid) << qPrintable(resource);

    QString presenceIconName;

    QXmppRosterManager &rmgr = m_xmppClient->rosterManager();
    QXmppPresence presence = rmgr.getPresence(bareJid, resource);
    switch (presence.type()) {
    case QXmppPresence::Error:
        presenceIconName = ":/pixmaps/bullet_red.png";
        break;
    case QXmppPresence::Available:
        presenceIconName = ":/pixmaps/bullet_green.png";
        break;
    case QXmppPresence::Unavailable:
        presenceIconName = ":/pixmaps/bullet_black.png";
        break;
    case QXmppPresence::Subscribe:
    case QXmppPresence::Subscribed:
    case QXmppPresence::Unsubscribe:
    case QXmppPresence::Unsubscribed:
    case QXmppPresence::Probe:
        presenceIconName = ":/pixmaps/bullet_yellow.png";
        break;
    default:
        presenceIconName = ":/pixmaps/bullet_red.png";
        break;
    }

    // Don't add this client!
    if (bareJid == m_xmppClient->configuration().jidBare()) {
        ui->onlineIconLabel->setPixmap(presenceIconName);
        return;
    }

    QString jid = bareJid + "/" + resource;

    QList<QListWidgetItem *> items = ui->rosterListWidget->findItems(jid, Qt::MatchFixedString | Qt::MatchCaseSensitive);
    if (items.count() > 0) {
        items[0]->setIcon(QIcon(presenceIconName));
    } else {
        ui->rosterListWidget->addItem(jid);
        int row = ui->rosterListWidget->count() - 1;
        ui->rosterListWidget->item(row)->setIcon(QIcon(presenceIconName));
    }
}

void MainWindow::onRosterReceived()
{
    QXmppRosterManager &rmgr = m_xmppClient->rosterManager();

    QStringList rosterJids = rmgr.getRosterBareJids();

//    debugRosters(rosterJids);

    ui->rosterListWidget->clear();
    ui->rosterListWidget->addItems(rosterJids);
/*
    for (int i = 0; i < ui->rosterListWidget->count(); i++) {
        ui->rosterListWidget->item(i)->setIcon(QIcon(":/pixmaps/bullet_black.png"));
    }
*/
}

void MainWindow::onPresenceReceived(const QXmppPresence &presence)
{
#if 0
    QString presenceString;
    QString presenceIconName = ":/pixmaps/bullet_red.png";

    switch (presence.type()) {
    case QXmppPresence::Error:
        // An error has occurred regarding processing or delivery of a
        // previously-sent presence stanza.
        presenceString = "[Error]";
        break;
    case QXmppPresence::Available:
        presenceString = "[Available]";
        presenceIconName = ":/pixmaps/bullet_green.png";
        break;
    case QXmppPresence::Unavailable:
        presenceString = "[Unavailable]";
        presenceIconName = ":/pixmaps/bullet_black.png";
        break;
    case QXmppPresence::Subscribe:
        // The sender wishes to subscribe to the recipient's presence.
        presenceString = "[Subscribe]";
        break;
    case QXmppPresence::Subscribed:
        // The sender has allowed the recipient to receive their presence.
        presenceString = "[Subscribed]";
        break;
    case QXmppPresence::Unsubscribe:
        // The sender is unsubscribing from another entity's presence.
        presenceString = "Unsubscribe";
        break;
    case QXmppPresence::Unsubscribed:
        // The subscription request has been denied or a previously-granted
        // subscription has been cancelled.
        presenceString = "[Unsubscribed]";
        break;
    case QXmppPresence::Probe:
        // A request for an entity's current presence; SHOULD be generated only
        // by a server on behalf of a user.
        presenceString = "[Probe]";
        break;
    default:
        presenceString = "[?]";
        break;
    }

    QString id = presence.from(); // id = QXmppUtils::jidToBareJid(presence.from()); // Use Bare jid
/*
    QList<QListWidgetItem *> items = ui->rosterListWidget->findItems(id, Qt::MatchFixedString | Qt::MatchCaseSensitive);
    if (items.count() > 0) {
        items[0]->setIcon(QIcon(presenceIconName));
    } else {
        qDebug() << __func__ << id << "Item not found";
    }
*/
    qDebug() << __func__ << qPrintable(id) << presenceString;
#endif
}

void MainWindow::on_rosterListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current) {
        ui->toLineEdit->setText(current->text());
    }
}

void MainWindow::on_sendFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        sendFile(ui->toLineEdit->text(), fileName);
    }
}

/// A file transfer request was received.
void MainWindow::onFileReceived(QXmppTransferJob *job)
{
    qDebug() << "Got transfer request from:" << job->jid();

    connect(job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(onTransferJobError(QXmppTransferJob::Error)));
    connect(job, SIGNAL(finished()), this, SLOT(onTransferJobFinished()));
    //connect(job, SIGNAL(progress(qint64,qint64)), this, SLOT(slotProgress(qint64,qint64)));

    qDebug() << /*QUrl*/ job->localFileUrl();
    // allocate a buffer to receive the file
    QBuffer *buffer = new QBuffer(this);
    buffer->open(QIODevice::WriteOnly);
    job->accept(buffer);
}

void MainWindow::sendFile(const QString &toJid, const QString &fileName)
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

    // Send the file and connect to the job's signals
    // Transfer description is taken from message body widget
    QXmppTransferJob *job = m_transferManager->sendFile(toJid, fileName, ui->messageTextEdit->toPlainText());

    connect(job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(onTransferJobError(QXmppTransferJob::Error)));
    connect(job, SIGNAL(finished()), this, SLOT(onTransferJobFinished()));
    //connect(job, SIGNAL(progress(qint64,qint64)), this, SLOT(slotProgress(qint64,qint64)));
}

void MainWindow::onTransferJobError(QXmppTransferJob::Error error)
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

void MainWindow::onTransferJobFinished()
{
    qDebug() << "Transmission finished";

}

