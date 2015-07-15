#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_


#include <QMainWindow>
#include "QLens.h"

class AboutDialog;
class NewLensTableDialog;

namespace Ui 
{
	class MainWindow;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	
	explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	void fileNew();
	void fileOpen();
	void fileSave();
	void fileSaveAs();
	void aboutDialogShow();
	void onTableUpdate();
	void updateSample(double zoom, double focus, double iris, double fov);

private:

	Ui::MainWindow *ui;
	QString currentFileName;
	AboutDialog* aboutDialog;
	NewLensTableDialog* newTableDialog;
};

#endif // _MAIN_WINDOW_H_