#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QImage>

class ColorHistogram
{
	public:
		ColorHistogram();
		void calc(QImage *img);
		void calcNormalized(QImage *img);

		unsigned int compareSumOfDiff(const ColorHistogram &h2) const;
		double compareChiSquare(const ColorHistogram &h2) const;
		double compareChiSquareNormalized(const ColorHistogram &h2) const;
		unsigned int index(unsigned int i) const;
		double indexN(unsigned int i) const;
		unsigned int count() const;

	private:
		unsigned int m_histogram[64];
		double m_normalized_histogram[64];
		unsigned int m_count; // num. pixels
		QSize m_img_size;
};

#endif
