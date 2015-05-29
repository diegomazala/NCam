
#ifndef __CLENS_FUJINON_READER_H__
#define __CLENS_FUJINON_READER_H__

/*
#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif
*/

class /*EXPORT_API*/ cSerialData
{

public:
	cSerialData();
	~cSerialData();

	unsigned char * getTxBuffer();
	int getLenght();

	void RequestIrisPos();
	void RequestZoomPos();
	void RequestFocusPos();

	void SetZoomPos( unsigned short usInVal);
	void SetFocusPos( unsigned short usInVal );

	void printBuffer();

	void ReqData( unsigned char Code );

private:
	void TxPutByte( unsigned char b );
	void PutShortInt( unsigned short i );
	void PutWord( unsigned short Data );
	void ReqLen2 ( unsigned char Code , unsigned short Data );
	void cleanBuffer();

public:
	int iLenght;
	unsigned char ucSum;
	unsigned char ucTxBuffer[19];

};

class FujinonEncoder
{
public:
	FujinonEncoder();
	~FujinonEncoder();

	void setPortNumber( int iComm );
	bool initConnection();
	void finishConnection();

	int getIrisPos();
	int getZoomPos();
	int getFocusPos();

	void moveFocusPos( int inc );

private:
	static unsigned short transmitWord( unsigned char H, unsigned char L );
	void cleanBuffer();

public:
	int iPortComm;
	cSerialData mData;
	SerialPort mPort;

	int iIrisVal;
	int iZoomVal;
	int iFocusVal;

	int iRxBufferSize;
	unsigned char ucRxBuffer[32];
	int iLimit;
};

double getInterpolationFromLongInt( int iVal );

#endif //__CLENS_FUJINON_READER_H__
