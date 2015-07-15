#include <QApplication>
#include "LensImageWidget.h"
#include "QLens.h"
#include "QSystemTrayWindow.h"
#include "LensServerMainWindow.h"

int main(int argc, char* argv[])
{
	

#if 1

	LensTable table;
	if (!table.load("../../../data/Fujinon_HA14x45BERD-S6B_6701118.lens"))
		std::cout << "Could not load file" << std::endl;
	else
		std::cout << "File loaded successfuly!!" << std::endl;

	float zoom, focus, fov;
	float z_dist = 0, f_dist = 0;
	LensSample s, q11, q12, q21, q22;
	float x1, x2, y1, y2;

	table.roundSamples(3);

#if 1
	//zoom = 1;
	//focus = 0;
	zoom = atof(argv[1]);
	focus = atof(argv[2]);
	table.findNeighbours(zoom, focus, q11, q12, q21, q22);
	
	x1 = (q11.zoom + q12.zoom) / 2.0f;
	x2 = (q21.zoom + q22.zoom) / 2.0f;
	y1 = (q11.focus + q21.focus) / 2.0f;
	y2 = (q12.focus + q22.focus) / 2.0f;

	fov = table.bilerp(q11.fov, q12.fov, q21.fov, q22.fov, x1, x2, y1, y2, zoom, focus);

	std::cout << std::endl << std::fixed
		<< "Looking for: " << zoom << ", " << focus << std::endl
		<< q11.zoom << ", " << q11.focus << " : " << q11.fov << std::endl
		<< q12.zoom << ", " << q12.focus << " : " << q12.fov << std::endl
		<< q21.zoom << ", " << q21.focus << " : " << q21.fov << std::endl
		<< q22.zoom << ", " << q22.focus << " : " << q22.fov << std::endl
		<< "Result: " << fov << std::endl;

	
	std::cout << "result: " << fov << std::endl << std::endl << std::endl;



	

#else

	for (int z = 0; z < 11; ++z)
	{
		for (int f = 10; f >-1; --f)
		{
			zoom = float(z) / 10.0f;
			focus = float(f) / 10.0f ;
			table.findNeighbours(zoom, focus, q11, q12, q21, q22);
			x1 = (q11.zoom + q12.zoom) / 2.0f;
			x2 = (q21.zoom + q22.zoom) / 2.0f;
			y1 = (q11.focus + q21.focus) / 2.0f;
			y2 = (q12.focus + q22.focus) / 2.0f;
			fov = table.bilerp(q11.fov, q12.fov, q21.fov, q22.fov, x1, x2, y1, y2, zoom, focus);
			std::cout << std::fixed << zoom << ", " << focus << " : " << table.getFov(zoom, focus) << std::endl;
		}

	}
#endif

	

	return EXIT_SUCCESS;
#else

	
	QApplication a(argc, argv);
	//QSystemTrayWindow window;
	LensServerMainWindow window;
	window.show();
	return a.exec();
#endif
	
	
}
