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

	LENS_TABLE_API bool			LensTableReadFile(const char* filename, bool compute_fov_from_matrix = true);

	LENS_TABLE_API int			LensTableRowCount();
	LENS_TABLE_API int			LensTableColumnCount();

	LENS_TABLE_API float		LensTableFovMin();
	LENS_TABLE_API float		LensTableFovMax();

	LENS_TABLE_API void			LensTableUpdate(float zoom, float focus);
	LENS_TABLE_API float		LensTableFov();
	LENS_TABLE_API void			LensTableProjection(void* floatArray16_GLProjectionMatrix);
	
	LENS_TABLE_API void			LensTableSetDistortionMapId(void* distort_tex_ptr);
	LENS_TABLE_API bool			LensTableUpdateDistortionMap();
	LENS_TABLE_API int			LensTableDistortionMapWidth();
	LENS_TABLE_API int			LensTableDistortionMapHeight();
	LENS_TABLE_API int			LensTableDistortionMapChannelCount();
	LENS_TABLE_API int			LensTableDistortionData(void* floatArray);

}; // extern "C"



#endif // __LENS_TABLE_PLUGIN_H__
