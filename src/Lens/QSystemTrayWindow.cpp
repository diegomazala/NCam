

#include "QSystemTrayWindow.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QtGui>

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>
#include <iostream>

QSystemTrayWindow::QSystemTrayWindow() :icon(":/images/bad.png")
{
    createActions();
    createTrayIcon();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));


	trayIcon->setIcon(icon);
	setWindowIcon(icon);
	trayIcon->setToolTip("Tooltip");
    trayIcon->show();

    setWindowTitle(tr("Systray"));
    resize(400, 300);
}


void QSystemTrayWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}


void QSystemTrayWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) 
	{
        hide();
        event->ignore();
    }
}


void QSystemTrayWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
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



void QSystemTrayWindow::createActions()
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

void QSystemTrayWindow::createTrayIcon()
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

#endif
