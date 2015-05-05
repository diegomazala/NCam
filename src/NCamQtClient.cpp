#include "NCamQtClient.h"
#include <QTimer>
#include <QTcpSocket>
#include <QTime>
#include <iostream>


NCamQtClient::NCamQtClient():
	QThread(),
	NCamClient(),
	mpTcpSocket(nullptr),
	mbThreadShouldRun(true),
	mFrameDataBufferSwap(),
	mCachedPackets(nullptr),
	mNbPackets(0),
	mSamplesPerSecond(0.0),
	mErrorCode(ENone)
{
	mpCurrTime = new QTime();
	mpCurrTime->start();

}

NCamQtClient::~NCamQtClient()
{
	StopExec();
}



void NCamQtClient::ComputeStats(unsigned int lMinNbSample)
{
	if (mNbPackets > lMinNbSample || mpCurrTime->elapsed()>2000)
	{
		float lSampleTimeInSeconds = mpCurrTime->elapsed()/1000.0;
		mSamplesPerSecond = (mNbPackets)/lSampleTimeInSeconds;
		mpCurrTime->restart();
		mNbPackets = 0;
		std::cout << "FPS: " << mSamplesPerSecond << std::endl;
	}
}

void NCamQtClient::AckOnePacket()
{
	mNbPackets++;
}


bool NCamQtClient::OnCheckDataFromClient()
{
	bool IsASucces = false;


	if (this->IsConnected())
	{
		if (this->IsNewDataToMap())
		{
			// This is only for the exercise, then we will display the values of the first field only
			
			const std::pair<Packets_t,Packets_t>& lFramePackets = this->MapFrameData(IsASucces, true, 1000);
			const Packets_t& lPackets = lFramePackets.first;

			if (IsASucces)
			{
				AckOnePacket();

				const Packets_t& lPacket_0 = lFramePackets.first;
				const Packets_t& lPacket_1 = lFramePackets.second;

				//Get the CamTrack Packet.
				{
					NcDataStreamBase::PacketType_t lSearchedPacket = NcDataStreamBase::CameraTracking;
					auto lIter_0 = lPacket_0.find(lSearchedPacket);
					if (lIter_0 !=lPacket_0.end())
					{
						const NcDataStreamCamTrack* lPacket = static_cast<const NcDataStreamCamTrack*> (lIter_0->second);
						if (lPacket != nullptr)
							mTrackingPacket[0] = lPacket->GetData();
					}

					auto lIter_1 = lPacket_1.find(lSearchedPacket);
					if (lIter_1 !=lPacket_1.end())
					{
						const NcDataStreamCamTrack* lPacket = static_cast<const NcDataStreamCamTrack*> (lIter_1->second);
						if (lPacket != nullptr)
							mTrackingPacket[1] = lPacket->GetData();
					}


				}

				//Get optical parameters if any
				{
					NcDataStreamBase::PacketType_t lSearchedPacket = NcDataStreamBase::OpticalParameters;
					auto lIter_0 = lPacket_0.find(lSearchedPacket);
					if (lIter_0 !=lPacket_0.end())
					{
						const NcDataStreamOpticalParameters* lPacket = static_cast<const NcDataStreamOpticalParameters*> (lIter_0->second);
						//Lets Give this to the Renderer
						mOpticalPacket[0] = lPacket->GetData();	// copying current data
					}
					auto lIter_1 = lPacket_1.find(lSearchedPacket);
					if (lIter_1 !=lPacket_1.end())
					{
						const NcDataStreamOpticalParameters* lPacket = static_cast<const NcDataStreamOpticalParameters*> (lIter_1->second);
						//Lets Give this to the Renderer
						mOpticalPacket[1] = lPacket->GetData();	// copying current data
					}
				}

				//Get DistortMap if any
				{
					NcDataStreamBase::PacketType_t lSearchedPacket = NcDataStreamBase::DistortMap;
					auto lIter_0 = lPacket_0.find(lSearchedPacket);
					if (lIter_0 !=lPacket_0.end())
					{
						const NcDataStreamDistortMap* lPacket = static_cast<const NcDataStreamDistortMap*> (lIter_0->second);
						//Lets Give this to the Renderer
						mDistortMap[0].FromPtr(lPacket->ToPtr(), lPacket->GetSizeInBytes());
					}
					auto lIter_1 = lPacket_1.find(lSearchedPacket);
					if (lIter_1 !=lPacket_1.end())
					{
						const NcDataStreamDistortMap* lPacket = static_cast<const NcDataStreamDistortMap*> (lIter_1->second);
						//Lets Give this to the Renderer
						mDistortMap[1].FromPtr(lPacket->ToPtr(), lPacket->GetSizeInBytes());
					}
				}
				// Once we finished to use the data, we give them back to the client
				this->UnMapFrameData();
			}
		}
	}
	
	ComputeStats();

	return IsASucces;
}


