#include <malloc.h>
#include <string.h>
#include <errno.h>

#include "php.h"
#include "readdir.h"

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

DIR *opendir(const char *dir)
{
	DIR *dp;
	char *filespec;
	HANDLE handle;
	int index;

	filespec = (char *)malloc(strlen(dir) + 2 + 1);
	strcpy(filespec, dir);
	index = strlen(filespec) - 1;
	if (index >= 0 && (filespec[index] == '/' || 
	   (filespec[index] == '\\' && (index == 0 || !IsDBCSLeadByte(filespec[index-1])))))
		filespec[index] = '\0';
	strcat(filespec, "/*");

	dp = (DIR *) malloc(sizeof(DIR));
	dp->offset = 0;
	dp->finished = 0;

	if ((handle = FindFirstFile(filespec, &(dp->fileinfo))) == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		if (err == ERROR_NO_MORE_FILES) {
			dp->finished = 1;
		} else {
			free(dp);
			free(filespec);
			return NULL;
		}
	}
	dp->dir = strdup(dir);
	dp->handle = handle;
	free(filespec);

	return dp;
}

struct dirent *readdir(DIR *dp)
{
	if (!dp || dp->finished)
		return NULL;

	if (dp->offset != 0) {
		if (FindNextFile(dp->handle, &(dp->fileinfo)) == 0) {
			dp->finished = 1;
			return NULL;
		}
	}
	dp->offset++;

	strlcpy(dp->dent.d_name, dp->fileinfo.cFileName, _MAX_FNAME+1);
	dp->dent.d_ino = 1;
	dp->dent.d_reclen = strlen(dp->dent.d_name);
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
		if (FindNextFile(dp->handle, &(dp->fileinfo)) == 0) {
			dp->finished = 1;
			*result = NULL;
			return 0;
		}
	}
	dp->offset++;

	strlcpy(dp->dent.d_name, dp->fileinfo.cFileName, _MAX_FNAME+1);
	dp->dent.d_ino = 1;
	dp->dent.d_reclen = strlen(dp->dent.d_name);
	dp->dent.d_off = dp->offset;

	memcpy(entry, &dp->dent, sizeof(*entry));

	*result = &dp->dent;

	return 0;
}

int closedir(DIR *dp)
{
	if (!dp)
		return 0;
	FindClose(dp->handle);
	if (dp->dir)
		free(dp->dir);
	if (dp)
		free(dp);

	return 0;
}

int rewinddir(DIR *dp)
{
	/* Re-set to the beginning */
	char *filespec;
	HANDLE handle;
	int index;

	FindClose(dp->handle);

	dp->offset = 0;
	dp->finished = 0;

	filespec = (char *)malloc(strlen(dp->dir) + 2 + 1);
	strcpy(filespec, dp->dir);
	index = strlen(filespec) - 1;
	if (index >= 0 && (filespec[index] == '/' || 
	   (filespec[index] == '\\' && (index == 0 || !IsDBCSLeadByte(filespec[index-1])))))
		filespec[index] = '\0';
	strcat(filespec, "/*");

	if ((handle = FindFirstFile(filespec, &(dp->fileinfo))) == INVALID_HANDLE_VALUE) {
		dp->finished = 1;
	}
	
	dp->handle = handle;
	free(filespec);

	return 0;
}
