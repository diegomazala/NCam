#include "MainWindow.h"
#include <QApplication>


#if CONSOLE_APP
int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
	int argc = 0;
	QApplication a(argc, 0);
#endif
	MainWindow w;
	w.show();

	return a.exec();
}


