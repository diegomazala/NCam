
#include "LensWidget.h"
#include "ui_LensWidget.h"
#include <QStandardItemModel>
#include <QTableWidgetItem>
#include <QKeyEvent>
#include "NCam/NCam.h"

LensWidget::LensWidget(QWidget *parent) : 
		QWidget(parent), 
		ui(new Ui::LensWidget),
		columnSelected(-1)
{
	ui->setupUi(this);

	tableLens.zoomDeviation = (float)ui->zoomSpinBox->value();
	tableLens.focusDeviation = (float)ui->focusSpinBox->value();

	tableLens.createKeys(5, 5);
	tableLens.createMatrix(5, 5);

	connect(ui->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(horizontalHeaderClicked(int)));
	connect(ui->tableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(verticalHeaderClicked(int)));

	update();
}

LensWidget::~LensWidget()
{
}


bool LensWidget::load(QString filename)
{
	bool success = false;
	if (tableLens.load(filename.toStdString()))
	{
		update();
		success = true;
	}
	
	return success;
}


void LensWidget::reset()
{
	int rows = 5;
	int columns = 5;
	tableLens.createKeys(rows, columns);
	tableLens.createMatrix(rows, columns);
	emit tableUpdated();
}


void LensWidget::horizontalHeaderClicked(int column)
{
	int i, j;
	float z_dist, f_dist;
	float z = 0.25;
	float f = 0.0f;
	tableLens.find(tableLens.zoomDeviation, tableLens.focusDeviation, z_dist, f_dist, i, j);
}


void LensWidget::verticalHeaderClicked(int row)
{
}


void LensWidget::zoomDeviationChanged(double value)
{
	tableLens.zoomDeviation = value;
}

void LensWidget::focusDeviationChanged(double value)
{
	tableLens.focusDeviation = value;
}


void LensWidget::update()
{
	// setting intial table
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(tableLens.rowCount());
	ui->tableWidget->setColumnCount(tableLens.columnCount());

	// setting vertical labels
	QStringList labels;
	for (auto z : tableLens.zoomKeys)
		labels.push_back(QString::number(z));
	ui->tableWidget->setVerticalHeaderLabels(labels);

	// setting horizontal labels
	labels.clear();
	for (auto f : tableLens.focusKeys)
		labels.push_back(QString::number(f));
	ui->tableWidget->setHorizontalHeaderLabels(labels);

	// setting table items
	for (int i = 0; i < tableLens.rowCount(); ++i)
	{
		for (int j = 0; j < tableLens.columnCount(); ++j)
		{
			QTableWidgetItem* titem = new QTableWidgetItem(QString::number(tableLens.matrix[i][j].fov));
			ui->tableWidget->setItem(i, j, titem);
		}
	}
}


void LensWidget::cellChanged(int i, int j)
{
	bool ok = false;
	float old_item = tableLens.matrix[i][j].fov;
	float new_item = ui->tableWidget->item(i, j)->text().toFloat(&ok);
	if (ok)
		tableLens.matrix[i][j].fov = new_item;
	else
		tableLens.matrix[i][j].fov = old_item;

	emit tableUpdated();
}


void LensWidget::cellClicked(int i, int j)
{
	Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

	if (keyMod.testFlag(Qt::ControlModifier))
	{
		tableLens.matrix[i][j].reset();
		emit tableUpdated();
	}
}


void LensWidget::onLensDataChanged(double zoom, double focus, double iris, double fov)
{
	if (!ui->encodeFeedingGroupBox->isChecked())
		return;

	encodeSample.zoom = zoom;
	encodeSample.focus = focus;
	encodeSample.iris = iris;
	encodeSample.fov = fov;
	
	ui->zoomEncoderLineEdit->setText(QString::number(zoom));
	ui->focusEncoderLineEdit->setText(QString::number(focus));
	ui->fovEncoderLineEdit->setText(QString::number(fov));

	int i, j;
	float encode_z_dist, encode_f_dist;

	tableLens.find(encodeSample.zoom, encodeSample.focus, encode_z_dist, encode_f_dist, i, j);
	LensSample& sample = tableLens.matrix[i][j];

	float sample_z_dist = std::fabs(sample.zoom - tableLens.zoomKeys[i]);
	float sample_f_dist = std::fabs(sample.focus - tableLens.focusKeys[j]);

	float enc_dist = encode_z_dist + encode_f_dist;
	float table_dist = tableLens.zoomDeviation + tableLens.focusDeviation;
	float sample_dist = sample_z_dist + sample_f_dist;

	if (enc_dist < table_dist && enc_dist < sample_dist)
	{
		//update table lens 
		sample = encodeSample;
		

		// updating distortion sample
		sample.distortion.width = NCamDistortMapWidth();
		sample.distortion.height = NCamDistortMapHeight();
		sample.distortion.channelCount = NCamDistortMapChannelsCount();
		float* imgptr = (float*)NCamDistortMapDataPtr();
		int size = sample.distortion.width * sample.distortion.height * sample.distortion.channelCount;	
		sample.distortion.data.resize(size);
		for (int i = 0; i < size; ++i)
			sample.distortion.data[i] = imgptr[i];

		//update ui table
		ui->tableWidget->item(i, j)->setText(QString::number(sample.fov));

		//emit signal
		emit tableUpdated();
	}
}




void LensWidget::onEncodeFeedingToggled(bool toggle)
{
	if (toggle)  // turn on
	{
		
	}
	else	// turn off
	{

	}
}


void LensWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		ui->tableWidget->clearSelection();
		columnSelected = -1;
	}
}