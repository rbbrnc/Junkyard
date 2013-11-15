#include "searchpage.h"
#include "ui_searchpage.h"

#include <QMessageBox>
#include <QtConcurrentMap>

#include <QDebug>

#include "compare_functions.h"

#ifdef QT_NO_CONCURRENT
#error Qt cuncurrent needed!!
#endif

SearchPage::SearchPage(QWidget *parent) :
	QWidget(parent), ui(new Ui::SearchPage)
{
	ui->setupUi(this);

	ui->tresholdLayout->hide();

	m_dirModel.setRootPath("");
//	m_dirModel.setRootPath("/home");
	m_dirModel.setFilter(QDir::Dirs | QDir::NoDotAndDotDot); // Only Directories

	ui->dirTree->setModel(&m_dirModel);

	// Do it after the model has been initialized
	ui->dirTree->header()->hide();
	ui->dirTree->hideColumn(1);
	ui->dirTree->hideColumn(2);
	ui->dirTree->hideColumn(3);

	m_progressDialog = new QProgressDialog();
	m_progressDialog->setWindowModality(Qt::WindowModal);

	connect(&m_futureWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(onReadyAt(int)));
	connect(&m_futureWatcher, SIGNAL(finished()), this, SLOT(onFinished()));
	connect(&m_futureWatcher, SIGNAL(finished()), m_progressDialog, SLOT(reset()));
	connect(&m_futureWatcher, SIGNAL(progressRangeChanged(int,int)), m_progressDialog, SLOT(setRange(int,int)));
	connect(&m_futureWatcher, SIGNAL(progressValueChanged(int)), m_progressDialog, SLOT(setValue(int)));

	connect(m_progressDialog, SIGNAL(canceled()), &m_futureWatcher, SLOT(cancel()));

}

void SearchPage::onReadyAt(int index)
{
//	qDebug() << __func__ << index << m_futureWatcher.future().resultAt(index);

	// The result QPair is filename, key (eg. md5)
	QPair<QString, QString> p = m_futureWatcher.future().resultAt(index);

	// Insert only valid pairs
	if (p.first.isNull() || p.second.isNull()) {
		return;
	}

	addDuplicate(p.second, p.first);
}

/*
void SearchPage::setRootPath(const QString &path)
{
	if (path.isNull()) {
		m_dirModel.setRootPath("");
	} else {
		m_dirModel.setRootPath(path);
	}
}
*/

SearchPage::~SearchPage()
{
	delete m_progressDialog;
	delete ui;
}

// [private SLOT] Selection changed on directory tree
void SearchPage::on_dirTree_clicked(const QModelIndex &index)
{
	QFileInfo fi = m_dirModel.fileInfo(index);
	m_currentDirSelection = fi.absoluteFilePath();
}

// [private SLOT] Insert entry into the selected directory list
void SearchPage::on_insertDir_clicked()
{
	QList<QListWidgetItem *> lst;
	lst = ui->dirList->findItems(m_currentDirSelection, Qt::MatchExactly);

	// Do not add duplicate item
	if (lst.isEmpty()) {
		ui->dirList->addItem(m_currentDirSelection);
		//qDebug() << __PRETTY_FUNCTION__ << "Insert item:" << m_currentDirSelection;
	}
}

// [private SLOT] Remove entry from the selected directory list
void SearchPage::on_removeDir_clicked()
{
	delete ui->dirList->takeItem(ui->dirList->currentRow());
}

// [private SLOT] Clear the selected directory list
void SearchPage::on_clearDir_clicked()
{
	if (ui->dirList->count()) {
		if (QMessageBox::Yes == QMessageBox::question(this, tr("Clear List"), "Clear Directory List?", QMessageBox::Yes, QMessageBox::No)) {
			ui->dirList->clear();
		}
	}
}

// [private SLOT]
void SearchPage::on_compareType_currentIndexChanged(int index)
{
	if (index == 1) {
		ui->tresholdValue->setText(QString::number(ui->tresholdHistogramDiffSlider->value()));
		ui->tresholdLayout->show();
	} else {
		ui->tresholdLayout->hide();
	}
}

// [private SLOT]
void SearchPage::on_tresholdHistogramDiffSlider_valueChanged(int value)
{
	ui->tresholdValue->setText(QString::number(value));
}

// [private SLOT]
void SearchPage::on_resultsButton_clicked()
{
//	emit changePage(DUPLICATE_PAGE);
}

