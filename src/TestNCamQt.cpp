#include "NCamWidget.h"
#include <QApplication>


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	NCamWidget w;
	w.show();

	return a.exec();
}

