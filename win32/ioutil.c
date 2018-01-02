/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

#include <assert.h>
#include <stdlib.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <stdio.h>

#include "php.h"
#include "SAPI.h"
#include "win32/winutil.h"
#include "win32/time.h"
#include "win32/ioutil.h"
#include "win32/codepage.h"

#include <pathcch.h>

/*
#undef NONLS
#undef _WINNLS_
#include <winnls.h>
*/

typedef HRESULT (__stdcall *MyPathCchCanonicalizeEx)(wchar_t *pszPathOut, size_t cchPathOut, const wchar_t *pszPathIn, unsigned long dwFlags);

static MyPathCchCanonicalizeEx canonicalize_path_w = NULL;

PW32IO BOOL php_win32_ioutil_posix_to_open_opts(int flags, mode_t mode, php_ioutil_open_opts *opts)
{/*{{{*/
	int current_umask;

	opts->attributes = 0;

	/* Obtain the active umask. umask() never fails and returns the previous */
	/* umask. */
	current_umask = umask(0);
	umask(current_umask);

	/* convert flags and mode to CreateFile parameters */
	switch (flags & (_O_RDONLY | _O_WRONLY | _O_RDWR)) {
		case _O_RDONLY:
			opts->access = FILE_GENERIC_READ;
			/* XXX not opening dirs yet, see also at the bottom of this function. Should be evaluated properly. */
			/*opts->attributes |= FILE_FLAG_BACKUP_SEMANTICS;*/
			break;
		case _O_WRONLY:
			opts->access = FILE_GENERIC_WRITE;
			break;
		case _O_RDWR:
			opts->access = FILE_GENERIC_READ | FILE_GENERIC_WRITE;
			break;
		default:
			goto einval;
	}

	if (flags & _O_APPEND) {
		/* XXX this might look wrong, but i just leave it here. Disabling FILE_WRITE_DATA prevents the current truncate behaviors for files opened with "a". */
		/* access &= ~FILE_WRITE_DATA;*/
		opts->access |= FILE_APPEND_DATA;
		opts->attributes &= ~FILE_FLAG_BACKUP_SEMANTICS;
	}

	/*
	* Here is where we deviate significantly from what CRT's _open()
	* does. We indiscriminately use all the sharing modes, to match
	* UNIX semantics. In particular, this ensures that the file can
	* be deleted even whilst it's open.
	*/
	/* opts->share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE; */
	/* XXX No UINX behavior  Good to know it's doable. 
	   Not being done as this means a behavior change. Should be evaluated properly. */
	opts->share = FILE_SHARE_READ | FILE_SHARE_WRITE;

	switch (flags & (_O_CREAT | _O_EXCL | _O_TRUNC)) {
		case 0:
		case _O_EXCL:
			opts->disposition = OPEN_EXISTING;
			break;
		case _O_CREAT:
			opts->disposition = OPEN_ALWAYS;
			break;
		case _O_CREAT | _O_EXCL:
		case _O_CREAT | _O_TRUNC | _O_EXCL:
			opts->disposition = CREATE_NEW;
			break;
		case _O_TRUNC:
		case _O_TRUNC | _O_EXCL:
			opts->disposition = TRUNCATE_EXISTING;
			break;
		case _O_CREAT | _O_TRUNC:
			opts->disposition = CREATE_ALWAYS;
			break;
		default:
			goto einval;
	}

	opts->attributes |= FILE_ATTRIBUTE_NORMAL;
	if (flags & _O_CREAT) {
		if (!((mode & ~current_umask) & _S_IWRITE)) {
			opts->attributes |= FILE_ATTRIBUTE_READONLY;
		}
	}

	if (flags & _O_TEMPORARY ) {
		opts->attributes |= FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY;
		opts->access |= DELETE;
	}

	if (flags & _O_SHORT_LIVED) {
		opts->attributes |= FILE_ATTRIBUTE_TEMPORARY;
	}

	switch (flags & (_O_SEQUENTIAL | _O_RANDOM)) {
		case 0:
			break;
		case _O_SEQUENTIAL:
			opts->attributes |= FILE_FLAG_SEQUENTIAL_SCAN;
			break;
		case _O_RANDOM:
			opts->attributes |= FILE_FLAG_RANDOM_ACCESS;
			break;
		default:
			goto einval;
	}

	/* Very compat options */
	/*if (flags & O_ASYNC) {
		opts->attributes |= FILE_FLAG_OVERLAPPED;
	} else if (flags & O_SYNC) {
		opts->attributes &= ~FILE_FLAG_OVERLAPPED;
	}*/

	/* Setting this flag makes it possible to open a directory. */
	/* XXX not being done as this means a behavior change. Should be evaluated properly. */
	/* opts->attributes |= FILE_FLAG_BACKUP_SEMANTICS; */

	return 1;

einval:
	_set_errno(EINVAL);
	return 0;
}/*}}}*/

