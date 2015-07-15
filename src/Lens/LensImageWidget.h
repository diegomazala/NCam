#ifndef __LENS_IMAGE_WIDGET_H__
#define __LENS_IMAGE_WIDGET_H__

#include "GLImageWidget.h"
#include "Lens.h"
#include "TexelRender.h"

class LensImageWidget : public GLImageWidget
{
	Q_OBJECT

public:
	LensImageWidget(QWidget *parent = 0);
	~LensImageWidget();

	void setLensMatrix(const LensMatrix& lm);
	void setTexelCoord(float x, float y);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

public slots:
	void zoomChannel(int checked);
	void focusChannel(int checked);
	void fovChannel(int checked);
	//QVector4D getTexelColor(const QVector2D& coord);
	QVector4D getTexelColor();

private:

	QOpenGLShaderProgram* buildTextureChannelProgram() const;

	QPoint clickBegin;
	QPoint clickEnd;
	
	bool mousePressed;
	QPoint mousePos;
	QVector2D glPixelCoord;
	QVector4D glPixelColor;
	QVector4D channels;

	TexelRender texelRender;
};


#endif // __LENS_IMAGE_WIDGET_H__
