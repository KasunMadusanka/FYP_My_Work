#ifndef CAMFILEHANDLE_GLOBAL_H
#define CAMFILEHANDLE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef CAMFILEHANDLE_LIB
# define CAMFILEHANDLE_EXPORT Q_DECL_EXPORT
#else
# define CAMFILEHANDLE_EXPORT Q_DECL_IMPORT
#endif

#endif // CAMFILEHANDLE_GLOBAL_H
