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

#ifndef NCDATASTREAMCLIENTBASE_H
#define NCDATASTREAMCLIENTBASE_H
#include <NcDataStreamIOBase.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamClientBase class is the virtual class used to transfer packets from server to client.
 * @see NcSimpleDataStreamClient different implementations for examples.
 */
class NC_SHARED_LIB NcDataStreamClientBase: public NcDataStreamIOBase
{
public:
    /**
     * @brief Default constructor.
     */
    NcDataStreamClientBase();
    /**
     * @brief Default destructor, in charge of nothing here.
     */
    virtual ~NcDataStreamClientBase();

    /**
     * @brief Returns the capabilities of the server to the client, using StartStreaming.
     * @param lSuccess Flag to indicate the success or fail of the request.
     * @return The availabilities of the server in a NcDataStreamCapabilities packet.
     */
    const NcDataStreamCapabilities &GetCapabilities(bool &lSuccess);
    /**
     * @brief Ask the specify packet to the server.
     * @param lQuery The packet type to ask.
     * @return True if the type is available, false otherwise.
     */
    bool AskForPackets(const NcDataStreamBase::PacketType_t &lQuery);
    /**
     * @brief Set the connection parameters to the client.
     *
     * This function must be call to set IPv4 address and Port to connect through the socket.
     * @see The samples in Examples directory for implementation examples.
     * @param lIpAdress The IPv4 address of the server.
     * @param lIpPort The IPv4 port of the server, default is 38860.
     */
    void SetConnectionParameters(const std::string& lIpAdress, const IpPort_t& lIpPort);
    /**
     * @brief Start the streaming of packets from server to client.
     *
     * This function is the initialization of any event loop in client.
     * @return True if the connection succeed, false otherwise and call the error callback OnStartStreamingError.
     */
    bool StartStreaming();
    /**
     * @brief Stop the streaming at the end of the communication.
     *
     * This function is the last function of event loop between client and server.
     * It calls the OnStopStreamingError callback.
     * @return True if the connection successfully stop, false otherwise.
     */
    bool StopStreaming();
    /**
     * @brief Main function of the event loop, usually call in an infinite loop to get new packets.
     * @return True if the client successfully receive a packet.
     */
    bool DoStreaming();
    /**
     * @return The IPv4 address used to connect to the server.
     */
    const std::string& GetIpAddress() const;

protected:
    /**
     * @brief MapPackets is a virtual function used to get a thread safe access to packets sent or received.
     * @param lSuccess Flag to indicate the success of lock.
     * @return A reference to the last packet received.
     * @note This function has to be overloaded with the plateform specific lock in the client.
     * @see The OpenGL client in Examples directory.
     */
    virtual Packets_t& MapPackets(bool& lSuccess) = 0;
    /**
     * @brief Release the access to the packet after usage.
     * @note Same as MapPackets, this function is delegated to the user, and is plateform specific.
     */
    virtual void UnmapPackets() = 0;

    /**
     * @brief Callback call on error during connection start.
     * @param lErrorDecription Message return by the connection.
     * @note Default implementation is dummy, you probably should overload the callback to set a plateform specific error message.
     * @note Message are print in the standart error file descriptor.
     */
    virtual void OnStartStreamingError(const std::string& lErrorDecription = std::string());

    /**
     * @brief Callback call on error during connection end.
     * @param lErrorDecription Message return by the connection.
     * @note Default implementation is dummy, you probably should overload the callback to set a plateform specific error message.
     * @note Message are print in the standart error file descriptor.
     */
    virtual void OnStopStreamingError(const std::string& lErrorDecription = std::string());

    /**
     * @brief Callback call on error during sending or receiving packets.
     * @param lErrorDecription Message return by the connection.
     * @note Default implementation is dummy, you probably should overload the callback to set a plateform specific error message.
     * @note Message are print in the standart error file descriptor.
     *//** @}*/
    virtual void OnDoStreamingError(const std::string& lErrorDecription = std::string());

private:

    bool UpdateCapabilitiesWithQuery();
    void ClearPackets(Packets_t&lPackets) const;

    NcDataStreamCapabilities mServerCapabilities;
    NcDataStreamQuery mClientQuery;
    std::string *mpIpAdress;
};
/** @}*/
#endif // NCDATASTREAMCLIENTBASE_H
