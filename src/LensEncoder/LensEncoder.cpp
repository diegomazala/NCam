
#include "LensEncoder.h"
#include "SerialPort.h"
#include "FujinonEncoder.h"
#include "ThreadReader.h"
#include <fstream>

#include "UnityPlugin.h"
#include "IUnityGraphics.h"

enum LensEncoderRenderEvent
{
	Initialize,
	Update,
	Uninitialize
};


extern "C"
{	

	static unsigned int Port = 3;

	static FujinonEncoder* lensHA22x7 = nullptr;
	static ThreadReader* threadLens = nullptr;
	int encoder[3];
	bool isConnected = false;
	bool multithread = false;
	

	LENS_ENCODER_API bool LensEncoderConnect(unsigned int port, bool multithread_reading)
	{
		multithread = multithread_reading;
		if (multithread)
		{
			if (threadLens)
				delete threadLens;
			
			threadLens = new ThreadReader();
			isConnected = threadLens->Start(port);
		}
		else
		{
			if (lensHA22x7)
				delete lensHA22x7;

			lensHA22x7 = new FujinonEncoder();
			lensHA22x7->setPortNumber(port);
			isConnected = lensHA22x7->initConnection();
		}

		return isConnected;
	}


	LENS_ENCODER_API bool LensEncoderIsConnected()
	{
		return isConnected;
	}


	LENS_ENCODER_API void LensEncoderDisconnect()
	{		
		if (multithread)
		{
			if (threadLens)
			{
				delete threadLens;
				threadLens = nullptr;
			}
		}
		else
		{
			if (lensHA22x7)
			{
				delete lensHA22x7;
				lensHA22x7 = nullptr;
			}
		}
		
		isConnected = false;
	}


	LENS_ENCODER_API void LensEncoderUpdate()
	{
		if (multithread)
		{
			threadLens->Data(encoder[0], encoder[1], encoder[2]);
		}
		else
		{
			if (!lensHA22x7)
				return;

			encoder[0] = lensHA22x7->getZoomPos();	// Request zoom
			encoder[1] = lensHA22x7->getFocusPos();	// Request focus
			encoder[2] = lensHA22x7->getIrisPos();	// Request iris
		}
	}


	LENS_ENCODER_API bool LensEncoderGetData(void* pArrayInt3_Encoder)
	{
		// safeguard - pointer must be not null
		if (!pArrayInt3_Encoder)
			return false;

		int* pArrayInt = (int*)pArrayInt3_Encoder;

		// safeguard - pointer must be not null
		if (!pArrayInt)
			return false;

		// copying to array
		std::memcpy(pArrayInt, encoder, sizeof(int) * 3);

		return true;
	}

	static void UNITY_INTERFACE_API OnLensEncoderRenderEvent(int render_event_id)
	{
		switch (static_cast<LensEncoderRenderEvent>(render_event_id))
		{
		case LensEncoderRenderEvent::Initialize:
		{
			break;
		}
		case LensEncoderRenderEvent::Update:
		{
			LensEncoderUpdate();
			break;
		}
		case LensEncoderRenderEvent::Uninitialize:
		{
			break;
		}
		default:
			break;
		}
	}


	// --------------------------------------------------------------------------
	// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.
	UnityRenderingEvent LENS_ENCODER_API UNITY_INTERFACE_API GetLensEncoderRenderEventFunc()
	{
		return OnLensEncoderRenderEvent;
	}

};
