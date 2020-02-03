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
#include "main/streams/php_stream_plain_wrapper.h"

#include <pathcch.h>
#include <winioctl.h>
#include <winnt.h>

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
	opts->share = PHP_WIN32_IOUTIL_DEFAULT_SHARE_MODE;

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

PW32IO int php_win32_ioutil_mkdir_w(const wchar_t *path, mode_t mode)
{/*{{{*/
	size_t path_len;
	const wchar_t *my_path;

	if (!path) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return -1;
	}

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, -1, 0)

	path_len = wcslen(path);
	if (path_len < _MAX_PATH && path_len >= _MAX_PATH - 12) {
		/* Special case here. From the doc:

		 "When using an API to create a directory, the specified path cannot be
		 so long that you cannot append an 8.3 file name ..."

		 Thus, if the directory name length happens to be in this range, it
		 already needs to be a long path. The given path is already normalized
		 and prepared, need only to prefix it.
		 */
		wchar_t *tmp = (wchar_t *) malloc((path_len + 1) * sizeof(wchar_t));
		if (!tmp) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
			return -1;
		}
		memmove(tmp, path, (path_len + 1) * sizeof(wchar_t));

		if (PHP_WIN32_IOUTIL_NORM_FAIL == php_win32_ioutil_normalize_path_w(&tmp, path_len, &path_len)) {
			free(tmp);
			return -1;
		}

		if (!PHP_WIN32_IOUTIL_IS_LONG_PATHW(tmp, path_len)) {
			wchar_t *_tmp = (wchar_t *) malloc((path_len + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW + 1) * sizeof(wchar_t));
			wchar_t *src, *dst;
			if (!_tmp) {
				SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
				free(tmp);
				return -1;
			}
			memmove(_tmp, PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW, PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW * sizeof(wchar_t));
			src = tmp;
			dst = _tmp + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
			while (src < tmp + path_len) {
				if (*src == PHP_WIN32_IOUTIL_FW_SLASHW) {
					*dst++ = PHP_WIN32_IOUTIL_DEFAULT_SLASHW;
					src++;
				} else {
					*dst++ = *src++;
				}
			}
			path_len += PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
			_tmp[path_len] = L'\0';
			free(tmp);
			tmp = _tmp;
		}

		my_path = tmp;

	} else {
		my_path = path;
	}

	if (!CreateDirectoryW(my_path, NULL)) {
		DWORD err = GetLastError();
		if (my_path != path) {
			free((void *)my_path);
		}
		SET_ERRNO_FROM_WIN32_CODE(err);
		return -1;
	}

	if (my_path != path) {
		free((void *)my_path);
	}

	return 0;
}/*}}}*/

PW32IO int php_win32_ioutil_unlink_w(const wchar_t *path)
{/*{{{*/
	DWORD err = 0;
	HANDLE h;
	BY_HANDLE_FILE_INFORMATION info;
	FILE_DISPOSITION_INFO disposition;
	BOOL status;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, -1, 0)

	h = CreateFileW(path,
					FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES | DELETE,
					PHP_WIN32_IOUTIL_DEFAULT_SHARE_MODE,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
					NULL);

	if (INVALID_HANDLE_VALUE == h) {
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return -1;
	}

	if (!GetFileInformationByHandle(h, &info)) {
		err = GetLastError();
		CloseHandle(h);
		SET_ERRNO_FROM_WIN32_CODE(err);
		return -1;
	}

	if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		/* TODO Handle possible reparse point. */
		CloseHandle(h);
		SET_ERRNO_FROM_WIN32_CODE(ERROR_DIRECTORY_NOT_SUPPORTED);
		return -1;
	}

