#include <QCryptographicHash>
#include <QDebug>
#include <QPair>
#include <QImage>
#include <QBuffer>

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

	return qMakePair(file, QString(QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex()));
}
