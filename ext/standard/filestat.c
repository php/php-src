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
   | Author:  Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "safe_mode.h"
#include "fopen_wrappers.h"
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

#if HAVE_SYS_VFS_H
# include <sys/vfs.h>
#endif

#ifdef OS2
#  define INCL_DOS
#  include <os2.h>
#endif

#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
# include <sys/statvfs.h>
#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_STATFS)
# include <sys/statfs.h>
#elif defined(HAVE_SYS_MOUNT_H) && defined(HAVE_STATFS)
# include <sys/mount.h>
#endif

#if HAVE_PWD_H
# ifdef PHP_WIN32
#  include "win32/pwd.h"
# elif defined(NETWARE)
#  include "netware/pwd.h"
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

#define S_IXROOT ( S_IXUSR | S_IXGRP | S_IXOTH )

/* Switches for various filestat functions: */
#define FS_PERMS    0
#define FS_INODE    1
#define FS_SIZE     2
#define FS_OWNER    3
#define FS_GROUP    4
#define FS_ATIME    5
#define FS_MTIME    6
#define FS_CTIME    7
#define FS_TYPE     8
#define FS_IS_W     9
#define FS_IS_R    10
#define FS_IS_X    11
#define FS_IS_FILE 12
#define FS_IS_DIR  13
#define FS_IS_LINK 14
#define FS_EXISTS  15
#define FS_LSTAT   16
#define FS_STAT    17


PHP_RINIT_FUNCTION(filestat)
{
	BG(CurrentStatFile)=NULL;
	BG(CurrentStatLength)=0;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(filestat) 
{
	if (BG(CurrentStatFile)) {
		efree (BG(CurrentStatFile));
	}
	return SUCCESS;
}

/* {{{ proto float disk_total_space(string path)
   Get total disk space for filesystem that path is on */
PHP_FUNCTION(disk_total_space)
{
	pval **path;
#ifdef WINDOWS
	double bytestotal;

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
#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	struct statfs buf;
#endif
	double bytestotal = 0;
#endif /* WINDOWS */

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &path)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(path);

	if (php_check_open_basedir(Z_STRVAL_PP(path) TSRMLS_CC)) {
		RETURN_FALSE;
	}

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
			if (func(Z_STRVAL_PP(path),
				&FreeBytesAvailableToCaller,
				&TotalNumberOfBytes,
				&TotalNumberOfFreeBytes) == 0) RETURN_FALSE;

			/* i know - this is ugly, but i works <thies@thieso.net> */
			bytestotal  = TotalNumberOfBytes.HighPart *
				(double) (((unsigned long)1) << 31) * 2.0 +
				TotalNumberOfBytes.LowPart;
		}
		/* If it's not available, we just use GetDiskFreeSpace */
		else {
			if (GetDiskFreeSpace(Z_STRVAL_PP(path),
				&SectorsPerCluster, &BytesPerSector,
				&NumberOfFreeClusters, &TotalNumberOfClusters) == 0) RETURN_FALSE;
			bytestotal = (double)TotalNumberOfClusters * (double)SectorsPerCluster * (double)BytesPerSector;
		}
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load kernel32.dll");
		RETURN_FALSE;
	}

#elif defined(OS2)
	{
		FSALLOCATE fsinfo;
		char drive = Z_STRVAL_PP(path)[0] & 95;

		if (DosQueryFSInfo( drive ? drive - 64 : 0, FSIL_ALLOC, &fsinfo, sizeof( fsinfo ) ) == 0)
			bytestotal = (double)fsinfo.cbSector * fsinfo.cSectorUnit * fsinfo.cUnit;
	}
#else /* WINDOWS, OS/2 */
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	if (statvfs(Z_STRVAL_PP(path), &buf)) RETURN_FALSE;
	if (buf.f_frsize) {
		bytestotal = (((double)buf.f_blocks) * ((double)buf.f_frsize));
	} else {
		bytestotal = (((double)buf.f_blocks) * ((double)buf.f_bsize));
	}

#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	if (statfs(Z_STRVAL_PP(path), &buf)) RETURN_FALSE;
	bytestotal = (((double)buf.f_bsize) * ((double)buf.f_blocks));
#endif
#endif /* WINDOWS */

	RETURN_DOUBLE(bytestotal);
}
/* }}} */

/* {{{ proto float disk_free_space(string path)
   Get free disk space for filesystem that path is on */
PHP_FUNCTION(disk_free_space)
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
#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	struct statfs buf;
#endif
	double bytesfree = 0;
