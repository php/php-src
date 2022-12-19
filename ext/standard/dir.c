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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
 */

/* {{{ includes/startup/misc */

#include "php.h"
#include "fopen_wrappers.h"
#include "file.h"
#include "php_dir.h"
#include "php_string.h"
#include "php_scandir.h"
#include "basic_functions.h"
#include "dir_arginfo.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <errno.h>

#ifdef PHP_WIN32
#include "win32/readdir.h"
#endif


#ifdef HAVE_GLOB
#ifndef PHP_WIN32
#include <glob.h>
#else
#include "win32/glob.h"
#endif
#endif

typedef struct {
	zend_resource *default_dir;
} php_dir_globals;

#ifdef ZTS
#define DIRG(v) ZEND_TSRMG(dir_globals_id, php_dir_globals *, v)
int dir_globals_id;
#else
#define DIRG(v) (dir_globals.v)
php_dir_globals dir_globals;
#endif

static zend_class_entry *dir_class_entry_ptr;

#define Z_DIRECTORY_PATH_P(zv) OBJ_PROP_NUM(Z_OBJ_P(zv), 0)
#define Z_DIRECTORY_HANDLE_P(zv) OBJ_PROP_NUM(Z_OBJ_P(zv), 1)

#define FETCH_DIRP() \
	myself = getThis(); \
	if (!myself) { \
		ZEND_PARSE_PARAMETERS_START(0, 1) \
			Z_PARAM_OPTIONAL \
			Z_PARAM_RESOURCE_OR_NULL(id) \
		ZEND_PARSE_PARAMETERS_END(); \
		if (id) { \
			if ((dirp = (php_stream *)zend_fetch_resource(Z_RES_P(id), "Directory", php_file_le_stream())) == NULL) { \
				RETURN_THROWS(); \
			} \
		} else { \
			if (!DIRG(default_dir)) { \
				zend_type_error("No resource supplied"); \
				RETURN_THROWS(); \
			} \
			if ((dirp = (php_stream *)zend_fetch_resource(DIRG(default_dir), "Directory", php_file_le_stream())) == NULL) { \
				RETURN_THROWS(); \
			} \
		} \
	} else { \
		ZEND_PARSE_PARAMETERS_NONE(); \
		zval *handle_zv = Z_DIRECTORY_HANDLE_P(myself); \
		if (Z_TYPE_P(handle_zv) != IS_RESOURCE) { \
			zend_throw_error(NULL, "Unable to find my handle property"); \
			RETURN_THROWS(); \
		} \
		if ((dirp = (php_stream *)zend_fetch_resource_ex(handle_zv, "Directory", php_file_le_stream())) == NULL) { \
			RETURN_THROWS(); \
		} \
	}


static void php_set_default_dir(zend_resource *res)
{
	if (DIRG(default_dir)) {
		zend_list_delete(DIRG(default_dir));
	}

	if (res) {
		GC_ADDREF(res);
	}

	DIRG(default_dir) = res;
}

PHP_RINIT_FUNCTION(dir)
{
	DIRG(default_dir) = NULL;
	return SUCCESS;
}

