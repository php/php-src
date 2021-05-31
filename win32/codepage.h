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
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_WIN32_CODEPAGE_H
#define PHP_WIN32_CODEPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PHP_EXPORTS
# define PW32CP __declspec(dllexport)
#else
# define PW32CP __declspec(dllimport)
#endif

#define PHP_WIN32_CP_IGNORE_LEN (0)
#define PHP_WIN32_CP_IGNORE_LEN_P ((size_t *)-1)

struct php_win32_cp {
	DWORD id;
	DWORD to_w_fl;
	DWORD from_w_fl;
	DWORD char_size;
	char *name;
	char *enc;
	char *desc;
};

PW32CP BOOL php_win32_cp_use_unicode(void);
PW32CP const struct php_win32_cp *php_win32_cp_do_setup(const char *);
#define php_win32_cp_setup() php_win32_cp_do_setup(NULL)
PW32CP const struct php_win32_cp *php_win32_cp_do_update(const char *);
#define php_win32_cp_update() php_win32_cp_do_update(NULL)
PW32CP const struct php_win32_cp *php_win32_cp_shutdown(void);
PW32CP const struct php_win32_cp *php_win32_cp_get_current(void);
PW32CP const struct php_win32_cp *php_win32_cp_get_orig(void);
PW32CP const struct php_win32_cp *php_win32_cp_get_by_id(DWORD id);
PW32CP const struct php_win32_cp *php_win32_cp_set_by_id(DWORD id);
PW32CP const struct php_win32_cp *php_win32_cp_get_by_enc(const char *enc);
PW32CP const struct php_win32_cp *php_win32_cp_cli_do_setup(DWORD);
#define php_win32_cp_cli_setup() php_win32_cp_cli_do_setup(0)
#define php_win32_cp_cli_update() php_win32_cp_cli_do_setup(0)
PW32CP const struct php_win32_cp *php_win32_cp_cli_do_restore(DWORD);
#define php_win32_cp_cli_restore() php_win32_cp_cli_do_restore(0)

/* This API is binary safe and expects a \0 terminated input.
   The returned out is \0 terminated, but the length doesn't count \0. */
PW32CP wchar_t *php_win32_cp_conv_to_w(DWORD in_cp, DWORD flags, const char* in, size_t in_len, size_t *out_len);
PW32CP wchar_t *php_win32_cp_conv_utf8_to_w(const char* in, size_t in_len, size_t *out_len);
#define php_win32_cp_utf8_to_w(in) php_win32_cp_conv_utf8_to_w(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)
PW32CP wchar_t *php_win32_cp_conv_cur_to_w(const char* in, size_t in_len, size_t *out_len);
#define php_win32_cp_cur_to_w(in) php_win32_cp_conv_cur_to_w(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)
PW32CP wchar_t *php_win32_cp_conv_ascii_to_w(const char* in, size_t in_len, size_t *out_len);
#define php_win32_cp_ascii_to_w(in) php_win32_cp_conv_ascii_to_w(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)
PW32CP char *php_win32_cp_conv_from_w(DWORD out_cp, DWORD flags, const wchar_t* in, size_t in_len, size_t *out_len);
PW32CP char *php_win32_cp_conv_w_to_utf8(const wchar_t* in, size_t in_len, size_t *out_len);
#define php_win32_cp_w_to_utf8(in) php_win32_cp_conv_w_to_utf8(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)
PW32CP char *php_win32_cp_conv_w_to_cur(const wchar_t* in, size_t in_len, size_t *out_len);
#define php_win32_cp_w_to_cur(in) php_win32_cp_conv_w_to_cur(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)
PW32CP wchar_t *php_win32_cp_env_any_to_w(const char* env);

/* This function tries to make the best guess to convert any
   given string to a wide char, also preferring the fastest code
   path to unicode. It returns NULL on fail. */
__forceinline static wchar_t *php_win32_cp_conv_any_to_w(const char* in, size_t in_len, size_t *out_len)
{/*{{{*/
	wchar_t *ret = NULL;

	if (php_win32_cp_use_unicode()) {
		/* First try the pure ascii conversion. This is the fastest way to do the
			thing. Only applicable if the source string is UTF-8 in general.
			While it could possibly be ok with European encodings, usage with
			Asian encodings can cause unintended side effects. Lookup the term
			"mojibake" if need more. */
		ret = php_win32_cp_conv_ascii_to_w(in, in_len, out_len);

		/* If that failed, try to convert to multibyte. */
		if (!ret) {
			ret = php_win32_cp_conv_utf8_to_w(in, in_len, out_len);

			/* Still need this fallback with regard to possible broken data
				in the existing scripts. Broken data might be hardcoded in
				the user scripts, as UTF-8 settings was de facto ignored in
				older PHP versions. The fallback can be removed later for
				the sake of purity, keep now for BC reasons. */
			if (!ret) {
				const struct php_win32_cp *acp = php_win32_cp_get_by_id(GetACP());

				if (acp) {
					ret = php_win32_cp_conv_to_w(acp->id, acp->to_w_fl, in, in_len, out_len);
				}
			}
		}
	} else {
		/* No unicode, convert from the current thread cp. */
		ret = php_win32_cp_conv_cur_to_w(in, in_len, out_len);
	}

	return ret;
}/*}}}*/
#define php_win32_cp_any_to_w(in) php_win32_cp_conv_any_to_w(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)

/* This function converts from unicode function output back to PHP. If
	the PHP's current charset is not compatible with unicode, so the currently
	configured CP will be used. */
__forceinline static char *php_win32_cp_conv_w_to_any(const wchar_t* in, size_t in_len, size_t *out_len)
{/*{{{*/
	return php_win32_cp_conv_w_to_cur(in, in_len, out_len);
}/*}}}*/
#define php_win32_cp_w_to_any(in) php_win32_cp_conv_w_to_any(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)

#define PHP_WIN32_CP_W_TO_ANY_ARRAY(aw, aw_len, aa, aa_len) do { \
	int i; \
	aa_len = aw_len; \
	aa = (char **) malloc(aw_len * sizeof(char *)); \
	if (!aa) { \
		break; \
	} \
	for (i = 0; i < aw_len; i++) { \
		aa[i] = php_win32_cp_w_to_any(aw[i]); \
	} \
} while (0);


#define PHP_WIN32_CP_FREE_ARRAY(a, a_len) do { \
	int i; \
	for (i = 0; i < a_len; i++) { \
		free(a[i]); \
	} \
	free(a); \
} while (0);

#ifdef __cplusplus
}
#endif

#endif /* PHP_WIN32_CODEPAGE_H */
