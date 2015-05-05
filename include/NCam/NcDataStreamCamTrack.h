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
#ifndef NCDATASTREAMCAMTRACK_H
#define NCDATASTREAMCAMTRACK_H
#include <NcDataStreamBase.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamCamTrack class is a specification of abstract packet type for camera tracking data.
 */
class NC_SHARED_LIB NcDataStreamCamTrack: public NcDataStreamBase
{
public:

    /**
     * @brief The LensEncoderPacket struct is a structure to send/retrieve lens encoders values (physical values and normalized in range [0;1]).
     */
    struct LensEncoderPacket
    {
        double mNormalized;  ///< Normalized values of encoder, in range \f$[0;1]\f$.
        double mMappedValue; ///< Physical value of the encoder (could be in mm for focale, meter for focuse, T-stop for iris, etc.) as stated in Ncam reality tracking system.
    };

    /**
     * @brief The RigidTransfoPacket structure stores the camera rotation and translation as tracked by Ncam reality server.
     */
    struct RigidTransfoPacket
    {
        double mRotation [9];    ///< Rotation vector in sergent major.
        double mTranslation [3]; ///< Translation vector.
    };

    /**
     * @brief The TrackingPacket structure store all tracking information sent by Ncam Reality.
     * @see PacketType_t, LensEncoderPacket, RigidTransfoPacket.
     */
    struct TrackingPacket
    {
        //Should be the first packet
        PacketType_t mPacketType;         ///< The packet type.
        LensEncoderPacket mZoomEncoder;   ///< The zoom encoder values.
        LensEncoderPacket mFocusEncoder;  ///< The focus encoder values.
        LensEncoderPacket mIrisEncoder;   ///< The iris encoder values.
        RigidTransfoPacket mCamFromWorld; ///< The transformation matrix, From the world to the camera.
        uint32_t mTimeCode;               ///< The timecode associated to the tracking information.
    };

    /**
     * @brief Create a tracking packet.
     */
    NcDataStreamCamTrack();
    /**
     * @brief Release the tracking packet.
     */
    virtual ~NcDataStreamCamTrack();
    /**
     * @brief
     * @return The size in bytes of the received tracking packet.
     */
    virtual size_t GetSizeInBytes() const;
    /**
     * @brief
     * @return The type of tracking packet received.
     */
    ePacketType GetPacketType() const;
    /**
     * @brief
     * @return The constant reference to the internal packet received.
     */
    const TrackingPacket& GetData() const;
    /**
     * @brief
     * @return The access to the internal tracking packet.
     */
    TrackingPacket& GetData();
protected:

    /**
     * @brief Overload for Tracking data packet.
     * @return The tracking packet through a uint8_t data buffer (to retrieve it through socket).
     */
    virtual uint8_t* Ptr();
    /**
     * @brief Overload for tracking data packet.
     * @return A constant tracking packet through a uint8_t data buffer (to send it through socket).
     */
    virtual const uint8_t* Ptr() const;

    //Members
private:
    /**
     * @brief Affectation operator overloading (to prevent copy).
     * @return A reference to this.
     */
    NcDataStreamCamTrack& operator =(NcDataStreamCamTrack&){ return *this;}
    /**
     * @brief Copy constructor definition (in private to prevent from copy).
     */
    NcDataStreamCamTrack(NcDataStreamCamTrack& ){}

    TrackingPacket* mData; ///< The internal tracking packet.
};
/** @}*/
#endif // NCDATASTREAMCAMTRACK_H
