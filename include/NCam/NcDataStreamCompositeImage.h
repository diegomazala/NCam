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
#ifndef NCDATASTREAMCOMPOSITEIMAGE_H
#define NCDATASTREAMCOMPOSITEIMAGE_H
#include <NcDataStreamImage.h>
/** \addtogroup SDK
 *  @{
 */
/**
 * @brief The NcDataStreamCompositeImage class returns a composite image (Film camera image and CG in the position set by the tracking).
 */
class NC_SHARED_LIB NcDataStreamCompositeImage: public NcDataStreamImage
{
public:
    /**
     * @brief Create a composite image with specified parameters.
     * @param lChannels The number of channels for the composite image (usually 4).
     * @param lDepth The depth of each channel.
     */
    NcDataStreamCompositeImage(Channels lChannels = UnknownChannels,Depth lDepth = UnknownDepth);
    /**
     * @brief Release a composite image.
     */
    virtual~NcDataStreamCompositeImage();
};
/** @}*/
#endif // NCDATASTREAMCOMPOSITEIMAGE_H
