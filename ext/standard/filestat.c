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
# ifdef PHP_WIN32
#  include "win32/pwd.h"
# else
#  include <pwd.h>
# endif
#endif

#if HAVE_GRP_H
# ifdef PHP_WIN32
#  include "win32/grp.h"
# else
#  include <grp.h>
# endif
#endif

#if HAVE_UTIME
# ifdef PHP_WIN32
#  include <sys/utime.h>
# else
#  include <utime.h>
# endif
#endif

#include "basic_functions.h"

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

#ifdef PHP_WIN32
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC

#define S_IRGRP S_IREAD
#define S_IWGRP S_IWRITE
#define S_IXGRP S_IEXEC

#define S_IROTH S_IREAD
#define S_IWOTH S_IWRITE
#define S_IXOTH S_IEXEC

#undef getgid
#define getgroups(a,b) 0
#define getgid() 1
#define getuid() 1
#endif

#define S_IXROOT ( S_IXUSR | S_IXGRP | S_IXOTH )

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

/* {{{ proto double diskfreespace(string path)
   Get free diskspace for filesystem that path is on */
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

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1,&path)==FAILURE) {
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
/* }}} */

/* {{{ proto bool chown(string filename, mixed user)
   Change file owner */
PHP_FUNCTION(chgrp)
{
#ifndef WINDOWS
	pval **filename, **group;
	gid_t gid;
	struct group *gr=NULL;
	int ret;
	PLS_FETCH();

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2,&filename,&group)==FAILURE) {
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

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val, NULL, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val))
		RETURN_FALSE;

	ret = V_CHOWN((*filename)->value.str.val, -1, gid);
	if (ret == -1) {
		php_error(E_WARNING, "chgrp failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool chown (string filename, mixed user)
   Change file owner */
PHP_FUNCTION(chown)
{
#ifndef WINDOWS
	pval **filename, **user;
	int ret;
	uid_t uid;
	struct passwd *pw = NULL;
	PLS_FETCH();

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2,&filename,&user)==FAILURE) {
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

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val, NULL, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val))
		RETURN_FALSE;

	ret = V_CHOWN((*filename)->value.str.val, uid, -1);
	if (ret == -1) {
		php_error(E_WARNING, "chown failed: %s", strerror(errno));
		RETURN_FALSE;
	}
#endif
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool chmod(string filename, int mode)
   Change file mode */
