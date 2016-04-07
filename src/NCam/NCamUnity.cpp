
#include "NCamUnity.h"

extern "C"
{	

	static void UNITY_INTERFACE_API OnNCamRenderEvent(int render_event_id)
	{
		

		switch (static_cast<NCamRenderEvent>(render_event_id))
		{
			case NCamRenderEvent::Initialize:
			{
				NCamClose();
				NCamResetError();
				NCamSetPacketType(true, true, true);
				NCamOpen();
				break;
			}
			case NCamRenderEvent::Update:
			{
				NCamUpdate();
				break;
			}
			case NCamRenderEvent::UpdateDistortion:
			{
				NCamUpdateDistortMap();
				break;
			}
			case NCamRenderEvent::Uninitialize:
			{
				NCamClose();
				break;
			}
			default:
				break;
		}
	}

	// --------------------------------------------------------------------------
	// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.
	UnityRenderingEvent NCAM_API UNITY_INTERFACE_API GetNCamRenderEventFunc()
	{
		return OnNCamRenderEvent;
	}
};