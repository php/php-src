/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:  Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "fopen_wrappers.h"
#include "php_globals.h"
#include "ext/standard/file.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_SYS_PARAM_H
# include <sys/param.h>
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

#ifdef PHP_WIN32
#include "win32/winutil.h"
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

PHP_RINIT_FUNCTION(filestat) /* {{{ */
{
	BG(CurrentStatFile)=NULL;
	BG(CurrentLStatFile)=NULL;
	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(filestat) /* {{{ */
{
	if (BG(CurrentStatFile)) {
		efree (BG(CurrentStatFile));
		BG(CurrentStatFile) = NULL;
	}
	if (BG(CurrentLStatFile)) {
		efree (BG(CurrentLStatFile));
		BG(CurrentLStatFile) = NULL;
	}
	return SUCCESS;
}
/* }}} */

static int php_disk_total_space(char *path, double *space TSRMLS_DC) /* {{{ */
#if defined(WINDOWS) /* {{{ */
{
	double bytestotal = 0;
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

	/* GetDiskFreeSpaceEx is only available in NT and Win95 post-OSR2,
	   so we have to jump through some hoops to see if the function
	   exists. */
	kernel32 = LoadLibrary("kernel32.dll");
	if (kernel32) {
		gdfse = GetProcAddress(kernel32, "GetDiskFreeSpaceExA");
		/* It's available, so we can call it. */
		if (gdfse) {
			func = (gdfse_func)gdfse;
			if (func(path,
						&FreeBytesAvailableToCaller,
						&TotalNumberOfBytes,
						&TotalNumberOfFreeBytes) == 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", php_win_err());
				return FAILURE;
			}

			/* i know - this is ugly, but i works <thies@thieso.net> */
			bytestotal  = TotalNumberOfBytes.HighPart *
				(double) (((unsigned long)1) << 31) * 2.0 +
				TotalNumberOfBytes.LowPart;
		} else { /* If it's not available, we just use GetDiskFreeSpace */
			if (GetDiskFreeSpace(path,
						&SectorsPerCluster, &BytesPerSector,
						&NumberOfFreeClusters, &TotalNumberOfClusters) == 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", php_win_err());
				return FAILURE;
			}
			bytestotal = (double)TotalNumberOfClusters * (double)SectorsPerCluster * (double)BytesPerSector;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load kernel32.dll");
		return FAILURE;
	}

	*space = bytestotal;
	return SUCCESS;
}
/* }}} */
#elif defined(OS2) /* {{{ */
{
	double bytestotal = 0;
	FSALLOCATE fsinfo;
	char drive = path[0] & 95;

	if (DosQueryFSInfo( drive ? drive - 64 : 0, FSIL_ALLOC, &fsinfo, sizeof( fsinfo ) ) == 0) {
		bytestotal = (double)fsinfo.cbSector * fsinfo.cSectorUnit * fsinfo.cUnit;
		*space = bytestotal;
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */
#else /* {{{ if !defined(OS2) && !defined(WINDOWS) */
{
	double bytestotal = 0;
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	struct statvfs buf;
#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	struct statfs buf;
#endif

#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	if (statvfs(path, &buf)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		return FAILURE;
	}
	if (buf.f_frsize) {
		bytestotal = (((double)buf.f_blocks) * ((double)buf.f_frsize));
	} else {
		bytestotal = (((double)buf.f_blocks) * ((double)buf.f_bsize));
	}

#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	if (statfs(path, &buf)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		return FAILURE;
	}
	bytestotal = (((double)buf.f_bsize) * ((double)buf.f_blocks));
#endif

	*space = bytestotal;
	return SUCCESS;
}
#endif
/* }}} */
/* }}} */

/* {{{ proto float disk_total_space(string path) U
   Get total disk space for filesystem that path is on */
PHP_FUNCTION(disk_total_space)
{
	char *path;
	int path_len;
	zend_uchar path_type;
	double bytestotal;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &path, &path_len, &path_type) == FAILURE) {
		return;
	}

	if (path_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &path, &path_len, (UChar*)path, path_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}

	if (php_check_open_basedir(path TSRMLS_CC)) {
		goto totalspace_failure;
	}

	if (php_disk_total_space(path, &bytestotal TSRMLS_CC) == SUCCESS) {
		if (path_type == IS_UNICODE) {
			efree(path);
		}
		RETURN_DOUBLE(bytestotal);
	}

totalspace_failure:
	if (path_type == IS_UNICODE) {
		efree(path);
	}
	RETURN_FALSE;
}
/* }}} */

