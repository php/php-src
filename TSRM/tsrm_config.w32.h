#ifndef TSRM_CONFIG_W32_H
#define TSRM_CONFIG_W32_H

#define HAVE_UTIME 1
#define HAVE_ALLOCA 1

#include <malloc.h>
#include <stdlib.h>
#include <crtdbg.h>

#undef inline
#ifdef ZEND_WIN32_FORCE_INLINE
# define inline __forceinline
#else
# define inline
#endif


#endif
