
#include "Lens.h"

#include <algorithm>



LensTable::LensTable()
{
}


LensTable::~LensTable()
{
}


void LensTable::CreateKeys(int rows, int columns)
{
	zoomKeys.resize(rows);
	focusKeys.resize(columns);

	// calculating sample interval
	float zoom_interval = 1.0f / float(zoomKeys.size() - 1);
	float focus_interval = 1.0f / float(focusKeys.size() - 1);

	// filling zoom key vector
	float zkey = 0.0f;
	for (int z = 0; z < zoomKeys.size(); ++z, zkey += zoom_interval)
		zoomKeys[z] = zkey;

	// filling focus key vector
	float fkey = 0.0f;
	for (int f = 0; f < focusKeys.size(); ++f, fkey += focus_interval)
		focusKeys[f] = fkey;
}


void LensTable::CreateMatrix(int rows, int columns)
{
	// reseting the lens matrix
	matrix.resize(rows);
	for (auto& l : matrix)
		l.resize(columns);
}


bool LensTable::Load(std::string filename)
{
	std::ifstream in(filename, std::ios::in);

	if (in.is_open())
	{
		char c;	// used for semicolon, space ou line break
		int rows = 0;
		int columns = 0;

		in >> rows >> columns;

		zoomKeys.resize(rows);
		focusKeys.resize(columns);

		// reseting the lens matrix
		matrix.resize(rows);
		for (auto& l : matrix)
			l.resize(columns);

		// reading zoom sample keys
		for (int i = 0; i < rows; ++i)
			in >> zoomKeys[i];
		
		// reading focus sample keys
		for (int i = 0; i < columns; ++i)
			in >> focusKeys[i];
		
		// reading matrix values
		for (int i = 0; i < rows; ++i)
			for (int j = 0; j < columns; ++j)
				in >> matrix[i][j];

		return in.good();
	}

	return false;
}


bool LensTable::Save(std::string filename) const
{
	std::ofstream out(filename, std::ios::out);

	if (out.is_open())
	{
		int rows = zoomKeys.size();
		int columns = focusKeys.size();

		out << rows << ' ' << columns << ' ' << std::endl;

		for (const auto& z : zoomKeys)
			out << z << ' ';
		out << std::endl;

		for (const auto& f : focusKeys)
			out << f << ' ';
		out << std::endl;

		for (const auto& lz : matrix)
			for (const auto& ls : lz)
				out << ls << std::endl;

		return out.good();
	}

	return false;
}


bool LensTable::ReadFile(std::string filename, LensMatrix& l)
{
	std::ifstream in(filename, std::ios::in);

	if (in.is_open())
	{
		char c;	// used for semicolon, space ou line break
		int rows = 0;
		int columns = 0;

		in >> rows >> columns;

		CreateEmptyLensTable(l, rows, columns);

		for (int i = 0; i < rows; ++i)
			for (int j = 0; j < columns; ++j)
				in >> l[i][j];

		return in.good();
	}

	return false;
}


bool LensTable::WriteFile(std::string filename, const LensMatrix& l)
{
	std::ofstream out(filename, std::ios::out);

	if (out.is_open())
	{
		int rows = l.size();
		int columns = l[0].size();

		out << rows << ' ' << columns << ' ' << std::endl;

		for (const auto& lz : l)
			for (const auto& ls : lz)
				out << ls << std::endl;

		return out.good();
	}

	return false;
}





static void printFocus(const std::pair<int, LensSample>& l)
{
	std::cout << '\t' << l.first << '\t' << l.second.fov << "   (" << l.second.zoom << ", " << l.second.focus << ")" << std::endl;
}

static void printZoom(const std::pair<int, FocusMap>& l)
{
	const FocusMap& f = l.second;
	std::cout << l.first << std::endl;

	std::for_each(f.begin(), f.end(), printFocus);
}

void LensTable::Print()
{
	std::for_each(gLensSamples.begin(), gLensSamples.end(), printZoom);
}



