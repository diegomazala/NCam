#ifndef _LENS_H_
#define _LENS_H_

#include <fstream>
#include <iostream>
#include <map>
#include <vector>


struct LensSample
{
	double zoom;
	double focus;
	double iris;
	double fov;
	std::vector<double> distortion;

	LensSample() : zoom(-1.0), focus(-1.0), iris(-1.0), fov(-1.0)
	{}

	LensSample(double z, double f, double i, double _fov) : zoom(z), focus(f), iris(i), fov(_fov)
	{}

	LensSample(double z, double f, double _fov) : zoom(z), focus(f), iris(0.0), fov(_fov)
	{}

	LensSample(const LensSample& l) : zoom(l.zoom), focus(l.focus), iris(l.iris), fov(l.fov)
	{}


	friend std::ostream& operator << (std::ostream& os, const LensSample& l)
	{
		os << std::fixed << l.zoom << ' ' << l.focus << ' ' << l.iris << ' ' << l.fov;
		//for (auto d = l.distortion.begin(); d != l.distortion.end(); ++d)
		//	os << std::fixed << *d << '\t';
		return os;
	}


	friend std::istream& operator >> (std::istream& is, LensSample& l)
	{
		char c;
		is >> l.zoom >> l.focus >> l.iris >> l.fov;
		return is;
	}



	LensSample& operator=(const LensSample& other) // copy assignment
	{
		if (this != &other)	// self-assignment check expected
		{ 
			this->zoom = other.zoom;
			this->focus = other.focus;
			this->iris = other.iris;
			this->fov = other.fov;
		}
		return *this;
	}
};


typedef std::map<int, LensSample> FocusMap;
typedef std::map<int, FocusMap> ZoomMap;
typedef std::vector<std::vector<LensSample>> LensMatrix;

class LensTable
{
public:
	LensTable();
	~LensTable();

	void CreateKeys(int rows, int columns);
	void CreateMatrix(int rows, int columns);

	bool Load(std::string filename);
	bool Save(std::string filename) const;

	static bool ReadFile(std::string filename, LensMatrix& l);
	static bool WriteFile(std::string filename, const LensMatrix& l);

	void Print();

	int rowCount() const { return zoomKeys.size(); }
	int columnCount() const { return focusKeys.size(); }

	FocusMap gFocusMap;
	ZoomMap gLensSamples;

	float zoomDeviation;
	float focusDeviation;

	std::vector<float> zoomKeys;
	std::vector<float> focusKeys;
	LensMatrix matrix;
};



static void CreateEmptyLensTable(LensMatrix& lt, int zoom_sample_count, int focus_sample_count)
{
	int rows = zoom_sample_count;
	int columns = focus_sample_count;

	std::vector<float> zoomKeys(zoom_sample_count);
	std::vector<float> focusKeys(focus_sample_count);

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


	// reseting the lens matrix
	lt.clear();

	if (lt.size() != rows)
		lt.resize(rows);

	if (lt[0].size() != columns)
		for (auto& l : lt)
			l.resize(columns);
}


static void CreateLensTable(LensMatrix& lt, int zoom_sample_count, int focus_sample_count)
{
	int rows = zoom_sample_count;
	int columns = focus_sample_count;

	std::vector<float> zoomKeys(zoom_sample_count);
	std::vector<float> focusKeys(focus_sample_count);

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


	// reseting the lens matrix
	lt.clear();

	if (lt.size() != rows)
		lt.resize(rows);

	if (lt[0].size() != columns)
		for (auto& l : lt)
			l.resize(columns);

	float iris = 1.0f;
	float fov = 0.0f;

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			fov = float(i * columns + j) / float(rows * columns) * 100.0f;
			lt[i][j] = LensSample(zoomKeys[i], focusKeys[j], iris, fov);
			//std::cout << lt[i][j] << '\t';
		}
		//std::cout << std::endl;
	}
}



#endif // _LENS_H_
