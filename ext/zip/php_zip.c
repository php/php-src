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
  | Author: Piere-Alain Joye <pierre@php.net>                            |
  +----------------------------------------------------------------------+
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/pcre/php_pcre.h"
#include "ext/standard/php_filestat.h"
#include "zend_attributes.h"
#include "zend_interfaces.h"
#include "php_zip.h"
#include "php_zip_arginfo.h"

#ifdef HAVE_GLOB
#ifndef PHP_WIN32
#include <glob.h>
#else
#include "win32/glob.h"
#endif
#endif

/* {{{ Resource le */
static int le_zip_dir;
#define le_zip_dir_name "Zip Directory"
static int le_zip_entry;
#define le_zip_entry_name "Zip Entry"
/* }}} */

/* {{{ PHP_ZIP_STAT_INDEX(za, index, flags, sb) */
#define PHP_ZIP_STAT_INDEX(za, index, flags, sb) \
	if (zip_stat_index(za, index, flags, &sb) != 0) { \
		RETURN_FALSE; \
	}
/* }}} */

/* {{{  PHP_ZIP_STAT_PATH(za, path, path_len, flags, sb)
	This is always used for the first argument*/
#define PHP_ZIP_STAT_PATH(za, path, path_len, flags, sb) \
	if (path_len == 0) { \
		zend_argument_value_error(1, "cannot be empty"); \
		RETURN_THROWS(); \
	} \
	if (zip_stat(za, path, flags, &sb) != 0) { \
		RETURN_FALSE; \
	}
/* }}} */

/* {{{ PHP_ZIP_SET_FILE_COMMENT(za, index, comment, comment_len) */
#define PHP_ZIP_SET_FILE_COMMENT(za, index, comment, comment_len) \
	if (comment_len == 0) { \
		/* Passing NULL remove the existing comment */ \
		if (zip_file_set_comment(za, index, NULL, 0, 0) < 0) { \
			RETURN_FALSE; \
		} \
	} else if (zip_file_set_comment(za, index, comment, comment_len, 0) < 0) { \
		RETURN_FALSE; \
	} \
	RETURN_TRUE;
/* }}} */

# define add_ascii_assoc_string add_assoc_string
# define add_ascii_assoc_long add_assoc_long

/* Flatten a path by making a relative path (to .)*/
static char * php_zip_make_relative_path(char *path, size_t path_len) /* {{{ */
{
	char *path_begin = path;
	size_t i;

	if (path_len < 1 || path == NULL) {
		return NULL;
	}

	if (IS_ABSOLUTE_PATH(path, path_len)) {
		return path + COPY_WHEN_ABSOLUTE(path) + 1;
	}

	i = path_len;

	while (1) {
		while (i > 0 && !IS_SLASH(path[i])) {
			i--;
		}

		if (!i) {
			return path;
		}

		if (i >= 2 && path[i -1] == '.') {
			/* i is the position of ., add 1 for / */
			path_begin = path + i + 1;
			break;
		}
		i--;
	}

	return path_begin;
}
/* }}} */

# define CWD_STATE_ALLOC(l) emalloc(l)
# define CWD_STATE_FREE(s)  efree(s)

/* {{{ php_zip_extract_file */
static int php_zip_extract_file(struct zip * za, char *dest, const char *file, size_t file_len, zip_int64_t idx)
{
	php_stream_statbuf ssb;
	struct zip_file *zf;
	struct zip_stat sb;
	char b[8192];
	int n, ret;
	php_stream *stream;
	char *fullpath;
	char *file_dirname_fullpath;
	char file_dirname[MAXPATHLEN];
	size_t dir_len, len;
	int is_dir_only = 0;
	char *path_cleaned;
	size_t path_cleaned_len;
	cwd_state new_state;
	zend_string *file_basename;

	new_state.cwd = CWD_STATE_ALLOC(1);
	new_state.cwd[0] = '\0';
	new_state.cwd_length = 0;

	if (idx <0) {
		idx = zip_name_locate(za, file, 0);
		if (idx < 0) {
			return 0;
		}
	}
	/* Clean/normlize the path and then transform any path (absolute or relative)
		 to a path relative to cwd (../../mydir/foo.txt > mydir/foo.txt)
	 */
	virtual_file_ex(&new_state, file, NULL, CWD_EXPAND);
	path_cleaned =  php_zip_make_relative_path(new_state.cwd, new_state.cwd_length);
	if(!path_cleaned) {
		CWD_STATE_FREE(new_state.cwd);
		return 0;
	}
	path_cleaned_len = strlen(path_cleaned);

	if (path_cleaned_len >= MAXPATHLEN || zip_stat_index(za, idx, 0, &sb) != 0) {
		CWD_STATE_FREE(new_state.cwd);
		return 0;
	}

	/* it is a directory only, see #40228 */
	if (path_cleaned_len > 1 && IS_SLASH(path_cleaned[path_cleaned_len - 1])) {
		len = spprintf(&file_dirname_fullpath, 0, "%s/%s", dest, path_cleaned);
		is_dir_only = 1;
	} else {
		memcpy(file_dirname, path_cleaned, path_cleaned_len);
		dir_len = php_dirname(file_dirname, path_cleaned_len);

		if (!dir_len || (dir_len == 1 && file_dirname[0] == '.')) {
			len = spprintf(&file_dirname_fullpath, 0, "%s", dest);
		} else {
			len = spprintf(&file_dirname_fullpath, 0, "%s/%s", dest, file_dirname);
		}

		file_basename =	php_basename(path_cleaned, path_cleaned_len, NULL, 0);

		if (ZIP_OPENBASEDIR_CHECKPATH(file_dirname_fullpath)) {
			efree(file_dirname_fullpath);
			zend_string_release_ex(file_basename, 0);
			CWD_STATE_FREE(new_state.cwd);
			return 0;
		}
	}

	/* let see if the path already exists */
	if (php_stream_stat_path_ex(file_dirname_fullpath, PHP_STREAM_URL_STAT_QUIET, &ssb, NULL) < 0) {
		ret = php_stream_mkdir(file_dirname_fullpath, 0777,  PHP_STREAM_MKDIR_RECURSIVE|REPORT_ERRORS, NULL);
		if (!ret) {
			efree(file_dirname_fullpath);
			if (!is_dir_only) {
				zend_string_release_ex(file_basename, 0);
			}
			CWD_STATE_FREE(new_state.cwd);
			return 0;
		}
	}

	/* it is a standalone directory, job done */
	if (is_dir_only) {
#ifdef ZIP_OPSYS_DEFAULT
		php_stream_wrapper *wrapper;
		zip_uint8_t opsys;
		zip_uint32_t attr;
		zend_long mode;

		if (zip_file_get_external_attributes(za, idx, 0, &opsys, &attr) >= 0 && opsys == ZIP_OPSYS_UNIX) {
			wrapper = php_stream_locate_url_wrapper(file_dirname_fullpath, NULL, 0);
			if(wrapper && wrapper->wops->stream_metadata) {
				mode = (attr >> 16) & 0777;
				wrapper->wops->stream_metadata(wrapper, file_dirname_fullpath, PHP_STREAM_META_ACCESS, &mode, NULL);
			}
		}
#endif
		efree(file_dirname_fullpath);
		CWD_STATE_FREE(new_state.cwd);
		return 1;
	}

	len = spprintf(&fullpath, 0, "%s/%s", file_dirname_fullpath, ZSTR_VAL(file_basename));
	if (!len) {
		efree(file_dirname_fullpath);
		zend_string_release_ex(file_basename, 0);
		CWD_STATE_FREE(new_state.cwd);
		return 0;
	} else if (len > MAXPATHLEN) {
		php_error_docref(NULL, E_WARNING, "Full extraction path exceed MAXPATHLEN (%i)", MAXPATHLEN);
		efree(file_dirname_fullpath);
		zend_string_release_ex(file_basename, 0);
		CWD_STATE_FREE(new_state.cwd);
		return 0;
	}

	/* check again the full path, not sure if it
	 * is required, does a file can have a different
	 * safemode status as its parent folder?
	 */
	if (ZIP_OPENBASEDIR_CHECKPATH(fullpath)) {
		efree(fullpath);
		efree(file_dirname_fullpath);
		zend_string_release_ex(file_basename, 0);
		CWD_STATE_FREE(new_state.cwd);
		return 0;
	}

	zf = zip_fopen_index(za, idx, 0);
	if (zf == NULL) {
		n = -1;
		goto done;
	}

	stream = php_stream_open_wrapper(fullpath, "w+b", REPORT_ERRORS, NULL);

	if (stream == NULL) {
		n = -1;
		zip_fclose(zf);
		goto done;
	}

	n = 0;

	while ((n=zip_fread(zf, b, sizeof(b))) > 0) {
		php_stream_write(stream, b, n);
	}

	if (stream->wrapper->wops->stream_metadata) {
		struct utimbuf ut;
#ifdef ZIP_OPSYS_DEFAULT
		zip_uint8_t opsys;
		zip_uint32_t attr;
		zend_long mode;

		if (zip_file_get_external_attributes(za, idx, 0, &opsys, &attr) >= 0
			&& opsys == ZIP_OPSYS_UNIX) {
			mode = (attr >> 16) & 0777;
			stream->wrapper->wops->stream_metadata(stream->wrapper, fullpath, PHP_STREAM_META_ACCESS, &mode, NULL);
		}
#endif
		ut.modtime = ut.actime = sb.mtime;
		stream->wrapper->wops->stream_metadata(stream->wrapper, fullpath, PHP_STREAM_META_TOUCH, &ut, NULL);
	}

	php_stream_close(stream);
	n = zip_fclose(zf);

done:
	efree(fullpath);
	zend_string_release_ex(file_basename, 0);
	efree(file_dirname_fullpath);
	CWD_STATE_FREE(new_state.cwd);

	if (n<0) {
		return 0;
	} else {
		return 1;
	}
}
/* }}} */

