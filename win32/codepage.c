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

#define _CRT_STDIO_ISO_WIDE_SPECIFIERS 1

#include <assert.h>

#include "php.h"
#include "SAPI.h"

PW32CP wchar_t *php_win32_cp_mb_to_w_full(const char* path, size_t path_len, size_t *pathw_len)
{/*{{{*/
	wchar_t *ret;
	int ret_len, tmp_len;

	if (!path || path_len > (size_t)INT_MAX) {
		return NULL;
	}

	tmp_len = !path_len ? -1 : (int)path_len + 1;

	ret_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, tmp_len, NULL, 0);
	if (ret_len == 0) {
		return NULL;
	}

	ret = malloc(ret_len * sizeof(wchar_t));
	if (!ret) {
		return NULL;
	}

	tmp_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, tmp_len, ret, ret_len);

	assert(tmp_len == ret_len);

	if (PHP_WIN32_CP_IGNORE_LEN_P != pathw_len) {
		*pathw_len = ret_len - 1;
	}

	return ret;
}/*}}}*/

PW32CP wchar_t *php_win32_cp_thread_to_w_full(const char* path, size_t path_len, size_t *pathw_len)
{/*{{{*/
	wchar_t *ret;
	int ret_len, tmp_len;

	if (!path || path_len > (size_t)INT_MAX) {
		return NULL;
	}

	tmp_len = !path_len ? -1 : (int)path_len + 1;

	ret_len = MultiByteToWideChar(CP_THREAD_ACP, 0, path, tmp_len, NULL, 0);
	if (ret_len == 0) {
		return NULL;
	}

	ret = malloc(ret_len * sizeof(wchar_t));
	if (!ret) {
		return NULL;
	}

	tmp_len = MultiByteToWideChar(CP_THREAD_ACP, 0, path, tmp_len, ret, ret_len);

	assert(tmp_len == ret_len);

	if (PHP_WIN32_CP_IGNORE_LEN_P != pathw_len) {
		*pathw_len = ret_len - 1;
	}

	return ret;

}/*}}}*/

/* \0 terminated string is expected! */
PW32CP wchar_t *php_win32_cp_ascii_to_w_full(const char* path, size_t path_len, size_t *pathw_len)
{/*{{{*/
	wchar_t *ret = NULL;
	const char *idx = path, *end; 
	size_t it;

	if (!path) {
		return NULL;
	} else if (0 == (size_t)path_len) {
		/* Not binary safe. */
		path_len = strlen(path);
	}

	end = path + path_len;

	while (idx != end) {
		if (!__isascii(*idx) && '\0' != *idx) {
			break;
		}
		idx++;
	}

	if (idx == end) {
		size_t i = 0;
		int k = 0;
		wchar_t *ret_idx;

		ret = malloc((path_len+1)*sizeof(wchar_t));
		if (!ret) {
			return NULL;
		}

		ret_idx = ret;
		do {
			k = _snwprintf(ret_idx, path_len - i, L"%.*hs", (int)path_len - i, path);

			if (-1 == k) {
				free(ret);
				return NULL;
			}

			i += k + 1;

			if (i < path_len) {
				/* Advance as this seems to be a string with \0 in it. */
				path += k + 1;
				ret_idx += k + 1;
			}


		} while (i < path_len);
		ret[path_len] = L'\0';
	}

	if (PHP_WIN32_CP_IGNORE_LEN_P != pathw_len) {
		*pathw_len = path_len;
	}

	return ret;
}/*}}}*/

PW32CP char *php_win32_cp_w_to_utf8_full(wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	int r;
	int target_len, tmp_len;
	char* target;

	if (!in || in_len > INT_MAX) {
		return NULL;
	}

	tmp_len = !in_len ? -1 : (int)in_len + 1;

	target_len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in, tmp_len, NULL, 0, NULL, NULL);
	if (target_len == 0) {
		return NULL;
	}

	target = malloc(target_len);
	if (target == NULL) {
		SetLastError(ERROR_OUTOFMEMORY);
		_set_errno(ENOMEM);
		return NULL;
	}

	r = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, in, tmp_len, target, target_len, NULL, NULL);

	assert(r == target_len);

	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
		*out_len = target_len - 1;
	}

	return target;
}/*}}}*/

PW32CP char *php_win32_cp_w_to_thread_full(wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	int r;
	int target_len, tmp_len;
	char* target;

	if (!in || in_len > (size_t)INT_MAX) {
		return NULL;
	}

	tmp_len = !in_len ? -1 : (int)in_len + 1;

	target_len = WideCharToMultiByte(CP_THREAD_ACP, 0, in, tmp_len, NULL, 0, NULL, NULL);
	if (target_len == 0) {
		return NULL;
	}

	target = malloc(target_len);
	if (target == NULL) {
		SetLastError(ERROR_OUTOFMEMORY);
		_set_errno(ENOMEM);
		return NULL;
	}

	r = WideCharToMultiByte(CP_THREAD_ACP, 0, in, tmp_len, target, target_len, NULL, NULL);

	assert(r == target_len);

	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
		*out_len = target_len - 1;
	}

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

PW32CP wchar_t *php_win32_cp_env_any_to_w(const char* env)
{/*{{{*/
	wchar_t *envw = NULL, ew[32760];
	char *cur = env, *prev;
	size_t bin_len = 0;

	if (!env) {
		return NULL;
	}

	do {
		wchar_t *tmp;
		size_t tmp_len;

		tmp = php_win32_cp_any_to_w(cur);
		tmp_len = wcslen(tmp) + 1;
		memmove(ew + bin_len, tmp, tmp_len * sizeof(wchar_t));
		free(tmp);

		bin_len += tmp_len;
		prev = cur;

	} while (NULL != (cur = strchr(prev, '\0')) && cur++ && *cur && bin_len + (cur - prev) < 32760);

	envw = (wchar_t *) malloc((bin_len + 3) * sizeof(wchar_t));	
	memmove(envw, ew, bin_len * sizeof(wchar_t));
	envw[bin_len] = L'\0';
	envw[bin_len + 1] = L'\0';
	envw[bin_len + 2] = L'\0';

	return envw;
}/*}}}*/

/* Userspace functions, see basic_functions.* for arginfo and decls. */

/* {{{ proto bool sapi_windows_set_cp(int cp)
 * Set process codepage. */
PHP_FUNCTION(sapi_windows_set_cp) 
{
	zend_long cp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &cp) == FAILURE) {
		return;
	}

	RETURN_BOOL(SetConsoleOutputCP((UINT)cp) && SetConsoleCP((UINT)cp));
}
/* }}} */

/* {{{ proto int sapi_windows_set_cp(void)
 * Get process codepage. */
PHP_FUNCTION(sapi_windows_get_cp)
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


/* {{{ proto bool sapi_windows_is_cp_utf8(void)
 * Indicates whether the codepage is UTF-8 compatible. */
PHP_FUNCTION(sapi_windows_is_cp_utf8)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(php_win32_cp_use_unicode());
}
/* }}} */

/* {{{ proto string sapi_windows_cp_conv_utf8_to_thread(string subject)
 * Convert string from UTF-8 to the current thread codepage. */
PHP_FUNCTION(sapi_windows_cp_conv_utf8_to_thread)
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

/* {{{ proto string sapi_windows_cp_conv_thread_to_utf8(string subject)
 * Convert string from the current thread codepage to UTF-8. */
PHP_FUNCTION(sapi_windows_cp_conv_thread_to_utf8)
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
