/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001 The PHP Group                                     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   |         Rui Hirokawa <hirokawa@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zend_API.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfl_allocators.h"

static void *__php__malloc(unsigned int);
static void *__php__realloc(void *, unsigned int);
static void *__php__calloc(unsigned int, unsigned int);
static void __php__free(void *);

static mbfl_allocators php_allocators = {
	__php__malloc,
	__php__realloc,
	__php__calloc,
	__php__free,
	__php__malloc,
	__php__realloc,
	__php__free
};

mbfl_allocators *__mbfl_allocators = &php_allocators;

static void *__php__malloc(unsigned int sz)
{
	return emalloc(sz);
}

static void *__php__realloc(void *ptr, unsigned int sz)
{
	return erealloc(ptr, sz);
}

static void *__php__calloc(unsigned int nelems, unsigned int szelem)
{
	return ecalloc(nelems, szelem);
}

static void __php__free(void *ptr)
{
	efree(ptr);
} 

