#include <iostream>
#include <unistd.h>

#include <QStringList>
#include <QDebug>

#include "histogram.h"

static QMap<QString, QStringList> dupMap;


// Compare Method: "Color Histogtram"

// 1. x ogni file calcola l'istogramma
// 2. compara l'istogramma con tutti i  file
// 3. se il risultato >= threshold inserisci i file nella mappa dei duplicati

int cm_histogram(int threshold, const QStringList &fileList)
{
	qDebug() << __func__ << "TH:" << threshold << "files:" << fileList.size();

	for (int count = 0; count < 5; count++ ) {
		std::cout << "step:" << count << std::endl;
		sleep(1);
	}
	return 8;
}

#if 10
static void debugDupMap()
{
	qDebug() << "-------" << __PRETTY_FUNCTION__ << "-----------";
	if (dupMap.isEmpty()) {
		qDebug() << "No Duplicate found!";
	} else {
		// (Debug) Map
		QMap<QString, QStringList>::const_iterator i = dupMap.constBegin();
		 while (i != dupMap.constEnd()) {
			qDebug() << i.key() << ": " << i.value();
			++i;
		 }
	}
	qDebug() << "-------------------------";
}

// Adds an entry to the map of duplicates.
static void addDuplicate(const QString key, const QString value)
{
	QStringList sl;

	if (dupMap.contains(key)) {
		// <key> already in the map, get <value> list for updating.
		sl = dupMap[key];
	}

	sl << value;		// Update <value> file list
	dupMap[key] = sl;	// Add to map
}

// Remove entries without duplicates
static void removeNonDuplicate()
{
	QMap<QString, QStringList>::const_iterator i = dupMap.constBegin();
	 while (i != dupMap.constEnd()) {
		if (i.value().size() < 2) {
			//qDebug() << "Removing key:" << i.key() << ":" << i.value();
			dupMap.remove(i.key());
		}
		++i;
	 }
}

// Compare Method: "Color Histogtram"
void compareHistogram(const QStringList &fileList, int treshold)
{
	QMap<QString, ColorHistogram> histogramMap;

	for (int i = 0; i < fileList.size(); i++) {
		QImage img(fileList.at(i));
		if (img.isNull()) {
			continue;
		}

		ColorHistogram h;
		h.calcNormalized(&img);
		histogramMap[fileList.at(i)] = h; // Add to map
	}
	ColorHistogram h1;
	ColorHistogram h2;

	//qDebug() << __PRETTY_FUNCTION__ << "Treshold:" << treshold;
	QMap<QString, ColorHistogram>::const_iterator i = histogramMap.constBegin();
	QMap<QString, ColorHistogram>::const_iterator j = histogramMap.constBegin();

	// Iterate until the (last - 1) element on the map because we don't need to
	// compare the last element!
	while (i != histogramMap.constEnd() - 1) {
		j = i + 1;
		h1 = i.value();

		// Add file to Map -- This will be removed by removeNonDuplicate
		// if contains only one element.
		addDuplicate(i.key(), i.key());
		qDebug() << "Add:[" << i.key() << "]--" << i.key();
		while (j != histogramMap.constEnd()) {
			h2 = j.value();
			double lhn = h1.compareChiSquareNormalized(h2);
			if (treshold < ((1.0 - lhn)*100.0)) {
				qDebug() << "ADD: " << i.key()
					 << j.key()
					 << (1.0 - lhn)*100.0 << "%";

				// Add duplicate to map value
				addDuplicate(i.key(), j.key());
			}
			++j;
		}
		i++;
	}

	debugDupMap();

	// Remove entries without duplicates
	removeNonDuplicate();

	debugDupMap();

//	debugHistogramMap();
}

void debugHistogramMap(const QMap<QString, ColorHistogram> &map)
{
	qDebug() << "-------" << __PRETTY_FUNCTION__ << "-----------";

	if (map.isEmpty()) {
		qDebug() << "No histogram found!";
	} else {
		// (Debug) Map
		QMap<QString, ColorHistogram>::const_iterator i = map.constBegin();
		 while (i != map.constEnd()) {
			qDebug() << i.key(); // << ": " << i.value();
			++i;
		 }
	}
	qDebug() << "-------------------------";
}
#endif


