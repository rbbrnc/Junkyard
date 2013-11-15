#include <histogram.h>

#include <QDebug>

ColorHistogram::ColorHistogram() :
	m_count(0)
{
}

unsigned int ColorHistogram::count() const
{
	return m_count;
}

unsigned int ColorHistogram::index(unsigned int i) const
{
	return m_histogram[i];
}

double ColorHistogram::indexN(unsigned int i) const
{
	return m_normalized_histogram[i];
}

/*
 * Calculate the Sum of differences of 64 bins
 *
 * NOTE: Compare works only for images of the same size
 */
unsigned int ColorHistogram::compareSumOfDiff(const ColorHistogram &h2) const
{
	unsigned int sdiff = 0;

	for (int i = 0 ; i < 64; i++) {
		if (m_histogram[i] >= h2.index(i)) {
			sdiff += m_histogram[i] - h2.index(i);
		} else {
			sdiff += h2.index(i) - m_histogram[i];
		}
	}

	return sdiff;
}

/*
 *                         [h1(i) - h2(i)]^2
 * d(h1, h2) = (1/2) * Sum ------------------
 *                      i    h1(i) + h2(i)
 *
 *
 */
double ColorHistogram::compareChiSquareNormalized(const ColorHistogram &h2) const
{
	double sdiff = 0.0;

	for (int i = 0 ; i < 64; i++) {
		if (m_normalized_histogram[i] > 0 || h2.indexN(i) > 0) {
			double si;
			si = m_normalized_histogram[i] - h2.indexN(i);
			//qDebug() << i << ") h1-h2 =" << m_normalized_histogram[i] << "-" << h2.indexN(i) << "=" << si;
			si *= si;
			//qDebug() << "(h1-h2)^2 =" << si;
			si /= m_normalized_histogram[i] + h2.indexN(i);
			//qDebug() << "[(h1-h2)^2]/(h1+h2) =" << si;
			sdiff += si;
			//qDebug() << "sdiff[" << i << "]=" << sdiff;
		} else {
			//qDebug() << "[" << i << "] h1:" << m_normalized_histogram[i] << "h2:" << h2.indexN(i);
		}
	}

	return sdiff/2.0;
}
/*
 *                         [h1(i) - h2(i)]^2
 * d(h1, h2) = (1/2) * Sum ------------------
 *                      i    h1(i) + h2(i)
 *
 *
 */
double ColorHistogram::compareChiSquare(const ColorHistogram &h2) const
{
	double sdiff = 0.0;

	for (int i = 0 ; i < 64; i++) {
		if (m_histogram[i] == h2.index(i)) {
			continue;
		}

		int N;
		double si;
		N = m_histogram[i] - h2.index(i);
		N *= N;
		si = (double) N;
		si /= m_histogram[i] + h2.index(i);
		sdiff += si;
	}

	return sdiff/2.0;
}

void ColorHistogram::calcNormalized(QImage *img)
{
	m_count = img->width()*img->height();

	for (int i = 0; i < 64; i++) {
		m_normalized_histogram[i] = 0.0;
	}

	unsigned int idx = 0;
	QRgb c;

	for (int y = 0 ; y < img->height(); y++) {
		for (int x = 0 ; x < img->width(); x++) {
			c = img->pixel(x, y);
			if (qRed(c) > 191)
				idx = 48;
			else if (qRed(c) > 127)
				idx = 32;
			else if (qRed(c) > 63)
				idx = 16;
			else
				idx = 0;

			if (qGreen(c) > 191)
				idx += 12;
			else if (qGreen(c) > 127)
				idx += 8;
			else if (qGreen(c) > 63)
				idx += 4;
			else
				idx += 0;

			if (qBlue(c) > 191)
				idx += 3;
			else if (qBlue(c) > 127)
				idx += 2;
			else if (qBlue(c) > 63)
				idx += 1;
			else
				idx += 0;

			m_normalized_histogram[idx] = m_normalized_histogram[idx] + 1.0;
		}
	}

	double sum = 0;

	for (int i = 0; i < 64; i++) {
		m_normalized_histogram[i] = m_normalized_histogram[i]/m_count;
		//qDebug() << m_normalized_histogram[i];
		sum += m_normalized_histogram[i];
	}
	//qDebug() << __func__ << "sum:" << sum;
}

void ColorHistogram::calc(QImage *img)
{
	m_count = img->width()*img->height();

	for (int i = 0; i < 64; i++) {
		m_histogram[i] = 0;
	}

	unsigned int idx = 0;
	QRgb c;

	for (int y = 0 ; y < img->height(); y++) {
		for (int x = 0 ; x < img->width(); x++) {
			c = img->pixel(x, y);
			if (qRed(c) > 191)
				idx = 48;
			else if (qRed(c) > 127)
				idx = 32;
			else if (qRed(c) > 63)
				idx = 16;
			else
				idx = 0;

			if (qGreen(c) > 191)
				idx += 12;
			else if (qGreen(c) > 127)
				idx += 8;
			else if (qGreen(c) > 63)
				idx += 4;
			else
				idx += 0;

			if (qBlue(c) > 191)
				idx += 3;
			else if (qBlue(c) > 127)
				idx += 2;
			else if (qBlue(c) > 63)
				idx += 1;
			else
				idx += 0;

			m_histogram[idx] = m_histogram[idx] + 1;
		}
	}

	unsigned int sum = 0;

	for (int i = 0; i < 64; i++) {
		//qDebug() << m_histogram[i];
		sum += m_histogram[i];
	}
	//qDebug() << __func__ << "sum:" << sum << "=" << m_count;
}

