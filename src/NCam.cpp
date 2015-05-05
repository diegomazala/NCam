
#include "NCam.h"
#include "NCamClient.h"

#ifdef NCAM_QT_LIB
#include "NCamQtClient.h"
#elif NCAM_WIN_LIB
#include "NCamWinClient.h"
#endif


#include <Windows.h>

#include "ClockTime.h"

#ifdef USE_DISTORT_MAP
	#include "GL/GL.h"
	#pragma comment (lib, "Opengl32.lib")
	#define GL_CLAMP_TO_EDGE 0x812F
	#define GL_RG            0x8227
	#define GL_RG32F         0x8230
#endif

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "NcDataStreamCamTrackHelper.h"
#include "NcDataStreamOpticalParametersHelper.h"


struct LensSample
{
	double zoom;
	double focus;
	double iris;
	double fov;
	std::vector<double> distortion;

	LensSample() : zoom(-1.0), focus(-1.0), iris(-1.0), fov(-1.0)
	{}

	LensSample(double z, double f, double i, double _fov) : zoom(z), focus(f), iris(i), fov(_fov)
	{}

	friend std::ostream& operator << (std::ostream& os, const LensSample& l)
	{
		os << std::fixed << l.zoom << '\t' << l.focus << '\t' << l.iris << '\t' << l.fov << '\t';
		for (auto d = l.distortion.begin(); d != l.distortion.end(); ++d)
			os << std::fixed << *d << '\t';
		return os;
	}
};

typedef std::map<int, LensSample> FocusMap;
typedef std::map<int, FocusMap> ZoomMap;
FocusMap gFocusMap;
ZoomMap gLensSamples;
bool lensBuilding = false;



unsigned int	FieldIndex = 0;
NCamClient*		gpNCamClient = nullptr;



