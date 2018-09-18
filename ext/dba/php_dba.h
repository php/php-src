/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_DBA_H
#define PHP_DBA_H

#include "php_version.h"
#define PHP_DBA_VERSION PHP_VERSION

#if HAVE_DBA

typedef enum {
	/* do not allow 0 here */
	DBA_READER = 1,
	DBA_WRITER,
	DBA_TRUNC,
	DBA_CREAT
} dba_mode_t;

typedef struct dba_lock {
	php_stream *fp;
	char *name;
	int mode; /* LOCK_EX,LOCK_SH */
} dba_lock;

typedef struct dba_info {
	/* public */
	void *dbf;               /* ptr to private data or whatever */
	char *path;
	dba_mode_t mode;
	php_stream *fp;  /* this is the database stream for builtin handlers */
	int fd;
	/* arg[cv] are only available when the dba_open handler is called! */
	int argc;
	zval *argv;
	/* private */
	int flags; /* whether and how dba did locking and other flags*/
	struct dba_handler *hnd;
	dba_lock lock;
} dba_info;

#define DBA_LOCK_READER  (0x0001)
#define DBA_LOCK_WRITER  (0x0002)
#define DBA_LOCK_CREAT   (0x0004)
#define DBA_LOCK_TRUNC   (0x0008)

#define DBA_LOCK_EXT     (0)
#define DBA_LOCK_ALL     (DBA_LOCK_READER|DBA_LOCK_WRITER|DBA_LOCK_CREAT|DBA_LOCK_TRUNC)
#define DBA_LOCK_WCT     (DBA_LOCK_WRITER|DBA_LOCK_CREAT|DBA_LOCK_TRUNC)

#define DBA_STREAM_OPEN  (0x0010)
#define DBA_PERSISTENT   (0x0020)

#define DBA_CAST_AS_FD   (0x0050)
#define DBA_NO_APPEND    (0x00D0)

extern zend_module_entry dba_module_entry;
#define dba_module_ptr &dba_module_entry

typedef struct dba_handler {
	char *name; /* handler name */
	int flags; /* whether and how dba does locking and other flags*/
	int (*open)(dba_info *, char **error);
	void (*close)(dba_info *);
	char* (*fetch)(dba_info *, char *, size_t, int, size_t *);
	int (*update)(dba_info *, char *, size_t, char *, size_t, int);
	int (*exists)(dba_info *, char *, size_t);
	int (*delete)(dba_info *, char *, size_t);
	char* (*firstkey)(dba_info *, size_t *);
	char* (*nextkey)(dba_info *, size_t *);
	int (*optimize)(dba_info *);
	int (*sync)(dba_info *);
	char* (*info)(struct dba_handler *hnd, dba_info *);
		/* dba_info==NULL: Handler info, dba_info!=NULL: Database info */
} dba_handler;

/* common prototypes which must be supplied by modules */

#define DBA_OPEN_FUNC(x) \
	int dba_open_##x(dba_info *info, char **error)
#define DBA_CLOSE_FUNC(x) \
	void dba_close_##x(dba_info *info)
#define DBA_FETCH_FUNC(x) \
	char *dba_fetch_##x(dba_info *info, char *key, size_t keylen, int skip, size_t *newlen)
#define DBA_UPDATE_FUNC(x) \
	int dba_update_##x(dba_info *info, char *key, size_t keylen, char *val, size_t vallen, int mode)
#define DBA_EXISTS_FUNC(x) \
	int dba_exists_##x(dba_info *info, char *key, size_t keylen)
#define DBA_DELETE_FUNC(x) \
	int dba_delete_##x(dba_info *info, char *key, size_t keylen)
#define DBA_FIRSTKEY_FUNC(x) \
	char *dba_firstkey_##x(dba_info *info, size_t *newlen)
#define DBA_NEXTKEY_FUNC(x) \
	char *dba_nextkey_##x(dba_info *info, size_t *newlen)
#define DBA_OPTIMIZE_FUNC(x) \
	int dba_optimize_##x(dba_info *info)
#define DBA_SYNC_FUNC(x) \
	int dba_sync_##x(dba_info *info)
#define DBA_INFO_FUNC(x) \
	char *dba_info_##x(dba_handler *hnd, dba_info *info)

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
	DBA_SYNC_FUNC(x); \
	DBA_INFO_FUNC(x)

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
PHP_FUNCTION(dba_handlers);
PHP_FUNCTION(dba_list);
PHP_FUNCTION(dba_key_split);

#else
#define dba_module_ptr NULL
#endif

#define phpext_dba_ptr dba_module_ptr

#endif
