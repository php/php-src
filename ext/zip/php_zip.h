/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
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

/* since 1.10.1 */
#ifndef ZIP_LENGTH_TO_END
#define ZIP_LENGTH_TO_END 0
#endif

/* Additional flags not from libzip */
#define ZIP_FL_OPEN_FILE_NOW (1u<<30)

#include "php_version.h"
#define PHP_ZIP_VERSION PHP_VERSION

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
	/* Used to check if the zip resource still exists,
	 * without holding a reference. This works because the IDs are unique. */
	zend_long zip_rsrc_handle;
} zip_read_rsrc;

/* Refcounted holder for the native archive state.
 * Owned by a ZipArchive object and streams opened from it. */
typedef struct _php_zip_archive {
	struct zip *za;
	uint32_t refcount;
	zend_string *out_str;
	bool from_string;
	bool bailout_callback;
#ifdef HAVE_PROGRESS_CALLBACK
	zend_fcall_info_cache progress_callback;
#endif
#ifdef HAVE_CANCEL_CALLBACK
	zend_fcall_info_cache cancel_callback;
#endif
} php_zip_archive;

/* Extends zend object */
typedef struct _ze_zip_object {
	/* NULL when there is no open archive, non-NULL otherwise.
	 * Owns one ref to the struct. */
	php_zip_archive *archive;
	HashTable *prop_handler;
	char *filename;
	size_t filename_len;
	zip_int64_t last_id;
	int err_zip;
	int err_sys;
	zend_object zo;
} ze_zip_object;

#define php_zip_fetch_object(obj) ZEND_CONTAINER_OF(obj, ze_zip_object, zo)

/* The archive an object currently has open, or NULL. */
static zend_always_inline struct zip *php_zip_object_za(const ze_zip_object *obj) {
	return obj->archive ? obj->archive->za : NULL;
}

#define Z_ZIP_P(zv) php_zip_fetch_object(Z_OBJ_P((zv)))

php_stream *php_stream_zip_opener(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
php_stream *php_stream_zip_open(ze_zip_object *obj, struct zip_stat *sb, const char *mode, zip_flags_t flags STREAMS_DC);

void php_zip_archive_addref(php_zip_archive *archive);
bool php_zip_archive_release(php_zip_archive *archive);

extern const php_stream_wrapper php_stream_zip_wrapper;

zip_source_t * php_zip_create_string_source(zend_string *str, zend_string **dest, zip_error_t *err);

#define LIBZIP_ATLEAST(m,n,p) (((m<<16) + (n<<8) + p) <= ((LIBZIP_VERSION_MAJOR<<16) + (LIBZIP_VERSION_MINOR<<8) + LIBZIP_VERSION_MICRO))

#endif	/* PHP_ZIP_H */
