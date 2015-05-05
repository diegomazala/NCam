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
#ifndef LIBNCAMDATASTREAMING_GLOBAL_H
#define LIBNCAMDATASTREAMING_GLOBAL_H

#ifdef QT_CORE
#include <QtCore/qglobal.h>

#if defined(LIBNCAMDATASTREAMING_LIBRARY)
#  define NC_SHARED_LIB Q_DECL_EXPORT
#else
#  define NC_SHARED_LIB Q_DECL_IMPORT
#endif

#else // QT unavailable

#ifdef WIN32
#if defined(LIBNCAMDATASTREAMING_LIBRARY)
#define NC_SHARED_LIB __declspec(dllexport)
#else
#define NC_SHARED_LIB __declspec(dllimport)
#endif
#else
#define NC_SHARED_LIB
#endif // WIN32

#endif

#endif // LIBNCAMDATASTREAMING_GLOBAL_H