#if 0	
	//typedef std::map<int, LensSample> FocusMap;
	//typedef std::map<int, FocusMap> ZoomMap;

	NCAM_API bool NCamCreateLensTable(int zoomSamples, int focusSamples)
	{
		gLensSamples.clear();

		// in order to have integer keys for the maps, we move the normalization from [0,1] to [0,100]
		int resolution = 100;


		int zoomStep = resolution / zoomSamples;
		int focusStep = resolution / focusSamples;


		for (int z = 0; z <= zoomStep * zoomSamples; z += zoomStep)
		{
			FocusMap lFocusMap;

			for (int f = 0; f <= focusStep * focusSamples; f += focusStep)
			{
				double zz = (double)z / resolution;
				double ff = (double)f / resolution;
				lFocusMap.insert( std::make_pair(f, LensSample()) );
			}

			gLensSamples.insert( std::make_pair( z,  lFocusMap) );
		}

		return false;
	}

	void printFocus(const std::pair<int, LensSample>& l) 
	{
		std::cout << '\t' << l.first << '\t' << l.second.fov << "   (" << l.second.zoom << ", " << l.second.focus << ")" << std::endl;
	}

	void printZoom(const std::pair<int, FocusMap>& l) 
	{
		const FocusMap& f = l.second;
		std::cout << l.first << std::endl;

		std::for_each(f.begin(), f.end(), printFocus);
	}


	NCAM_API void NCamPrintLensTable()
	{
		std::for_each(gLensSamples.begin(), gLensSamples.end(), printZoom);
	}

	
	NCAM_API bool NCamUpdateLensSample(double z, double f, double i, double _fov, int resolution)
	{
		int zoom = z * resolution;
		int focus = f * resolution;

		auto itZoom = gLensSamples.find(zoom);
		if (itZoom == gLensSamples.end())	// not found
			itZoom = gLensSamples.find(zoom - 1);	// try a neighbor value 
		if (itZoom == gLensSamples.end())	// not found
			itZoom = gLensSamples.find(zoom + 1);	// try a neighbor value 


		if (itZoom != gLensSamples.end())	// zoom sample found
		{
			
			FocusMap& focusMap = itZoom->second;	// get focus map
			
			auto itFocus = focusMap.find(focus);
			if (itFocus == focusMap.end())	// not found
				itFocus = focusMap.find(focus - 1); // try a neighbor value 
			if (itFocus == focusMap.end())	// not found
				itFocus = focusMap.find(focus + 1); // try a neighbor value 


			if (itFocus != focusMap.end())	// focus sample found
			{
				LensSample& l = itFocus->second;	// get LensSample

				int diffZoomActual = abs(itZoom->first - z * resolution);
				int diffZoomSample = abs(itZoom->first - l.zoom * resolution);

				int diffFocusActual = abs(itFocus->first - f * resolution);
				int diffFocusSample = abs(itFocus->first - l.focus * resolution);


				// if sample is not initialized or it has a closer encoder value than previous one : update it
				if ((l.fov < 0) || (diffZoomActual <= diffZoomSample && diffFocusActual <= diffFocusSample))
				{
					l.zoom = z;
					l.focus = f;
					l.iris = i;
					l.fov = _fov;
					return true;
				}

			}
			else
			{
				// focus sample NOT found
				return false;
			}
		}
		else
		{
			// zoom sample NOT found
			return false;
		}

		return false;
	}




	NCAM_API bool NCamSaveLensTable()
	{
		try
		{
			std::ofstream outputFile("NCamLensFile.txt", std::ofstream::out);

#if 1
			const FocusMap& focusMap = gLensSamples.begin()->second;

			outputFile << "ZoomSamples " << gLensSamples.size() << std::endl;
			outputFile << "FocusSamples " << focusMap.size() << std::endl;

			// write zoom samples
			for (auto itZoom = gLensSamples.begin(); itZoom != gLensSamples.end(); ++itZoom)
				outputFile << itZoom->first << '\t';
			
			outputFile << std::endl;
			
			// write focus samples
			
			for (auto itFocus = focusMap.begin(); itFocus != focusMap.end(); ++itFocus)
				outputFile << itFocus->first << '\t';
			
			outputFile << std::endl;
			
			// write fov and distortion samples
			for (auto itZoom = gLensSamples.begin(); itZoom != gLensSamples.end(); ++itZoom)
			{
				const FocusMap& focusMap = itZoom->second;
				for (auto itFocus = focusMap.begin(); itFocus != focusMap.end(); ++itFocus)
				{
					outputFile << itFocus->second << '\t';

					float* imgptr = (float*)gpNCamClient->DistortMap(FieldIndex).GetImagePtr();
					uint32_t lWidth, lHeight;
					gpNCamClient->DistortMap(FieldIndex).GetSize(lWidth, lHeight);
					int size = lWidth * lHeight * 2 ;	// 2 channels
					for (int i=0; i<size; ++i)
						outputFile << std::fixed << imgptr[i]  << '\t';
					outputFile << std::endl;
				}
			}

#else
			for (auto itZoom = gLensSamples.begin(); itZoom != gLensSamples.end(); ++itZoom)
			{
				const FocusMap& focusMap = itZoom->second;
				for (auto itFocus = focusMap.begin(); itFocus != focusMap.end(); ++itFocus)
				{
					outputFile << itZoom->first << '\t' << itFocus->first << '\t' << itFocus->second << std::endl;
				}
			}
#endif

			outputFile.close();
		}
		catch(const std::exception& ex)
		{
			ncamLogFile << "<Error> trying to save lens table: " << ex.what() << std::endl;
			return false;
		}

		return true;

	}
#endif