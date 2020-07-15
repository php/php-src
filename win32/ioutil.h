/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

/* these are not defined in win32 headers */
#ifndef W_OK
#define W_OK 0x02
#endif
#ifndef R_OK
#define R_OK 0x04
#endif
#ifndef X_OK
#define X_OK 0x01
#endif
#ifndef F_OK
#define F_OK 0x00
#endif

/* from ntifs.h */
#ifndef SYMLINK_FLAG_RELATIVE
#define SYMLINK_FLAG_RELATIVE 0x01
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
	|| path_lenw >= 3 && PHP_WIN32_IOUTIL_IS_LETTERW(pathw[0]) && L':' == pathw[1] && PHP_WIN32_IOUTIL_IS_SLASHW(pathw[2]))
#define PHP_WIN32_IOUTIL_IS_UNC(pathw, path_lenw) (path_lenw >= 2 && PHP_WIN32_IOUTIL_IS_SLASHW(pathw[0]) && PHP_WIN32_IOUTIL_IS_SLASHW(pathw[1]) \
	|| path_lenw >= PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW && 0 == wcsncmp((pathw), PHP_WIN32_IOUTIL_UNC_PATH_PREFIXW, PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW))

#define PHP_WIN32_IOUTIL_DEFAULT_SHARE_MODE (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE)

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
		size_t _len = wcslen(pathw); \
		if (!PHP_WIN32_IOUTIL_PATH_IS_OK_W(pathw, _len)) { \
			if (dealloc) { \
				free((void *)pathw); \
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
	if (mb_len >= _MAX_PATH) {
		size_t new_mb_len;

		ret = (wchar_t *) malloc((mb_len + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW + 1) * sizeof(wchar_t));
		if (!ret) {
			free(mb);
			return NULL;
		}

		if (PHP_WIN32_IOUTIL_NORM_FAIL == php_win32_ioutil_normalize_path_w(&mb, mb_len, &new_mb_len)) {
				free(ret);
				free(mb);
				return NULL;
		}

		if (new_mb_len > mb_len) {
			wchar_t *tmp = (wchar_t *) realloc(ret, (new_mb_len + 1) * sizeof(wchar_t));
			if (!tmp) {
				free(ret);
				free(mb);
				return NULL;
			}
			ret = tmp;
			mb_len = new_mb_len;
		}

		if (PHP_WIN32_IOUTIL_IS_LONG_PATHW(mb, mb_len) || PHP_WIN32_IOUTIL_IS_JUNCTION_PATHW(mb, mb_len) || PHP_WIN32_IOUTIL_IS_UNC_PATHW(mb, mb_len)) {
			memmove(ret, mb, mb_len * sizeof(wchar_t));
			ret[mb_len] = L'\0';
		} else {
			wchar_t *src = mb, *dst = ret + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
			memmove(ret, PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW, PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW * sizeof(wchar_t));
			while (src < mb + mb_len) {
				if (*src == PHP_WIN32_IOUTIL_FW_SLASHW) {
					*dst++ = PHP_WIN32_IOUTIL_DEFAULT_SLASHW;
					src++;
				} else {
					*dst++ = *src++;
				}
			}
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
PW32IO size_t php_win32_ioutil_dirname(char *buf, size_t len);

PW32IO int php_win32_ioutil_open_w(const wchar_t *path, int flags, ...);
PW32IO int php_win32_ioutil_chdir_w(const wchar_t *path);
PW32IO int php_win32_ioutil_rename_w(const wchar_t *oldname, const wchar_t *newname);
PW32IO wchar_t *php_win32_ioutil_getcwd_w(wchar_t *buf, size_t len);
PW32IO int php_win32_ioutil_unlink_w(const wchar_t *path);
PW32IO int php_win32_ioutil_access_w(const wchar_t *path, mode_t mode);
PW32IO int php_win32_ioutil_mkdir_w(const wchar_t *path, mode_t mode);
PW32IO FILE *php_win32_ioutil_fopen_w(const wchar_t *path, const wchar_t *mode);
PW32IO wchar_t *php_win32_ioutil_realpath_w(const wchar_t *path, wchar_t *resolved);
PW32IO wchar_t *php_win32_ioutil_realpath_w_ex0(const wchar_t *path, wchar_t *resolved, PBY_HANDLE_FILE_INFORMATION info);
PW32IO int php_win32_ioutil_symlink_w(const wchar_t *target, const wchar_t *link);
PW32IO int php_win32_ioutil_link_w(const wchar_t *target, const wchar_t *link);

__forceinline static int php_win32_ioutil_access(const char *path, mode_t mode)
{/*{{{*/
	PHP_WIN32_IOUTIL_INIT_W(path)
	int ret, err;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, -1, 1)

	ret = php_win32_ioutil_access_w(pathw, mode);
	if (0 > ret) {
		err = GetLastError();
	}
	PHP_WIN32_IOUTIL_CLEANUP_W()

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
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
	int ret = -1;
	DWORD err;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	ret = php_win32_ioutil_unlink_w(pathw);
	if (0 > ret) {
		err = GetLastError();
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

__forceinline static FILE *php_win32_ioutil_fopen(const char *patha, const char *modea)
{/*{{{*/
	FILE *ret;
	wchar_t modew[16] = {0};
	int i = 0;

	PHP_WIN32_IOUTIL_INIT_W(patha)
	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, NULL, 1)

	while (i < (sizeof(modew)-1)/sizeof(wchar_t) && modea[i]) {
		modew[i] = (wchar_t)modea[i];
		i++;
	}

	ret = php_win32_ioutil_fopen_w(pathw, modew);
	if (!ret) {
		int err = GetLastError();
		PHP_WIN32_IOUTIL_CLEANUP_W()
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	PHP_WIN32_IOUTIL_CLEANUP_W()

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
	} else {
		size_t newnamew_len = wcslen(newnamew);
		if (!PHP_WIN32_IOUTIL_PATH_IS_OK_W(newnamew, newnamew_len)) {
			free(oldnamew);
			free(newnamew);
			SET_ERRNO_FROM_WIN32_CODE(ERROR_ACCESS_DENIED);
			return -1;
		}
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

__forceinline static char *php_win32_ioutil_getcwd(char *buf, size_t len)
{/*{{{*/
	wchar_t tmp_bufw[PHP_WIN32_IOUTIL_MAXPATHLEN];
	char *tmp_bufa = NULL;
	size_t tmp_bufa_len;
	DWORD err = 0;

	if (php_win32_ioutil_getcwd_w(tmp_bufw, len ? len : PHP_WIN32_IOUTIL_MAXPATHLEN) == NULL) {
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
	} else if (tmp_bufa_len + 1 > len) {
		free(tmp_bufa);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INSUFFICIENT_BUFFER);
		return NULL;
	}

	if (!buf && !len) {
		/* If buf was NULL, the result has to be freed outside here. */
		buf = tmp_bufa;
	} else {
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

__forceinline static int php_win32_ioutil_mkdir(const char *path, mode_t mode)
{/*{{{*/
	int ret;
	DWORD err = 0;

	PHP_WIN32_IOUTIL_INIT_W(path)
	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	ret = php_win32_ioutil_mkdir_w(pathw, mode);
	if (0 > ret) {
		err = GetLastError();
	}

	PHP_WIN32_IOUTIL_CLEANUP_W()

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

__forceinline static int php_win32_ioutil_symlink(const char *target, const char *link)
{/*{{{*/
	wchar_t *targetw, *linkw;
	int ret;

	targetw = php_win32_ioutil_any_to_w(target);
	if (!targetw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	linkw = php_win32_ioutil_any_to_w(link);
	if (!linkw) {
		free(targetw);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	ret = php_win32_ioutil_symlink_w(targetw, linkw);

	free(targetw);
	free(linkw);

	return ret;
}/*}}}*/

__forceinline static int php_win32_ioutil_link(const char *target, const char *link)
{/*{{{*/
	wchar_t *targetw, *linkw;
	int ret;

	targetw = php_win32_ioutil_any_to_w(target);
	if (!targetw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}
	linkw = php_win32_ioutil_any_to_w(link);
	if (!linkw) {
		free(targetw);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	ret = php_win32_ioutil_link_w(targetw, linkw);

	free(targetw);
	free(linkw);

	return ret;
}/*}}}*/

#define HAVE_REALPATH 1
PW32IO char *realpath(const char *path, char *resolved);

__forceinline static char *php_win32_ioutil_realpath_ex0(const char *path, char *resolved, PBY_HANDLE_FILE_INFORMATION info)
{/*{{{*/
	wchar_t retw[PHP_WIN32_IOUTIL_MAXPATHLEN];
	char *reta;
	size_t reta_len;

	PHP_WIN32_IOUTIL_INIT_W(path)
	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	if (NULL == php_win32_ioutil_realpath_w_ex0(pathw, retw, info)) {
		DWORD err = GetLastError();
		PHP_WIN32_IOUTIL_CLEANUP_W()
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	reta = php_win32_cp_conv_w_to_any(retw, PHP_WIN32_CP_IGNORE_LEN, &reta_len);
	if (!reta || reta_len > PHP_WIN32_IOUTIL_MAXPATHLEN) {
		DWORD err = GetLastError();
		PHP_WIN32_IOUTIL_CLEANUP_W()
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	if (NULL == resolved) {
		/* ret is expected to be either NULL or a buffer of capable size. */
		resolved = (char *) malloc(reta_len + 1);
		if (!resolved) {
			free(reta);
			PHP_WIN32_IOUTIL_CLEANUP_W()
			SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
			return NULL;
		}
	}
	memmove(resolved, reta, reta_len+1);

	PHP_WIN32_IOUTIL_CLEANUP_W()
	free(reta);

	return resolved;
}/*}}}*/

__forceinline static char *php_win32_ioutil_realpath(const char *path, char *resolved)
{/*{{{*/
	return php_win32_ioutil_realpath_ex0(path, resolved, NULL);
}/*}}}*/

#include <sys/stat.h>
#if _WIN64
typedef unsigned __int64 php_win32_ioutil_dev_t;
typedef unsigned __int64 php_win32_ioutil_ino_t;
typedef __time64_t php_win32_ioutil_time_t;
typedef __int64 php_win32_ioutil_size_t;
#else
typedef unsigned __int32 php_win32_ioutil_dev_t;
typedef unsigned __int32 php_win32_ioutil_ino_t;
typedef __time32_t php_win32_ioutil_time_t;
typedef __int32 php_win32_ioutil_size_t;
#endif
typedef struct {
	php_win32_ioutil_dev_t st_dev;
	php_win32_ioutil_ino_t st_ino;
	unsigned __int32 st_mode;
	unsigned __int32 st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	php_win32_ioutil_dev_t st_rdev;
	php_win32_ioutil_size_t st_size;
#if 0
	__int32 st_blksize;
	__int32 st_blocks;
#endif
	php_win32_ioutil_time_t st_atime;
	php_win32_ioutil_time_t st_mtime;
	php_win32_ioutil_time_t st_ctime;
} php_win32_ioutil_stat_t;

typedef struct {
	unsigned long  ReparseTag;
	unsigned short ReparseDataLength;
	unsigned short Reserved;
	union {
		struct {
			unsigned short SubstituteNameOffset;
			unsigned short SubstituteNameLength;
			unsigned short PrintNameOffset;
			unsigned short PrintNameLength;
			unsigned long  Flags;
			wchar_t        ReparseTarget[1];
		} SymbolicLinkReparseBuffer;
		struct {
			unsigned short SubstituteNameOffset;
			unsigned short SubstituteNameLength;
			unsigned short PrintNameOffset;
			unsigned short PrintNameLength;
			wchar_t        ReparseTarget[1];
		} MountPointReparseBuffer;
		struct {
			unsigned char  ReparseTarget[1];
		} GenericReparseBuffer;
	};
} PHP_WIN32_IOUTIL_REPARSE_DATA_BUFFER, *PHP_WIN32_IOUTIL_PREPARSE_DATA_BUFFER;

PW32IO int php_win32_ioutil_stat_ex_w(const wchar_t *path, size_t path_len, php_win32_ioutil_stat_t *buf, int lstat);
PW32IO int php_win32_ioutil_fstat(int fd, php_win32_ioutil_stat_t *buf);

__forceinline static int php_win32_ioutil_stat_ex(const char *path, php_win32_ioutil_stat_t *buf, int lstat)
{/*{{{*/
	size_t pathw_len;
	wchar_t *pathw = php_win32_ioutil_conv_any_to_w(path, PHP_WIN32_CP_IGNORE_LEN, &pathw_len);
	int ret;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	ret = php_win32_ioutil_stat_ex_w(pathw, pathw_len, buf, lstat);

	free(pathw);

	return ret;
}/*}}}*/
#define php_win32_ioutil_stat(path, buf) php_win32_ioutil_stat_ex(path, buf, 0)
#define php_win32_ioutil_lstat(path, buf) php_win32_ioutil_stat_ex(path, buf, 1)

PW32IO ssize_t php_win32_ioutil_readlink_w(const wchar_t *path, wchar_t *buf, size_t buf_len);

__forceinline static ssize_t php_win32_ioutil_readlink(const char *path, char *buf, size_t buf_len)
{/*{{{*/
	size_t pathw_len, ret_buf_len;
	wchar_t *pathw = php_win32_ioutil_conv_any_to_w(path, PHP_WIN32_CP_IGNORE_LEN, &pathw_len);
	wchar_t retw[PHP_WIN32_IOUTIL_MAXPATHLEN];
	char *ret_buf;
	ssize_t ret;

	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	ret = php_win32_ioutil_readlink_w(pathw, retw, sizeof(retw)-1);
	if (ret < 0) {
		DWORD _err = GetLastError();
		free(pathw);
		SET_ERRNO_FROM_WIN32_CODE(_err);
		return ret;
	}

	ret_buf = php_win32_ioutil_conv_w_to_any(retw, ret, &ret_buf_len);
	if (!ret_buf || ret_buf_len >= buf_len || ret_buf_len >= MAXPATHLEN) {
		free(ret_buf);
		free(pathw);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_BAD_PATHNAME);
		return -1;
	}
	memcpy(buf, ret_buf, ret_buf_len + 1);

	free(ret_buf);
	free(pathw);

	return ret_buf_len;
}/*}}}*/

#ifdef __cplusplus
}
#endif

#endif /* PHP_WIN32_IOUTIL_H */
