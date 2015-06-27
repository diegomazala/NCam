
#include "Lens.h"

#include <algorithm>



LensTable::LensTable()
{
	minZoom = minFocus = minIris = minFov = 0.0f;
	maxZoom = maxFocus = maxIris = maxFov = 1.0f;
}


LensTable::~LensTable()
{
}


void LensTable::createKeys(int rows, int columns)
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


void LensTable::createMatrix(int rows, int columns)
{
	// reseting the lens matrix
	matrix.resize(rows);
	for (auto& l : matrix)
		l.resize(columns, LensSample());
}

void LensTable::updateMinMaxFov()
{
	minFov = FLT_MAX;
	maxFov = FLT_MIN;

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
}

void LensTable::normalizeMatrix()
{
	for (int i = 0; i < rowCount(); ++i)
	{
		for (int j = 0; j < columnCount(); ++j)
		{
			matrix[i][j].zoom = normalizeZoom(matrix[i][j].zoom);
			matrix[i][j].focus = normalizeFocus(matrix[i][j].focus);
			matrix[i][j].iris = normalizeIris(matrix[i][j].iris);
			matrix[i][j].fov = normalizeFov(matrix[i][j].fov);
		}
	}
}



void LensTable::find(float z, float f, float& z_distance, float& f_distance, int& i, int &j)
{
	int min_z_idx = 0;
	int max_z_idx = (int)zoomKeys.size() - 1;
	
	std::vector<float>::iterator zlow, zup;
	zlow = std::lower_bound(zoomKeys.begin(), zoomKeys.end(), z);
	zup = std::upper_bound(zoomKeys.begin(), zoomKeys.end(), z);

	if (zlow == zup)
		--zlow;

	std::vector<float>::iterator flow, fup;
	flow = std::lower_bound(focusKeys.begin(), focusKeys.end(), f);
	fup = std::upper_bound(focusKeys.begin(), focusKeys.end(), f); 

	if (flow == fup)
		--flow;

	if (zlow == zoomKeys.end())
		zlow = zoomKeys.end() - 1;

	if (zup == zoomKeys.end())
		zup = zoomKeys.end() - 1;
	
	if (flow == focusKeys.end())
		flow = focusKeys.end() - 1;

	if (fup == focusKeys.end())
		fup = focusKeys.end() - 1;

	float z_dist[2] = { std::fabs(z - *zlow), std::fabs(z - *zup) };
	float f_dist[2] = { std::fabs(f - *flow), std::fabs(f - *fup) };

	if (z_dist[0] < z_dist[1])
	{
		i = zlow - zoomKeys.begin();
		z_distance = z_dist[0];
	}
	else
	{
		i = zup - zoomKeys.begin();
		z_distance = z_dist[1];
	}

	if (f_dist[0] < f_dist[1])
	{
		j = flow - focusKeys.begin();
		f_distance = f_dist[0];
	}
	else
	{
		j = fup - focusKeys.begin();
		f_distance = f_dist[1];
	}
	
}


LensSample& LensTable::find(float z, float f, float& z_distance, float& f_distance)
{
	int i, j;
	find(z, f, z_distance, f_distance, i, j);
	return matrix[i][j];
}


bool LensTable::load(std::string filename)
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

		// reading matrix values
		for (int i = 0; i < rows; ++i)
			for (int j = 0; j < columns; ++j)
				in >> matrix[i][j].distortion;

		updateMinMaxFov();

		return in.good();
	}

	return false;
}


bool LensTable::save(std::string filename) const
{
	std::ofstream out(filename, std::ios::out);

	if (out.is_open())
	{
		int rows = zoomKeys.size();
		int columns = focusKeys.size();

		// writing count of rows and columns
		out << rows << ' ' << columns << ' ' << std::endl;

		// writing zoom keys samples
		for (const auto& z : zoomKeys)
			out << z << ' ';
		out << std::endl;

		// writing focus keys samples
		for (const auto& f : focusKeys)
			out << f << ' ';
		out << std::endl;

		// writing zoom, focus, iris and fov values
		for (const auto& lz : matrix)
			for (const auto& ls : lz)
				out << ls << std::endl;

		// writing distortion values
		for (const auto& lz : matrix)
			for (const auto& ls : lz)
				out << ls.distortion << std::endl;

		return out.good();
	}

	return false;
}


bool LensTable::readFile(std::string filename, LensMatrix& l)
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


bool LensTable::writeFile(std::string filename, const LensMatrix& l)
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

void LensTable::print()
{
	std::for_each(gLensSamples.begin(), gLensSamples.end(), printZoom);
}