static int php_zip_add_file(ze_zip_object *obj, const char *filename, size_t filename_len,
	char *entry_name, size_t entry_name_len, /* unused if replace >= 0 */
	zip_uint64_t offset_start, zip_uint64_t offset_len,
	zend_long replace, /* index to replace, add new file if < 0 */
	zip_flags_t flags
) /* {{{ */
{
	struct zip_source *zs;
	char resolved_path[MAXPATHLEN];
	php_stream_statbuf ssb;

	if (ZIP_OPENBASEDIR_CHECKPATH(filename)) {
		return -1;
	}

	if (!expand_filepath(filename, resolved_path)) {
		php_error_docref(NULL, E_WARNING, "No such file or directory");
		return -1;
	}

	if (php_stream_stat_path_ex(resolved_path, PHP_STREAM_URL_STAT_QUIET, &ssb, NULL)) {
		php_error_docref(NULL, E_WARNING, "No such file or directory");
		return -1;
	}

	if (flags & ZIP_FL_OPEN_FILE_NOW) {
		FILE *fd;
		fd = fopen(resolved_path, "rb");
		if (!fd) {
			return -1;
		}
		flags ^= ZIP_FL_OPEN_FILE_NOW;
		zs = zip_source_filep(obj->za, fd, offset_start, offset_len);
	} else {
		zs = zip_source_file(obj->za, resolved_path, offset_start, offset_len);
	}
	if (!zs) {
		return -1;
	}
	/* Replace */
	if (replace >= 0) {
		if (zip_file_replace(obj->za, replace, zs, flags) < 0) {
			zip_source_free(zs);
			return -1;
		}
		zip_error_clear(obj->za);
		return 1;
	}
	/* Add */
	obj->last_id = zip_file_add(obj->za, entry_name, zs, flags);
	if (obj->last_id < 0) {
		zip_source_free(zs);
		return -1;
	}
	zip_error_clear(obj->za);
	return 1;
}
/* }}} */

typedef struct {
	zend_long    remove_all_path;
	char        *remove_path;
	size_t       remove_path_len;
	char        *add_path;
	size_t       add_path_len;
	zip_flags_t  flags;
	zip_int32_t  comp_method;
	zip_uint32_t comp_flags;
#ifdef HAVE_ENCRYPTION
	zip_int16_t  enc_method;
	char        *enc_password;
#endif
} zip_options;

static int php_zip_parse_options(HashTable *options, zip_options *opts)
/* {{{ */
{
	zval *option;

	/* default values */
	memset(opts, 0, sizeof(zip_options));
	opts->flags = ZIP_FL_OVERWRITE;
	opts->comp_method = -1; /* -1 to not change default */
#ifdef HAVE_ENCRYPTION
	opts->enc_method = -1;  /* -1 to not change default */
#endif

	if ((option = zend_hash_str_find(options, "remove_all_path", sizeof("remove_all_path") - 1)) != NULL) {
		if (Z_TYPE_P(option) != IS_FALSE && Z_TYPE_P(option) != IS_TRUE) {
			php_error_docref(NULL, E_WARNING, "Option \"remove_all_path\" must be of type bool, %s given",
				zend_zval_value_name(option));
		}
		opts->remove_all_path = zval_get_long(option);
	}

	if ((option = zend_hash_str_find(options, "comp_method", sizeof("comp_method") - 1)) != NULL) {
		if (Z_TYPE_P(option) != IS_LONG) {
			php_error_docref(NULL, E_WARNING, "Option \"comp_method\" must be of type int, %s given",
				zend_zval_value_name(option));
		}
		opts->comp_method = zval_get_long(option);

		if ((option = zend_hash_str_find(options, "comp_flags", sizeof("comp_flags") - 1)) != NULL) {
			if (Z_TYPE_P(option) != IS_LONG) {
				php_error_docref(NULL, E_WARNING, "Option \"comp_flags\" must be of type int, %s given",
					zend_zval_value_name(option));
			}
			opts->comp_flags = zval_get_long(option);
		}
	}

#ifdef HAVE_ENCRYPTION
	if ((option = zend_hash_str_find(options, "enc_method", sizeof("enc_method") - 1)) != NULL) {
		if (Z_TYPE_P(option) != IS_LONG) {
			php_error_docref(NULL, E_WARNING, "Option \"enc_method\" must be of type int, %s given",
				zend_zval_value_name(option));
		}
		opts->enc_method = zval_get_long(option);

		if ((option = zend_hash_str_find(options, "enc_password", sizeof("enc_password") - 1)) != NULL) {
			if (Z_TYPE_P(option) != IS_STRING) {
				zend_type_error("Option \"enc_password\" must be of type string, %s given",
					zend_zval_value_name(option));
				return -1;
			}
			opts->enc_password = Z_STRVAL_P(option);
		}
	}
#endif

	if ((option = zend_hash_str_find(options, "remove_path", sizeof("remove_path") - 1)) != NULL) {
		if (Z_TYPE_P(option) != IS_STRING) {
			zend_type_error("Option \"remove_path\" must be of type string, %s given",
				zend_zval_value_name(option));
			return -1;
		}

		if (Z_STRLEN_P(option) == 0) {
			zend_value_error("Option \"remove_path\" cannot be empty");
			return -1;
		}

		if (Z_STRLEN_P(option) >= MAXPATHLEN) {
			zend_value_error("Option \"remove_path\" must be less than %d bytes", MAXPATHLEN - 1);
			return -1;
		}
		opts->remove_path_len = Z_STRLEN_P(option);
		opts->remove_path = Z_STRVAL_P(option);
	}

	if ((option = zend_hash_str_find(options, "add_path", sizeof("add_path") - 1)) != NULL) {
		if (Z_TYPE_P(option) != IS_STRING) {
			zend_type_error("Option \"add_path\" must be of type string, %s given",
				zend_zval_value_name(option));
			return -1;
		}

		if (Z_STRLEN_P(option) == 0) {
			zend_value_error("Option \"add_path\" cannot be empty");
			return -1;
		}

		if (Z_STRLEN_P(option) >= MAXPATHLEN) {
			zend_value_error("Option \"add_path\" must be less than %d bytes", MAXPATHLEN - 1);
			return -1;
		}
		opts->add_path_len = Z_STRLEN_P(option);
		opts->add_path = Z_STRVAL_P(option);
	}

	if ((option = zend_hash_str_find(options, "flags", sizeof("flags") - 1)) != NULL) {
		if (Z_TYPE_P(option) != IS_LONG) {
			zend_type_error("Option \"flags\" must be of type int, %s given",
				zend_zval_value_name(option));
			return -1;
		}
		opts->flags = Z_LVAL_P(option);
	}

	return 1;
}
/* }}} */

/* {{{ ZIP_FROM_OBJECT */
#define ZIP_FROM_OBJECT(intern, object) \
	{ \
		ze_zip_object *obj = Z_ZIP_P(object); \
		intern = obj->za; \
		if (!intern) { \
			zend_value_error("Invalid or uninitialized Zip object"); \
			RETURN_THROWS(); \
		} \
	}
/* }}} */

/* {{{ RETURN_SB(sb) */
#ifdef HAVE_ENCRYPTION
#define RETURN_SB(sb) \
	{ \
		array_init(return_value); \
		add_ascii_assoc_string(return_value, "name", (char *)(sb)->name); \
		add_ascii_assoc_long(return_value, "index", (zend_long) (sb)->index); \
		add_ascii_assoc_long(return_value, "crc", (zend_long) (sb)->crc); \
		add_ascii_assoc_long(return_value, "size", (zend_long) (sb)->size); \
		add_ascii_assoc_long(return_value, "mtime", (zend_long) (sb)->mtime); \
		add_ascii_assoc_long(return_value, "comp_size", (zend_long) (sb)->comp_size); \
		add_ascii_assoc_long(return_value, "comp_method", (zend_long) (sb)->comp_method); \
		add_ascii_assoc_long(return_value, "encryption_method", (zend_long) (sb)->encryption_method); \
	}
#else
#define RETURN_SB(sb) \
	{ \
		array_init(return_value); \
		add_ascii_assoc_string(return_value, "name", (char *)(sb)->name); \
		add_ascii_assoc_long(return_value, "index", (zend_long) (sb)->index); \
		add_ascii_assoc_long(return_value, "crc", (zend_long) (sb)->crc); \
		add_ascii_assoc_long(return_value, "size", (zend_long) (sb)->size); \
		add_ascii_assoc_long(return_value, "mtime", (zend_long) (sb)->mtime); \
		add_ascii_assoc_long(return_value, "comp_size", (zend_long) (sb)->comp_size); \
		add_ascii_assoc_long(return_value, "comp_method", (zend_long) (sb)->comp_method); \
	}
#endif
/* }}} */

static zend_long php_zip_status(ze_zip_object *obj) /* {{{ */
{
	int zep = obj->err_zip; /* saved err if closed */

	if (obj->za) {
#if LIBZIP_VERSION_MAJOR < 1
		int syp;

		zip_error_get(obj->za, &zep, &syp);
#else
		zip_error_t *err;

		err = zip_get_error(obj->za);
		zep = zip_error_code_zip(err);
		zip_error_fini(err);
#endif
	}
	return zep;
}
/* }}} */

static zend_long php_zip_last_id(ze_zip_object *obj) /* {{{ */
{
	return obj->last_id;
}
/* }}} */

static zend_long php_zip_status_sys(ze_zip_object *obj) /* {{{ */
{
	int syp = obj->err_sys;  /* saved err if closed */

	if (obj->za) {
#if LIBZIP_VERSION_MAJOR < 1
		int zep;

		zip_error_get(obj->za, &zep, &syp);
#else
		zip_error_t *err;

		err = zip_get_error(obj->za);
		syp = zip_error_code_system(err);
		zip_error_fini(err);
#endif
	}
	return syp;
}
/* }}} */

static zend_long php_zip_get_num_files(ze_zip_object *obj) /* {{{ */
{
	if (obj->za) {
		zip_int64_t num = zip_get_num_entries(obj->za, 0);
		return MIN(num, ZEND_LONG_MAX);
	}
	return 0;
}
/* }}} */

static char * php_zipobj_get_filename(ze_zip_object *obj, int *len) /* {{{ */
{
	if (obj && obj->filename) {
		*len = strlen(obj->filename);
		return obj->filename;
	}
	return NULL;
}
/* }}} */

static char * php_zipobj_get_zip_comment(ze_zip_object *obj, int *len) /* {{{ */
{
	if (obj->za) {
		return (char *)zip_get_archive_comment(obj->za, len, 0);
	}
	return NULL;
}
/* }}} */

#ifdef HAVE_GLOB /* {{{ */
#ifndef GLOB_ONLYDIR
#define GLOB_ONLYDIR (1<<30)
#define GLOB_EMULATE_ONLYDIR
#define GLOB_FLAGMASK (~GLOB_ONLYDIR)
#else
#define GLOB_FLAGMASK (~0)
#endif
#ifndef GLOB_BRACE
# define GLOB_BRACE 0
#endif
#ifndef GLOB_MARK
# define GLOB_MARK 0
#endif
#ifndef GLOB_NOSORT
# define GLOB_NOSORT 0
#endif
#ifndef GLOB_NOCHECK
# define GLOB_NOCHECK 0
#endif
#ifndef GLOB_NOESCAPE
# define GLOB_NOESCAPE 0
#endif
#ifndef GLOB_ERR
# define GLOB_ERR 0
#endif

