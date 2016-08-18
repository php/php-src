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
   | Author: Shane Caraveo <shane@caraveo.com>                            |
   |         Ilia Alshanetsky <ilia@prohost.org>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

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
#ifndef NETWARE
#include <search.h>
#endif

#endif /* HAVE_SCANDIR */

#ifndef HAVE_ALPHASORT

#ifdef HAVE_STRING_H
#include <string.h>
#endif

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
	char entry[sizeof(struct dirent)+MAXPATHLEN];
	struct dirent *dp = (struct dirent *)&entry;

	if (namelist == NULL) {
		return -1;
	}

	if (!(dirp = opendir(dirname))) {
		return -1;
	}

	while (!php_readdir_r(dirp, (struct dirent *)entry, &dp) && dp) {
		int dsize = 0;
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
				return -1;
			}
			vector = newv;
		}

		dsize = sizeof (struct dirent) + (((int)strlen(dp->d_name) + 1) * sizeof(char));
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
	return -1;
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
