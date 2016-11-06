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

/* typedef DIR - not the same as Unix */
struct DIR_W32 {
	HANDLE handle;				/* _findfirst/_findnext handle */
	int offset;					/* offset into directory */
	short finished;				/* 1 if there are not more files */
	WIN32_FIND_DATAW fileinfo;  /* from _findfirst/_findnext */
	wchar_t *dirw;		/* the dir we are reading */
	struct dirent dent;			/* the dirent to return */
};

DIR *opendir(const char *dir)
{
	DIR *dp;
	wchar_t *filespecw, *resolvedw;
	HANDLE handle;
	int index;
	char resolved_path_buff[MAXPATHLEN];
	size_t resolvedw_len, filespecw_len;

	if (!VCWD_REALPATH(dir, resolved_path_buff)) {
		return NULL;
	}

	dp = (DIR *) malloc(sizeof(DIR));
	if (dp == NULL) {
		return NULL;
	}

	resolvedw = php_win32_ioutil_conv_any_to_w(resolved_path_buff, PHP_WIN32_CP_IGNORE_LEN, &resolvedw_len);
	if (!resolvedw) {
		free(dp);
		return NULL;
	}

	filespecw_len = resolvedw_len + 2;
	filespecw = (wchar_t *)malloc((filespecw_len + 1)*sizeof(wchar_t));
	if (filespecw == NULL) {
		free(dp);
		free(resolvedw);
		return NULL;
	}

	wcscpy(filespecw, resolvedw);
	index = (int)filespecw_len - 1;
	if (index >= 0 && filespecw[index] == L'/' || index == 0 && filespecw[index] == L'\\')
		filespecw[index] = L'\0';
	wcscat(filespecw, L"\\*");

	if ((handle = FindFirstFileW(filespecw, &(dp->fileinfo))) == INVALID_HANDLE_VALUE) {
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
}

struct dirent *readdir(DIR *dp)
{
	char *_tmp;

	if (!dp || dp->finished)
		return NULL;

	if (dp->offset != 0) {
		if (FindNextFileW(dp->handle, &(dp->fileinfo)) == 0) {
			dp->finished = 1;
			return NULL;
		}
	}

	_tmp = php_win32_ioutil_w_to_any(dp->fileinfo.cFileName);
	if (!_tmp) {
		/* wide to utf8 failed, should never happen. */
		return NULL;
	}
	strlcpy(dp->dent.d_name, _tmp, _MAX_FNAME+1);
	dp->dent.d_reclen = (unsigned short)strlen(dp->dent.d_name);
	free(_tmp);
	
	dp->offset++;

	dp->dent.d_ino = 1;
	dp->dent.d_off = dp->offset;

	return &(dp->dent);
}

int readdir_r(DIR *dp, struct dirent *entry, struct dirent **result)
{
	char *_tmp;

	if (!dp || dp->finished) {
		*result = NULL;
		return 0;
	}

	if (dp->offset != 0) {
		if (FindNextFileW(dp->handle, &(dp->fileinfo)) == 0) {
			dp->finished = 1;
			*result = NULL;
			return 0;
		}
	}

	_tmp = php_win32_ioutil_w_to_any(dp->fileinfo.cFileName);
	if (!_tmp) {
		/* wide to utf8 failed, should never happen. */
		result = NULL;
		return 0;
	}
	strlcpy(dp->dent.d_name, _tmp, _MAX_FNAME+1);
	dp->dent.d_reclen = (unsigned short)strlen(dp->dent.d_name);
	free(_tmp);

	dp->offset++;

	dp->dent.d_ino = 1;
	dp->dent.d_off = dp->offset;

	memcpy(entry, &dp->dent, sizeof(*entry));

	*result = &dp->dent;

	return 0;
}

int closedir(DIR *dp)
{
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
}

int rewinddir(DIR *dp)
{
	/* Re-set to the beginning */
	wchar_t *filespecw;
	HANDLE handle;
	int index;

	FindClose(dp->handle);

	dp->offset = 0;
	dp->finished = 0;

	filespecw = (wchar_t *)malloc((wcslen((wchar_t *)dp->dirw) + 2 + 1)*sizeof(wchar_t));
	if (filespecw == NULL) {
		return -1;
	}

	wcscpy(filespecw, (wchar_t *)dp->dirw);
	index = (int)wcslen(filespecw) - 1;
	if (index >= 0 && (filespecw[index] == L'/' ||
	   (filespecw[index] == L'\\' && index == 0)))
		filespecw[index] = L'\0';
	wcscat(filespecw, L"/*");

	if ((handle = FindFirstFileW(filespecw, &(dp->fileinfo))) == INVALID_HANDLE_VALUE) {
		dp->finished = 1;
	}

	free(filespecw);
	dp->handle = handle;

	return 0;
}

#ifdef __cplusplus
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