void NCamQtClient::Connect(const std::string ipAddress, int port)
{	
	//Set the connection Parameters
	this->SetConnectionParameters(ipAddress, port);
	this->Exec();

}


void NCamQtClient::Disconnect()
{
	if (this->IsConnected())
		this->StopExec();
}




void NCamQtClient::Exec()
{
	mbThreadShouldRun.SetValue(true);
	start();
}

void NCamQtClient::StopExec()
{
	if (isRunning())
	{
		mbThreadShouldRun.SetValue(false);
		wait();
	}
}

bool NCamQtClient::IsConnected() const
{
	return (nullptr != mpTcpSocket);
}

void NCamQtClient::run()
{
	mCachedPackets = nullptr;
	mLastPacketId = 1;
	bool lRetValue = false;
	lRetValue = StartStreaming();
	while(mbThreadShouldRun.GetValue() && lRetValue)
	{
		lRetValue = DoStreaming();
	}
	StopStreaming();
}


bool NCamQtClient::InternalOpen()
{
	if (IsConnected())
	{
		return false;
	}
	mpTcpSocket = new QTcpSocket();
	//Disable nagle's algo
	mpTcpSocket->setSocketOption(QAbstractSocket::LowDelayOption,true);
	mpTcpSocket->connectToHost(QString(GetIpAddress().c_str()),GetPort());
	if (mpTcpSocket->waitForConnected(1000))
	{
		return true;
	}
	delete(mpTcpSocket);
	mpTcpSocket = nullptr;
	return false;
}

bool NCamQtClient::InternalClose()
{
	if (!IsConnected())
	{
		return false;
	}
	//Ask the Thread to Stop;
	mpTcpSocket->close();
	delete(mpTcpSocket);
	mpTcpSocket = nullptr;
	return true;
}

ssize_t NCamQtClient::InternalRead(uint8_t *data, const size_t& maxlen)
{
	ssize_t lSizeRead = -1;
	if (IsConnected())
	{
		bool lSuccess = (mpTcpSocket->bytesAvailable()>0);
		if (false == lSuccess)
			lSuccess = mpTcpSocket->waitForReadyRead(1000);
		if (lSuccess)
		{
			lSizeRead = mpTcpSocket->read((char*)data, maxlen);
		}
	}
	return lSizeRead;
}

ssize_t NCamQtClient::InternalWrite(const uint8_t *data, const size_t& maxlen)
{
	ssize_t lSizeWritten = -1;
	if (IsConnected())
	{
		bool lSuccess = false;
		lSizeWritten = mpTcpSocket->write((char*)data, maxlen);
		if (lSizeWritten>0)
		{
			lSuccess = mpTcpSocket->waitForBytesWritten(1000);
		}
		if (false == lSuccess)
		{
			lSizeWritten = -1;
		}
	}
	return lSizeWritten;
}

Packets_t& NCamQtClient::MapPackets(bool& lSuccess)
{
	lSuccess = true;
	if (1 == mLastPacketId)
	{
		mLastPacketId = 0;
		mCachedPackets = &(mFrameDataBufferSwap.MapProductorBuffer());
		return mCachedPackets->first;
	}
	else
	{
		mLastPacketId = 1;
		return mCachedPackets->second;
	}
}

void NCamQtClient::UnmapPackets()
{
	if (1 == mLastPacketId)
	{
		mFrameDataBufferSwap.UnMapProductorBuffer();
		mCachedPackets = nullptr;
	}
}

bool NCamQtClient::IsNewDataToMap()
{
	return mFrameDataBufferSwap.IsNewProductAvailable();
}

bool NCamQtClient::WaitForNewDataToMap(unsigned long luiTime)
{
	return mFrameDataBufferSwap.WaitForNewProductAvailable(luiTime);
}

std::pair<Packets_t,Packets_t>& NCamQtClient::MapFrameData(bool& lIsASuccess, bool lWaitForNewData, unsigned long luiTime)
{
	return mFrameDataBufferSwap.MapConsumerBuffer(lIsASuccess,lWaitForNewData,luiTime);
}

void NCamQtClient::UnMapFrameData()
{
	mFrameDataBufferSwap.UnMapConsumerBuffer();
}

//Error Handling
void NCamQtClient::OnStartStreamingError(const std::string& lDescription)
{
	NCamClient::OnStartStreamingError(lDescription);
	emit StartStreamingError(lDescription.c_str());
}

void NCamQtClient::OnStopStreamingError(const std::string& lDescription)
{
	NCamClient::OnStopStreamingError(lDescription);
	emit StopStreamingError(lDescription.c_str());
}

void NCamQtClient::OnDoStreamingError(const std::string& lDescription)
{
	NCamClient::OnDoStreamingError(lDescription);
	emit DoStreamingError(lDescription.c_str());
}
