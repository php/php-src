/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#ifndef _CONFIG_UNIX_H
#define _CONFIG_UNIX_H

#define ZEND_API

#include <stdlib.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#if HAVE_LIBDL
#	include <dlfcn.h>
#	define DL_LOAD(libname)	dlopen(libname, RTLD_NOW)
#	define DL_UNLOAD		dlclose
#	define DL_FETCH_SYMBOL	dlsym
#	define DL_HANDLE		void *
#	define ZEND_EXTENSIONS_SUPPORT 1
#else
#	define DL_HANDLE		void *
#	define ZEND_EXTENSIONS_SUPPORT 0
#endif

#if BROKEN_SPRINTF
int zend_sprintf(char *buffer, const char *format, ...);
#else
#	define zend_sprintf sprintf
#endif

#if ZEND_DEBUG
#	define inline
#endif

#endif /* _CONFIG_UNIX_H */