PHP_FUNCTION(chmod)
{
	pval **filename, **mode;
	int ret,imode;
	PLS_FETCH();

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2,&filename,&mode)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	convert_to_long_ex(mode);

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val, NULL, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val))
		RETURN_FALSE;

	imode = (*mode)->value.lval;
	/* in safe mode, do not allow to setuid files.
	   Setuiding files could allow users to gain privileges
	   that safe mode doesn't give them.
	*/
	if(PG(safe_mode))
	  imode &= 0777;

	ret = V_CHMOD((*filename)->value.str.val, imode);
	if (ret == -1) {
		php_error(E_WARNING, "chmod failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool touch(string filename [, int time])
   Set modification time of file */
PHP_FUNCTION(touch)
{
#if HAVE_UTIME
	pval **filename, **filetime;
	int ret;
	struct stat sb;
	FILE *file;
	struct utimbuf *newtime = NULL;
	int ac = ZEND_NUM_ARGS();
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

	if (PG(safe_mode) &&(!php_checkuid((*filename)->value.str.val, NULL, 2))) {
		if (newtime) efree(newtime);
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir((*filename)->value.str.val)) {
		if (newtime) efree(newtime);
		RETURN_FALSE;
	}

	/* create the file if it doesn't exist already */
	ret = V_STAT((*filename)->value.str.val, &sb);
	if (ret == -1) {
		file = V_FOPEN((*filename)->value.str.val, "w");
		if (file == NULL) {
			php_error(E_WARNING, "unable to create file %s because %s", (*filename)->value.str.val, strerror(errno));
			if (newtime) efree(newtime);
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = V_UTIME((*filename)->value.str.val, newtime);
	if (newtime) efree(newtime);
	if (ret == -1) {
		php_error(E_WARNING, "utime failed: %s", strerror(errno));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
#endif
}
/* }}} */


/* {{{ proto void clearstatcache(void)
   Clear file stat cache */
PHP_FUNCTION(clearstatcache)
{
	BLS_FETCH();

	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
		BG(CurrentStatFile) = NULL;
	}
}
/* }}} */


static void php_stat(const char *filename, int type, pval *return_value)
{
	struct stat *stat_sb;
	int rmask=S_IROTH,wmask=S_IWOTH,xmask=S_IXOTH; /* access rights defaults to other */
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
		if (V_STAT(BG(CurrentStatFile),&BG(sb))==-1) {
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
			if (V_LSTAT(BG(CurrentStatFile),&BG(lsb)) == -1) {
				php_error(E_NOTICE,"lstat failed for %s (errno=%d - %s)",BG(CurrentStatFile),errno,strerror(errno));
				RETURN_FALSE;
			}
		}
	}
#endif


	if(BG(sb).st_uid==getuid()) {
		rmask=S_IRUSR;
		wmask=S_IWUSR;
		xmask=S_IXUSR;
	} else if(BG(sb).st_gid==getgid()) {
		rmask=S_IRGRP;
		wmask=S_IWGRP;
		xmask=S_IXGRP;
	} else {
		int   groups,n,i;
		gid_t *gids;

		groups = getgroups(0,NULL);
		if(groups) {
			gids=(gid_t *)emalloc(groups*sizeof(gid_t));
			n=getgroups(groups,gids);
			for(i=0;i<n;i++){
				if(BG(sb).st_gid==gids[i]) {
					rmask=S_IRGRP;
					wmask=S_IWGRP;
					xmask=S_IXGRP;
					break;
				}
			}
			efree(gids);
		}
	}

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
		if(getuid()==0) RETURN_LONG(1); /* root */
		RETURN_LONG((BG(sb).st_mode&wmask)!=0);
	case 10: /*is readable*/
		if(getuid()==0) RETURN_LONG(1); /* root */
		RETURN_LONG((BG(sb).st_mode&rmask)!=0);
	case 11: /*is executable*/
		if(getuid()==0) xmask = S_IXROOT; /* root */
		RETURN_LONG((BG(sb).st_mode&xmask)!=0 && !S_ISDIR(BG(sb).st_mode));
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
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1,&filename) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} \
	convert_to_string_ex(filename); \
	if ((*filename)->value.str.len) \
		php_stat((*filename)->value.str.val, funcnum, return_value); \
}

/* {{{ proto int fileperms(string filename)
   Get file permissions */
FileFunction(PHP_FN(fileperms),0)
/* }}} */

/* {{{ proto int fileinode(string filename)
   Get file inode */
FileFunction(PHP_FN(fileinode),1)
/* }}} */

/* {{{ proto int filesize(string filename)
   Get file size */
FileFunction(PHP_FN(filesize), 2)
/* }}} */

/* {{{ proto int fileowner(string filename)
   Get file owner */
FileFunction(PHP_FN(fileowner),3)
/* }}} */

/* {{{ proto nt filegroup(string filename)
   Get file group */
FileFunction(PHP_FN(filegroup),4)
/* }}} */

/* {{{ proto int fileatime(string filename)
   Get last access time of file */
FileFunction(PHP_FN(fileatime),5)
/* }}} */

/* {{{ proto int filemtime(string filename)
   Get last modification time of file */
FileFunction(PHP_FN(filemtime),6)
/* }}} */

/* {{{ proto int filectime(string filename)
   Get inode modification time of file */
FileFunction(PHP_FN(filectime),7)
/* }}} */

/* {{{ proto string filetype(string filename)
   Get file type */
FileFunction(PHP_FN(filetype), 8)
/* }}} */

/* {{{ proto int is_writable(string filename)
   Returns true if file can be written */
FileFunction(PHP_FN(is_writable), 9)
/* }}} */

/* {{{ proto int is_readable(string filename)
   Returns true if file can be read */
FileFunction(PHP_FN(is_readable),10)
/* }}} */

/* {{{ proto int is_executable(string filename)
   Returns true if file is executable */
FileFunction(PHP_FN(is_executable),11)
/* }}} */

/* {{{ proto int is_file(string filename)
   Returns true if file is a regular file */
FileFunction(PHP_FN(is_file),12)
/* }}} */

/* {{{ proto int is_dir(string filename)
   Returns true if file is directory */
FileFunction(PHP_FN(is_dir),13)
/* }}} */

/* {{{ proto int is_link(string filename)
   Returns true if file is symbolic link */
FileFunction(PHP_FN(is_link),14)
/* }}} */

/* {{{ proto bool file_exists(string filename)
   Returns true if filename exists */
FileFunction(PHP_FN(file_exists),15)
/* }}} */

/* {{{ proto array lstat(string filename)
   Give information about a file or symbolic link */
FileFunction(PHP_FN(lstat),16)
/* }}} */

/* {{{ proto array stat(string filename)
   Give information about a file */
FileFunction(PHP_FN(stat),17)
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
