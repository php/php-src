#ifndef _ZEND_CONFIG_W32_H
#define _ZEND_CONFIG_W32_H


#include <string.h>
#include <windows.h>

typedef unsigned long ulong;
typedef unsigned int uint;

#define HAVE_ALLOCA 1
#include <malloc.h>

#define HAVE_DIRENT_H 0

#ifdef inline
#undef inline
#endif

#define zend_sprintf sprintf

/* Visual C++ doesn't really work with inline for C */
#define inline


#define DL_LOAD(libname)	LoadLibrary(libname)
#define DL_FETCH_SYMBOL		GetProcAddress
#define DL_UNLOAD			FreeLibrary
#define DL_HANDLE			HMODULE
#define ZEND_EXTENSIONS_SUPPORT 1

#ifdef LIBZEND_EXPORTS
#	define ZEND_API __declspec(dllexport)
#else
#	define ZEND_API __declspec(dllimport)
#endif

#endif /* _ZEND_CONFIG_W32_H */