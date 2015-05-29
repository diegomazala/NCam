#include "LensServerMainWindow.h"
#include "ui_LensServerMainWindow.h"
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>
#include <QMessageBox>


LensServerMainWindow::LensServerMainWindow(QWidget *parent) :
			QMainWindow(parent),
			ui(new Ui::LensServerMainWindow),
			currentFileName(QString())
			//icon(":/images/bad.png")
{
	QApplication::setQuitOnLastWindowClosed(false);

	ui->setupUi(this);


	createSystemTrayActions();
	createTrayIcon();

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));


	trayIcon->setIcon(windowIcon());
	trayIcon->setToolTip("Lens Server");
	trayIcon->show();
}


LensServerMainWindow::~LensServerMainWindow()
{
	delete ui;
}




void LensServerMainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "../data", tr("Lens File (*.lens)"));

	if (!fileName.isEmpty())
	{
		currentFileName = fileName;
		if (tableLens.load(currentFileName.toStdString()))
		{
			QImage img;
			LensMatrix2QImage(tableLens.matrix, img);
			ui->lensImage->setImage(img);
		}
		else
		{
			QMessageBox::information(this, tr("Lens Server"),
				tr("Could not load lens file"));
		}
	}
}



void LensServerMainWindow::aboutDialogShow()
{
}


void LensServerMainWindow::setVisible(bool visible)
{
	minimizeAction->setEnabled(visible);
	maximizeAction->setEnabled(!isMaximized());
	restoreAction->setEnabled(isMaximized() || !visible);
	QMainWindow::setVisible(visible);
}

void LensServerMainWindow::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible())
	{
		hide();
		event->ignore();
	}
}


void LensServerMainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		;
	}
}



void LensServerMainWindow::createSystemTrayActions()
{
	minimizeAction = new QAction(tr("Mi&nimize"), this);
	connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

	maximizeAction = new QAction(tr("Ma&ximize"), this);
	connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

	restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void LensServerMainWindow::createTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(minimizeAction);
	trayIconMenu->addAction(maximizeAction);
	trayIconMenu->addAction(restoreAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
}
