/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Shane Caraveo <shane@caraveo.com>                            |
   |         Ilia Alshanetsky <ilia@prohost.org>                          |
   +----------------------------------------------------------------------+
 */

#include "php_scandir.h"
#include "php_config.h"

#ifndef HAVE_SCANDIR
 #include <sys/types.h>

 #ifdef HAVE_DIRENT_H
  #include <dirent.h>
 #endif

 #ifdef PHP_WIN32
  #include "win32/readdir.h"
 #endif  

 #include <stdlib.h>
 #include <search.h>
#endif

#ifndef HAVE_ALPHASORT
int alphasort(const struct dirent **a, const struct dirent **b)
{
	return strcoll((*a)->d_name,(*b)->d_name);
}
#endif

#ifndef HAVE_SCANDIR
int scandir(const char *dirname, struct dirent **namelist[], int (*selector) (const struct dirent *entry), int (*compare) (const struct dirent **a, const struct dirent **b))
{
	DIR *dirp = NULL;
	struct dirent **vector = NULL;
	struct dirent *dp = NULL;
	int vector_size = 0;
	int nfiles = 0;

	if (namelist == NULL) {
		return -1;
	}

	if (!(dirp = opendir(dirname))) {
		return -1;
	}

	while ((dp = readdir(dirp)) != NULL) {
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
		qsort (*namelist, nfiles, sizeof(struct dirent *), compare);
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
