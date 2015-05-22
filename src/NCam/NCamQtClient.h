#ifndef _NCAM_CLIENT_QT_H_
#define _NCAM_CLIENT_QT_H_

#include "NCamClient.h"
#include "NCamQtGlobal.h"
#include <QThread>
#include <NcDataStreamClientBase.h>
#include "NcThreadSafeTools.h"

class QTcpSocket;
class QTime;

class NCAMQT_EXPORT NCamQtClient : public QThread, public NCamClient
{
	Q_OBJECT

public:

	enum ErrorCode
	{
		ENone,
		EStartStreaming,
		EStopStreaming,
		EDoStreaming
	};

	NCamQtClient();
	~NCamQtClient();


	void Connect(const std::string ipAddress, int port);
	void Disconnect();

	virtual bool IsConnected() const;

	void Exec();
	void StopExec();

	bool IsNewDataToMap();
	bool WaitForNewDataToMap(unsigned long luiTime);
	std::pair<Packets_t,Packets_t>& MapFrameData(bool &lIsASuccess, bool lWaitForNewData, unsigned long luiTime);
	void UnMapFrameData();

	

signals:
	void StartStreamingError(QString lDescription);
	void StopStreamingError(QString lDescription);
	void DoStreamingError(QString lDescription);

protected:

	bool OnCheckDataFromClient();

	//Error Handling
	virtual void OnStartStreamingError(const std::string& lErrorDecription = std::string());
	virtual void OnStopStreamingError(const std::string& lErrorDecription = std::string());
	virtual void OnDoStreamingError(const std::string& lErrorDecription = std::string());

	// This is the method called in the thread
	virtual void run();

	virtual bool InternalOpen();
	virtual bool InternalClose();

	virtual ssize_t InternalRead(uint8_t *data, const size_t& maxlen);
	virtual ssize_t InternalWrite(const uint8_t *data, const size_t& maxlen);

	virtual Packets_t& MapPackets(bool& lSuccess) ;
	virtual void UnmapPackets();

	void ComputeStats(unsigned int lMinNbSample = 50);
	void AckOnePacket();

	

private:
	QTcpSocket* mpTcpSocket;

	//Should be protected by a mutex
	NcThreadSafeResource<bool> mbThreadShouldRun;
	NcDataBufferSwap<std::pair<Packets_t,Packets_t>> mFrameDataBufferSwap;

	// No need to protect the following data as there is only one thread that will access to this.
	std::pair<Packets_t,Packets_t>*  mCachedPackets;
	uint32_t mLastPacketId;

	QTime *mpCurrTime;
	quint32 mNbPackets;
	float mSamplesPerSecond;

	ErrorCode mErrorCode;
};

#endif // _NCAM_CLIENT_QT_H_
