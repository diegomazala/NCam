
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
	zoomKeys.clear();
	focusKeys.clear();
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
	float fkey = 1.0f;
	for (int f = 0; f < focusKeys.size(); ++f, fkey -= focus_interval)
		focusKeys[f] = fkey;
}


void LensTable::createMatrix(int rows, int columns)
{
	// reseting the lens matrix
	matrix.clear();
	matrix.resize(rows);
	for (auto& l : matrix)
	{
		l.clear();
		l.resize(columns, LensSample());
	}
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

	if (zlow == zoomKeys.end())
		zlow = zoomKeys.end() - 1;

	if (zup == zoomKeys.end())
		zup = zoomKeys.end() - 1;

	std::vector<float>::reverse_iterator flow, fup;
	flow = std::lower_bound(focusKeys.rbegin(), focusKeys.rend(), f);
	fup = std::upper_bound(focusKeys.rbegin(), focusKeys.rend(), f); 

	if (flow == focusKeys.rbegin())
	{
		if (flow == fup)
			fup = focusKeys.rbegin() + 1;
		flow = focusKeys.rbegin() + 1;
	}

	if (fup == focusKeys.rend())
		fup = focusKeys.rend() - 1;

	if (flow == fup)
		++fup;

	//if ((flow - 1) == focusKeys.rbegin())
	//	std::cout << "((flow - 1) == focusKeys.rbegin())" << std::endl;
	//if ((flow - 1) == focusKeys.rend())
	//	std::cout << "flow - 1) == focusKeys.rend()" << std::endl;

	//if ((fup - 1) == focusKeys.rbegin() || (fup - 1) == focusKeys.rend())
	//	std::cout << "((flup - 1) == focusKeys.rbegin()) || (flup - 1) == focusKeys.rend()" << std::endl;

	float z_dist[2] = { std::fabs(z - *zlow), std::fabs(z - *zup) };
	float f_dist[2] = { std::fabs(f - *(flow-1)), std::fabs(f - *(fup-1)) };

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
		j = focusKeys.rend() - flow;
		f_distance = f_dist[0];
	}
	else
	{
		j = focusKeys.rend() - fup;
		f_distance = f_dist[1];
	}
	
}


void LensTable::findNeighbours(float z, float f, LensSample& q11, LensSample& q12, LensSample& q21, LensSample& q22)
{
	int min_z_idx = 0;
	int max_z_idx = (int)zoomKeys.size() - 1;

	std::vector<float>::iterator zlow, zup;
	zlow = std::lower_bound(zoomKeys.begin(), zoomKeys.end(), z);
	zup = std::upper_bound(zoomKeys.begin(), zoomKeys.end(), z);
		

	if (zlow == zoomKeys.end())
		zlow = zoomKeys.end() - 1;

	if (zup == zoomKeys.end())
		zup = zoomKeys.end() - 1;

	if (zlow == zup)
		--zlow;

	std::vector<float>::reverse_iterator flow, fup;
	flow = std::lower_bound(focusKeys.rbegin(), focusKeys.rend(), f);
	fup = std::upper_bound(focusKeys.rbegin(), focusKeys.rend(), f);

	if (flow == focusKeys.rbegin())
		flow = focusKeys.rbegin() + 1;

	if (fup == focusKeys.rend())
		fup = focusKeys.rend() - 1;

	if (flow == fup)
		++fup;

	float z_dist[2] = { std::fabs(z - *zlow), std::fabs(z - *zup) };
	float f_dist[2] = { std::fabs(f - *(flow - 1)), std::fabs(f - *(fup - 1)) };
	
	int i1 = zlow - zoomKeys.begin();
	int i2 = zup - zoomKeys.begin();
	int j1 = focusKeys.rend() - flow;
	int j2 = focusKeys.rend() - fup;

	q11 = matrix[i1][j1];
	q12 = matrix[i1][j2];
	q21 = matrix[i2][j1];
	q22 = matrix[i2][j2];
}




LensSample& LensTable::find(float z, float f, float& z_distance, float& f_distance)
{
	int i, j;
	find(z, f, z_distance, f_distance, i, j);
	return matrix[i][j];
}


float LensTable::getFov(float zoom, float focus)
{
	LensSample q11, q12, q21, q22;
	findNeighbours(zoom, focus, q11, q12, q21, q22);
	float x1 = (q11.zoom + q12.zoom) / 2.0f;
	float x2 = (q21.zoom + q22.zoom) / 2.0f;
	float y1 = (q11.focus + q21.focus) / 2.0f;
	float y2 = (q12.focus + q22.focus) / 2.0f;
	return bilerp(q11.fov, q12.fov, q21.fov, q22.fov, x1, x2, y1, y2, zoom, focus);
}



void LensTable::computeSample(float zoom, float focus, LensSample& rSample)
{
	zoom = clamp(zoom, 0.0f, 1.0f);
	focus = clamp(focus, 0.0f, 1.0f);
	LensSample q11, q12, q21, q22;
	findNeighbours(zoom, focus, q11, q12, q21, q22);
	float x1 = (q11.zoom + q12.zoom) / 2.0f;
	float x2 = (q21.zoom + q22.zoom) / 2.0f;
	float y1 = (q11.focus + q21.focus) / 2.0f;
	float y2 = (q12.focus + q22.focus) / 2.0f;
	float fov = bilerp(q11.fov, q12.fov, q21.fov, q22.fov, x1, x2, y1, y2, zoom, focus);
	
	rSample.zoom = zoom;
	rSample.focus = focus;
	rSample.fov = fov;

	for (int i = 0; i < rSample.projection.size(); ++i)
		rSample.projection[i] = bilerp(q11.projection[i], q12.projection[i], q21.projection[i], q22.projection[i], x1, x2, y1, y2, zoom, focus);

	for (int i = 0; i < rSample.optical.size(); ++i)
		rSample.optical[i] = bilerp(q11.optical[i], q12.optical[i], q21.optical[i], q22.optical[i], x1, x2, y1, y2, zoom, focus);

	for (int i = 0; i < rSample.distortion.data.size(); ++i)
		rSample.distortion.data[i] = bilerp(q11.distortion.data[i], q12.distortion.data[i], q21.distortion.data[i], q22.distortion.data[i], x1, x2, y1, y2, zoom, focus);
}



void LensTable::roundSamples(int precision)
{
	int rows = zoomKeys.size();
	int columns = focusKeys.size();

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			if (matrix[i][j].zoom > 0.99)		// rounding zoom borders
				matrix[i][j].zoom = 1;
			if (matrix[i][j].focus > 0.99)		// rounding focus borders
				matrix[i][j].focus = 1;

			if (matrix[i][j].zoom < 0.01)		// rounding zoom borders
				matrix[i][j].zoom = 0;
			if (matrix[i][j].focus < 0.01)		// rounding focus borders
				matrix[i][j].focus = 0;

			matrix[i][j].zoom = roundP(matrix[i][j].zoom, precision);
			matrix[i][j].focus = roundP(matrix[i][j].focus, precision);
		}
	}
}

bool LensTable::load(std::string filename, bool compute_fov_from_prj_matrix)
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

		// reading distortion values
		for (int i = 0; i < rows; ++i)
			for (int j = 0; j < columns; ++j)
				in >> matrix[i][j].distortion;

		if (compute_fov_from_prj_matrix)
		{
			for (int i = 0; i < rows; ++i)
				for (int j = 0; j < columns; ++j)
					matrix[i][j].computeFovFromProjectionMatrix();
		}

		updateMinMaxFov();
		roundSamples(3);
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

