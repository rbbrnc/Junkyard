#include <QtGui>
#include <QProcess>
#include "window.h"

#include <QDebug>

#define MSG_DURATION (10 * 1000)	// message duration (ms)

Window::Window()
{
	// Create Actions
	quitAction = new QAction(QIcon(":/cancel.png"), tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	// Create Tray Icon
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(quitAction);
	trayIconMenu->addSeparator();

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);

	QIcon icon = QIcon(":/omega.png");
	trayIcon->setIcon(icon);
	trayIcon->setVisible(true);

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	trayIcon->show();

	// Create Udev Thread
	udevThread *m_udevThread = new udevThread();
	connect(m_udevThread, SIGNAL(deviceAdd(const QString &, const QString &)),
		this,         SLOT(onDeviceAdd(const QString &, const QString &)));

	connect(m_udevThread, SIGNAL(deviceRemove(const QString &)), this, SLOT(onDeviceRemove(const QString &)));
	connect(m_udevThread, SIGNAL(deviceChange(const QString &)),  this, SLOT(onDeviceChange(const QString &)));
	m_udevThread->start();
}

Window::~Window()
{
}

void Window::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		hide();
		event->ignore();
	}
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		break;
	case QSystemTrayIcon::MiddleClick:
		showMessage();
		break;
	default:
		break;
	}
}

void Window::showMessage()
{
	trayIcon->showMessage("KUdev", "KUdev v.1.0.9", QSystemTrayIcon::Information, MSG_DURATION);
}

void Window::onDeviceAdd(const QString &node, const QString &serial)
{
	QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
	trayIcon->showMessage("Device Add", node, icon, MSG_DURATION);

	QAction *act = new QAction("PhiAct", this);
	act->setText(node);
	act->setData(serial);
	connect(act, SIGNAL(triggered()), this, SLOT(phiAct()));
	trayIconMenu->addAction(act);

	//qDebug() << __PRETTY_FUNCTION__;
}

void Window::onDeviceRemove(const QString &node)
{
	QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
	trayIcon->showMessage("Device Remove", node, icon, MSG_DURATION);

	QList<QAction *> acts = trayIconMenu->actions();
	for (int i = 0; i < acts.count(); i++) {
		if (0 == QString::compare(acts.at(i)->text(), node)) {
			trayIconMenu->removeAction(acts.at(i));
			//qDebug() << __PRETTY_FUNCTION__ << "Remove" << node;
		}
	}
//	qDebug() << __PRETTY_FUNCTION__;
}

// usb-notify needs root privileges, so..
// 1. chown root:root <yourbin>
// 2. chmod +s <yourbin>
void Window::phiAct()
{
	QAction *act = qobject_cast<QAction *>(sender());

	qDebug() << "PHI:" << act->data().toString();
	QStringList arguments;
	arguments << act->text() << act->data().toString();
	bool b = QProcess::startDetached("/sbin/usb-notify", arguments);
	if (b) {
		QIcon icon = QIcon(":/accept.png");
		act->setIcon(icon);
	} else {
		qDebug() << "PHI: none";
	}
}

void Window::onDeviceChange(const QString &node)
{
//	qDebug() << __PRETTY_FUNCTION__ << "Change" << node;
}

