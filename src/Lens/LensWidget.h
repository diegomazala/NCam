#ifndef __LENS_WIDGET_H__
#define __LENS_WIDGET_H__

#include <QWidget>
#include "Lens.h"

class QStandardItemModel;

namespace Ui
{
	class LensWidget;
}


class LensWidget : public QWidget
{
	Q_OBJECT

public:
	LensWidget(QWidget *parent = 0);
	~LensWidget();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE{ return QSize(50, 50); };
	QSize sizeHint() const Q_DECL_OVERRIDE{ return QSize(1280, 400); };

public slots:
	bool load(QString filename);

private:

	Ui::LensWidget *ui;

	QStandardItemModel* tableModel;
	LensTable			tableLens;
};


#endif // __LENS_WIDGET_H__
