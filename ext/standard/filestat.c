/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
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

static int php_disk_total_space(char *path, double *space) /* {{{ */
#if defined(WINDOWS) /* {{{ */
{
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	PHP_WIN32_IOUTIL_INIT_W(path)

	if (GetDiskFreeSpaceExW(pathw, &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes) == 0) {
		char *err = php_win_err();
		php_error_docref(NULL, E_WARNING, "%s", err);
		php_win_err_free(err);
		PHP_WIN32_IOUTIL_CLEANUP_W()
		return FAILURE;
	}

	/* i know - this is ugly, but i works <thies@thieso.net> */
	*space = TotalNumberOfBytes.HighPart * (double) (((zend_ulong)1) << 31) * 2.0 + TotalNumberOfBytes.LowPart;

	PHP_WIN32_IOUTIL_CLEANUP_W()

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
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		return FAILURE;
	}
	if (buf.f_frsize) {
		bytestotal = (((double)buf.f_blocks) * ((double)buf.f_frsize));
	} else {
		bytestotal = (((double)buf.f_blocks) * ((double)buf.f_bsize));
	}

#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	if (statfs(path, &buf)) {
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
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

/* {{{ proto float disk_total_space(string path)
   Get total disk space for filesystem that path is on */
PHP_FUNCTION(disk_total_space)
{
	double bytestotal;
	char *path;
	size_t path_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(path, path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (php_check_open_basedir(path)) {
		RETURN_FALSE;
	}

	if (php_disk_total_space(path, &bytestotal) == SUCCESS) {
		RETURN_DOUBLE(bytestotal);
	}
	RETURN_FALSE;
}
/* }}} */

static int php_disk_free_space(char *path, double *space) /* {{{ */
#if defined(WINDOWS) /* {{{ */
{
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	PHP_WIN32_IOUTIL_INIT_W(path)

	if (GetDiskFreeSpaceExW(pathw, &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes) == 0) {
		char *err = php_win_err();
		php_error_docref(NULL, E_WARNING, "%s", err);
		php_win_err_free(err);
		PHP_WIN32_IOUTIL_CLEANUP_W()
		return FAILURE;
	}

	/* i know - this is ugly, but i works <thies@thieso.net> */
	*space = FreeBytesAvailableToCaller.HighPart * (double) (((zend_ulong)1) << 31) * 2.0 + FreeBytesAvailableToCaller.LowPart;

	PHP_WIN32_IOUTIL_CLEANUP_W()

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
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		return FAILURE;
	}
	if (buf.f_frsize) {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_frsize));
	} else {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_bsize));
	}
#elif (defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_MOUNT_H)) && defined(HAVE_STATFS)
	if (statfs(path, &buf)) {
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		return FAILURE;
	}
	bytesfree = (((double)buf.f_bsize) * ((double)buf.f_bavail));
#endif

	*space = bytesfree;
	return SUCCESS;
}
#endif
/* }}} */
/* }}} */

/* {{{ proto float disk_free_space(string path)
   Get free disk space for filesystem that path is on */
