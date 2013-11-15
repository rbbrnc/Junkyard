#ifndef SEARCH_PAGE_H
#define SEARCH_PAGE_H

#include <QWidget>
#include <QFileSystemModel>

#include <QFutureWatcher>
#include <QProgressDialog>

namespace Ui {
	class SearchPage;
}


class SearchPage : public QWidget
{
	Q_OBJECT

	public:
		explicit SearchPage(QWidget *parent = 0);
		~SearchPage();

		QMap<QString, QStringList> dupMap() const;

//		void setRootPath(const QString &path);

	private:
		void fillFileList(const QString path, bool recursive);

		void addDuplicate(const QString key, const QString value);
		void removeNonDuplicate();

		// Debug Methods
		void debugDupMap() const;
		void debugFileList() const;

	signals:
		void changePage(int);

	public slots:
		void onFinished();
		void onReadyAt(int index);

	private slots:
		void on_dirTree_clicked(const QModelIndex &index);
		void on_insertDir_clicked();
		void on_removeDir_clicked();
		void on_workButton_clicked();
		void on_resultsButton_clicked();
		void on_clearDir_clicked();

		void on_compareType_currentIndexChanged(int index);

	        void on_tresholdHistogramDiffSlider_valueChanged(int value);

	private:
		Ui::SearchPage *ui;

		QFileSystemModel m_dirModel;
		QString		 m_currentDirSelection;
		QString		 m_currentWorkingDirSelection;

		bool m_mimeFilter;

		// map of duplicate file [Key = md5, Value = File Path]
		QMap<QString, QStringList> m_dupMap;
//		QMap<QString, ColorHistogram> m_histogramMap;

		QStringList m_fileList;  // files to compare

		QFutureWatcher<QPair<QString, QString> > m_futureWatcher;
		QProgressDialog *m_progressDialog;
};

#endif // SEARCH_PAGE_H
