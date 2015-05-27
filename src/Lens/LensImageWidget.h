#ifndef __LENS_IMAGE_WIDGET_H__
#define __LENS_IMAGE_WIDGET_H__

#include "GLImageWidget.h"
#include "Lens.h"


class LensImageWidget : public GLImageWidget
{
	Q_OBJECT

public:
	LensImageWidget(QWidget *parent = 0);
	~LensImageWidget();

	void setLensMatrix(const LensMatrix& lm);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;


private:

	QOpenGLShaderProgram* buildImageRenderTexel2D();
	QOpenGLShaderProgram* buildTextureChannelProgram();

	QScopedPointer<QOpenGLShaderProgram> renderTexel;

	QPoint clickBegin;
	QPoint clickEnd;
	
	bool mousePressed;
	QPoint mousePos;
	QVector2D glPixelCoord;
	QVector4D glPixelColor;
	QVector4D channels;
};


#endif // __LENS_IMAGE_WIDGET_H__
