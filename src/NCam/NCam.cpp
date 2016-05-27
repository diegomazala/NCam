
#include "NCam.h"
#include "NCamClient.h"

#ifdef NCAM_QT_LIB
#include "NCamQtClient.h"
#elif NCAM_WIN_LIB
#include "NCamWinClient.h"
#endif

#include <Windows.h>
#include <fstream>
#include "ClockTime.h"

#ifdef USE_DISTORT_MAP
	#include "GL/GL.h"
	#pragma comment (lib, "Opengl32.lib")
	#define GL_CLAMP_TO_EDGE 0x812F
	#define GL_RG            0x8227
	#define GL_RG32F         0x8230
#endif


#include "NcDataStreamCamTrackHelper.h"
#include "NcDataStreamOpticalParametersHelper.h"


unsigned int	FieldIndex = 0;
NCamClient*		gpNCamClient = nullptr;
GLuint			distortMapId = 0;

extern "C"
{	

	static std::string IpAddress;
	static unsigned int Port = 38860;

	std::ofstream ncamLogFile; 


	NCAM_API unsigned int NCamErrorCode()
	{
		if (gpNCamClient != nullptr)
			return gpNCamClient->GetErrorCode();
		else
			return NCamClient::EUnknown;
	}


	NCAM_API void NCamResetError()
	{
		if (gpNCamClient != nullptr)
			gpNCamClient->ResetError();
	}


	NCAM_API void NCamCreate()
	{
		if (gpNCamClient != nullptr)
		{
			delete gpNCamClient;
			gpNCamClient = nullptr;
		}

		ncamLogFile << "Creating NCamClient... " << std::endl;

#ifdef NCAM_QT_LIB
		gpNCamClient = new NCamQtClient();
#elif NCAM_WIN_LIB
		gpNCamClient = new NCamWinClient();
#endif
	}


	NCAM_API void NCamDestroy()
	{
		ncamLogFile << "Destroying NCamClient... " << std::endl;
		if (gpNCamClient != nullptr)
		{
			delete gpNCamClient;
			gpNCamClient = nullptr;
		}
	}


	NCAM_API void NCamSetPacketType(bool cameraTracking, bool opticalParamters, bool distortMap)
	{
		NcDataStreamBase::PacketType_t lPacketType = NcDataStreamBase::UnknownType;
		if (cameraTracking)
			lPacketType = ( lPacketType | NcDataStreamBase::CameraTracking);
		if (opticalParamters)
			lPacketType = ( lPacketType | NcDataStreamBase::OpticalParameters);
		if (distortMap)
			lPacketType = ( lPacketType | NcDataStreamBase::DistortMap);

		if (gpNCamClient != nullptr)
			gpNCamClient->SetPacketType(lPacketType);

		ncamLogFile << "NCam Packet Type: " << lPacketType << std::endl;
	}
	
	NCAM_API void NCamSetIpAddress(const char* ip_address, unsigned int ip_port)
	{
		IpAddress = ip_address;
		Port = ip_port;
	}

	
	//NCAM_API bool NCamOpen(const char* ip_address, unsigned int ip_port)
	NCAM_API bool NCamOpen()
	{
		if (ncamLogFile.is_open())
			ncamLogFile.close();

		ncamLogFile.open("NCam.log", std::ios::out);


		if (!gpNCamClient->AskForPackets(gpNCamClient->GetPacketType()))
		{
			ncamLogFile << "AskForPackets Failed : Unable to get the capabilities : " << IpAddress << " " << Port << std::endl;
			return false;
		}
		else
		{
			ncamLogFile << "AskForPacket Successfully: " << gpNCamClient->GetPacketType() << std::endl;
		}

		ncamLogFile << "NCamOpen " << IpAddress << " " << Port << std::endl;

		gpNCamClient->Connect(IpAddress, Port);
		
		gpNCamClient->TrackingPacket(0).mTimeCode = gpNCamClient->TrackingPacket(1).mTimeCode = 0;

		//return gpNCamClient->IsConnected();
		return true;
	}


	NCAM_API bool NCamIsOpen()
	{
		if (gpNCamClient != nullptr)
			return gpNCamClient->IsConnected();
		else
			return false;
	}


	NCAM_API void NCamClose()
	{		
		if (gpNCamClient != nullptr)
		{
			gpNCamClient->Disconnect();

			ncamLogFile << "Field Lost Count: " << gpNCamClient->FieldLostCount() << std::endl;
			ncamLogFile << "Frame Lost Count: " << gpNCamClient->FrameLostCount() << std::endl;
			ncamLogFile << "NCamClose " << std::endl;
		}
			
		ncamLogFile.close();
	}


	NCAM_API void NCamFieldIndex(unsigned int field_index)
	{
		FieldIndex = field_index;
	}


	NCAM_API double NCamFrameRate()
	{
		if (gpNCamClient != nullptr)
			return gpNCamClient->FrameRate();
		else
			return 0.0;
	}

	NCAM_API int NCamFrameLostCount()
	{
		if (gpNCamClient != nullptr)
			return gpNCamClient->FrameLostCount();
		else
			return -1;
	}

	NCAM_API int NCamFieldLostCount()
	{
		if (gpNCamClient != nullptr)
			return gpNCamClient->FieldLostCount();
		else
			return -1;
	}


	NCAM_API bool NCamUpdate()
	{
		bool IsASucces = false;

		if (gpNCamClient != nullptr)
		{
			clk::Time t;
			t.Start();

			unsigned int lastTrackingTimeCode = gpNCamClient->TrackingPacket(1).mTimeCode;
			IsASucces = gpNCamClient->Update();
			
			//std::cout << gpNCamClient->OpticalPacket(0).mTimeCode << " " << gpNCamClient->OpticalPacket(1).mTimeCode << " - " 
			//	<< gpNCamClient->TrackingPacket(0).mTimeCode << " " << gpNCamClient->TrackingPacket(1).mTimeCode << std::endl;

			t.Stop();
		}

		return IsASucces;
	}



	NCAM_API void NCamGetData(	void* pArrayUInt6_OpticalTimeCode, 
								void* pArrayUInt6_TrackingTimeCode,
								void* pArrayDouble6_EncoderParameters,
								void* pArrayDouble13_OpticalParameters,
								void* pArrayDouble12_TrackingParameters,
								void* pArrayDouble16_GLProjectionMatrix,
								void* pArrayDouble16_GLModelViewMatrix,
								double near_plane, double far_plane)
	{
		if (pArrayUInt6_OpticalTimeCode != nullptr)
			NCamOpticalTimeCode(pArrayUInt6_OpticalTimeCode);

		if (pArrayUInt6_TrackingTimeCode != nullptr)
			NCamTrackingTimeCode(pArrayUInt6_TrackingTimeCode);

		if (pArrayDouble6_EncoderParameters != nullptr)
			NCamEncoderParameters(pArrayDouble6_EncoderParameters);

		if (pArrayDouble13_OpticalParameters != nullptr)
			NCamOpticalParameters(pArrayDouble13_OpticalParameters);

		if (pArrayDouble12_TrackingParameters != nullptr)
			NCamTrackingParameters(pArrayDouble12_TrackingParameters);

		if (pArrayDouble16_GLProjectionMatrix != nullptr && pArrayDouble16_GLModelViewMatrix != nullptr)
			NCamGLMatrices(pArrayDouble16_GLProjectionMatrix, pArrayDouble16_GLModelViewMatrix, near_plane, far_plane);
			
	}
		


	NCAM_API unsigned int NCamOpticalTimeCode(void* pArrayUInt6)
	{
		unsigned int* pArrayUInt = (unsigned int*) pArrayUInt6;

		// safeguard - pointer must be not null
		if(!pArrayUInt)
			return 0;

		uint32_t lHours, lMinutes, lSeconds, lFrames;
		uint8_t lDropFrame;

		NcDataStreamCamHelper::DecodeTimeCode(gpNCamClient->OpticalPacket(FieldIndex).mTimeCode, lHours, lMinutes, lSeconds, lFrames, lDropFrame);
		pArrayUInt[0] = lHours;
		pArrayUInt[1] = lMinutes;
		pArrayUInt[2] = lSeconds;
		pArrayUInt[3] = lFrames;
		pArrayUInt[4] = lDropFrame;
		pArrayUInt[5] = gpNCamClient->TrackingPacket(FieldIndex).mTimeCode;

		return gpNCamClient->TrackingPacket(FieldIndex).mTimeCode;
	}


	NCAM_API unsigned int NCamTrackingTimeCode(void* pArrayUInt6)
	{
		unsigned int* pArrayUInt = (unsigned int*) pArrayUInt6;

		// safeguard - pointer must be not null
		if(!pArrayUInt)
			return 0;

		uint32_t lHours, lMinutes, lSeconds, lFrames;
		uint8_t lDropFrame;

		NcDataStreamCamHelper::DecodeTimeCode(gpNCamClient->TrackingPacket(FieldIndex).mTimeCode, lHours, lMinutes, lSeconds, lFrames, lDropFrame);
		pArrayUInt[0] = lHours;
		pArrayUInt[1] = lMinutes;
		pArrayUInt[2] = lSeconds;
		pArrayUInt[3] = lFrames;
		pArrayUInt[4] = lDropFrame;
		pArrayUInt[5] = gpNCamClient->TrackingPacket(FieldIndex).mTimeCode;

		return gpNCamClient->TrackingPacket(FieldIndex).mTimeCode;
	}


	NCAM_API double NCamFovHorizontal()
	{
		return gpNCamClient->OpticalPacket(FieldIndex).mFovInDegrees[0];
	}


	NCAM_API double NCamFovVertical()
	{
		return gpNCamClient->OpticalPacket(FieldIndex).mFovInDegrees[1];
	}

	NCAM_API void NCamEncoderParameters(void* pArrayDouble6)
	{
		if (gpNCamClient == nullptr)
			return;

		double* pArrayDouble = (double*) pArrayDouble6;
		
		// safeguard - pointer must be not null
		if(!pArrayDouble)
			return;

		pArrayDouble[0] = gpNCamClient->TrackingPacket(FieldIndex).mZoomEncoder.mNormalized;
		pArrayDouble[1] = gpNCamClient->TrackingPacket(FieldIndex).mZoomEncoder.mMappedValue;
		pArrayDouble[2] = gpNCamClient->TrackingPacket(FieldIndex).mFocusEncoder.mNormalized;		
		pArrayDouble[3] = gpNCamClient->TrackingPacket(FieldIndex).mFocusEncoder.mMappedValue;
		pArrayDouble[4] = gpNCamClient->TrackingPacket(FieldIndex).mIrisEncoder.mNormalized;
		pArrayDouble[5] = gpNCamClient->TrackingPacket(FieldIndex).mIrisEncoder.mMappedValue;
	}


	NCAM_API void NCamOpticalParameters(void* pArrayDouble13)
	{
		int i;

		if (gpNCamClient == nullptr)
			return;

		double* pArrayDouble = (double*) pArrayDouble13;

		// safeguard - pointer must be not null
		if(!pArrayDouble)
			return;

		pArrayDouble[0] = gpNCamClient->OpticalPacket(FieldIndex).mFovInDegrees[0];
		pArrayDouble[1] = gpNCamClient->OpticalPacket(FieldIndex).mFovInDegrees[1];
		pArrayDouble[2] = gpNCamClient->OpticalPacket(FieldIndex).mProjectionCenterNormalized[0];
		pArrayDouble[3] = gpNCamClient->OpticalPacket(FieldIndex).mProjectionCenterNormalized[1];
		pArrayDouble[4] = gpNCamClient->OpticalPacket(FieldIndex).mImageSensorSizeInMm[0];
		pArrayDouble[5] = gpNCamClient->OpticalPacket(FieldIndex).mImageSensorSizeInMm[1];
		pArrayDouble[6] = gpNCamClient->OpticalPacket(FieldIndex).mImageResolution[0];
		pArrayDouble[7] = gpNCamClient->OpticalPacket(FieldIndex).mImageResolution[1];
		pArrayDouble[8] = gpNCamClient->OpticalPacket(FieldIndex).mImageAspectRatio;
		pArrayDouble[9] = gpNCamClient->OpticalPacket(FieldIndex).mTop;
		pArrayDouble[10] = gpNCamClient->OpticalPacket(FieldIndex).mBottom;
		pArrayDouble[11] = gpNCamClient->OpticalPacket(FieldIndex).mLeft;
		pArrayDouble[12] = gpNCamClient->OpticalPacket(FieldIndex).mRight;
	}


	NCAM_API void NCamTrackingParameters(void* pArrayDouble12)
	{
		int i;

		if (gpNCamClient == nullptr)
			return;

		double* pArrayDouble = (double*) pArrayDouble12;

		// safeguard - pointer must be not null
		if(!pArrayDouble)
			return;

		for (i = 0; i < 9; ++i)
		{
			pArrayDouble[i] = gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mRotation[i];
		}

		for (i = 0; i < 3; ++i)
		{
			pArrayDouble[9 + i] = gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mTranslation[i];
		}
	}


	NCAM_API void NCamGLMatrices(void* pArrayDouble16_Projection, void* pArrayDouble16_ModelView, double near_plane, double far_plane)
	{
		if (pArrayDouble16_Projection != nullptr)
		{
			double* pPrjMatrix = (double*) pArrayDouble16_Projection;
			NcDataStreamOpticalParametersHelper::GetProjectionMatrix(gpNCamClient->OpticalPacket(FieldIndex), &(*pPrjMatrix), near_plane, far_plane);
		}

		if (pArrayDouble16_ModelView != nullptr)
		{
			double* pMdvMatrix = (double*) pArrayDouble16_ModelView;
			NcDataStreamCamHelper::ToOpenGLModelView(gpNCamClient->TrackingPacket(FieldIndex), &(*pMdvMatrix));
		}
	}

	NCAM_API void NCamGLProjectionMatrix(void* pArrayDouble16_Projection, double near_plane, double far_plane)
	{
		if (pArrayDouble16_Projection != nullptr)
		{
			double* pPrjMatrix = (double*)pArrayDouble16_Projection;
			NcDataStreamOpticalParametersHelper::GetProjectionMatrix(gpNCamClient->OpticalPacket(FieldIndex), &(*pPrjMatrix), near_plane, far_plane);
		}
	}


	NCAM_API void NCamGLModelViewMatrix(void* pArrayDouble16)
	{
		if (gpNCamClient == nullptr)
			return;

		double* pArrayDouble = (double*) pArrayDouble16;

		// safeguard - pointer must be not null
		if(!pArrayDouble)
			return;

		unsigned int i = 0;
		unsigned int j = 0;

		// Set Matrix To Identity()
		for (i = 0; i<4; ++i)
		{
			for (j = 0 ; j<4; ++j)
			{
				pArrayDouble [i *4 + j] = (i == j)?1.0 : 0.0;
			}
		}
		// Rotation Part
		for (i = 0; i<3; ++i)
		{
			for (j = 0; j<3; ++j)
			{
				pArrayDouble[i*4 + j] = gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mRotation[i*3 + j];
			}
		}

		for (j = 0; j<3; ++j)
		{
			pArrayDouble[12 + j] = gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mTranslation[j];
		}
	}


	NCAM_API void NCamPrintData()
	{
		std::cout << std::endl
			<< "TimeCode: " << gpNCamClient->TrackingPacket(FieldIndex).mTimeCode << std::endl
			<< "Fov(x,y): " << gpNCamClient->OpticalPacket(FieldIndex).mFovInDegrees[0] << ", " << gpNCamClient->OpticalPacket(FieldIndex).mFovInDegrees[1] << std::endl
			<< "Offset(x,y): " << gpNCamClient->OpticalPacket(FieldIndex).mProjectionCenterNormalized[0] << ", " << gpNCamClient->OpticalPacket(FieldIndex).mProjectionCenterNormalized[1] << std::endl;

		std::cout << "Rotation: ";
		for (int i=0; i<9; ++i)
		{
			if (i%3 == 0)
				std::cout << std::endl;
			std::cout << gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mRotation[i] << "  ";
		}

		std::cout << "\nTranslation: " << std::endl
			<< gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mTranslation[0] << ", " 
			<< gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mTranslation[1] << ", " 
			<< gpNCamClient->TrackingPacket(FieldIndex).mCamFromWorld.mTranslation[2] 
			<< std::endl;
	}


	NCAM_API void NCamSetDistortMapPtr(void* distort_tex_ptr)
	{
		distortMapId = (GLuint)(size_t)(distort_tex_ptr);
	}


	NCAM_API bool NCamUpdateDistortMap()
	{
		GLuint distort_tex_id = distortMapId;
		GLint lInternalFormat = 0;
		GLenum lFormat = 0;
		GLenum lType = 0;

		if (distort_tex_id == 0)
		{
			glGenTextures(1, &distort_tex_id);
			glBindTexture(GL_TEXTURE_2D, distort_tex_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, distort_tex_id);
		}


		if (NcDataStreamDistortMap::Nc2Channels == gpNCamClient->DistortMap(FieldIndex).GetChannels() && NcDataStreamDistortMap::OneFloatPerPixel == gpNCamClient->DistortMap(FieldIndex).GetDepth())
		{
			lInternalFormat = GL_RG32F;
			lFormat = GL_RG;
			lType = GL_FLOAT;
		}
		else
		{
			std::cerr << "NcDataStreamDistortMap -> the image format is not implemented Yet "<< __FUNCTION__ << __LINE__;
			ncamLogFile << "NcDataStreamDistortMap -> the image format is not implemented Yet "<< __FUNCTION__ << __LINE__ << std::endl;
			return false;
		}
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gpNCamClient->DistortMap(FieldIndex).GetWidth(), gpNCamClient->DistortMap(FieldIndex).GetHeight(), lFormat, lType, (const GLvoid*)gpNCamClient->DistortMap(FieldIndex).GetImagePtr());
		
		return true;
	}

	NCAM_API int NCamDistortMapWidth()
	{
		return gpNCamClient->DistortMap(FieldIndex).GetWidth();
	}

	NCAM_API int NCamDistortMapHeight()
	{
		return gpNCamClient->DistortMap(FieldIndex).GetHeight();
	}

	NCAM_API int NCamDistortMapChannelsCount()
	{
		return gpNCamClient->DistortMap(FieldIndex).GetChannels();	// 2
	}

	NCAM_API float* NCamDistortMapDataPtr()
	{
		return (float*)gpNCamClient->DistortMap(FieldIndex).GetImagePtr();
	}

	NCAM_API void NCamLensSample(double& zoom, double& focus, double& iris, double& fov, void* distortionMapFloatArray)
	{
		if (gpNCamClient == nullptr)
			return;

		if (distortionMapFloatArray == nullptr)
			return;

		float* distortionMap = (float*)distortionMapFloatArray;

		// safeguard - pointer must be not null
		if (!distortionMap)
			return;

		zoom	= (float)gpNCamClient->TrackingPacket(FieldIndex).mZoomEncoder.mNormalized;
		focus	= (float)gpNCamClient->TrackingPacket(FieldIndex).mFocusEncoder.mNormalized;
		iris	= (float)gpNCamClient->TrackingPacket(FieldIndex).mIrisEncoder.mNormalized;
		fov		= (float)gpNCamClient->OpticalPacket(FieldIndex).mFovInDegrees[1];

		float* imgptr = (float*)gpNCamClient->DistortMap(FieldIndex).GetImagePtr();
		uint32_t lWidth, lHeight;
		gpNCamClient->DistortMap(FieldIndex).GetSize(lWidth, lHeight);
		int size = lWidth * lHeight * 2;	// 2 channels
		
		for (int i = 0; i < size; ++i)
			distortionMap[i] = imgptr[i];
	}

};