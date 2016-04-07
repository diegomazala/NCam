
#include "NCamGLWindow.h"
#include "NCam.h"
#include <iostream>

#include "GLFont.h"
GLFont gFont;

static void drawAxis(float lineWidth, float size);
static void drawGrid(float lineWidth);


NCamGLWindow::NCamGLWindow()
{
	NCamCreate();
}


NCamGLWindow::~NCamGLWindow()
{
	NCamDestroy();
}

bool NCamGLWindow::OpenNCam()
{

	std::cout << IpAddress << ":" << Port << std::endl;
	NCamSetPacketType(true, true, true);
	NCamSetIpAddress(IpAddress.c_str(), Port);
	return NCamOpen();
}

void NCamGLWindow::CloseNCam()
{
	NCamClose();
}


void NCamGLWindow::Update()
{
	NCamUpdate();
	NCamTrackingTimeCode(mTimeCode);
	NCamGLMatrices(mProjection, mModelView, 0.1, 1024.0);
}

void NCamGLWindow::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset view parameters
	glViewport(0, 0, Width(), Height());

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(mProjection);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(mModelView);

	glDisable(GL_LIGHTING);
	drawGrid(3);
	drawAxis(5, 1);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gFont.Plot(-0.95f, 0.90f, "%s:%d", IpAddress.c_str(), Port);
	gFont.Plot(-0.95f, 0.80f, "%d:%d:%d:%d:<%d>", mTimeCode[0], mTimeCode[1], mTimeCode[2], mTimeCode[3], mTimeCode[4]);
	gFont.Plot(-0.95f, 0.70f, "Fov(y): %.2f", NCamFovVertical());
	gFont.Plot(-0.95f, 0.60f, "Error: %d", NCamErrorCode());


	std::cout <<  mTimeCode[0] << ':' <<  mTimeCode[1] << ':' <<  mTimeCode[2] << ':' <<  mTimeCode[3] << ':' <<  mTimeCode[0] 
		<< " - Fov(y): " << NCamFovVertical() << " - Err: " << NCamErrorCode() << std::endl; 
};


bool NCamGLWindow::InitGL()
{
	GLWindow::InitGL();

	gFont.Create(-12, "Arial");

	glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	return true;
}


void NCamGLWindow::OnKeyEvent(const KeyEvent* pEvent)
{
	GLWindow::OnKeyEvent(pEvent);


	if (pEvent->PressedDown)
		return;

	if(pEvent->Key==KEY_ESCAPE)
	{
		::PostQuitMessage(0);
	}
	else if (pEvent->Key==KEY_RETURN)
	{
		std::cout << "Starting NCam..." << std::endl;
		OpenNCam();
	}
	else if (pEvent->Key==KEY_BACK)
	{
		std::cout << "Stopping NCam..." << std::endl;
		CloseNCam();
	}

}


static void drawAxis(float lineWidth, float size)
{
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(lineWidth);

	glBegin(GL_LINES);
	{
		glColor4d(1.f, 0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(size, 0.f, 0.f);

		glColor4d(0.f, 1.f, 0.f ,1.f);
		glVertex3d(0.f, 0.f, 0.f);
		glVertex3d(0.f, size, 0.f);

		glColor4d(0.f, 0.f, 1.f, 1.f);
		glVertex3d(0.f, 0.f, 0.f);
		glVertex3d(0.f, 0.f, size);

		glColor4d(1.f, 0.f, 1.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(-size, 0.f, 0.f);

		glColor4d(0.f, 1.f, 1.f, 1.f);
		glVertex3d(0.f, 0.f, 0.f);
		glVertex3d(0.f, 0.f, -size);
	}
	glEnd();
}

static void drawGrid(float lineWidth)
{
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(lineWidth);

	glBegin(GL_LINES);
	glColor4d(1.0,0.0,0.0,1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.1, 0.0, 0.0);

	glColor4d(0.0,1.0,0.0,1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.1, 0.0);

	glColor4d(0.0,0.0,1.0,1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 0.1);
	glEnd();

	glLineWidth(lineWidth * 0.5f);

	glBegin(GL_LINES);
	for(int x=-10; x<=10; x++)
	{
		if(x==0)
			glColor4d(1.0,1.0,1.0,1.0);
		else
			glColor4d(0.3,0.3,0.3,1.0);
		glVertex3d(x , 0.0, -10.0);
		glVertex3d(x , 0.0, 10.0);
	}

	for(int y=-10; y<=10; y++)
	{
		if(y==0)
			glColor4d(1.0,1.0,1.0,1.0);
		else
			glColor4d(0.3,0.3,0.3,1.0);

		glVertex3d(-10.0 , 0.0, y);
		glVertex3d(10.0 , 0.0, y);
	}
	glEnd();
}