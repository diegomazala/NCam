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

#ifndef NCDATASTREAMOPTICALPARAMETERSHELPER_CPP
#define NCDATASTREAMOPTICALPARAMETERSHELPER_CPP
/** \addtogroup SDK
 *  @{
 */
#include <NcDataStreamOpticalParameters.h>

namespace NcDataStreamOpticalParametersHelper
{
/**
  * @brief Generate the Opengl Projection Matrix project the geometry that will be distorted.
  * This matrix takes into account the lens shift, the vertical an horizontal focal length.
  * @note The H and V FOV ar bigger that the one in lOpticalParametersPacket. This is because it is necessary to render a much bigger area to have a mapping between each pixel of the film camera and a pixel of the rendere geometry.
  * @param lOpticalParametersPacket contains the frustum parameters of the FilmCamera.
  * @param lGLMatrix is the resulting matrix.
  * @param NearPlane define the near plane of the camera. Every point of the geometry in front of this plane will be discarded.
  * @param NearPlane define the far plane of the camera. Every point of the geometry beyond this plane will be discarded.
  */
void NC_SHARED_LIB GetProjectionMatrix(const NcDataStreamOpticalParameters::OpticalParametersPacket& lOpticalParametersPacket, double lGLMatrix[16], const double& NearPlane = 0.1, const double& FarPlane = 1000.0);
}

/** @}*/

#endif // NCDATASTREAMOPTICALPARAMETERSHELPER_CPP
