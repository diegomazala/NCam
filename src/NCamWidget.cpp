

#include "NCamWidget.h"
#include "ui_NCamWidget.h"
#include "NCam.h"
#include <QDebug>
#include <QTimer>

NCamWidget::NCamWidget(QWidget *parent) : 
			QWidget(parent),
			ui(new Ui::NCamWidget)
{
	ui->setupUi(this);

	// set port validator
	ui->portLineEdit->setValidator(new QIntValidator(0, 65535, this));

	// set ip validator
	QRegExpValidator *v = new QRegExpValidator(this);
	QRegExp rx("((1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})\\.){3,3}(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})");
	v->setRegExp(rx);
	ui->ipLineEdit->setValidator(v);


	// loop call update
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(33);


	NCamCreate();
}



NCamWidget::~NCamWidget()
{
	delete ui;

	NCamDestroy();
}


void NCamWidget::onConnect()
{
	NCamSetPacketType(true, true, true);

	if (!NCamOpen(ui->ipLineEdit->text().toStdString().c_str(), ui->portLineEdit->text().toInt()))
	{
		qDebug() << "<ERROR> Could not connect";
	}
	else
	{
		qDebug() << "============= NCamOpen ==============";
	}
}


void NCamWidget::onDisconnect()
{
	NCamClose();
	qDebug() << "============= NCamClose =============";
}



void NCamWidget::update()
{
	if (!NCamIsOpen())
		return;

	NCamUpdate();

	int	opticalTimeCode[5];
	int	trackingTimeCode[5];
	double encoder[6];
	NCamOpticalTimeCode(opticalTimeCode);
	NCamTrackingTimeCode(trackingTimeCode);
	NCamEncoderParameters(encoder);

	ui->opticalTimeCodeHourLabel->setText(QString::number(opticalTimeCode[0]));
	ui->opticalTimeCodeMinLabel->setText(QString::number(opticalTimeCode[1]));
	ui->opticalTimeCodeSecLabel->setText(QString::number(opticalTimeCode[2]));
	ui->opticalTimeCodeFrameLabel->setText(QString::number(opticalTimeCode[3]));

	ui->trackingTimeCodeHourLabel->setText(QString::number(trackingTimeCode[0]));
	ui->trackingTimeCodeMinLabel->setText(QString::number(trackingTimeCode[1]));
	ui->trackingTimeCodeSecLabel->setText(QString::number(trackingTimeCode[2]));
	ui->trackingTimeCodeFrameLabel->setText(QString::number(trackingTimeCode[3]));

	ui->zoomNormalizedLabel->setText(QString::number(encoder[0]));
	ui->zoomMappedLabel->setText(QString::number(encoder[1]));
	ui->focusNormalizedLabel->setText(QString::number(encoder[2]));
	ui->focusMappedLabel->setText(QString::number(encoder[3]));
	ui->irisNormalizedLabel->setText(QString::number(encoder[4]));
	ui->irisMappedLabel->setText(QString::number(encoder[5]));

}



