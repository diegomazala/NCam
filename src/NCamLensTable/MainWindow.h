#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_


#include <QMainWindow>

class AboutDialog;
class QStandardItemModel;

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

	void addColumn();

private:

	Ui::MainWindow *ui;
	QString currentFileName;
	AboutDialog* aboutDialog;

	QStandardItemModel* tableModel;
};

#endif // _MAIN_WINDOW_H_