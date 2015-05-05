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
#ifndef NCDATASTREAMFILMIMAGE_H
#define NCDATASTREAMFILMIMAGE_H
#include <NcDataStreamImage.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamFilmImage class returns the film image of the camera tracked.
 */
class NC_SHARED_LIB NcDataStreamFilmImage: public NcDataStreamImage
{
public:
    /**
     * @brief Create a film image with the specific parameters.
     * @param lChannels The number of channels (usually 4: Red Green Blue Alpha).
     * @param lDepth The depth of each channel.
     */
    NcDataStreamFilmImage(Channels lChannels = UnknownChannels,Depth lDepth = UnknownDepth);
    /**
     * @brief Release the film image.
     */
    virtual~NcDataStreamFilmImage();
};

/** @}*/
#endif // NCDATASTREAMFILMIMAGE_H
