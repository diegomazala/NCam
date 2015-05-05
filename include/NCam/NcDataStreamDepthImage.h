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
#ifndef NCDATASTREAMDEPTHIMAGE_H
#define NCDATASTREAMDEPTHIMAGE_H
#include <NcDataStreamImage.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamDepthImage class returns a depth image.
 */
class NC_SHARED_LIB NcDataStreamDepthImage: public NcDataStreamImage
{
public:
    /**
     * @brief Create a depth image with specified parameters.
     * @param lChannels The number of channels for the depth image (usually 1).
     * @param lDepth The depth of each channel, usually of float size.
     */
    NcDataStreamDepthImage(Channels lChannels = UnknownChannels,Depth lDepth = UnknownDepth);
    /**
     * @brief Release the depth image.
     */
    virtual~NcDataStreamDepthImage();
};
/** @}*/
#endif // NCDATASTREAMDEPTHIMAGE_H
