

#include "NCamWinClient.h"

#include <iostream>

#include <ws2tcpip.h>


#include "NcDataStreamBase.h"
#include "NcDataStreamClientBase.h"
#include "NcDataStreaming.h"
#include "NcDataStreamCamTrack.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

NCamWinClient::NCamWinClient() :
		NCamClient(),
		mSocket(INVALID_SOCKET),
		mSocketAddr(NULL)
{
}
NCamWinClient::~NCamWinClient()
{
}



void NCamWinClient::Connect(const std::string ipAddress, int port)
{	
	//Set the connection Parameters
	this->SetConnectionParameters(ipAddress, port);
	this->StartStreaming();
}


void NCamWinClient::Disconnect()
{
	this->StopStreaming();
}



/**
* @brief Map the buffer for packet access.
* @param lSuccess Flag indicate if the mapping success.
* @return The last packet read through the socket.
* @warning No concurrent access allowed!
*/
Packets_t& NCamWinClient::MapPackets(bool &lSuccess)
{
	lSuccess = true;
	return mDataBuffer;
}


/**
* @brief Unmap the buffer.
* @warning No concurrent access allowed!
*/
void NCamWinClient::UnmapPackets()
{
}


/**
* @brief IsConnected is pure virtual, has to be overloaded as it depends on socket implementation.
* @return True if the socket is opened.
*/
bool NCamWinClient::IsConnected() const
{
	return (INVALID_SOCKET != mSocket);
}



/**
* @brief InternalOpen This function has to be overloaded with the specific socket openning process.
* @return True if the connection can be opened, false if the socket is already openned or if the socket cannot be openned.
*/
bool NCamWinClient::InternalOpen()
{
	if (IsConnected())
		return false;
	int iResult = 0;
	// Initialize Winsock
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
		return false;
	}
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	char lPort[256];
	sprintf_s(lPort, 256, "%d", GetPort());
	iResult = getaddrinfo(GetIpAddress().c_str(), lPort, &hints, &mSocketAddr);
	if (0 != iResult)
	{
		std::cerr << "Unable to get socket information" << std::endl;
		WSACleanup();
		return false;
	}
	mSocket = socket(mSocketAddr->ai_family, mSocketAddr->ai_socktype, mSocketAddr->ai_protocol);
	if (mSocket == INVALID_SOCKET)
	{
		std::cerr << "Unable to open socket" << WSAGetLastError() << std::endl;
		return false;
	}
	iResult = connect(mSocket, mSocketAddr->ai_addr, (int)mSocketAddr->ai_addrlen);
	if (SOCKET_ERROR == iResult)
	{
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
	if (INVALID_SOCKET == mSocket)
	{
		std::cerr << "Unable to connect to the server" << std::endl;
		WSACleanup();
		return false;
	}
	return true;
}


/**
* @brief InternalClose This function has to be overloaded with socket-depend close process.
* @return True if the socket has been closed, false if the socket wasn't opened.
*/
bool NCamWinClient::InternalClose()
{
	if (!IsConnected())
		return false;
	int iResult = closesocket(mSocket);
	if (iResult == SOCKET_ERROR)
	{
		std::cerr << "Unable to close socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		mSocket = INVALID_SOCKET;
		return false;
	}
	WSACleanup();
	mSocket = INVALID_SOCKET;
	return true;
}


/**
* @brief InternalRead Read a buffer of bytes.
* @param data The buffer used to store the read bytes.
* @param maxlen The maximum size allowed for the data buffer.
* @return The number of bytes read.
*/
ssize_t NCamWinClient::InternalRead(uint8_t *data, const size_t &maxlen)
{
	ssize_t lSizeRead = -1;
	lSizeRead = recv(mSocket, (char*)data, (int)maxlen, 0);
	if (lSizeRead >= 0)
		return lSizeRead;
	else
		std::cerr << "Reception failed: " << WSAGetLastError() << std::endl;
	lSizeRead = -1;
	//exit(1);
	return lSizeRead;
}


/**
* @brief InternalWrite Write to the socket (for communication to the server).
* @param data The buffer to write through the socket.
* @param maxlen The size of the buffer.
* @return The number of bytes written, or -1 if the writting failed.
*/
ssize_t NCamWinClient::InternalWrite(const uint8_t *data, const size_t &maxlen)
{
	ssize_t lSizeWritten = send(mSocket, (char*)data, (int)maxlen, 0);
	if (SOCKET_ERROR == lSizeWritten)
	{
		std::cerr << "Sending failed: " << WSAGetLastError() << std::endl;
		closesocket(mSocket);
		WSACleanup();
		return -1;
	}
	return lSizeWritten;
}




bool NCamWinClient::OnCheckDataFromClient()
{
	bool IsASucces = false;


	if (this->IsConnected())
	{
		if (this->DoStreaming())
		{

			const Packets_t& lPackets = this->MapPackets(IsASucces);

			if (IsASucces)
			{

				//Get the CamTrack Packet.
				{
					NcDataStreamBase::PacketType_t lSearchedPacket = NcDataStreamBase::CameraTracking;
					auto lIter = lPackets.find(lSearchedPacket);
					if (lIter != lPackets.end())
					{
						const NcDataStreamCamTrack* lPacket = static_cast<const NcDataStreamCamTrack*> (lIter->second);
						mTrackingPacket[0] = mTrackingPacket[1];
						mTrackingPacket[1] = lPacket->GetData();
					}
				}

				//Get optical parameters if any
				{
					NcDataStreamBase::PacketType_t lSearchedPacket = NcDataStreamBase::OpticalParameters;
					auto lIter = lPackets.find(lSearchedPacket);
					if (lIter != lPackets.end())
					{
						const NcDataStreamOpticalParameters* lPacket = static_cast<const NcDataStreamOpticalParameters*> (lIter->second);
						//Lets Give this to the Renderer
						mOpticalPacket[0] = mOpticalPacket[1];
						mOpticalPacket[1] = lPacket->GetData();	// copying current data
					}
				}

				//Get DistortMap if any
				{
					NcDataStreamBase::PacketType_t lSearchedPacket = NcDataStreamBase::DistortMap;
					auto lIter = lPackets.find(lSearchedPacket);
					if (lIter != lPackets.end())
					{
						const NcDataStreamDistortMap* lPacket = static_cast<const NcDataStreamDistortMap*> (lIter->second);
						//Lets Give this to the Renderer
						mDistortMap[0].FromPtr(lPacket->ToPtr(), lPacket->GetSizeInBytes());
						mDistortMap[1].FromPtr(lPacket->ToPtr(), lPacket->GetSizeInBytes());
					}
				}


				// Once we finished to use the data, we give them back to the client
				this->UnmapPackets();
			}
		}
	}


	return IsASucces;

}