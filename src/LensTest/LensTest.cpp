#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "Lens.h"
#include "QLens.h"
#include "LensWidget.h"
#include <QApplication>
#include <QTimer>
#include <fstream>
#include <string>

static bool AreFilesEqual(const std::string& filename0, const std::string& filename1, std::wstring& message)
{
	std::ifstream file0(filename0, std::ios::in);
	std::ifstream file1(filename1, std::ios::in);

	if (!file0.is_open() || !file1.is_open())
	{
		message = L"Could not open files for comparison";
		return false;	// could not open files
	}

	while (!file0.eof() && !file1.eof())
	{
		char a = file0.get();
		char b = file1.get();
		if (a != b)
		{
			message = L"The files are not identical";
			return false;
		}
	}

	message = L"The files are identical";
	return true;
}

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

		TEST_METHOD(LoadSaveLensMatrix)
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

		TEST_METHOD(LoadSaveLensTable)
		{
			int rows = 3;
			int columns = 3;
			LensTable lt0;
			lt0.createKeys(rows, columns);
			lt0.createMatrix(rows, columns);

			float z = 0.0f;
			float f = 0.0f;
			float fov = 0.0f;
			float i = 1.0f;
			for (int i = 0; i < rows; ++i)
			{
				for (int j = 0; j < columns; ++j)
				{
					lt0.matrix[i][j] = LensSample(z, f, i, fov);
					f += 0.5f;
				}
				z += 0.5f;
			}

			lt0.save("lt0.lens");

			LensTable lt1;
			lt1.load("lt0.lens");
			lt1.save("lt1.lens");

			std::wstring wmessage;
			bool areEqual = AreFilesEqual("lt0.lens", "lt1.lens", wmessage);
			Assert::IsTrue(areEqual, wmessage.c_str(), LINE_INFO());
		}

		TEST_METHOD(LensWidgetTest)
		{
			int argc = 1;
			char* argv[] = { "LensWidget" };
			QApplication a(argc, argv);

			LensWidget w;
			w.show();

			Assert::IsTrue(w.load("../../../data/emulator.lens"), L"\n<Could not open 'emulator.lens' file>\n", LINE_INFO());

			QTimer *timer = new QTimer();
			timer->start(2000);
			QApplication::connect(timer, SIGNAL(timeout()), &a, SLOT(quit()));

			Assert::AreEqual(EXIT_SUCCESS, a.exec(), L"\n<QApplication did not finish properly>\n", LINE_INFO());
		}

		TEST_METHOD(LensWidgetIOTest)
		{
			int argc = 1;
			char* argv[] = { "LensWidget" };
			QApplication a(argc, argv);

			LensWidget w;
			w.show();

			const std::string inputFile("../../../data/emulator.lens");
			const std::string outputFile("../../../data/emulator_copy.lens");

			Assert::IsTrue(w.load(inputFile.c_str()), L"\n<Could not open 'emulator.lens' file>\n", LINE_INFO());
			Assert::IsTrue(w.save(outputFile.c_str()), L"\n<Could not save 'emulator_copy.lens' file>\n", LINE_INFO());
			
			std::wstring wmessage;
			bool areEqual = AreFilesEqual(inputFile, outputFile, wmessage);
			Assert::IsTrue(areEqual, wmessage.c_str(), LINE_INFO());


			QTimer *timer = new QTimer();
			timer->start(2000);
			QApplication::connect(timer, SIGNAL(timeout()), &a, SLOT(quit()));

			Assert::AreEqual(EXIT_SUCCESS, a.exec(), L"\n<QApplication did not finish properly>\n", LINE_INFO());
		}
	};
}

