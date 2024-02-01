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
  | Author: Pierre-Alain Joye <pajoye@php.net>                           |
  +----------------------------------------------------------------------+
*/


#ifndef PHP_ZIP_H
#define PHP_ZIP_H

extern zend_module_entry zip_module_entry;
#define phpext_zip_ptr &zip_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#include <zip.h>

#ifndef ZIP_OVERWRITE
#define ZIP_OVERWRITE ZIP_TRUNCATE
#endif

#define PHP_ZIP_VERSION "1.21.1"

#ifdef HAVE_LIBZIP_VERSION
#define LIBZIP_VERSION_STR zip_libzip_version()
#else
#define LIBZIP_VERSION_STR LIBZIP_VERSION
#endif

#define ZIP_OPENBASEDIR_CHECKPATH(filename) php_check_open_basedir(filename)

typedef struct _ze_zip_rsrc {
	struct zip *za;
	zip_uint64_t index_current;
	zip_int64_t num_files;
} zip_rsrc;

typedef zip_rsrc * zip_rsrc_ptr;

typedef struct _ze_zip_read_rsrc {
	struct zip_file *zf;
	struct zip_stat sb;
} zip_read_rsrc;

/* Extends zend object */
typedef struct _ze_zip_object {
	struct zip *za;
	char **buffers;
	HashTable *prop_handler;
	char *filename;
	int filename_len;
	int buffers_cnt;
	zip_int64_t last_id;
	int err_zip;
	int err_sys;
#ifdef HAVE_PROGRESS_CALLBACK
	zval progress_callback;
#endif
#ifdef HAVE_CANCEL_CALLBACK
	zval cancel_callback;
#endif
	zend_object zo;
} ze_zip_object;

static inline ze_zip_object *php_zip_fetch_object(zend_object *obj) {
	return (ze_zip_object *)((char*)(obj) - XtOffsetOf(ze_zip_object, zo));
}

#define Z_ZIP_P(zv) php_zip_fetch_object(Z_OBJ_P((zv)))

php_stream *php_stream_zip_opener(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
php_stream *php_stream_zip_open(struct zip *arch, struct zip_stat *sb, const char *mode, zip_flags_t flags STREAMS_DC);

extern const php_stream_wrapper php_stream_zip_wrapper;

#define LIBZIP_ATLEAST(m,n,p) (((m<<16) + (n<<8) + p) <= ((LIBZIP_VERSION_MAJOR<<16) + (LIBZIP_VERSION_MINOR<<8) + LIBZIP_VERSION_MICRO))

#endif	/* PHP_ZIP_H */
