/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_DBA_H
#define PHP_DBA_H

#if HAVE_DBA

typedef enum { 
	DBA_READER = 1,
	DBA_WRITER,
	DBA_TRUNC,
	DBA_CREAT
} dba_mode_t;

typedef struct dba_info {
	/* public */
	void *dbf;               /* ptr to private data or whatever */
	char *path;
	dba_mode_t mode;
	/* arg[cv] are only available when the dba_open handler is called! */
	int argc;
	pval ***argv;
	/* private */
	struct dba_handler *hnd;
} dba_info;

extern zend_module_entry dba_module_entry;
#define dba_module_ptr &dba_module_entry

/* common prototypes which must be supplied by modules */

#define DBA_OPEN_FUNC(x) \
	int dba_open_##x(dba_info *info)
#define DBA_CLOSE_FUNC(x) \
	void dba_close_##x(dba_info *info)
#define DBA_FETCH_FUNC(x) \
	char *dba_fetch_##x(dba_info *info, char *key, int keylen, int *newlen)
#define DBA_UPDATE_FUNC(x) \
	int dba_update_##x(dba_info *info, char *key, int keylen, char *val, int vallen, int mode)
#define DBA_EXISTS_FUNC(x) \
	int dba_exists_##x(dba_info *info, char *key, int keylen)
#define DBA_DELETE_FUNC(x) \
	int dba_delete_##x(dba_info *info, char *key, int keylen)
#define DBA_FIRSTKEY_FUNC(x) \
	char *dba_firstkey_##x(dba_info *info, int *newlen)
#define DBA_NEXTKEY_FUNC(x) \
	char *dba_nextkey_##x(dba_info *info, int *newlen)
#define DBA_OPTIMIZE_FUNC(x) \
	int dba_optimize_##x(dba_info *info)
#define DBA_SYNC_FUNC(x) \
	int dba_sync_##x(dba_info *info)

#define DBA_FUNCS(x) \
	DBA_OPEN_FUNC(x); \
	DBA_CLOSE_FUNC(x); \
	DBA_FETCH_FUNC(x); \
	DBA_UPDATE_FUNC(x); \
	DBA_DELETE_FUNC(x); \
	DBA_EXISTS_FUNC(x); \
	DBA_FIRSTKEY_FUNC(x); \
	DBA_NEXTKEY_FUNC(x); \
	DBA_OPTIMIZE_FUNC(x); \
	DBA_SYNC_FUNC(x)

#define VALLEN(p) Z_STRVAL_PP(p), Z_STRLEN_PP(p)
	
PHP_FUNCTION(dba_open);
PHP_FUNCTION(dba_popen);
PHP_FUNCTION(dba_close);
PHP_FUNCTION(dba_firstkey);
PHP_FUNCTION(dba_nextkey);
PHP_FUNCTION(dba_replace);
PHP_FUNCTION(dba_insert);
PHP_FUNCTION(dba_delete);
PHP_FUNCTION(dba_exists);
PHP_FUNCTION(dba_fetch);
PHP_FUNCTION(dba_optimize);
PHP_FUNCTION(dba_sync);

#else
#define dba_module_ptr NULL
#endif

#define phpext_dba_ptr dba_module_ptr

#endif
