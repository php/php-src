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
	short is_w;
	struct {
		WIN32_FIND_DATAA a;
		WIN32_FIND_DATAW w;
	} fileinfo;    /* from _findfirst/_findnext */
	char *dira;		/* the dir we are reading */
	wchar_t *dirw;		/* the dir we are reading */
	struct dirent dent;			/* the dirent to return */
};

DIR *opendir(const char *dir)
{
	DIR *dp;
	char *filespec;
	wchar_t *filespecw, *resolvedw;
	HANDLE handle;
	int index;
	char resolved_path_buff[MAXPATHLEN];

	if (!VCWD_REALPATH(dir, resolved_path_buff)) {
		return NULL;
	}

	dp = (DIR *) malloc(sizeof(DIR));
	if (dp == NULL) {
		return NULL;
	}

	resolvedw = php_win32_ioutil_any_to_w(resolved_path_buff);

	if (resolvedw) {
		filespecw = (wchar_t *)malloc((wcslen(resolvedw) + 2 + 1)*sizeof(wchar_t));
		if (filespecw == NULL) {
			return NULL;
		}
		wcscpy(filespecw, resolvedw);
		index = (int)wcslen(filespecw) - 1;
		if (index >= 0 && (filespecw[index] == L'/' ||
		   (filespecw[index] == L'\\' && index == 0)))
			filespecw[index] = L'\0';
		wcscat(filespecw, L"\\*");

		if ((handle = FindFirstFileW(filespecw, &(dp->fileinfo.w))) == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			if (err == ERROR_NO_MORE_FILES || err == ERROR_FILE_NOT_FOUND) {
				dp->finished = 1;
			} else {
				free(dp);
				free(filespecw);
				return NULL;
			}
		}
		dp->dirw = _wcsdup(resolvedw);
		dp->dira = NULL;
		free(filespecw);
		free(resolvedw);
		dp->is_w = 1;
	} else {
		filespec = (char *)malloc(strlen(resolved_path_buff) + 2 + 1);
		if (filespec == NULL) {
			return NULL;
		}
		strcpy(filespec, resolved_path_buff);
		index = (int)strlen(filespec) - 1;
		if (index >= 0 && (filespec[index] == '/' ||
		   (filespec[index] == '\\' && (index == 0 || !IsDBCSLeadByte(filespec[index-1])))))
			filespec[index] = '\0';
		strcat(filespec, "\\*");

		if ((handle = FindFirstFile(filespec, &(dp->fileinfo.a))) == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			if (err == ERROR_NO_MORE_FILES || err == ERROR_FILE_NOT_FOUND) {
				dp->finished = 1;
			} else {
				free(dp);
				free(filespec);
				return NULL;
			}
		}
		dp->dira = _strdup(resolved_path_buff);
		dp->dirw = NULL;
		free(filespec);

		dp->is_w = 0;
	}
	dp->handle = handle;
	dp->offset = 0;
	dp->finished = 0;

	return dp;
}

struct dirent *readdir(DIR *dp)
{
	if (!dp || dp->finished)
		return NULL;

	if (dp->offset != 0) {
		if (dp->is_w) {
			if (FindNextFileW(dp->handle, &(dp->fileinfo.w)) == 0) {
				dp->finished = 1;
				return NULL;
			}
		} else {
			if (FindNextFileA(dp->handle, &(dp->fileinfo.a)) == 0) {
				dp->finished = 1;
				return NULL;
			}
		}
	}
	if (dp->is_w) {
		char *_tmp;

		_tmp = php_win32_ioutil_w_to_utf8(dp->fileinfo.w.cFileName);
		if (!_tmp) {
			/* wide to utf8 failed, should never happen. */
			return NULL;
		}
		strlcpy(dp->dent.d_name, _tmp, _MAX_FNAME+1);
		dp->dent.d_reclen = (unsigned short)strlen(dp->dent.d_name);
		free(_tmp);
	} else {
		strlcpy(dp->dent.d_name, dp->fileinfo.a.cFileName, _MAX_FNAME+1);
		dp->dent.d_reclen = (unsigned short)strlen(dp->dent.d_name);
	}
	dp->offset++;

	dp->dent.d_ino = 1;
	dp->dent.d_off = dp->offset;

	return &(dp->dent);
}

int readdir_r(DIR *dp, struct dirent *entry, struct dirent **result)
{
	if (!dp || dp->finished) {
		*result = NULL;
		return 0;
	}

	if (dp->offset != 0) {
		if (dp->is_w) {
			if (FindNextFileW(dp->handle, &(dp->fileinfo.w)) == 0) {
				dp->finished = 1;
				*result = NULL;
				return 0;
			}
		} else {
			if (FindNextFileA(dp->handle, &(dp->fileinfo.a)) == 0) {
				dp->finished = 1;
				*result = NULL;
				return 0;
			}
		}
	}

	if (dp->is_w) {
		char *_tmp;
		_tmp = php_win32_ioutil_w_to_utf8(dp->fileinfo.w.cFileName);
		if (!_tmp) {
			/* wide to utf8 failed, should never happen. */
			result = NULL;
			return 0;
		}
		strlcpy(dp->dent.d_name, _tmp, _MAX_FNAME+1);
		dp->dent.d_reclen = (unsigned short)strlen(dp->dent.d_name);
		free(_tmp);
	} else {
		strlcpy(dp->dent.d_name, dp->fileinfo.a.cFileName, _MAX_FNAME+1);
		dp->dent.d_reclen = (unsigned short)strlen(dp->dent.d_name);
	}
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
	if (dp->dira)
		free(dp->dira);
	if (dp->dirw)
		free(dp->dirw);
	if (dp)
		free(dp);

	return 0;
}

int rewinddir(DIR *dp)
{
	/* Re-set to the beginning */
	char *filespec;
	wchar_t *filespecw;
	HANDLE handle;
	int index;

	FindClose(dp->handle);

	dp->offset = 0;
	dp->finished = 0;

	if (dp->is_w) {
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

		if ((handle = FindFirstFileW(filespecw, &(dp->fileinfo.w))) == INVALID_HANDLE_VALUE) {
			dp->finished = 1;
		}

		free(filespecw);
	} else {
		filespec = (char *)malloc(strlen((char *)dp->dira) + 2 + 1);
		if (filespec == NULL) {
			return -1;
		}

		strcpy(filespec, (char *)dp->dira);
		index = (int)strlen(filespec) - 1;
		if (index >= 0 && (filespec[index] == '/' ||
		   (filespec[index] == '\\' && (index == 0 || !IsDBCSLeadByte(filespec[index-1])))))
			filespec[index] = '\0';
		strcat(filespec, "/*");

		if ((handle = FindFirstFile(filespec, &(dp->fileinfo.a))) == INVALID_HANDLE_VALUE) {
			dp->finished = 1;
		}

		free(filespec);
	}
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
