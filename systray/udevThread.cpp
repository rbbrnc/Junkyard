#include "udevThread.h"
#include <unistd.h>
#include <sys/select.h>

#include <QDebug>

udevThread::udevThread()
{
	setTerminationEnabled(true);
}

udevThread::~udevThread()
{
	if (udev) {
		//qDebug() << __PRETTY_FUNCTION__ << "unref udev";
		udev_unref(udev);
		udev = 0;
	}

//	qDebug() << __PRETTY_FUNCTION__;
//	exit(0);
}

bool udevThread::init()
{

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		qDebug() << __PRETTY_FUNCTION__ << "Can't create udev";
		return false;
	}

	// Set up a monitor to monitor devices
	mon = udev_monitor_new_from_netlink(udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
	udev_monitor_enable_receiving(mon);

	return true;
}

void udevThread::stop()
{
//	qDebug() << __PRETTY_FUNCTION__;
	m_sc_run = false;
}

void udevThread::run()
{
	int fd = -1;

	m_sc_run = init();

	// Get the file descriptor for the monitor.
	fd = udev_monitor_get_fd(mon);

	while (m_sc_run) {
		fd_set fds;
		struct timeval tv;
		int ret;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		ret = select(fd + 1, &fds, NULL, NULL, NULL/*&tv*/);

		/* Check if our file descriptor has received data. */
		if (ret > 0 && FD_ISSET(fd, &fds)) {
			dev = udev_monitor_receive_device(mon);
			if (dev) {
#if 0
				qDebug() << "Action:" << udev_device_get_action(dev)
					 << "Node:" << udev_device_get_devnode(dev)
				         << "Subsystem:" << udev_device_get_subsystem(dev)
				         << "Devtype:" << udev_device_get_devtype(dev);
#endif
				//Devtype: disk | partition
				if (QString::compare("disk", udev_device_get_devtype(dev)) == 0) {
				}

				// Actions: add | remove | change | online | offline.
				//
				if (QString::compare("add", udev_device_get_action(dev)) == 0) {
					emit deviceAdd(udev_device_get_devnode(dev), udev_device_get_property_value(dev, "ID_SERIAL_SHORT"));
				} else if (QString::compare("remove", udev_device_get_action(dev)) == 0) {
					emit deviceRemove(udev_device_get_devnode(dev));
				} else if (QString::compare("change", udev_device_get_action(dev)) == 0) {
					// Get CD insert/remove
					if (udev_device_get_property_value(dev, "ID_CDROM")) {
						if (udev_device_get_property_value(dev, "ID_CDROM_MEDIA_STATE")) {
							emit deviceAdd(udev_device_get_devnode(dev), "CDR");
						} else {
							emit deviceRemove(udev_device_get_devnode(dev));
						}
					} else {
						emit deviceChange(udev_device_get_devnode(dev));
					}
				} else {
					//qDebug() << "Action:" << udev_device_get_action(dev);
				}

				//qDebug() << "ID_SERIAL_SHORT=" << udev_device_get_property_value(dev, "ID_SERIAL_SHORT");
				udev_device_unref(dev);
			} else {
				qDebug() << "No Device from receive_device(). An error occured.";
			}
		}
		//usleep(250*1000);
	}

	if (udev) {
		//qDebug() << __PRETTY_FUNCTION__ << "unref udev";
		udev_unref(udev);
		udev = 0;
	}
}

void udevThread::debugStatus()
{
}


