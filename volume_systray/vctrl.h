#ifndef VCTRL_WINDOW_H
#define VCTRL_WINDOW_H

#include <QSystemTrayIcon>
#include <QWidget>

class QAction;
class QMenu;
class QSlider;
class QLabel;

class Vctrl : public QWidget
{
	Q_OBJECT

	public:
		Vctrl();
		~Vctrl();

	private:
		void setTrayIcon(int volume);

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:
		void iconActivated(QSystemTrayIcon::ActivationReason reason);
		void onVolumeControl();
	    void onSliderValueChanged(int volume);

	private:
		QAction *quitAction;
		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu;

		QLabel  *m_label;
		QSlider *m_slider;

		const QList<QIcon> volumeIcon = {
			QIcon(":/audio-volume-mute.png"),
			QIcon(":/audio-volume-low.png"),
			QIcon(":/audio-volume-medium.png"),
			QIcon(":/audio-volume-high.png")
		};


};

#endif
