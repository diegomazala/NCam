
#include "NCamClient.h"
#include <iostream>


NCamClient::NCamClient(): 
						NcDataStreamClientBase(),
						mErrorCode(ENone),
						mFieldInterval(0),
						mFrameInterval(0),
						mFieldLostCount(0),
						mFrameLostCount(0)
{
}

NCamClient::~NCamClient()
{
}



bool NCamClient::Update()
{
	unsigned int lastTrackingTimeCode = mTrackingPacket[1].mTimeCode;

	bool success =  OnCheckDataFromClient();

	mFieldInterval = mTrackingPacket[1].mTimeCode - mTrackingPacket[0].mTimeCode;
	mFrameInterval = mTrackingPacket[1].mTimeCode - lastTrackingTimeCode;

	if (mFieldInterval != 1)
		++mFieldLostCount;

	if (mFrameInterval != 2)
		++mFrameLostCount;

	return success;
}




/**
* @brief OnStartStreamingError overloaded error callback when opening the connection.
* @param lErrorDecription The error message returned by the API.
*/
void NCamClient::OnStartStreamingError(const std::string &lErrorDecription)
{
	mErrorCode = EStartStreaming;
	std::cerr << std::endl << __FUNCTION__ << ": " << lErrorDecription << std::endl;
}


/**
* @brief OnStopStreamingError overloaded error callback when stoping the connection.
* @param lErrorDecription The error message returned by the API.
*/
void NCamClient::OnStopStreamingError(const std::string &lErrorDecription)
{
	mErrorCode = EStopStreaming;
	std::cerr << std::endl << __FUNCTION__ << ": " << lErrorDecription << std::endl;
}


/**
* @brief OnDoStreamingError overloading error callback when sending/receiving packets through the socket.
* @param lErrorDecription The error message returned by the API.
*/
void NCamClient::OnDoStreamingError(const std::string &lErrorDecription)
{
	mErrorCode = EDoStreaming;
	std::cerr << std::endl << __FUNCTION__ << ": " << lErrorDecription << std::endl;
}
