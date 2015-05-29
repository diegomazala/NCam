#include <QApplication>
#include "LensImageWidget.h"
#include "QLens.h"
#include "QSystemTrayWindow.h"
#include "LensServerMainWindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

#if 0
	LensImageWidget w;
	w.show();


	LensTable lt;
	lt.load("../data/lenstable5x5_dist.lens");
	QImage img;
	LensMatrix2QImage(lt.matrix, img);
	img.save("../data/lenstable5x5_dist.png", "png", 100);
	//img.save("../data/lenstable5x5_dist.png");
	w.setImage(img);
#else

	
	
	//QSystemTrayWindow window;
	LensServerMainWindow window;
	window.show();
#endif
	
	return a.exec();
}
