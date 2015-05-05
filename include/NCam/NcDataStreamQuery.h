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
#ifndef NCDATASTREAMQUERY_H
#define NCDATASTREAMQUERY_H
/** \addtogroup SDK
 *  @{
 */
#include <NcDataStreamBase.h>

/**
 * @brief The NcDataStreamQuery class is used to query availabilities of the server.
 */
class NC_SHARED_LIB NcDataStreamQuery: public NcDataStreamBase
{
public:

    /**
     * @brief The QueryPacket structure is used to store availabilities of packet types.
     */
    struct QueryPacket
    {
        //Should be the first packet
        PacketType_t mPacketType;        ///< Type of the packet (query).
        PacketType_t mPacketsToActivate; ///< Union of packet types to query activation onto the server.
    };
    // Methods
    /**
     * @brief Default constructor.
     */
    NcDataStreamQuery();
    /**
     * @brief Default destructor.
     */
    virtual ~NcDataStreamQuery();
    /**
     * @return The size in bytes of the packet.
     */
    virtual size_t GetSizeInBytes() const;
    /**
     * @return The type of the packet.
     */
    ePacketType GetPacketType() const;
    /**
     * @brief Get access to the data of the packet.
     * @return A constant reference to the packet received.
     */
    const QueryPacket& GetData() const;
    /**
     * @brief Get access to the data of the packet.
     * @return A reference to the packet (eg. for writing).
     */
    QueryPacket& GetData();
protected:
    virtual uint8_t* Ptr();
    virtual const uint8_t* Ptr() const;
    //Members
private:
    QueryPacket* mData;
};
/** @}*/
#endif // NCDATASTREAMQUERY_H
