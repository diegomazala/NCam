#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "Lens.h"
#include "QLens.h"
#include "LensWidget.h"
#include <QApplication>
#include <QTimer>

namespace LensTest
{
	TEST_CLASS(LensTableTest)
	{
	public:

		TEST_METHOD(LensMatrixTest)
		{
			int rows = 3;
			int columns = 3;
			LensTable lt;
			lt.CreateKeys(rows, columns);
			lt.CreateMatrix(rows, columns);

			float z = 0.0f;
			float f = 0.0f;
			float fov = 0.0f;
			float i = 1.0f;
			for (int i = 0; i < rows; ++i)
			{
				for (int j = 0; j < columns; ++j)
				{
					lt.matrix[i][j] = LensSample(z, f, i, fov);
					f += 0.5f;
				}
				z += 0.5f;
			}

			
			Assert::AreEqual(rows, (int)lt.zoomKeys.size(), L"\n<Row count and zoom sample keys count are not equal>\n", LINE_INFO());
			Assert::AreEqual(columns, (int)lt.focusKeys.size(), L"\n<Column count and focus sample keys count are not equal>\n", LINE_INFO());
			Assert::AreEqual(rows, (int)lt.matrix.size(), L"\n<Row count and matrix zoom count are not equal in lens matrix>\n", LINE_INFO());
			Assert::AreEqual(columns, (int)lt.matrix[0].size(), L"\n<Column count and focus count are not equal in lens matrix>\n", LINE_INFO());

			QImage img1;
			LensMatrix2QImage(lt.matrix, img1);
			Assert::IsTrue(img1.save("lens_table_1.png", "png", 100), L"\n<Fail saving lens table into image>\n", LINE_INFO());

			QImage img2;
			Assert::IsTrue(img2.load("lens_table_1.png"), L"\n<Fail loading lens table from image>\n", LINE_INFO());
			LensMatrix lmat;
			QImage2LensMatrix(img2, lmat);

			Assert::AreEqual(rows, (int)lmat.size(), L"\n<Row count and matrix zoom count are not equal in lens matrix>\n", LINE_INFO());
			Assert::AreEqual(columns, (int)lmat[0].size(), L"\n<Column count and focus count are not equal in lens matrix>\n", LINE_INFO());

			QImage img3;
			LensMatrix2QImage(lmat, img3);
			Assert::IsTrue(img3.save("lens_table_2.png", "png", 100), L"\n<Fail saving lens matrix into image>\n", LINE_INFO());
		}


		TEST_METHOD(LensWidgetTest)
		{
			int argc = 1;
			char* argv[] = { "LensWidget" };
			QApplication a(argc, argv);

			LensWidget w;
			w.show();

			w.load("../lenstable3x3.lens");

			QTimer *timer = new QTimer();
			timer->start(2000);
			QApplication::connect(timer, SIGNAL(timeout()), &a, SLOT(quit()));

			Assert::AreEqual(EXIT_SUCCESS, a.exec(), L"\n<QApplication did not finish properly>\n", LINE_INFO());
		}
	};
}

/*

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	NCamLensReaderMainWindow w;
	w.show();
	
	LensMatrix lensTable;
	CreateLensTable(lensTable, 3, 3);
	
	
	QImage img1;
	LensTable2QImage(lensTable, img1);
	img1.save("../img1.png", "png", 100);


	QImage img2;
	img2.load("../img1.png");
	LensMatrix lensTable2;
	QImage2LensTable(img2, lensTable2);

	for (int i = 0; i < img1.height(); ++i)
	{
		for (int j = 0; j < img1.width(); ++j)
		{
			QRgb rgba = img1.pixel(j, i);
			
			//double dr = (double)qRed(rgba) / 255.0;
			//double dg = (double)qGreen(rgba) / 255.0;
			//double db = (double)qBlue(rgba) / 255.0;
			//double da = (double)qAlpha(rgba) / 255.0;
			//std::cout << std::fixed << dr << ' ' << dg << ' ' << db << ' ' << da << '\t';

			int ir = qRed(rgba);
			int ig = qGreen(rgba);
			int ib = qBlue(rgba);
			int ia = qAlpha(rgba);
			//std::cout << std::fixed << ir << ' ' << ig << ' ' << ib << ' ' << ia << '\t';
		}
		std::cout << std::endl;
	}

	QImage img3;
	LensTable2QImage(lensTable2, img3);
	img3.save("../img3.png", "png", 100);


	LensTable lt1, lt2;
	QImage2LensTable(img1, lt1.matrix);
	lt1.CreateKeys(lt1.matrix.size(), lt1.matrix[0].size());
	lt1.Save("../lenstable1.lens");

	lt2.Load("../lenstable1.lens");
	lt2.Save("../lenstable2.lens");


#if 0
	if (!LensTable::WriteFile("../lensmatrix.lens", lensTable))
		std::cerr << "<error> Could not write lens file" << std::endl;

	LensMatrix lensTable3;
	if (!LensTable::ReadFile("../lensmatrix.lens", lensTable3))
		std::cerr << "<error> Could not read lens file" << std::endl;


	if (!LensTable::WriteFile("../lensmatrix2.lens", lensTable3))
		std::cerr << "<error> Could not write lens file" << std::endl;
#else


	return a.exec();
}


*/