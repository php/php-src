/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sander Steffann (sander@steffann.nl)                         |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>

#ifndef MAXSYMLINKS
#define MAXSYMLINKS		32
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode)&S_IFMT) == S_IFDIR)
#endif

char *php_realpath(char *path, char resolved_path[]);

char *php_realpath(char *path, char resolved_path []) {
	char path_construction[MAXPATHLEN]; 	/* We build the result in here */
	char *writepos;							/* Position to write next char */

	char path_copy[MAXPATHLEN];				/* A work-copy of the path */
	char *workpos;							/* working position in *path */

#if !defined(PHP_WIN32)
	char buf[MAXPATHLEN];					/* Buffer for readlink */
	int linklength;							/* The result from readlink */
#endif
	int linkcount = 0;						/* Count symlinks to avoid loops */
	
	struct stat filestat;					/* result from stat */

#ifdef PHP_WIN32
	char *temppos;						/* position while counting '.' */
	int dotcount;						/* number of '.' */
	int t;								/* counter */
#endif

	/* Set the work-position to the beginning of the given path */
	strcpy(path_copy, path);
	workpos = path_copy;
	
#ifdef PHP_WIN32
	/* Find out where we start - Windows version */
	if ((*workpos == '\\') || (*workpos == '/')) {
		/* We start at the root of the current drive */
		/* Get the current directory */
		if (V_GETCWD(path_construction, MAXPATHLEN-1) == NULL) {
			/* Unable to get cwd */
			resolved_path[0] = 0;
			return NULL;
		}
		/* We only need the first three chars (for example "C:\") */
		path_construction[3] = 0;
		workpos++;
	} else if (workpos[1] == ':') {
		/* A drive-letter is specified, copy it */
		strncpy(path_construction, path, 2);
		strcat(path_construction, "\\");
		workpos++;
		workpos++;
	} else {
		/* Use the current directory */
		if (V_GETCWD(path_construction, MAXPATHLEN-1) == NULL) {
			/* Unable to get cwd */
			resolved_path[0] = 0;
			return NULL;
		}
		strcat(path_construction, "\\");
	}
#else
	/* Find out where we start - Unix version */
	if (*workpos == '/') {
		/* We start at the root */
		strcpy(path_construction, "/");
		workpos++;
	} else {
		/* Use the current directory */
		if (V_GETCWD(path_construction, MAXPATHLEN-1) == NULL) {
			/* Unable to get cwd */
			resolved_path[0] = 0;
			return NULL;
		}
		strcat(path_construction, "/");
	}
#endif

	/* Set the next-char-position */
	writepos = &path_construction[strlen(path_construction)];

	/* Go to the end, then stop */
	while(*workpos != 0) {
		/* Strip (back)slashes */
#ifdef PHP_WIN32
		while(*workpos == '\\') workpos++;
#else
		while(*workpos == '/') workpos++;
#endif

#ifdef PHP_WIN32
		/* reset dotcount */
		dotcount = 0;

		/* Look for .. */
		if ((workpos[0] == '.') && (workpos[1] != 0)) {
			/* Windows accepts \...\ as \..\..\, \....\ as \..\..\..\, etc */
			/* At least Win98 does */
			
			temppos = workpos;
			while(*temppos++ == '.') {
				dotcount++;
				if ((*temppos != '\\') && (*temppos != 0) && (*temppos != '.')) {
					/* This is not a /../ component, but a filename that starts with '.' */
					dotcount = 0;
				}
			}
			
			/* Go back dotcount-1 times */
			for (t=0 ; t<(dotcount-1) ; t++) {
				workpos++;		/* move to next '.' */
				
				/* Can we still go back? */
				if ((writepos-3) <= path_construction) return NULL;

				/* Go back */
				writepos--;						/* move to '\' */
				while(*--writepos != '\\') ;	/* skip until previous '\\' */
			}
		}

		/* No special case */
		if (dotcount == 0) {
			/* Append */
			while((*workpos != '\\') && (*workpos != 0)) {
				*writepos++ = *workpos++;
			}
		}

		/* Just one '.', go to next element */
		if (dotcount == 1) {
			while((*workpos != '\\') && (*workpos != 0)) {
				*workpos++;
			}
			
			/* Avoid double \ in the result */
			writepos--;
		}
		
		/* If it was a directory, append a slash */
		if (*workpos == '\\') {
			*writepos++ = *workpos++;
		}
		*writepos = 0;
#else /* defined(PHP_WIN32) */
		/* Look for .. */
		if ((workpos[0] == '.') && (workpos[1] != 0)) {
			if ((workpos[1] == '.') && ((workpos[2] == '/') || (workpos[2] == 0))) {
				/* One directory back */
				/* Set pointers to right position */
				workpos++;						/* move to second '.' */
				workpos++;						/* move to '/' */
				
				/* Only apply .. if not in root */
				if ((writepos-1) > path_construction) {
					writepos--;						/* move to '/' */
					while(*--writepos != '/') ;		/* skip until previous '/' */
				}
			} else {
				if (workpos[1] == '/') {
					/* Found a /./ skip it */
					workpos++;					/* move to '/' */

					/* Avoid double / in the result */
					writepos--;
				} else {
					/* No special case, the name just started with a . */
					/* Append */
					while((*workpos != '/') && (*workpos != 0)) {
						*writepos++ = *workpos++;
					}
				}
			}
		} else {
			/* No special case */
			/* Append */
			while((*workpos != '/') && (*workpos != 0)) {
				*writepos++ = *workpos++;
			}
		}

#if HAVE_SYMLINK
		/* We are going to use path_construction, so close it */
		*writepos = 0;

		/* Check the current location to see if it is a symlink */
		if((linklength = readlink(path_construction, buf, MAXPATHLEN)) != -1) {
			/* Check linkcount */
			if (linkcount > MAXSYMLINKS) return NULL;

			/* Count this symlink */
			linkcount++;

			/* Set end of buf */
			buf[linklength] = 0;

			/* Check for overflow */
			if ((strlen(workpos) + strlen(buf) + 1) >= MAXPATHLEN) return NULL;

			/* Remove the symlink-component wrom path_construction */
			writepos--;						/* move to '/' */
			while(*--writepos != '/') ;		/* skip until previous '/' */
			*++writepos = 0;				/* end of string after '/' */

			/* If the symlink starts with a '/', empty path_construction */
			if (*buf == '/') {
				*path_construction = 0;
				writepos = path_construction;
			}

			/* Insert symlink into path_copy */
			strcat(buf, workpos);
			strcpy(path_copy, buf);
			workpos = path_copy;
		}
#endif /* HAVE_SYMLINK */

		/* If it was a directory, append a slash */
		if (*workpos == '/') {
			*writepos++ = *workpos++;
		}
		*writepos = 0;
#endif /* defined(PHP_WIN32) */
	}

	/* Check if the resolved path is a directory */
	if (V_STAT(path_construction, &filestat) != 0) {
		if (errno != ENOENT) return NULL;
	} else {
		if (S_ISDIR(filestat.st_mode)) {
			/* It's a directory, append a / if needed */
			if (*(writepos-1) != '/') {
				/* C	heck for overflow */
				if ((strlen(workpos) + 2) >= MAXPATHLEN) return NULL;
				
				*writepos++ = '/';
				*writepos = 0;
			}
		}
	}
	
	strcpy(resolved_path, path_construction);
	return resolved_path;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
