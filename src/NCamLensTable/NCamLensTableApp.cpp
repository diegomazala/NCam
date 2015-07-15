#include "MainWindow.h"
#include <QApplication>


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
	int argc = 0;
	QApplication a(argc, 0);

	MainWindow w;
	w.show();

	return a.exec();
}