#endif /* WINDOWS */

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &path)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(path);

	if (php_check_open_basedir(Z_STRVAL_PP(path) TSRMLS_CC)) {
		RETURN_FALSE;
	}

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
			if (func(Z_STRVAL_PP(path),
				&FreeBytesAvailableToCaller,
				&TotalNumberOfBytes,
				&TotalNumberOfFreeBytes) == 0) RETURN_FALSE;

			/* i know - this is ugly, but i works <thies@thieso.net> */
			bytesfree  = FreeBytesAvailableToCaller.HighPart *
				(double) (((unsigned long)1) << 31) * 2.0 +
				FreeBytesAvailableToCaller.LowPart;
		}
		/* If it's not available, we just use GetDiskFreeSpace */
		else {
			if (GetDiskFreeSpace(Z_STRVAL_PP(path),
				&SectorsPerCluster, &BytesPerSector,
				&NumberOfFreeClusters, &TotalNumberOfClusters) == 0) RETURN_FALSE;
			bytesfree = (double)NumberOfFreeClusters * (double)SectorsPerCluster * (double)BytesPerSector;
		}
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load kernel32.dll");
		RETURN_FALSE;
	}

#elif defined(OS2)
	{
		FSALLOCATE fsinfo;
		char drive = Z_STRVAL_PP(path)[0] & 95;

		if (DosQueryFSInfo( drive ? drive - 64 : 0, FSIL_ALLOC, &fsinfo, sizeof( fsinfo ) ) == 0)
			bytesfree = (double)fsinfo.cbSector * fsinfo.cSectorUnit * fsinfo.cUnitAvail;
	}
#else /* WINDOWS, OS/2 */
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	if (statvfs(Z_STRVAL_PP(path), &buf)) RETURN_FALSE;
	if (buf.f_frsize) {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_frsize));
	} else {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_bsize));
	}
#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	if (statfs(Z_STRVAL_PP(path), &buf)) RETURN_FALSE;
	bytesfree = (((double)buf.f_bsize) * ((double)buf.f_bavail));
#endif
#endif /* WINDOWS */

	RETURN_DOUBLE(bytesfree);
}
/* }}} */

/* {{{ proto bool chgrp(string filename, mixed group)
   Change file group */