PW32IO int php_win32_ioutil_open_w(const wchar_t *path, int flags, ...)
{/*{{{*/
	php_ioutil_open_opts open_opts;
	HANDLE file;
	int fd;
	mode_t mode = 0;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, -1, 0)

	if (flags & O_CREAT) {
		va_list arg;

		va_start(arg, flags);
		mode = (mode_t) va_arg(arg, int);
		va_end(arg);
	}

	if (!php_win32_ioutil_posix_to_open_opts(flags, mode, &open_opts)) {
		goto einval;
	}

	/* XXX care about security attributes here if needed, see tsrm_win32_access() */
	file = CreateFileW(path,
		open_opts.access,
		open_opts.share,
		NULL,
		open_opts.disposition,
		open_opts.attributes,
		NULL);

	if (file == INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();

		if (error == ERROR_FILE_EXISTS && (flags & _O_CREAT) &&
			!(flags & _O_EXCL)) {
			/* Special case: when ERROR_FILE_EXISTS happens and O_CREAT was */
			/* specified, it means the path referred to a directory. */
			_set_errno(EISDIR);
		} else {
			SET_ERRNO_FROM_WIN32_CODE(error);
		}
		return -1;
	}

	fd = _open_osfhandle((intptr_t) file, flags);
	if (fd < 0) {
		DWORD error = GetLastError();

		/* The only known failure mode for _open_osfhandle() is EMFILE, in which
		 * case GetLastError() will return zero. However we'll try to handle other
		 * errors as well, should they ever occur.
		 */
		if (errno == EMFILE) {
			_set_errno(EMFILE);
		} else if (error != ERROR_SUCCESS) {
			SET_ERRNO_FROM_WIN32_CODE(error);
		}
		CloseHandle(file);
		return -1;
	}

	if (flags & _O_TEXT) {
		_setmode(fd, _O_TEXT);
	} else if (flags & _O_BINARY) {
		_setmode(fd, _O_BINARY);
	}

	return fd;

	einval:
		_set_errno(EINVAL);
		return -1;
}/*}}}*/

PW32IO int php_win32_ioutil_close(int fd)
{/*{{{*/
	int result = -1;

	if (-1 == fd) {
		_set_errno(EBADF);
		return result;
	}

	if (fd > 2) {
		result = _close(fd);
	} else {
		result = 0;
	}

	/* _close doesn't set _doserrno on failure, but it does always set errno
	* to EBADF on failure.
	*/
	if (result == -1) {
		_set_errno(EBADF);
	}

	return result;
}/*}}}*/

