/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:  Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "safe_mode.h"
#include "fopen-wrappers.h"
#include "php_globals.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef OS2
#  define INCL_DOS
#  include <os2.h>
#endif

#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
# include <sys/statvfs.h>
#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_STATFS)
# include <sys/statfs.h>
#endif

#if HAVE_PWD_H
# if MSVC5
#  include "win32/pwd.h"
# else
#  include <pwd.h>
# endif
#endif

#if HAVE_GRP_H
# if MSVC5
#  include "win32/grp.h"
# else
#  include <grp.h>
# endif
#endif

#if HAVE_UTIME
# if MSVC5
#  include <sys/utime.h>
# else
#  include <utime.h>
# endif
#endif

#include "ext/standard/basic_functions.h"

#include "php_filestat.h"

#ifndef S_ISDIR
#define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode)	(((mode)&S_IFMT) == S_IFREG)
#endif
#ifndef S_ISLNK
#define S_ISLNK(mode)	(((mode)&S_IFMT) == S_IFLNK)
#endif


PHP_RINIT_FUNCTION(filestat)
{
	BLS_FETCH();

	BG(CurrentStatFile)=NULL;
	BG(CurrentStatLength)=0;
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(filestat)
{
	BLS_FETCH();

	if (BG(CurrentStatFile)) {
		efree (BG(CurrentStatFile));
	}
	return SUCCESS;
}

PHP_FUNCTION(diskfreespace)
{
	pval **path;
#ifdef WINDOWS
	double bytesfree;

	HINSTANCE kernel32;
	FARPROC gdfse;
	typedef BOOL (WINAPI *gdfse_func)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
	gdfse_func func;

	/* These are used by GetDiskFreeSpaceEx, if available. */
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
  	ULARGE_INTEGER TotalNumberOfFreeBytes;

	/* These are used by GetDiskFreeSpace otherwise. */
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;

#else /* not - WINDOWS */
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	struct statvfs buf;
#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_STATFS)
	struct statfs buf;
#endif
	double bytesfree = 0;
#endif /* WINDOWS */

	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1,&path)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(path);

	if (php_check_open_basedir((*path)->value.str.val)) RETURN_FALSE;

#ifdef WINDOWS
	/* GetDiskFreeSpaceEx is only available in NT and Win95 post-OSR2,
	   so we have to jump through some hoops to see if the function
	   exists. */
	kernel32 = LoadLibrary("kernel32.dll");
	if (kernel32) {
		gdfse = GetProcAddress(kernel32, "GetDiskFreeSpaceExA");
		/* It's available, so we can call it. */
		if (gdfse) {
			func = (gdfse_func)gdfse;
			if (func((*path)->value.str.val,
				&FreeBytesAvailableToCaller,
				&TotalNumberOfBytes,
				&TotalNumberOfFreeBytes) == 0) RETURN_FALSE;

			/* i know - this is ugly, but i works (thies@digicol.de) */
			bytesfree  = FreeBytesAvailableToCaller.HighPart * 
				(double) (((unsigned long)1) << 31) * 2.0 +
				FreeBytesAvailableToCaller.LowPart;
		}
		/* If it's not available, we just use GetDiskFreeSpace */
		else {
			if (GetDiskFreeSpace((*path)->value.str.val,
				&SectorsPerCluster, &BytesPerSector,
				&NumberOfFreeClusters, &TotalNumberOfClusters) == 0) RETURN_FALSE;
			bytesfree = (double)NumberOfFreeClusters * (double)SectorsPerCluster * (double)BytesPerSector;
		}
	}
	else {
		php_error(E_WARNING, "Unable to load kernel32.dll");
		RETURN_FALSE;
	}

#elif defined(OS2)
	{
		FSALLOCATE fsinfo;
  		char drive = (*path)->value.str.val[0] & 95;
  		
		if (DosQueryFSInfo( drive ? drive - 64 : 0, FSIL_ALLOC, &fsinfo, sizeof( fsinfo ) ) == 0)
			bytesfree = (double)fsinfo.cbSector * fsinfo.cSectorUnit * fsinfo.cUnitAvail;
	}
#else /* WINDOWS, OS/2 */
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	if (statvfs((*path)->value.str.val,&buf)) RETURN_FALSE;
	if (buf.f_frsize) {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_frsize));
	} else {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_bsize));
	}
