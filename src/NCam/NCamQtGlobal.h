#ifndef NCAMQT_GLOBAL_H
#define NCAMQT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef NCAM_QT_LIB
# define NCAMQT_EXPORT Q_DECL_EXPORT
#else
# define NCAMQT_EXPORT Q_DECL_IMPORT
#endif

#endif // NCAMQT_GLOBAL_H
