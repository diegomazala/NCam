#ifndef _NCAM_H_
#define _NCAM_H_


#define USE_DISTORT_MAP



#if defined NCAM_QT_LIB || defined NCAM_WIN_LIB
	#define NCAM_API __declspec(dllexport)
#else
	#define NCAM_API __declspec(dllimport)
#endif

extern "C"
{
	
	


	NCAM_API unsigned int	NCamErrorCode();
	NCAM_API void			NCamResetError();

	NCAM_API void			NCamCreate();
	NCAM_API void			NCamDestroy();

	NCAM_API void			NCamSetPacketType(bool cameraTracking, bool opticalParamters, bool distortMap);
	
	NCAM_API void			NCamSetIpAddress(const char* ip_address, unsigned int ip_port);

	NCAM_API bool			NCamOpen();
	NCAM_API bool			NCamIsOpen();
	NCAM_API void			NCamClose();
	NCAM_API bool			NCamUpdate();

	NCAM_API double			NCamFrameRate();

	NCAM_API int			NCamFrameLostCount();
	NCAM_API int			NCamFieldLostCount();

	NCAM_API void			NCamFieldIndex(unsigned int field_index);

	NCAM_API void			NCamGetData(void* pArrayUInt6_OpticalTimeCode, 
										void* pArrayUInt6_TrackingTimeCode,
										void* pArrayDouble13_OpticalParameters,
										void* pArrayDouble12_TrackingParameters,
										void* pArrayDouble16_GLProjectionMatrix,
										void* pArrayDouble16_GLModelViewMatrix,
										double near_plane, double far_plane);

	NCAM_API unsigned int	NCamOpticalTimeCode(void* pArrayUInt6);
	NCAM_API unsigned int	NCamTrackingTimeCode(void* pArrayUInt6);

	NCAM_API double			NCamFovHorizontal();
	NCAM_API double			NCamFovVertical();

	NCAM_API void			NCamEncoderParameters(void* pArrayDouble6);

	NCAM_API void			NCamOpticalParameters(void* pArrayDouble13);
	NCAM_API void			NCamTrackingParameters(void* pArrayDouble12);

	NCAM_API void			NCamGLMatrices(	void* pArrayDouble16_Projection, 
											void* pArrayDouble16_ModelView, 
											double near_plane, double far_plane);

	NCAM_API void			NCamGLProjectionMatrix(void* pArrayDouble16_Projection,
											double near_plane, double far_plane);

	NCAM_API void			NCamGLModelViewMatrix(void* pArrayDouble16);

	NCAM_API void			NCamPrintData();

	NCAM_API void			NCamSetDistortMapPtr(void* distort_tex_ptr);
	NCAM_API bool			NCamUpdateDistortMap();
	NCAM_API int			NCamDistortMapWidth();
	NCAM_API int			NCamDistortMapHeight();
	NCAM_API int			NCamDistortMapChannelsCount();
	NCAM_API float*			NCamDistortMapDataPtr();

	NCAM_API void			NCamLensSample(double& zoom, double& focus, double& iris, double& fov, void* distortionMapFloatArray);
}; // extern "C"



#endif // _NCAM_H_
