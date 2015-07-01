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

	LensTable			tableLens;

public slots:
	bool load(QString filename);
	bool save(QString filename);
	void reset();
	void cellChanged(int i, int );
	void cellClicked(int i, int);
	void horizontalHeaderClicked(int column);
	void verticalHeaderClicked(int row);
	void zoomDeviationChanged(double value);
	void focusDeviationChanged(double value);
	void update();
	void onLensDataChanged(double zoom, double focus, double iris, double fov);
	void onEncodeFeedingToggled(bool toggle);
	
protected:
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

signals:
	void tableUpdated();

private:

	Ui::LensWidget *ui;

	LensSample encodeSample;
	int columnSelected;
};


#endif // __LENS_WIDGET_H__
