
#include "LensTablePlugin.h"
#include "Lens.h"

#include <Windows.h>
#include <gl/GL.h>
#pragma comment (lib, "Opengl32.lib")
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_RG            0x8227
#define GL_RG32F         0x8230


template <typename T>
T Lerp(T const& x, T const& x0, T const& x1, T const& y0, T const& y1)
{
	if ((x1 - x0) == 0)
		return (y0 + y1) / 2;
	return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

static void NormalizeLensMatrix(LensMatrix& matrix, float min_zoom, float max_zoom, float min_focus, float max_focus, float min_fov, float max_fov)
{
	int rowCount = matrix.size();
	int columnCount = matrix[0].size();
	for (int i = 0; i < rowCount; ++i)
	{
		for (int j = 0; j < columnCount; ++j)
		{
			matrix[i][j].zoom = Lerp((float)matrix[i][j].zoom, min_zoom, max_zoom, 0.0f, 1.0f);
			matrix[i][j].focus = Lerp((float)matrix[i][j].focus, min_focus, max_focus, 0.0f, 1.0f);
			matrix[i][j].iris = Lerp((float)matrix[i][j].iris, 0.0f, 1.0f, 0.0f, 1.0f);
			matrix[i][j].fov = Lerp((float)matrix[i][j].fov, min_fov, max_fov, 0.0f, 1.0f);
		}
	}
}

static std::pair<float, float> MinMaxFov(const LensMatrix& matrix)
{
	float minFov = FLT_MAX;
	float maxFov = FLT_MIN;

	for (auto& lz : matrix)
	{
		for (auto& ls : lz)
		{
			if (ls.fov < minFov)
				minFov = ls.fov;

			if (ls.fov > maxFov)
				maxFov = ls.fov;
		}
	}
	return std::make_pair(minFov, maxFov);
}


extern "C"
{	

	static unsigned int Port = 3;

	std::ofstream logFile; 

	int encoder[3];
	bool isConnected = false;

	static LensTable tableLens;
	static LensSample sample;
	

	LENS_TABLE_API void LensTableCreate()
	{
		logFile.open("LensTablePlugin.log", std::ios::out);
	}


	LENS_TABLE_API void LensTableDestroy()
	{
		logFile.close();
	}

	LENS_TABLE_API bool	LensTableReadFile(const char* filename, bool compute_fov_from_matrix)
	{
		if (tableLens.load(filename, compute_fov_from_matrix))
		{
			logFile << "<Info> Lens file '" << filename << "' read successfully" << std::endl;
			return true;
		}
		else
		{
			logFile << "<Error> Could not read lens file '" << filename << "'." << std::endl;
			return false;
		}
	}

	LENS_TABLE_API int LensTableRowCount()
	{
		return tableLens.zoomKeys.size();
	}

	LENS_TABLE_API int LensTableColumnCount()
	{
		return tableLens.focusKeys.size();
	}

	
	LENS_TABLE_API float LensTableFovMin()
	{
		return tableLens.minFov;
	}

	LENS_TABLE_API float LensTableFovMax()
	{
		return tableLens.maxFov;
	}
	


	LENS_TABLE_API void LensTableUpdate(float zoom, float focus)
	{
		tableLens.computeSample(zoom, focus, sample);
	}

	

	LENS_TABLE_API float LensTableFov()
	{
		return sample.fov;
	}


	LENS_TABLE_API void	LensTableProjection(void* floatArray16_GLProjectionMatrix)
	{
		float* pArrayFloat = (float*)floatArray16_GLProjectionMatrix;

		// safeguard - pointer must be not null
		if (!pArrayFloat)
		{
			logFile << "<Error> Trying to get projection parameters with a invalid float pointer" << std::endl;
			return;
		}

		std::memcpy(pArrayFloat, sample.projection.data(), sizeof(float) * 16);
	}



	LENS_TABLE_API bool	LensTableUpdateDistortionMap(float zoom, float focus, unsigned int dist_tex_id)
	{
		float z_dist = 0;
		float f_dist = 0;
		const LensSample& s = tableLens.find(zoom, focus, z_dist, f_dist);

		if (!glIsTexture(dist_tex_id))
		{
			logFile << "<Error> Trying to update distortion map with a invalid opengl texture id" << std::endl;
			return false;
		}

		GLenum lFormat = GL_RG;
		GLenum lType = GL_FLOAT;
		glBindTexture(GL_TEXTURE_2D, dist_tex_id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
			s.distortion.width,
			s.distortion.height,
			lFormat, lType,
			(const GLvoid*)s.distortion.data.data());

		return true;
	}


	LENS_TABLE_API int LensTableDistortionMapWidth()
	{
		return tableLens.matrix[0][0].distortion.width;
	}

	LENS_TABLE_API int LensTableDistortionMapHeight()
	{
		return tableLens.matrix[0][0].distortion.height;
	}

	LENS_TABLE_API int LensTableDistortionMapChannelCount()
	{
		return tableLens.matrix[0][0].distortion.channelCount;
	}

	LENS_TABLE_API int LensTableDistortionData(void* floatArray)
	{
		float* pArrayFloat = (float*)floatArray;

		// safeguard - pointer must be not null
		if (!pArrayFloat)
		{
			logFile << "<Error> Trying to get distortion data with a invalid float pointer" << std::endl;
			return 0;
		}

		int size = LensTableDistortionMapWidth() *							// width
					LensTableDistortionMapHeight() *						// height
					LensTableDistortionMapChannelCount() *					// channels
					tableLens.zoomKeys.size() * tableLens.focusKeys.size();	// depth
		
		int rows = tableLens.matrix.size();
		int columns = tableLens.matrix[0].size();
		int depth = tableLens.zoomKeys.size() * tableLens.focusKeys.size();
		int map_count = 0;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				const std::vector<float>& data = tableLens.matrix[i][j].distortion.data;
				
				const int array_index = map_count * size;
				std::memcpy(pArrayFloat + array_index, &data, size * sizeof(float));
				map_count++;
			}
		}

		return size;
	}
};