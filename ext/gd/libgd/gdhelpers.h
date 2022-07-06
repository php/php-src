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
#define gdPEstrdup(ptr)		pestrdup(ptr, 1)

/* Returns nonzero if multiplying the two quantities will
	result in integer overflow. Also returns nonzero if
	either quantity is negative. By Phil Knirsch based on
	netpbm fixes by Alan Cox. */

int overflow2(int a, int b);

#ifdef ZTS
#define gdMutexDeclare(x) MUTEX_T x
#define gdMutexSetup(x) x = tsrm_mutex_alloc()
#define gdMutexShutdown(x) tsrm_mutex_free(x)
#define gdMutexLock(x) tsrm_mutex_lock(x)
#define gdMutexUnlock(x) tsrm_mutex_unlock(x)
#else
#define gdMutexDeclare(x)
#define gdMutexSetup(x)
#define gdMutexShutdown(x)
#define gdMutexLock(x)
#define gdMutexUnlock(x)
#endif

#define DPCM2DPI(dpcm) (unsigned int)((dpcm)*2.54 + 0.5)
#define DPM2DPI(dpm)   (unsigned int)((dpm)*0.0254 + 0.5)
#define DPI2DPCM(dpi)  (unsigned int)((dpi)/2.54 + 0.5)
#define DPI2DPM(dpi)   (unsigned int)((dpi)/0.0254 + 0.5)

#endif /* GDHELPERS_H */

