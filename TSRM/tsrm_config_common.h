#ifndef TSRM_CONFIG_COMMON_H
#define TSRM_CONFIG_COMMON_H

#if WINNT|WIN32
# define TSRM_WIN32
#endif

#ifdef TSRM_WIN32
# include "tsrm_config.w32.h"
#elif defined(NETWARE)
# include "tsrm_config.nw.h"
#else
# include "tsrm_config.h"
# include <sys/param.h>
#endif

#if HAVE_ALLOCA_H && !defined(_ALLOCA_H)
#  include <alloca.h>
#endif

/* AIX requires this to be the first thing in the file.  */
#ifndef __GNUC__
# ifndef HAVE_ALLOCA_H
#  ifdef _AIX
 #pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#   endif
#  endif
# endif
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef MAXPATHLEN
# ifdef PATH_MAX
#  define MAXPATHLEN PATH_MAX
# else
#  define MAXPATHLEN 256
# endif
#endif

#if (HAVE_ALLOCA || (defined (__GNUC__) && __GNUC__ >= 2))
# define tsrm_do_alloca(p) alloca(p)
# define tsrm_free_alloca(p)
#else
# define tsrm_do_alloca(p)   malloc(p)
# define tsrm_free_alloca(p) free(p)
#endif

#endif /* TSRM_CONFIG_COMMON_H */
