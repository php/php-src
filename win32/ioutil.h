/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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

/* This file integrates several modified parts from the libuv project, which
 * is copyrighted to
 *
 * Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */ 

#ifndef PHP_WIN32_IOUTIL_H
#define PHP_WIN32_IOUTIL_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include "win32/winutil.h"
#include "win32/codepage.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PHP_EXPORTS
# define PW32IO __declspec(dllexport)
#else
# define PW32IO __declspec(dllimport)
#endif

#define PHP_WIN32_IOUTIL_MAXPATHLEN 2048

#if !defined(MAXPATHLEN) || MAXPATHLEN < PHP_WIN32_IOUTIL_MAXPATHLEN
# undef MAXPATHLEN
# define MAXPATHLEN PHP_WIN32_IOUTIL_MAXPATHLEN
#endif

#ifndef mode_t
typedef unsigned short mode_t;
#endif

typedef struct {
	DWORD access;
	DWORD share;
	DWORD disposition;
	DWORD attributes;
} php_ioutil_open_opts;

typedef enum {
	PHP_WIN32_IOUTIL_IS_ASCII,
	PHP_WIN32_IOUTIL_IS_ANSI,
	PHP_WIN32_IOUTIL_IS_UTF8
} php_win32_ioutil_encoding;

typedef enum {
	PHP_WIN32_IOUTIL_NORM_OK,
	PHP_WIN32_IOUTIL_NORM_PARTIAL,
	PHP_WIN32_IOUTIL_NORM_FAIL,
} php_win32_ioutil_normalization_result;

#define PHP_WIN32_IOUTIL_FW_SLASHW L'/'
#define PHP_WIN32_IOUTIL_FW_SLASH '/'
#define PHP_WIN32_IOUTIL_BW_SLASHW L'\\'
#define PHP_WIN32_IOUTIL_BW_SLASH '\\'
#define PHP_WIN32_IOUTIL_DEFAULT_SLASHW PHP_WIN32_IOUTIL_BW_SLASHW
#define PHP_WIN32_IOUTIL_DEFAULT_SLASH PHP_WIN32_IOUTIL_BW_SLASH

#define PHP_WIN32_IOUTIL_DEFAULT_DIR_SEPARATORW	L';'
#define PHP_WIN32_IOUTIL_IS_SLASHW(c) ((c) == PHP_WIN32_IOUTIL_BW_SLASHW || (c) == PHP_WIN32_IOUTIL_FW_SLASHW)
#define PHP_WIN32_IOUTIL_IS_LETTERW(c) (((c) >= L'a' && (c) <= L'z') || ((c) >= L'A' && (c) <= L'Z'))
#define PHP_WIN32_IOUTIL_JUNCTION_PREFIXW L"\\??\\"
#define PHP_WIN32_IOUTIL_JUNCTION_PREFIX_LENW 4
#define PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW L"\\\\?\\"
#define PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW 4
#define PHP_WIN32_IOUTIL_UNC_PATH_PREFIXW L"\\\\?\\UNC\\"
#define PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW 8

#define PHP_WIN32_IOUTIL_IS_LONG_PATHW(pathw, path_lenw) (path_lenw >= PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW \
	&& 0 == wcsncmp((pathw), PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW, PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW))
#define PHP_WIN32_IOUTIL_IS_UNC_PATHW(pathw, path_lenw) (path_lenw >= PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW \
	&& 0 == wcsncmp((pathw), PHP_WIN32_IOUTIL_UNC_PATH_PREFIXW, PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW))
#define PHP_WIN32_IOUTIL_IS_JUNCTION_PATHW(pathw, path_lenw) (path_lenw >= PHP_WIN32_IOUTIL_JUNCTION_PREFIX_LENW \
	&& 0 == wcsncmp((pathw), PHP_WIN32_IOUTIL_JUNCTION_PREFIXW, PHP_WIN32_IOUTIL_JUNCTION_PREFIX_LENW))
#define PHP_WIN32_IOUTIL_IS_ABSOLUTEW(pathw, path_lenw) (PHP_WIN32_IOUTIL_IS_LONG_PATHW(pathw, path_lenw) \
	|| path_lenw >= 3 && PHP_WIN32_IOUTIL_IS_LETTERW(pathw[0]) && L':' == pathw[1] && IS_SLASHW(pathw[2]))

