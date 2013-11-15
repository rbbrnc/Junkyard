//#include <iostream>
//#include <unistd.h>

#include <QCryptographicHash>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QPair>
//#include <QImage>
//#include <QBuffer>

QPair<QString, QString> cm_md5_map(const QString &file)
{
	QCryptographicHash crypto(QCryptographicHash::Md5);
	QFile f(file);
	f.open(QFile::ReadOnly);
	while (!f.atEnd()){
		crypto.addData(f.read(4*1024));  // 4k read
	}
//	qDebug() << __func__ << QString(crypto.result().toHex()) << "file:" << file;
	return qMakePair(file, QString(crypto.result().toHex()));
}
#if 0
QPair<QString, QString> cm_imageData_map(const QString &file)
{
	QCryptographicHash crypto();
	QImage img(file);
	if (img.isNull()) {
		// Not an image, return empty pair
		return qMakePair(QString(), QString());
	}

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	img.save(&buffer, "PNG"); // writes image into ba in PNG format

//	qDebug() << __func__ << QString(crypto.result().toHex()) << "file:" << file;
	return qMakePair(file, QString(QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex()));
}
#endif
#if 0
void cm_md5_reduce(QString &result, const QString &intermediate)
{
	result = "R" + intermediate;
	qDebug() << __func__ << "intermediate:" << intermediate
		 << "result:" << result;
}
#endif
