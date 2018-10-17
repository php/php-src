/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
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

#if defined(HAVE_LIBZIP)
#include <zip.h>
#else
#include "lib/zip.h"
#endif

#ifndef ZIP_OVERWRITE
#define ZIP_OVERWRITE ZIP_TRUNCATE
#endif

#define PHP_ZIP_VERSION "1.15.4"

#define ZIP_OPENBASEDIR_CHECKPATH(filename) php_check_open_basedir(filename)

typedef struct _ze_zip_rsrc {
	struct zip *za;
	int index_current;
	int num_files;
} zip_rsrc;

typedef zip_rsrc * zip_rsrc_ptr;

typedef struct _ze_zip_read_rsrc {
	struct zip_file *zf;
	struct zip_stat sb;
} zip_read_rsrc;

#define ZIPARCHIVE_ME(name, arg_info, flags) {#name, c_ziparchive_ ##name, arg_info,(uint32_t) (sizeof(arg_info)/sizeof(struct _zend_arg_info)-1), flags },
#define ZIPARCHIVE_METHOD(name)	ZEND_NAMED_FUNCTION(c_ziparchive_ ##name)

/* Extends zend object */
typedef struct _ze_zip_object {
	struct zip *za;
	char **buffers;
	HashTable *prop_handler;
	char *filename;
	int filename_len;
	int buffers_cnt;
	zend_object zo;
} ze_zip_object;

static inline ze_zip_object *php_zip_fetch_object(zend_object *obj) {
	return (ze_zip_object *)((char*)(obj) - XtOffsetOf(ze_zip_object, zo));
}

#define Z_ZIP_P(zv) php_zip_fetch_object(Z_OBJ_P((zv)))

php_stream *php_stream_zip_opener(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
php_stream *php_stream_zip_open(const char *filename, const char *path, const char *mode STREAMS_DC);

extern const php_stream_wrapper php_stream_zip_wrapper;

#endif	/* PHP_ZIP_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
