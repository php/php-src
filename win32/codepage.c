/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
#include <emmintrin.h>

#include "win32/console.h"

ZEND_TLS const struct php_win32_cp *cur_cp = NULL;
ZEND_TLS const struct php_win32_cp *orig_cp = NULL;
ZEND_TLS const struct php_win32_cp *cur_out_cp = NULL;
ZEND_TLS const struct php_win32_cp *orig_out_cp = NULL;
ZEND_TLS const struct php_win32_cp *cur_in_cp = NULL;
ZEND_TLS const struct php_win32_cp *orig_in_cp = NULL;

#include "cp_enc_map.c"

__forceinline static wchar_t *php_win32_cp_to_w_int(const char* in, size_t in_len, size_t *out_len, UINT cp, DWORD flags)
{/*{{{*/
	wchar_t *ret;
	int ret_len, tmp_len;

	if (!in || in_len > (size_t)INT_MAX) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}
	assert(in_len ? (in[in_len] == L'\0') : 1);

	tmp_len = !in_len ? -1 : (int)in_len + 1;

	ret_len = MultiByteToWideChar(cp, flags, in, tmp_len, NULL, 0);
	if (ret_len == 0) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return NULL;
	}

	ret = malloc(ret_len * sizeof(wchar_t));
	if (!ret) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_OUTOFMEMORY);
		return NULL;
	}

	tmp_len = MultiByteToWideChar(cp, flags, in, tmp_len, ret, ret_len);
	if (tmp_len == 0) {
		free(ret);
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return NULL;
	}

	assert(ret ? tmp_len == ret_len : 1);
	assert(ret && !in_len ? wcslen(ret) == ret_len - 1 : 1);

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

	return ret;
}/*}}}*/

PW32CP wchar_t *php_win32_cp_conv_to_w(DWORD cp, DWORD flags, const char* in, size_t in_len, size_t *out_len)
{/*{{{*/
	return php_win32_cp_to_w_int(in, in_len, out_len, cp, flags);
}/*}}}*/

#define ASCII_FAIL_RETURN() \
	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) { \
		*out_len = 0; \
	} \
	return NULL;
PW32CP wchar_t *php_win32_cp_conv_ascii_to_w(const char* in, size_t in_len, size_t *out_len)
{/*{{{*/
	wchar_t *ret, *ret_idx;
	const char *idx = in, *end;
	char ch_err = 0;

#if PHP_DEBUG
	size_t save_in_len = in_len;
#endif

	assert(in && in_len ? in[in_len] == '\0' : 1);

	if (!in) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	} else if (0 == in_len) {
		/* Not binary safe. */
		in_len = strlen(in);
	}

	end = in + in_len;

	if (in_len > 15) {
		const char *aidx = (const char *)ZEND_SLIDE_TO_ALIGNED16(in);

		/* Process unaligned chunk. */
		while (idx < aidx) {
			ch_err |= *idx;
			idx++;
		}
		if (ch_err & 0x80) {
			ASCII_FAIL_RETURN()
		}

		/* Process aligned chunk. */
		__m128i vec_err = _mm_setzero_si128();
		while (end - idx > 15) {
			const __m128i block = _mm_load_si128((__m128i *)idx);
			vec_err = _mm_or_si128(vec_err, block);
			idx += 16;
		}
		if (_mm_movemask_epi8(vec_err)) {
			ASCII_FAIL_RETURN()
		}
	}

	/* Process the trailing part, or otherwise process string < 16 bytes. */
	while (idx < end) {
		ch_err |= *idx;
		idx++;
	}
	if (ch_err & 0x80) {
		ASCII_FAIL_RETURN()
	}

	ret = malloc((in_len+1)*sizeof(wchar_t));
	if (!ret) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_OUTOFMEMORY);
		return NULL;
	}

	ret_idx = ret;
	idx = in;

	if (in_len > 15) {
		const char *aidx = (const char *)ZEND_SLIDE_TO_ALIGNED16(in);

		/* Process unaligned chunk. */
		while (idx < aidx) {
			*ret_idx++ = (wchar_t)*idx++;
		}

		/* Process aligned chunk. */
		if (end - idx > 15) {
			const __m128i mask = _mm_set1_epi32(0);
			while (end - idx > 15) {
				const __m128i block = _mm_load_si128((__m128i *)idx);

				const __m128i lo = _mm_unpacklo_epi8(block, mask);
				_mm_storeu_si128((__m128i *)ret_idx, lo);

				ret_idx += 8;
				const __m128i hi = _mm_unpackhi_epi8(block, mask);
				_mm_storeu_si128((__m128i *)ret_idx, hi);

				idx += 16;
				ret_idx += 8;
			}
		}
	}

	/* Process the trailing part, or otherwise process string < 16 bytes. */
	while (idx < end) {
		*ret_idx++ = (wchar_t)*idx++;
	}

	ret[in_len] = L'\0';

	assert(ret && !save_in_len ? wcslen(ret) == in_len : 1);

	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
		*out_len = in_len;
	}

	return ret;
}/*}}}*/
#undef ASCII_FAIL_RETURN

