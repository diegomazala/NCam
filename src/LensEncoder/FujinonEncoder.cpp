#include <iostream>
#include <stdio.h>
#include "SerialPort.h"
#include "FujinonEncoder.h"

/////////////////////////////////////////////////////////////////////////////
//	Class cSerialData
/////////////////////////////////////////////////////////////////////////////

void cSerialData::printBuffer()
{
	printf ("\nBuffer in %d ", iLenght );
	for( int i = 0 ; i < iLenght ; i++)
	{
		printf (" c: %u ", ucTxBuffer[i] );
	}
	return;
}

void cSerialData::TxPutByte( unsigned char b )
{
	ucTxBuffer[iLenght] = b;   
	iLenght++;         
	ucSum = ucSum + b;  
	return;
}

void cSerialData::PutShortInt( unsigned short i )
{
	ucTxBuffer[iLenght] = (unsigned char) i;
	iLenght++;
	ucSum = ucSum + i;
	return;
}

void cSerialData::PutWord( unsigned short Data )
{
	unsigned char D1 ,D2;
	D1 = (unsigned char) ( Data >> 8 );
	D2 = (unsigned char) Data; 
	TxPutByte( D1 & 0xff );   
	TxPutByte( D2 & 0xff );
	return;
}

void cSerialData::ReqData( unsigned char Code )
{
	iLenght = 0;
	ucSum = 0;
	TxPutByte( 0x00 );
	TxPutByte( Code );
	PutShortInt( ~(ucSum & 0xff) + 1 );
	return;
}

void cSerialData::ReqLen2 ( unsigned char Code , unsigned short Data)
{
	iLenght = 0;
	ucSum = 0;
	TxPutByte( 0x02 );
	TxPutByte( Code );
	PutWord( Data );
	PutShortInt( ~(ucSum & 0xff) + 1 );
	return;
}

cSerialData::cSerialData()
{
	iLenght = 0;
	ucSum = 0;
	cleanBuffer(); 
}

cSerialData::~cSerialData()
{
	iLenght = 0;
}

void cSerialData::RequestIrisPos()
{
	ReqData(0x30);
	return;
}

void cSerialData::RequestZoomPos()
{
	ReqData(0x31);
	return;
}

void cSerialData::RequestFocusPos()
{
	ReqData(0x32);
	return;
}

void cSerialData::SetZoomPos( unsigned short usInVal )
{
	if( usInVal >= 0 && usInVal <= 65535)
	{
		ReqLen2 ( 0x21 , usInVal );
	}
	return;
}

void cSerialData::SetFocusPos( unsigned short usInVal )
{
	if( usInVal >= 0 && usInVal <= 65535)
	{
		ReqLen2 ( 0x22 , usInVal );
	}
	return;
}

void cSerialData::cleanBuffer() 
{ 
	for ( int i = 0; i < 19; i++)
	{
		ucTxBuffer[i] = 0;
	}
}

unsigned char * cSerialData::getTxBuffer()
{
	return ucTxBuffer;
}

int cSerialData::getLenght()
{
	return iLenght;
}

/////////////////////////////////////////////////////////////////////////////
//	Class FujinonEncoder
/////////////////////////////////////////////////////////////////////////////


FujinonEncoder::FujinonEncoder()
{
	iPortComm = 0;
	iLimit = 32;
	cleanBuffer();
}

FujinonEncoder::~FujinonEncoder()
{
	iPortComm = 0;
	finishConnection();
	cleanBuffer();
}

void FujinonEncoder::setPortNumber( int iComm )
{
	iPortComm = iComm; 
	return;
}

bool FujinonEncoder::initConnection()
{
	if( !mPort.Open( iPortComm, CBR_38400 ) )
	{
		std::cout << "ERROR: Could not open port - " << iPortComm << std::endl;
		std::cerr << "ERROR: Could not open port - " << iPortComm << std::endl;
		return false;
	}
	else
	{
		std::cout << "SUCCESS: open port - " << iPortComm << std::endl;
		std::cerr << "SUCCESS: open port - " << iPortComm << std::endl;
		return true;
	}
}

void FujinonEncoder::finishConnection()
{
	if( mPort.IsOpened() )
	{
		mPort.Close();
	}
}

int FujinonEncoder::getIrisPos()
{
	int iCounter = 0;
	if( mPort.IsOpened() )
	{
		cleanBuffer();

		mData.RequestIrisPos();
		do 
		{
			mPort.SendData( mData.getTxBuffer() , mData.getLenght() );
			Sleep(10);
			iRxBufferSize = mPort.ReadData( ucRxBuffer , iLimit );
			iCounter++;
		} while (iRxBufferSize == 0 && iCounter < 25 );

		if ( iRxBufferSize == 5 )
		{
			iIrisVal = (int)transmitWord( ucRxBuffer[2], ucRxBuffer[3] );
		}
		else
		{
			iIrisVal = -1;
		}
	}
	else
	{
		iIrisVal = -1;
	}

	double dIrisVal = 0.0;
	dIrisVal = getInterpolationFromLongInt( iIrisVal );

	printf(" Iris: %d , %.3lf", iIrisVal , dIrisVal );


	return iIrisVal;
}

