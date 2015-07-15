#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AboutDialog.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) : 
			QMainWindow(parent),
			ui(new Ui::MainWindow),
			currentFileName(QString())
{
	ui->setupUi(this);

	connect(ui->ncamWidget, SIGNAL(lensDataUpdated(double, double, double, double)), ui->lensWidget, SLOT(onLensDataChanged(double, double, double, double)));
	connect(ui->ncamWidget, SIGNAL(lensDataUpdated(double, double, double, double)), this, SLOT(updateSample(double, double, double, double)));

	// loop call update
	/*QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateSample()));
	timer->start(33);*/
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



void MainWindow::updateSample(double zoom, double focus, double iris, double fov)
{
	
	std::cout << zoom << ", " << focus << " : " << fov << std::endl;
	ui->lensImage->setTexelCoord(zoom, 1.0f - focus);

	QVector4D texel = ui->lensImage->getTexelColor();
	ui->zoomLineEdit->setText(QString::number(texel.x()));
	ui->focusLineEdit->setText(QString::number(texel.y()));
	ui->fovLineEdit->setText(QString::number(ui->lensWidget->tableLens.denormalizeFov(texel.z())));

	
}