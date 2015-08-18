/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   |          Pierre Joye <pierre@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef VIRTUAL_CWD_H
#define VIRTUAL_CWD_H

#include "TSRM.h"
#include "tsrm_config_common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#ifdef HAVE_UTIME_H
#include <utime.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef ZTS
#define VIRTUAL_DIR
#endif

#ifndef ZEND_WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif

#if defined(__osf__) || defined(_AIX)
#include <errno.h>
#endif

#ifdef ZEND_WIN32
#include "readdir.h"
#include <sys/utime.h>
/* mode_t isn't defined on Windows */
typedef unsigned short mode_t;

#define DEFAULT_SLASH '\\'
#define DEFAULT_DIR_SEPARATOR	';'
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define IS_SLASH_P(c)	(*(c) == '/' || \
        (*(c) == '\\' && !IsDBCSLeadByte(*(c-1))))

/* COPY_WHEN_ABSOLUTE is 2 under Win32 because by chance both regular absolute paths
   in the file system and UNC paths need copying of two characters */
#define COPY_WHEN_ABSOLUTE(path) 2
#define IS_UNC_PATH(path, len) \
	(len >= 2 && IS_SLASH(path[0]) && IS_SLASH(path[1]))
#define IS_ABSOLUTE_PATH(path, len) \
	(len >= 2 && (/* is local */isalpha(path[0]) && path[1] == ':' || /* is UNC */IS_SLASH(path[0]) && IS_SLASH(path[1])))

#elif defined(NETWARE)
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#define DEFAULT_SLASH '/'
#define DEFAULT_DIR_SEPARATOR	';'
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define IS_SLASH_P(c)	IS_SLASH(*(c))
/* Colon indicates volume name, either first character should be forward slash or backward slash */
#define IS_ABSOLUTE_PATH(path, len) \
    ((strchr(path, ':') != NULL) || ((len >= 1) && ((path[0] == '/') || (path[0] == '\\'))))

#else
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#define DEFAULT_SLASH '/'

#ifdef __riscos__
#define DEFAULT_DIR_SEPARATOR  ';'
#else
#define DEFAULT_DIR_SEPARATOR  ':'
#endif

#define IS_SLASH(c)	((c) == '/')
#define IS_SLASH_P(c)	(*(c) == '/')

#endif


#ifndef COPY_WHEN_ABSOLUTE
#define COPY_WHEN_ABSOLUTE(path) 0
#endif

#ifndef IS_ABSOLUTE_PATH
#define IS_ABSOLUTE_PATH(path, len) \
	(IS_SLASH(path[0]))
#endif

#ifdef TSRM_EXPORTS
#define CWD_EXPORTS
#endif

#ifdef ZEND_WIN32
#	ifdef CWD_EXPORTS
#		define CWD_API __declspec(dllexport)
#	else
#		define CWD_API __declspec(dllimport)
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define CWD_API __attribute__ ((visibility("default")))
#else
#	define CWD_API
#endif

#ifdef ZEND_WIN32
CWD_API int php_sys_stat_ex(const char *path, zend_stat_t *buf, int lstat);
# define php_sys_stat(path, buf) php_sys_stat_ex(path, buf, 0)
# define php_sys_lstat(path, buf) php_sys_stat_ex(path, buf, 1)
CWD_API int php_sys_readlink(const char *link, char *target, size_t target_len);
#else
# define php_sys_stat stat
# define php_sys_lstat lstat
# ifdef HAVE_SYMLINK
# define php_sys_readlink(link, target, target_len) readlink(link, target, target_len)
# endif
#endif

typedef struct _cwd_state {
	char *cwd;
	int cwd_length;
} cwd_state;

typedef int (*verify_path_func)(const cwd_state *);

