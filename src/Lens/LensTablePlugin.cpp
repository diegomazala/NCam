
#include "LensTablePlugin.h"
#include "Lens.h"

#include <Windows.h>
#include "GL/GL.h"
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

	LENS_TABLE_API bool	LensTableReadFile(const char* filename)
	{
		if (tableLens.load(filename))
		{
			logFile << "<Info> Lens file '" << filename << "' read successfuly" << std::endl;
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
	

		
	LENS_TABLE_API bool	LensTableUpdateLensMap(unsigned int lens_tex_id)
	{
		if (!glIsTexture(lens_tex_id))
		{
			logFile << "<Error> Trying to update lens map with a invalid opengl texture id" << std::endl;
			return false;
		}
		
		GLenum lFormat = GL_RGBA;
		GLenum lType = GL_FLOAT;
		glBindTexture(GL_TEXTURE_2D, lens_tex_id);

		LensMatrix matrix = tableLens.matrix;
		int rows = matrix.size();
		int columns = matrix[0].size();
		tableLens.updateMinMaxFov();
		tableLens.normalizeMatrix();

		std::vector<float> texels;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				const LensSample& ls = matrix[i][j];
				texels.push_back(ls.zoom);	// red
				texels.push_back(ls.focus);	// green
				texels.push_back(ls.fov);	// blue
				texels.push_back(ls.iris);	// alpha
				
			}
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
						tableLens.zoomKeys.size(), 
						tableLens.focusKeys.size(), 
						lFormat, lType, 
						texels.data());
		return true;
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

		return false;
	}




	LENS_TABLE_API bool	LensTableUpdateSample(float zoom, float focus, float iris, unsigned int lens_tex_id)
	{
		if (!glIsTexture(lens_tex_id))
		{
			logFile << "<Error> Trying to update lens sample with a invalid opengl texture id" << std::endl;
			return false;
		}

		GLenum lFormat = GL_RGBA;
		GLenum lType = GL_FLOAT;
		glBindTexture(GL_TEXTURE_2D, lens_tex_id);
		float texel[4];
		glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, texel);

		sample.zoom	= texel[0];
		sample.focus= texel[1];
		sample.fov	= texel[2];
		sample.iris = texel[3];

		return false;
	}

	LENS_TABLE_API float LensTableZoom()
	{
		return sample.zoom;
	}

	LENS_TABLE_API float LensTableFocus()
	{
		return sample.focus;
	}

	LENS_TABLE_API float LensTableFov()
	{
		return sample.fov;
	}

	LENS_TABLE_API float LensTableIris()
	{
		return sample.iris;
	}

};