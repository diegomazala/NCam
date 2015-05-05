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

#ifndef NCDATASTREAMDISTORTMAP_H
#define NCDATASTREAMDISTORTMAP_H
#include <NcDataStreamImage.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamDistortMap class retuns a distortion map, according to the camera calibration.
 */
class NC_SHARED_LIB NcDataStreamDistortMap: public NcDataStreamImage
{
public:
    /**
     * @brief Create a distortion map for the film camera calibrated.
     * @param lChannels The number of channels for the distortion map.
     * @param lDepth The depth of each channel.
     */
    NcDataStreamDistortMap(Channels lChannels = UnknownChannels,Depth lDepth = UnknownDepth);
    /**
     * @brief Release the distortion map.
     */
    virtual~NcDataStreamDistortMap();
};
/** @}*/
#endif // NCDATASTREAMDISTORTMAP_H
