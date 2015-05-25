
#include "LensWidget.h"
#include "ui_LensWidget.h"
#include <QStandardItemModel>
#include <QTableWidgetItem>

LensWidget::LensWidget(QWidget *parent) : 
		QWidget(parent), 
		ui(new Ui::LensWidget),
		tableModel(nullptr)
{
	ui->setupUi(this);
}


LensWidget::~LensWidget()
{
}


bool LensWidget::load(QString filename)
{
	bool success = false;
	if (tableLens.load(filename.toStdString()))
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

		success = true;
	}
	
	return success;
}

