#include <QApplication>
#include <QMessageBox>
#include "vctrl.h"

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(vctrl);

	QApplication app(argc, argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"),
		                      QObject::tr("I couldn't detect any system tray on this system."));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);

	Vctrl w;
	return app.exec();
}