int FujinonEncoder::getZoomPos()
{
	int iCounter = 0;
	if( mPort.IsOpened() )
	{
		cleanBuffer();

		mData.RequestZoomPos();
		do 
		{
			mPort.SendData( mData.getTxBuffer() , mData.getLenght() );
			Sleep(10);
			iRxBufferSize = mPort.ReadData( ucRxBuffer , iLimit );
			iCounter++;
		} while (iRxBufferSize == 0 && iCounter < 25 );

		if ( iRxBufferSize == 5 )
		{
			iZoomVal = (int)transmitWord( ucRxBuffer[2], ucRxBuffer[3] );
		}
		else
		{
			iZoomVal = -1;
		}
	}
	else
	{
		iZoomVal = -1;
	}

	double dZoomVal = 0.0;
	dZoomVal = getInterpolationFromLongInt( iZoomVal );

	printf(" Zoom: %d , %.3lf", iZoomVal , dZoomVal );
	return iZoomVal;
}

int FujinonEncoder::getFocusPos()
{
	int iCounter = 0;
	if( mPort.IsOpened() )
	{
		cleanBuffer();

		mData.RequestFocusPos();
		do 
		{
			mPort.SendData( mData.getTxBuffer() , mData.getLenght() );
			Sleep(10);
			iRxBufferSize = mPort.ReadData( ucRxBuffer , iLimit );
			iCounter++;
		} while (iRxBufferSize == 0 && iCounter < 25 );

		if ( iRxBufferSize == 5 )
		{
			iFocusVal = (int)transmitWord( ucRxBuffer[2], ucRxBuffer[3] );
		}
		else
		{
			iFocusVal = -1;
		}
	}
	else
	{
		iFocusVal = -1;
	}

	double dFocusVal = 0.0;
	dFocusVal = getInterpolationFromLongInt( iFocusVal );

	printf(" Focus: %d , %.3lf", iFocusVal , dFocusVal );
	return iFocusVal;
}

void FujinonEncoder::moveFocusPos( int inc )
{
	int iCounter;
	if( mPort.IsOpened() )
	{
		cleanBuffer();
		mPort.Clean();

		int iNewFocusVal = iFocusVal + inc;

		if ( iNewFocusVal >= 0 && iNewFocusVal <= 65535  )
		{
			mData.SetFocusPos( iNewFocusVal  );
		} 
		else
		{
			mData.SetFocusPos( iFocusVal );
		}

		mData.SetFocusPos( iNewFocusVal  );

		iCounter = 0;
		do 
		{
			mPort.SendData( mData.getTxBuffer() , mData.getLenght() );
			Sleep(10);
			iRxBufferSize = mPort.ReadData( ucRxBuffer , iLimit );
			iCounter++;
		} while (iRxBufferSize == 0 && iCounter < 25 );

		cleanBuffer();
		mPort.Clean();
		mData.ReqData( 0x01  );
		iCounter = 0;
		do 
		{
			mPort.SendData( mData.getTxBuffer() , mData.getLenght() );
			Sleep(10);
			iRxBufferSize = mPort.ReadData( ucRxBuffer , iLimit );
			iCounter++;
		} while (iRxBufferSize == 0 && iCounter < 25 );

		cleanBuffer();
		mPort.Clean();
		mData.RequestFocusPos();
		iCounter = 0;
		do 
		{
			mPort.SendData( mData.getTxBuffer() , mData.getLenght() );
			Sleep(10);
			iRxBufferSize = mPort.ReadData( ucRxBuffer , iLimit );
			iCounter++;
		} while (iRxBufferSize == 0 && iCounter < 25 );

		if ( iRxBufferSize == 5 )
		{
			iFocusVal = (int)transmitWord( ucRxBuffer[2], ucRxBuffer[3] );
		}
		else
		{
			iFocusVal = -1;
		}

		printf("\nNew Focus: %d \n ", iFocusVal );
	}

	return ;
}

void FujinonEncoder::cleanBuffer() 
{ 
	iRxBufferSize = 0;
	for ( int i = 0; i < 32; i++)
	{
		ucRxBuffer[i] = 0;
	}
	return;
}

unsigned short FujinonEncoder::transmitWord(unsigned char H, unsigned char L)
{  
	unsigned short result;
	result = ( H << 8 ) + L;      
	return result;
}

double getInterpolationFromLongInt( int iVal )
{
	double dVal;

	dVal = 0;
	dVal = (double)iVal / (double)65535;

	return dVal;
}