__forceinline static char *php_win32_cp_from_w_int(const wchar_t* in, size_t in_len, size_t *out_len, UINT cp, DWORD flags)
{/*{{{*/
	int r;
	int target_len, tmp_len;
	char* target;

	if (!in || in_len > INT_MAX) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}
	assert(in_len ? in[in_len] == '\0' : 1);

	tmp_len = !in_len ? -1 : (int)in_len + 1;

	target_len = WideCharToMultiByte(cp, flags, in, tmp_len, NULL, 0, NULL, NULL);
	if (target_len == 0) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return NULL;
	}

	target = malloc(target_len);
	if (target == NULL) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_OUTOFMEMORY);
		return NULL;
	}

	r = WideCharToMultiByte(cp, flags, in, tmp_len, target, target_len, NULL, NULL);
	if (r == 0) {
		free(target);
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return NULL;
	}

	assert(target ? r == target_len : 1);
	assert(target && !in_len ? strlen(target) == target_len - 1 : 1);

	target[target_len-1] = '\0';

	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
		*out_len = target_len - 1;
	}

	return target;
}/*}}}*/

PW32CP char *php_win32_cp_conv_w_to_utf8(const wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	return php_win32_cp_from_w_int(in, in_len, out_len, CP_UTF8, WC_ERR_INVALID_CHARS);
}/*}}}*/

PW32CP char *php_win32_cp_conv_w_to_cur(const wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	char *ret;

	ret = php_win32_cp_from_w_int(in, in_len, out_len, cur_cp->id, cur_cp->from_w_fl);

	return ret;
}/*}}}*/