#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_STATFS)
	if (statfs((*path)->value.str.val,&buf)) RETURN_FALSE;
	bytesfree = (((double)buf.f_bsize) * ((double)buf.f_bavail));
#endif
#endif /* WINDOWS */

	RETURN_DOUBLE(bytesfree);
}

PHP_FUNCTION(chgrp)
{
#ifndef WINDOWS
	pval **filename, **group;
	gid_t gid;
	struct group *gr=NULL;
	int ret;
	PLS_FETCH();

	if (ARG_COUNT(ht)!=2 || zend_get_parameters_ex(2,&filename,&group)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	if ((*group)->type == IS_STRING) {
		gr = getgrnam((*group)->value.str.val);
		if (!gr) {
			php_error(E_WARNING, "unable to find gid for %s",
					   (*group)->value.str.val);
			RETURN_FALSE;
		}
		gid = gr->gr_gid;
	} else {
		convert_to_long_ex(group);
		gid = (*group)->value.lval;
	}

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val,1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val))
		RETURN_FALSE;

	ret = chown((*filename)->value.str.val, -1, gid);
	if (ret == -1) {
		php_error(E_WARNING, "chgrp failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}


PHP_FUNCTION(chown)
{
#ifndef WINDOWS
	pval **filename, **user;
	int ret;
	uid_t uid;
	struct passwd *pw = NULL;
	PLS_FETCH();

	if (ARG_COUNT(ht)!=2 || zend_get_parameters_ex(2,&filename,&user)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	if ((*user)->type == IS_STRING) {
		pw = getpwnam((*user)->value.str.val);
		if (!pw) {
			php_error(E_WARNING, "unable to find uid for %s",
					   (*user)->value.str.val);
			RETURN_FALSE;
		}
		uid = pw->pw_uid;
	} else {
		convert_to_long_ex(user);
		uid = (*user)->value.lval;
	}

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val,1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val))
		RETURN_FALSE;

	ret = chown((*filename)->value.str.val, uid, -1);
	if (ret == -1) {
		php_error(E_WARNING, "chown failed: %s", strerror(errno));
		RETURN_FALSE;
	}
#endif
	RETURN_TRUE;
}


PHP_FUNCTION(chmod)
{
	pval **filename, **mode;
	int ret;
	PLS_FETCH();
	
	if (ARG_COUNT(ht)!=2 || zend_get_parameters_ex(2,&filename,&mode)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	convert_to_long_ex(mode);

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val,1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val))
		RETURN_FALSE;

	ret = chmod((*filename)->value.str.val, (*mode)->value.lval);
	if (ret == -1) {
		php_error(E_WARNING, "chmod failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}


PHP_FUNCTION(touch)
{
#if HAVE_UTIME
	pval **filename, **filetime;
	int ret;
	struct stat sb;
	FILE *file;
	struct utimbuf *newtime = NULL;
	int ac = ARG_COUNT(ht);
	PLS_FETCH();
	
	if (ac == 1 && zend_get_parameters_ex(1,&filename) != FAILURE) {
#ifndef HAVE_UTIME_NULL
		newtime = (struct utimbuf *)emalloc(sizeof(struct utimbuf));
		if (!newtime) {
			php_error(E_WARNING, "unable to emalloc memory for changing time");
			return;
		}
		newtime->actime = time(NULL);
		newtime->modtime = newtime->actime;
#endif
	} else if (ac == 2 && zend_get_parameters_ex(2,&filename,&filetime) != FAILURE) {
		newtime = (struct utimbuf *)emalloc(sizeof(struct utimbuf));
		if (!newtime) {
			php_error(E_WARNING, "unable to emalloc memory for changing time");
			return;
		}
		convert_to_long_ex(filetime);
		newtime->actime = (*filetime)->value.lval;
		newtime->modtime = (*filetime)->value.lval;
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val,1))) {
		if (newtime) efree(newtime);
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val))
		RETURN_FALSE;

	/* create the file if it doesn't exist already */
	ret = stat((*filename)->value.str.val, &sb);
	if (ret == -1) {
		file = fopen((*filename)->value.str.val, "w");
		if (file == NULL) {
			php_error(E_WARNING, "unable to create file %s because %s", (*filename)->value.str.val, strerror(errno));
			if (newtime) efree(newtime);
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = utime((*filename)->value.str.val, newtime);
	if (newtime) efree(newtime);
	if (ret == -1) {
		php_error(E_WARNING, "utime failed: %s", strerror(errno));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
#endif
}


PHP_FUNCTION(clearstatcache)
{
	BLS_FETCH();

	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
		BG(CurrentStatFile) = NULL;
	}
}


static void php_stat(const char *filename, int type, pval *return_value)
{
	struct stat *stat_sb;
	BLS_FETCH();

	stat_sb = &BG(sb);

	if (!BG(CurrentStatFile) || strcmp(filename,BG(CurrentStatFile))) {
		if (!BG(CurrentStatFile)
			|| strlen(filename) > BG(CurrentStatLength)) {
			if (BG(CurrentStatFile)) efree(BG(CurrentStatFile));
			BG(CurrentStatLength) = strlen(filename);
			BG(CurrentStatFile) = estrndup(filename,BG(CurrentStatLength));
		} else {
			strcpy(BG(CurrentStatFile),filename);
		}
#if HAVE_SYMLINK
		BG(lsb).st_mode = 0; /* mark lstat buf invalid */
#endif
		if (stat(BG(CurrentStatFile),&BG(sb))==-1) {
			if (type != 15 || errno != ENOENT) { /* fileexists() test must print no error */
				php_error(E_NOTICE,"stat failed for %s (errno=%d - %s)",BG(CurrentStatFile),errno,strerror(errno));
			}
			efree(BG(CurrentStatFile));
			BG(CurrentStatFile)=NULL;
			RETURN_FALSE;
		}
	}

#if HAVE_SYMLINK
	if (8 == type /* filetype */
		|| 14 == type /* is link */
		|| 16 == type) { /* lstat */

		/* do lstat if the buffer is empty */

		if (!BG(lsb).st_mode) {
			if (lstat(BG(CurrentStatFile),&BG(lsb)) == -1) {
				php_error(E_NOTICE,"lstat failed for %s (errno=%d - %s)",BG(CurrentStatFile),errno,strerror(errno));
				RETURN_FALSE;
			}
		}
	}
#endif

	switch(type) {
	case 0: /* fileperms */
		RETURN_LONG((long)BG(sb).st_mode);
	case 1: /* fileinode */
		RETURN_LONG((long)BG(sb).st_ino);
	case 2: /* filesize  */
		RETURN_LONG((long)BG(sb).st_size);
	case 3: /* fileowner */
		RETURN_LONG((long)BG(sb).st_uid);
	case 4: /* filegroup */
		RETURN_LONG((long)BG(sb).st_gid);
	case 5: /* fileatime */
		RETURN_LONG((long)BG(sb).st_atime);
	case 6: /* filemtime */
		RETURN_LONG((long)BG(sb).st_mtime);
	case 7: /* filectime */
		RETURN_LONG((long)BG(sb).st_ctime);
	case 8: /* filetype */
#if HAVE_SYMLINK
		if (S_ISLNK(BG(lsb).st_mode)) {
			RETURN_STRING("link",1);
		}
#endif
		switch(BG(sb).st_mode&S_IFMT) {
		case S_IFIFO: RETURN_STRING("fifo",1);
		case S_IFCHR: RETURN_STRING("char",1);
		case S_IFDIR: RETURN_STRING("dir",1);
		case S_IFBLK: RETURN_STRING("block",1);
		case S_IFREG: RETURN_STRING("file",1);
		}
		php_error(E_WARNING,"Unknown file type (%d)",BG(sb).st_mode&S_IFMT);
		RETURN_STRING("unknown",1);
	case 9: /*is writable*/
		RETURN_LONG((BG(sb).st_mode&S_IWRITE)!=0);
	case 10: /*is readable*/
		RETURN_LONG((BG(sb).st_mode&S_IREAD)!=0);
	case 11: /*is executable*/
		RETURN_LONG((BG(sb).st_mode&S_IEXEC)!=0 && !S_ISDIR(BG(sb).st_mode));
	case 12: /*is file*/
		RETURN_LONG(S_ISREG(BG(sb).st_mode));
	case 13: /*is dir*/
		RETURN_LONG(S_ISDIR(BG(sb).st_mode));
	case 14: /*is link*/
#if HAVE_SYMLINK
		RETURN_LONG(S_ISLNK(BG(lsb).st_mode));
#else
		RETURN_FALSE;
#endif
	case 15: /*file exists*/
		RETURN_TRUE; /* the false case was done earlier */
	case 16: /* lstat */
#if HAVE_SYMLINK
		stat_sb = &BG(lsb);
#endif
		/* FALLTHROUGH */
	case 17: /* stat */
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_next_index_long(return_value, stat_sb->st_dev);
		add_next_index_long(return_value, stat_sb->st_ino);
		add_next_index_long(return_value, stat_sb->st_mode);
		add_next_index_long(return_value, stat_sb->st_nlink);
		add_next_index_long(return_value, stat_sb->st_uid);
		add_next_index_long(return_value, stat_sb->st_gid);
#ifdef HAVE_ST_BLKSIZE
		add_next_index_long(return_value, stat_sb->st_rdev);
#else
		add_next_index_long(return_value, -1);
#endif
		add_next_index_long(return_value, stat_sb->st_size);
		add_next_index_long(return_value, stat_sb->st_atime);
		add_next_index_long(return_value, stat_sb->st_mtime);
		add_next_index_long(return_value, stat_sb->st_ctime);
#ifdef HAVE_ST_BLKSIZE
		add_next_index_long(return_value, stat_sb->st_blksize);
#else
		add_next_index_long(return_value, -1);
#endif
#ifdef HAVE_ST_BLOCKS
		add_next_index_long(return_value, stat_sb->st_blocks);
#else
		add_next_index_long(return_value, -1);
#endif
		return;
	}
	php_error(E_WARNING, "didn't understand stat call");
	RETURN_FALSE;
}

/* another quickie macro to make defining similar functions easier */
#define FileFunction(name, funcnum) \
void name(INTERNAL_FUNCTION_PARAMETERS) { \
	pval **filename; \
	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1,&filename) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} \
	convert_to_string_ex(filename); \
	php_stat((*filename)->value.str.val, funcnum, return_value); \
}

