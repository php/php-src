#include <malloc.h>
#include <string.h>
#include <errno.h>

#include "readdir.h"
#include "php.h"

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
	long handle;
	int index;

	filespec = malloc(strlen(dir) + 2 + 1);
	strcpy(filespec, dir);
	index = strlen(filespec) - 1;
	if (index >= 0 && (filespec[index] == '/' || 
	   (filespec[index] == '\\' && !IsDBCSLeadByte(filespec[index-1]))))
		filespec[index] = '\0';
	strcat(filespec, "/*");

	dp = (DIR *) malloc(sizeof(DIR));
	dp->offset = 0;
	dp->finished = 0;
	dp->dir = strdup(dir);

	if ((handle = _findfirst(filespec, &(dp->fileinfo))) < 0) {
		if (errno == ENOENT)
			dp->finished = 1;
		else
			return NULL;
	}
	dp->handle = handle;
	free(filespec);

	return dp;
}

struct dirent *readdir(DIR *dp)
{
	if (!dp || dp->finished)
		return NULL;

	if (dp->offset != 0) {
		if (_findnext(dp->handle, &(dp->fileinfo)) < 0) {
			dp->finished = 1;
			return NULL;
		}
	}
	dp->offset++;

	strlcpy(dp->dent.d_name, dp->fileinfo.name, _MAX_FNAME+1);
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
		if (_findnext(dp->handle, &(dp->fileinfo)) < 0) {
			dp->finished = 1;
			*result = NULL;
			return 0;
		}
	}
	dp->offset++;

	strlcpy(dp->dent.d_name, dp->fileinfo.name, _MAX_FNAME+1);
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
	_findclose(dp->handle);
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
	long handle;
	int index;

	_findclose(dp->handle);

	dp->offset = 0;
	dp->finished = 0;

	filespec = malloc(strlen(dp->dir) + 2 + 1);
	strcpy(filespec, dp->dir);
	index = strlen(filespec) - 1;
	if (index >= 0 && (filespec[index] == '/' || filespec[index] == '\\'))
		filespec[index] = '\0';
	strcat(filespec, "/*");

	if ((handle = _findfirst(filespec, &(dp->fileinfo))) < 0) {
		if (errno == ENOENT)
			dp->finished = 1;
		}
	dp->handle = handle;
	free(filespec);

return 0;
}

int alphasort(const struct dirent **a, const struct dirent **b)
{
	return strcoll((*a)->d_name,(*b)->d_name);
}

int scandir(const char *dirname,
			struct dirent **namelist[],
			int (*selector) (const struct dirent *entry),
			int (*compare) (const struct dirent **a, const struct dirent **b))
{
	DIR *dirp = NULL;
	struct dirent **vector = NULL;
	struct dirent *dp = NULL;
	int vector_size = 0;

	int nfiles = 0;
	int fail = 0;

	if (namelist == NULL)
		return -1;

	dirp = opendir(dirname);
	if (dirp == NULL)
		return -1;

	for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
	{
		int dsize = 0;
		struct dirent *newdp = NULL;

		if (selector && (*selector)(dp) == 0)
			continue;

		if (nfiles == vector_size)
		{
			struct dirent **newv;
			if (vector_size == 0)
				vector_size = 10;
			else
				vector_size *= 2;

			newv = (struct dirent **) realloc (vector, vector_size * sizeof (struct dirent *));
			if (newv == NULL)
			{
				fail = 1;
				break;
			}
			vector = newv;
		}

		dsize = sizeof (struct dirent) + ((strlen(dp->d_name) + 1) * sizeof(char));
		newdp = (struct dirent *) malloc(dsize);

		if (newdp == NULL)
		{
			fail = 1;
			break;
		}

		vector[nfiles++] = (struct dirent *) memcpy(newdp, dp, dsize);
	}

	closedir(dirp);

	if (fail)
	{
		while (nfiles-- > 0) free(vector[nfiles]);
		free(vector);
		return -1;
	}


	*namelist = vector;

	if (compare)
		qsort (*namelist,nfiles,sizeof (struct dirent *),compare);

	return nfiles;
}
