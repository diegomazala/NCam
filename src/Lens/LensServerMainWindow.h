#ifndef _LENS_SERVER_MAIN_WINDOW_H_
#define _LENS_SERVER_MAIN_WINDOW_H_


#include <QMainWindow>
#include <QSystemTrayIcon>
#include "QLens.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

namespace Ui 
{
	class LensServerMainWindow;
}


class LensServerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	
	explicit LensServerMainWindow(QWidget *parent = 0);
	~LensServerMainWindow();

	void setVisible(bool visible) Q_DECL_OVERRIDE;

public slots:
	void fileOpen();
	void aboutDialogShow();


protected:
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);


private:

	void createSystemTrayActions();
	void createTrayIcon();

	

	Ui::LensServerMainWindow *ui;
	QString currentFileName;

	LensTable tableLens;

	// system tray attributes
	QAction*			minimizeAction;
	QAction*			maximizeAction;
	QAction*			restoreAction;
	QAction*			quitAction;
	QSystemTrayIcon*	trayIcon;
	QMenu*				trayIconMenu;
};

#endif // _LENS_SERVER_MAIN_WINDOW_H_
