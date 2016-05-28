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

ZEND_TLS const struct php_win32_cp *cur_cp  = NULL;
ZEND_TLS const struct php_win32_cp *orig_cp = NULL;

#include "cp_enc_map.c"

__forceinline static wchar_t *php_win32_cp_to_w_int(const char* in, size_t in_len, size_t *out_len, UINT cp, DWORD flags)
{/*{{{*/
	wchar_t *ret;
	int ret_len, tmp_len;

	if (!in || in_len > (size_t)INT_MAX) {
		return NULL;
	}
	assert(in_len ? in[in_len] == L'\0' : 1);

	tmp_len = !in_len ? -1 : (int)in_len + 1;

	ret_len = MultiByteToWideChar(cp, flags, in, tmp_len, NULL, 0);
	if (ret_len == 0) {
		return NULL;
	}

	ret = malloc(ret_len * sizeof(wchar_t));
	if (!ret) {
		return NULL;
	}

	tmp_len = MultiByteToWideChar(cp, flags, in, tmp_len, ret, ret_len);
	if (tmp_len == 0) {
		free(ret);
		return NULL;
	}

	assert(ret ? tmp_len == ret_len : 1);
	assert(ret ? wcslen(ret) == ret_len - 1 : 1);

	ret[ret_len-1] = L'\0';

	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
		*out_len = ret_len - 1;
	}

	return ret;
}/*}}}*/

PW32CP wchar_t *php_win32_cp_conv_utf8_to_w(const char* in, size_t in_len, size_t *out_len)
{/*{{{*/
	return php_win32_cp_to_w_int(in, in_len, out_len, CP_UTF8, MB_ERR_INVALID_CHARS);
}/*}}}*/

PW32CP wchar_t *php_win32_cp_conv_cur_to_w(const char* in, size_t in_len, size_t *out_len)
{/*{{{*/
	wchar_t *ret;

	ret = php_win32_cp_to_w_int(in, in_len, out_len, cur_cp->id, cur_cp->from_w_fl);
#if 0
	if (!ret) {
		ret = php_win32_cp_to_w_int(in, in_len, out_len, CP_THREAD_ACP, 0);
	}
#endif
	return ret;
}/*}}}*/

PW32CP wchar_t *php_win32_cp_conv_ascii_to_w(const char* in, size_t in_len, size_t *out_len)
{/*{{{*/
	wchar_t *ret = NULL;
	const char *idx = in, *end; 

	assert(in && in_len ? in[in_len] == '\0' : 1);

	if (!in) {
		return NULL;
	} else if (0 == in_len) {
		/* Not binary safe. */
		in_len = strlen(in);
		if (in_len > (size_t)INT_MAX) {
			return NULL;
		}
	}

	end = in + in_len;

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

		ret = malloc((in_len+1)*sizeof(wchar_t));
		if (!ret) {
			return NULL;
		}

		ret_idx = ret;
		do {
			k = _snwprintf(ret_idx, in_len - i, L"%.*hs", (int)(in_len - i), in);

			if (-1 == k) {
				free(ret);
				return NULL;
			}

			i += k + 1;

			if (i < in_len) {
				/* Advance as this seems to be a string with \0 in it. */
				in += k + 1;
				ret_idx += k + 1;
			}


		} while (i < in_len);
		ret[in_len] = L'\0';

		assert(ret ? wcslen(ret) == in_len : 1);

		if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
			*out_len = in_len;
		}
	} else {
		if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
			*out_len = 0;
		}
	}

	return ret;
}/*}}}*/

__forceinline static char *php_win32_cp_from_w_int(wchar_t* in, size_t in_len, size_t *out_len, UINT cp, DWORD flags)
{/*{{{*/
	int r;
	int target_len, tmp_len;
	char* target;

	if (!in || in_len > INT_MAX) {
		return NULL;
	}
	assert(in_len ? in[in_len] == '\0' : 1);

	tmp_len = !in_len ? -1 : (int)in_len + 1;

	target_len = WideCharToMultiByte(cp, flags, in, tmp_len, NULL, 0, NULL, NULL);
	if (target_len == 0) {
		return NULL;
	}

	target = malloc(target_len);
	if (target == NULL) {
		SetLastError(ERROR_OUTOFMEMORY);
		_set_errno(ENOMEM);
		return NULL;
	}

	r = WideCharToMultiByte(cp, flags, in, tmp_len, target, target_len, NULL, NULL);
	if (r == 0) {
		free(target);
		return NULL;
	}

	assert(target ? r == target_len : 1);
	assert(target ? strlen(target) == target_len - 1 : 1);

	target[target_len-1] = '\0';

	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
		*out_len = target_len - 1;
	}

	return target;
}/*}}}*/

