#ifndef TSRM_CONFIG_COMMON_H
#define TSRM_CONFIG_COMMON_H

#ifndef __CYGWIN__
# if WINNT|WIN32
#  define TSRM_WIN32
# endif
#endif

#ifdef TSRM_WIN32
# include "tsrm_config.w32.h"
#else
# include <tsrm_config.h>
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
#    ifndef NETWARE
char *alloca ();
#    endif
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
# elif defined(MAX_PATH)
#  define MAXPATHLEN MAX_PATH
# else
#  define MAXPATHLEN 256
# endif
#endif

#if (HAVE_ALLOCA || (defined (__GNUC__) && __GNUC__ >= 2))
# define TSRM_ALLOCA_MAX_SIZE 4096
# define TSRM_ALLOCA_FLAG(name) \
	int name;
# define tsrm_do_alloca_ex(size, limit, use_heap) \
	((use_heap = ((size) > (limit))) ? malloc(size) : alloca(size))
# define tsrm_do_alloca(size, use_heap) \
	tsrm_do_alloca_ex(size, TSRM_ALLOCA_MAX_SIZE, use_heap)
# define tsrm_free_alloca(p, use_heap) \
	do { if (use_heap) free(p); } while (0)
#else
# define TSRM_ALLOCA_FLAG(name)
# define tsrm_do_alloca(p, use_heap)	malloc(p)
# define tsrm_free_alloca(p, use_heap)	free(p)
#endif

#endif /* TSRM_CONFIG_COMMON_H */
