#ifndef __GL_IMAGE_WIDGET_H__
#define __GL_IMAGE_WIDGET_H__

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QScopedPointer>
#include <QOpenGLShaderProgram>
#include <QImage>



class GLImageWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	GLImageWidget(QWidget *parent = 0);
	~GLImageWidget();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE{ return QSize(50, 50); };
	QSize sizeHint() const Q_DECL_OVERRIDE{ return QSize(1280, 720); };

public slots:
	virtual void cleanup();
	void setImage(const QImage& image);
	void setImage(const QString& fileimage);
	
	

protected:	// methods

	virtual void initializeGL() Q_DECL_OVERRIDE;
	virtual void paintGL() Q_DECL_OVERRIDE;
	virtual void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	virtual void buildTexture();

private: // methods
	QOpenGLShaderProgram* buildImagePassthroughProgram() const;
	QOpenGLBuffer* buildQuadTextured() const;
	

protected : // attributes
	QOpenGLVertexArrayObject vao;
	QScopedPointer<QOpenGLBuffer> vbo;
	
	QSharedPointer<QOpenGLTexture> texture;
	QScopedPointer<QOpenGLShaderProgram> program;
	
	QImage image;

};


#endif // __GL_IMAGE_WIDGET_H__
