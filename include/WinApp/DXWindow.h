#ifndef __GL_WINDOW_H__
#define __GL_WINDOW_H__


#include "BasicWindow.h"
#include <d3d11.h>
#include <d3dx11.h>


class WINAPP_API DXWindow : public BasicWindow
{

public:

	DXWindow();

	virtual ~DXWindow();

	virtual bool Create(const CreationParameters& params, WinApp* _app);

	virtual void Render();

	virtual void Destroy();
	
protected:

	virtual bool InitDevice();


	D3D_DRIVER_TYPE         driverType;
	D3D_FEATURE_LEVEL       featureLevel;
	ID3D11Device*           pd3dDevice;
	ID3D11DeviceContext*    pImmediateContext;
	IDXGISwapChain*         pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
	

};


#endif	// __GL_WINDOW_H__