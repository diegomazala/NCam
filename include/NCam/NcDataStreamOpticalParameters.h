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

#ifndef NCDATASTREAMOPTICALPARAMETERS_H
#define NCDATASTREAMOPTICALPARAMETERS_H
/** \addtogroup SDK
 *  @{
 */
#include <NcDataStreamBase.h>

/**
 * @brief The NcDataStreamOpticalParameters class store the optical parameters of images.
 */
class NC_SHARED_LIB NcDataStreamOpticalParameters: public NcDataStreamBase
{
public:
    typedef uint32_t DistortionsType_t; ///< Specify the distortion type.

    /**
     * @brief Helper for distortion parameters.
     */
    enum eDistortions
    {
        DistPinHole = 0,         ///< Pin hole calibration model.
        DistSimpleSphericalK1,   ///< Simple spherical model K1 parameter.
        DistSimpleSphericalK1K2, ///< Simple spherical model K2 parameter.
        Spherical,               ///< Spherical model.
        SimpleAnamorphic,        ///< Simple anamorphic model.
        Anamorphic,              ///< Anamorphic model.
        FishEye,                 ///< Fish eye model.
        DistBegin = DistPinHole, ///< First distortion model.
        DistEnd = FishEye+1      ///< Last distortion model.
    };

    /**
     * @brief The OpticalParametersPacket structure store the optical parameters from calibration.
     */
    struct OpticalParametersPacket
    {
        PacketType_t mPacketType;               ///< Packet type.
        double mFovInDegrees[2];                ///< Field of view in degrees: Horizontal, Vertical.
        double mProjectionCenterNormalized[2];  ///< Center of projection (Cx,Cy)
        DistortionsType_t mDistortionType;      ///< Distortion type.
        double mImageSensorSizeInMm[2];         ///< Sensor size in mm.
        uint32_t mImageResolution[2];           ///< Resolution of the image.
        double mImageAspectRatio;               ///< Aspect ratio of the image.
        double mTop;                            ///< Top of the image
        double mBottom;                         ///< Bottom of the image.
        double mLeft;                           ///< Left of the image.
        double mRight;                          ///< Right of the image.
        double mDistortionParameters[24];       ///< Reserve for futher usage.
        TimeCode_t mTimeCode;                   ///< Timecode associated with the packet.
    };

    /**
     * @brief Default constructor.
     */
    NcDataStreamOpticalParameters();
    /**
     * @brief Default destructor.
     */
    virtual ~NcDataStreamOpticalParameters();

    /**
     * @return The size in bytes of the packet.
     */
    virtual size_t GetSizeInBytes() const;
    /**
     * @return The type of packet received.
     */
    ePacketType GetPacketType() const;

    /**
     * @return A constant pointer to the optical parameters.
     */
    const OpticalParametersPacket& GetData() const;
    /**
     * @return A reference to the optical parameters (to receive them).
     */
    OpticalParametersPacket& GetData();

protected:
    virtual uint8_t* Ptr();
    virtual const uint8_t* Ptr() const;

private:
    NcDataStreamOpticalParameters& operator =(NcDataStreamOpticalParameters&){ return *this;}
    NcDataStreamOpticalParameters(NcDataStreamOpticalParameters& ){}
    OpticalParametersPacket mData;
};
/** @}*/

#endif // NCDATASTREAMOPTICALPARAMETERS_H