PHP_FUNCTION(chgrp)
{
#if !defined(WINDOWS) && !defined(NETWARE)  /* I guess 'chgrp' won't be available on NetWare */
	pval **filename, **group;
	gid_t gid;
	struct group *gr=NULL;
	int ret;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &filename, &group)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	if (Z_TYPE_PP(group) == IS_STRING) {
		gr = getgrnam(Z_STRVAL_PP(group));
		if (!gr) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find gid for %s",
					   Z_STRVAL_PP(group));
			RETURN_FALSE;
		}
		gid = gr->gr_gid;
	} else {
		convert_to_long_ex(group);
		gid = Z_LVAL_PP(group);
	}

	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(filename), NULL, CHECKUID_ALLOW_FILE_NOT_EXISTS))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir(Z_STRVAL_PP(filename) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ret = VCWD_CHOWN(Z_STRVAL_PP(filename), -1, gid);
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
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
#if !defined(WINDOWS) && !defined(NETWARE)  /* I guess 'chown' won't be available on NetWare */
	pval **filename, **user;
	int ret;
	uid_t uid;
	struct passwd *pw = NULL;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &filename, &user)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	if (Z_TYPE_PP(user) == IS_STRING) {
		pw = getpwnam(Z_STRVAL_PP(user));
		if (!pw) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find uid for %s",
					   Z_STRVAL_PP(user));
			RETURN_FALSE;
		}
		uid = pw->pw_uid;
	} else {
		convert_to_long_ex(user);
		uid = Z_LVAL_PP(user);
	}

	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(filename), NULL, CHECKUID_ALLOW_FILE_NOT_EXISTS))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir(Z_STRVAL_PP(filename) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ret = VCWD_CHOWN(Z_STRVAL_PP(filename), uid, -1);
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
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
	int ret;
	mode_t imode;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &filename, &mode)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	convert_to_long_ex(mode);

	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(filename), NULL, CHECKUID_ALLOW_FILE_NOT_EXISTS))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir(Z_STRVAL_PP(filename) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	imode = (mode_t) Z_LVAL_PP(mode);
	/* in safe mode, do not allow to setuid files.
	   Setuiding files could allow users to gain privileges
	   that safe mode doesn't give them.
	*/
	if(PG(safe_mode))
		imode &= 0777;

	ret = VCWD_CHMOD(Z_STRVAL_PP(filename), imode);
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#if HAVE_UTIME
/* {{{ proto bool touch(string filename [, int time [, int atime]])
   Set modification time of file */
PHP_FUNCTION(touch)
{
	pval **filename, **filetime, **fileatime;
	int ret;
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	struct stat_libc sb;
#else
	struct stat sb;
#endif
	FILE *file;
	struct utimbuf newtimebuf;
	struct utimbuf *newtime = NULL;
	int ac = ZEND_NUM_ARGS();


	if (ac == 1 && zend_get_parameters_ex(1, &filename) != FAILURE) {
#ifndef HAVE_UTIME_NULL
		newtime = &newtimebuf;
		newtime->modtime = newtime->actime = time(NULL);
#endif
	} else if (ac == 2 && zend_get_parameters_ex(2, &filename, &filetime) != FAILURE) {
		convert_to_long_ex(filetime);
		newtime = &newtimebuf;
		newtime->modtime = newtime->actime = Z_LVAL_PP(filetime);
	} else if (ac == 3 && zend_get_parameters_ex(3, &filename, &filetime, &fileatime) != FAILURE) {
		convert_to_long_ex(fileatime);
		convert_to_long_ex(filetime);
		newtime = &newtimebuf;
		newtime->actime = Z_LVAL_PP(fileatime);
		newtime->modtime = Z_LVAL_PP(filetime);
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(filename), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir(Z_STRVAL_PP(filename) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	/* create the file if it doesn't exist already */
	ret = VCWD_STAT(Z_STRVAL_PP(filename), &sb);
	if (ret == -1) {
		file = VCWD_FOPEN(Z_STRVAL_PP(filename), "w");
		if (file == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create file %s because %s", Z_STRVAL_PP(filename), strerror(errno));
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = VCWD_UTIME(Z_STRVAL_PP(filename), newtime);
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Utime failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void clearstatcache(void)
   Clear file stat cache */
PHP_FUNCTION(clearstatcache)
{
	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
		BG(CurrentStatFile) = NULL;
	}
}
/* }}} */

#define IS_LINK_OPERATION(__t) ((__t) == FS_TYPE || (__t) == FS_IS_LINK || (__t) == FS_LSTAT)
#define IS_EXISTS_CHECK(__t) ((__t) == FS_EXISTS  || (__t) == FS_IS_W || (__t) == FS_IS_R || (__t) == FS_IS_X || (__t) == FS_IS_FILE || (__t) == FS_IS_DIR || (__t) == FS_IS_LINK)

/* {{{ php_stat
 */
static void php_stat(const char *filename, php_stat_len filename_length, int type, pval *return_value TSRMLS_DC)
{
	zval *stat_dev, *stat_ino, *stat_mode, *stat_nlink, *stat_uid, *stat_gid, *stat_rdev,
	 	*stat_size, *stat_atime, *stat_mtime, *stat_ctime, *stat_blksize, *stat_blocks;
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	struct stat_libc *stat_sb;
#else
	struct stat *stat_sb;
#endif
	int rmask=S_IROTH, wmask=S_IWOTH, xmask=S_IXOTH; /* access rights defaults to other */
	char *stat_sb_names[13]={"dev", "ino", "mode", "nlink", "uid", "gid", "rdev",
			      "size", "atime", "mtime", "ctime", "blksize", "blocks"};

	if (PG(safe_mode) &&(!php_checkuid_ex(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR, IS_EXISTS_CHECK(type) ? CHECKUID_NO_ERRORS : 0))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(filename TSRMLS_CC)) {
		RETURN_FALSE;
	}

#ifndef PHP_WIN32

	switch (type) {
		case FS_IS_W:
			RETURN_BOOL (!VCWD_ACCESS(filename, W_OK));
		case FS_IS_R:
			RETURN_BOOL (!VCWD_ACCESS(filename, R_OK));
		case FS_IS_X:
			RETURN_BOOL (!VCWD_ACCESS(filename, X_OK));
		case FS_EXISTS:
			RETURN_BOOL (!VCWD_ACCESS(filename, F_OK));
	}
#endif

	stat_sb = &BG(sb);

	if (!BG(CurrentStatFile) || strcmp(filename, BG(CurrentStatFile))) {
		if (!BG(CurrentStatFile) || filename_length > BG(CurrentStatLength)) {
			if (BG(CurrentStatFile)) {
				efree(BG(CurrentStatFile));
			}
			BG(CurrentStatLength) = filename_length;
			BG(CurrentStatFile) = estrndup(filename, filename_length);
		} else {
			memcpy(BG(CurrentStatFile), filename, filename_length+1);
		}
#if HAVE_SYMLINK
		BG(lsb).st_mode = 0; /* mark lstat buf invalid */
#endif
		if (VCWD_STAT(BG(CurrentStatFile), &BG(sb)) == -1) {
			if (!IS_LINK_OPERATION(type) && (!IS_EXISTS_CHECK(type) || errno != ENOENT)) { /* fileexists() test must print no error */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Stat failed for %s (errno=%d - %s)", BG(CurrentStatFile), errno, strerror(errno));
			}
			efree(BG(CurrentStatFile));
			BG(CurrentStatFile) = NULL;
			if (!IS_LINK_OPERATION(type)) { /* Don't require success for link operation */
				RETURN_FALSE;
			}
		}
	}

#if HAVE_SYMLINK
	if (IS_LINK_OPERATION(type) && !BG(lsb).st_mode) {
		/* do lstat if the buffer is empty */
		if (VCWD_LSTAT(filename, &BG(lsb)) == -1) {
			if (!IS_EXISTS_CHECK(type) || errno != ENOENT) { /* fileexists() test must print no error */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Lstat failed for %s (errno=%d - %s)", BG(CurrentStatFile), errno, strerror(errno));
			}
			RETURN_FALSE;
		}
	}
#endif


#ifndef NETWARE
	if (type >= FS_IS_W && type <= FS_IS_X) {
		if(BG(sb).st_uid==getuid()) {
			rmask=S_IRUSR;
			wmask=S_IWUSR;
			xmask=S_IXUSR;
		} else if(BG(sb).st_gid==getgid()) {
			rmask=S_IRGRP;
			wmask=S_IWGRP;
			xmask=S_IXGRP;
		} else {
			int   groups, n, i;
			gid_t *gids;

			groups = getgroups(0, NULL);
			if(groups) {
				gids=(gid_t *)emalloc(groups*sizeof(gid_t));
				n=getgroups(groups, gids);
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
	}
#endif

	switch (type) {
	case FS_PERMS:
		RETURN_LONG((long)BG(sb).st_mode);
	case FS_INODE:
		RETURN_LONG((long)BG(sb).st_ino);
	case FS_SIZE:
#if defined(NETWARE) && defined(NEW_LIBC)
		RETURN_LONG((long)(stat_sb->st_size));
#else
		RETURN_LONG((long)BG(sb).st_size);
#endif
	case FS_OWNER:
		RETURN_LONG((long)BG(sb).st_uid);
	case FS_GROUP:
		RETURN_LONG((long)BG(sb).st_gid);
	case FS_ATIME:
#if defined(NETWARE) && defined(NEW_LIBC)
		RETURN_LONG((long)((stat_sb->st_atime).tv_sec));
#else
		RETURN_LONG((long)BG(sb).st_atime);
#endif
	case FS_MTIME:
#if defined(NETWARE) && defined(NEW_LIBC)
		RETURN_LONG((long)((stat_sb->st_mtime).tv_sec));
#else
		RETURN_LONG((long)BG(sb).st_mtime);
#endif
	case FS_CTIME:
#if defined(NETWARE) && defined(NEW_LIBC)
		RETURN_LONG((long)((stat_sb->st_ctime).tv_sec));
#else
		RETURN_LONG((long)BG(sb).st_ctime);
#endif
	case FS_TYPE:
#if HAVE_SYMLINK
		if (S_ISLNK(BG(lsb).st_mode)) {
			RETURN_STRING("link", 1);
		}
#endif
		switch(BG(sb).st_mode&S_IFMT) {
		case S_IFIFO: RETURN_STRING("fifo", 1);
		case S_IFCHR: RETURN_STRING("char", 1);
		case S_IFDIR: RETURN_STRING("dir", 1);
		case S_IFBLK: RETURN_STRING("block", 1);
		case S_IFREG: RETURN_STRING("file", 1);
#if defined(S_IFSOCK) && !defined(ZEND_WIN32)&&!defined(__BEOS__)
		case S_IFSOCK: RETURN_STRING("socket", 1);
#endif
		}
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown file type (%d)", BG(sb).st_mode&S_IFMT);
		RETURN_STRING("unknown", 1);
	case FS_IS_W:
#ifndef NETWARE	/* getuid is not available on NetWare */
		if (getuid()==0) {
			RETURN_TRUE; /* root */
		}
#endif	/* NETWARE */
		RETURN_BOOL((BG(sb).st_mode & wmask) != 0);
	case FS_IS_R:
#ifndef NETWARE	/* getuid is not available on NetWare */
		if (getuid()==0) {
			RETURN_TRUE; /* root */
		}
#endif	/* NETWARE */
		RETURN_BOOL((BG(sb).st_mode&rmask)!=0);
	case FS_IS_X:
#ifndef NETWARE	/* getuid is not available on NetWare */
		if (getuid()==0) {
			xmask = S_IXROOT; /* root */
		}
#endif	/* NETWARE */
		RETURN_BOOL((BG(sb).st_mode&xmask)!=0 && !S_ISDIR(BG(sb).st_mode));
	case FS_IS_FILE:
		RETURN_BOOL(S_ISREG(BG(sb).st_mode));
	case FS_IS_DIR:
		RETURN_BOOL(S_ISDIR(BG(sb).st_mode));
	case FS_IS_LINK:
#if HAVE_SYMLINK
		RETURN_BOOL(S_ISLNK(BG(lsb).st_mode));
#else
		RETURN_FALSE;
#endif
	case FS_EXISTS:
		RETURN_TRUE; /* the false case was done earlier */
	case FS_LSTAT:
#if HAVE_SYMLINK
		stat_sb = &BG(lsb);
#endif
		/* FALLTHROUGH */
	case FS_STAT:
		array_init(return_value);

		MAKE_LONG_ZVAL_INCREF(stat_dev, stat_sb->st_dev);
		MAKE_LONG_ZVAL_INCREF(stat_ino, stat_sb->st_ino);
		MAKE_LONG_ZVAL_INCREF(stat_mode, stat_sb->st_mode);
		MAKE_LONG_ZVAL_INCREF(stat_nlink, stat_sb->st_nlink);
		MAKE_LONG_ZVAL_INCREF(stat_uid, stat_sb->st_uid);
		MAKE_LONG_ZVAL_INCREF(stat_gid, stat_sb->st_gid);
#ifdef HAVE_ST_RDEV
		MAKE_LONG_ZVAL_INCREF(stat_rdev, stat_sb->st_rdev); 
#else
		MAKE_LONG_ZVAL_INCREF(stat_rdev, -1); 
#endif
		MAKE_LONG_ZVAL_INCREF(stat_size, stat_sb->st_size);
#if defined(NETWARE) && defined(NEW_LIBC)
		MAKE_LONG_ZVAL_INCREF(stat_atime, (stat_sb->st_atime).tv_sec);
		MAKE_LONG_ZVAL_INCREF(stat_mtime, (stat_sb->st_mtime).tv_sec);
		MAKE_LONG_ZVAL_INCREF(stat_ctime, (stat_sb->st_ctime).tv_sec);
#else
		MAKE_LONG_ZVAL_INCREF(stat_atime, stat_sb->st_atime);
		MAKE_LONG_ZVAL_INCREF(stat_mtime, stat_sb->st_mtime);
		MAKE_LONG_ZVAL_INCREF(stat_ctime, stat_sb->st_ctime);
#endif
#ifdef HAVE_ST_BLKSIZE
		MAKE_LONG_ZVAL_INCREF(stat_blksize, stat_sb->st_blksize); 
#else
		MAKE_LONG_ZVAL_INCREF(stat_blksize,-1);
#endif
#ifdef HAVE_ST_BLOCKS
		MAKE_LONG_ZVAL_INCREF(stat_blocks, stat_sb->st_blocks);
#else
		MAKE_LONG_ZVAL_INCREF(stat_blocks,-1);
#endif
		/* Store numeric indexes in propper order */
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_dev, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_ino, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_mode, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_nlink, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_uid, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_gid, sizeof(zval *), NULL);
        
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_rdev, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_size, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_atime, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_mtime, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_ctime, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_blksize, sizeof(zval *), NULL);
		zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_blocks, sizeof(zval *), NULL);

		/* Store string indexes referencing the same zval*/
		zend_hash_update(HASH_OF(return_value), stat_sb_names[0], strlen(stat_sb_names[0])+1, (void *) &stat_dev, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[1], strlen(stat_sb_names[1])+1, (void *) &stat_ino, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[2], strlen(stat_sb_names[2])+1, (void *) &stat_mode, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[3], strlen(stat_sb_names[3])+1, (void *) &stat_nlink, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[4], strlen(stat_sb_names[4])+1, (void *) &stat_uid, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[5], strlen(stat_sb_names[5])+1, (void *) &stat_gid, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[6], strlen(stat_sb_names[6])+1, (void *) &stat_rdev, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[7], strlen(stat_sb_names[7])+1, (void *) &stat_size, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[8], strlen(stat_sb_names[8])+1, (void *) &stat_atime, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[9], strlen(stat_sb_names[9])+1, (void *) &stat_mtime, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[10], strlen(stat_sb_names[10])+1, (void *) &stat_ctime, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[11], strlen(stat_sb_names[11])+1, (void *) &stat_blksize, sizeof(zval *), NULL);
		zend_hash_update(HASH_OF(return_value), stat_sb_names[12], strlen(stat_sb_names[12])+1, (void *) &stat_blocks, sizeof(zval *), NULL);

		return;
	}
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Didn't understand stat call");
	RETURN_FALSE;
}
/* }}} */

/* another quickie macro to make defining similar functions easier */
#define FileFunction(name, funcnum) \
void name(INTERNAL_FUNCTION_PARAMETERS) { \
	pval **filename; \
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} \
	convert_to_string_ex(filename); \
	php_stat(Z_STRVAL_PP(filename), (php_stat_len) Z_STRLEN_PP(filename), funcnum, return_value TSRMLS_CC); \
}

/* {{{ proto int fileperms(string filename)
   Get file permissions */
FileFunction(PHP_FN(fileperms), FS_PERMS)
/* }}} */

/* {{{ proto int fileinode(string filename)
   Get file inode */
FileFunction(PHP_FN(fileinode), FS_INODE)
/* }}} */

/* {{{ proto int filesize(string filename)
   Get file size */
FileFunction(PHP_FN(filesize), FS_SIZE)
/* }}} */

/* {{{ proto int fileowner(string filename)
   Get file owner */
FileFunction(PHP_FN(fileowner), FS_OWNER)
/* }}} */

/* {{{ proto int filegroup(string filename)
   Get file group */
FileFunction(PHP_FN(filegroup), FS_GROUP)
/* }}} */

/* {{{ proto int fileatime(string filename)
   Get last access time of file */
FileFunction(PHP_FN(fileatime), FS_ATIME)
/* }}} */

/* {{{ proto int filemtime(string filename)
   Get last modification time of file */
FileFunction(PHP_FN(filemtime), FS_MTIME)
/* }}} */

/* {{{ proto int filectime(string filename)
   Get inode modification time of file */
FileFunction(PHP_FN(filectime), FS_CTIME)
/* }}} */

/* {{{ proto string filetype(string filename)
   Get file type */
FileFunction(PHP_FN(filetype), FS_TYPE)
/* }}} */

/* {{{ proto bool is_writable(string filename)
   Returns true if file can be written */
FileFunction(PHP_FN(is_writable), FS_IS_W)
/* }}} */

/* {{{ proto bool is_readable(string filename)
   Returns true if file can be read */
FileFunction(PHP_FN(is_readable), FS_IS_R)
/* }}} */

/* {{{ proto bool is_executable(string filename)
   Returns true if file is executable */
FileFunction(PHP_FN(is_executable), FS_IS_X)
/* }}} */

/* {{{ proto bool is_file(string filename)
   Returns true if file is a regular file */
FileFunction(PHP_FN(is_file), FS_IS_FILE)
/* }}} */

/* {{{ proto bool is_dir(string filename)
   Returns true if file is directory */
FileFunction(PHP_FN(is_dir), FS_IS_DIR)
/* }}} */

/* {{{ proto bool is_link(string filename)
   Returns true if file is symbolic link */
FileFunction(PHP_FN(is_link), FS_IS_LINK)
/* }}} */

/* {{{ proto bool file_exists(string filename)
   Returns true if filename exists */
FileFunction(PHP_FN(file_exists), FS_EXISTS)
/* }}} */

/* {{{ proto array lstat(string filename)
   Give information about a file or symbolic link */
FileFunction(php_if_lstat, FS_LSTAT)
/* }}} */

/* {{{ proto array stat(string filename)
   Give information about a file */
FileFunction(php_if_stat, FS_STAT)
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