PHP_MINIT_FUNCTION(dir)
{
	static char dirsep_str[2], pathsep_str[2];

	dir_class_entry_ptr = register_class_Directory();

#ifdef ZTS
	ts_allocate_id(&dir_globals_id, sizeof(php_dir_globals), NULL, NULL);
#endif

	dirsep_str[0] = DEFAULT_SLASH;
	dirsep_str[1] = '\0';
	REGISTER_STRING_CONSTANT("DIRECTORY_SEPARATOR", dirsep_str, CONST_PERSISTENT);

	pathsep_str[0] = ZEND_PATHS_SEPARATOR;
	pathsep_str[1] = '\0';
	REGISTER_STRING_CONSTANT("PATH_SEPARATOR", pathsep_str, CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SCANDIR_SORT_ASCENDING",  PHP_SCANDIR_SORT_ASCENDING,  CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SCANDIR_SORT_DESCENDING", PHP_SCANDIR_SORT_DESCENDING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SCANDIR_SORT_NONE",       PHP_SCANDIR_SORT_NONE,       CONST_PERSISTENT);

#ifdef HAVE_GLOB

#ifdef GLOB_BRACE
	REGISTER_LONG_CONSTANT("GLOB_BRACE", GLOB_BRACE, CONST_PERSISTENT);
#else
# define GLOB_BRACE 0
#endif

#ifdef GLOB_MARK
	REGISTER_LONG_CONSTANT("GLOB_MARK", GLOB_MARK, CONST_PERSISTENT);
#else
# define GLOB_MARK 0
#endif

#ifdef GLOB_NOSORT
	REGISTER_LONG_CONSTANT("GLOB_NOSORT", GLOB_NOSORT, CONST_PERSISTENT);
#else
# define GLOB_NOSORT 0
#endif

#ifdef GLOB_NOCHECK
	REGISTER_LONG_CONSTANT("GLOB_NOCHECK", GLOB_NOCHECK, CONST_PERSISTENT);
#else
# define GLOB_NOCHECK 0
#endif

#ifdef GLOB_NOESCAPE
	REGISTER_LONG_CONSTANT("GLOB_NOESCAPE", GLOB_NOESCAPE, CONST_PERSISTENT);
#else
# define GLOB_NOESCAPE 0
#endif

#ifdef GLOB_ERR
	REGISTER_LONG_CONSTANT("GLOB_ERR", GLOB_ERR, CONST_PERSISTENT);
#else
# define GLOB_ERR 0
#endif

#ifndef GLOB_ONLYDIR
# define GLOB_ONLYDIR (1<<30)
# define GLOB_EMULATE_ONLYDIR
# define GLOB_FLAGMASK (~GLOB_ONLYDIR)
#else
# define GLOB_FLAGMASK (~0)
#endif

/* This is used for checking validity of passed flags (passing invalid flags causes segfault in glob()!! */
#define GLOB_AVAILABLE_FLAGS (0 | GLOB_BRACE | GLOB_MARK | GLOB_NOSORT | GLOB_NOCHECK | GLOB_NOESCAPE | GLOB_ERR | GLOB_ONLYDIR)

	REGISTER_LONG_CONSTANT("GLOB_ONLYDIR", GLOB_ONLYDIR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GLOB_AVAILABLE_FLAGS", GLOB_AVAILABLE_FLAGS, CONST_PERSISTENT);

#endif /* HAVE_GLOB */

	return SUCCESS;
}
/* }}} */

/* {{{ internal functions */
static void _php_do_opendir(INTERNAL_FUNCTION_PARAMETERS, int createobject)
{
	char *dirname;
	size_t dir_len;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;
	php_stream *dirp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(dirname, dir_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	context = php_stream_context_from_zval(zcontext, 0);

	dirp = php_stream_opendir(dirname, REPORT_ERRORS, context);

	if (dirp == NULL) {
		RETURN_FALSE;
	}

	dirp->flags |= PHP_STREAM_FLAG_NO_FCLOSE;

	php_set_default_dir(dirp->res);

	if (createobject) {
		object_init_ex(return_value, dir_class_entry_ptr);
		ZVAL_STRINGL(Z_DIRECTORY_PATH_P(return_value), dirname, dir_len);
		ZVAL_RES(Z_DIRECTORY_HANDLE_P(return_value), dirp->res);
		php_stream_auto_cleanup(dirp); /* so we don't get warnings under debug */
	} else {
		php_stream_to_zval(dirp, return_value);
	}
}
/* }}} */

/* {{{ Open a directory and return a dir_handle */
PHP_FUNCTION(opendir)
{
	_php_do_opendir(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Directory class with properties, handle and class and methods read, rewind and close */
PHP_FUNCTION(dir)
{
	_php_do_opendir(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Close directory connection identified by the dir_handle */
PHP_FUNCTION(closedir)
{
	zval *id = NULL, *myself;
	php_stream *dirp;
	zend_resource *res;

	FETCH_DIRP();

	if (!(dirp->flags & PHP_STREAM_FLAG_IS_DIR)) {
		zend_argument_type_error(1, "must be a valid Directory resource");
		RETURN_THROWS();
	}

	res = dirp->res;
	zend_list_close(dirp->res);

	if (res == DIRG(default_dir)) {
		php_set_default_dir(NULL);
	}
}
/* }}} */

#if defined(HAVE_CHROOT) && !defined(ZTS) && defined(ENABLE_CHROOT_FUNC)
/* {{{ Change root directory */
PHP_FUNCTION(chroot)
{
	char *str;
	int ret;
	size_t str_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(str, str_len)
	ZEND_PARSE_PARAMETERS_END();

	ret = chroot(str);
	if (ret != 0) {
		php_error_docref(NULL, E_WARNING, "%s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	php_clear_stat_cache(1, NULL, 0);

	ret = chdir("/");

	if (ret != 0) {
		php_error_docref(NULL, E_WARNING, "%s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Change the current directory */
PHP_FUNCTION(chdir)
{
	char *str;
	int ret;
	size_t str_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(str, str_len)
	ZEND_PARSE_PARAMETERS_END();

	if (php_check_open_basedir(str)) {
		RETURN_FALSE;
	}
	ret = VCWD_CHDIR(str);

	if (ret != 0) {
		php_error_docref(NULL, E_WARNING, "%s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	if (BG(CurrentStatFile) && !IS_ABSOLUTE_PATH(ZSTR_VAL(BG(CurrentStatFile)), ZSTR_LEN(BG(CurrentStatFile)))) {
		zend_string_release(BG(CurrentStatFile));
		BG(CurrentStatFile) = NULL;
	}
	if (BG(CurrentLStatFile) && !IS_ABSOLUTE_PATH(ZSTR_VAL(BG(CurrentLStatFile)), ZSTR_LEN(BG(CurrentLStatFile)))) {
		zend_string_release(BG(CurrentLStatFile));
		BG(CurrentLStatFile) = NULL;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Gets the current directory */
PHP_FUNCTION(getcwd)
{
	char path[MAXPATHLEN];
	char *ret=NULL;

	ZEND_PARSE_PARAMETERS_NONE();

#ifdef HAVE_GETCWD
	ret = VCWD_GETCWD(path, MAXPATHLEN);
#elif defined(HAVE_GETWD)
	ret = VCWD_GETWD(path);
#endif

	if (ret) {
		RETURN_STRING(path);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Rewind dir_handle back to the start */
PHP_FUNCTION(rewinddir)
{
	zval *id = NULL, *myself;
	php_stream *dirp;

	FETCH_DIRP();

	if (!(dirp->flags & PHP_STREAM_FLAG_IS_DIR)) {
		zend_argument_type_error(1, "must be a valid Directory resource");
		RETURN_THROWS();
	}

	php_stream_rewinddir(dirp);
}
/* }}} */

/* {{{ Read directory entry from dir_handle */
PHP_FUNCTION(readdir)
{
	zval *id = NULL, *myself;
	php_stream *dirp;
	php_stream_dirent entry;

	FETCH_DIRP();

	if (!(dirp->flags & PHP_STREAM_FLAG_IS_DIR)) {
		zend_argument_type_error(1, "must be a valid Directory resource");
		RETURN_THROWS();
	}

	if (php_stream_readdir(dirp, &entry)) {
		RETURN_STRINGL(entry.d_name, strlen(entry.d_name));
	}
	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_GLOB
/* {{{ Find pathnames matching a pattern */
PHP_FUNCTION(glob)
{
	size_t cwd_skip = 0;
#ifdef ZTS
	char cwd[MAXPATHLEN];
	char work_pattern[MAXPATHLEN];
	char *result;
#endif
	char *pattern = NULL;
	size_t pattern_len;
	zend_long flags = 0;
	glob_t globbuf;
	size_t n;
	int ret;
	bool basedir_limit = 0;
	zval tmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(pattern, pattern_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	if (pattern_len >= MAXPATHLEN) {
		php_error_docref(NULL, E_WARNING, "Pattern exceeds the maximum allowed length of %d characters", MAXPATHLEN);
		RETURN_FALSE;
	}

	if ((GLOB_AVAILABLE_FLAGS & flags) != flags) {
		php_error_docref(NULL, E_WARNING, "At least one of the passed flags is invalid or not supported on this platform");
		RETURN_FALSE;
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


	memset(&globbuf, 0, sizeof(glob_t));
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
			goto no_results;
		}
#endif
		RETURN_FALSE;
	}

	/* now catch the FreeBSD style of "no matches" */
	if (!globbuf.gl_pathc || !globbuf.gl_pathv) {
#ifdef GLOB_NOMATCH
no_results:
#endif
		array_init(return_value);
		return;
	}

	array_init(return_value);
	for (n = 0; n < (size_t)globbuf.gl_pathc; n++) {
		if (PG(open_basedir) && *PG(open_basedir)) {
			if (php_check_open_basedir_ex(globbuf.gl_pathv[n], 0)) {
				basedir_limit = 1;
				continue;
			}
		}
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
		ZVAL_STRING(&tmp, globbuf.gl_pathv[n]+cwd_skip);
		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
	}

	globfree(&globbuf);

	if (basedir_limit && !zend_hash_num_elements(Z_ARRVAL_P(return_value))) {
		zend_array_destroy(Z_ARR_P(return_value));
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ List files & directories inside the specified path */
PHP_FUNCTION(scandir)
{
	char *dirn;
	size_t dirn_len;
	zend_long flags = PHP_SCANDIR_SORT_ASCENDING;
	zend_string **namelist;
	int n, i;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH(dirn, dirn_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	if (dirn_len < 1) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (zcontext) {
		context = php_stream_context_from_zval(zcontext, 0);
	}

	if (flags == PHP_SCANDIR_SORT_ASCENDING) {
		n = php_stream_scandir(dirn, &namelist, context, (void *) php_stream_dirent_alphasort);
	} else if (flags == PHP_SCANDIR_SORT_NONE) {
		n = php_stream_scandir(dirn, &namelist, context, NULL);
	} else {
		n = php_stream_scandir(dirn, &namelist, context, (void *) php_stream_dirent_alphasortr);
	}
	if (n < 0) {
		php_error_docref(NULL, E_WARNING, "(errno %d): %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0; i < n; i++) {
		add_next_index_str(return_value, namelist[i]);
	}

	if (n) {
		efree(namelist);
	}
}
/* }}} */