/* This is used for checking validity of passed flags (passing invalid flags causes segfault in glob()!! */
#define GLOB_AVAILABLE_FLAGS (0 | GLOB_BRACE | GLOB_MARK | GLOB_NOSORT | GLOB_NOCHECK | GLOB_NOESCAPE | GLOB_ERR | GLOB_ONLYDIR)

#endif /* }}} */

int php_zip_glob(char *pattern, int pattern_len, zend_long flags, zval *return_value) /* {{{ */
{
#ifdef HAVE_GLOB
	int cwd_skip = 0;
#ifdef ZTS
	char cwd[MAXPATHLEN];
	char work_pattern[MAXPATHLEN];
	char *result;
#endif
	glob_t globbuf;
	size_t n;
	int ret;

	if (pattern_len >= MAXPATHLEN) {
		php_error_docref(NULL, E_WARNING, "Pattern exceeds the maximum allowed length of %d characters", MAXPATHLEN);
		return -1;
	}

	if ((GLOB_AVAILABLE_FLAGS & flags) != flags) {

		php_error_docref(NULL, E_WARNING, "At least one of the passed flags is invalid or not supported on this platform");
		return -1;
	}

#ifdef ZTS
	if (!IS_ABSOLUTE_PATH(pattern, pattern_len)) {
		result = VCWD_GETCWD(cwd, MAXPATHLEN);
		if (!result) {
			cwd[0] = '\0';
		}
#ifdef PHP_WIN32
		if (IS_SLASH(*pattern)) {
			cwd[2] = '\0';
		}
#endif
		cwd_skip = strlen(cwd)+1;

		snprintf(work_pattern, MAXPATHLEN, "%s%c%s", cwd, DEFAULT_SLASH, pattern);
		pattern = work_pattern;
	}
#endif

	globbuf.gl_offs = 0;
	if (0 != (ret = glob(pattern, flags & GLOB_FLAGMASK, NULL, &globbuf))) {
#ifdef GLOB_NOMATCH
		if (GLOB_NOMATCH == ret) {
			/* Some glob implementation simply return no data if no matches
			   were found, others return the GLOB_NOMATCH error code.
			   We don't want to treat GLOB_NOMATCH as an error condition
			   so that PHP glob() behaves the same on both types of
			   implementations and so that 'foreach (glob() as ...'
			   can be used for simple glob() calls without further error
			   checking.
			*/
			array_init(return_value);
			return 0;
		}
#endif
		return 0;
	}

	/* now catch the FreeBSD style of "no matches" */
	if (!globbuf.gl_pathc || !globbuf.gl_pathv) {
		array_init(return_value);
		return 0;
	}

	/* we assume that any glob pattern will match files from one directory only
	   so checking the dirname of the first match should be sufficient */
	if (ZIP_OPENBASEDIR_CHECKPATH(globbuf.gl_pathv[0])) {
		return -1;
	}

	array_init(return_value);
	for (n = 0; n < globbuf.gl_pathc; n++) {
		/* we need to do this every time since GLOB_ONLYDIR does not guarantee that
		 * all directories will be filtered. GNU libc documentation states the
		 * following:
		 * If the information about the type of the file is easily available
		 * non-directories will be rejected but no extra work will be done to
		 * determine the information for each file. I.e., the caller must still be
		 * able to filter directories out.
		 */
		if (flags & GLOB_ONLYDIR) {
			zend_stat_t s = {0};

			if (0 != VCWD_STAT(globbuf.gl_pathv[n], &s)) {
				continue;
			}

			if (S_IFDIR != (s.st_mode & S_IFMT)) {
				continue;
			}
		}
		add_next_index_string(return_value, globbuf.gl_pathv[n]+cwd_skip);
	}

	ret = globbuf.gl_pathc;
	globfree(&globbuf);
	return ret;
#else
	zend_throw_error(NULL, "Glob support is not available");
	return 0;
#endif  /* HAVE_GLOB */
}
/* }}} */

int php_zip_pcre(zend_string *regexp, char *path, int path_len, zval *return_value) /* {{{ */
{
#ifdef ZTS
	char cwd[MAXPATHLEN];
	char work_path[MAXPATHLEN];
	char *result;
#endif
	int files_cnt;
	zend_string **namelist;
	pcre2_match_context *mctx = php_pcre_mctx();

#ifdef ZTS
	if (!IS_ABSOLUTE_PATH(path, path_len)) {
		result = VCWD_GETCWD(cwd, MAXPATHLEN);
		if (!result) {
			cwd[0] = '\0';
		}
#ifdef PHP_WIN32
		if (IS_SLASH(*path)) {
			cwd[2] = '\0';
		}
#endif
		snprintf(work_path, MAXPATHLEN, "%s%c%s", cwd, DEFAULT_SLASH, path);
		path = work_path;
	}
#endif

	if (ZIP_OPENBASEDIR_CHECKPATH(path)) {
		return -1;
	}

	files_cnt = php_stream_scandir(path, &namelist, NULL, (void *) php_stream_dirent_alphasort);

	if (files_cnt > 0) {
		pcre2_code *re = NULL;
		pcre2_match_data *match_data = NULL;
		uint32_t i, capture_count;
		int rc;

		re = pcre_get_compiled_regex(regexp, &capture_count);
		if (!re) {
			php_error_docref(NULL, E_WARNING, "Invalid expression");
			return -1;
		}

		array_init(return_value);

		/* only the files, directories are ignored */
		for (i = 0; i < files_cnt; i++) {
			zend_stat_t s = {0};
			char   fullpath[MAXPATHLEN];
			size_t    namelist_len = ZSTR_LEN(namelist[i]);

			if ((namelist_len == 1 && ZSTR_VAL(namelist[i])[0] == '.') ||
				(namelist_len == 2 && ZSTR_VAL(namelist[i])[0] == '.' && ZSTR_VAL(namelist[i])[1] == '.')) {
				zend_string_release_ex(namelist[i], 0);
				continue;
			}

			if ((path_len + namelist_len + 1) >= MAXPATHLEN) {
				php_error_docref(NULL, E_WARNING, "add_path string too long (max: %u, %zu given)",
						MAXPATHLEN - 1, (path_len + namelist_len + 1));
				zend_string_release_ex(namelist[i], 0);
				break;
			}

			match_data = php_pcre_create_match_data(capture_count, re);
			if (!match_data) {
				/* Allocation failed, but can proceed to the next pattern. */
				zend_string_release_ex(namelist[i], 0);
				continue;
			}
			rc = pcre2_match(re, (PCRE2_SPTR)ZSTR_VAL(namelist[i]), ZSTR_LEN(namelist[i]), 0, 0, match_data, mctx);
			php_pcre_free_match_data(match_data);
			/* 0 means that the vector is too small to hold all the captured substring offsets */
			if (rc < 0) {
				zend_string_release_ex(namelist[i], 0);
				continue;
			}

			snprintf(fullpath, MAXPATHLEN, "%s%c%s", path, DEFAULT_SLASH, ZSTR_VAL(namelist[i]));

			if (0 != VCWD_STAT(fullpath, &s)) {
				php_error_docref(NULL, E_WARNING, "Cannot read <%s>", fullpath);
				zend_string_release_ex(namelist[i], 0);
				continue;
			}

			if (S_IFDIR == (s.st_mode & S_IFMT)) {
				zend_string_release_ex(namelist[i], 0);
				continue;
			}

			add_next_index_string(return_value, fullpath);
			zend_string_release_ex(namelist[i], 0);
		}
		efree(namelist);
	}
	return files_cnt;
}
/* }}} */

/* {{{ ZE2 OO definitions */
static zend_class_entry *zip_class_entry;
static zend_object_handlers zip_object_handlers;

static HashTable zip_prop_handlers;

typedef zend_long (*zip_read_int_t)(ze_zip_object *obj);
typedef char *(*zip_read_const_char_t)(ze_zip_object *obj, int *len);

typedef struct _zip_prop_handler {
	zip_read_int_t read_int_func;
	zip_read_const_char_t read_const_char_func;

	int type;
} zip_prop_handler;
/* }}} */

static void php_zip_register_prop_handler(HashTable *prop_handler, char *name, zip_read_int_t read_int_func, zip_read_const_char_t read_char_func, int rettype) /* {{{ */
{
	zip_prop_handler hnd;
	zend_string *str;

	hnd.read_const_char_func = read_char_func;
	hnd.read_int_func = read_int_func;
	hnd.type = rettype;
	str = zend_string_init_interned(name, strlen(name), 1);
	zend_hash_add_mem(prop_handler, str, &hnd, sizeof(zip_prop_handler));
	zend_string_release_ex(str, 1);
}
/* }}} */

static zval *php_zip_property_reader(ze_zip_object *obj, zip_prop_handler *hnd, zval *rv) /* {{{ */
{
	const char *retchar = NULL;
	zend_long retint = 0;
	int len = 0;

	if (hnd->read_const_char_func) {
		retchar = hnd->read_const_char_func(obj, &len);
	} else if (hnd->read_int_func) {
		retint = hnd->read_int_func(obj);
	}

	switch (hnd->type) {
		case IS_STRING:
			if (retchar) {
				ZVAL_STRINGL(rv, (char *) retchar, len);
			} else {
				ZVAL_EMPTY_STRING(rv);
			}
			break;
		case IS_LONG:
			ZVAL_LONG(rv, retint);
			break;
		default:
			ZVAL_NULL(rv);
	}

	return rv;
}
/* }}} */

static zval *php_zip_get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot) /* {{{ */
{
	ze_zip_object *obj;
	zval *retval = NULL;
	zip_prop_handler *hnd = NULL;

	obj = php_zip_fetch_object(object);

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}

	if (hnd == NULL) {
		retval = zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
	}

	return retval;
}
/* }}} */


static zval *php_zip_write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	ze_zip_object *obj;
	zip_prop_handler *hnd = NULL;

	obj = php_zip_fetch_object(object);

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}

	if (hnd != NULL) {
		zend_throw_error(NULL, "Cannot write read-only property %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
		return &EG(error_zval);
	}

	return zend_std_write_property(object, name, value, cache_slot);
}

static zval *php_zip_read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv) /* {{{ */
{
	ze_zip_object *obj;
	zval *retval = NULL;
	zip_prop_handler *hnd = NULL;

	obj = php_zip_fetch_object(object);

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}

	if (hnd != NULL) {
		retval = php_zip_property_reader(obj, hnd, rv);
		if (retval == NULL) {
			retval = &EG(uninitialized_zval);
		}
	} else {
		retval = zend_std_read_property(object, name, type, cache_slot, rv);
	}

	return retval;
}
/* }}} */

