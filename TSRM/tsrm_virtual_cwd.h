/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Sascha Schumann <sascha@schumann.cx>                        |
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

#ifndef TSRM_WIN32
#include <unistd.h>
#endif

#ifdef TSRM_WIN32
#include "readdir.h"
#include <sys/utime.h>
/* mode_t isn't defined on Windows */
typedef unsigned short mode_t;

#define DEFAULT_SLASH '\\'
#define DEFAULT_DIR_SEPARATOR	';'
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define IS_SLASH_P(c)	(*(c) == '/' || \
        (*(c) == '\\' && !IsDBCSLeadByte(*(c-1))))

/* COPY_WHEN_ABSOLUTE also takes path as argument because netware needs it
 * to account for volume name that is unique to NetWare absolute paths
 */
#define COPY_WHEN_ABSOLUTE(path) 2
#define IS_ABSOLUTE_PATH(path, len) \
	(len >= 2 && isalpha(path[0]) && path[1] == ':')
#define IS_UNC_PATH(path, len) \
	(len >= 2 && IS_SLASH(path[0]) && IS_SLASH(path[1]))

#elif defined(NETWARE)
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#define DEFAULT_SLASH '/'
#define DEFAULT_DIR_SEPARATOR	';'
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define COPY_WHEN_ABSOLUTE(path) \
    (strchr(path, ':') - path + 1)  /* Take the volume name which ends with a colon */
#define IS_ABSOLUTE_PATH(path, len) \
    (strchr(path, ':') != NULL) /* Colon indicates volume name */

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

#ifdef TSRM_WIN32
#       ifdef CWD_EXPORTS
#       define CWD_API __declspec(dllexport)
#       else
#       define CWD_API __declspec(dllimport)
#       endif
#else
#define CWD_API
#endif

typedef struct _cwd_state {
	char *cwd;
	int cwd_length;
} cwd_state;

typedef int (*verify_path_func)(const cwd_state *);

CWD_API void virtual_cwd_startup(void);
CWD_API void virtual_cwd_shutdown(void);
CWD_API char *virtual_getcwd_ex(size_t *length TSRMLS_DC);
CWD_API char *virtual_getcwd(char *buf, size_t size TSRMLS_DC);
CWD_API int virtual_chdir(const char *path TSRMLS_DC);
CWD_API int virtual_chdir_file(const char *path, int (*p_chdir)(const char *path TSRMLS_DC) TSRMLS_DC);
CWD_API int virtual_filepath(const char *path, char **filepath TSRMLS_DC);
CWD_API int virtual_filepath_ex(const char *path, char **filepath, verify_path_func verify_path TSRMLS_DC);
CWD_API char *virtual_realpath(const char *path, char *real_path TSRMLS_DC);
CWD_API FILE *virtual_fopen(const char *path, const char *mode TSRMLS_DC);
CWD_API int virtual_open(const char *path TSRMLS_DC, int flags, ...);
CWD_API int virtual_creat(const char *path, mode_t mode TSRMLS_DC);
CWD_API int virtual_rename(char *oldname, char *newname TSRMLS_DC);
#if !(defined(NETWARE) && defined(CLIB_STAT_PATCH))
CWD_API int virtual_stat(const char *path, struct stat *buf TSRMLS_DC);
#else
CWD_API int virtual_stat(const char *path, struct stat_libc *buf TSRMLS_DC);
#endif
#if !defined(TSRM_WIN32) && !defined(NETWARE)
CWD_API int virtual_lstat(const char *path, struct stat *buf TSRMLS_DC);
#endif
CWD_API int virtual_unlink(const char *path TSRMLS_DC);
CWD_API int virtual_mkdir(const char *pathname, mode_t mode TSRMLS_DC);
CWD_API int virtual_rmdir(const char *pathname TSRMLS_DC);
CWD_API DIR *virtual_opendir(const char *pathname TSRMLS_DC);
CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC);

#if !defined(TSRM_WIN32)
CWD_API int virtual_access(const char *pathname, int mode TSRMLS_DC);
#endif

#if HAVE_UTIME
CWD_API int virtual_utime(const char *filename, struct utimbuf *buf TSRMLS_DC);
#endif
CWD_API int virtual_chmod(const char *filename, mode_t mode TSRMLS_DC);
#if !defined(TSRM_WIN32) && !defined(NETWARE)
CWD_API int virtual_chown(const char *filename, uid_t owner, gid_t group TSRMLS_DC);
#endif

CWD_API int virtual_file_ex(cwd_state *state, const char *path, verify_path_func verify_path, int use_realpath);

typedef struct _virtual_cwd_globals {
	cwd_state cwd;
} virtual_cwd_globals;

