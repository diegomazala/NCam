
#include "TexelRender.h"



TexelRender::TexelRender(QObject *parent)
	: QObject(parent), initialized(false)
{
}


void TexelRender::initGL()
{
	initializeOpenGLFunctions();
	initialized = true;

	program.reset(buildImageRenderTexel2D());
	program->release();
	
	vao.create();
	QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

	vbo.reset(buildQuadTextured());
}

void TexelRender::render()
{
	if (!initialized)
		initGL();

	

	vao.bind();

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glDisable(GL_DEPTH_TEST);

	program->bind();
	program->setUniformValue("uv", glPixelCoord);
	f->glDrawArrays(GL_QUADS, 0, 4);
	program->release();
	
	glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, &glPixelColor[0]);
		

	vao.release();
}


QOpenGLShaderProgram* TexelRender::buildImageRenderTexel2D() const
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



QOpenGLBuffer* TexelRender::buildQuadTextured() const
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

