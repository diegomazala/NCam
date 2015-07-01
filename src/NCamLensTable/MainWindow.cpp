#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AboutDialog.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>


MainWindow::MainWindow(QWidget *parent) : 
			QMainWindow(parent),
			ui(new Ui::MainWindow),
			currentFileName(QString())
{
	ui->setupUi(this);

	connect(ui->ncamWidget, SIGNAL(lensDataUpdated(double, double, double, double)), ui->lensWidget, SLOT(onLensDataChanged(double, double, double, double)));
}


MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::fileNew()
{
	currentFileName.clear();
	ui->lensWidget->reset();
}


void MainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "../data", tr("Lens File (*.lens)"));

	if (!fileName.isEmpty())
	{
		currentFileName = fileName;
		ui->lensWidget->load(currentFileName);
		QImage img;
		LensMatrix2QImage(ui->lensWidget->tableLens.matrix, img);
		ui->lensImage->setImage(img);
	}
}


void MainWindow::fileSave()
{
	if (!currentFileName.isEmpty())
	{
		ui->lensWidget->save(currentFileName);
	}
	else
	{
		fileSaveAs();
	}
}


void MainWindow::fileSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Lens File (*.lens)"));
	if (!fileName.isEmpty())
	{
		currentFileName = fileName;
		fileSave();
	}
}


void MainWindow::aboutDialogShow()
{
	aboutDialog = new AboutDialog(this);
	aboutDialog->show();
}


void MainWindow::onTableUpdate()
{
	ui->lensImage->setLensMatrix(ui->lensWidget->tableLens.matrix);
}