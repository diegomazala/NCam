#ifndef _NEW_LENS_TABLE_DIALOG_H_
#define _NEW_LENS_TABLE_DIALOG_H_


#include <QDialog>

namespace Ui
{
	class NewLensTableDialog;
}


class NewLensTableDialog : public QDialog
{
	Q_OBJECT

public:
	
	explicit NewLensTableDialog(QWidget *parent = 0);
	~NewLensTableDialog();

	int zoomSamples() const;
	int focusSamples() const;

private:

	Ui::NewLensTableDialog *ui;

};

#endif // _NEW_LENS_TABLE_DIALOG_H_