static int php_zip_has_property(zend_object *object, zend_string *name, int type, void **cache_slot) /* {{{ */
{
	ze_zip_object *obj;
	zip_prop_handler *hnd = NULL;
	int retval = 0;

	obj = php_zip_fetch_object(object);

	if (obj->prop_handler != NULL) {
		hnd = zend_hash_find_ptr(obj->prop_handler, name);
	}

	if (hnd != NULL) {
		zval tmp, *prop;

		if (type == 2) {
			retval = 1;
		} else if ((prop = php_zip_property_reader(obj, hnd, &tmp)) != NULL) {
			if (type == 1) {
				retval = zend_is_true(&tmp);
			} else if (type == 0) {
				retval = (Z_TYPE(tmp) != IS_NULL);
			}
		}

		zval_ptr_dtor(&tmp);
	} else {
		retval = zend_std_has_property(object, name, type, cache_slot);
	}

	return retval;
}
/* }}} */

static HashTable *php_zip_get_gc(zend_object *object, zval **gc_data, int *gc_data_count) /* {{{ */
{
	*gc_data = NULL;
	*gc_data_count = 0;
	return zend_std_get_properties(object);
}
/* }}} */

static HashTable *php_zip_get_properties(zend_object *object)/* {{{ */
{
	ze_zip_object *obj;
	HashTable *props;
	zip_prop_handler *hnd;
	zend_string *key;

	obj = php_zip_fetch_object(object);
	props = zend_std_get_properties(object);

	if (obj->prop_handler == NULL) {
		return NULL;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(obj->prop_handler, key, hnd) {
		zval *ret, val;
		ret = php_zip_property_reader(obj, hnd, &val);
		if (ret == NULL) {
			ret = &EG(uninitialized_zval);
		}
		zend_hash_update(props, key, ret);
	} ZEND_HASH_FOREACH_END();

	return props;
}
/* }}} */

#ifdef HAVE_PROGRESS_CALLBACK
static void _php_zip_progress_callback_free(void *ptr)
{
	ze_zip_object *obj = ptr;

	if (!Z_ISUNDEF(obj->progress_callback)) {
		zval_ptr_dtor(&obj->progress_callback);
		ZVAL_UNDEF(&obj->progress_callback);
	}
}
#endif

#ifdef HAVE_CANCEL_CALLBACK
static void _php_zip_cancel_callback_free(void *ptr)
{
	ze_zip_object *obj = ptr;

	if (!Z_ISUNDEF(obj->cancel_callback)) {
		zval_ptr_dtor(&obj->cancel_callback);
		ZVAL_UNDEF(&obj->cancel_callback);
	}
}
#endif

static void php_zip_object_free_storage(zend_object *object) /* {{{ */
{
	ze_zip_object * intern = php_zip_fetch_object(object);
	int i;

	if (!intern) {
		return;
	}
	if (intern->za) {
		if (zip_close(intern->za) != 0) {
			php_error_docref(NULL, E_WARNING, "Cannot destroy the zip context: %s", zip_strerror(intern->za));
			zip_discard(intern->za);
		}
	}

	if (intern->buffers_cnt>0) {
		for (i=0; i<intern->buffers_cnt; i++) {
			efree(intern->buffers[i]);
		}
		efree(intern->buffers);
	}

#ifdef HAVE_PROGRESS_CALLBACK
	/* if not properly called by libzip */
	_php_zip_progress_callback_free(intern);
#endif

#ifdef HAVE_CANCEL_CALLBACK
	/* if not properly called by libzip */
	_php_zip_cancel_callback_free(intern);
#endif

	intern->za = NULL;
	zend_object_std_dtor(&intern->zo);

	if (intern->filename) {
		efree(intern->filename);
	}
}
/* }}} */

static zend_object *php_zip_object_new(zend_class_entry *class_type) /* {{{ */
{
	ze_zip_object *intern;

	intern = zend_object_alloc(sizeof(ze_zip_object), class_type);
	intern->prop_handler = &zip_prop_handlers;
	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);
	intern->last_id = -1;

	return &intern->zo;
}
/* }}} */

/* {{{ Resource dtors */

/* {{{ php_zip_free_dir */
static void php_zip_free_dir(zend_resource *rsrc)
{
	zip_rsrc * zip_int = (zip_rsrc *) rsrc->ptr;

	if (zip_int) {
		if (zip_int->za) {
			if (zip_close(zip_int->za) != 0) {
				php_error_docref(NULL, E_WARNING, "Cannot destroy the zip context");
			}
			zip_int->za = NULL;
		}

		efree(rsrc->ptr);

		rsrc->ptr = NULL;
	}
}
/* }}} */

/* {{{ php_zip_free_entry */
static void php_zip_free_entry(zend_resource *rsrc)
{
	zip_read_rsrc *zr_rsrc = (zip_read_rsrc *) rsrc->ptr;

	if (zr_rsrc) {
		if (zr_rsrc->zf) {
			zip_fclose(zr_rsrc->zf);
			zr_rsrc->zf = NULL;
		}
		efree(zr_rsrc);
		rsrc->ptr = NULL;
	}
}
/* }}} */

/* }}}*/

/* reset macro */

/* {{{ function prototypes */
static PHP_MINIT_FUNCTION(zip);
static PHP_MSHUTDOWN_FUNCTION(zip);
static PHP_MINFO_FUNCTION(zip);
/* }}} */

/* {{{ zip_module_entry */
zend_module_entry zip_module_entry = {
	STANDARD_MODULE_HEADER,
	"zip",
	ext_functions,
	PHP_MINIT(zip),
	PHP_MSHUTDOWN(zip),
	NULL,
	NULL,
	PHP_MINFO(zip),
	PHP_ZIP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ZIP
ZEND_GET_MODULE(zip)
#endif
/* set macro */

/* {{{ Create new zip using source uri for output */
PHP_FUNCTION(zip_open)
{
	char resolved_path[MAXPATHLEN + 1];
	zip_rsrc *rsrc_int;
	int err = 0;
	zend_string *filename;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P", &filename) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(filename) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (ZIP_OPENBASEDIR_CHECKPATH(ZSTR_VAL(filename))) {
		RETURN_FALSE;
	}

	if(!expand_filepath(ZSTR_VAL(filename), resolved_path)) {
		php_error_docref(NULL, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	rsrc_int = (zip_rsrc *)emalloc(sizeof(zip_rsrc));

	rsrc_int->za = zip_open(resolved_path, 0, &err);
	if (rsrc_int->za == NULL) {
		efree(rsrc_int);
		RETURN_LONG((zend_long)err);
	}

	rsrc_int->index_current = 0;
	rsrc_int->num_files = zip_get_num_entries(rsrc_int->za, 0);

	RETURN_RES(zend_register_resource(rsrc_int, le_zip_dir));
}
/* }}} */

/* {{{ Close a Zip archive */
PHP_FUNCTION(zip_close)
{
	zval * zip;
	zip_rsrc *z_rsrc = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zip) == FAILURE) {
		RETURN_THROWS();
	}

	if ((z_rsrc = (zip_rsrc *)zend_fetch_resource(Z_RES_P(zip), le_zip_dir_name, le_zip_dir)) == NULL) {
		RETURN_THROWS();
	}

	/* really close the zip will break BC :-D */
	zend_list_close(Z_RES_P(zip));
}
/* }}} */

/* {{{ Returns the next file in the archive */
PHP_FUNCTION(zip_read)
{
	zval *zip_dp;
	zip_read_rsrc *zr_rsrc;
	int ret;
	zip_rsrc *rsrc_int;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zip_dp) == FAILURE) {
		RETURN_THROWS();
	}

	if ((rsrc_int = (zip_rsrc *)zend_fetch_resource(Z_RES_P(zip_dp), le_zip_dir_name, le_zip_dir)) == NULL) {
		RETURN_THROWS();
	}

	if (rsrc_int && rsrc_int->za) {
		if (rsrc_int->index_current >= rsrc_int->num_files) {
			RETURN_FALSE;
		}

		zr_rsrc = emalloc(sizeof(zip_read_rsrc));

		ret = zip_stat_index(rsrc_int->za, rsrc_int->index_current, 0, &zr_rsrc->sb);

		if (ret != 0) {
			efree(zr_rsrc);
			RETURN_FALSE;
		}

		zr_rsrc->zf = zip_fopen_index(rsrc_int->za, rsrc_int->index_current, 0);
		if (zr_rsrc->zf) {
			rsrc_int->index_current++;
			RETURN_RES(zend_register_resource(zr_rsrc, le_zip_entry));
		} else {
			efree(zr_rsrc);
			RETURN_FALSE;
		}

	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Open a Zip File, pointed by the resource entry */
/* Dummy function to follow the old API */
PHP_FUNCTION(zip_entry_open)
{
	zval * zip;
	zval * zip_entry;
	char *mode = NULL;
	size_t mode_len = 0;
	zip_read_rsrc * zr_rsrc;
	zip_rsrc *z_rsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr|s", &zip, &zip_entry, &mode, &mode_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((zr_rsrc = (zip_read_rsrc *)zend_fetch_resource(Z_RES_P(zip_entry), le_zip_entry_name, le_zip_entry)) == NULL) {
		RETURN_THROWS();
	}

	if ((z_rsrc = (zip_rsrc *)zend_fetch_resource(Z_RES_P(zip), le_zip_dir_name, le_zip_dir)) == NULL) {
		RETURN_THROWS();
	}

	if (zr_rsrc->zf != NULL) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Close a zip entry */
PHP_FUNCTION(zip_entry_close)
{
	zval * zip_entry;
	zip_read_rsrc * zr_rsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zip_entry) == FAILURE) {
		RETURN_THROWS();
	}

	if ((zr_rsrc = (zip_read_rsrc *)zend_fetch_resource(Z_RES_P(zip_entry), le_zip_entry_name, le_zip_entry)) == NULL) {
		RETURN_THROWS();
	}

	zend_list_close(Z_RES_P(zip_entry));
	RETURN_TRUE;
}
/* }}} */

