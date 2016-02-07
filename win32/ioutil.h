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

#include "win32/winutil.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defining to 1 will force the APIs old behaviors as a fallback. */
#ifndef PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
#define PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE 1
#endif

#ifdef PHP_EXPORTS
# define PW32IO __declspec(dllexport)
#else
# define PW32IO __declspec(dllimport)
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

PW32IO wchar_t *php_win32_ioutil_mb_to_w(const char* path);
PW32IO char *php_win32_ioutil_w_to_utf8(wchar_t* w_source_ptr);
/* This function tries to make the best guess to convert any
   given string to a wide char, also prefering the fastest code
   path to unicode. It returns NULL on fail. */
__forceinline wchar_t *php_win32_ioutil_any_to_w(const char* in)
{
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	const char *idx = in, *end = in + strlen(in);

	while (idx != end) {
		if (!__isascii(*idx)) {
			break;
		}
		idx++;
	}

	/* This means we've got an ASCII path, an ANSI function can be used
		safely. This is the fastest way to do the thing. */
	if (idx == end) {
		return NULL;
	}
#endif
	/* Otherwise, go try to convert to multibyte. If it is failed, still
	   NULL is delivered, indicating an ANSI IO function should be used.
	   That still might not work with every ANSI string when the current
	   system locale is incompatible. Then, it is up to the high level to
	   convert the path to a multibyte string before. */
	return php_win32_ioutil_mb_to_w(in);
}

PW32IO int php_win32_ioutil_close(int fd);
PW32IO BOOL php_win32_ioutil_posix_to_open_opts(int flags, mode_t mode, php_ioutil_open_opts *opts);
PW32IO int php_win32_ioutil_mkdir(const char *path, mode_t mode);

#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
PW32IO int php_win32_ioutil_mkdir_a(const char *path, mode_t mode);
PW32IO int php_win32_ioutil_open_a(const char *path, int flags, ...);
PW32IO int php_win32_ioutil_chdir_a(const char *path);
PW32IO int php_win32_ioutil_rename_a(const char *oldname, const char *newname);
PW32IO char *php_win32_ioutil_getcwd_a(const char *buf, int len);
#endif

PW32IO int php_win32_ioutil_mkdir_w(const wchar_t *path, mode_t mode);
PW32IO int php_win32_ioutil_open_w(const wchar_t *path, int flags, ...);
PW32IO int php_win32_ioutil_chdir_w(const wchar_t *path);
PW32IO int php_win32_ioutil_rename_w(const wchar_t *oldname, const wchar_t *newname);
PW32IO wchar_t *php_win32_ioutil_getcwd_w(const wchar_t *buf, int len);

#if 0
PW32IO int php_win32_ioutil_access_a(const char *path, mode_t mode);
PW32IO int php_win32_ioutil_access_w(const wchar_t *path, mode_t mode);
PW32IO int php_win32_ioutil_open(const char *path, int flags, ...);
#endif

/* A boolean use_w has to be defined for this to work. */
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
#define PHP_WIN32_IOUTIL_INIT_W(path) \
	const char *patha = path; \
	wchar_t *pathw = php_win32_ioutil_any_to_w(path); \
	BOOL use_w = (NULL != pathw);

#define PHP_WIN32_IOUTIL_CLEANUP_W() \
	if (pathw) { \
		free(pathw); \
	}

#define php_win32_ioutil_access_cond(path, mode) (use_w) ? _waccess(pathw, mode) : _access(patha, mode) 
#define php_win32_ioutil_unlink_cond(path) (use_w) ? php_win32_ioutil_unlink_w(path) : php_win32_ioutil_unlink_a(path);
#define php_win32_ioutil_rmdir_cond(path) (use_w) ? php_win32_ioutil_rmdir_w(path) : php_win32_ioutil_rmdir_a(path);

#else

#define PHP_WIN32_IOUTIL_INIT_W(path) \
	wchar_t *pathw = php_win32_ioutil_any_to_w(path); \
	BOOL use_w = 1;

#define PHP_WIN32_IOUTIL_CLEANUP_W() \
	if (pathw) { \
		free(pathw); \
	}

#define php_win32_ioutil_access_cond(path, mode) _waccess(pathw, mode)
#define php_win32_ioutil_unlink_cond(path) php_win32_ioutil_unlink_w(pathw)
#define php_win32_ioutil_rmdir_cond(path) php_win32_ioutil_rmdir_w(pathw)

#endif

/* #if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE */
__forceinline static int php_win32_ioutil_access(const char *path, mode_t mode)
{
	PHP_WIN32_IOUTIL_INIT_W(path);
	int ret;

	if (use_w) {
		ret = _waccess(pathw, mode);
		PHP_WIN32_IOUTIL_CLEANUP_W();
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		ret = _access(patha, mode);
#endif
	}

	return ret;
}

