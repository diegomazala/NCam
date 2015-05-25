
#include "LensImageWidget.h"
#include <QMouseEvent>
#include <QKeyEvent>






LensImageWidget::LensImageWidget(QWidget *parent)
	: GLImageWidget(parent)
{
	// loop call update
	// QTimer *timer = new QTimer(this);
	// connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	// timer->start(33);
}


LensImageWidget::~LensImageWidget()
{
	cleanup();
}


void LensImageWidget::initializeGL()
{
	GLImageWidget::initializeGL();
	
	renderTexel.reset(buildImageRenderTexel2D());
	renderTexel->release();

	QString textureFile("G:/VgpCode/NCam/testlens.png");
	image = QImage(textureFile);
	texture.reset(new QOpenGLTexture(image.mirrored()));
	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Nearest);
	texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
}




void LensImageWidget::paintGL()
{
	QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	if (!texture.isNull())
		texture->bind();

	renderTexel->bind();
	renderTexel->setUniformValue("uv", glPixelCoord);
	f->glDrawArrays(GL_QUADS, 0, 4);
	glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, &glPixelColor[0]);
	qDebug() << glPixelColor.x() << ',' << glPixelColor.y() << ',' << glPixelColor.z() << ',' << glPixelColor.w();
	renderTexel->release();

	program->bind();
	f->glDrawArrays(GL_QUADS, 0, 4);
	program->release();

	f->glViewport(0, 0, 30, 30);
	renderTexel->bind();
	f->glDrawArrays(GL_QUADS, 0, 4);
	renderTexel->release();

}




void LensImageWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::MiddleButton)
		clickBegin = event->pos();

	mousePressed = true;
}


void LensImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	mousePressed = false;
	this->update();

	if (event->button() == Qt::MouseButton::MiddleButton)
	{
		clickEnd = event->pos();
		texture->setMagnificationFilter(QOpenGLTexture::Linear);
	}
	
	if (event->button() == Qt::MouseButton::RightButton)
	{
		texture->setMagnificationFilter(QOpenGLTexture::Nearest);
	}
	

	this->update();
}


void LensImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	mousePos = event->pos();

	glPixelCoord.setX(float(mousePos.x()) / float(width()));
	glPixelCoord.setY(1.0f - float(mousePos.y()) / float(height()));
	
	this->update();
}


void LensImageWidget::keyPressEvent(QKeyEvent *event) 
{
	//QMessageBox* box = new QMessageBox();
	//box->setWindowTitle(QString("Hello"));
	//box->setText(QString("You Pressed: ") + event->text());
	//box->show();

}


void LensImageWidget::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Q:
		close();
		break;
	case Qt::Key_1:
		break;
	case Qt::Key_2:
		break;
	default:
		QWidget::keyReleaseEvent(event);
	}
}




QOpenGLShaderProgram* LensImageWidget::buildImageRenderTexel2D()
{
	QOpenGLShaderProgram* prog = new QOpenGLShaderProgram();

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, prog);
	const char *vsrc =
		"attribute highp vec4 in_Vertex;\n"
		"attribute mediump vec4 in_TexCoord;\n"
		"varying mediump vec4 texc;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = in_Vertex;\n"
		"    texc = in_TexCoord;\n"
		"}\n";
	vshader->compileSourceCode(vsrc);

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, prog);
	const char *fsrc =
		"uniform sampler2D in_Texture;\n"
		"varying mediump vec4 texc;\n"
		"uniform vec2 uv;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = texture(in_Texture, uv);\n"
		"}\n";
	fshader->compileSourceCode(fsrc);

	prog->addShader(vshader);
	prog->addShader(fshader);
	prog->bindAttributeLocation("in_Vertex", 0);
	prog->bindAttributeLocation("in_TexCoord", 1);
	prog->link();
	prog->bind();
	prog->setUniformValue("in_Texture", 0);
	prog->release();

	return prog;
}

