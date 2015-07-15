

#include "NewLensTableDialog.h"
#include "ui_NewLensTableDialog.h"


NewLensTableDialog::NewLensTableDialog(QWidget *parent) :
			QDialog(parent),
			ui(new Ui::NewLensTableDialog)
{
	ui->setupUi(this);
}

NewLensTableDialog::~NewLensTableDialog()
{
	
}

int NewLensTableDialog::zoomSamples() const
{
	return ui->zoomSpinBox->value();
}


int NewLensTableDialog::focusSamples() const
{
	return ui->focusSpinBox->value();
}

