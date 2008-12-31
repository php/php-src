/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:                                                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_filestat.h"
#include "php_globals.h"

#ifdef HAVE_SYMLINK

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <string.h>
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#if HAVE_GRP_H
#ifdef PHP_WIN32
#include "win32/grp.h"
#else
#include <grp.h>
#endif
#endif
#include <errno.h>
#include <ctype.h>

#include "safe_mode.h"
#include "php_link.h"
#include "php_string.h"

/* {{{ proto string readlink(string filename)
   Return the target of a symbolic link */
PHP_FUNCTION(readlink)
{
	zval **filename;
	char buff[MAXPATHLEN];
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	if (PG(safe_mode) && !php_checkuid(Z_STRVAL_PP(filename), NULL, CHECKUID_CHECK_FILE_AND_DIR)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(filename) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ret = readlink(Z_STRVAL_PP(filename), buff, MAXPATHLEN-1);

	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	/* Append NULL to the end of the string */
	buff[ret] = '\0';

	RETURN_STRING(buff, 1);
}
/* }}} */

/* {{{ proto int linkinfo(string filename)
   Returns the st_dev field of the UNIX C stat structure describing the link */
PHP_FUNCTION(linkinfo)
{
	zval **filename;
	struct stat sb;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	ret = VCWD_LSTAT(Z_STRVAL_PP(filename), &sb);
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_LONG(-1L);
	}

	RETURN_LONG((long) sb.st_dev);
}
/* }}} */

/* {{{ proto int symlink(string target, string link)
   Create a symbolic link */
PHP_FUNCTION(symlink)
{
	zval **topath, **frompath;
	int ret;
	char source_p[MAXPATHLEN];
	char dest_p[MAXPATHLEN];
	char dirname[MAXPATHLEN];
	size_t len;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &topath, &frompath) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(topath);
	convert_to_string_ex(frompath);

	if (!expand_filepath(Z_STRVAL_PP(frompath), source_p TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	memcpy(dirname, source_p, sizeof(source_p));
	len = php_dirname(dirname, strlen(dirname));

	if (!expand_filepath_ex(Z_STRVAL_PP(topath), dest_p, dirname, len TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	if (php_stream_locate_url_wrapper(source_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) ||
		php_stream_locate_url_wrapper(dest_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) ) 
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to symlink to a URL");
		RETURN_FALSE;
	}

	if (PG(safe_mode) && !php_checkuid(dest_p, NULL, CHECKUID_CHECK_FILE_AND_DIR)) {
		RETURN_FALSE;
	}

	if (PG(safe_mode) && !php_checkuid(source_p, NULL, CHECKUID_CHECK_FILE_AND_DIR)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(dest_p TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(source_p TSRMLS_CC)) {
		RETURN_FALSE;
	}

	/* For the source, an expanded path must be used (in ZTS an other thread could have changed the CWD).
	 * For the target the exact string given by the user must be used, relative or not, existing or not.
	 * The target is relative to the link itself, not to the CWD. */
	ret = symlink(Z_STRVAL_PP(topath), source_p);

	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int link(string target, string link)
   Create a hard link */
PHP_FUNCTION(link)
{
	zval **topath, **frompath;
	int ret;
	char source_p[MAXPATHLEN];
	char dest_p[MAXPATHLEN];

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &topath, &frompath) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(topath);
	convert_to_string_ex(frompath);

	if (!expand_filepath(Z_STRVAL_PP(frompath), source_p TSRMLS_CC) || !expand_filepath(Z_STRVAL_PP(topath), dest_p TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	if (php_stream_locate_url_wrapper(source_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) ||
		php_stream_locate_url_wrapper(dest_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) ) 
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to link to a URL");
		RETURN_FALSE;
	}

	if (PG(safe_mode) && !php_checkuid(dest_p, NULL, CHECKUID_CHECK_FILE_AND_DIR)) {
		RETURN_FALSE;
	}

	if (PG(safe_mode) && !php_checkuid(source_p, NULL, CHECKUID_CHECK_FILE_AND_DIR)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(dest_p TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(source_p TSRMLS_CC)) {
		RETURN_FALSE;
	}

#ifndef ZTS
	ret = link(Z_STRVAL_PP(topath), Z_STRVAL_PP(frompath));
#else 
	ret = link(dest_p, source_p);	
#endif	
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
