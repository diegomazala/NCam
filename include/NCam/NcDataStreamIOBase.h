/*
*  Copyright (c) 2012-2014 Ncam Technologies Ltd. All rights reserved.
*  Unpublished - rights reserved under the copyright laws of the
*  United States. Use of a copyright notice is precautionary only
*  and does not imply publication or disclosure.
*  This software contains confidential information and trade secrets
*  of Ncam Technologies Limited. Use, disclosure, or reproduction
*  is prohibited without the prior express written permission of
*  Ncam Technologies Limited.
*/
/** \addtogroup SDK
 *  @{
 */
#ifndef NCDATASTREAMIOBASE_H
#define NCDATASTREAMIOBASE_H
#include <NcDataStreaming.h>
#include <string>
#include <vector>

/**
 * @brief The NcDataStreamIOBase class is the upper hierachy class of network communication through the Ncam Data Streaming library.
 */
class NC_SHARED_LIB NcDataStreamIOBase
{
public:
    typedef uint16_t IpPort_t; ///< Type for socket port.

    /**
     * @brief Default constructor.
     */
    NcDataStreamIOBase();
    /**
     * @brief Default destructor.
     */
    virtual ~NcDataStreamIOBase();

    /**
     * @brief Virtual start streaming function.
     * @return Should returns true if streaming successfully started.
     */
    virtual bool StartStreaming() = 0;
    /**
     * @brief Virtual stop streaming function.
     * @return Should returns true if the steaming successfully stoped.
     */
    virtual bool StopStreaming() = 0;
    /**
     * @brief Virtual main loop of streaming.
     * @return Should returns true if a new packet has been received.
     */
    virtual bool DoStreaming() = 0;
    /**
     * @return Should returns true if the connection with the server has been establised.
     */
    virtual bool IsConnected() const = 0;
    /**
     * @return Accessor to the socket port used.
     */
    const IpPort_t& GetPort() const;

protected:

    //Error Handling
    virtual void OnStartStreamingError(const std::string& lErrorDecription = std::string()) = 0;
    virtual void OnStopStreamingError(const std::string& lErrorDecription = std::string()) = 0;
    virtual void OnDoStreamingError(const std::string& lErrorDecription = std::string()) = 0;

    /**
     * @brief Default read of packets through the establised connection.
     * @param[out] data The buffer where to put the data.
     * @param len The size of the buffer allocated (maximal size allowed to read).
     * @return True if at least len bytes have been read to the buffer.
     */
    bool Read(uint8_t *data, const size_t& len);
    /**
     * @brief Default write of packets through the establised connection.
     * @param[in] data The buffer to write through the socket.
     * @param len The len of data buffer.
     * @return True if the full buffer has been written through the socket.
     */
    bool Write(const uint8_t *data, const size_t& len);

    /**
     * @brief Passive wait for new packet.
     * @return The new packet read or nullptr if any.
     */
    NcDataStreamBase* WaitAndGetANewPacket();
    /**
     * @brief Send a packet through the socket.
     * @param[in] lpPacket The packet to send.
     * @return True if the packet has been successfully sent.
     */
    bool SendPacket(const NcDataStreamBase *lpPacket);

    // Following Methods should be defined with your socket implementation.
    /**
     * @brief Internal openning of socket.
     * @return True if the openning success.
     * @note This function has to be overloaded with the plateform specific socket implementation.
     */
    virtual bool InternalOpen() = 0;
    /**
     * @brief Internal closing of socket.
     * @return True if the socket has been successfully closed.
     * @note This function has to be overloaded with the plateform specific socket implementation.
     */
    virtual bool InternalClose() = 0;

    /**
     * @brief Internal reading of data through the socket.
     * @param[in] data The data buffer to fill.
     * @param maxlen The size of data buffer.
     * @return The size of data read.
     * @note This function has to be overloaded with the plateform specific socket implementation.
     */
    virtual ssize_t InternalRead(uint8_t *data, const size_t& maxlen) = 0;
    /**
     * @brief Internal writing of data through the socket.
     * @param[out] data The buffer data used to send data.
     * @param maxlen The size of data buffer.
     * @return The size of data buffer sent.
     * @note This function has to be overloaded with the plateform specific socket implementation.
     */
    virtual ssize_t InternalWrite(const uint8_t *data, const size_t& maxlen) = 0;

    IpPort_t mIpPort; ///< The socket port.

private:
    NcDataStreamBase::PacketType_t* GetInternalPacketPtr();
    const NcDataStreamBase::PacketType_t* GetInternalPacketPtr() const;

    uint8_t* GetRemainingDataPtr();
    uint8_t* GetInternalBufferPtr();
    const uint8_t* GetInternalBufferPtr() const;

    void UpdateInternalBufferSize(const size_t& lSize);
    NcDataStreamBase::PacketType_t GetValidPacketTypeFromInternalBuffer() const;

    template <class T>
    NcDataStreamBase* ReadPacket(const uint64_t& lSizeInBytes);


    std::vector<uint8_t> *mpInternalBuffer;
};
/** @}*/
#endif // NCDATASTREAMIOBASE_H
