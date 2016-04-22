
#include "LensEncoder.h"
#include "SerialPort.h"
#include "FujinonEncoder.h"
#include "ThreadReader.h"
#include <fstream>



extern "C"
{	

	static unsigned int Port = 3;

	static FujinonEncoder lensHA22x7;
	static ThreadReader threadLens;
	int encoder[3];
	bool isConnected = false;
	bool multithread = false;
	

	LENS_ENCODER_API bool LensEncoderConnect(unsigned int port, bool multithread_reading)
	{
		multithread = multithread_reading;
		if (multithread)
		{
			isConnected = threadLens.Start(port);
		}
		else
		{
			lensHA22x7.setPortNumber(port);
			isConnected = lensHA22x7.initConnection();
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
			threadLens.Stop();
		}
		else
		{
			lensHA22x7.finishConnection();
		}
		
		isConnected = false;
	}


	LENS_ENCODER_API void LensEncoderUpdate()
	{
		if (!multithread)
		{
			encoder[0] = lensHA22x7.getZoomPos();	// Request zoom
			encoder[1] = lensHA22x7.getFocusPos();	// Request focus
			encoder[2] = lensHA22x7.getIrisPos();	// Request iris
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

		if (multithread)
		{ 
			// copying to array
			for (int i = 0; i < 3; ++i)
				pArrayInt[i] = threadLens.Data()[i];
		}
		else
		{
			// copying to array
			for (int i = 0; i < 3; ++i)
				pArrayInt[i] = encoder[i];
		}

		return true;
	}

};
