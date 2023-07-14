/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

#ifdef HAVE_DBA

typedef enum {
	/* do not allow 0 here */
	DBA_READER = 1,
	DBA_WRITER,
	DBA_TRUNC,
	DBA_CREAT
} dba_mode_t;

typedef struct dba_lock {
	php_stream *fp;
	int mode; /* LOCK_EX,LOCK_SH */
} dba_lock;

typedef struct dba_info {
	/* public */
	void *dbf;               /* ptr to private data or whatever */
	zend_string *path;
	dba_mode_t mode;
	php_stream *fp;  /* this is the database stream for builtin handlers */
	int fd;
	int file_permission;
	zend_long map_size;
	/* -1 for default driver flags */
	zend_long driver_flags;
	/* private */
	int flags; /* whether and how dba did locking and other flags*/
	const struct dba_handler *hnd;
	dba_lock lock;
} dba_info;

#define DBA_DEFAULT_DRIVER_FLAGS -1

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
	const char *name; /* handler name */
	int flags; /* whether and how dba does locking and other flags*/
	zend_result (*open)(dba_info *, const char **error);
	void (*close)(dba_info *);
	zend_string* (*fetch)(dba_info *, zend_string *, int);
	zend_result (*update)(dba_info *, zend_string *, zend_string *, int);
	zend_result (*exists)(dba_info *, zend_string *);
	zend_result (*delete)(dba_info *, zend_string *);
	zend_string* (*firstkey)(dba_info *);
	zend_string* (*nextkey)(dba_info *);
	zend_result (*optimize)(dba_info *);
	zend_result (*sync)(dba_info *);
	char* (*info)(const struct dba_handler *hnd, dba_info *);
		/* dba_info==NULL: Handler info, dba_info!=NULL: Database info */
} dba_handler;

/* common prototypes which must be supplied by modules */

#define DBA_OPEN_FUNC(x) \
	zend_result dba_open_##x(dba_info *info, const char **error)
#define DBA_CLOSE_FUNC(x) \
	void dba_close_##x(dba_info *info)
#define DBA_FETCH_FUNC(x) \
	zend_string *dba_fetch_##x(dba_info *info, zend_string *key, int skip)
#define DBA_UPDATE_FUNC(x) \
	zend_result dba_update_##x(dba_info *info, zend_string *key, zend_string *val, int mode)
#define DBA_EXISTS_FUNC(x) \
	zend_result dba_exists_##x(dba_info *info, zend_string *key)
#define DBA_DELETE_FUNC(x) \
	zend_result dba_delete_##x(dba_info *info, zend_string *key)
#define DBA_FIRSTKEY_FUNC(x) \
	zend_string *dba_firstkey_##x(dba_info *info)
#define DBA_NEXTKEY_FUNC(x) \
	zend_string *dba_nextkey_##x(dba_info *info)
#define DBA_OPTIMIZE_FUNC(x) \
	zend_result dba_optimize_##x(dba_info *info)
#define DBA_SYNC_FUNC(x) \
	zend_result dba_sync_##x(dba_info *info)
#define DBA_INFO_FUNC(x) \
	char *dba_info_##x(const dba_handler *hnd, dba_info *info)

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

#else
#define dba_module_ptr NULL
#endif

#define phpext_dba_ptr dba_module_ptr

#endif
