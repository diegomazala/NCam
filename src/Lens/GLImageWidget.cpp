
#include "GLImageWidget.h"






GLImageWidget::GLImageWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	image(1, 1, QImage::Format::Format_ARGB32)
{
	image.fill(0);

	// loop call update
	// QTimer *timer = new QTimer(this);
	// connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	// timer->start(33);
}


GLImageWidget::~GLImageWidget()
{
	cleanup();
}


void GLImageWidget::setImage(const QImage& inputImage)
{
	this->makeCurrent();
	this->image = inputImage;
	texture.reset(new QOpenGLTexture(inputImage.mirrored()));
	texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
	texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
}


void GLImageWidget::setImage(const QString& fileImage)
{
	setImage(QImage(fileImage));
}


void GLImageWidget::cleanup()
{
	this->makeCurrent();
	vbo->destroy();
	texture.clear();
	this->doneCurrent();
}


void GLImageWidget::initializeGL()
{
	this->initializeOpenGLFunctions();

#if 0
	// get context opengl-version
	qDebug() << "Widget OpenGl: " << format().majorVersion() << "." << format().minorVersion();
	qDebug() << "Context valid: " << context()->isValid();
	qDebug() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
	qDebug() << "OpenGL information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
	qDebug() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
	qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
	qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
#endif

	texture.reset(new QOpenGLTexture(image.mirrored()));
	texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
	texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);

	program.reset(buildImagePassthroughProgram());
	buildImagePassthroughProgram();

	vao.create();
	QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

	vbo.reset(buildQuadTextured());
}





void GLImageWidget::paintGL()
{
	QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
	
	if (!texture.isNull())
		texture->bind();

	program->bind();
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glDrawArrays(GL_QUADS, 0, 4);
	program->release();
}



void GLImageWidget::resizeGL(int w, int h)
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glViewport(0, 0, this->width(), this->height());
}


QOpenGLShaderProgram* GLImageWidget::buildImagePassthroughProgram() const
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
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = texture2D(in_Texture, texc.st);\n"
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



QOpenGLBuffer* GLImageWidget::buildQuadTextured() const 
{
	QOpenGLBuffer* lpvbo = new QOpenGLBuffer();

	typedef struct
	{
		float xyz[3];
		float uv[2];
	} VertexUV;

	VertexUV Vertices[] =
	{
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }
	};

	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t UVOffset = sizeof(Vertices[0].xyz);

	// Setup our vertex buffer object.
	lpvbo->create();
	lpvbo->bind();
	lpvbo->allocate(Vertices, BufferSize);

	lpvbo->bind();

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, 0);
	f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VertexSize, reinterpret_cast<void *>(UVOffset));

	lpvbo->release();

	return lpvbo;
}