static int php_disk_free_space(char *path, double *space TSRMLS_DC) /* {{{ */
#if defined(WINDOWS) /* {{{ */
{
	double bytesfree = 0;

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

	/* GetDiskFreeSpaceEx is only available in NT and Win95 post-OSR2,
	   so we have to jump through some hoops to see if the function
	   exists. */
	kernel32 = LoadLibrary("kernel32.dll");
	if (kernel32) {
		gdfse = GetProcAddress(kernel32, "GetDiskFreeSpaceExA");
		/* It's available, so we can call it. */
		if (gdfse) {
			func = (gdfse_func)gdfse;
			if (func(path,
						&FreeBytesAvailableToCaller,
						&TotalNumberOfBytes,
						&TotalNumberOfFreeBytes) == 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", php_win_err());
				return FAILURE;
			}

			/* i know - this is ugly, but i works <thies@thieso.net> */
			bytesfree  = FreeBytesAvailableToCaller.HighPart *
				(double) (((unsigned long)1) << 31) * 2.0 +
				FreeBytesAvailableToCaller.LowPart;
		} else { /* If it's not available, we just use GetDiskFreeSpace */
			if (GetDiskFreeSpace(path,
						&SectorsPerCluster, &BytesPerSector,
						&NumberOfFreeClusters, &TotalNumberOfClusters) == 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", php_win_err());
				return FAILURE;
			}
			bytesfree = (double)NumberOfFreeClusters * (double)SectorsPerCluster * (double)BytesPerSector;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load kernel32.dll");
		return FAILURE;
	}

	*space = bytesfree;
	return SUCCESS;
}
/* }}} */
#elif defined(OS2) /* {{{ */
{
	double bytesfree = 0;
	FSALLOCATE fsinfo;
	char drive = path[0] & 95;

	if (DosQueryFSInfo( drive ? drive - 64 : 0, FSIL_ALLOC, &fsinfo, sizeof( fsinfo ) ) == 0) {
		bytesfree = (double)fsinfo.cbSector * fsinfo.cSectorUnit * fsinfo.cUnitAvail;
		*space = bytesfree;
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */
#else /* {{{ if !defined(OS2) && !defined(WINDOWS) */
{
	double bytesfree = 0;
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	struct statvfs buf;
#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	struct statfs buf;
#endif

#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	if (statvfs(path, &buf)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		return FAILURE;
	}
	if (buf.f_frsize) {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_frsize));
	} else {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_bsize));
	}
#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	if (statfs(path, &buf)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		return FAILURE;
	}
#ifdef NETWARE
	bytesfree = (((double)buf.f_bsize) * ((double)buf.f_bfree));
#else
	bytesfree = (((double)buf.f_bsize) * ((double)buf.f_bavail));
#endif
#endif

	*space = bytesfree;
	return SUCCESS;
}
#endif
/* }}} */
/* }}} */


/* {{{ proto float disk_free_space(string path) U
   Get free disk space for filesystem that path is on */
PHP_FUNCTION(disk_free_space)
{
	char *path;
	int path_len;
	zend_uchar path_type;
	double bytesfree;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &path, &path_len, &path_type) == FAILURE) {
		return;
	}

	if (path_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &path, &path_len, (UChar*)path, path_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}

	if (php_check_open_basedir(path TSRMLS_CC)) {
		goto freespace_failure;
	}

	if (php_disk_free_space(path, &bytesfree TSRMLS_CC) == SUCCESS) {
		if (path_type == IS_UNICODE) {
			efree(path);
		}
		RETURN_DOUBLE(bytesfree);
	}

freespace_failure:
	if (path_type == IS_UNICODE) {
		efree(path);
	}
	RETURN_FALSE;
}
/* }}} */

