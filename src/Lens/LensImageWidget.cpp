
#include "LensImageWidget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include "QLens.h"


LensImageWidget::LensImageWidget(QWidget *parent)
	: GLImageWidget(parent), 
	channels(0, 0, 1, 0)
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

void LensImageWidget::setLensMatrix(const LensMatrix& lm)
{
	LensMatrix2QImage(lm, image);
	buildTexture();
	update();
}

void LensImageWidget::initializeGL()
{
	GLImageWidget::initializeGL();

	program.reset(buildTextureChannelProgram());
	program->release();
	
	texture.reset(new QOpenGLTexture(image.mirrored()));
	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Nearest);
	texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);

	texelRender.initGL();
}




void LensImageWidget::paintGL()
{
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	if (!texture.isNull())
		texture->bind();

	// render for interpolation of texel
	texelRender.setCoord(glPixelCoord);
	texelRender.render();
	glPixelColor = texelRender.color();

	// render texture on screen
	vao.bind();
	program->bind();
	program->setUniformValue("channels", channels);
	f->glDrawArrays(GL_QUADS, 0, 4);
	program->release();
	vao.release();

	// render mini window
	f->glViewport(0, 0, 20, 20);
	//f->glViewport(mousePos.x() - 10, height()- mousePos.y() - 10, 20, 20);
	texelRender.render();
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



void LensImageWidget::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Q:
		close();
		break;
	case Qt::Key_R:
		channels.setX( float( !bool(channels.x()) ) );
		update();
		break;
	case Qt::Key_G:
		channels.setY(float(!bool(channels.y())));
		update();
		break;
	case Qt::Key_B:
		channels.setZ(float(!bool(channels.z())));
		update();
		break;
	case Qt::Key_A:
		channels.setW(float(!bool(channels.w())));
		update();
		break;
	default:
		QWidget::keyReleaseEvent(event);
	}
}

void LensImageWidget::zoomChannel(int checked)	
{ 
	channels.setX(float(std::min(std::max(checked, 0), 1)));
	update();
}

void LensImageWidget::focusChannel(int checked)	
{ 
	channels.setY(float(std::min(std::max(checked, 0), 1)));
	update();
}

void LensImageWidget::fovChannel(int checked)
{ 
	channels.setZ(float(std::min(std::max(checked, 0), 1)));
	update();
}



QOpenGLShaderProgram* LensImageWidget::buildTextureChannelProgram() const
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
		"uniform mediump vec4 channels;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = texture2D(in_Texture, texc.st) * channels;\n"
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