FileFunction(PHP_FN(fileperms),0)
FileFunction(PHP_FN(fileinode),1)
FileFunction(PHP_FN(filesize), 2)
FileFunction(PHP_FN(fileowner),3)
FileFunction(PHP_FN(filegroup),4)
FileFunction(PHP_FN(fileatime),5)
FileFunction(PHP_FN(filemtime),6)
FileFunction(PHP_FN(filectime),7)
FileFunction(PHP_FN(filetype), 8)
FileFunction(PHP_FN(is_writable), 9)
FileFunction(PHP_FN(is_readable),10)
FileFunction(PHP_FN(is_executable),11)
FileFunction(PHP_FN(is_file),12)
FileFunction(PHP_FN(is_dir),13)
FileFunction(PHP_FN(is_link),14)
FileFunction(PHP_FN(file_exists),15)
FileFunction(PHP_FN(lstat),16)
FileFunction(PHP_FN(stat),17)

function_entry php_filestat_functions[] = {
	PHP_FE(fileatime,								NULL)
	PHP_FE(filectime,								NULL)
	PHP_FE(filegroup,								NULL)
	PHP_FE(fileinode,								NULL)
	PHP_FE(filemtime,								NULL)
	PHP_FE(fileowner,								NULL)
	PHP_FE(fileperms,								NULL)
	PHP_FE(filesize,								NULL)
	PHP_FE(filetype,								NULL)
	PHP_FE(file_exists,								NULL)
	PHP_FE(is_writable,								NULL)
	PHP_FALIAS(is_writeable,	is_writable,		NULL)
	PHP_FE(is_readable,								NULL)
	PHP_FE(is_executable,							NULL)
	PHP_FE(is_file,									NULL)
	PHP_FE(is_dir,									NULL)
	PHP_FE(is_link,									NULL)
	PHP_FE(stat,									NULL)
	PHP_FE(lstat,									NULL)
	PHP_FE(chown,									NULL)
	PHP_FE(chgrp,									NULL)
	PHP_FE(chmod,									NULL)
	PHP_FE(touch,									NULL)
	PHP_FE(clearstatcache,							NULL)
	PHP_FE(diskfreespace,							NULL)
	{NULL, NULL, NULL}
};


zend_module_entry php_filestat_module_entry = {
	"PHP_filestat", php_filestat_functions, NULL, NULL, PHP_RINIT(filestat),
					PHP_RSHUTDOWN(filestat), NULL, STANDARD_MODULE_PROPERTIES
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
