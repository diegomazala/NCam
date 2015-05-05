#ifndef _NCAM_GL_WINDOW_H_
#define _NCAM_GL_WINDOW_H_

#include "GLWindow.h"


class NCamGLWindow : public GLWindow
{
public:
	NCamGLWindow();
	~NCamGLWindow();
	void Update();
	void Render();
	void OnKeyEvent(const KeyEvent* pEvent);

	bool OpenNCam();
	void CloseNCam();

	std::string IpAddress;
	int			Port;

protected:

	virtual bool InitGL();

	int		mTimeCode[5];
	double	mProjection[16];
	double	mModelView[16];
};


#endif // _NCAM_GL_WINDOW_H_