PW32CP char *php_win32_cp_conv_from_w(DWORD cp, DWORD flags, const wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	return php_win32_cp_from_w_int(in, in_len, out_len, cp, flags);
}/*}}}*/

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

	if (id < php_win32_cp_map[0].id) {
		switch (id) {
			case CP_ACP:
				id = GetACP();
				break;
			case CP_OEMCP:
				id = GetOEMCP();
				break;
		}
	}

	for (i = 0; i < sizeof(php_win32_cp_map)/sizeof(struct php_win32_cp); i++) {
		if (php_win32_cp_map[i].id == id) {
			return &php_win32_cp_map[i];
		}
	}

	SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_FOUND);

	return NULL;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_get_by_enc(const char *enc)
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

		if (0 == zend_binary_strcasecmp(enc, enc_len, cp->name, strlen(cp->name))) {
			return cp;
		}

		if (cp->enc) {
			char *start = cp->enc, *idx;

			idx = strpbrk(start, "|");

			while (NULL != idx) {
				if (0 == zend_binary_strcasecmp(enc, enc_len, start, idx - start)) {
					return cp;
				}
				start = idx + 1;
				idx = strpbrk(start, "|");
			}
			/* Last in the list, or single charset specified. */
			if (0 == zend_binary_strcasecmp(enc, enc_len, start, strlen(start))) {
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
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
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
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	do {
		wchar_t *tmp;

		tmp = php_win32_cp_any_to_w(cur);
		if (tmp) {
			size_t tmp_len = wcslen(tmp) + 1;
			memmove(ew + bin_len, tmp, tmp_len * sizeof(wchar_t));
			free(tmp);

			bin_len += tmp_len;
		}

		prev = cur;

	} while (NULL != (cur = strchr(prev, '\0')) && cur++ && *cur && bin_len + (cur - prev) < 32760);

	envw = (wchar_t *) malloc((bin_len + 3) * sizeof(wchar_t));
	if (!envw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_OUTOFMEMORY);
		return NULL;
	}
	memmove(envw, ew, bin_len * sizeof(wchar_t));
	envw[bin_len] = L'\0';
	envw[bin_len + 1] = L'\0';
	envw[bin_len + 2] = L'\0';

	return envw;
}/*}}}*/

