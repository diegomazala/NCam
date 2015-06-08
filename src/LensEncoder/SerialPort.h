
#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__


#include <windows.h>

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

class SerialPort
{

public:
	SerialPort();
	~SerialPort();

	BOOL Open( int nPort = 2, int nBaud = CBR_38400 , int nByteSize = 8, int nStopBit = ONESTOPBIT , int nParity = NOPARITY );
	BOOL Close( void );

	int ReadData( void *, int );
	int SendData( const char *, int );
	int SendData( unsigned char *, int );
	int ReadDataWaiting( void );

	BOOL IsOpened( void ) const { return( m_bOpened ); }

	void Clean();

protected:
	BOOL WriteCommByte( unsigned char );
	BOOL WriteCommBytes( unsigned char * , int );

	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	BOOL m_bOpened;

};

#endif //__SERIAL_PORT_H__
