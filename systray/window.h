#ifndef WINDOW_H
#define WINDOW_H

#include <QSystemTrayIcon>
#include <QDialog>
#include "udevThread.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class Window : public QDialog
{
	Q_OBJECT

	public:
		Window();
		~Window();

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:
		void iconActivated(QSystemTrayIcon::ActivationReason reason);
		void showMessage();
		void phiAct();

		void onDeviceAdd(const QString &node, const QString &serial);
		void onDeviceRemove(const QString &node);
		void onDeviceChange(const QString &node);

	private:
		QAction *quitAction;
		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu;

		udevThread *m_udevThread;
};

#endif