#define PHP_WIN32_IOUTIL_INIT_W(path) \
	wchar_t *pathw = php_win32_ioutil_any_to_w(path); \

#define PHP_WIN32_IOUTIL_CLEANUP_W() do { \
		free(pathw); \
		pathw = NULL; \
} while (0);

#define PHP_WIN32_IOUTIL_REINIT_W(path) do { \
	PHP_WIN32_IOUTIL_CLEANUP_W() \
	pathw = php_win32_ioutil_any_to_w(path); \
} while (0);

#define PHP_WIN32_IOUTIL_PATH_IS_OK_W(pathw, len) \
	(!((len) >= 1 && L' ' == pathw[(len)-1] || \
	(len) > 1 && !PHP_WIN32_IOUTIL_IS_SLASHW(pathw[(len)-2]) && L'.' != pathw[(len)-2] && L'.' == pathw[(len)-1]))

#define PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, ret, dealloc) do { \
		if (!PHP_WIN32_IOUTIL_PATH_IS_OK_W(pathw, wcslen(pathw))) { \
			if (dealloc) { \
				free(pathw); \
			} \
			SET_ERRNO_FROM_WIN32_CODE(ERROR_ACCESS_DENIED); \
			return ret; \
		} \
} while (0);

PW32IO php_win32_ioutil_normalization_result php_win32_ioutil_normalize_path_w(wchar_t **buf, size_t len, size_t *new_len);
#ifdef PHP_EXPORTS
/* This symbols are needed only for the DllMain, but should not be exported 
	or be available when used with PHP binaries. */
BOOL php_win32_ioutil_init(void);
#endif

/* Keep these functions aliased for case some additional handling
   is needed later. */
__forceinline static wchar_t *php_win32_ioutil_conv_any_to_w(const char* in, size_t in_len, size_t *out_len)
{/*{{{*/
	wchar_t *mb, *ret;
	size_t mb_len;
	
	mb = php_win32_cp_conv_any_to_w(in, in_len, &mb_len);
	if (!mb) {
		return NULL;
	}

	/* Only prefix with long if it's needed. */
	if (mb_len > _MAX_PATH) {
		size_t new_mb_len;

		ret = (wchar_t *) malloc((mb_len + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW + 1) * sizeof(wchar_t));
		if (!ret) {
			free(mb);
			return NULL;
		}

		/* The return can be ignored here, as the normalization can fail for
		   various reasons not directly related to the operation itself.
		   Partial normalization could still do a better job further. And
		   otherwise, the path might be unchanged which is ok if the path
		   was valid long one. */
		(void)php_win32_ioutil_normalize_path_w(&mb, mb_len, &new_mb_len);

		if (new_mb_len > mb_len) {
			wchar_t *tmp = (wchar_t *) realloc(ret, (new_mb_len + 1) * sizeof(wchar_t));
			if (!tmp) {
				free(ret);
				return NULL;
			}
			ret = tmp;
			mb_len = new_mb_len;
		}

		if (PHP_WIN32_IOUTIL_IS_LONG_PATHW(mb, mb_len) || PHP_WIN32_IOUTIL_IS_JUNCTION_PATHW(mb, mb_len) || PHP_WIN32_IOUTIL_IS_UNC_PATHW(mb, mb_len)) {
			memmove(ret, mb, mb_len * sizeof(wchar_t));
			ret[mb_len] = L'\0';
		} else {
			memmove(ret, PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW, PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW * sizeof(wchar_t));
			memmove(ret+PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW, mb, mb_len * sizeof(wchar_t));
			ret[mb_len + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW] = L'\0';

			mb_len += PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
		}

		free(mb);
	} else {
		ret = mb;
	}

	if (PHP_WIN32_CP_IGNORE_LEN_P != out_len) {
		*out_len = mb_len;
	}

	return ret;
}/*}}}*/
#define php_win32_ioutil_any_to_w(in) php_win32_ioutil_conv_any_to_w(in, PHP_WIN32_CP_IGNORE_LEN, PHP_WIN32_CP_IGNORE_LEN_P)

