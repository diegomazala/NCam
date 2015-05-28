#ifndef _LENS_H_
#define _LENS_H_

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

struct DistortionMap
{
	int width;
	int height; 
	int channelCount;
	std::vector<float> data;

	DistortionMap() : width(32), height(32), channelCount(2), data(width * height * channelCount, 0.0f){}

	DistortionMap(int w, int h, int ch) : width(w), height(h), channelCount(ch), data(width * height * channelCount, 0.0f){}

	DistortionMap(const DistortionMap& d) : width(d.width), height(d.height), channelCount(d.channelCount), data(d.data){}

	friend std::ostream& operator << (std::ostream& os, const DistortionMap& d)
	{
		os << d.width << ' ' << d.height << ' ' << d.channelCount << ' ';
		for (const auto& v : d.data)
			os << std::fixed << v << '\t';
		return os;
	}


	friend std::istream& operator >> (std::istream& is, DistortionMap& d)
	{
		is >> d.width >> d.height >> d.channelCount;
		d.data.resize(d.width * d.height * d.channelCount);
		for (auto& v : d.data)
			is >> v;
		return is;
	}
	

	DistortionMap& operator=(const DistortionMap& other) // copy assignment
	{
		if (this != &other)	// self-assignment check expected
		{
			this->width = other.width;
			this->height = other.height;
			this->channelCount = other.channelCount;
			this->data = other.data;
		}
		return *this;
	}

};



struct LensSample
{
	double zoom;
	double focus;
	double iris;
	double fov;
	//std::vector<float> distortion;
	DistortionMap distortion;

	LensSample() : zoom(FLT_MAX), focus(FLT_MAX), iris(FLT_MAX), fov(0.0f), distortion()
	{}

	LensSample(double z, double f, double i, double _fov) : zoom(z), focus(f), iris(i), fov(_fov), distortion()
	{}

	LensSample(double z, double f, double _fov) : zoom(z), focus(f), iris(1.0), fov(_fov), distortion()
	{}

	LensSample(const LensSample& l) : zoom(l.zoom), focus(l.focus), iris(l.iris), fov(l.fov), distortion(l.distortion)
	{}

	void reset()
	{
		zoom = FLT_MAX;
		focus = FLT_MAX;
		iris = FLT_MAX;
		fov = 0.0f;
	}

	friend std::ostream& operator << (std::ostream& os, const LensSample& l)
	{
		os << std::fixed << l.zoom << ' ' << l.focus << ' ' << l.iris << ' ' << l.fov;
		return os;
	}


	friend std::istream& operator >> (std::istream& is, LensSample& l)
	{
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

	int rowCount() const { return zoomKeys.size(); }
	int columnCount() const { return focusKeys.size(); }
	
	void createKeys(int rows, int columns);
	void createMatrix(int rows, int columns);

	void updateMinMaxFov();
	void normalizeMatrix();

	void find(float z, float f, float& z_distance, float& f_distance, int& i, int &j);
	LensSample& find(float z, float f, float& z_distance, float& f_distance);

	bool load(std::string filename);
	bool save(std::string filename) const;

	static bool readFile(std::string filename, LensMatrix& l);
	static bool writeFile(std::string filename, const LensMatrix& l);

	void print();

	

	FocusMap gFocusMap;
	ZoomMap gLensSamples;

	float zoomDeviation;
	float focusDeviation;

	std::vector<float> zoomKeys;
	std::vector<float> focusKeys;
	LensMatrix matrix;

	

	static float lerp(float x, float x0, float x1, float y0, float y1)
	{
		if ((x1 - x0) == 0)
			return (y0 + y1) / 2;
		return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
	}

	

	float normalizeZoom(float value) const { return lerp(value, minZoom, maxZoom, 0.0f, 1.0f); }
	float normalizeFocus(float value) const { return lerp(value, minFocus, maxFocus, 0.0f, 1.0f); }
	float normalizeIris(float value) const { return lerp(value, minIris, maxIris, 0.0f, 1.0f); }
	float normalizeFov(float value) const { return lerp(value, minFov, maxFov, 0.0f, 1.0f); }
	
	float denormalizeZoom(float value) const { return lerp(value, 0.0f, 1.0f, minZoom, maxZoom); }
	float denormalizeFocus(float value) const { return lerp(value, 0.0f, 1.0f, minFocus, maxFocus); }
	float denormalizeIris(float value) const { return lerp(value, 0.0f, 1.0f, minIris, maxIris); }
	float denormalizeFov(float value) const { return lerp(value, 0.0f, 1.0f, minFov, maxFov); }



	float minZoom;
	float maxZoom;
	float minFocus;
	float maxFocus;
	float minIris;
	float maxIris;
	float minFov;
	float maxFov;
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