/* {{{ Read from an open directory entry */
PHP_FUNCTION(zip_entry_read)
{
	zval * zip_entry;
	zend_long len = 0;
	zip_read_rsrc * zr_rsrc;
	zend_string *buffer;
	int n = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &zip_entry, &len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((zr_rsrc = (zip_read_rsrc *)zend_fetch_resource(Z_RES_P(zip_entry), le_zip_entry_name, le_zip_entry)) == NULL) {
		RETURN_THROWS();
	}

	if (len <= 0) {
		len = 1024;
	}

	if (zr_rsrc->zf) {
		buffer = zend_string_safe_alloc(1, len, 0, 0);
		n = zip_fread(zr_rsrc->zf, ZSTR_VAL(buffer), ZSTR_LEN(buffer));
		if (n > 0) {
			ZSTR_VAL(buffer)[n] = '\0';
			ZSTR_LEN(buffer) = n;
			RETURN_NEW_STR(buffer);
		} else {
			zend_string_efree(buffer);
			RETURN_EMPTY_STRING();
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

static void php_zip_entry_get_info(INTERNAL_FUNCTION_PARAMETERS, int opt) /* {{{ */
{
	zval * zip_entry;
	zip_read_rsrc * zr_rsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zip_entry) == FAILURE) {
		RETURN_THROWS();
	}

	if ((zr_rsrc = (zip_read_rsrc *)zend_fetch_resource(Z_RES_P(zip_entry), le_zip_entry_name, le_zip_entry)) == NULL) {
		RETURN_THROWS();
	}

	if (!zr_rsrc->zf) {
		RETURN_FALSE;
	}

	switch (opt) {
		case 0:
			RETURN_STRING((char *)zr_rsrc->sb.name);
		case 1:
			RETURN_LONG((zend_long) (zr_rsrc->sb.comp_size));
		case 2:
			RETURN_LONG((zend_long) (zr_rsrc->sb.size));
		case 3:
			switch (zr_rsrc->sb.comp_method) {
				case 0:
					RETURN_STRING("stored");
				case 1:
					RETURN_STRING("shrunk");
				case 2:
				case 3:
				case 4:
				case 5:
					RETURN_STRING("reduced");
				case 6:
					RETURN_STRING("imploded");
				case 7:
					RETURN_STRING("tokenized");
					break;
				case 8:
					RETURN_STRING("deflated");
				case 9:
					RETURN_STRING("deflatedX");
					break;
				case 10:
					RETURN_STRING("implodedX");
				default:
					RETURN_FALSE;
			}
	}

}
/* }}} */

/* {{{ Return the name given a ZZip entry */
PHP_FUNCTION(zip_entry_name)
{
	php_zip_entry_get_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Return the compressed size of a ZZip entry */
PHP_FUNCTION(zip_entry_compressedsize)
{
	php_zip_entry_get_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Return the actual filesize of a ZZip entry */
PHP_FUNCTION(zip_entry_filesize)
{
	php_zip_entry_get_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ Return a string containing the compression method used on a particular entry */
PHP_FUNCTION(zip_entry_compressionmethod)
{
	php_zip_entry_get_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ Create new zip using source uri for output, return TRUE on success or the error code */
PHP_METHOD(ZipArchive, open)
{
	struct zip *intern;
	int err = 0;
	zend_long flags = 0;
	char *resolved_path;
	zend_string *filename;
	zval *self = ZEND_THIS;
	ze_zip_object *ze_obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|l", &filename, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	/* We do not use ZIP_FROM_OBJECT, zip init function here */
	ze_obj = Z_ZIP_P(self);

	if (ZSTR_LEN(filename) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (ZIP_OPENBASEDIR_CHECKPATH(ZSTR_VAL(filename))) {
		RETURN_FALSE;
	}

	if (!(resolved_path = expand_filepath(ZSTR_VAL(filename), NULL))) {
		php_error_docref(NULL, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	if (ze_obj->za) {
		/* we already have an opened zip, free it */
		if (zip_close(ze_obj->za) != 0) {
			php_error_docref(NULL, E_WARNING, "Empty string as source");
			efree(resolved_path);
			RETURN_FALSE;
		}
		ze_obj->za = NULL;
	}
	if (ze_obj->filename) {
		efree(ze_obj->filename);
		ze_obj->filename = NULL;
	}

	/* open for write without option to empty the archive */
#ifdef ZIP_RDONLY
	if ((flags & (ZIP_TRUNCATE | ZIP_RDONLY)) == 0) {
#else
	if ((flags & ZIP_TRUNCATE) == 0) {
#endif
		zend_stat_t st = {0};

		/* exists and is empty */
		if (VCWD_STAT(resolved_path, &st) == 0 && st.st_size == 0) {
			php_error_docref(NULL, E_DEPRECATED, "Using empty file as ZipArchive is deprecated");

			/* reduce BC break introduced in libzip 1.6.0
			   "Do not accept empty files as valid zip archives any longer" */
			flags |= ZIP_TRUNCATE;
		}
	}

	intern = zip_open(resolved_path, flags, &err);
	if (!intern || err) {
		efree(resolved_path);
		RETURN_LONG((zend_long)err);
	}
	ze_obj->filename = resolved_path;
	ze_obj->filename_len = strlen(resolved_path);
	ze_obj->za = intern;
	RETURN_TRUE;
}
/* }}} */

/* {{{ Set the password for the active archive */
PHP_METHOD(ZipArchive, setPassword)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	char *password;
	size_t	password_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &password, &password_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (password_len < 1) {
		RETURN_FALSE;
	}

	int res = zip_set_default_password(intern, (const char *)password);
	if (res == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ close the zip archive */
PHP_METHOD(ZipArchive, close)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	ze_zip_object *ze_obj;
	int err;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	ze_obj = Z_ZIP_P(self);

	err = zip_close(intern);
	if (err) {
		php_error_docref(NULL, E_WARNING, "%s", zip_strerror(intern));
		/* Save error for property reader */
		#if LIBZIP_VERSION_MAJOR < 1
			zip_error_get(intern, &ze_obj->err_zip, &ze_obj->err_sys);
		#else
			{
			zip_error_t *ziperr;

			ziperr = zip_get_error(intern);
			ze_obj->err_zip = zip_error_code_zip(ziperr);
			ze_obj->err_sys = zip_error_code_system(ziperr);
			zip_error_fini(ziperr);
			}
		#endif
		zip_discard(intern);
	} else {
		ze_obj->err_zip = 0;
		ze_obj->err_sys = 0;
	}

	/* clear cache as empty zip are not created but deleted */
	php_clear_stat_cache(1, ze_obj->filename, ze_obj->filename_len);

	efree(ze_obj->filename);
	ze_obj->filename = NULL;
	ze_obj->filename_len = 0;
	ze_obj->za = NULL;

	if (!err) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ close the zip archive */
PHP_METHOD(ZipArchive, count)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zip_int64_t num;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	num = zip_get_num_entries(intern, 0);
	RETVAL_LONG(MIN(num, ZEND_LONG_MAX));
}
/* }}} */

/* {{{ clear the internal status */
PHP_METHOD(ZipArchive, clearError)
{
	zval *self = ZEND_THIS;
	ze_zip_object *ze_obj;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ze_obj = Z_ZIP_P(self); /* not ZIP_FROM_OBJECT as we can use saved error after close */
	if (ze_obj->za) {
		zip_error_clear(ze_obj->za);
	} else {
		ze_obj->err_zip = 0;
		ze_obj->err_sys = 0;
	}
}
/* }}} */

/* {{{ Returns the status error message, system and/or zip messages */
PHP_METHOD(ZipArchive, getStatusString)
{
	zval *self = ZEND_THIS;
#if LIBZIP_VERSION_MAJOR < 1
	int zep, syp, len;
	char error_string[128];
#endif
	ze_zip_object *ze_obj;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ze_obj = Z_ZIP_P(self); /* not ZIP_FROM_OBJECT as we can use saved error after close */

#if LIBZIP_VERSION_MAJOR < 1
	if (ze_obj->za) {
		zip_error_get(ze_obj->za, &zep, &syp);
		len = zip_error_to_str(error_string, 128, zep, syp);
	} else {
		len = zip_error_to_str(error_string, 128, ze_obj->err_zip, ze_obj->err_sys);
	}
	RETVAL_STRINGL(error_string, len);
#else
	if (ze_obj->za) {
		zip_error_t *err;

		err = zip_get_error(ze_obj->za);
		RETVAL_STRING(zip_error_strerror(err));
		zip_error_fini(err);
	} else {
		zip_error_t err;

		zip_error_init(&err);
		zip_error_set(&err, ze_obj->err_zip, ze_obj->err_sys);
		RETVAL_STRING(zip_error_strerror(&err));
		zip_error_fini(&err);
	}
#endif
}
/* }}} */

/* {{{ Returns the index of the entry named filename in the archive */
PHP_METHOD(ZipArchive, addEmptyDir)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	char *dirname;
	size_t   dirname_len;
	char *s;
	zend_long flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l",
				&dirname, &dirname_len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (dirname_len<1) {
		RETURN_FALSE;
	}

	if (dirname[dirname_len-1] != '/') {
		s=(char *)safe_emalloc(dirname_len, 1, 2);
		strcpy(s, dirname);
		s[dirname_len] = '/';
		s[dirname_len+1] = '\0';
	} else {
		s = dirname;
	}

	if ((Z_ZIP_P(self)->last_id = zip_dir_add(intern, (const char *)s, flags)) == -1) {
		RETVAL_FALSE;
	} else {
		zip_error_clear(intern);
		RETVAL_TRUE;
	}

	if (s != dirname) {
		efree(s);
	}
}
/* }}} */

static void php_zip_add_from_pattern(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *self = ZEND_THIS;
	char *path = ".";
	size_t  path_len = 1;
	zend_long glob_flags = 0;
	HashTable *options = NULL;
	zip_options opts;
	int found;
	zend_string *pattern;

	/* 1 == glob, 2 == pcre */
	if (type == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|lh",
					&pattern, &glob_flags, &options) == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|sh",
					&pattern, &path, &path_len, &options) == FAILURE) {
			RETURN_THROWS();
		}
	}

	if (ZSTR_LEN(pattern) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}
	if (options && zend_hash_num_elements(options) > 0 && (php_zip_parse_options(options, &opts) < 0)) {
		RETURN_THROWS();
	}

	if (type == 1) {
		found = php_zip_glob(ZSTR_VAL(pattern), ZSTR_LEN(pattern), glob_flags, return_value);
	} else {
		found = php_zip_pcre(pattern, path, path_len, return_value);
	}

	if (found > 0) {
		int i;
		zval *zval_file;
		ze_zip_object *ze_obj;

		ze_obj = Z_ZIP_P(self);

		for (i = 0; i < found; i++) {
			char *file_stripped, *entry_name;
			size_t entry_name_len, file_stripped_len;
			char entry_name_buf[MAXPATHLEN];
			zend_string *basename = NULL;

			if ((zval_file = zend_hash_index_find(Z_ARRVAL_P(return_value), i)) != NULL) {
				if (opts.remove_all_path) {
					basename = php_basename(Z_STRVAL_P(zval_file), Z_STRLEN_P(zval_file), NULL, 0);
					file_stripped = ZSTR_VAL(basename);
					file_stripped_len = ZSTR_LEN(basename);
				} else if (opts.remove_path && !memcmp(Z_STRVAL_P(zval_file), opts.remove_path, opts.remove_path_len)) {
					if (IS_SLASH(Z_STRVAL_P(zval_file)[opts.remove_path_len])) {
						file_stripped = Z_STRVAL_P(zval_file) + opts.remove_path_len + 1;
						file_stripped_len = Z_STRLEN_P(zval_file) - opts.remove_path_len - 1;
					} else {
						file_stripped = Z_STRVAL_P(zval_file) + opts.remove_path_len;
						file_stripped_len = Z_STRLEN_P(zval_file) - opts.remove_path_len;
					}
				} else {
					file_stripped = Z_STRVAL_P(zval_file);
					file_stripped_len = Z_STRLEN_P(zval_file);
				}

				if (opts.add_path) {
					if ((opts.add_path_len + file_stripped_len) > MAXPATHLEN) {
						php_error_docref(NULL, E_WARNING, "Entry name too long (max: %d, %zd given)",
						MAXPATHLEN - 1, (opts.add_path_len + file_stripped_len));
						zend_array_destroy(Z_ARR_P(return_value));
						RETURN_FALSE;
					}
					snprintf(entry_name_buf, MAXPATHLEN, "%s%s", opts.add_path, file_stripped);
				} else {
					snprintf(entry_name_buf, MAXPATHLEN, "%s", file_stripped);
				}

				entry_name = entry_name_buf;
				entry_name_len = strlen(entry_name);
				if (basename) {
					zend_string_release_ex(basename, 0);
					basename = NULL;
				}

				if (php_zip_add_file(ze_obj, Z_STRVAL_P(zval_file), Z_STRLEN_P(zval_file),
					entry_name, entry_name_len, 0, 0, -1, opts.flags) < 0) {
					zend_array_destroy(Z_ARR_P(return_value));
					RETURN_FALSE;
				}
				if (opts.comp_method >= 0) {
					if (zip_set_file_compression(ze_obj->za, ze_obj->last_id, opts.comp_method, opts.comp_flags)) {
						zend_array_destroy(Z_ARR_P(return_value));
						RETURN_FALSE;
					}
				}
#ifdef HAVE_ENCRYPTION
				if (opts.enc_method >= 0) {
					if (zip_file_set_encryption(ze_obj->za, ze_obj->last_id, opts.enc_method, opts.enc_password)) {
						zend_array_destroy(Z_ARR_P(return_value));
						RETURN_FALSE;
					}
				}
#endif
			}
		}
	}
}
/* }}} */

/* {{{ Add files matching the glob pattern. See php's glob for the pattern syntax. */
PHP_METHOD(ZipArchive, addGlob)
{
	php_zip_add_from_pattern(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Add files matching the pcre pattern. See php's pcre for the pattern syntax. */
PHP_METHOD(ZipArchive, addPattern)
{
	php_zip_add_from_pattern(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ Add a file in a Zip archive using its path and the name to use. */
PHP_METHOD(ZipArchive, addFile)
{
	zval *self = ZEND_THIS;
	char *entry_name = NULL;
	size_t entry_name_len = 0;
	zend_long offset_start = 0, offset_len = 0;
	zend_string *filename;
	zend_long flags = ZIP_FL_OVERWRITE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|slll",
			&filename, &entry_name, &entry_name_len, &offset_start, &offset_len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(filename) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (entry_name_len == 0) {
		entry_name = ZSTR_VAL(filename);
		entry_name_len = ZSTR_LEN(filename);
	}

	if (php_zip_add_file(Z_ZIP_P(self), ZSTR_VAL(filename), ZSTR_LEN(filename),
			entry_name, entry_name_len, offset_start, offset_len, -1, flags) < 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Add a file in a Zip archive using its path and the name to use. */
PHP_METHOD(ZipArchive, replaceFile)
{
	zval *self = ZEND_THIS;
	zend_long index;
	zend_long offset_start = 0, offset_len = 0;
	zend_string *filename;
	zend_long flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Pl|lll",
			&filename, &index, &offset_start, &offset_len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(filename) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (index < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (php_zip_add_file(Z_ZIP_P(self), ZSTR_VAL(filename), ZSTR_LEN(filename),
			NULL, 0, offset_start, offset_len, index, flags) < 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Add a file using content and the entry name */
PHP_METHOD(ZipArchive, addFromString)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_string *buffer;
	char *name;
	size_t name_len;
	ze_zip_object *ze_obj;
	struct zip_source *zs;
	int pos = 0;
	zend_long flags = ZIP_FL_OVERWRITE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sS|l",
			&name, &name_len, &buffer, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	ze_obj = Z_ZIP_P(self);
	if (ze_obj->buffers_cnt) {
		ze_obj->buffers = (char **)safe_erealloc(ze_obj->buffers, sizeof(char *), (ze_obj->buffers_cnt+1), 0);
		pos = ze_obj->buffers_cnt++;
	} else {
		ze_obj->buffers = (char **)emalloc(sizeof(char *));
		ze_obj->buffers_cnt++;
		pos = 0;
	}
	ze_obj->buffers[pos] = (char *)safe_emalloc(ZSTR_LEN(buffer), 1, 1);
	memcpy(ze_obj->buffers[pos], ZSTR_VAL(buffer), ZSTR_LEN(buffer) + 1);

	zs = zip_source_buffer(intern, ze_obj->buffers[pos], ZSTR_LEN(buffer), 0);

	if (zs == NULL) {
		RETURN_FALSE;
	}

	ze_obj->last_id = zip_file_add(intern, name, zs, flags);
	if (ze_obj->last_id == -1) {
		zip_source_free(zs);
		RETURN_FALSE;
	} else {
		zip_error_clear(intern);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Returns the information about a the zip entry filename */
PHP_METHOD(ZipArchive, statName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long flags = 0;
	struct zip_stat sb;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|l", &name, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	PHP_ZIP_STAT_PATH(intern, ZSTR_VAL(name), ZSTR_LEN(name), flags, sb);

	RETURN_SB(&sb);
}
/* }}} */

/* {{{ Returns the zip entry information using its index */
PHP_METHOD(ZipArchive, statIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index, flags = 0;

	struct zip_stat sb;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l",
			&index, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (zip_stat_index(intern, index, flags, &sb) != 0) {
		RETURN_FALSE;
	}
	RETURN_SB(&sb);
}
/* }}} */

/* {{{ Returns the index of the entry named filename in the archive */
PHP_METHOD(ZipArchive, locateName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long flags = 0;
	zend_long idx = -1;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|l", &name, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (ZSTR_LEN(name) < 1) {
		RETURN_FALSE;
	}

	idx = (zend_long)zip_name_locate(intern, (const char *)ZSTR_VAL(name), flags);

	if (idx >= 0) {
		RETURN_LONG(idx);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns the name of the file at position index */
PHP_METHOD(ZipArchive, getNameIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	const char *name;
	zend_long flags = 0, index = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l",
			&index, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	name = zip_get_name(intern, (int) index, flags);

	if (name) {
		RETVAL_STRING((char *)name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Set or remove (NULL/'') the comment of the archive */
PHP_METHOD(ZipArchive, setArchiveComment)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	size_t comment_len;
	char * comment;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &comment, &comment_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (comment_len > 0xffff) {
		zend_argument_value_error(1, "must be less than 65535 bytes");
		RETURN_THROWS();
	}

	if (zip_set_archive_comment(intern, (const char *)comment, comment_len)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Returns the comment of an entry using its index */
PHP_METHOD(ZipArchive, getArchiveComment)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long flags = 0;
	const char * comment;
	int comment_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	comment = zip_get_archive_comment(intern, &comment_len, (int)flags);
	if(comment==NULL) {
		RETURN_FALSE;
	}
	RETURN_STRINGL((char *)comment, (zend_long)comment_len);
}
/* }}} */

PHP_METHOD(ZipArchive, setArchiveFlag)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long flag, value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &flag, &value) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (zip_set_archive_flag(intern, flag, (int)value)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

PHP_METHOD(ZipArchive, getArchiveFlag)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long flag, flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &flag, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	RETURN_LONG(zip_get_archive_flag(intern, flag, flags));
}

/* {{{ Set or remove (NULL/'') the comment of an entry using its Name */
PHP_METHOD(ZipArchive, setCommentName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	size_t comment_len, name_len;
	char * comment, *name;
	int idx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
			&name, &name_len, &comment, &comment_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (comment_len > 0xffff) {
		zend_argument_value_error(2, "must be less than 65535 bytes");
		RETURN_THROWS();
	}

	idx = zip_name_locate(intern, name, 0);
	if (idx < 0) {
		RETURN_FALSE;
	}
	PHP_ZIP_SET_FILE_COMMENT(intern, idx, comment, comment_len);
}
/* }}} */

/* {{{ Set or remove (NULL/'') the comment of an entry using its index */
PHP_METHOD(ZipArchive, setCommentIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index;
	size_t comment_len;
	char * comment;
	struct zip_stat sb;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls",
			&index, &comment, &comment_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (comment_len > 0xffff) {
		zend_argument_value_error(2, "must be less than 65535 bytes");
		RETURN_THROWS();
	}

	PHP_ZIP_STAT_INDEX(intern, index, 0, sb);
	PHP_ZIP_SET_FILE_COMMENT(intern, index, comment, comment_len);
}
/* }}} */

/* those constants/functions are only available in libzip since 0.11.2 */
#ifdef ZIP_OPSYS_DEFAULT

/* {{{ Set external attributes for file in zip, using its name */
PHP_METHOD(ZipArchive, setExternalAttributesName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	size_t name_len;
	char *name;
	zend_long flags=0, opsys, attr;
	zip_int64_t idx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sll|l",
			&name, &name_len, &opsys, &attr, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	idx = zip_name_locate(intern, name, 0);

	if (idx < 0) {
		RETURN_FALSE;
	}
	if (zip_file_set_external_attributes(intern, idx, (zip_flags_t)flags,
			(zip_uint8_t)(opsys&0xff), (zip_uint32_t)attr) < 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Set external attributes for file in zip, using its index */
PHP_METHOD(ZipArchive, setExternalAttributesIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index, flags=0, opsys, attr;
	struct zip_stat sb;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lll|l",
			&index, &opsys, &attr, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	PHP_ZIP_STAT_INDEX(intern, index, 0, sb);
	if (zip_file_set_external_attributes(intern, (zip_uint64_t)index,
			(zip_flags_t)flags, (zip_uint8_t)(opsys&0xff), (zip_uint32_t)attr) < 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get external attributes for file in zip, using its name */
PHP_METHOD(ZipArchive, getExternalAttributesName)
{
	struct zip *intern;
	zval *self = ZEND_THIS, *z_opsys, *z_attr;
	size_t name_len;
	char *name;
	zend_long flags=0;
	zip_uint8_t opsys;
	zip_uint32_t attr;
	zip_int64_t idx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l",
			&name, &name_len, &z_opsys, &z_attr, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	idx = zip_name_locate(intern, name, 0);

	if (idx < 0) {
		RETURN_FALSE;
	}
	if (zip_file_get_external_attributes(intern, idx,
			(zip_flags_t)flags, &opsys, &attr) < 0) {
		RETURN_FALSE;
	}
	ZEND_TRY_ASSIGN_REF_LONG(z_opsys, opsys);
	ZEND_TRY_ASSIGN_REF_LONG(z_attr, attr);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get external attributes for file in zip, using its index */
PHP_METHOD(ZipArchive, getExternalAttributesIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS, *z_opsys, *z_attr;
	zend_long index, flags=0;
	zip_uint8_t opsys;
	zip_uint32_t attr;
	struct zip_stat sb;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lzz|l",
			&index, &z_opsys, &z_attr, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	PHP_ZIP_STAT_INDEX(intern, index, 0, sb);
	if (zip_file_get_external_attributes(intern, (zip_uint64_t)index,
			(zip_flags_t)flags, &opsys, &attr) < 0) {
		RETURN_FALSE;
	}
	ZEND_TRY_ASSIGN_REF_LONG(z_opsys, opsys);
	ZEND_TRY_ASSIGN_REF_LONG(z_attr, attr);
	RETURN_TRUE;
}
/* }}} */
#endif /* ifdef ZIP_OPSYS_DEFAULT */

#ifdef HAVE_ENCRYPTION
/* {{{ Set encryption method for file in zip, using its name */
PHP_METHOD(ZipArchive, setEncryptionName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long method;
	zip_int64_t idx;
	char *name, *password = NULL;
	size_t name_len, password_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|s!",
			&name, &name_len, &method, &password, &password_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	idx = zip_name_locate(intern, name, 0);

	if (idx < 0) {
		RETURN_FALSE;
	}

	if (zip_file_set_encryption(intern, idx, (zip_uint16_t)method, password)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Set encryption method for file in zip, using its index */
PHP_METHOD(ZipArchive, setEncryptionIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index, method;
	char *password = NULL;
	size_t password_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|s!",
			&index, &method, &password, &password_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (zip_file_set_encryption(intern, index, (zip_uint16_t)method, password)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Returns the comment of an entry using its name */
PHP_METHOD(ZipArchive, getCommentName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	size_t name_len;
	int idx;
	zend_long flags = 0;
	zip_uint32_t comment_len = 0;
	const char * comment;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l",
			&name, &name_len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	idx = zip_name_locate(intern, name, 0);

	if (idx < 0) {
		RETURN_FALSE;
	}

	comment = zip_file_get_comment(intern, idx, &comment_len, (zip_flags_t)flags);
	RETURN_STRINGL((char *)comment, comment_len);
}
/* }}} */

/* {{{ Returns the comment of an entry using its index */
PHP_METHOD(ZipArchive, getCommentIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index, flags = 0;
	const char * comment;
	zip_uint32_t comment_len = 0;
	struct zip_stat sb;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l",
				&index, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	PHP_ZIP_STAT_INDEX(intern, index, 0, sb);
	comment = zip_file_get_comment(intern, index, &comment_len, (zip_flags_t)flags);
	RETURN_STRINGL((char *)comment, comment_len);
}
/* }}} */

/* {{{ Set the compression of a file in zip, using its name */
PHP_METHOD(ZipArchive, setCompressionName)
{
	struct zip *intern;
	zval *this = ZEND_THIS;
	size_t name_len;
	char *name;
	zip_int64_t idx;
	zend_long comp_method, comp_flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l",
			&name, &name_len, &comp_method, &comp_flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, this);

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	idx = zip_name_locate(intern, name, 0);

	if (idx < 0) {
		RETURN_FALSE;
	}

	if (zip_set_file_compression(intern, (zip_uint64_t)idx,
			(zip_int32_t)comp_method, (zip_uint32_t)comp_flags) != 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Set the compression of a file in zip, using its index */
PHP_METHOD(ZipArchive, setCompressionIndex)
{
	struct zip *intern;
	zval *this = ZEND_THIS;
	zend_long index;
	zend_long comp_method, comp_flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l",
			&index, &comp_method, &comp_flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zip_set_file_compression(intern, (zip_uint64_t)index,
			(zip_int32_t)comp_method, (zip_uint32_t)comp_flags) != 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_SET_MTIME
/* {{{ Set the modification time of a file in zip, using its name */
PHP_METHOD(ZipArchive, setMtimeName)
{
	struct zip *intern;
	zval *this = ZEND_THIS;
	size_t name_len;
	char *name;
	zip_int64_t idx;
	zend_long mtime, flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l",
			&name, &name_len, &mtime,  &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, this);

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	idx = zip_name_locate(intern, name, 0);

	if (idx < 0) {
		RETURN_FALSE;
	}

	if (zip_file_set_mtime(intern, (zip_uint64_t)idx,
			(time_t)mtime, (zip_uint32_t)flags) != 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Set the modification time of a file in zip, using its index */
PHP_METHOD(ZipArchive, setMtimeIndex)
{
	struct zip *intern;
	zval *this = ZEND_THIS;
	zend_long index;
	zend_long mtime, flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l",
			&index, &mtime, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zip_file_set_mtime(intern, (zip_uint64_t)index,
			(time_t)mtime, (zip_uint32_t)flags) != 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Delete a file using its index */
PHP_METHOD(ZipArchive, deleteIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (index < 0) {
		RETURN_FALSE;
	}

	if (zip_delete(intern, index) < 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Delete a file using its index */
PHP_METHOD(ZipArchive, deleteName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	size_t name_len;
	char *name;
	struct zip_stat sb;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (name_len < 1) {
		RETURN_FALSE;
	}

	PHP_ZIP_STAT_PATH(intern, name, name_len, 0, sb);
	if (zip_delete(intern, sb.index)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Rename an entry selected by its index to new_name */
PHP_METHOD(ZipArchive, renameIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	char *new_name;
	size_t new_name_len;
	zend_long index;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &index, &new_name, &new_name_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (index < 0) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, self);

	if (new_name_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if (zip_file_rename(intern, index, (const char *)new_name, 0) != 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Rename an entry selected by its name to new_name */
PHP_METHOD(ZipArchive, renameName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	struct zip_stat sb;
	char *name, *new_name;
	size_t name_len, new_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &name_len, &new_name, &new_name_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (new_name_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	PHP_ZIP_STAT_PATH(intern, name, name_len, 0, sb);

	if (zip_file_rename(intern, sb.index, (const char *)new_name, 0)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Changes to the file at position index are reverted */
PHP_METHOD(ZipArchive, unchangeIndex)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (index < 0) {
		RETURN_FALSE;
	}

	if (zip_unchange(intern, index) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Changes to the file named 'name' are reverted */
PHP_METHOD(ZipArchive, unchangeName)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	struct zip_stat sb;
	char *name;
	size_t name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (name_len < 1) {
		RETURN_FALSE;
	}

	PHP_ZIP_STAT_PATH(intern, name, name_len, 0, sb);

	if (zip_unchange(intern, sb.index) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ All changes to files and global information in archive are reverted */
PHP_METHOD(ZipArchive, unchangeAll)
{
	struct zip *intern;
	zval *self = ZEND_THIS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (zip_unchange_all(intern) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Revert all global changes to the archive archive.  For now, this only reverts archive comment changes. */
PHP_METHOD(ZipArchive, unchangeArchive)
{
	struct zip *intern;
	zval *self = ZEND_THIS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	if (zip_unchange_archive(intern) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Extract one or more file from a zip archive */
/* TODO:
 * - allow index or array of indices
 * - replace path
 * - patterns
 */
PHP_METHOD(ZipArchive, extractTo)
{
	struct zip *intern;

	zval *self = ZEND_THIS;
	zend_string *files_str = NULL;
	HashTable *files_ht = NULL;

	php_stream_statbuf ssb;
	char *pathto;
	size_t pathto_len;
	int ret;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(pathto, pathto_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(files_ht, files_str)
	ZEND_PARSE_PARAMETERS_END();

	ZIP_FROM_OBJECT(intern, self);

	if (pathto_len < 1) {
		RETURN_FALSE;
	}

	if (php_stream_stat_path_ex(pathto, PHP_STREAM_URL_STAT_QUIET, &ssb, NULL) < 0) {
		ret = php_stream_mkdir(pathto, 0777, PHP_STREAM_MKDIR_RECURSIVE, NULL);
		if (!ret) {
			RETURN_FALSE;
		}
	}

	uint32_t nelems, i;

	if (files_str) {
		if (!php_zip_extract_file(intern, pathto, ZSTR_VAL(files_str), ZSTR_LEN(files_str), -1)) {
			RETURN_FALSE;
		}
	} else if (files_ht) {
		nelems = zend_hash_num_elements(files_ht);
		if (nelems == 0 ) {
			RETURN_FALSE;
		}
		for (i = 0; i < nelems; i++) {
			zval *zval_file;
			if ((zval_file = zend_hash_index_find_deref(files_ht, i)) != NULL) {
				switch (Z_TYPE_P(zval_file)) {
					case IS_LONG:
						break;
					case IS_STRING:
						if (!php_zip_extract_file(intern, pathto, Z_STRVAL_P(zval_file), Z_STRLEN_P(zval_file), -1)) {
							RETURN_FALSE;
						}
						break;
				}
			}
		}
	} else {
		/* Extract all files */
		zip_int64_t i, filecount = zip_get_num_entries(intern, 0);

		if (filecount == -1) {
				php_error_docref(NULL, E_WARNING, "Illegal archive");
				RETURN_FALSE;
		}

		for (i = 0; i < filecount; i++) {
			const char *file = zip_get_name(intern, i, ZIP_FL_UNCHANGED);
			if (!file || !php_zip_extract_file(intern, pathto, file, strlen(file), i)) {
					RETURN_FALSE;
			}
		}
	}

	RETURN_TRUE;
}
/* }}} */

static void php_zip_get_from(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	struct zip *intern;
	zval *self = ZEND_THIS;

	struct zip_stat sb;
	struct zip_file *zf;

	zend_long index = -1;
	zend_long flags = 0;
	zend_long len = 0;

	zend_string *filename;
	zend_string *buffer;

	int n = 0;

	if (type == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|ll", &filename, &len, &flags) == FAILURE) {
			RETURN_THROWS();
		}

		ZIP_FROM_OBJECT(intern, self);

		PHP_ZIP_STAT_PATH(intern, ZSTR_VAL(filename), ZSTR_LEN(filename), flags, sb);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|ll", &index, &len, &flags) == FAILURE) {
			RETURN_THROWS();
		}

		ZIP_FROM_OBJECT(intern, self);

		PHP_ZIP_STAT_INDEX(intern, index, 0, sb);
	}

	if (sb.size < 1) {
		RETURN_EMPTY_STRING();
	}

	if (len < 1) {
		len = sb.size;
	}
	if (index >= 0) {
		zf = zip_fopen_index(intern, index, flags);
	} else {
		zf = zip_fopen(intern, ZSTR_VAL(filename), flags);
	}

	if (zf == NULL) {
		RETURN_FALSE;
	}

	buffer = zend_string_safe_alloc(1, len, 0, 0);
	n = zip_fread(zf, ZSTR_VAL(buffer), ZSTR_LEN(buffer));
	if (n < 1) {
		zend_string_efree(buffer);
		RETURN_EMPTY_STRING();
	}

	zip_fclose(zf);
	ZSTR_VAL(buffer)[n] = '\0';
	ZSTR_LEN(buffer) = n;
	RETURN_NEW_STR(buffer);
}
/* }}} */

/* {{{ get the contents of an entry using its name */
PHP_METHOD(ZipArchive, getFromName)
{
	php_zip_get_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ get the contents of an entry using its index */
PHP_METHOD(ZipArchive, getFromIndex)
{
	php_zip_get_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

static void php_zip_get_stream(INTERNAL_FUNCTION_PARAMETERS, int type, bool accept_flags) /* {{{ */
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_long index;
	zend_long flags = 0;
	struct zip_stat sb;
	char *mode = "rb";
	zend_string *filename;
	php_stream *stream;

	if (type) {
		if (accept_flags) {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|l", &filename, &flags) == FAILURE) {
				RETURN_THROWS();
			}
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "P", &filename) == FAILURE) {
				RETURN_THROWS();
			}
		}
	} else {
		ZEND_ASSERT(accept_flags);
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &index, &flags) == FAILURE) {
			RETURN_THROWS();
		}
	}

	ZIP_FROM_OBJECT(intern, self);

	if (type) {
		PHP_ZIP_STAT_PATH(intern, ZSTR_VAL(filename), ZSTR_LEN(filename), flags, sb);
	} else {
		PHP_ZIP_STAT_INDEX(intern, index, flags, sb);
	}

	stream = php_stream_zip_open(intern, &sb, mode, flags STREAMS_CC);
	if (stream) {
		php_stream_to_zval(stream, return_value);
	} else {
		RETURN_FALSE;
	}
}

/* {{{ get a stream for an entry using its name */
PHP_METHOD(ZipArchive, getStreamName)
{
	php_zip_get_stream(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, /* accept_flags */ true);
}
/* }}} */

/* {{{ get a stream for an entry using its index */
PHP_METHOD(ZipArchive, getStreamIndex)
{
	php_zip_get_stream(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, /* accept_flags */ true);
}
/* }}} */

PHP_METHOD(ZipArchive, getStream)
{
	php_zip_get_stream(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, /* accept_flags */ false);
}

#ifdef HAVE_PROGRESS_CALLBACK
static void _php_zip_progress_callback(zip_t *arch, double state, void *ptr)
{
	zval cb_args[1];
	zval cb_retval;
	ze_zip_object *obj = ptr;

	ZVAL_DOUBLE(&cb_args[0], state);
	if (call_user_function(EG(function_table), NULL, &obj->progress_callback, &cb_retval, 1, cb_args) == SUCCESS && !Z_ISUNDEF(cb_retval)) {
		zval_ptr_dtor(&cb_retval);
	}
}

/* {{{ register a progression callback: void callback(double state); */
PHP_METHOD(ZipArchive, registerProgressCallback)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	double rate;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	ze_zip_object *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "df", &rate, &fci, &fcc) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	obj = Z_ZIP_P(self);

	/* free if called twice */
	_php_zip_progress_callback_free(obj);

	/* register */
	ZVAL_COPY(&obj->progress_callback, &fci.function_name);
	if (zip_register_progress_callback_with_state(intern, rate, _php_zip_progress_callback, _php_zip_progress_callback_free, obj)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_CANCEL_CALLBACK
static int _php_zip_cancel_callback(zip_t *arch, void *ptr)
{
	zval cb_retval;
	int retval = 0;
	ze_zip_object *obj = ptr;

	if (call_user_function(EG(function_table), NULL, &obj->cancel_callback, &cb_retval, 0, NULL) == SUCCESS && !Z_ISUNDEF(cb_retval)) {
		retval = zval_get_long(&cb_retval);
		zval_ptr_dtor(&cb_retval);
	}

	return retval;
}

/* {{{ register a progression callback: int callback(double state); */
PHP_METHOD(ZipArchive, registerCancelCallback)
{
	struct zip *intern;
	zval *self = ZEND_THIS;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	ze_zip_object *obj;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f", &fci, &fcc) == FAILURE) {
		RETURN_THROWS();
	}

	ZIP_FROM_OBJECT(intern, self);

	obj = Z_ZIP_P(self);

	/* free if called twice */
	_php_zip_cancel_callback_free(obj);

	/* register */
	ZVAL_COPY(&obj->cancel_callback, &fci.function_name);
	if (zip_register_cancel_callback_with_state(intern, _php_zip_cancel_callback, _php_zip_cancel_callback_free, obj)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_METHOD_SUPPORTED
/* {{{ check if a compression method is available in used libzip */
PHP_METHOD(ZipArchive, isCompressionMethodSupported)
{
	zend_long method;
	bool enc = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|b", &method, &enc) == FAILURE) {
		return;
	}
	RETVAL_BOOL(zip_compression_method_supported((zip_int32_t)method, enc));
}
/* }}} */

/* {{{ check if a encryption method is available in used libzip */
PHP_METHOD(ZipArchive, isEncryptionMethodSupported)
{
	zend_long method;
	bool enc = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|b", &method, &enc) == FAILURE) {
		return;
	}
	RETVAL_BOOL(zip_encryption_method_supported((zip_uint16_t)method, enc));
}
/* }}} */
#endif

static void php_zip_free_prop_handler(zval *el) /* {{{ */ {
	pefree(Z_PTR_P(el), 1);
} /* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(zip)
{
	memcpy(&zip_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zip_object_handlers.offset = XtOffsetOf(ze_zip_object, zo);
	zip_object_handlers.free_obj = php_zip_object_free_storage;
	zip_object_handlers.clone_obj = NULL;
	zip_object_handlers.get_property_ptr_ptr = php_zip_get_property_ptr_ptr;

	zip_object_handlers.get_gc          = php_zip_get_gc;
	zip_object_handlers.get_properties = php_zip_get_properties;
	zip_object_handlers.read_property	= php_zip_read_property;
	zip_object_handlers.has_property	= php_zip_has_property;
	zip_object_handlers.write_property = php_zip_write_property;

	zip_class_entry = register_class_ZipArchive(zend_ce_countable);
	zip_class_entry->create_object = php_zip_object_new;
	zip_class_entry->default_object_handlers = &zip_object_handlers;

	zend_hash_init(&zip_prop_handlers, 0, NULL, php_zip_free_prop_handler, 1);
	php_zip_register_prop_handler(&zip_prop_handlers, "lastId",    php_zip_last_id, NULL, IS_LONG);
	php_zip_register_prop_handler(&zip_prop_handlers, "status",    php_zip_status, NULL, IS_LONG);
	php_zip_register_prop_handler(&zip_prop_handlers, "statusSys", php_zip_status_sys, NULL, IS_LONG);
	php_zip_register_prop_handler(&zip_prop_handlers, "numFiles",  php_zip_get_num_files, NULL, IS_LONG);
	php_zip_register_prop_handler(&zip_prop_handlers, "filename",  NULL, php_zipobj_get_filename, IS_STRING);
	php_zip_register_prop_handler(&zip_prop_handlers, "comment",   NULL, php_zipobj_get_zip_comment, IS_STRING);

	php_register_url_stream_wrapper("zip", &php_stream_zip_wrapper);

	le_zip_dir   = zend_register_list_destructors_ex(php_zip_free_dir,   NULL, le_zip_dir_name,   module_number);
	le_zip_entry = zend_register_list_destructors_ex(php_zip_free_entry, NULL, le_zip_entry_name, module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(zip)
{
	zend_hash_destroy(&zip_prop_handlers);
	php_unregister_url_stream_wrapper("zip");
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(zip)
{
	php_info_print_table_start();

	php_info_print_table_row(2, "Zip", "enabled");
	php_info_print_table_row(2, "Zip version", PHP_ZIP_VERSION);
#ifdef HAVE_LIBZIP_VERSION
	if (strcmp(LIBZIP_VERSION, zip_libzip_version())) {
		php_info_print_table_row(2, "Libzip headers version", LIBZIP_VERSION);
		php_info_print_table_row(2, "Libzip library version", zip_libzip_version());
	} else
#endif
	{
		php_info_print_table_row(2, "Libzip version", LIBZIP_VERSION);
	}
#ifdef HAVE_METHOD_SUPPORTED
	php_info_print_table_row(2, "BZIP2 compression",
		zip_compression_method_supported(ZIP_CM_BZIP2, 1) ? "Yes" : "No");
	php_info_print_table_row(2, "XZ compression",
		zip_compression_method_supported(ZIP_CM_XZ, 1) ? "Yes" : "No");
#ifdef ZIP_CM_ZSTD
	php_info_print_table_row(2, "ZSTD compression",
		zip_compression_method_supported(ZIP_CM_ZSTD, 1) ? "Yes" : "No");
#else
	php_info_print_table_row(2, "ZSTD compression", "No");
#endif
	php_info_print_table_row(2, "AES-128 encryption",
		zip_encryption_method_supported(ZIP_EM_AES_128, 1) ? "Yes" : "No");
	php_info_print_table_row(2, "AES-192 encryption",
		zip_encryption_method_supported(ZIP_EM_AES_128, 1) ? "Yes" : "No");
	php_info_print_table_row(2, "AES-256 encryption",
		zip_encryption_method_supported(ZIP_EM_AES_128, 1) ? "Yes" : "No");
#endif

	php_info_print_table_end();
}
/* }}} */