static BOOL php_win32_cp_cli_io_setup(void)
{/*{{{*/
	BOOL ret = TRUE;

	if (PG(input_encoding) && PG(input_encoding)[0]) {
		cur_in_cp = php_win32_cp_get_by_enc(PG(input_encoding));
		if (!cur_in_cp) {
			cur_in_cp = cur_cp;
		}
	} else {
		cur_in_cp = cur_cp;
	}

	if (PG(output_encoding) && PG(output_encoding)[0]) {
		cur_out_cp = php_win32_cp_get_by_enc(PG(output_encoding));
		if (!cur_out_cp) {
			cur_out_cp = cur_cp;
		}
	} else {
		cur_out_cp = cur_cp;
	}

	if(php_get_module_initialized()) {
		ret = SetConsoleCP(cur_in_cp->id) && SetConsoleOutputCP(cur_out_cp->id);
	}

	return ret;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_do_setup(const char *enc)
{/*{{{*/
	if (!enc) {
		enc = php_win32_cp_get_enc();
	}

	cur_cp = php_win32_cp_get_by_enc(enc);
	if (!orig_cp) {
		orig_cp = php_win32_cp_get_by_id(GetACP());
	}
	if (php_win32_console_is_cli_sapi()) {
		if (!orig_in_cp) {
			orig_in_cp = php_win32_cp_get_by_id(GetConsoleCP());
			if (!orig_in_cp) {
				orig_in_cp = orig_cp;
			}
		}
		if (!orig_out_cp) {
			orig_out_cp = php_win32_cp_get_by_id(GetConsoleOutputCP());
			if (!orig_out_cp) {
				orig_out_cp = orig_cp;
			}
		}
		php_win32_cp_cli_io_setup();
	}

	return cur_cp;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_do_update(const char *enc)
{/*{{{*/
	if (!enc) {
		enc = php_win32_cp_get_enc();
	}
	cur_cp = php_win32_cp_get_by_enc(enc);

	if (php_win32_console_is_cli_sapi()) {
		php_win32_cp_cli_do_setup(cur_cp->id);
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

	if (php_win32_cp_cli_io_setup()) {
		return cp;
	}

	return cp;
}/*}}}*/

PW32CP const struct php_win32_cp *php_win32_cp_cli_do_restore(DWORD id)
{/*{{{*/
	BOOL cli_io_restored = TRUE;

	if (orig_in_cp) {
		cli_io_restored = cli_io_restored && SetConsoleCP(orig_in_cp->id);
	}

	if (orig_out_cp) {
		cli_io_restored = cli_io_restored && SetConsoleOutputCP(orig_out_cp->id);
	}

	if (cli_io_restored && id) {
		return php_win32_cp_set_by_id(id);
	}

	return NULL;
}/*}}}*/

/* Userspace functions, see basic_functions.* for arginfo and decls. */

/* {{{ Set process codepage. */
PHP_FUNCTION(sapi_windows_cp_set)
{
	zend_long id;
	const struct php_win32_cp *cp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZEND_LONG_UINT_OVFL(id)) {
		zend_argument_value_error(1, "must be between 0 and %u", UINT_MAX);
		RETURN_THROWS();
	}

	if (php_win32_console_is_cli_sapi()) {
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

/* {{{ Get process codepage. */
PHP_FUNCTION(sapi_windows_cp_get)
{
	char *kind;
	size_t kind_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &kind, &kind_len) == FAILURE) {
		RETURN_THROWS();
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


/* {{{ Indicates whether the codepage is UTF-8 compatible. */
PHP_FUNCTION(sapi_windows_cp_is_utf8)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(php_win32_cp_use_unicode());
}
/* }}} */

/* {{{ Convert string from one codepage to another. */
PHP_FUNCTION(sapi_windows_cp_conv)
{
	char *ret;
	size_t ret_len, tmpw_len;
	wchar_t *tmpw;
	const struct php_win32_cp *in_cp, *out_cp;
	zend_string *string_in_codepage = NULL;
	zend_long int_in_codepage = 0;
	zend_string *string_out_codepage = NULL;
	zend_long int_out_codepage = 0;
	zend_string *subject;

	ZEND_PARSE_PARAMETERS_START(3, 3)
    	Z_PARAM_STR_OR_LONG(string_in_codepage, int_in_codepage)
    	Z_PARAM_STR_OR_LONG(string_out_codepage, int_out_codepage)
    	Z_PARAM_STR(subject)
    ZEND_PARSE_PARAMETERS_END();

	if (ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(subject))) {
		zend_argument_value_error(1, "is too long");
		RETURN_THROWS();
	}

	if (string_in_codepage != NULL) {
		in_cp = php_win32_cp_get_by_enc(ZSTR_VAL(string_in_codepage));
		if (!in_cp) {
			zend_argument_value_error(1, "must be a valid charset");
			RETURN_THROWS();
		}
	} else {
		if (ZEND_LONG_UINT_OVFL(int_in_codepage)) {
			zend_argument_value_error(1, "must be between 0 and %u", UINT_MAX);
			RETURN_THROWS();
		}

		in_cp = php_win32_cp_get_by_id((DWORD)int_in_codepage);
		if (!in_cp) {
			zend_argument_value_error(1, "must be a valid codepage");
			RETURN_THROWS();
		}
	}

	if (string_out_codepage != NULL) {
		out_cp = php_win32_cp_get_by_enc(ZSTR_VAL(string_out_codepage));
		if (!out_cp) {
			zend_argument_value_error(2, "must be a valid charset");
			RETURN_THROWS();
		}
	} else {
		if (ZEND_LONG_UINT_OVFL(int_out_codepage)) {
			zend_argument_value_error(2, "must be between 0 and %u", UINT_MAX);
			RETURN_THROWS();
		}

		out_cp = php_win32_cp_get_by_id((DWORD)int_out_codepage);
		if (!out_cp) {
			zend_argument_value_error(2, "must be a valid codepage");
			RETURN_THROWS();
		}
	}

	tmpw = php_win32_cp_conv_to_w(in_cp->id, in_cp->to_w_fl, ZSTR_VAL(subject), ZSTR_LEN(subject), &tmpw_len);
	if (!tmpw) {
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	ret = php_win32_cp_conv_from_w(out_cp->id, out_cp->from_w_fl, tmpw, tmpw_len, &ret_len);
	if (!ret) {
		free(tmpw);
		php_error_docref(NULL, E_WARNING, "Wide char conversion failed");
		RETURN_NULL();
	}

	RETVAL_STRINGL(ret, ret_len);

	free(tmpw);
	free(ret);
}
/* }}} */

/* }}} */
