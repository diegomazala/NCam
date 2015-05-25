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

		TEST_METHOD(CreateLens5x5)
		{
			int rows = 5;
			int columns = 5;

			LensTable lt;
			lt.createKeys(rows, columns);
			lt.createMatrix(rows, columns);

			float z_step = 1.0f / (rows - 1);
			float f_step = 1.0f / (columns - 1);
			float z = 0.0f;
			float f = 0.0f;
			float fov = 35.0f;
			float iris = 1.0f;
	
			for (int i = 0; i < rows; ++i)
				for (int j = 0; j < columns; ++j)
					lt.matrix[i][j] = LensSample(lt.zoomKeys[i], lt.focusKeys[j], iris, fov);

			Assert::AreEqual(rows, (int)lt.zoomKeys.size(), L"\n<Row count and zoom sample keys count are not equal>\n", LINE_INFO());
			Assert::AreEqual(columns, (int)lt.focusKeys.size(), L"\n<Column count and focus sample keys count are not equal>\n", LINE_INFO());
			Assert::AreEqual(rows, (int)lt.matrix.size(), L"\n<Row count and matrix zoom count are not equal in lens matrix>\n", LINE_INFO());
			Assert::AreEqual(columns, (int)lt.matrix[0].size(), L"\n<Column count and focus count are not equal in lens matrix>\n", LINE_INFO());

			lt.save("lens5x5.lens");
		}

		TEST_METHOD(LensMatrixTest)
		{
			int rows = 3;
			int columns = 3;
			LensTable lt;
			lt.createKeys(rows, columns);
			lt.createMatrix(rows, columns);

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

		TEST_METHOD(LoadSave)
		{
			int rows = 3;
			int columns = 3;
			LensTable lt;
			lt.createKeys(rows, columns);
			lt.createMatrix(rows, columns);

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

