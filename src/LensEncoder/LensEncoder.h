#ifndef _LENS_ENCODER_H_
#define _LENS_ENCODER_H_


#define USE_DISTORT_MAP



#if defined LENS_ENCODER_LIB
	#define LENS_ENCODER_API __declspec(dllexport)
#else
	#define LENS_ENCODER_API __declspec(dllimport)
#endif



extern "C"
{

	LENS_ENCODER_API bool			LensEncoderConnect(unsigned int port, bool multithread);
	LENS_ENCODER_API bool			LensEncoderIsConnected();
	LENS_ENCODER_API void			LensEncoderDisconnect();
	LENS_ENCODER_API void			LensEncoderUpdate();
	LENS_ENCODER_API bool			LensEncoderGetData(void* pArrayInt3_Encoder);

}; // extern "C"



#endif // _LENS_ENCODER_H_