#define php_win32_ioutil_ascii_to_w php_win32_cp_ascii_to_w
#define php_win32_ioutil_utf8_to_w php_win32_cp_utf8_to_w
#define php_win32_ioutil_cur_to_w php_win32_cp_cur_to_w
#define php_win32_ioutil_w_to_any php_win32_cp_w_to_any
#define php_win32_ioutil_conv_w_to_any php_win32_cp_conv_w_to_any
/*__forceinline static char *php_win32_ioutil_w_to_any(wchar_t* w_source_ptr)
{
	return php_win32_cp_w_to_any(w_source_ptr);
}*/
#define php_win32_ioutil_w_to_utf8 php_win32_cp_w_to_utf8
#define php_win32_ioutil_w_to_thread php_win32_cp_w_to_thread

PW32IO int php_win32_ioutil_close(int fd);
PW32IO BOOL php_win32_ioutil_posix_to_open_opts(int flags, mode_t mode, php_ioutil_open_opts *opts);
PW32IO int php_win32_ioutil_mkdir(const char *path, mode_t mode);
PW32IO size_t php_win32_ioutil_dirname(char *buf, size_t len);

PW32IO int php_win32_ioutil_open_w(const wchar_t *path, int flags, ...);
PW32IO int php_win32_ioutil_chdir_w(const wchar_t *path);
PW32IO int php_win32_ioutil_rename_w(const wchar_t *oldname, const wchar_t *newname);
PW32IO wchar_t *php_win32_ioutil_getcwd_w(const wchar_t *buf, int len);

#if 0
PW32IO int php_win32_ioutil_mkdir_w(const wchar_t *path, mode_t mode);
PW32IO int php_win32_ioutil_access_w(const wchar_t *path, mode_t mode);
#endif

#define php_win32_ioutil_access_cond(path, mode) _waccess(pathw, mode)
#define php_win32_ioutil_unlink_cond(path) php_win32_ioutil_unlink_w(pathw)
#define php_win32_ioutil_rmdir_cond(path) php_win32_ioutil_rmdir_w(pathw)

__forceinline static int php_win32_ioutil_access(const char *path, mode_t mode)
{/*{{{*/
	PHP_WIN32_IOUTIL_INIT_W(path)
	int ret, err;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, -1, 1)

	ret = _waccess(pathw, mode);
	if (0 > ret) {
		_get_errno(&err);
	}
	PHP_WIN32_IOUTIL_CLEANUP_W()

	if (0 > ret) {
		_set_errno(err);
	}

	return ret;
}/*}}}*/

__forceinline static int php_win32_ioutil_open(const char *path, int flags, ...)
{/*{{{*/
	mode_t mode = 0;
	PHP_WIN32_IOUTIL_INIT_W(path)
	int ret = -1;
	DWORD err;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, -1, 1)

	if (flags & O_CREAT) {
		va_list arg;

		va_start(arg, flags);
		mode = (mode_t) va_arg(arg, int);
		va_end(arg);
	}

	ret = php_win32_ioutil_open_w(pathw, flags, mode);
	if (0 > ret) {
		err = GetLastError();
	}
	PHP_WIN32_IOUTIL_CLEANUP_W()

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

__forceinline static int php_win32_ioutil_unlink(const char *path)
{/*{{{*/
	PHP_WIN32_IOUTIL_INIT_W(path)
	int ret = 0;
	DWORD err = 0;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, -1, 1)

	if (!DeleteFileW(pathw)) {
		err = GetLastError();
		ret = -1;
	}
	PHP_WIN32_IOUTIL_CLEANUP_W()

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

__forceinline static int php_win32_ioutil_rmdir(const char *path)
{/*{{{*/
	PHP_WIN32_IOUTIL_INIT_W(path)
	int ret = 0;
	DWORD err = 0;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, -1, 1)

	if (!RemoveDirectoryW(pathw)) {
		err = GetLastError();
		ret = -1;
	}

	PHP_WIN32_IOUTIL_CLEANUP_W()

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