PW32CP char *php_win32_cp_conv_w_to_utf8(wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	return php_win32_cp_from_w_int(in, in_len, out_len, CP_UTF8, WC_ERR_INVALID_CHARS);
}/*}}}*/

PW32CP char *php_win32_cp_conv_w_to_thread(wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	char *ret;

	ret = php_win32_cp_from_w_int(in, in_len, out_len, cur_cp->id, cur_cp->from_w_fl);
#if 0
	if (!ret) {
		ret = php_win32_cp_from_w_int(in, in_len, out_len, CP_THREAD_ACP, 0);
	}
#endif
	return ret;
}/*}}}*/

/* #define PHP_WIN32_CP_ENC_STR_UTF8(enc) ((len = strlen(enc)) != 0 && sizeof("UTF-8")-1 == len && (zend_binary_strcasecmp(enc, len, "UTF-8", sizeof("UTF-8")-1) == 0))*/

/* This is only usable after the startup phase*/
__forceinline static char *php_win32_cp_get_enc(void)
{/*{{{*/
	char *enc = NULL;
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

	return enc;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_get_current(void)
{/*{{{*/
	return cur_cp;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_get_orig(void)
{/*{{{*/
	return orig_cp;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_get_by_id(DWORD id)
{/*{{{*/
	size_t i;

	for (i = 0; i < sizeof(php_win32_cp_map)/sizeof(struct php_win32_cp); i++) {
		const struct php_win32_cp *cp = &php_win32_cp_map[i];

		if (cp->id == id) {
			return cp;
		}
	}

	return NULL;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_get_by_enc(char *enc)
{/*{{{*/
	size_t enc_len = 0, i;

	if (!enc || !enc[0]) {
		return php_win32_cp_get_by_id(65001U);
	}

	enc_len = strlen(enc);

	for (i = 0; i < sizeof(php_win32_cp_map)/sizeof(struct php_win32_cp); i++) {
		const struct php_win32_cp *cp = &php_win32_cp_map[i];

		if (!cp->name || !cp->name[0]) {
			continue;
		}

		if (0 == zend_binary_strcasecmp(enc, enc_len, cp->name, sizeof(cp->name) - 1)) {
			cur_cp = cp;
			return cp;
		}

		if (cp->enc) {
			char *start = cp->enc, *idx;

			idx = strpbrk(start, "|");

			while (NULL != idx) {
				if (0 == zend_binary_strcasecmp(enc, enc_len, start, idx - start)) {
					cur_cp = cp;
					return cp;
				}
				start = idx + 1;
				idx = strpbrk(start, "|");
			}
			/* Last in the list, or single charset specified. */
			if (0 == zend_binary_strcasecmp(enc, enc_len, start, strlen(start))) {
				cur_cp = cp;
				return cp;
			}
		}
	}

	return php_win32_cp_get_by_id(GetACP());
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_set_by_id(DWORD id)
{/*{{{*/
	const struct php_win32_cp *tmp;
	if (!IsValidCodePage(id)) {
		return NULL;
	}

	tmp = php_win32_cp_get_by_id(id);
	if (tmp) {
		cur_cp = tmp;
	}

	return cur_cp;
}/*}}}*/

PW32CP BOOL php_win32_cp_use_unicode(void)
{/*{{{*/
	return 65001 == cur_cp->id;
}/*}}}*/

PW32CP wchar_t *php_win32_cp_env_any_to_w(const char* env)
{/*{{{*/
	wchar_t *envw = NULL, ew[32760];
	char *cur = (char *)env, *prev;
	size_t bin_len = 0;

	if (!env) {
		return NULL;
	}

	do {
		wchar_t *tmp;
		size_t tmp_len;

		tmp = php_win32_cp_any_to_w(cur);
		if (tmp) {
			tmp_len = wcslen(tmp) + 1;
			memmove(ew + bin_len, tmp, tmp_len * sizeof(wchar_t));
			free(tmp);

			bin_len += tmp_len;
		}

		prev = cur;

	} while (NULL != (cur = strchr(prev, '\0')) && cur++ && *cur && bin_len + (cur - prev) < 32760);

	envw = (wchar_t *) malloc((bin_len + 3) * sizeof(wchar_t));	
	memmove(envw, ew, bin_len * sizeof(wchar_t));
	envw[bin_len] = L'\0';
	envw[bin_len + 1] = L'\0';
	envw[bin_len + 2] = L'\0';

	return envw;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_do_setup(char *enc)
{/*{{{*/
	if (!enc) {
		enc = php_win32_cp_get_enc();
	}

	if (!strcmp(sapi_module.name, "cli")) {
		orig_cp = php_win32_cp_get_by_id(GetConsoleCP());
	} else {
		orig_cp = php_win32_cp_get_by_id(GetACP());
	}
	cur_cp = php_win32_cp_get_by_enc(enc);

	return cur_cp;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_do_update(char *enc)
{/*{{{*/
	if (!enc) {
		enc = php_win32_cp_get_enc();
	}
	cur_cp = php_win32_cp_get_by_enc(enc);

	if (!strcmp(sapi_module.name, "cli")) {
		php_win32_cp_cli_update();
	}

	return cur_cp;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_shutdown(void)
{/*{{{*/
	return orig_cp;
}/*}}}*/

/* php_win32_cp_setup() needs to have run before! */
PW32CP const struct php_win32_cp *php_win32_cp_cli_do_setup(DWORD id)
{/*{{{*/
	const struct php_win32_cp *cp;

	if (!orig_cp) {
		php_win32_cp_setup();
	}

	if (id) {
		cp = php_win32_cp_set_by_id(id);
	} else {
		cp = cur_cp;
	}

	if (!cp) {
		return NULL;
	}

	if (SetConsoleOutputCP(cp->id) && SetConsoleCP(cp->id)) {
		return cp;
	}

	return cp;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_cli_do_restore(DWORD id)
{/*{{{*/
	if (!id && orig_cp) {
		id = orig_cp->id;
	}

	if (SetConsoleOutputCP(id) && SetConsoleCP(id)) {
		if (orig_cp) {
			return orig_cp;
		} else {
			return php_win32_cp_set_by_id(id);
		}
	}

	return NULL;
}/*}}}*/

/* Userspace functions, see basic_functions.* for arginfo and decls. */

/* {{{ proto bool sapi_windows_set_cp(int cp)
 * Set process codepage. */
PHP_FUNCTION(sapi_windows_set_cp) 
{
	zend_long id;
	const struct php_win32_cp *cp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE) {
		return;
	}

	if (ZEND_LONG_UINT_OVFL(id)) {
		php_error_docref(NULL, E_WARNING, "Argument %d is out of range", id);
		RETURN_FALSE;
	}

	if (!strcmp(sapi_module.name, "cli")) {
		cp = php_win32_cp_cli_do_setup((DWORD)id);
	} else {
		cp = php_win32_cp_set_by_id((DWORD)id);
	}
	if (!cp) {
		php_error_docref(NULL, E_WARNING, "Failed to switch to codepage %d", id);
		RETURN_FALSE;
	}

	RETURN_BOOL(cp->id == id);
}
/* }}} */

/* {{{ proto int sapi_windows_get_cp([string kind])
 * Get process codepage. */
PHP_FUNCTION(sapi_windows_get_cp)
{
	char *kind;
	size_t kind_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &kind, &kind_len) == FAILURE) {
		return;
	}

	if (kind_len == sizeof("ansi")-1 && !strncasecmp(kind, "ansi", kind_len)) {
		RETURN_LONG(GetACP());
	} else if (kind_len == sizeof("oem")-1 && !strncasecmp(kind, "oem", kind_len)) {
		RETURN_LONG(GetOEMCP());
	} else {
		const struct php_win32_cp *cp = php_win32_cp_get_current();
		RETURN_LONG(cp->id);
	}
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

	tmp = php_win32_cp_utf8_to_w(subj);
	if (!tmp) {
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	subj = php_win32_cp_conv_w_to_thread(tmp, PHP_WIN32_CP_IGNORE_LEN, &subj_len);
	if (!subj) {
		free(tmp);
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	RETVAL_STRINGL(subj, subj_len);

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

	tmp = php_win32_cp_cur_to_w(subj);
	if (!tmp) {
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	subj = php_win32_cp_conv_w_to_utf8(tmp, PHP_WIN32_CP_IGNORE_LEN, &subj_len);
	if (!subj) {
		free(tmp);
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	RETVAL_STRINGL(subj, subj_len);

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
