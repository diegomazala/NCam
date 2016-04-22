#include <iostream>
#include "LensEncoder.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr
			<< "Please, set the number of port. " << std::endl
			<< "Usage: LenEncoderApp.exe 3" << std::endl
			<< std::endl;
		return EXIT_FAILURE;
	}

	int port = atoi(argv[1]);
	bool multithread = false;
	int data[3] = {0, 0, 0};

	if (!LensEncoderConnect(port, multithread))
	{
		std::cerr << "Could not connect to port " << port << std::endl;
		return EXIT_FAILURE;
	}

	for (int i = 0; i < 99999; ++i)
	{

		LensEncoderUpdate();

		if (LensEncoderGetData(data))
		{
			std::cout << data[0] << '\t' << data[1] << '\t' << data[2] << std::endl;
		}

		_sleep(30);
	}

	LensEncoderDisconnect();



	return EXIT_SUCCESS;
}