#if 0
PW32IO int php_win32_ioutil_mkdir_w(const wchar_t *path, mode_t mode)
{/*{{{*/
	int ret = 0;
	DWORD err = 0;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, -1, 0)

	/* TODO extend with mode usage */
	if (!CreateDirectoryW(path, NULL)) {
		err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/
#endif

PW32IO int php_win32_ioutil_mkdir(const char *path, mode_t mode)
{/*{{{*/
	size_t pathw_len = 0;
	wchar_t *pathw = php_win32_ioutil_conv_any_to_w(path, 0, &pathw_len);
	int ret = 0;
	DWORD err = 0;

	if (pathw_len < _MAX_PATH && pathw_len > _MAX_PATH - 12) {
		/* Special case here. From the doc:

		 "When using an API to create a directory, the specified path cannot be
		 so long that you cannot append an 8.3 file name ..."

		 Thus, if the directory name length happens to be in this range, it
		 already needs to be a long path. The given path is already normalized
		 and prepared, need only to prefix it.
		 */
		wchar_t *tmp = (wchar_t *) malloc((pathw_len + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW + 1) * sizeof(wchar_t));
		if (!tmp) {
			free(pathw);
			SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
			return -1;
		}

		memmove(tmp, PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW, PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW * sizeof(wchar_t));
		memmove(tmp+PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW, pathw, pathw_len * sizeof(wchar_t));
		pathw_len += PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
		tmp[pathw_len] = L'\0';

		free(pathw);
		pathw = tmp;
	}

	/* TODO extend with mode usage */
	if (!pathw) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(pathw, -1, 1)

	if (!CreateDirectoryW(pathw, NULL)) {
		err = GetLastError();
		ret = -1;
	}
	free(pathw);

	if (0 > ret) {
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO int php_win32_ioutil_unlink_w(const wchar_t *path)
{/*{{{*/
	int ret = 0;
	DWORD err = 0;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, -1, 0)

	if (!DeleteFileW(path)) {
		err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO int php_win32_ioutil_rmdir_w(const wchar_t *path)
{/*{{{*/
	int ret = 0;
	DWORD err = 0;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, -1, 0)

	if (!RemoveDirectoryW(path)) {
		err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO int php_win32_ioutil_chdir_w(const wchar_t *path)
{/*{{{*/
	int ret = 0;
	DWORD err = 0;
	
	if (!SetCurrentDirectoryW(path)) {
		err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO int php_win32_ioutil_rename_w(const wchar_t *oldname, const wchar_t *newname)
{/*{{{*/
	int ret = 0;
	DWORD err = 0;
	
	PHP_WIN32_IOUTIL_CHECK_PATH_W(oldname, -1, 0)
	PHP_WIN32_IOUTIL_CHECK_PATH_W(newname, -1, 0)


	if (!MoveFileExW(oldname, newname, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED)) {
		err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO wchar_t *php_win32_ioutil_getcwd_w(const wchar_t *buf, int len)
{/*{{{*/
	DWORD err = 0;
	wchar_t *tmp_buf = NULL;

	/* If buf was NULL, the result has to be freed outside here. */
	if (!buf) {
		DWORD tmp_len = GetCurrentDirectoryW(0, NULL) + 1;
		if (!tmp_len) {
			err = GetLastError();
			SET_ERRNO_FROM_WIN32_CODE(err);
			return NULL;
		} else if (tmp_len > len) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_INSUFFICIENT_BUFFER);
			return NULL;
		}

		len = tmp_len;

		tmp_buf = (wchar_t *)malloc((len)*sizeof(wchar_t));
		if (!tmp_buf) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
			return NULL;
		}
		buf = tmp_buf;
	}
	
	if (!GetCurrentDirectoryW(len, buf)) {
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		free(tmp_buf);
		return NULL;
	}

	return (wchar_t *)buf;
}/*}}}*/

/* based on zend_dirname(). */
PW32IO size_t php_win32_ioutil_dirname(char *path, size_t len)
{/*{{{*/
	char *ret = NULL, *start;
	size_t ret_len, len_adjust = 0, pathw_len;
	wchar_t *endw, *pathw, *startw;

	if (len == 0) {
		return 0;
	}
	
	start = path;

	/* Don't really care about the path normalization, pure parsing here. */
	startw = pathw = php_win32_cp_conv_any_to_w(path, len, &pathw_len);
	if (!pathw) {
		return 0;
	}

	endw = pathw + pathw_len - 1;

	if ((2 <= len) && isalpha((int)((unsigned char *)path)[0]) && (':' == path[1])) {
		pathw += 2;
		path += 2;
		len_adjust += 2;
		if (2 == len) {
			free(startw);
			return len;
		}
	}

	/* Strip trailing slashes */
	while (endw >= pathw && PHP_WIN32_IOUTIL_IS_SLASHW(*endw)) {
		endw--;
	}
	if (endw < pathw) {
		free(startw);
		/* The path only contained slashes */
		path[0] = PHP_WIN32_IOUTIL_DEFAULT_SLASH;
		path[1] = '\0';
		return 1 + len_adjust;
	}

	/* Strip filename */
	while (endw >= pathw && !PHP_WIN32_IOUTIL_IS_SLASHW(*endw)) {
		endw--;
	}
	if (endw < pathw) {
		free(startw);
		path[0] = '.';
		path[1] = '\0';
		return 1 + len_adjust;
	}

	/* Strip slashes which came before the file name */
	while (endw >= pathw && PHP_WIN32_IOUTIL_IS_SLASHW(*endw)) {
		endw--;
	}
	if (endw < pathw) {
		free(startw);
		path[0] = PHP_WIN32_IOUTIL_DEFAULT_SLASH;
		path[1] = '\0';
		return 1 + len_adjust;
	}
	*(endw+1) = L'\0';

	ret_len = (endw + 1 - startw);
	if (PHP_WIN32_IOUTIL_IS_LONG_PATHW(startw, ret_len)) {
		ret = php_win32_ioutil_conv_w_to_any(startw + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW, ret_len - PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW, &ret_len);
	} else {
		ret = php_win32_ioutil_conv_w_to_any(startw, ret_len, &ret_len);
	}
	memmove(start, ret, ret_len+1);
	assert(start[ret_len] == '\0');
	free(ret);
	free(startw);

	return ret_len;
}/*}}}*/

/* Partial normalization can still be acceptable, explicit fail has to be caught. */
PW32IO php_win32_ioutil_normalization_result php_win32_ioutil_normalize_path_w(wchar_t **buf, size_t len, size_t *new_len)
{/*{{{*/
	wchar_t *pos, *idx = *buf, canonicalw[MAXPATHLEN];
	size_t ret_len = len;

	if (len >= MAXPATHLEN) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_BAD_LENGTH);
		*new_len = 0;
		return PHP_WIN32_IOUTIL_NORM_FAIL;
	}

	while (NULL != (pos = wcschr(idx, PHP_WIN32_IOUTIL_FW_SLASHW)) && idx - *buf <= len) {
		*pos = PHP_WIN32_IOUTIL_DEFAULT_SLASHW;
		idx = pos++;
	}

	if (S_OK != canonicalize_path_w(canonicalw, MAXPATHLEN, *buf, PATHCCH_ALLOW_LONG_PATHS)) {
		/* Length unchanged. */
		*new_len = len;
		return PHP_WIN32_IOUTIL_NORM_PARTIAL;
	}
	ret_len = wcslen(canonicalw);
	if (ret_len != len) {
		if (ret_len > len) {
			wchar_t *tmp = realloc(*buf, (ret_len + 1) * sizeof(wchar_t));
			if (!tmp) {
				SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
				/* Length unchanged. */
				*new_len = len;
				return PHP_WIN32_IOUTIL_NORM_PARTIAL;
			}
			*buf = tmp;
		}
		memmove(*buf, canonicalw, (ret_len + 1) * sizeof(wchar_t));
	}
	*new_len = ret_len;

	return PHP_WIN32_IOUTIL_NORM_OK;
}/*}}}*/

static HRESULT __stdcall MyPathCchCanonicalizeExFallback(wchar_t *pszPathOut, size_t cchPathOut, const wchar_t *pszPathIn, unsigned long dwFlags)
{/*{{{*/
	return -42;
}/*}}}*/

BOOL php_win32_ioutil_init(void)
{/*{{{*/
	HMODULE hMod = GetModuleHandle("api-ms-win-core-path-l1-1-0");

	if (hMod) {
		canonicalize_path_w = (MyPathCchCanonicalizeEx)GetProcAddress(hMod, "PathCchCanonicalizeEx");
		if (!canonicalize_path_w) {
			canonicalize_path_w = (MyPathCchCanonicalizeEx)MyPathCchCanonicalizeExFallback;
		}
	} else {
		canonicalize_path_w = (MyPathCchCanonicalizeEx)MyPathCchCanonicalizeExFallback;
	}

	return TRUE;
}/*}}}*/

/* an extended version could be implemented, for now direct functions can be used. */
#if 0
PW32IO int php_win32_ioutil_access_w(const wchar_t *path, mode_t mode)
{
	return _waccess(path, mode);
}
#endif

#if 0
PW32IO HANDLE php_win32_ioutil_findfirstfile_w(char *path, WIN32_FIND_DATA *data)
{
	HANDLE ret = INVALID_HANDLE_VALUE;
	DWORD err;

	if (!path) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return ret;
	}

	pathw = php_win32_ioutil_any_to_w(path);

	if (!pathw) {
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(ret);
		return ret;
	}

		ret = FindFirstFileW(pathw, data);
	
	if (INVALID_HANDLE_VALUE == ret && path) {
		ret = FindFirstFileA(path, data);
	}

	/* XXX set errno */
	return ret;
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
