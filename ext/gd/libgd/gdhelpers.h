#ifndef GDHELPERS_H 
#define GDHELPERS_H 1

#include <sys/types.h>
#include "php.h"

/* TBB: strtok_r is not universal; provide an implementation of it. */

extern char *gd_strtok_r(char *s, char *sep, char **state);

/* These functions wrap memory management. gdFree is
	in gd.h, where callers can utilize it to correctly
	free memory allocated by these functions with the
	right version of free(). */
#define gdCalloc(nmemb, size)	ecalloc(nmemb, size)
#define gdMalloc(size)		emalloc(size)
#define gdRealloc(ptr, size)	erealloc(ptr, size)
#define gdEstrdup(ptr)		estrdup(ptr)
#define gdFree(ptr)		efree(ptr)
#define gdPMalloc(ptr)		pemalloc(ptr, 1)
#define gdPFree(ptr)		pefree(ptr, 1)

#endif /* GDHELPERS_H */