CWD_API void virtual_cwd_startup(void);
CWD_API void virtual_cwd_shutdown(void);
CWD_API int virtual_cwd_activate(void);
CWD_API int virtual_cwd_deactivate(void);
CWD_API char *virtual_getcwd_ex(size_t *length);
CWD_API char *virtual_getcwd(char *buf, size_t size);
CWD_API int virtual_chdir(const char *path);
CWD_API int virtual_chdir_file(const char *path, int (*p_chdir)(const char *path));
CWD_API int virtual_filepath(const char *path, char **filepath);
CWD_API int virtual_filepath_ex(const char *path, char **filepath, verify_path_func verify_path);
CWD_API char *virtual_realpath(const char *path, char *real_path);
CWD_API FILE *virtual_fopen(const char *path, const char *mode);
CWD_API int virtual_open(const char *path, int flags, ...);
CWD_API int virtual_creat(const char *path, mode_t mode);
CWD_API int virtual_rename(const char *oldname, const char *newname);
CWD_API int virtual_stat(const char *path, zend_stat_t *buf);
CWD_API int virtual_lstat(const char *path, zend_stat_t *buf);
CWD_API int virtual_unlink(const char *path);
CWD_API int virtual_mkdir(const char *pathname, mode_t mode);
CWD_API int virtual_rmdir(const char *pathname);
CWD_API DIR *virtual_opendir(const char *pathname);
CWD_API FILE *virtual_popen(const char *command, const char *type);
CWD_API int virtual_access(const char *pathname, int mode);
#if defined(ZEND_WIN32)
/* these are not defined in win32 headers */
#ifndef W_OK
#define W_OK 0x02
#endif
#ifndef R_OK
#define R_OK 0x04
#endif
#ifndef X_OK
#define X_OK 0x01
#endif
#ifndef F_OK
#define F_OK 0x00
#endif
#endif

#if HAVE_UTIME
CWD_API int virtual_utime(const char *filename, struct utimbuf *buf);
#endif
CWD_API int virtual_chmod(const char *filename, mode_t mode);
#if !defined(ZEND_WIN32) && !defined(NETWARE)
CWD_API int virtual_chown(const char *filename, uid_t owner, gid_t group, int link);
#endif

/* One of the following constants must be used as the last argument
   in virtual_file_ex() call. */

#define CWD_EXPAND   0 /* expand "." and ".." but dont resolve symlinks      */
#define CWD_FILEPATH 1 /* resolve symlinks if file is exist otherwise expand */
#define CWD_REALPATH 2 /* call realpath(), resolve symlinks. File must exist */

CWD_API int virtual_file_ex(cwd_state *state, const char *path, verify_path_func verify_path, int use_realpath);

CWD_API char *tsrm_realpath(const char *path, char *real_path);

#define REALPATH_CACHE_TTL  (2*60) /* 2 minutes */
#define REALPATH_CACHE_SIZE 0      /* disabled while php.ini isn't loaded */

typedef struct _realpath_cache_bucket {
	zend_ulong                    key;
	char                          *path;
	char                          *realpath;
	struct _realpath_cache_bucket *next;
	time_t                         expires;
	int                            path_len;
	int                            realpath_len;
	int                            is_dir;
#ifdef ZEND_WIN32
	unsigned char                  is_rvalid;
	unsigned char                  is_readable;
	unsigned char                  is_wvalid;
	unsigned char                  is_writable;
#endif
} realpath_cache_bucket;

typedef struct _virtual_cwd_globals {
	cwd_state cwd;
	zend_long                   realpath_cache_size;
	zend_long                   realpath_cache_size_limit;
	zend_long                   realpath_cache_ttl;
	realpath_cache_bucket *realpath_cache[1024];
} virtual_cwd_globals;

#ifdef ZTS
extern ts_rsrc_id cwd_globals_id;
# define CWDG(v) ZEND_TSRMG(cwd_globals_id, virtual_cwd_globals *, v)
#else
extern virtual_cwd_globals cwd_globals;
# define CWDG(v) (cwd_globals.v)
#endif

CWD_API void realpath_cache_clean(void);
CWD_API void realpath_cache_del(const char *path, int path_len);
CWD_API realpath_cache_bucket* realpath_cache_lookup(const char *path, int path_len, time_t t);
CWD_API zend_long realpath_cache_size(void);
CWD_API zend_long realpath_cache_max_buckets(void);
CWD_API realpath_cache_bucket** realpath_cache_get_buckets(void);

/* The actual macros to be used in programs using TSRM
 * If the program defines VIRTUAL_DIR it will use the
 * virtual_* functions
 */

#ifdef VIRTUAL_DIR

