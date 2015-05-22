#include <QApplication>
#include "LensWidget.h"
#include <QTimer>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	LensWidget w;
	w.show();

	w.load("../data/lenstable3x3.lens");
	

	QTimer *timer = new QTimer();
	timer->start(5000);
	QApplication::connect(timer, SIGNAL(timeout()), &a, SLOT(quit()));

	return a.exec();
}
