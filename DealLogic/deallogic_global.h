#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DEALLOGIC_LIB)
#  define DEALLOGIC_EXPORT Q_DECL_EXPORT
# else
#  define DEALLOGIC_EXPORT Q_DECL_IMPORT
# endif
#else
# define DEALLOGIC_EXPORT
#endif
