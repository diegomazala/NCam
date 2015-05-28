

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


bool NCamWidget::isConnected() const
{
	return NCamIsOpen();
}


void NCamWidget::onConnect()
{
	NCamSetPacketType(true, true, true);

	if (!NCamOpen(ui->ipLineEdit->text().toStdString().c_str(), ui->portLineEdit->text().toInt()))
	{
		qDebug() << "<ERROR> Could not connect";
	}
}


void NCamWidget::onDisconnect()
{
	NCamClose();
}



void NCamWidget::update()
{
	if (!isConnected())
		return;

	NCamUpdate();

	int	opticalTimeCode[5];
	NCamOpticalTimeCode(opticalTimeCode);
	
	int	trackingTimeCode[5];
	NCamTrackingTimeCode(trackingTimeCode);

	double encoder[6];
	NCamEncoderParameters(encoder);

	double optical[13];
	NCamOpticalParameters(optical);

	double tracking[12];
	NCamTrackingParameters(tracking);

	ui->timeCodeHourLineEdit->setText(QString::number(opticalTimeCode[0]));
	ui->timeCodeMinLineEdit->setText(QString::number(opticalTimeCode[1]));
	ui->timeCodeSecLineEdit->setText(QString::number(opticalTimeCode[2]));
	ui->timeCodeFrameLineEdit->setText(QString::number(opticalTimeCode[3]));

	ui->zoomNormalizedLineEdit->setText(QString::number(encoder[0]));
	ui->zoomMappedLineEdit->setText(QString::number(encoder[1]));
	ui->focusNormalizedLineEdit->setText(QString::number(encoder[2]));
	ui->focusMappedLineEdit->setText(QString::number(encoder[3]));
	ui->irisNormalizedLineEdit->setText(QString::number(encoder[4]));
	ui->irisMappedLineEdit->setText(QString::number(encoder[5]));

	ui->fovYLineEdit->setText(QString::number(optical[1]));
	ui->fovXLineEdit->setText(QString::number(optical[0]));
	ui->aspectLineEdit->setText(QString::number(optical[8]));
	ui->imageWidthLineEdit->setText(QString::number(optical[6]));
	ui->imageHeightLineEdit->setText(QString::number(optical[7]));
	ui->sensorWidthLineEdit->setText(QString::number(optical[4]));
	ui->sensorHeightLineEdit->setText(QString::number(optical[5]));
	ui->offsetXLineEdit->setText(QString::number(optical[2]));
	ui->offsetYLineEdit->setText(QString::number(optical[3]));

	ui->rot0LineEdit->setText(QString::number(tracking[0]));
	ui->rot1LineEdit->setText(QString::number(tracking[1]));
	ui->rot2LineEdit->setText(QString::number(tracking[2]));
	ui->rot3LineEdit->setText(QString::number(tracking[3]));
	ui->rot4LineEdit->setText(QString::number(tracking[4]));
	ui->rot5LineEdit->setText(QString::number(tracking[5]));
	ui->rot6LineEdit->setText(QString::number(tracking[6]));
	ui->rot7LineEdit->setText(QString::number(tracking[7]));
	ui->rot8LineEdit->setText(QString::number(tracking[8]));

	ui->trans0LineEdit->setText(QString::number(tracking[9]));
	ui->trans1LineEdit->setText(QString::number(tracking[10]));
	ui->trans2LineEdit->setText(QString::number(tracking[11]));

	emit lensDataUpdated(encoder[0], encoder[2], encoder[4], optical[1]);
	emit lensUpdated();
}



