#ifndef READDIR_H
#define READDIR_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structures and types used to implement opendir/readdir/closedir
 * on Windows.
 */

#include <config.w32.h>

#include "ioutil.h"

/* struct dirent - same as Unix */
struct dirent {
	long d_ino;					/* inode (always 1 in WIN32) */
	off_t d_off;					/* offset to this dirent */
	unsigned short d_reclen;			/* length of d_name */
	char d_name[1];	/* null terminated filename in the current encoding, glyph number <= 255 wchar_t's + \0 byte */
};

/* typedef DIR - not the same as Unix */
struct DIR_W32 {
	HANDLE handle;			/* _findfirst/_findnext handle */
	uint16_t offset;		/* offset into directory */
	uint8_t finished;		/* 1 if there are not more files */
	WIN32_FIND_DATAW fileinfo;	/* from _findfirst/_findnext */
	wchar_t *dirw;			/* the dir we are reading */
	struct dirent dent;		/* the dirent to return */
};
typedef struct DIR_W32 DIR;

/* Function prototypes */
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int closedir(DIR *);
int rewinddir(DIR *);

#ifdef __cplusplus
}
#endif

#endif /* READDIR_H */