// [private SLOT]
void SearchPage::on_workButton_clicked()
{
	if (ui->dirList->count() < 1) {
		QMessageBox::information(this, tr("Find Duplicate"), "No Dir Selected");
		return;
	}

	if (!m_fileList.isEmpty()) {
		m_fileList.clear();
	}

	for (int i = 0; i < ui->dirList->count(); i++) {
		QString path = ui->dirList->item(i)->text();
		fillFileList(path, ui->recursiveCheck->isChecked());
	}
	debugFileList();

	if (m_fileList.isEmpty()) {
		QMessageBox::information(this, tr("Find Duplicate"), "No Image in Directory List");
		return;
	}

	m_dupMap.clear();

#if 0 // XXX
	m_histogramMap.clear();
#endif

	// Start the job
//	QFuture<QPair<QString, QString> > future;

	// TODO: For MD5, Image and Byte-to-Byte compare only files with the same format
	switch (ui->compareType->currentIndex()) {
	case 0:
		// Compare Method: "File MD5"
		//future = QtConcurrent::mappedReduced(m_fileList, cm_md5_map, cm_md5_reduce);
//		future = QtConcurrent::mapped(m_fileList, cm_md5_map);
//		m_futureWatcher.setFuture(future);
		m_futureWatcher.setFuture(QtConcurrent::mapped(m_fileList, cm_md5_map));
		break;
	case 1:
		::compareHistogram(m_fileList, ui->tresholdHistogramDiffSlider->value());
		// Compare Method: "Color Histogram"
		//future = QtConcurrent::run(cm_histogram,
		//		ui->tresholdHistogramDiffSlider->value(),
		//		m_fileList);

		//m_futureWatcher.setFuture(future);
		break;
	case 2:
		// Compare Method: "Image, Only skip Metadata"
		m_futureWatcher.setFuture(QtConcurrent::mapped(m_fileList, cm_imageData_map));
		break;
	case 3:
		// Compare Method: "Metadata Only Skip Image Data
		//compareMetadata();
		break;
	case 4:
		// Compare Method: "Byte-to-Byte"
		//compareByteToByte();
		break;
	default:
		qDebug() << __PRETTY_FUNCTION__ << "Unknown Compare Method!";
		return;
	}

	// Start compare job
	m_progressDialog->exec();
}

#if 0
QStringList findFiles(const QString &startDir, QStringList filters)
{
    QStringList names;
    QDir dir(startDir);

    foreach (QString file, dir.entryList(filters, QDir::Files))
        names += startDir + "/" + file;

    foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
        names += findFiles(startDir + "/" + subdir, filters);
    return names;
}
#endif

void SearchPage::fillFileList(const QString path, bool recursive)
{
	QDir dir;
	dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	dir.setCurrent(path);

	QFileInfoList list = dir.entryInfoList();

//	qDebug() << "DIR:" << qPrintable(QDir::current().path());

	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fi = list.at(i);
		if (fi.isDir()) {
			if ((fi.fileName() != ".") && (fi.fileName() != "..")) {
				if (recursive) {
					QDir::setCurrent(fi.absoluteFilePath());
					fillFileList(fi.absoluteFilePath(), recursive);
					QDir::setCurrent("..");
				}
			}
		} else {
			//qDebug() << "FILE:" << qPrintable(fi.absoluteFilePath());
#if 0 //----
			FileData fd(fi);

			// Check mime filter
			if (ui->mimeFilterImages->isChecked()) {
				if (!fd.isImage()) {
					continue;
				}
			}
			m_fileList << fd;
#else
			m_fileList << fi.absoluteFilePath();
#endif
		}
	}
}

// [public SLOT]
void SearchPage::onFinished()
{
	if (m_futureWatcher.isCanceled()) {
		qDebug() << "CANCEL";

		m_dupMap.clear();
		return;
	}

	// Remove entries without duplicates
	removeNonDuplicate();

	qDebug() << __PRETTY_FUNCTION__
		 << "File processed:" << m_fileList.size()
		 << "xxx" << "Duplicate in" << m_dupMap.size() << "sets.";

	if (m_dupMap.isEmpty()) {
		QMessageBox::information(this, tr("Find Duplicate"), "No Duplicate found!");
	} else {
		// XXX enable show result page button.
		debugDupMap();
	}
}

QMap<QString, QStringList> SearchPage::dupMap() const
{
	return m_dupMap;
}

// Adds an entry to the map of duplicates.
void SearchPage::addDuplicate(const QString key, const QString value)
{
	QStringList sl;

	if (m_dupMap.contains(key)) {
		// <key> already in the map, get <value> list for updating.
		sl = m_dupMap[key];
	}

	sl << value;		// Update <value> file list
	m_dupMap[key] = sl;	// Add to map
}

// Remove entries without duplicates
void SearchPage::removeNonDuplicate()
{
	QMap<QString, QStringList>::const_iterator i = m_dupMap.constBegin();
	 while (i != m_dupMap.constEnd()) {
		if (i.value().size() < 2) {
			//qDebug() << "Removing key:" << i.key() << ":" << i.value();
			m_dupMap.remove(i.key());
		}
		++i;
	 }
}

/// DEBUG ============================================================================
//
void SearchPage::debugFileList() const
{
	qDebug() << __PRETTY_FUNCTION__ << "File count:" << m_fileList.size();
#if 0 // ___OK___
	qDebug() << "-------" << __PRETTY_FUNCTION__ << "-----------";
	if (m_fileList.isEmpty()) {
		qDebug() << "No file found!";
	} else {
		for (int i = 0; i < m_fileList.size(); i++) {
			//qDebug() << m_fileList.at(i).filePath();
			qDebug() << m_fileList.at(i);
		 }
	}
	qDebug() << "-------------------------";
#endif
}

void SearchPage::debugDupMap() const
{
	qDebug() << "-------" << __PRETTY_FUNCTION__ << "-----------";
	if (m_dupMap.isEmpty()) {
		qDebug() << "No Duplicate found!";
	} else {
		// (Debug) Map
		QMap<QString, QStringList>::const_iterator i = m_dupMap.constBegin();
		 while (i != m_dupMap.constEnd()) {
			qDebug() << i.key() << ": " << i.value();
			++i;
		 }
	}
	qDebug() << "-------------------------";
}