#if 0
	/* XXX BC breach! */
	if (info.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
		/* Remove read-only attribute */
		FILE_BASIC_INFO basic = { 0 };

		basic.FileAttributes = info.dwFileAttributes & ~(FILE_ATTRIBUTE_READONLY);

		status = SetFileInformationByHandle(h, FileBasicInfo, &basic, sizeof basic);
		if (!status) {
			err = GetLastError();
			SET_ERRNO_FROM_WIN32_CODE(err);
			CloseHandle(h);
			return -1;
		}
	}
#endif

	/* Try to set the delete flag. */
	disposition.DeleteFile = TRUE;
	status = SetFileInformationByHandle(h, FileDispositionInfo, &disposition, sizeof disposition);
	if (!status) {
		err = GetLastError();
		CloseHandle(h);
		SET_ERRNO_FROM_WIN32_CODE(err);
		return -1;
	}

	CloseHandle(h);

	return 0;
}/*}}}*/

PW32IO int php_win32_ioutil_rmdir_w(const wchar_t *path)
{/*{{{*/
	int ret = 0;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, -1, 0)

	if (!RemoveDirectoryW(path)) {
		DWORD err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO int php_win32_ioutil_chdir_w(const wchar_t *path)
{/*{{{*/
	int ret = 0;

	if (!SetCurrentDirectoryW(path)) {
		DWORD err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO int php_win32_ioutil_rename_w(const wchar_t *oldname, const wchar_t *newname)
{/*{{{*/
	int ret = 0;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(oldname, -1, 0)
	PHP_WIN32_IOUTIL_CHECK_PATH_W(newname, -1, 0)


	if (!MoveFileExW(oldname, newname, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED)) {
		DWORD err = GetLastError();
		ret = -1;
		SET_ERRNO_FROM_WIN32_CODE(err);
	}

	return ret;
}/*}}}*/

PW32IO wchar_t *php_win32_ioutil_getcwd_w(wchar_t *buf, size_t len)
{/*{{{*/
	DWORD err = 0;
	wchar_t *tmp_buf = NULL;
	DWORD tmp_len = (DWORD)len;

	/* If buf was NULL, the result has to be freed outside here. */
	if (!buf) {
		tmp_len = GetCurrentDirectoryW(0, NULL) + 1;
		if (!tmp_len) {
			err = GetLastError();
			SET_ERRNO_FROM_WIN32_CODE(err);
			return NULL;
		} else if (tmp_len > len) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_INSUFFICIENT_BUFFER);
			return NULL;
		}

		tmp_buf = (wchar_t *)malloc((tmp_len)*sizeof(wchar_t));
		if (!tmp_buf) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
			return NULL;
		}
		buf = tmp_buf;
	}

	if (!GetCurrentDirectoryW(tmp_len, buf)) {
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

	if ((2 <= pathw_len) && iswalpha((wint_t)(pathw)[0]) && (L':' == pathw[1])) {
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
	wchar_t *idx = *buf, canonicalw[MAXPATHLEN], _tmp[MAXPATHLEN], *pos = _tmp;
	size_t ret_len = len;

	if (len >= MAXPATHLEN) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_BAD_LENGTH);
		*new_len = 0;
		return PHP_WIN32_IOUTIL_NORM_FAIL;
	}

	for (; (size_t)(idx - *buf) <= len; idx++, pos++) {
		*pos = *idx;
		if (PHP_WIN32_IOUTIL_FW_SLASHW == *pos) {
			*pos = PHP_WIN32_IOUTIL_DEFAULT_SLASHW;
		}
		while (PHP_WIN32_IOUTIL_IS_SLASHW(*idx) && PHP_WIN32_IOUTIL_IS_SLASHW(*(idx+1))) {
			idx++;
		}
	}

	if (S_OK != canonicalize_path_w(canonicalw, MAXPATHLEN, _tmp, PATHCCH_ALLOW_LONG_PATHS)) {
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

PW32IO int php_win32_ioutil_access_w(const wchar_t *path, mode_t mode)
{/*{{{*/
	DWORD attr;

	if ((mode & X_OK) == X_OK) {
		DWORD type;
		return GetBinaryTypeW(path, &type) ? 0 : -1;
	}

	attr = GetFileAttributesW(path);
	if (attr == INVALID_FILE_ATTRIBUTES) {
		DWORD err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return -1;
	}

	if (F_OK == mode) {
		return 0;
	}

	if ((mode &W_OK) == W_OK && (attr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_ACCESS_DENIED);
		return -1;
	}

	return 0;
}/*}}}*/

PW32IO FILE *php_win32_ioutil_fopen_w(const wchar_t *path, const wchar_t *mode)
{/*{{{*/
	FILE *ret;
	char modea[16] = {0};
	int err = 0, fd, flags, i = 0;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, NULL, 0)

	/* Using the converter from streams, char only. */
	while (i < sizeof(modea)-1 && mode[i]) {
		modea[i] = (char)mode[i];
		i++;
	}
	if (SUCCESS != php_stream_parse_fopen_modes(modea, &flags)) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	fd = php_win32_ioutil_open_w(path, flags, 0666);
	if (0 > fd) {
		err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	ret = _wfdopen(fd, mode);
	if (!ret) {
		err = GetLastError();
		php_win32_ioutil_close(fd);
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	return ret;
}/*}}}*/

static size_t php_win32_ioutil_realpath_h(HANDLE *h, wchar_t **resolved)
{/*{{{*/
	wchar_t ret[PHP_WIN32_IOUTIL_MAXPATHLEN], *ret_real;
	DWORD ret_len, ret_real_len;

	ret_len = GetFinalPathNameByHandleW(h, ret, PHP_WIN32_IOUTIL_MAXPATHLEN-1, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
	if (0 == ret_len) {
		DWORD err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return (size_t)-1;
	} else if (ret_len > PHP_WIN32_IOUTIL_MAXPATHLEN) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_INVALID_PARAMETER);
		return (size_t)-1;
	}

	if (NULL == *resolved) {
		/* ret is expected to be either NULL or a buffer of capable size. */
		*resolved = (wchar_t *) malloc((ret_len + 1)*sizeof(wchar_t));
		if (!*resolved) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
			return (size_t)-1;
		}
	}

	ret_real = ret;
	ret_real_len = ret_len;
	if (0 == wcsncmp(ret, PHP_WIN32_IOUTIL_UNC_PATH_PREFIXW, PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW)) {
		ret_real += (PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW - 2);
		ret_real[0] = L'\\';
		ret_real_len -= (PHP_WIN32_IOUTIL_UNC_PATH_PREFIX_LENW - 2);
	} else if (PHP_WIN32_IOUTIL_IS_LONG_PATHW(ret, ret_len)) {
		ret_real += PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
		ret_real_len -= PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
	}
	memmove(*resolved, ret_real, (ret_real_len+1)*sizeof(wchar_t));

	return ret_real_len;
}/*}}}*/

PW32IO wchar_t *php_win32_ioutil_realpath_w(const wchar_t *path, wchar_t *resolved)
{/*{{{*/
	return php_win32_ioutil_realpath_w_ex0(path, resolved, NULL);
}/*}}}*/

PW32IO wchar_t *php_win32_ioutil_realpath_w_ex0(const wchar_t *path, wchar_t *resolved, PBY_HANDLE_FILE_INFORMATION info)
{/*{{{*/
	HANDLE h;
	size_t ret_len;

	PHP_WIN32_IOUTIL_CHECK_PATH_W(path, NULL, 0)

	h = CreateFileW(path,
					0,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
					NULL);
	if (INVALID_HANDLE_VALUE == h) {
		DWORD err = GetLastError();
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	ret_len = php_win32_ioutil_realpath_h(h, &resolved);
	if ((size_t)-1 == ret_len) {
		DWORD err = GetLastError();
		CloseHandle(h);
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	if (NULL != info && !GetFileInformationByHandle(h, info)) {
		DWORD err = GetLastError();
		CloseHandle(h);
		SET_ERRNO_FROM_WIN32_CODE(err);
		return NULL;
	}

	CloseHandle(h);

	return resolved;
}/*}}}*/

PW32IO char *realpath(const char *path, char *resolved)
{/*{{{*/
	return php_win32_ioutil_realpath(path, resolved);
}/*}}}*/

PW32IO int php_win32_ioutil_symlink_w(const wchar_t *target, const wchar_t *link)
{/*{{{*/
	DWORD attr;
	BOOLEAN res;

	if ((attr = GetFileAttributesW(target)) == INVALID_FILE_ATTRIBUTES) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return -1;
	}

	res = CreateSymbolicLinkW(link, target, (attr & FILE_ATTRIBUTE_DIRECTORY ? 1 : 0));
	if (!res) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return -1;
	}

	return 0;
}/*}}}*/

PW32IO int php_win32_ioutil_link_w(const wchar_t *target, const wchar_t *link)
{/*{{{*/
	BOOL res;

	res = CreateHardLinkW(link, target, NULL);
	if (!res) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return -1;
	}

	return 0;
}/*}}}*/

