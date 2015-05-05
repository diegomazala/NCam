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
#ifndef NCDATASTREAMCAPABILITIES_H
#define NCDATASTREAMCAPABILITIES_H
#include <NcDataStreamBase.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamCapabilities class returns the capabilities of the tracking server.
 */
class NC_SHARED_LIB NcDataStreamCapabilities: public NcDataStreamBase
{
public:
    //Data Abstract Type Decl
    /**
     * @brief The CapabilitiesPacket structure is used to store server availabilities.
     */
    struct CapabilitiesPacket
    {
        PacketType_t mPacketType;       ///< Store the packet type.
        PacketType_t mAvailablePackets; ///< Store the available packets.
        PacketType_t mActivatedPackets; ///< Store the activated packets on the current session.
    };
    // Methods
    /**
     * @brief Constructor of packet availabilities.
     */
    NcDataStreamCapabilities();
    /**
     * @brief Release packet availabilities.
     */
    virtual ~NcDataStreamCapabilities();
    /**
     * @return The size in bytes of packet.
     */
    virtual size_t GetSizeInBytes() const;
    /**
     * @brief
     * @return The type of packet send.
     */
    ePacketType GetPacketType() const;
    /**
     * @brief
     * @return A constant access to the packet (for sent).
     */
    const CapabilitiesPacket& GetData() const;
    /**
     * @brief
     * @return The raw access to the packets store (or to received).
     */
    CapabilitiesPacket& GetData();
protected:
    virtual uint8_t* Ptr();
    virtual const uint8_t* Ptr()const;

    //Members
private:
    CapabilitiesPacket* mData;
    NcDataStreamCapabilities& operator =(NcDataStreamCapabilities&){ return *this;}
    NcDataStreamCapabilities(NcDataStreamCapabilities& ){}
};
/** @}*/
#endif // NCDATASTREAMCAPABILITIES_H
