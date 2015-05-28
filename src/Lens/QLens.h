#ifndef _QLENS_H_
#define _QLENS_H_

#include "Lens.h"
#include <QImage>



template <typename T>
inline T Lerp(T const& x, T const& x0, T const& x1, T const& y0, T const& y1)
{
	if ((x1 - x0) == 0)
		return (y0 + y1) / 2;
	return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}
	

static void NormalizeLensMatrix(LensMatrix& matrix, float min_zoom, float max_zoom, float min_focus, float max_focus, float min_fov, float max_fov)
{
	int rowCount = matrix.size();
	int columnCount = matrix[0].size();
	for (int i = 0; i < rowCount; ++i)
	{
		for (int j = 0; j < columnCount; ++j)
		{
			matrix[i][j].zoom = Lerp((float)matrix[i][j].zoom, min_zoom, max_zoom, 0.0f, 1.0f);
			matrix[i][j].focus = Lerp((float)matrix[i][j].focus, min_focus, max_focus, 0.0f, 1.0f);
			matrix[i][j].iris = Lerp((float)matrix[i][j].iris, 0.0f, 1.0f, 0.0f, 1.0f);
			matrix[i][j].fov = Lerp((float)matrix[i][j].fov, min_fov, max_fov, 0.0f, 1.0f);
		}
	}
}

static std::pair<float, float> MinMaxFov(const LensMatrix& matrix)
{
	float minFov = FLT_MAX;
	float maxFov = FLT_MIN;

	for (auto& lz : matrix)
	{
		for (auto& ls : lz)
		{
			if (ls.fov < minFov)
				minFov = ls.fov;

			if (ls.fov > maxFov)
				maxFov = ls.fov;
		}
	}
	return std::make_pair(minFov, maxFov);
}

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


static void LensMatrix2QImage(const LensMatrix& lens_matrix, QImage& img)
{
	LensMatrix matrix = lens_matrix;
	int rows = matrix.size();
	int columns = matrix[0].size();

	auto min_max_fov = MinMaxFov(matrix);
	NormalizeLensMatrix(matrix, 0.0f, 1.0f, 0.0f, 1.0f, min_max_fov.first, min_max_fov.second);

	img = QImage(columns, rows, QImage::Format::Format_RGBA8888);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			const LensSample& ls = matrix[i][j];

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