#ifdef ZTS
# define CWDG(v) TSRMG(cwd_globals_id, virtual_cwd_globals *, v)
#else
# define CWDG(v) (cwd_globals.v)
#endif

/* The actual macros to be used in programs using TSRM
 * If the program defines VIRTUAL_DIR it will use the
 * virtual_* functions 
 */

#ifdef VIRTUAL_DIR

#define VCWD_GETCWD(buff, size) virtual_getcwd(buff, size TSRMLS_CC)
#define VCWD_FOPEN(path, mode) virtual_fopen(path, mode TSRMLS_CC)
/* Because open() has two modes, we have to macros to replace it */
#define VCWD_OPEN(path, flags) virtual_open(path TSRMLS_CC, flags)
#define VCWD_OPEN_MODE(path, flags, mode) virtual_open(path TSRMLS_CC, flags, mode)
#define VCWD_CREAT(path, mode) virtual_creat(path, mode TSRMLS_CC)
#define VCWD_CHDIR(path) virtual_chdir(path TSRMLS_CC)
#define VCWD_CHDIR_FILE(path) virtual_chdir_file(path, virtual_chdir TSRMLS_CC)
#define VCWD_GETWD(buf)
#define VCWD_REALPATH(path, real_path) virtual_realpath(path, real_path TSRMLS_CC)
#define VCWD_RENAME(oldname, newname) virtual_rename(oldname, newname TSRMLS_CC)
#define VCWD_STAT(path, buff) virtual_stat(path, buff TSRMLS_CC)
#if !defined(TSRM_WIN32) && !defined(NETWARE)
#define VCWD_LSTAT(path, buff) virtual_lstat(path, buff TSRMLS_CC)
#else
#define VCWD_LSTAT(path, buff) virtual_lstat(path, buff TSRMLS_CC)
#endif
#define VCWD_UNLINK(path) virtual_unlink(path TSRMLS_CC)
#define VCWD_MKDIR(pathname, mode) virtual_mkdir(pathname, mode TSRMLS_CC)
#define VCWD_RMDIR(pathname) virtual_rmdir(pathname TSRMLS_CC)
#define VCWD_OPENDIR(pathname) virtual_opendir(pathname TSRMLS_CC)
#define VCWD_POPEN(command, type) virtual_popen(command, type TSRMLS_CC)
#define VCWD_ACCESS(pathname, mode) virtual_access(pathname, mode TSRMLS_CC)
#if HAVE_UTIME
#define VCWD_UTIME(path, time) virtual_utime(path, time TSRMLS_CC)
#endif
#define VCWD_CHMOD(path, mode) virtual_chmod(path, mode TSRMLS_CC)
#if !defined(TSRM_WIN32) && !defined(NETWARE)
#define VCWD_CHOWN(path, owner, group) virtual_chown(path, owner, group TSRMLS_CC)
#endif

#else

#define VCWD_GETCWD(buff, size) getcwd(buff, size)
#define VCWD_FOPEN(path, mode)  fopen(path, mode)
#define VCWD_OPEN(path, flags) open(path, flags)
#define VCWD_OPEN_MODE(path, flags, mode)	open(path, flags, mode)
#define VCWD_CREAT(path, mode) creat(path, mode)
#define VCWD_RENAME(oldname, newname) rename(oldname, newname)
#define VCWD_CHDIR(path) chdir(path)
#define VCWD_CHDIR_FILE(path) virtual_chdir_file(path, chdir)
#define VCWD_GETWD(buf) getwd(buf)
#define VCWD_STAT(path, buff) stat(path, buff)
#define VCWD_LSTAT(path, buff) lstat(path, buff)
#define VCWD_UNLINK(path) unlink(path)
#define VCWD_MKDIR(pathname, mode) mkdir(pathname, mode)
#define VCWD_RMDIR(pathname) rmdir(pathname)
#define VCWD_OPENDIR(pathname) opendir(pathname)
#define VCWD_POPEN(command, type) popen(command, type)
#define VCWD_ACCESS(pathname, mode) access(pathname, mode)

#if !defined(TSRM_WIN32) && !defined(NETWARE)
#define VCWD_REALPATH(path, real_path) realpath(path, real_path)
#else
#define VCWD_REALPATH(path, real_path) strcpy(real_path, path)
#endif

#if HAVE_UTIME
#define VCWD_UTIME(path, time) utime(path, time)
#endif
#define VCWD_CHMOD(path, mode) chmod(path, mode)
#if !defined(TSRM_WIN32) && !defined(NETWARE)
#define VCWD_CHOWN(path, owner, group) chown(path, owner, group)
#endif

#endif

#endif /* VIRTUAL_CWD_H */
