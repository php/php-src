#ifndef READDIR_H
#define READDIR_H


/*
 * Structures and types used to implement opendir/readdir/closedir
 * on Windows 95/NT.
 */

#define _WIN32_WINNT 0x0400

#include <windows.h>

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <direct.h>

/* struct dirent - same as Unix */

struct dirent {
	long d_ino;					/* inode (always 1 in WIN32) */
	off_t d_off;				/* offset to this dirent */
	unsigned short d_reclen;	/* length of d_name */
	char d_name[_MAX_FNAME + 1];	/* filename (null terminated) */
};


/* typedef DIR - not the same as Unix */
typedef struct {
	HANDLE handle;				/* _findfirst/_findnext handle */
	short offset;				/* offset into directory */
	short finished;				/* 1 if there are not more files */
	WIN32_FIND_DATA fileinfo;	/* from _findfirst/_findnext */
	char *dir;					/* the dir we are reading */
	struct dirent dent;			/* the dirent to return */
} DIR;

/* Function prototypes */
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *, struct dirent *, struct dirent **);
int closedir(DIR *);
int rewinddir(DIR *);

#endif /* READDIR_H */
