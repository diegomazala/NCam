

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

	tableModel = new QStandardItemModel(this);

	tableModel->setHorizontalHeaderItem(0, new QStandardItem(QString("0.00")));
	tableModel->setHorizontalHeaderItem(1, new QStandardItem(QString("0.25")));
	tableModel->setHorizontalHeaderItem(2, new QStandardItem(QString("0.50")));
	tableModel->setHorizontalHeaderItem(3, new QStandardItem(QString("0.75")));
	tableModel->setHorizontalHeaderItem(4, new QStandardItem(QString("1.00")));

	int currentRow = 0;
	QStandardItem *row = new QStandardItem(QString::number(0.5));
	tableModel->setItem(currentRow, 0, row);
	row = new QStandardItem("0.6");
	tableModel->setItem(currentRow, 1, row);
	row = new QStandardItem("0.7");
	tableModel->setItem(currentRow, 2, row);
	row = new QStandardItem("0.8");
	tableModel->setItem(currentRow, 3, row);
	row = new QStandardItem("0.9");
	tableModel->setItem(currentRow, 4, row);
	currentRow++;


	int currentCol = 0;
	row = new QStandardItem(QString::number(0.5));
	tableModel->setItem(currentRow, currentCol++, row);
	row = new QStandardItem("0.66");
	tableModel->setItem(currentRow, currentCol++, row);
	row = new QStandardItem("0.77");
	tableModel->setItem(currentRow, currentCol++, row);
	row = new QStandardItem("0.88");
	tableModel->setItem(currentRow, currentCol++, row);
	row = new QStandardItem("0.99");
	tableModel->setItem(currentRow, currentCol++, row);
	currentRow++;

	ui->tableView->setModel(tableModel);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::fileNew()
{

}

void MainWindow::fileOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "../../../data", tr("Lens Table (*.nlt)"));

	if (!fileName.isEmpty())
	{
		currentFileName = fileName;
	}
}


void MainWindow::fileSave()
{
	if (!currentFileName.isEmpty())
	{
	}
	else
	{
		fileSaveAs();
	}
}


void MainWindow::fileSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Images (*.png *.bmp *.jpg *.tiff)"));
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



void MainWindow::addColumn()
{
	if (!ui->ncamWidget->isConnected())
	{
		QMessageBox::critical(this, tr("NCam Lens Table"),
			QString("There is no connection to a NCam server."),
			QMessageBox::Abort);
		return;
	}

	//ui->tableView->
}
