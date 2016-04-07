#ifndef _NCAM_UNITY_H_
#define _NCAM_UNITY_H_


#include "NCam.h"
#include "UnityPlugin.h"
#include "IUnityGraphics.h"

enum NCamRenderEvent
{
	Initialize,
	Update,
	UpdateDistortion,
	Uninitialize
};

extern "C"
{

	/// Function called by Unity through rendering events
	UnityRenderingEvent NCAM_API UNITY_INTERFACE_API GetSdiOutputRenderEventFunc();

	NCAM_API void			NCamLensSample(double& zoom, double& focus, double& iris, double& fov, void* distortionMapFloatArray);

}; // extern "C"



#endif // _NCAM_UNITY_H_
