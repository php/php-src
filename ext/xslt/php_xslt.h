/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifndef _PHP_XSLT_H
#define _PHP_XSLT_H

#include "php.h"
#include "ext/standard/php_filestat.h"

#ifdef HAVE_XSLT

#define XSLT_OBJ(__func)       (&(__func)->obj)
#define XSLT_FUNC(__func)      ((__func)->func)

#define XSLT_IS_FILE 0
#define XSLT_IS_DATA 1

struct xslt_function {
	zval *obj;
	zval *func;
};

struct _xslt_argument {
	char *ptr;
	int type;
};

typedef struct {
	struct _xslt_argument xml;
	struct _xslt_argument xsl;
	struct _xslt_argument result;
} xslt_args;

extern xslt_args *xslt_parse_arguments(char *, char *, char *, char **);
extern void xslt_free_arguments(xslt_args *);

extern void xslt_assign_handler(struct xslt_function **, zval **);
extern void xslt_free_handler(struct xslt_function *);
extern void xslt_call_function(char *, zval *, zval *, int, zval **, zval **);

extern void xslt_debug(char *, char *, ...);

extern void xslt_make_array(zval **zarr, char ***carr);
extern void xslt_free_array(char **arr);

#endif

#endif
