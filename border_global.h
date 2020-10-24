#ifndef BORDER_GLOBAL_H
#define BORDER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BORDER_LIBRARY)
#  define BORDERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BORDERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BORDER_GLOBAL_H