#if !defined(WINDOWS)
static void php_do_chgrp(INTERNAL_FUNCTION_PARAMETERS, int do_lchgrp) /* {{{ */
{
	char *filename;
	int filename_len;
	zend_uchar filename_type;
	zval *group;
	gid_t gid;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tz/", &filename, &filename_len, &filename_type, &group) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(group) == IS_LONG) {
		gid = (gid_t)Z_LVAL_P(group);
	} else if (Z_TYPE_P(group) == IS_STRING ||
		   Z_TYPE_P(group) == IS_UNICODE
	) {
		if (Z_TYPE_P(group) == IS_UNICODE) {
			zval_unicode_to_string(group TSRMLS_CC);
		}
#if defined(ZTS) && defined(HAVE_GETGRNAM_R) && defined(_SC_GETGR_R_SIZE_MAX)
		struct group gr;
		struct group *retgrptr;
		long grbuflen = sysconf(_SC_GETGR_R_SIZE_MAX);
		char *grbuf;

		if (grbuflen < 1) {
			RETURN_FALSE;
		}

		grbuf = emalloc(grbuflen);
		if (getgrnam_r(Z_STRVAL_P(group), &gr, grbuf, grbuflen, &retgrptr) != 0 || retgrptr == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find gid for %s", Z_STRVAL_P(group));
			efree(grbuf);
			RETURN_FALSE;
		}
		efree(grbuf);
		gid = gr.gr_gid;
#else
{
		struct group *gr = getgrnam(Z_STRVAL_P(group));

		if (!gr) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find gid for %s", Z_STRVAL_P(group));
			RETURN_FALSE;
		}
		gid = gr->gr_gid;
}
#endif
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "parameter 2 should be string or integer, %s given", zend_zval_type_name(group));
		RETURN_FALSE;
	}

	if (filename_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &filename, &filename_len, (UChar*)filename, filename_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		if (filename_type == IS_UNICODE) {
			efree(filename);
		}
		RETURN_FALSE;
	}

	if (do_lchgrp) {
#if HAVE_LCHOWN
		ret = VCWD_LCHOWN(filename, -1, gid);
#endif
	} else {
		ret = VCWD_CHOWN(filename, -1, gid);
	}

	if (filename_type == IS_UNICODE) {
		efree(filename);
	}

	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#endif

#ifndef NETWARE
/* {{{ proto bool chgrp(string filename, mixed group) U
   Change file group */
PHP_FUNCTION(chgrp)
{
#if !defined(WINDOWS)
	php_do_chgrp(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool lchgrp(string filename, mixed group) U
   Change symlink group */
#if HAVE_LCHOWN
PHP_FUNCTION(lchgrp)
{
# if !defined(WINDOWS)
	php_do_chgrp(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
# else
	RETURN_FALSE;
# endif
}
#endif
/* }}} */
#endif /* !NETWARE */

#if !defined(WINDOWS)
static void php_do_chown(INTERNAL_FUNCTION_PARAMETERS, int do_lchown) /* {{{ */
{
	char *filename;
	int filename_len;
	zend_uchar filename_type;
	zval *user;
	uid_t uid;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tz/", &filename, &filename_len, &filename_type, &user) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(user) == IS_LONG) {
		uid = (uid_t) Z_LVAL_P(user);
	} else if (Z_TYPE_P(user) == IS_STRING ||
		   Z_TYPE_P(user) == IS_UNICODE
	) {
		if (Z_TYPE_P(user) == IS_UNICODE) {
			zval_unicode_to_string(user TSRMLS_CC);
		}
#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWNAM_R)
		struct passwd pw;
		struct passwd *retpwptr = NULL;
		long pwbuflen = sysconf(_SC_GETPW_R_SIZE_MAX);
		char *pwbuf;

		if (pwbuflen < 1) {
			RETURN_FALSE;
		}

		pwbuf = emalloc(pwbuflen);
		if (getpwnam_r(Z_STRVAL_P(user), &pw, pwbuf, pwbuflen, &retpwptr) != 0 || retpwptr == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find uid for %s", Z_STRVAL_P(user));
			efree(pwbuf);
			RETURN_FALSE;
		}
		efree(pwbuf);
		uid = pw.pw_uid;
#else
{
		struct passwd *pw = getpwnam(Z_STRVAL_P(user));

		if (!pw) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find uid for %s", Z_STRVAL_P(user));
			RETURN_FALSE;
		}
		uid = pw->pw_uid;
}
#endif
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "parameter 2 should be string or integer, %s given", zend_zval_type_name(user));
		RETURN_FALSE;
	}

	if (filename_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &filename, &filename_len, (UChar*)filename, filename_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		if (filename_type == IS_UNICODE) {
			efree(filename);
		}
		RETURN_FALSE;
	}

	if (do_lchown) {
#if HAVE_LCHOWN
		ret = VCWD_LCHOWN(filename, uid, -1);
#endif
	} else {
		ret = VCWD_CHOWN(filename, uid, -1);
	}
	if (filename_type == IS_UNICODE) {
		efree(filename);
	}

	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
}
/* }}} */
#endif