#define FILETIME_TO_UINT(filetime)                                          \
   (*((uint64_t*) &(filetime)) - 116444736000000000ULL)

#define FILETIME_TO_TIME_T(filetime)                                        \
   (time_t)(FILETIME_TO_UINT(filetime) / 10000000ULL)

static int php_win32_ioutil_fstat_int(HANDLE h, php_win32_ioutil_stat_t *buf, const wchar_t *pathw, size_t pathw_len, PBY_HANDLE_FILE_INFORMATION dp)
{/*{{{*/
	BY_HANDLE_FILE_INFORMATION d;
	PBY_HANDLE_FILE_INFORMATION data;
	LARGE_INTEGER t;
	wchar_t mypath[MAXPATHLEN];
	uint8_t is_dir;

	data = !dp ? &d : dp;

	if(!GetFileInformationByHandle(h, data)) {
		if (INVALID_HANDLE_VALUE != h) {
			/* Perhaps it's a fileless stream like stdio, reuse the normal stat info. */
			struct __stat64 _buf;
			if (_fstat64(_open_osfhandle((intptr_t)h, 0), &_buf)) {
				return -1;
			}
			buf->st_dev = _buf.st_dev;
			buf->st_ino = _buf.st_ino;
			buf->st_mode = _buf.st_mode;
			buf->st_nlink = _buf.st_nlink;
			buf->st_uid = _buf.st_uid;
			buf->st_gid = _buf.st_gid;
			buf->st_rdev = _buf.st_rdev;
			buf->st_size = _buf.st_size;
			buf->st_atime = _buf.st_atime;
			buf->st_mtime = _buf.st_mtime;
			buf->st_ctime = _buf.st_ctime;
			return 0;
		} else if(h == INVALID_HANDLE_VALUE && pathw_len > 0) {
			/* An abnormal situation it is. For example, the user is the file
				owner, but the file has an empty DACL. In that case, it is
				possible CreateFile would fail, but the attributes still can
				be read. Some info is still going to be missing. */
			WIN32_FILE_ATTRIBUTE_DATA _data;
			if (!GetFileAttributesExW(pathw, GetFileExInfoStandard, &_data)) {
				DWORD err = GetLastError();
				SET_ERRNO_FROM_WIN32_CODE(err);
				return -1;
			}
			data->dwFileAttributes = _data.dwFileAttributes;
			data->ftCreationTime = _data.ftCreationTime;
			data->ftLastAccessTime = _data.ftLastAccessTime;
			data->ftLastWriteTime = _data.ftLastWriteTime;
			data->nFileSizeHigh = _data.nFileSizeHigh;
			data->nFileSizeLow = _data.nFileSizeLow;
			data->dwVolumeSerialNumber = 0;
			data->nNumberOfLinks = 1;
			data->nFileIndexHigh = 0;
			data->nFileIndexLow = 0;
		} else {
			DWORD err = GetLastError();
			SET_ERRNO_FROM_WIN32_CODE(err);
			return -1;
		}
	}

	is_dir = (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;

	buf->st_dev = data->dwVolumeSerialNumber;

	buf->st_rdev = buf->st_uid = buf->st_gid = 0;

	buf->st_ino = (((uint64_t)data->nFileIndexHigh) << 32) + data->nFileIndexLow;

	buf->st_mode = 0;

	if (!is_dir) {
		if (pathw_len >= 4 &&
			pathw[pathw_len-4] == L'.') {
			if (_wcsnicmp(pathw+pathw_len-3, L"exe", 3) == 0 ||
			_wcsnicmp(pathw+pathw_len-3, L"com", 3) == 0) {
				DWORD type;
				if (GetBinaryTypeW(pathw, &type)) {
					buf->st_mode  |= (S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6));
				}
			/* The below is actually incorrect, but keep for BC. */
			} else if (_wcsnicmp(pathw+pathw_len-3, L"bat", 3) == 0 ||
				_wcsnicmp(pathw+pathw_len-3, L"cmd", 3) == 0) {
				buf->st_mode  |= (S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6));
			}
		}
	}

	if ((data->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0) {
		if (is_dir) {
			buf->st_mode |= (S_IFDIR|S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6));
		} else {
			switch (GetFileType(h)) {
				case FILE_TYPE_CHAR:
					buf->st_mode |= S_IFCHR;
					break;
				case FILE_TYPE_PIPE:
					buf->st_mode |= S_IFIFO;
					break;
				default:
					buf->st_mode |= S_IFREG;
			}
		}
		buf->st_mode |= (data->dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)) : (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)|S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6));
	}

	buf->st_nlink = data->nNumberOfLinks;
	t.HighPart = data->nFileSizeHigh;
	t.LowPart = data->nFileSizeLow;
	/* It's an overflow on 32 bit, however it won't fix as long
	as zend_long is 32 bit. */
	buf->st_size = (zend_long)t.QuadPart;
	buf->st_atime = FILETIME_TO_TIME_T(data->ftLastAccessTime);
	buf->st_ctime = FILETIME_TO_TIME_T(data->ftCreationTime);
	buf->st_mtime = FILETIME_TO_TIME_T(data->ftLastWriteTime);

	return 0;
}/*}}}*/

