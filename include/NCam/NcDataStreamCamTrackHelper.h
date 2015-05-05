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
#ifndef NCDATASTREAMCAMTRACKHELPER_H
#define NCDATASTREAMCAMTRACKHELPER_H
#include <NcDataStreamCamTrack.h>
/** \addtogroup SDK
 *  @{
 */

namespace NcInternal
{
    /**
      * @brief Inversion of transformtation matrix (R|t) with fast computation.
      * @param ltransfo The NcDataStreamCamTrack::RigidTransfoPacket matrix to inverse.
      */
    NcDataStreamCamTrack::RigidTransfoPacket NC_SHARED_LIB Inverse(const NcDataStreamCamTrack::RigidTransfoPacket& lTransfo);
    /**
      * @brief Convert a rigid transformation to the OpenGL format and store it into lGLMatrix.
      * @param lTransfo The NcDataStreamCamTrack::RigidTransfoPacket matrix to convert
      * @param lGLMatrix OpenGL compatible \f$4 \times 4\f$ homogeneous matrix converted from lTransfo packet.
      */
    void NC_SHARED_LIB ToOpenGLMatrix(const NcDataStreamCamTrack::RigidTransfoPacket& lTransfo, double lGLMatrix [16]);
    /**
      * @brief Convert a binary coded decimal to an integer.
      * @param bcd Binary coded decimal field to convert.
      * @return An integer on 32 bits.
      */
    uint32_t NC_SHARED_LIB Bcd2Uint(uint8_t bcd);
}

namespace NcDataStreamCamHelper
{
    /**
      * @brief Return the rigid transformation (R|t) matrix form camera to world.
      */
    NcDataStreamCamTrack::RigidTransfoPacket NC_SHARED_LIB GetWorldFromCamera(const NcDataStreamCamTrack::TrackingPacket& lTP);
    /**
      * @brief Convert a rigid transformation packet into an OpenGL modelview matrix.
      */
    void NC_SHARED_LIB ToOpenGLModelView(const NcDataStreamCamTrack::TrackingPacket& lTP,double lGLMatrix [16]);
    /**
      * @brief Convert a rigid transformation packets to an OpenGL view model matrix.
      */
    void NC_SHARED_LIB ToOpenGLViewModel(const NcDataStreamCamTrack::TrackingPacket& lTP,double lGLMatrix [16]);
    /**
      * @brief
      */
    void NC_SHARED_LIB CorrectZDirection(const double MinusZOpenGLProjectionMatrix[16], double PlusZOpenGLProjectionMatrix[16]);
    /**
      * @brief Convert a timecode, as received in a tracking packet to a candy eye display.
      */
    void NC_SHARED_LIB DecodeTimeCode(const uint32_t& lTimeCodel, uint32_t& lHours, uint32_t& lMinutes, uint32_t& Seconds,uint32_t& lFrames, uint8_t& lDropFrame );
}
/** @}*/
#endif // NCDATASTREAMCAMTRACKHELPER_H
