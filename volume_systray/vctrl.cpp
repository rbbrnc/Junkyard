#include <QtGui>
#include <QSlider>
#include <QMenu>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

#include "vctrl.h"
#include "mixer.h"

#define SLIDER_WIDTH   50
#define SLIDER_HEIGHT 150

Vctrl::Vctrl()
{
	if (::mixer_open() < 0) {
		qCritical() << "Cannot open mixer device";
	}

	int volume = ::mixer_get_volume();

	setWindowFlags(Qt::Popup);

	QVBoxLayout *layout = new QVBoxLayout(this);
	m_label = new QLabel(QString::number(volume), this);

	m_slider = new QSlider(Qt::Vertical, this);
    m_slider->setRange(0, 100);
    m_slider->setSingleStep(5);
    m_slider->setPageStep(10);
	m_slider->setValue(volume);
    m_slider->resize(SLIDER_WIDTH, SLIDER_HEIGHT);

	layout->addWidget(m_label);
	layout->addWidget(m_slider);

	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));

	// Create Actions
	quitAction = new QAction(QIcon(":/cancel.png"), tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	// Create Tray Icon
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(quitAction);
	trayIconMenu->addSeparator();

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);

	setTrayIcon(volume);
	trayIcon->setVisible(true);

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	trayIcon->show();

    resize(SLIDER_WIDTH, SLIDER_HEIGHT);
}

void Vctrl::setTrayIcon(int volume)
{
	if (volume == 0) {
		trayIcon->setIcon(volumeIcon.at(0));
	} else if (volume > 75) {
		trayIcon->setIcon(volumeIcon.at(3));
	} else if (volume > 30) {
		trayIcon->setIcon(volumeIcon.at(2));
	} else if (volume > 0) {
		trayIcon->setIcon(volumeIcon.at(1));
	} else {
		//volume < 0
	}
}

void Vctrl::onSliderValueChanged(int volume)
{
	(void) mixer_set_volume(volume);
	m_label->setText(QString::number(volume));
	setTrayIcon(volume);
}

Vctrl::~Vctrl()
{
	(void) ::mixer_close();
}

void Vctrl::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		hide();
		event->ignore();
	}
}

void Vctrl::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::MiddleClick:
	case QSystemTrayIcon::DoubleClick:
		onVolumeControl();
		break;
	default:
		break;
	}
}

void Vctrl::onVolumeControl()
{
	QRect r = trayIcon->geometry();
	move(QPoint(r.x(), r.y() - SLIDER_HEIGHT));
	setVisible(!isVisible());
}