PW32IO int php_win32_ioutil_stat_ex_w(const wchar_t *path, size_t path_len, php_win32_ioutil_stat_t *buf, int lstat)
{/*{{{*/
	BY_HANDLE_FILE_INFORMATION data;
	HANDLE hLink = NULL;
	DWORD flags_and_attrs = lstat ? FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OPEN_REPARSE_POINT : FILE_FLAG_BACKUP_SEMANTICS;
	int ret;
	ALLOCA_FLAG(use_heap_large)

	hLink = CreateFileW(path,
			FILE_READ_ATTRIBUTES,
			PHP_WIN32_IOUTIL_DEFAULT_SHARE_MODE,
			NULL,
			OPEN_EXISTING,
			flags_and_attrs,
			NULL
	);

	ret = php_win32_ioutil_fstat_int(hLink, buf, path, path_len, &data);

	if (lstat && data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
		/* File is a reparse point. Get the target */
		PHP_WIN32_IOUTIL_REPARSE_DATA_BUFFER * pbuffer;
		DWORD retlength = 0;

		pbuffer = (PHP_WIN32_IOUTIL_REPARSE_DATA_BUFFER *)do_alloca(MAXIMUM_REPARSE_DATA_BUFFER_SIZE, use_heap_large);
		if(!DeviceIoControl(hLink, FSCTL_GET_REPARSE_POINT, NULL, 0, pbuffer,  MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &retlength, NULL)) {
			free_alloca(pbuffer, use_heap_large);
			CloseHandle(hLink);
			return -1;
		}

		if(pbuffer->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
			buf->st_mode = S_IFLNK;
			buf->st_mode |= (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)) : (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)|S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6));
		}

