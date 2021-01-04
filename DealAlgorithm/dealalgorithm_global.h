#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DEALALGORITHM_LIB)
#  define DEALALGORITHM_EXPORT Q_DECL_EXPORT
# else
#  define DEALALGORITHM_EXPORT Q_DECL_IMPORT
# endif
#else
# define DEALALGORITHM_EXPORT
#endif
