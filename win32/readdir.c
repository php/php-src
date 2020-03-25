#include <malloc.h>
#include <string.h>
#include <errno.h>

#include "php.h"
#include "readdir.h"
#include "win32/ioutil.h"

/**********************************************************************
 * Implement dirent-style opendir/readdir/rewinddir/closedir on Win32
 *
 * Functions defined are opendir(), readdir(), rewinddir() and
 * closedir() with the same prototypes as the normal dirent.h
 * implementation.
 *
 * Does not implement telldir(), seekdir(), or scandir().  The dirent
 * struct is compatible with Unix, except that d_ino is always 1 and
 * d_off is made up as we go along.
 *
 * The DIR typedef is not compatible with Unix.
 **********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

DIR *opendir(const char *dir)
{/*{{{*/
	DIR *dp;
	wchar_t *filespecw, *resolvedw;
	HANDLE handle;
	char resolved_path_buff[MAXPATHLEN];
	size_t resolvedw_len, filespecw_len, index;
	zend_bool might_need_prefix;

	if (!VCWD_REALPATH(dir, resolved_path_buff)) {
		return NULL;
	}

	resolvedw = php_win32_ioutil_conv_any_to_w(resolved_path_buff, PHP_WIN32_CP_IGNORE_LEN, &resolvedw_len);
	if (!resolvedw) {
		return NULL;
	}

	might_need_prefix = resolvedw_len >= 3 && PHP_WIN32_IOUTIL_IS_LETTERW(resolvedw[0]) && L':' == resolvedw[1] && PHP_WIN32_IOUTIL_IS_SLASHW(resolvedw[2]);

	filespecw_len = resolvedw_len + 2;
	if (filespecw_len >= _MAX_PATH && might_need_prefix) {
		filespecw_len += PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
	}
	filespecw = (wchar_t *)malloc((filespecw_len + 1)*sizeof(wchar_t));
	if (filespecw == NULL) {
		free(resolvedw);
		return NULL;
	}

	if (filespecw_len >= _MAX_PATH && might_need_prefix) {
		wcscpy(filespecw, PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW);
		wcscpy(filespecw + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW, resolvedw);
		index = resolvedw_len + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW - 1;
	} else {
		wcscpy(filespecw, resolvedw);
		index = resolvedw_len - 1;
	}
	if (index >= 0 && filespecw[index] == L'/' || index == 0 && filespecw[index] == L'\\')
		filespecw[index] = L'\0';
	wcscat(filespecw, L"\\*");

	dp = (DIR *) calloc(1, sizeof(DIR) + (_MAX_FNAME*5+1)*sizeof(char));
	if (dp == NULL) {
		free(filespecw);
		free(resolvedw);
		return NULL;
	}

	if ((handle = FindFirstFileExW(filespecw, FindExInfoBasic, &(dp->fileinfo), FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH)) == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		if (err == ERROR_NO_MORE_FILES || err == ERROR_FILE_NOT_FOUND) {
			dp->finished = 1;
		} else {
			free(dp);
			free(filespecw);
			free(resolvedw);
			return NULL;
		}
	}
	dp->dirw = _wcsdup(resolvedw);
	dp->handle = handle;
	dp->offset = 0;
	dp->finished = 0;

	free(filespecw);
	free(resolvedw);

	return dp;
}/*}}}*/

struct dirent *readdir(DIR *dp)
{/*{{{*/
	char *_tmp;
	size_t reclen;

	if (!dp || dp->finished)
		return NULL;

	if (dp->offset != 0) {
		if (FindNextFileW(dp->handle, &(dp->fileinfo)) == 0) {
			dp->finished = 1;
			return NULL;
		}
	}

	_tmp = php_win32_cp_conv_w_to_any(dp->fileinfo.cFileName, PHP_WIN32_CP_IGNORE_LEN, &reclen);
	if (!_tmp) {
		/* wide to utf8 failed, should never happen. */
		return NULL;
	}
	memmove(dp->dent.d_name, _tmp, reclen + 1);
	free(_tmp);
	dp->dent.d_reclen = (unsigned short)reclen;

	dp->offset++;

	dp->dent.d_ino = 1;
	dp->dent.d_off = dp->offset;

	return &(dp->dent);
}/*}}}*/

int closedir(DIR *dp)
{/*{{{*/
	if (!dp)
		return 0;
	/* It is valid to scan an empty directory but we have an invalid
	   handle in this case (no first file found). */
	if (dp->handle != INVALID_HANDLE_VALUE) {
		FindClose(dp->handle);
	}
	if (dp->dirw)
		free(dp->dirw);
	if (dp)
		free(dp);

	return 0;
}/*}}}*/

int rewinddir(DIR *dp)
{/*{{{*/
	/* Re-set to the beginning */
	wchar_t *filespecw;
	HANDLE handle;
	size_t dirw_len, filespecw_len, index;
	zend_bool might_need_prefix;

	FindClose(dp->handle);

	dp->offset = 0;
	dp->finished = 0;

	/* XXX save the dir len into the struct. */
	dirw_len = wcslen((wchar_t *)dp->dirw);

	might_need_prefix = dirw_len >= 3 && PHP_WIN32_IOUTIL_IS_LETTERW(dp->dirw[0]) && L':' == dp->dirw[1] && PHP_WIN32_IOUTIL_IS_SLASHW(dp->dirw[2]);

	filespecw_len = dirw_len + 2;
	if (filespecw_len >= _MAX_PATH && might_need_prefix) {
		filespecw_len += PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW;
	}

	filespecw = (wchar_t *)malloc((filespecw_len + 1)*sizeof(wchar_t));
	if (filespecw == NULL) {
		return -1;
	}

	if (filespecw_len >= _MAX_PATH && might_need_prefix) {
		wcscpy(filespecw, PHP_WIN32_IOUTIL_LONG_PATH_PREFIXW);
		wcscpy(filespecw + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW, dp->dirw);
		index = dirw_len + PHP_WIN32_IOUTIL_LONG_PATH_PREFIX_LENW - 1;
	} else {
		wcscpy(filespecw, dp->dirw);
		index = dirw_len - 1;
	}

	if (index >= 0 && (filespecw[index] == L'/' ||
	   (filespecw[index] == L'\\' && index == 0)))
		filespecw[index] = L'\0';
	wcscat(filespecw, L"\\*");

	if ((handle = FindFirstFileExW(filespecw, FindExInfoBasic, &(dp->fileinfo), FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH)) == INVALID_HANDLE_VALUE) {
		dp->finished = 1;
	}

	free(filespecw);
	dp->handle = handle;

	return 0;
}/*}}}*/

#ifdef __cplusplus
}
#endif
