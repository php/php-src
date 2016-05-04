/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#include <assert.h>

#include "php.h"
#include "SAPI.h"

PW32CP wchar_t *php_win32_cp_mb_to_w(const char* path)
{/*{{{*/
	wchar_t *ret;
	int ret_len, tmp_len;

	if (!path) {
		return NULL;
	}

    ret_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (ret_len == 0) {
		return NULL;
    }

	ret = malloc(ret_len * sizeof(wchar_t));
	if (!ret) {
		return NULL;
	}

	tmp_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, ret, ret_len);

    /* assert(tmp_len == ret_len); */

	return ret;
}/*}}}*/

PW32CP wchar_t *php_win32_cp_thread_to_w(const char* path)
{/*{{{*/
	wchar_t *ret;
	int ret_len, tmp_len;

	if (!path) {
		return NULL;
	}

    ret_len = MultiByteToWideChar(CP_THREAD_ACP, 0, path, -1, NULL, 0);
    if (ret_len == 0) {
		return NULL;
    }

	ret = malloc(ret_len * sizeof(wchar_t));
	if (!ret) {
		return NULL;
	}

	tmp_len = MultiByteToWideChar(CP_THREAD_ACP, 0, path, -1, ret, ret_len);

    /* assert(tmp_len == ret_len); */

	return ret;

}/*}}}*/

PW32CP wchar_t *php_win32_cp_ascii_to_w(const char* path)
{/*{{{*/
	wchar_t *ret = NULL;
	size_t len = strlen(path);
	const char *idx = path, *end = path + len;

	while (idx != end) {
		if (!__isascii(*idx)) {
			break;
		}
		idx++;
	}

	if (idx == end) {
		ret = malloc((len+1)*sizeof(wchar_t));
		if (!ret) {
			return NULL;
		}
		if (-1 == swprintf(ret, len+1, L"%hs", path)) {
			return NULL;
		}
	}

	return ret;
}/*}}}*/

PW32CP char *php_win32_cp_w_to_utf8(wchar_t* w_source_ptr)
{/*{{{*/
	int r;
	int target_len;
	char* target;


	target_len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w_source_ptr, -1, NULL, 0, NULL, NULL);
	if (target_len == 0) {
		return NULL;
	}

	target = malloc(target_len);
	if (target == NULL) {
		SetLastError(ERROR_OUTOFMEMORY);
		_set_errno(ENOMEM);
		return NULL;
	}

	r = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w_source_ptr, -1, target, target_len, NULL, NULL);

	/*assert(r == target_len);*/

	return target;
}/*}}}*/

PW32CP char *php_win32_cp_w_to_thread(wchar_t* w_source_ptr)
{/*{{{*/
	int r;
	int target_len;
	char* target;


	target_len = WideCharToMultiByte(CP_THREAD_ACP, 0, w_source_ptr, -1, NULL, 0, NULL, NULL);
	if (target_len == 0) {
		return NULL;
	}

	target = malloc(target_len);
	if (target == NULL) {
		SetLastError(ERROR_OUTOFMEMORY);
		_set_errno(ENOMEM);
		return NULL;
	}

	r = WideCharToMultiByte(CP_THREAD_ACP, 0, w_source_ptr, -1, target, target_len, NULL, NULL);

	/*assert(r == target_len);*/

	return target;
}/*}}}*/

PW32CP BOOL php_win32_cp_use_unicode(void)
{/*{{{*/
	char *enc = NULL;
	size_t len = 0;
	const zend_encoding *zenc;

	if (PG(internal_encoding) && PG(internal_encoding)[0]) {
		enc = PG(internal_encoding);
	} else if (SG(default_charset) && SG(default_charset)[0] ) {
		enc = SG(default_charset);
	} else {
		zenc = zend_multibyte_get_internal_encoding();
		if (zenc) {
			enc = (char *)zend_multibyte_get_encoding_name(zenc);
		}
	}

	if (NULL == enc) {
		return 1;
	}

	if ((len = strlen(enc)) != 0 && sizeof("UTF-8")-1 == len &&
		(zend_binary_strcasecmp(enc, len, "UTF-8", sizeof("UTF-8")-1) == 0)) {
		return 1;
	}

	return 0;
}/*}}}*/

/* Userspace functions, see basic_functions.* for arginfo and decls. */

/* {{{ proto bool proc_set_cp(int cp)
 * Set process codepage. */
PHP_FUNCTION(proc_set_cp) 
{
	zend_long cp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &cp) == FAILURE) {
		return;
	}

	RETURN_BOOL(SetConsoleOutputCP((UINT)cp) && SetConsoleCP((UINT)cp));
}
/* }}} */

/* {{{ proto int proc_set_cp(void)
 * Get process codepage. */
PHP_FUNCTION(proc_get_cp)
{
	UINT in_cp, out_cp;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	in_cp = GetConsoleCP();
	out_cp = GetConsoleOutputCP();

	RETURN_LONG(in_cp == out_cp ? in_cp : -1);
}
/* }}} */


/* {{{ proto bool proc_is_cp_utf8(void)
 * Indicates whether the codepage is UTF-8 compatible. */
PHP_FUNCTION(proc_is_cp_utf8)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(php_win32_cp_use_unicode());
}
/* }}} */

/* {{{ proto string proc_cp_conv_utf8_to_threa(string subject)
 * Convert string from UTF-8 to the current thread codepage. */
PHP_FUNCTION(proc_cp_conv_utf8_to_thread)
{
	char *subj;
	size_t subj_len;
	wchar_t *tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &subj, &subj_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_INT_OVFL(subj_len)) {
		php_error_docref(NULL, E_WARNING, "String is too long");
		RETURN_NULL();
	}

	tmp = php_win32_cp_mb_to_w(subj);
	if (!tmp) {
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	RETVAL_STRING(php_win32_cp_w_to_thread(tmp));

	free(tmp);
}
/* }}} */

/* {{{ proto string proc_cp_conv_utf8_to_thread(string subject)
 * Convert string from the current thread codepage to UTF-8. */
PHP_FUNCTION(proc_cp_conv_thread_to_utf8)
{
	char *subj;
	size_t subj_len;
	wchar_t *tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &subj, &subj_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_INT_OVFL(subj_len)) {
		php_error_docref(NULL, E_WARNING, "String is too long");
		RETURN_NULL();
	}

	tmp = php_win32_cp_thread_to_w(subj);
	if (!tmp) {
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	RETVAL_STRING(php_win32_cp_w_to_utf8(tmp));

	free(tmp);
}

/* }}} */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
