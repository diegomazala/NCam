#ifndef _NCAM_WIN_CLIENT_H_
#define _NCAM_WIN_CLIENT_H_


#include <winsock2.h>
#include "NCamClient.h"


class NCamWinClient : public NCamClient
{
public:

	NCamWinClient();

	virtual ~NCamWinClient();


	void Connect(const std::string ipAddress, int port);
	
	void Disconnect();


	/**
	* @brief Map the buffer for packet access.
	* @param lSuccess Flag indicate if the mapping success.
	* @return The last packet read through the socket.
	* @warning No concurrent access allowed!
	*/
	Packets_t &MapPackets(bool &lSuccess);


	/**
	* @brief Unmap the buffer.
	* @warning No concurrent access allowed!
	*/
	void UnmapPackets();


	/**
	* @brief IsConnected is pure virtual, has to be overloaded as it depends on socket implementation.
	* @return True if the socket is opened.
	*/
	bool IsConnected() const;




protected:


	bool OnCheckDataFromClient();

	/**
	* @brief InternalOpen This function has to be overloaded with the specific socket openning process.
	* @return True if the connection can be opened, false if the socket is already openned or if the socket cannot be openned.
	*/
	bool InternalOpen();



	/**
	* @brief InternalClose This function has to be overloaded with socket-depend close process.
	* @return True if the socket has been closed, false if the socket wasn't opened.
	*/
	bool InternalClose();



	/**
	* @brief InternalRead Read a buffer of bytes.
	* @param data The buffer used to store the read bytes.
	* @param maxlen The maximum size allowed for the data buffer.
	* @return The number of bytes read.
	*/
	ssize_t InternalRead(uint8_t *data, const size_t &maxlen);



	/**
	* @brief InternalWrite Write to the socket (for communication to the server).
	* @param data The buffer to write through the socket.
	* @param maxlen The size of the buffer.
	* @return The number of bytes written, or -1 if the writting failed.
	*/
	ssize_t InternalWrite(const uint8_t *data, const size_t &maxlen);


protected:
	Packets_t mDataBuffer; ///< Buffer of packets.
	SOCKET mSocket;   ///< Socket object.
	struct addrinfo* mSocketAddr; ///< Structure of socket information (address, port, protocol, etc).


};

#endif // _NCAM_WIN_CLIENT_H_