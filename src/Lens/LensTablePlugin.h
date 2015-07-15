#ifndef __LENS_TABLE_PLUGIN_H__
#define __LENS_TABLE_PLUGIN_H__


#if defined LENS_TABLE_LIB
	#define LENS_TABLE_API __declspec(dllexport)
#else
	#define LENS_TABLE_API __declspec(dllimport)
#endif



extern "C"
{

	LENS_TABLE_API void			LensTableCrete();
	LENS_TABLE_API void			LensTableDestroy();

	LENS_TABLE_API bool			LensTableReadFile(const char* filename);

	LENS_TABLE_API int			LensTableRowCount();
	LENS_TABLE_API int			LensTableColumnCount();

	LENS_TABLE_API void			LensTableKeys(void* floatArrayRowCount_Zoom, void* floatArrayColumnCount_Focus);

	LENS_TABLE_API float		LensTableFovMin();
	LENS_TABLE_API float		LensTableFovMax();

	LENS_TABLE_API bool			LensTableUpdateLensMap(unsigned int lens_tex_id);
	LENS_TABLE_API bool			LensTableUpdateDistortionMap(float zoom, float focus, unsigned int dist_tex_id);
	
	LENS_TABLE_API bool			LensTableUpdateSample(float zoom, float focus, float iris, unsigned int lens_tex_id);

	LENS_TABLE_API void			LensTableProjectionMatrix(float zoom, float focus, void* floatArray16_GLProjectionMatrix);
	LENS_TABLE_API void			LensTableOpticalParameters(float zoom, float focus, void* floatArray13_GLProjectionMatrix);

	LENS_TABLE_API float		LensTableZoomSample();
	LENS_TABLE_API float		LensTableFocusSample();
	LENS_TABLE_API float		LensTableFovSample();
	LENS_TABLE_API float		LensTableIrisSample();

	LENS_TABLE_API void			LensTableUpdate(float zoom, float focus);
	LENS_TABLE_API float		LensTableFov(float zoom, float focus);
	LENS_TABLE_API void			LensTableProjection(void* floatArray16_GLProjectionMatrix);


	LENS_TABLE_API int			LensTableDistortionMapWidth();
	LENS_TABLE_API int			LensTableDistortionMapHeight();
	LENS_TABLE_API int			LensTableDistortionMapChannelCount();
	LENS_TABLE_API int			LensTableDistortionData(void* floatArray);

}; // extern "C"



#endif // __LENS_TABLE_PLUGIN_H__