#define VCWD_GETCWD(buff, size) virtual_getcwd(buff, size)
#define VCWD_FOPEN(path, mode) virtual_fopen(path, mode)
/* Because open() has two modes, we have to macros to replace it */
#define VCWD_OPEN(path, flags) virtual_open(path, flags)
#define VCWD_OPEN_MODE(path, flags, mode) virtual_open(path, flags, mode)
#define VCWD_CREAT(path, mode) virtual_creat(path, mode)
#define VCWD_CHDIR(path) virtual_chdir(path)
#define VCWD_CHDIR_FILE(path) virtual_chdir_file(path, virtual_chdir)
#define VCWD_GETWD(buf)
#define VCWD_REALPATH(path, real_path) virtual_realpath(path, real_path)
#define VCWD_RENAME(oldname, newname) virtual_rename(oldname, newname)
#define VCWD_STAT(path, buff) virtual_stat(path, buff)
# define VCWD_LSTAT(path, buff) virtual_lstat(path, buff)
#define VCWD_UNLINK(path) virtual_unlink(path)
#define VCWD_MKDIR(pathname, mode) virtual_mkdir(pathname, mode)
#define VCWD_RMDIR(pathname) virtual_rmdir(pathname)
#define VCWD_OPENDIR(pathname) virtual_opendir(pathname)
#define VCWD_POPEN(command, type) virtual_popen(command, type)
#define VCWD_ACCESS(pathname, mode) virtual_access(pathname, mode)
#if HAVE_UTIME
#define VCWD_UTIME(path, time) virtual_utime(path, time)
#endif
#define VCWD_CHMOD(path, mode) virtual_chmod(path, mode)
#if !defined(ZEND_WIN32) && !defined(NETWARE)
#define VCWD_CHOWN(path, owner, group) virtual_chown(path, owner, group, 0)
#if HAVE_LCHOWN
#define VCWD_LCHOWN(path, owner, group) virtual_chown(path, owner, group, 1)
#endif
#endif

#else

#define VCWD_GETCWD(buff, size) getcwd(buff, size)
#define VCWD_FOPEN(path, mode)  fopen(path, mode)
#define VCWD_OPEN(path, flags) open(path, flags)
#define VCWD_OPEN_MODE(path, flags, mode)	open(path, flags, mode)
#define VCWD_CREAT(path, mode) creat(path, mode)
/* rename on windows will fail if newname already exists.
   MoveFileEx has to be used */
#if defined(ZEND_WIN32)
# define VCWD_RENAME(oldname, newname) (MoveFileEx(oldname, newname, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED) == 0 ? -1 : 0)
#else
# define VCWD_RENAME(oldname, newname) rename(oldname, newname)
#endif
#define VCWD_CHDIR(path) chdir(path)
#define VCWD_CHDIR_FILE(path) virtual_chdir_file(path, chdir)
#define VCWD_GETWD(buf) getwd(buf)
#define VCWD_STAT(path, buff) php_sys_stat(path, buff)
#define VCWD_LSTAT(path, buff) lstat(path, buff)
#define VCWD_UNLINK(path) unlink(path)
#define VCWD_MKDIR(pathname, mode) mkdir(pathname, mode)
#define VCWD_RMDIR(pathname) rmdir(pathname)
#define VCWD_OPENDIR(pathname) opendir(pathname)
#define VCWD_POPEN(command, type) popen(command, type)
#if defined(ZEND_WIN32)
#define VCWD_ACCESS(pathname, mode) tsrm_win32_access(pathname, mode)
#else
#define VCWD_ACCESS(pathname, mode) access(pathname, mode)
#endif

#define VCWD_REALPATH(path, real_path) tsrm_realpath(path, real_path)

#if HAVE_UTIME
# ifdef ZEND_WIN32
#  define VCWD_UTIME(path, time) win32_utime(path, time)
# else
#  define VCWD_UTIME(path, time) utime(path, time)
# endif
#endif

#define VCWD_CHMOD(path, mode) chmod(path, mode)
#if !defined(ZEND_WIN32) && !defined(NETWARE)
#define VCWD_CHOWN(path, owner, group) chown(path, owner, group)
#if HAVE_LCHOWN
#define VCWD_LCHOWN(path, owner, group) lchown(path, owner, group)
#endif
#endif

#endif

/* Global stat declarations */
#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif

#ifndef _S_IFREG
#define _S_IFREG S_IFREG
#endif

#ifndef S_IFLNK
# define S_IFLNK 0120000
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)	(((mode)&S_IFMT) == S_IFREG)
#endif

#ifndef S_ISLNK
#define S_ISLNK(mode)	(((mode)&S_IFMT) == S_IFLNK)
#endif

#ifndef S_IXROOT
#define S_IXROOT ( S_IXUSR | S_IXGRP | S_IXOTH )
#endif

#endif /* VIRTUAL_CWD_H */