extern "C"
{	

	

	static char IpAddress[32];
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
	


	NCAM_API bool NCamOpen(const char* ip_address, unsigned int ip_port)
	{
		if (ncamLogFile.is_open())
			ncamLogFile.close();

		ncamLogFile.open("NCam.log", std::ios::out);


		if (!gpNCamClient->AskForPackets(gpNCamClient->GetPacketType()))
		{
			ncamLogFile << "AskForPackets Failed : Unable to get the capabilities : " << ip_address << " " << ip_port << std::endl;
			return false;
		}
		else
		{
			ncamLogFile << "AskForPacket Successfully: " << gpNCamClient->GetPacketType() << std::endl;
		}

		ncamLogFile << "NCamOpen " << ip_address << " " << ip_port << std::endl;

		gpNCamClient->Connect(ip_address, ip_port);
		
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



	NCAM_API bool NCamUpdateDistortMap(unsigned int distort_tex_id)
	{
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



	NCAM_API bool LensTableBuilder()
	{
		lensBuilding = true;

	

		lensBuilding = false;

		return false;
	}



	
	//typedef std::map<int, LensSample> FocusMap;
	//typedef std::map<int, FocusMap> ZoomMap;

	NCAM_API bool NCamCreateLensTable(int zoomSamples, int focusSamples)
	{
		gLensSamples.clear();

		// in order to have integer keys for the maps, we move the normalization from [0,1] to [0,100]
		int resolution = 100;


		int zoomStep = resolution / zoomSamples;
		int focusStep = resolution / focusSamples;


		for (int z = 0; z <= zoomStep * zoomSamples; z += zoomStep)
		{
			FocusMap lFocusMap;

			for (int f = 0; f <= focusStep * focusSamples; f += focusStep)
			{
				double zz = (double)z / resolution;
				double ff = (double)f / resolution;
				lFocusMap.insert( std::make_pair(f, LensSample()) );
			}

			gLensSamples.insert( std::make_pair( z,  lFocusMap) );
		}

		return false;
	}

	void printFocus(const std::pair<int, LensSample>& l) 
	{
		std::cout << '\t' << l.first << '\t' << l.second.fov << "   (" << l.second.zoom << ", " << l.second.focus << ")" << std::endl;
	}

	void printZoom(const std::pair<int, FocusMap>& l) 
	{
		const FocusMap& f = l.second;
		std::cout << l.first << std::endl;

		std::for_each(f.begin(), f.end(), printFocus);
	}


	NCAM_API void NCamPrintLensTable()
	{
		std::for_each(gLensSamples.begin(), gLensSamples.end(), printZoom);
	}


	NCAM_API bool NCamUpdateLensSample(double z, double f, double i, double _fov, int resolution)
	{
		int zoom = z * resolution;
		int focus = f * resolution;

		auto itZoom = gLensSamples.find(zoom);
		if (itZoom == gLensSamples.end())	// not found
			itZoom = gLensSamples.find(zoom - 1);	// try a neighbor value 
		if (itZoom == gLensSamples.end())	// not found
			itZoom = gLensSamples.find(zoom + 1);	// try a neighbor value 


		if (itZoom != gLensSamples.end())	// zoom sample found
		{
			
			FocusMap& focusMap = itZoom->second;	// get focus map
			
			auto itFocus = focusMap.find(focus);
			if (itFocus == focusMap.end())	// not found
				itFocus = focusMap.find(focus - 1); // try a neighbor value 
			if (itFocus == focusMap.end())	// not found
				itFocus = focusMap.find(focus + 1); // try a neighbor value 


			if (itFocus != focusMap.end())	// focus sample found
			{
				LensSample& l = itFocus->second;	// get LensSample

				int diffZoomActual = abs(itZoom->first - z * resolution);
				int diffZoomSample = abs(itZoom->first - l.zoom * resolution);

				int diffFocusActual = abs(itFocus->first - f * resolution);
				int diffFocusSample = abs(itFocus->first - l.focus * resolution);


				// if sample is not initialized or it has a closer encoder value than previous one : update it
				if ((l.fov < 0) || (diffZoomActual <= diffZoomSample && diffFocusActual <= diffFocusSample))
				{
					l.zoom = z;
					l.focus = f;
					l.iris = i;
					l.fov = _fov;
					return true;
				}

			}
			else
			{
				// focus sample NOT found
				return false;
			}
		}
		else
		{
			// zoom sample NOT found
			return false;
		}

		return false;
	}




	NCAM_API bool NCamSaveLensTable()
	{
		try
		{
			std::ofstream outputFile("NCamLensFile.txt", std::ofstream::out);

#if 1
			const FocusMap& focusMap = gLensSamples.begin()->second;

			outputFile << "ZoomSamples " << gLensSamples.size() << std::endl;
			outputFile << "FocusSamples " << focusMap.size() << std::endl;

			// write zoom samples
			for (auto itZoom = gLensSamples.begin(); itZoom != gLensSamples.end(); ++itZoom)
				outputFile << itZoom->first << '\t';
			
			outputFile << std::endl;
			
			// write focus samples
			
			for (auto itFocus = focusMap.begin(); itFocus != focusMap.end(); ++itFocus)
				outputFile << itFocus->first << '\t';
			
			outputFile << std::endl;
			
			// write fov and distortion samples
			for (auto itZoom = gLensSamples.begin(); itZoom != gLensSamples.end(); ++itZoom)
			{
				const FocusMap& focusMap = itZoom->second;
				for (auto itFocus = focusMap.begin(); itFocus != focusMap.end(); ++itFocus)
				{
					outputFile << itFocus->second << '\t';

					float* imgptr = (float*)gpNCamClient->DistortMap(FieldIndex).GetImagePtr();
					uint32_t lWidth, lHeight;
					gpNCamClient->DistortMap(FieldIndex).GetSize(lWidth, lHeight);
					int size = lWidth * lHeight * 2 ;	// 2 channels
					for (int i=0; i<size; ++i)
						outputFile << std::fixed << imgptr[i]  << '\t';
					outputFile << std::endl;
				}
			}

#else
			for (auto itZoom = gLensSamples.begin(); itZoom != gLensSamples.end(); ++itZoom)
			{
				const FocusMap& focusMap = itZoom->second;
				for (auto itFocus = focusMap.begin(); itFocus != focusMap.end(); ++itFocus)
				{
					outputFile << itZoom->first << '\t' << itFocus->first << '\t' << itFocus->second << std::endl;
				}
			}
#endif

			outputFile.close();
		}
		catch(const std::exception& ex)
		{
			ncamLogFile << "<Error> trying to save lens table: " << ex.what() << std::endl;
			return false;
		}

		return true;

	}

};