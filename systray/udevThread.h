#ifndef UDEV_THREAD_H
#define UDEV_THREAD_H

#include <QThread>
#include <QtGui>

#include <libudev.h>

class udevThread : public QThread
{
	Q_OBJECT

	public:
		udevThread();
		~udevThread();

		void run();
		void stop(void);

	private:
		void debugStatus(void);
		bool init(void);

	signals:
		void deviceAdd(const QString &node, const QString &serial);
		void deviceRemove(const QString &node);
		void deviceChange(const QString &node);

	private:
		bool m_sc_run;

		struct udev *udev;
		struct udev_enumerate *enumerate;
		struct udev_list_entry *devices;
		struct udev_list_entry *dev_list_entry;
		struct udev_device *dev;
		struct udev_monitor *mon;
};

#endif
