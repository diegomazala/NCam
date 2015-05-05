#ifndef _NCAM_CLIENT_H_
#define _NCAM_CLIENT_H_


#include <NcDataStreamClientBase.h>

class NCamClient : public NcDataStreamClientBase
{
public:

	enum ErrorCode
	{
		ENone,
		EStartStreaming,
		EStopStreaming,
		EDoStreaming,
		EUnknown
	};

	NCamClient();
	virtual ~NCamClient();

	uint32_t GetErrorCode() const {return mErrorCode;}
	void ResetError() {mErrorCode = ENone;}

	virtual void Connect(const std::string ipAddress, int port){}
	virtual void Disconnect(){}
	virtual bool IsConnected() const {return false;}

	float FrameRate() const {return mSamplesPerSecond;}

	virtual bool Update();
		

	void SetPacketType(NcDataStreamBase::PacketType_t packet_type) {mPacketType = packet_type;}
	NcDataStreamBase::PacketType_t GetPacketType() const {return mPacketType;}

	
	NcDataStreamCamTrack::TrackingPacket& TrackingPacket(int field) {return mTrackingPacket[field];}

	NcDataStreamOpticalParameters::OpticalParametersPacket& OpticalPacket(int field) {return mOpticalPacket[field];}

	NcDataStreamDistortMap& DistortMap(int field) {return mDistortMap[field];}

	int FieldInterval() const {return mFieldInterval;}
	int FrameInterval() const {return mFrameInterval;}
	int FieldLostCount() const {return mFieldLostCount;}
	int FrameLostCount() const {return mFrameLostCount;}

protected: // Methods

	virtual bool OnCheckDataFromClient() = 0;

	/**
	* @brief OnStartStreamingError overloaded error callback when opening the connection.
	* @param lErrorDecription The error message returned by the API.
	*/
	virtual void OnStartStreamingError(const std::string &lErrorDecription);


	/**
	* @brief OnStopStreamingError overloaded error callback when stoping the connection.
	* @param lErrorDecription The error message returned by the API.
	*/
	virtual void OnStopStreamingError(const std::string &lErrorDecription);


	/**
	* @brief OnDoStreamingError overloading error callback when sending/receiving packets through the socket.
	* @param lErrorDecription The error message returned by the API.
	*/
	virtual void OnDoStreamingError(const std::string &lErrorDecription);

protected:	// Attributes
	
	float mSamplesPerSecond;
	ErrorCode mErrorCode;

	unsigned int mFieldInterval;
	unsigned int mFrameInterval;
	unsigned int mFieldLostCount;
	unsigned int mFrameLostCount;

	NcDataStreamBase::PacketType_t	mPacketType;

	NcDataStreamCamTrack::TrackingPacket					mTrackingPacket[2];
	NcDataStreamOpticalParameters::OpticalParametersPacket	mOpticalPacket[2];
	NcDataStreamDistortMap									mDistortMap[2];
};

#endif // _NCAM_CLIENT_H_
