/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Shane Caraveo <shane@caraveo.com>                            |
   |         Ilia Alshanetsky <ilia@prohost.org>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_scandir.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifndef HAVE_SCANDIR

#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/readdir.h"
#endif

#include <stdlib.h>
#include <search.h>

#endif /* HAVE_SCANDIR */

#ifndef HAVE_ALPHASORT

#include <string.h>

PHPAPI int php_alphasort(const struct dirent **a, const struct dirent **b)
{
	return strcoll((*a)->d_name,(*b)->d_name);
}
#endif /* HAVE_ALPHASORT */

#ifndef HAVE_SCANDIR
PHPAPI int php_scandir(const char *dirname, struct dirent **namelist[], int (*selector) (const struct dirent *entry), int (*compare) (const struct dirent **a, const struct dirent **b))
{
	DIR *dirp = NULL;
	struct dirent **vector = NULL;
	int vector_size = 0;
	int nfiles = 0;
	struct dirent *dp;

	if (namelist == NULL) {
		return -1;
	}

	if (!(dirp = opendir(dirname))) {
		return -1;
	}

	while ((dp = readdir(dirp))) {
		size_t dsize = 0;
		struct dirent *newdp = NULL;

		if (selector && (*selector)(dp) == 0) {
			continue;
		}

		if (nfiles == vector_size) {
			struct dirent **newv;
			if (vector_size == 0) {
				vector_size = 10;
			} else {
				vector_size *= 2;
			}

			newv = (struct dirent **) realloc (vector, vector_size * sizeof (struct dirent *));
			if (!newv) {
				goto fail;
			}
			vector = newv;
		}

		dsize = sizeof (struct dirent) + ((strlen(dp->d_name) + 1) * sizeof(char));
		newdp = (struct dirent *) malloc(dsize);

		if (newdp == NULL) {
			goto fail;
		}

		vector[nfiles++] = (struct dirent *) memcpy(newdp, dp, dsize);
	}

	closedir(dirp);

	*namelist = vector;

	if (compare) {
		qsort (*namelist, nfiles, sizeof(struct dirent *), (int (*) (const void *, const void *)) compare);
	}

	return nfiles;

fail:
	while (nfiles-- > 0) {
		free(vector[nfiles]);
	}
	free(vector);
	closedir(dirp);
	return -1;
}
#endif