PHP_FUNCTION(disk_free_space)
{
	double bytesfree;
	char *path;
	size_t path_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(path, path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (php_check_open_basedir(path)) {
		RETURN_FALSE;
	}

	if (php_disk_free_space(path, &bytesfree) == SUCCESS) {
		RETURN_DOUBLE(bytesfree);
	}
	RETURN_FALSE;
}
/* }}} */

#ifndef PHP_WIN32
PHPAPI int php_get_gid_by_name(const char *name, gid_t *gid)
{
#if defined(ZTS) && defined(HAVE_GETGRNAM_R) && defined(_SC_GETGR_R_SIZE_MAX)
		struct group gr;
		struct group *retgrptr;
		long grbuflen = sysconf(_SC_GETGR_R_SIZE_MAX);
		char *grbuf;

		if (grbuflen < 1) {
			return FAILURE;
		}

		grbuf = emalloc(grbuflen);
		if (getgrnam_r(name, &gr, grbuf, grbuflen, &retgrptr) != 0 || retgrptr == NULL) {
			efree(grbuf);
			return FAILURE;
		}
		efree(grbuf);
		*gid = gr.gr_gid;
#else
		struct group *gr = getgrnam(name);

		if (!gr) {
			return FAILURE;
		}
		*gid = gr->gr_gid;
#endif
		return SUCCESS;
}
#endif

static void php_do_chgrp(INTERNAL_FUNCTION_PARAMETERS, int do_lchgrp) /* {{{ */
{
	char *filename;
	size_t filename_len;
	zval *group;
#if !defined(WINDOWS)
	gid_t gid;
	int ret;
#endif
	php_stream_wrapper *wrapper;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_ZVAL(group)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	wrapper = php_stream_locate_url_wrapper(filename, NULL, 0);
	if(wrapper != &php_plain_files_wrapper || strncasecmp("file://", filename, 7) == 0) {
		if(wrapper && wrapper->wops->stream_metadata) {
			int option;
			void *value;
			if (Z_TYPE_P(group) == IS_LONG) {
				option = PHP_STREAM_META_GROUP;
				value = &Z_LVAL_P(group);
			} else if (Z_TYPE_P(group) == IS_STRING) {
				option = PHP_STREAM_META_GROUP_NAME;
				value = Z_STRVAL_P(group);
			} else {
				php_error_docref(NULL, E_WARNING, "parameter 2 should be string or int, %s given", zend_zval_type_name(group));
				RETURN_FALSE;
			}
			if(wrapper->wops->stream_metadata(wrapper, filename, option, value, NULL)) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
		} else {
#if !defined(WINDOWS)
/* On Windows, we expect regular chgrp to fail silently by default */
			php_error_docref(NULL, E_WARNING, "Can not call chgrp() for a non-standard stream");
#endif
			RETURN_FALSE;
		}
	}

#if defined(WINDOWS)
	/* We have no native chgrp on Windows, nothing left to do if stream doesn't have own implementation */
	RETURN_FALSE;
#else
	if (Z_TYPE_P(group) == IS_LONG) {
		gid = (gid_t)Z_LVAL_P(group);
	} else if (Z_TYPE_P(group) == IS_STRING) {
		if(php_get_gid_by_name(Z_STRVAL_P(group), &gid) != SUCCESS) {
			php_error_docref(NULL, E_WARNING, "Unable to find gid for %s", Z_STRVAL_P(group));
			RETURN_FALSE;
		}
	} else {
		php_error_docref(NULL, E_WARNING, "parameter 2 should be string or int, %s given", zend_zval_type_name(group));
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename)) {
		RETURN_FALSE;
	}

	if (do_lchgrp) {
#if HAVE_LCHOWN
		ret = VCWD_LCHOWN(filename, -1, gid);
#endif
	} else {
		ret = VCWD_CHOWN(filename, -1, gid);
	}
	if (ret == -1) {
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#endif
}
/* }}} */

/* {{{ proto bool chgrp(string filename, mixed group)
   Change file group */
PHP_FUNCTION(chgrp)
{
	php_do_chgrp(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool lchgrp(string filename, mixed group)
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

#ifndef PHP_WIN32
PHPAPI uid_t php_get_uid_by_name(const char *name, uid_t *uid)
{
#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWNAM_R)
		struct passwd pw;
		struct passwd *retpwptr = NULL;
		long pwbuflen = sysconf(_SC_GETPW_R_SIZE_MAX);
		char *pwbuf;

		if (pwbuflen < 1) {
			return FAILURE;
		}

		pwbuf = emalloc(pwbuflen);
		if (getpwnam_r(name, &pw, pwbuf, pwbuflen, &retpwptr) != 0 || retpwptr == NULL) {
			efree(pwbuf);
			return FAILURE;
		}
		efree(pwbuf);
		*uid = pw.pw_uid;
#else
		struct passwd *pw = getpwnam(name);

		if (!pw) {
			return FAILURE;
		}
		*uid = pw->pw_uid;
#endif
		return SUCCESS;
}
#endif

static void php_do_chown(INTERNAL_FUNCTION_PARAMETERS, int do_lchown) /* {{{ */
{
	char *filename;
	size_t filename_len;
	zval *user;
#if !defined(WINDOWS)
	uid_t uid;
	int ret;
#endif
	php_stream_wrapper *wrapper;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_ZVAL(user)
	ZEND_PARSE_PARAMETERS_END();

	wrapper = php_stream_locate_url_wrapper(filename, NULL, 0);
	if(wrapper != &php_plain_files_wrapper || strncasecmp("file://", filename, 7) == 0) {
		if(wrapper && wrapper->wops->stream_metadata) {
			int option;
			void *value;
			if (Z_TYPE_P(user) == IS_LONG) {
				option = PHP_STREAM_META_OWNER;
				value = &Z_LVAL_P(user);
			} else if (Z_TYPE_P(user) == IS_STRING) {
				option = PHP_STREAM_META_OWNER_NAME;
				value = Z_STRVAL_P(user);
			} else {
				php_error_docref(NULL, E_WARNING, "parameter 2 should be string or int, %s given", zend_zval_type_name(user));
				RETURN_FALSE;
			}
			if(wrapper->wops->stream_metadata(wrapper, filename, option, value, NULL)) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
		} else {
#if !defined(WINDOWS)
/* On Windows, we expect regular chown to fail silently by default */
			php_error_docref(NULL, E_WARNING, "Can not call chown() for a non-standard stream");
#endif
			RETURN_FALSE;
		}
	}

#if defined(WINDOWS)
	/* We have no native chown on Windows, nothing left to do if stream doesn't have own implementation */
	RETURN_FALSE;
#else

	if (Z_TYPE_P(user) == IS_LONG) {
		uid = (uid_t)Z_LVAL_P(user);
	} else if (Z_TYPE_P(user) == IS_STRING) {
		if(php_get_uid_by_name(Z_STRVAL_P(user), &uid) != SUCCESS) {
			php_error_docref(NULL, E_WARNING, "Unable to find uid for %s", Z_STRVAL_P(user));
			RETURN_FALSE;
		}
	} else {
		php_error_docref(NULL, E_WARNING, "parameter 2 should be string or int, %s given", zend_zval_type_name(user));
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename)) {
		RETURN_FALSE;
	}

	if (do_lchown) {
#if HAVE_LCHOWN
		ret = VCWD_LCHOWN(filename, uid, -1);
#endif
	} else {
		ret = VCWD_CHOWN(filename, uid, -1);
	}
	if (ret == -1) {
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#endif
}
/* }}} */


/* {{{ proto bool chown(string filename, mixed user)
   Change file owner */
PHP_FUNCTION(chown)
{
	php_do_chown(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool chown(string filename, mixed user)
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

/* {{{ proto bool chmod(string filename, int mode)
   Change file mode */
PHP_FUNCTION(chmod)
{
	char *filename;
	size_t filename_len;
	zend_long mode;
	int ret;
	mode_t imode;
	php_stream_wrapper *wrapper;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	wrapper = php_stream_locate_url_wrapper(filename, NULL, 0);
	if(wrapper != &php_plain_files_wrapper || strncasecmp("file://", filename, 7) == 0) {
		if(wrapper && wrapper->wops->stream_metadata) {
			if(wrapper->wops->stream_metadata(wrapper, filename, PHP_STREAM_META_ACCESS, &mode, NULL)) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Can not call chmod() for a non-standard stream");
			RETURN_FALSE;
		}
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename)) {
		RETURN_FALSE;
	}

	imode = (mode_t) mode;

	ret = VCWD_CHMOD(filename, imode);
	if (ret == -1) {
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
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
	char *filename;
	size_t filename_len;
	zend_long filetime = 0, fileatime = 0;
	int ret, argc = ZEND_NUM_ARGS();
	FILE *file;
	struct utimbuf newtimebuf;
	struct utimbuf *newtime = &newtimebuf;
	php_stream_wrapper *wrapper;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(filetime)
		Z_PARAM_LONG(fileatime)
	ZEND_PARSE_PARAMETERS_END();

	if (!filename_len) {
		RETURN_FALSE;
	}

	switch (argc) {
		case 1:
			newtime = NULL;
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

	wrapper = php_stream_locate_url_wrapper(filename, NULL, 0);
	if(wrapper != &php_plain_files_wrapper || strncasecmp("file://", filename, 7) == 0) {
		if(wrapper && wrapper->wops->stream_metadata) {
			if(wrapper->wops->stream_metadata(wrapper, filename, PHP_STREAM_META_TOUCH, newtime, NULL)) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
		} else {
			php_stream *stream;
			if(argc > 1) {
				php_error_docref(NULL, E_WARNING, "Can not call touch() for a non-standard stream");
				RETURN_FALSE;
			}
			stream = php_stream_open_wrapper_ex(filename, "c", REPORT_ERRORS, NULL, NULL);
			if(stream != NULL) {
				php_stream_close(stream);
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
		}
	}

	/* Check the basedir */
	if (php_check_open_basedir(filename)) {
		RETURN_FALSE;
	}

	/* create the file if it doesn't exist already */
	if (VCWD_ACCESS(filename, F_OK) != 0) {
		file = VCWD_FOPEN(filename, "w");
		if (file == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to create file %s because %s", filename, strerror(errno));
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = VCWD_UTIME(filename, newtime);
	if (ret == -1) {
		php_error_docref(NULL, E_WARNING, "Utime failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ php_clear_stat_cache()
*/
PHPAPI void php_clear_stat_cache(zend_bool clear_realpath_cache, const char *filename, size_t filename_len)
{
	/* always clear CurrentStatFile and CurrentLStatFile even if filename is not NULL
	 * as it may contain outdated data (e.g. "nlink" for a directory when deleting a file
	 * in this directory, as shown by lstat_stat_variation9.phpt) */
	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
		BG(CurrentStatFile) = NULL;
	}
	if (BG(CurrentLStatFile)) {
		efree(BG(CurrentLStatFile));
		BG(CurrentLStatFile) = NULL;
	}
	if (clear_realpath_cache) {
		if (filename != NULL) {
			realpath_cache_del(filename, filename_len);
		} else {
			realpath_cache_clean();
		}
	}
}
/* }}} */

/* {{{ proto void clearstatcache([bool clear_realpath_cache[, string filename]])
   Clear file stat cache */
PHP_FUNCTION(clearstatcache)
{
	zend_bool  clear_realpath_cache = 0;
	char      *filename             = NULL;
	size_t     filename_len         = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(clear_realpath_cache)
		Z_PARAM_PATH(filename, filename_len)
	ZEND_PARSE_PARAMETERS_END();

	php_clear_stat_cache(clear_realpath_cache, filename, filename_len);
}
/* }}} */

#define IS_LINK_OPERATION(__t) ((__t) == FS_TYPE || (__t) == FS_IS_LINK || (__t) == FS_LSTAT)
#define IS_EXISTS_CHECK(__t) ((__t) == FS_EXISTS  || (__t) == FS_IS_W || (__t) == FS_IS_R || (__t) == FS_IS_X || (__t) == FS_IS_FILE || (__t) == FS_IS_DIR || (__t) == FS_IS_LINK)
#define IS_ABLE_CHECK(__t) ((__t) == FS_IS_R || (__t) == FS_IS_W || (__t) == FS_IS_X)
#define IS_ACCESS_CHECK(__t) (IS_ABLE_CHECK(type) || (__t) == FS_EXISTS)

/* {{{ php_stat
 */
PHPAPI void php_stat(const char *filename, size_t filename_length, int type, zval *return_value)
{
	zval stat_dev, stat_ino, stat_mode, stat_nlink, stat_uid, stat_gid, stat_rdev,
		 stat_size, stat_atime, stat_mtime, stat_ctime, stat_blksize, stat_blocks;
	zend_stat_t *stat_sb;
	php_stream_statbuf ssb;
	int flags = 0, rmask=S_IROTH, wmask=S_IWOTH, xmask=S_IXOTH; /* access rights defaults to other */
	char *stat_sb_names[13] = {
		"dev", "ino", "mode", "nlink", "uid", "gid", "rdev",
		"size", "atime", "mtime", "ctime", "blksize", "blocks"
	};
	const char *local;
	php_stream_wrapper *wrapper;

	if (!filename_length) {
		RETURN_FALSE;
	}

	if ((wrapper = php_stream_locate_url_wrapper(filename, &local, 0)) == &php_plain_files_wrapper && php_check_open_basedir(local)) {
		RETURN_FALSE;
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
		/* Error Occurred */
		if (!IS_EXISTS_CHECK(type)) {
			php_error_docref(NULL, E_WARNING, "%sstat failed for %s", IS_LINK_OPERATION(type) ? "L" : "", filename);
		}
		RETURN_FALSE;
	}

	stat_sb = &ssb.sb;

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

	if (IS_ABLE_CHECK(type) && getuid() == 0) {
		/* root has special perms on plain_wrapper */
		if (wrapper == &php_plain_files_wrapper) {
			if (type == FS_IS_X) {
				xmask = S_IXROOT;
			} else {
				RETURN_TRUE;
			}
		}
	}

	switch (type) {
	case FS_PERMS:
		RETURN_LONG((zend_long)ssb.sb.st_mode);
	case FS_INODE:
		RETURN_LONG((zend_long)ssb.sb.st_ino);
	case FS_SIZE:
		RETURN_LONG((zend_long)ssb.sb.st_size);
	case FS_OWNER:
		RETURN_LONG((zend_long)ssb.sb.st_uid);
	case FS_GROUP:
		RETURN_LONG((zend_long)ssb.sb.st_gid);
	case FS_ATIME:
		RETURN_LONG((zend_long)ssb.sb.st_atime);
	case FS_MTIME:
		RETURN_LONG((zend_long)ssb.sb.st_mtime);
	case FS_CTIME:
		RETURN_LONG((zend_long)ssb.sb.st_ctime);
	case FS_TYPE:
		if (S_ISLNK(ssb.sb.st_mode)) {
			RETURN_STRING("link");
		}
		switch(ssb.sb.st_mode & S_IFMT) {
		case S_IFIFO: RETURN_STRING("fifo");
		case S_IFCHR: RETURN_STRING("char");
		case S_IFDIR: RETURN_STRING("dir");
		case S_IFBLK: RETURN_STRING("block");
		case S_IFREG: RETURN_STRING("file");
#if defined(S_IFSOCK) && !defined(PHP_WIN32)
		case S_IFSOCK: RETURN_STRING("socket");
#endif
		}
		php_error_docref(NULL, E_NOTICE, "Unknown file type (%d)", ssb.sb.st_mode&S_IFMT);
		RETURN_STRING("unknown");
	case FS_IS_W:
		RETURN_BOOL((ssb.sb.st_mode & wmask) != 0);
	case FS_IS_R:
		RETURN_BOOL((ssb.sb.st_mode&rmask)!=0);
	case FS_IS_X:
		RETURN_BOOL((ssb.sb.st_mode&xmask)!=0);
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

		ZVAL_LONG(&stat_dev, stat_sb->st_dev);
		ZVAL_LONG(&stat_ino, stat_sb->st_ino);
		ZVAL_LONG(&stat_mode, stat_sb->st_mode);
		ZVAL_LONG(&stat_nlink, stat_sb->st_nlink);
		ZVAL_LONG(&stat_uid, stat_sb->st_uid);
		ZVAL_LONG(&stat_gid, stat_sb->st_gid);
#ifdef HAVE_STRUCT_STAT_ST_RDEV
		ZVAL_LONG(&stat_rdev, stat_sb->st_rdev);
#else
		ZVAL_LONG(&stat_rdev, -1);
#endif
		ZVAL_LONG(&stat_size, stat_sb->st_size);
		ZVAL_LONG(&stat_atime, stat_sb->st_atime);
		ZVAL_LONG(&stat_mtime, stat_sb->st_mtime);
		ZVAL_LONG(&stat_ctime, stat_sb->st_ctime);
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
		ZVAL_LONG(&stat_blksize, stat_sb->st_blksize);
#else
		ZVAL_LONG(&stat_blksize,-1);
#endif
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
		ZVAL_LONG(&stat_blocks, stat_sb->st_blocks);
#else
		ZVAL_LONG(&stat_blocks,-1);
#endif
		/* Store numeric indexes in proper order */
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_dev);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_ino);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_mode);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_nlink);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_uid);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_gid);

		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_rdev);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_size);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_atime);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_mtime);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_ctime);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_blksize);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_blocks);

		/* Store string indexes referencing the same zval*/
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[0], strlen(stat_sb_names[0]), &stat_dev);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[1], strlen(stat_sb_names[1]), &stat_ino);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[2], strlen(stat_sb_names[2]), &stat_mode);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[3], strlen(stat_sb_names[3]), &stat_nlink);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[4], strlen(stat_sb_names[4]), &stat_uid);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[5], strlen(stat_sb_names[5]), &stat_gid);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[6], strlen(stat_sb_names[6]), &stat_rdev);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[7], strlen(stat_sb_names[7]), &stat_size);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[8], strlen(stat_sb_names[8]), &stat_atime);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[9], strlen(stat_sb_names[9]), &stat_mtime);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[10], strlen(stat_sb_names[10]), &stat_ctime);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[11], strlen(stat_sb_names[11]), &stat_blksize);
		zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[12], strlen(stat_sb_names[12]), &stat_blocks);

		return;
	}
	php_error_docref(NULL, E_WARNING, "Didn't understand stat call");
	RETURN_FALSE;
}
/* }}} */

