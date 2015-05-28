#ifndef _NCAM_WIDGET_H_
#define _NCAM_WIDGET_H_


#include <QtWidgets/QWidget>

namespace Ui
{
	class NCamWidget;
}


class NCamWidget : public QWidget
{
	Q_OBJECT

public:
	explicit NCamWidget(QWidget *parent = 0);
	~NCamWidget();

	bool isConnected() const;

public slots:
	void onConnect();
	void onDisconnect();
	void update();

signals:
	void lensDataUpdated(double zoom, double focus, double iris, double fov);
	void lensUpdated();

private:

	Ui::NCamWidget *ui;
};

#endif // _NCAM_WIDGET_H_