#include <Windows.h>
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
	bool multithread = true;
	int data[3] = {0, 0, 0};


	//
	// Try to connect
	//
	if (!LensEncoderConnect(port, multithread))
	{
		std::cerr << "Could not connect to port " << port << std::endl;
		return EXIT_FAILURE;
	}


	//
	// If connected, run loop until ESCAPE be pressed
	//
	bool stop = false;
	while (!stop)
	{
		_sleep(30);
		if (GetAsyncKeyState(VK_ESCAPE))
			stop = true;
		
		if (LensEncoderGetData(data))
			std::cout << data[0] << '\t' << data[1] << '\t' << data[2] << std::endl;
	}


	//
	// Disconnect 
	//
	LensEncoderDisconnect();



	return EXIT_SUCCESS;
}
