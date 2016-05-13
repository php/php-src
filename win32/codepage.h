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

PW32CP BOOL php_win32_cp_use_unicode(void);
PW32CP wchar_t *php_win32_cp_mb_to_w(const char* path);
PW32CP wchar_t *php_win32_cp_thread_to_w(const char* path);
PW32CP wchar_t *php_win32_cp_ascii_to_w(const char* path);
PW32CP char *php_win32_cp_w_to_utf8(wchar_t* w_source_ptr);
PW32CP char *php_win32_cp_w_to_thread(wchar_t* w_source_ptr);
PW32CP wchar_t *php_win32_cp_env_any_to_w(const char* env);

/* This function tries to make the best guess to convert any
   given string to a wide char, also prefering the fastest code
   path to unicode. It returns NULL on fail. */
__forceinline static wchar_t *php_win32_cp_any_to_w(const char* in)
{
	wchar_t *ret;

	/* First try the pure ascii conversion. This is the fastest way to do the
		thing. */
	ret = php_win32_cp_ascii_to_w(in);

	/* If that failed, try to convert to multibyte. */
	if (!ret) {

		ret = php_win32_cp_mb_to_w(in);

		if (!ret) {
			ret = php_win32_cp_thread_to_w(in);
		}
	}

	return ret;
}

/* This function converts from unicode function output back to PHP. If
	the PHP's current charset is not compatible with unicode, so the current
	thread CP will be used. The latter is the default behavior in PHP < 7.1,
	as only ANSI complaint functions was used previously. */
__forceinline static char *php_win32_cp_w_to_any(wchar_t* w_source_ptr)
{
	if (php_win32_cp_use_unicode()) {
		return php_win32_cp_w_to_utf8(w_source_ptr);
	} else {
		return php_win32_cp_w_to_thread(w_source_ptr);
	}

	/* Never happens. */
	return NULL;
}

#define PHP_WIN32_CP_W_TO_A_ARRAY(aw, aw_len, aa, aa_len) do { \
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


#define PHP_WIN32_FREE_ARRAY(a, a_len) do { \
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
