#include <iostream>
#include "NCamGLWindow.h"






int main(int argc, char* argv[])
{
	unsigned int ip_port = 38860;
	std::string ip_address = "127.0.0.1";

	if( 2 == argc )
	{
		ip_address = std::string( argv[ 1 ] );
	} 
	else if ( 3 == argc )
	{
		ip_port = atoi(argv[2]);
	}
	else
	{
		std::cerr << "Usage: " << argv[0] << " <ip> <port(optional)>" << std::endl
			<< "where IP is the IPv4 address of the server running tracking. (Port is 38860)" << std::endl;
		//return EXIT_FAILURE;
	}

	


	WinApp lApp;
	NCamGLWindow lWindow;
	
	CreationParameters lCreationParams;					
	lCreationParams.Title = "Hello Window";
	lCreationParams.WindowSize.Width  = 712;
	lCreationParams.WindowSize.Height = 400;

	if(!lWindow.Create(lCreationParams, &lApp))        // Create Our Window
	{
		std::cerr << "ERROR: Cannot create the window application. Abort. " << std::endl;
		return EXIT_FAILURE;							// Quit If Window Was Not Created
	}

	lWindow.IpAddress = ip_address;
	lWindow.Port = ip_port;
	
	if (!lWindow.OpenNCam())
	{
		std::cerr << "Error: Could not open NCam" << std::endl;
	}


	lApp.InitSetup();
	while(lApp.ProcessMainLoop()==true)
	{
		//Sleep(33);
	}


	lWindow.CloseNCam();
	lWindow.Destroy();

	return EXIT_SUCCESS;
}