#ifndef NETWARE
/* {{{ proto bool chown (string filename, mixed user) U
   Change file owner */
PHP_FUNCTION(chown)
{
#if !defined(WINDOWS)
	RETVAL_TRUE;
	php_do_chown(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool lchown (string filename, mixed user) U
   Change file owner */
#if HAVE_LCHOWN
PHP_FUNCTION(lchown)
{
# if !defined(WINDOWS)
	RETVAL_TRUE;
	php_do_chown(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
# else
	RETURN_FALSE;
# endif
}
#endif
/* }}} */
#endif /* !NETWARE */

/* {{{ proto bool chmod(string filename, int mode) U
   Change file mode */
PHP_FUNCTION(chmod)
{
	char *filename;
	int filename_len;
	zend_uchar filename_type;
	long mode;
	int ret;
	mode_t imode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tl", &filename, &filename_len, &filename_type, &mode) == FAILURE) {
		return;
	}

	if (filename_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &filename, &filename_len, (UChar*)filename, filename_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		if (filename_type == IS_UNICODE) {
			efree(filename);
		}
		RETURN_FALSE;
	}

	imode = (mode_t) mode;

	ret = VCWD_CHMOD(filename, imode);
	if (filename_type == IS_UNICODE) {
		efree(filename);
	}

	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#if HAVE_UTIME
/* {{{ proto bool touch(string filename [, int time [, int atime]]) U
   Set modification time of file */
PHP_FUNCTION(touch)
{
	char *filename;
	int filename_len;
	zend_uchar filename_type;
	long filetime = 0, fileatime = 0;
	int ret, argc = ZEND_NUM_ARGS();
	FILE *file;
	struct utimbuf newtimebuf;
	struct utimbuf *newtime = &newtimebuf;

	if (zend_parse_parameters(argc TSRMLS_CC, "t|ll", &filename, &filename_len, &filename_type, &filetime, &fileatime) == FAILURE) {
		return;
	}

	switch (argc) {
		case 1:
#ifdef HAVE_UTIME_NULL
			newtime = NULL;
#else
			newtime->modtime = newtime->actime = time(NULL);
#endif
			break;
		case 2:
			newtime->modtime = newtime->actime = filetime;
			break;
		case 3:
			newtime->modtime = filetime;
			newtime->actime = fileatime;
			break;
		default:
			/* Never reached */
			WRONG_PARAM_COUNT;
	}

	if (filename_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &filename, &filename_len, (UChar*)filename, filename_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		if (filename_type == IS_UNICODE) {
			efree(filename);
		}
		RETURN_FALSE;
	}

	/* create the file if it doesn't exist already */
	if (VCWD_ACCESS(filename, F_OK) != 0) {
		file = VCWD_FOPEN(filename, "w");
		if (file == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create file %s because %s", filename, strerror(errno));
			if (filename_type == IS_UNICODE) {
				efree(filename);
			}
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = VCWD_UTIME(filename, newtime);
	if (filename_type == IS_UNICODE) {
		efree(filename);
	}

	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Utime failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ php_clear_stat_cache()
*/
PHPAPI void php_clear_stat_cache(TSRMLS_D)
{
	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
		BG(CurrentStatFile) = NULL;
	}
	if (BG(CurrentLStatFile)) {
		efree(BG(CurrentLStatFile));
		BG(CurrentLStatFile) = NULL;
	}
	realpath_cache_clean(TSRMLS_C);
}
/* }}} */

/* {{{ proto void clearstatcache(void)
   Clear file stat cache */
PHP_FUNCTION(clearstatcache)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	php_clear_stat_cache(TSRMLS_C);
}
/* }}} */

#define IS_LINK_OPERATION(__t) ((__t) == FS_TYPE || (__t) == FS_IS_LINK || (__t) == FS_LSTAT)
#define IS_EXISTS_CHECK(__t) ((__t) == FS_EXISTS  || (__t) == FS_IS_W || (__t) == FS_IS_R || (__t) == FS_IS_X || (__t) == FS_IS_FILE || (__t) == FS_IS_DIR || (__t) == FS_IS_LINK)
#define IS_ABLE_CHECK(__t) ((__t) == FS_IS_R || (__t) == FS_IS_W || (__t) == FS_IS_X)
#define IS_ACCESS_CHECK(__t) (IS_ABLE_CHECK(type) || (__t) == FS_EXISTS)

PHPAPI void php_u_stat(zend_uchar filename_type, const zstr filename, php_stat_len filename_length, int type, php_stream_context *context, zval *return_value TSRMLS_DC) /* {{{ */
{
	char *fn;

	if (filename_type == IS_STRING) {
		php_stat(filename.s, filename_length, type, return_value TSRMLS_CC);
	} else {
		if (FAILURE == php_stream_path_encode(NULL, &fn, &filename_length, filename.u, filename_length, REPORT_ERRORS, context)) {
			RETURN_FALSE;
		}
		php_stat(fn, filename_length, type, return_value TSRMLS_CC);
		efree(fn);
	}
}
/* }}} */

/* {{{ php_stat
 */
PHPAPI void php_stat(const char *filename, php_stat_len filename_length, int type, zval *return_value TSRMLS_DC)
{
	zval *stat_dev, *stat_ino, *stat_mode, *stat_nlink, *stat_uid, *stat_gid, *stat_rdev,
		 *stat_size, *stat_atime, *stat_mtime, *stat_ctime, *stat_blksize, *stat_blocks;
	struct stat *stat_sb;
	php_stream_statbuf ssb;
	int flags = 0, rmask=S_IROTH, wmask=S_IWOTH, xmask=S_IXOTH; /* access rights defaults to other */
	char *stat_sb_names[13] = {
		"dev", "ino", "mode", "nlink", "uid", "gid", "rdev",
		"size", "atime", "mtime", "ctime", "blksize", "blocks"
	};
	char *local;
	php_stream_wrapper *wrapper;

	if (!filename_length) {
		RETURN_FALSE;
	}

	if ((wrapper = php_stream_locate_url_wrapper(filename, &local, 0 TSRMLS_CC)) == &php_plain_files_wrapper) {
		if (php_check_open_basedir(local TSRMLS_CC)) {
			RETURN_FALSE;
		}
	}

	if (IS_ACCESS_CHECK(type)) {
		if (wrapper == &php_plain_files_wrapper) {

			switch (type) {
#ifdef F_OK
				case FS_EXISTS:
					RETURN_BOOL(VCWD_ACCESS(local, F_OK) == 0);
					break;
#endif
#ifdef W_OK
				case FS_IS_W:
					RETURN_BOOL(VCWD_ACCESS(local, W_OK) == 0);
					break;
#endif
#ifdef R_OK
				case FS_IS_R:
					RETURN_BOOL(VCWD_ACCESS(local, R_OK) == 0);
					break;
#endif
#ifdef X_OK
				case FS_IS_X:
					RETURN_BOOL(VCWD_ACCESS(local, X_OK) == 0);
					break;
#endif
			}
		}
	}

	if (IS_LINK_OPERATION(type)) {
		flags |= PHP_STREAM_URL_STAT_LINK;
	}
	if (IS_EXISTS_CHECK(type)) {
		flags |= PHP_STREAM_URL_STAT_QUIET;
	}

	if (php_stream_stat_path_ex((char *)filename, flags, &ssb, NULL)) {
		/* Error Occured */
		if (!IS_EXISTS_CHECK(type)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%sstat failed for %s", IS_LINK_OPERATION(type) ? "L" : "", filename);
		}
		RETURN_FALSE;
	}

	stat_sb = &ssb.sb;


#ifndef NETWARE
	if (type >= FS_IS_W && type <= FS_IS_X) {
		if(ssb.sb.st_uid==getuid()) {
			rmask=S_IRUSR;
			wmask=S_IWUSR;
			xmask=S_IXUSR;
		} else if(ssb.sb.st_gid==getgid()) {
			rmask=S_IRGRP;
			wmask=S_IWGRP;
			xmask=S_IXGRP;
		} else {
			int   groups, n, i;
			gid_t *gids;

			groups = getgroups(0, NULL);
			if(groups > 0) {
				gids=(gid_t *)safe_emalloc(groups, sizeof(gid_t), 0);
				n=getgroups(groups, gids);
				for(i=0;i<n;i++){
					if(ssb.sb.st_gid==gids[i]) {
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

#ifndef NETWARE
	if (IS_ABLE_CHECK(type) && getuid() == 0) {
		/* root has special perms on plain_wrapper
		   But we don't know about root under Netware */
		if (wrapper == &php_plain_files_wrapper) {
			if (type == FS_IS_X) {
				xmask = S_IXROOT;
			} else {
				RETURN_TRUE;
			}
		}
	}
#endif

	switch (type) {
	case FS_PERMS:
		RETURN_LONG((long)ssb.sb.st_mode);
	case FS_INODE:
		RETURN_LONG((long)ssb.sb.st_ino);
	case FS_SIZE:
		RETURN_LONG((long)ssb.sb.st_size);
	case FS_OWNER:
		RETURN_LONG((long)ssb.sb.st_uid);
	case FS_GROUP:
		RETURN_LONG((long)ssb.sb.st_gid);
	case FS_ATIME:
#ifdef NETWARE
		RETURN_LONG((long)ssb.sb.st_atime.tv_sec);
#else
		RETURN_LONG((long)ssb.sb.st_atime);
#endif
	case FS_MTIME:
#ifdef NETWARE
		RETURN_LONG((long)ssb.sb.st_mtime.tv_sec);
#else
		RETURN_LONG((long)ssb.sb.st_mtime);
#endif
	case FS_CTIME:
#ifdef NETWARE
		RETURN_LONG((long)ssb.sb.st_ctime.tv_sec);
#else
		RETURN_LONG((long)ssb.sb.st_ctime);
#endif
	case FS_TYPE:
		if (S_ISLNK(ssb.sb.st_mode)) {
			RETURN_ASCII_STRING("link", 1);
		}
		switch(ssb.sb.st_mode & S_IFMT) {
		case S_IFIFO: RETURN_ASCII_STRING("fifo", 1);
		case S_IFCHR: RETURN_ASCII_STRING("char", 1);
		case S_IFDIR: RETURN_ASCII_STRING("dir", 1);
		case S_IFBLK: RETURN_ASCII_STRING("block", 1);
		case S_IFREG: RETURN_ASCII_STRING("file", 1);
#if defined(S_IFSOCK) && !defined(ZEND_WIN32)&&!defined(__BEOS__)
		case S_IFSOCK: RETURN_ASCII_STRING("socket", 1);
#endif
		}
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown file type (%d)", ssb.sb.st_mode&S_IFMT);
		RETURN_ASCII_STRING("unknown", 1);
	case FS_IS_W:
		RETURN_BOOL((ssb.sb.st_mode & wmask) != 0);
	case FS_IS_R:
		RETURN_BOOL((ssb.sb.st_mode&rmask)!=0);
	case FS_IS_X:
		RETURN_BOOL((ssb.sb.st_mode&xmask)!=0 && !S_ISDIR(ssb.sb.st_mode));
	case FS_IS_FILE:
		RETURN_BOOL(S_ISREG(ssb.sb.st_mode));
	case FS_IS_DIR:
		RETURN_BOOL(S_ISDIR(ssb.sb.st_mode));
	case FS_IS_LINK:
		RETURN_BOOL(S_ISLNK(ssb.sb.st_mode));
	case FS_EXISTS:
		RETURN_TRUE; /* the false case was done earlier */
	case FS_LSTAT:
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
#ifdef NETWARE
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
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[0], strlen(stat_sb_names[0])+1, (void *) &stat_dev, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[1], strlen(stat_sb_names[1])+1, (void *) &stat_ino, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[2], strlen(stat_sb_names[2])+1, (void *) &stat_mode, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[3], strlen(stat_sb_names[3])+1, (void *) &stat_nlink, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[4], strlen(stat_sb_names[4])+1, (void *) &stat_uid, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[5], strlen(stat_sb_names[5])+1, (void *) &stat_gid, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[6], strlen(stat_sb_names[6])+1, (void *) &stat_rdev, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[7], strlen(stat_sb_names[7])+1, (void *) &stat_size, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[8], strlen(stat_sb_names[8])+1, (void *) &stat_atime, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[9], strlen(stat_sb_names[9])+1, (void *) &stat_mtime, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[10], strlen(stat_sb_names[10])+1, (void *) &stat_ctime, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[11], strlen(stat_sb_names[11])+1, (void *) &stat_blksize, sizeof(zval *), NULL);
		zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[12], strlen(stat_sb_names[12])+1, (void *) &stat_blocks, sizeof(zval *), NULL);

		return;
	}
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Didn't understand stat call");
	RETURN_FALSE;
}
/* }}} */

/* another quickie macro to make defining similar functions easier */
/* {{{ FileFunction(name, funcnum) */
#define FileFunction(name, funcnum) \
void name(INTERNAL_FUNCTION_PARAMETERS) { \
	char *filename; \
	int filename_len; \
	zend_uchar filename_type; \
	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &filename, &filename_len, &filename_type) == FAILURE) { \
		return; \
	} \
	\
	if (filename_type == IS_UNICODE) { \
		if (FAILURE == php_stream_path_encode(NULL, &filename, &filename_len, (UChar*)filename, filename_len, REPORT_ERRORS, FG(default_context))) { \
			RETURN_FALSE; \
		} \
	} \
	\
	php_stat(filename, (php_stat_len) filename_len, funcnum, return_value TSRMLS_CC); \
	\
	if (filename_type == IS_UNICODE) { \
		efree(filename); \
	} \
}
/* }}} */

/* {{{ proto int fileperms(string filename) U
   Get file permissions */
FileFunction(PHP_FN(fileperms), FS_PERMS)
/* }}} */

/* {{{ proto int fileinode(string filename) U
   Get file inode */
FileFunction(PHP_FN(fileinode), FS_INODE)
/* }}} */

/* {{{ proto int filesize(string filename) U
   Get file size */
FileFunction(PHP_FN(filesize), FS_SIZE)
/* }}} */

/* {{{ proto int fileowner(string filename) U
   Get file owner */
FileFunction(PHP_FN(fileowner), FS_OWNER)
/* }}} */

/* {{{ proto int filegroup(string filename) U
   Get file group */
FileFunction(PHP_FN(filegroup), FS_GROUP)
/* }}} */

/* {{{ proto int fileatime(string filename) U
   Get last access time of file */
FileFunction(PHP_FN(fileatime), FS_ATIME)
/* }}} */

/* {{{ proto int filemtime(string filename) U
   Get last modification time of file */
FileFunction(PHP_FN(filemtime), FS_MTIME)
/* }}} */

/* {{{ proto int filectime(string filename) U
   Get inode modification time of file */
FileFunction(PHP_FN(filectime), FS_CTIME)
/* }}} */

/* {{{ proto string filetype(string filename) U
   Get file type */
FileFunction(PHP_FN(filetype), FS_TYPE)
/* }}} */

/* {{{ proto bool is_writable(string filename) U
   Returns true if file can be written */
FileFunction(PHP_FN(is_writable), FS_IS_W)
/* }}} */

/* {{{ proto bool is_readable(string filename) U
   Returns true if file can be read */
FileFunction(PHP_FN(is_readable), FS_IS_R)
/* }}} */

/* {{{ proto bool is_executable(string filename) U
   Returns true if file is executable */
FileFunction(PHP_FN(is_executable), FS_IS_X)
/* }}} */

/* {{{ proto bool is_file(string filename) U
   Returns true if file is a regular file */
FileFunction(PHP_FN(is_file), FS_IS_FILE)
/* }}} */

/* {{{ proto bool is_dir(string filename) U
   Returns true if file is directory */
FileFunction(PHP_FN(is_dir), FS_IS_DIR)
/* }}} */

/* {{{ proto bool is_link(string filename) U
   Returns true if file is symbolic link */
FileFunction(PHP_FN(is_link), FS_IS_LINK)
/* }}} */

/* {{{ proto bool file_exists(string filename) U
   Returns true if filename exists */
FileFunction(PHP_FN(file_exists), FS_EXISTS)
/* }}} */

/* {{{ proto array lstat(string filename) U
   Give information about a file or symbolic link */
FileFunction(php_if_lstat, FS_LSTAT)
/* }}} */

/* {{{ proto array stat(string filename) U
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