/* another quickie macro to make defining similar functions easier */
/* {{{ FileFunction(name, funcnum) */
#define FileFunction(name, funcnum) \
ZEND_NAMED_FUNCTION(name) { \
	char *filename; \
	size_t filename_len; \
	\
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_PATH(filename, filename_len) \
	ZEND_PARSE_PARAMETERS_END(); \
	\
	php_stat(filename, filename_len, funcnum, return_value); \
}
/* }}} */

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

/* {{{ proto bool realpath_cache_size()
   Get current size of realpath cache */
PHP_FUNCTION(realpath_cache_size)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_LONG(realpath_cache_size());
}

/* {{{ proto bool realpath_cache_get()
   Get current size of realpath cache */
PHP_FUNCTION(realpath_cache_get)
{
	realpath_cache_bucket **buckets = realpath_cache_get_buckets(), **end = buckets + realpath_cache_max_buckets();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	while(buckets < end) {
		realpath_cache_bucket *bucket = *buckets;
		while(bucket) {
			zval entry;

			array_init(&entry);

			/* bucket->key is unsigned long */
			if (ZEND_LONG_MAX >= bucket->key) {
				add_assoc_long_ex(&entry, "key", sizeof("key") - 1, bucket->key);
			} else {
				add_assoc_double_ex(&entry, "key", sizeof("key") - 1, (double)bucket->key);
			}
			add_assoc_bool_ex(&entry, "is_dir", sizeof("is_dir") - 1, bucket->is_dir);
			add_assoc_stringl_ex(&entry, "realpath", sizeof("realpath") - 1, bucket->realpath, bucket->realpath_len);
			add_assoc_long_ex(&entry, "expires", sizeof("expires") - 1, bucket->expires);
#ifdef PHP_WIN32
			add_assoc_bool_ex(&entry, "is_rvalid", sizeof("is_rvalid") - 1, bucket->is_rvalid);
			add_assoc_bool_ex(&entry, "is_wvalid", sizeof("is_wvalid") - 1, bucket->is_wvalid);
			add_assoc_bool_ex(&entry, "is_readable", sizeof("is_readable") - 1, bucket->is_readable);
			add_assoc_bool_ex(&entry, "is_writable", sizeof("is_writable") - 1, bucket->is_writable);
#endif
			zend_hash_str_update(Z_ARRVAL_P(return_value), bucket->path, bucket->path_len, &entry);
			bucket = bucket->next;
		}
		buckets++;
	}
}
