#include <QApplication>
#include "LensImageWidget.h"
//#include "GLImageWidget.h"
#include "QLens.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	LensImageWidget w;
	//GLImageWidget w;
	w.show();

	//w.setImage("C:/temp/fbo.png");
	//w.load("../data/lenstable3x3.lens");
	
	QImage img;
	LensTable lt;
#if 0
	lt.load("../data/lenstable3x3.lens");
	lt.minFov = 0.0f;
	lt.maxFov = 1.0f;
#else
	lt.load("../data/lenstable5x5.lens");
	lt.minFov = 5.0f;
	lt.maxFov = 50.0f;
#endif
	

	lt.minZoom = lt.minFocus = lt.minIris = 0.0f;
	lt.maxZoom = lt.maxFocus = lt.maxIris = 1.0f;
		

	for (int i = 0; i < lt.rowCount(); ++i)
	{
		for (int j = 0; j < lt.columnCount(); ++j)
		{
			std::cout << lt.matrix[i][j] << std::endl;;
		}
	}

	lt.normalizeMatrix();

	std::cout << std::endl << std::endl;
	for (int i = 0; i < lt.rowCount(); ++i)
	{
		for (int j = 0; j < lt.columnCount(); ++j)
		{
			std::cout << lt.matrix[i][j] << std::endl;;
		}
	}

	LensMatrix2QImage(lt.matrix, img);
	w.setImage(img);

	
	
	return a.exec();
}
