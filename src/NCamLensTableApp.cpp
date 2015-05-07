#if 1


#include "MainWindow.h"
#include <QApplication>


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	MainWindow w;
	w.show();

	return a.exec();
}



#else
#include "NCam.h";

#include <Windows.h>

#include <iostream>
#include <string>

#include <stdint.h>	// integer variants 

#include <signal.h>	// To capture properly ending with CTRL-C



//void signal_callback_handler(int /*signum*/)
//{
	//NCamClose();

	// Terminate program
	//exit(0);
//}


int main(int argc, char* argv[])
{

	//	signal(SIGINT, signal_callback_handler);	// Register signal and signal handler

	uint16_t ip_port = 38860;
	//std::string ip_address = "192.168.1.107"; 
	std::string ip_address = "127.0.0.1";

	if (2 == argc)
	{
		ip_address = std::string(argv[1]);
	}
	else if (3 == argc)
	{
		ip_port = atoi(argv[2]);
	}
	else
	{
		std::cerr << "Usage: " << argv[0] << " <ip> <port(optional)>" << std::endl
			<< "where IP is the IPv4 address of the server running tracking. (Port is 38860)" << std::endl;
		//return EXIT_FAILURE;
	}

	NCamCreate();
	NCamSetPacketType(true, true, true);
	if (!NCamOpen(ip_address.c_str(), ip_port))
	{
		std::cout << "Error: Could not open NCam -> " << ip_address << ":" << ip_port << std::endl;
		return EXIT_FAILURE;
	}
	
	unsigned int time_code[6];

	std::cout << "wait..." << std::endl;

	Sleep(2000);
	
	for (int i=0; i<10; ++i)
	{
		NCamUpdate();
		NCamPrintData();
		Sleep(100);
	}

	NCamClose();
	NCamDestroy();

	std::cout << "--- \\\\ ---" << std::endl;

	return EXIT_SUCCESS;
}
#endif