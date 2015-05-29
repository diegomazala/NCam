
#ifndef _TEXEL_RENDER_H_
#define _TEXEL_RENDER_H_

//#include <QObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>


class TexelRender : public QObject, protected QOpenGLFunctions
{
	Q_OBJECT

public:

	TexelRender(QObject *parent = 0);
	void initGL();
	void render();

	QVector4D color() const { return glPixelColor; };
	QVector2D coord() const { return glPixelCoord; };
	void setCoord(const QVector2D& coord) { glPixelCoord = coord; }

private:

	QOpenGLShaderProgram* buildImageRenderTexel2D() const;
	QOpenGLBuffer* buildQuadTextured() const;

	QOpenGLVertexArrayObject vao;
	QScopedPointer<QOpenGLBuffer> vbo;
	QScopedPointer<QOpenGLShaderProgram> program;

	QVector4D glPixelColor;
	QVector2D glPixelCoord;
	bool initialized;
};


#endif //FRAGMENT_TOY_H