__forceinline static int php_win32_ioutil_open(const char *path, int flags, ...)
{
	mode_t mode = 0;
	PHP_WIN32_IOUTIL_INIT_W(path);
	int ret = -1;

	if (flags & O_CREAT) {
		va_list arg;

		va_start(arg, flags);
		mode = (mode_t) va_arg(arg, int);
		va_end(arg);
	}

	if (use_w) {
		ret = php_win32_ioutil_open_w(pathw, flags, mode);
		PHP_WIN32_IOUTIL_CLEANUP_W();
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		ret = php_win32_ioutil_open_a(patha, flags, mode);
#endif
	}

	if (0 > ret) {
		DWORD err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}

__forceinline static int php_win32_ioutil_unlink(const char *path)
{
	PHP_WIN32_IOUTIL_INIT_W(path);
	int ret = 0;
	DWORD err = 0;

	if (use_w) {
		if (!DeleteFileW(pathw)) {
			err = GetLastError();
			ret = -1;
		}
		PHP_WIN32_IOUTIL_CLEANUP_W();
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		if (!DeleteFileA(patha)) {
			err = GetLastError();
			ret = -1;
		}
#endif
	}

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}

__forceinline static int php_win32_ioutil_rmdir(const char *path)
{
	PHP_WIN32_IOUTIL_INIT_W(path);
	int ret = 0;
	DWORD err = 0;

	if (use_w) {
		if (!RemoveDirectoryW(pathw)) {
			err = GetLastError();
			ret = -1;
		}
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		if (!RemoveDirectoryA(patha)) {
			err = GetLastError();
			ret = -1;
		}
#endif
	}

	PHP_WIN32_IOUTIL_CLEANUP_W();

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}

/* This needs to be improved once long path support is implemented. Use ioutil_open() and then
fdopen() might be the way, if we learn how to convert the mode options (maybe grab the routine
 from the streams). That will allow to split for _a and _w. */
__forceinline static FILE *php_win32_ioutil_fopen(const char *patha, const char *modea)
{
	FILE *ret;
	wchar_t *pathw = php_win32_ioutil_any_to_w(patha);
	wchar_t *modew = php_win32_ioutil_mb_to_w(modea);
	int err = 0;

	if (pathw && modew) {
		ret = _wfopen(pathw, modew);
		_get_errno(&err);
		free(pathw);
		free(modew);
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		ret = fopen(patha, modea);
		_get_errno(&err);
#endif
	}

	if (0 > ret) {
		_set_errno(err);
	}
	return ret;
}

__forceinline static int php_win32_ioutil_rename(const char *oldnamea, const char *newnamea)
{
	wchar_t *oldnamew = php_win32_ioutil_any_to_w(oldnamea);
	wchar_t *newnamew = php_win32_ioutil_any_to_w(newnamea);
	int ret;
	DWORD err = 0;

	if (oldnamew && newnamew) {
		ret = php_win32_ioutil_rename_w(oldnamew, newnamew);
		err = GetLastError();
		free(oldnamew);
		free(newnamew);
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		ret = php_win32_ioutil_rename_a(oldnamea, newnamea);
		err = GetLastError();
#endif
	}

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}

__forceinline static int php_win32_ioutil_chdir(const char *patha)
{
	int ret;
	wchar_t *pathw = php_win32_ioutil_any_to_w(patha);
	DWORD err = 0;

	if (pathw) {
		ret = php_win32_ioutil_chdir_w(pathw);
		err = GetLastError();
		free(pathw);
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		ret = php_win32_ioutil_chdir_a(patha);
		err = GetLastError();
#endif
	}

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}

__forceinline static char *php_win32_ioutil_getcwd(char *buf, int len)
{
	wchar_t *tmp_bufw = NULL;
	char *tmp_bufa = NULL;
	DWORD err = 0;

	tmp_bufw = php_win32_ioutil_getcwd_w(tmp_bufw, len);
	if (!tmp_bufw) {
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	tmp_bufa = php_win32_ioutil_w_to_utf8(tmp_bufw);
	if (!tmp_bufa) {
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
		buf = php_win32_ioutil_getcwd_a(buf, len);
#else
		buf = NULL;
#endif
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return buf;
	} else if (strlen(tmp_bufa) > len) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INSUFFICIENT_BUFFER);
		return NULL;
	}

	if (!buf) {
		/* If buf was NULL, the result has to be freed outside here. */
		buf = tmp_bufa;
	} else {
		memmove(buf, tmp_bufa, len);
		free(tmp_bufa);
	}

	free(tmp_bufw);

	return buf;
}

/* TODO improve with usage of native APIs, split for _a and _w. */
__forceinline static int php_win32_ioutil_chmod(const char *patha, int mode)
{
	wchar_t *pathw = php_win32_ioutil_any_to_w(patha);
	int err = 0;
	int ret;

	if (pathw) {
		ret = _wchmod(pathw, mode);
		_get_errno(&err);
		free(pathw);
#if PHP_WIN32_IOUTIL_ANSI_COMPAT_MODE
	} else {
		ret = _chmod(patha, mode);
		_get_errno(&err);
#endif
	}

	if (0 > ret) {
		_set_errno(err);
	}
	return ret;
}

//#else /* no ANSI compat mode */
/*#define php_win32_ioutil_access _waccess
#define php_win32_ioutil_sys_stat_ex php_win32_ioutil_sys_stat_ex_w
#define php_win32_ioutil_open php_win32_ioutil_open_w
#define php_win32_ioutil_unlink_cond php_win32_ioutil_unlink_w
#define php_win32_ioutil_unlink php_win32_ioutil_unlink_w
#define php_win32_ioutil_rmdir_cond php_win32_ioutil_rmdir_w
#define php_win32_ioutil_rmdir php_win32_ioutil_rmdir_w
#define php_win32_ioutil_fopen _wfopen
#define php_win32_ioutil_rename php_win32_ioutil_rename_w
#define php_win32_ioutil_chdir php_win32_ioutil_chdir_w
#define php_win32_ioutil_chmod _wchmod
#define php_win32_ioutil_getcwd php_win32_ioutil_getcwd_w
#endif*/


#if 0
zend_always_inline static HANDLE php_win32_ioutil_get_file_handle(char *path, WIN32_FIND_DATA *data)
{
		HANDLE ret = FindFirstFileA(path, data);

}
#endif

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