#if 0 /* Not used yet */
		else if(pbuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
			buf->st_mode |=;
		}
#endif
		free_alloca(pbuffer, use_heap_large);
	}

	CloseHandle(hLink);

	return ret;

}/*}}}*/

PW32IO int php_win32_ioutil_fstat(int fd, php_win32_ioutil_stat_t *buf)
{/*{{{*/
	return php_win32_ioutil_fstat_int((HANDLE)_get_osfhandle(fd), buf, NULL, 0, NULL);
}/*}}}*/

static ssize_t php_win32_ioutil_readlink_int(HANDLE h, wchar_t *buf, size_t buf_len)
{/*{{{*/
	char buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
	PHP_WIN32_IOUTIL_REPARSE_DATA_BUFFER *reparse_data = (PHP_WIN32_IOUTIL_REPARSE_DATA_BUFFER*) buffer;
	wchar_t* reparse_target;
	DWORD reparse_target_len;
	DWORD bytes;

	if (!DeviceIoControl(h,
		FSCTL_GET_REPARSE_POINT,
		NULL,
		0,
		buffer,
		sizeof buffer,
		&bytes,
		NULL)) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return -1;
	}

	if (reparse_data->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
		/* Real symlink */

		/* BC - relative links are shown as absolute */
		if (reparse_data->SymbolicLinkReparseBuffer.Flags & SYMLINK_FLAG_RELATIVE) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_SYMLINK_NOT_SUPPORTED);
			return -1;
		}

		reparse_target = reparse_data->SymbolicLinkReparseBuffer.ReparseTarget +
			(reparse_data->SymbolicLinkReparseBuffer.SubstituteNameOffset /
			sizeof(wchar_t));
		reparse_target_len =
			reparse_data->SymbolicLinkReparseBuffer.SubstituteNameLength /
			sizeof(wchar_t);

		/* Real symlinks can contain pretty much everything, but the only thing we
		* really care about is undoing the implicit conversion to an NT namespaced
		* path that CreateSymbolicLink will perform on absolute paths. If the path
		* is win32-namespaced then the user must have explicitly made it so, and
		* we better just return the unmodified reparse data. */
		if (reparse_target_len >= 4 &&
		reparse_target[0] == L'\\' &&
		reparse_target[1] == L'?' &&
		reparse_target[2] == L'?' &&
		reparse_target[3] == L'\\') {
			/* Starts with \??\ */
			if (reparse_target_len >= 6 &&
				((reparse_target[4] >= L'A' && reparse_target[4] <= L'Z') ||
				(reparse_target[4] >= L'a' && reparse_target[4] <= L'z')) &&
				reparse_target[5] == L':' &&
				(reparse_target_len == 6 || reparse_target[6] == L'\\')) {
				/* \??\<drive>:\ */
				reparse_target += 4;
				reparse_target_len -= 4;

			} else if (reparse_target_len >= 8 &&
				(reparse_target[4] == L'U' || reparse_target[4] == L'u') &&
				(reparse_target[5] == L'N' || reparse_target[5] == L'n') &&
				(reparse_target[6] == L'C' || reparse_target[6] == L'c') &&
				reparse_target[7] == L'\\') {
				/* \??\UNC\<server>\<share>\ - make sure the final path looks like
				* \\<server>\<share>\ */
				reparse_target += 6;
				reparse_target[0] = L'\\';
				reparse_target_len -= 6;
			}
		}

	} else if (reparse_data->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
		/* Junction. */
		reparse_target = reparse_data->MountPointReparseBuffer.ReparseTarget +
			(reparse_data->MountPointReparseBuffer.SubstituteNameOffset /
			sizeof(wchar_t));
		reparse_target_len = reparse_data->MountPointReparseBuffer.SubstituteNameLength / sizeof(wchar_t);

		/* Only treat junctions that look like \??\<drive>:\ as symlink. Junctions
		* can also be used as mount points, like \??\Volume{<guid>}, but that's
		* confusing for programs since they wouldn't be able to actually
		* understand such a path when returned by uv_readlink(). UNC paths are
		* never valid for junctions so we don't care about them. */
		if (!(reparse_target_len >= 6 &&
			reparse_target[0] == L'\\' &&
			reparse_target[1] == L'?' &&
			reparse_target[2] == L'?' &&
			reparse_target[3] == L'\\' &&
			((reparse_target[4] >= L'A' && reparse_target[4] <= L'Z') ||
			(reparse_target[4] >= L'a' && reparse_target[4] <= L'z')) &&
			reparse_target[5] == L':' &&
			(reparse_target_len == 6 || reparse_target[6] == L'\\'))) {
			SET_ERRNO_FROM_WIN32_CODE(ERROR_SYMLINK_NOT_SUPPORTED);
			return -1;
		}

		/* Remove leading \??\ */
		reparse_target += 4;
		reparse_target_len -= 4;

	} else {
		/* Reparse tag does not indicate a symlink. */
		SET_ERRNO_FROM_WIN32_CODE(ERROR_SYMLINK_NOT_SUPPORTED);
		return -1;
	}

	if (reparse_target_len >= buf_len) {
		SET_ERRNO_FROM_WIN32_CODE(ERROR_NOT_ENOUGH_MEMORY);
		return -1;
	}

	memcpy(buf, reparse_target, reparse_target_len*sizeof(wchar_t));
	buf[reparse_target_len] = L'\0';

	return reparse_target_len;
}/*}}}*/

