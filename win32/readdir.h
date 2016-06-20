#ifndef READDIR_H
#define READDIR_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structures and types used to implement opendir/readdir/closedir
 * on Windows 95/NT.
 */

#include <config.w32.h>

#include <stdlib.h>
#include <sys/types.h>

#define php_readdir_r readdir_r

/* struct dirent - same as Unix */
struct dirent {
	long d_ino;					/* inode (always 1 in WIN32) */
	off_t d_off;				/* offset to this dirent */
	unsigned short d_reclen;	/* length of d_name */
	char d_name[_MAX_FNAME + 1];	/* filename (null terminated) */
};

/* typedef DIR - not the same as Unix */
typedef struct DIR_W32 DIR;

/* Function prototypes */
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *, struct dirent *, struct dirent **);
int closedir(DIR *);
int rewinddir(DIR *);

#ifdef __cplusplus
}
#endif

#endif /* READDIR_H */
