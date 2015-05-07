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

	friend std::ostream& operator << (std::ostream& os, const LensSample& l)
	{
		os << std::fixed << l.zoom << '\t' << l.focus << '\t' << l.iris << '\t' << l.fov << '\t';
		for (auto d = l.distortion.begin(); d != l.distortion.end(); ++d)
			os << std::fixed << *d << '\t';
		return os;
	}
};


typedef std::map<int, LensSample> FocusMap;
typedef std::map<int, FocusMap> ZoomMap;


class LensTable
{
public:
	LensTable();
	~LensTable();

	static bool ReadFile();
	static bool WriteFile();

	void Print();

	FocusMap gFocusMap;
	ZoomMap gLensSamples;

	float zoomDeviation;
	float focusDeviation;
};


#endif // _LENS_H_