PW32IO ssize_t php_win32_ioutil_readlink_w(const wchar_t *path, wchar_t *buf, size_t buf_len)
{/*{{{*/
	HANDLE h;
	ssize_t ret;

	/* Get a handle to the symbolic link (if path is a symbolic link) */
	h = CreateFileW(path,
					0,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
					NULL);

	if (h == INVALID_HANDLE_VALUE) {
		SET_ERRNO_FROM_WIN32_CODE(GetLastError());
		return -1;
	}

	ret = php_win32_ioutil_readlink_int(h, buf, buf_len);

	if (ret < 0) {
		wchar_t target[PHP_WIN32_IOUTIL_MAXPATHLEN];
		size_t target_len;
		size_t offset = 0;

		/* BC - get a handle to the target (if path is a symbolic link) */
		CloseHandle(h);
		h = CreateFileW(path,
						0,
						0,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);

		if (h == INVALID_HANDLE_VALUE) {
			SET_ERRNO_FROM_WIN32_CODE(GetLastError());
			return -1;
		}

		target_len = GetFinalPathNameByHandleW(h, target, PHP_WIN32_IOUTIL_MAXPATHLEN, VOLUME_NAME_DOS);

		if(target_len >= buf_len || target_len >= PHP_WIN32_IOUTIL_MAXPATHLEN || target_len == 0) {
			CloseHandle(h);
			return -1;
		}

		if(target_len > 4) {
			/* Skip first 4 characters if they are "\\?\" */
			if(target[0] == L'\\' && target[1] == L'\\' && target[2] == L'?' && target[3] ==  L'\\') {
				offset = 4;

				/* \\?\UNC\ */
				if (target_len > 7 && target[4] == L'U' && target[5] == L'N' && target[6] == L'C') {
					offset += 2;
					target[offset] = L'\\';
				}
			}
		}

		ret = target_len - offset;
		memcpy(buf, target + offset, (ret + 1)*sizeof(wchar_t));
	}

	CloseHandle(h);

	return ret;
}/*}}}*/
