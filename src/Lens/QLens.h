#ifndef _QLENS_H_
#define _QLENS_H_

#include "Lens.h"
#include <QImage>



static void QImage2LensMatrix(const QImage& img, LensMatrix& lt)
{
	lt.clear();
	lt.resize(img.height());
	for (auto& l : lt)
		l.resize(img.width());

	for (int x = 0; x < img.width(); ++x)
	{
		for (int y = 0; y < img.height(); ++y)
		{
			QRgb rgba = img.pixel(x, y);
			lt[y][x].zoom = (double)qRed(rgba) / 255.0;
			lt[y][x].focus = (double)qGreen(rgba) / 255.0;
#if 0
			lt[y][x].iris = (double)qBlue(rgba) / 255.0;
			lt[y][x].fov = (double)qAlpha(rgba) / 255.0;
#else
			lt[y][x].fov = (double)qBlue(rgba) / 255.0;
			lt[y][x].iris = (double)qAlpha(rgba) / 255.0;
#endif
		}
	}
}



static void LensMatrix2QImage(const LensMatrix& lt, QImage& img)
{
	int rows = lt.size();
	int columns = lt[0].size();

	img = QImage(columns, rows, QImage::Format::Format_RGBA8888);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			const LensSample& ls = lt[i][j];

			int red = ls.zoom * 255;
			int green = ls.focus * 255;
#if 0
			int blue = ls.iris * 255;
			int alpha = ls.fov * 255;
#else
			int alpha = ls.iris * 255;
			int blue = ls.fov * 255;
#endif

			img.setPixel(j, i, qRgba(red, green, blue, alpha));
		}
	}
}



#endif // _QLENS_H_