/* This needs to be improved once long path support is implemented. Use ioutil_open() and then
fdopen() might be the way, if we learn how to convert the mode options (maybe grab the routine
 from the streams). That will allow to split for _a and _w. */
__forceinline static FILE *php_win32_ioutil_fopen(const char *patha, const char *modea)
{/*{{{*/
	FILE *ret;
	wchar_t *pathw;
	wchar_t *modew;
	int err = 0;

	pathw = php_win32_ioutil_any_to_w(patha);
	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, NULL, 1)

	modew = php_win32_ioutil_ascii_to_w(modea);
	if (!modew) {
		free(pathw);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	ret = _wfopen(pathw, modew);
	if (!ret) {
		_get_errno(&err);
	}
	free(pathw);
	free(modew);

	if (!ret) {
		_set_errno(err);
	}
	return ret;
}/*}}}*/

__forceinline static int php_win32_ioutil_rename(const char *oldnamea, const char *newnamea)
{/*{{{*/
	wchar_t *oldnamew;
	wchar_t *newnamew;
	int ret;
	DWORD err = 0;

	oldnamew = php_win32_ioutil_any_to_w(oldnamea);
	if (!oldnamew) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}
	PHP_WIN32_IOUTIL_CHECK_PATH_W(oldnamew, -1, 1)

	newnamew = php_win32_ioutil_any_to_w(newnamea);
	if (!newnamew) {
		free(oldnamew);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	} else if (!PHP_WIN32_IOUTIL_PATH_IS_OK_W(newnamew, wcslen(newnamew))) {
		free(oldnamew);
		free(newnamew);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_ACCESS_DENIED);
		return -1;
	}

	ret = php_win32_ioutil_rename_w(oldnamew, newnamew);
	if (0 > ret) {
		err = GetLastError();
	}

	free(oldnamew);
	free(newnamew);

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

__forceinline static int php_win32_ioutil_chdir(const char *patha)
{/*{{{*/
	int ret;
	wchar_t *pathw = php_win32_ioutil_any_to_w(patha);
	DWORD err = 0;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	ret = php_win32_ioutil_chdir_w(pathw);
	if (0 > ret) {
		err = GetLastError();
	}

	free(pathw);

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

__forceinline static char *php_win32_ioutil_getcwd(char *buf, int len)
{/*{{{*/
	wchar_t tmp_bufw[PHP_WIN32_IOUTIL_MAXPATHLEN];
	char *tmp_bufa = NULL;
	size_t tmp_bufa_len;
	DWORD err = 0;

	if (php_win32_ioutil_getcwd_w(tmp_bufw, PHP_WIN32_IOUTIL_MAXPATHLEN) == NULL) {
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	tmp_bufa = php_win32_cp_conv_w_to_any(tmp_bufw, wcslen(tmp_bufw), &tmp_bufa_len);
	if (!tmp_bufa) {
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	} else if (tmp_bufa_len + 1 > PHP_WIN32_IOUTIL_MAXPATHLEN) {
		free(tmp_bufa);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_BAD_LENGTH);
		return NULL;
	}

	if (!buf) {
		/* If buf was NULL, the result has to be freed outside here. */
		buf = tmp_bufa;
	} else {
		if (tmp_bufa_len + 1 > len) {
			free(tmp_bufa);
			SET_ERRNO_FROM_WIN32_CODE(ERROR_INSUFFICIENT_BUFFER);
			return NULL;
		}
		memmove(buf, tmp_bufa, tmp_bufa_len + 1);
		free(tmp_bufa);
	}

	return buf;
}/*}}}*/

/* TODO improve with usage of native APIs, split for _a and _w. */
__forceinline static int php_win32_ioutil_chmod(const char *patha, int mode)
{/*{{{*/
	wchar_t *pathw = php_win32_ioutil_any_to_w(patha);
	int err = 0;
	int ret;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, -1, 1)

	ret = _wchmod(pathw, mode);
	if (0 > ret) {
		_get_errno(&err);
	}

	free(pathw);

	if (0 > ret) {
		_set_errno(err);
	}

	return ret;
}/*}}}*/

#ifdef __cplusplus
}
#endif

#endif /* PHP_WIN32_IOUTIL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
