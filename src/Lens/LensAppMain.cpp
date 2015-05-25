#include <QApplication>
#include "LensWidget.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	LensWidget w;
	w.show();

	w.load("../data/lenstable3x3.lens");
	
	return a.exec();
}
