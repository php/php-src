/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
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

#include "php3_filestat.h"

#ifndef THREAD_SAFE
static char *CurrentStatFile=NULL;
# if MSVC5
static unsigned int CurrentStatLength=0;
# else
static int CurrentStatLength=0;
# endif
static struct stat sb;
#if HAVE_SYMLINK
static struct stat lsb;
#endif
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


int php3_init_filestat(INIT_FUNC_ARGS)
{
	CurrentStatFile=NULL;
	CurrentStatLength=0;
	return SUCCESS;
}


int php3_shutdown_filestat(SHUTDOWN_FUNC_ARGS)
{
	if (CurrentStatFile) {
		efree (CurrentStatFile);
	}
	return SUCCESS;
}

PHP_FUNCTION(chgrp)
{
#ifndef WINDOWS
	pval *filename, *group;
	gid_t gid;
	struct group *gr=NULL;
	int ret;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&group)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	if (group->type == IS_STRING) {
		gr = getgrnam(group->value.str.val);
		if (!gr) {
			php3_error(E_WARNING, "unable to find gid for %s",
					   group->value.str.val);
			RETURN_FALSE;
		}
		gid = gr->gr_gid;
	} else {
		convert_to_long(group);
		gid = group->value.lval;
	}

	if (PG(safe_mode) &&(!_php3_checkuid(filename->value.str.val, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	ret = chown(filename->value.str.val, -1, gid);
	if (ret == -1) {
		php3_error(E_WARNING, "chgrp failed: %s", strerror(errno));
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
	pval *filename, *user;
	int ret;
	uid_t uid;
	struct passwd *pw = NULL;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&user)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	if (user->type == IS_STRING) {
		pw = getpwnam(user->value.str.val);
		if (!pw) {
			php3_error(E_WARNING, "unable to find uid for %s",
					   user->value.str.val);
			RETURN_FALSE;
		}
		uid = pw->pw_uid;
	} else {
		convert_to_long(user);
		uid = user->value.lval;
	}

	if (PG(safe_mode) &&(!_php3_checkuid(filename->value.str.val, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	ret = chown(filename->value.str.val, uid, -1);
	if (ret == -1) {
		php3_error(E_WARNING, "chown failed: %s", strerror(errno));
		RETURN_FALSE;
	}
#endif
	RETURN_TRUE;
}


PHP_FUNCTION(chmod)
{
	pval *filename, *mode;
	int ret;
	PLS_FETCH();
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&mode)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	convert_to_long(mode);

	if (PG(safe_mode) &&(!_php3_checkuid(filename->value.str.val, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	ret = chmod(filename->value.str.val, mode->value.lval);
	if (ret == -1) {
		php3_error(E_WARNING, "chmod failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}


PHP_FUNCTION(touch)
{
#if HAVE_UTIME
	pval *filename, *filetime;
	int ret;
	struct stat sb;
	FILE *file;
	struct utimbuf *newtime = NULL;
	int ac = ARG_COUNT(ht);
	PLS_FETCH();
	
	if (ac == 1 && getParameters(ht,1,&filename) != FAILURE) {
#ifndef HAVE_UTIME_NULL
		newtime = (struct utimbuf *)emalloc(sizeof(struct utimbuf));
		if (!newtime) {
			php3_error(E_WARNING, "unable to emalloc memory for changing time");
			return;
		}
		newtime->actime = time(NULL);
		newtime->modtime = newtime->actime;
#endif
	} else if (ac == 2 && getParameters(ht,2,&filename,&filetime) != FAILURE) {
		newtime = (struct utimbuf *)emalloc(sizeof(struct utimbuf));
		if (!newtime) {
			php3_error(E_WARNING, "unable to emalloc memory for changing time");
			return;
		}
		convert_to_long(filetime);
		newtime->actime = filetime->value.lval;
		newtime->modtime = filetime->value.lval;
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	if (PG(safe_mode) &&(!_php3_checkuid(filename->value.str.val, 1))) {
		if (newtime) efree(newtime);
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	/* create the file if it doesn't exist already */
	ret = stat(filename->value.str.val, &sb);
	if (ret == -1) {
		file = fopen(filename->value.str.val, "w");
		if (file == NULL) {
			php3_error(E_WARNING, "unable to create file %s because %s", filename->value.str.val, strerror(errno));
			if (newtime) efree(newtime);
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = utime(filename->value.str.val, newtime);
	if (newtime) efree(newtime);
	if (ret == -1) {
		php3_error(E_WARNING, "utime failed: %s", strerror(errno));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
#endif
}


PHP_FUNCTION(clearstatcache)
{
	if (CurrentStatFile) {
		efree(CurrentStatFile);
		CurrentStatFile = NULL;
	}
}


static void _php3_stat(const char *filename, int type, pval *return_value)
{
	struct stat *stat_sb = &sb;

	if (!CurrentStatFile || strcmp(filename,CurrentStatFile)) {
		if (!CurrentStatFile
			|| strlen(filename) > CurrentStatLength) {
			if (CurrentStatFile) efree(CurrentStatFile);
			CurrentStatLength = strlen(filename);
			CurrentStatFile = estrndup(filename,CurrentStatLength);
		} else {
			strcpy(CurrentStatFile,filename);
		}
#if HAVE_SYMLINK
		lsb.st_mode = 0; /* mark lstat buf invalid */
#endif
		if (stat(CurrentStatFile,&sb)==-1) {
			if (type != 15 || errno != ENOENT) { /* fileexists() test must print no error */
				php3_error(E_NOTICE,"stat failed for %s (errno=%d - %s)",CurrentStatFile,errno,strerror(errno));
			}
			efree(CurrentStatFile);
			CurrentStatFile=NULL;
			RETURN_FALSE;
		}
	}

#if HAVE_SYMLINK
	if (8 == type /* filetype */
		|| 14 == type /* is link */
		|| 16 == type) { /* lstat */

		/* do lstat if the buffer is empty */

		if (!lsb.st_mode) {
			if (lstat(CurrentStatFile,&lsb) == -1) {
				php3_error(E_NOTICE,"lstat failed for %s (errno=%d - %s)",CurrentStatFile,errno,strerror(errno));
				RETURN_FALSE;
			}
		}
	}
#endif

	switch(type) {
	case 0: /* fileperms */
		RETURN_LONG((long)sb.st_mode);
	case 1: /* fileinode */
		RETURN_LONG((long)sb.st_ino);
	case 2: /* filesize  */
		RETURN_LONG((long)sb.st_size);
	case 3: /* fileowner */
		RETURN_LONG((long)sb.st_uid);
	case 4: /* filegroup */
		RETURN_LONG((long)sb.st_gid);
	case 5: /* fileatime */
		RETURN_LONG((long)sb.st_atime);
	case 6: /* filemtime */
		RETURN_LONG((long)sb.st_mtime);
	case 7: /* filectime */
		RETURN_LONG((long)sb.st_ctime);
	case 8: /* filetype */
#if HAVE_SYMLINK
		if (S_ISLNK(lsb.st_mode)) {
			RETURN_STRING("link",1);
		}
#endif
		switch(sb.st_mode&S_IFMT) {
		case S_IFIFO: RETURN_STRING("fifo",1);
		case S_IFCHR: RETURN_STRING("char",1);
		case S_IFDIR: RETURN_STRING("dir",1);
		case S_IFBLK: RETURN_STRING("block",1);
		case S_IFREG: RETURN_STRING("file",1);
		}
		php3_error(E_WARNING,"Unknown file type (%d)",sb.st_mode&S_IFMT);
		RETURN_STRING("unknown",1);
	case 9: /*is writable*/
		RETURN_LONG((sb.st_mode&S_IWRITE)!=0);
	case 10: /*is readable*/
		RETURN_LONG((sb.st_mode&S_IREAD)!=0);
	case 11: /*is executable*/
		RETURN_LONG((sb.st_mode&S_IEXEC)!=0 && !S_ISDIR(sb.st_mode));
	case 12: /*is file*/
		RETURN_LONG(S_ISREG(sb.st_mode));
	case 13: /*is dir*/
		RETURN_LONG(S_ISDIR(sb.st_mode));
	case 14: /*is link*/
#if HAVE_SYMLINK
		RETURN_LONG(S_ISLNK(lsb.st_mode));
#else
		RETURN_FALSE;
#endif
	case 15: /*file exists*/
		RETURN_TRUE; /* the false case was done earlier */
	case 16: /* lstat */
#if HAVE_SYMLINK
		stat_sb = &lsb;
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
	php3_error(E_WARNING, "didn't understand stat call");
	RETURN_FALSE;
}

/* another quickie macro to make defining similar functions easier */
#define FileFunction(name, funcnum) \
void name(INTERNAL_FUNCTION_PARAMETERS) { \
	pval *filename; \
	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&filename) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} \
	convert_to_string(filename); \
	_php3_stat(filename->value.str.val, funcnum, return_value); \
}

FileFunction(php3_fileperms,0)
FileFunction(php3_fileinode,1)
FileFunction(php3_filesize, 2)
FileFunction(php3_fileowner,3)
FileFunction(php3_filegroup,4)
FileFunction(php3_fileatime,5)
FileFunction(php3_filemtime,6)
FileFunction(php3_filectime,7)
FileFunction(php3_filetype, 8)
FileFunction(php3_iswritable, 9)
FileFunction(php3_isreadable,10)
FileFunction(php3_isexec,11)
FileFunction(php3_isfile,12)
FileFunction(php3_isdir,13)
FileFunction(php3_islink,14)
FileFunction(php3_fileexists,15)
FileFunction(php3_lstat,16)
FileFunction(php3_stat,17)

function_entry php3_filestat_functions[] = {
	{"fileatime",	php3_fileatime,		NULL},
	{"filectime",	php3_filectime,		NULL},
	{"filegroup",	php3_filegroup,		NULL},
	{"fileinode",	php3_fileinode,		NULL},
	{"filemtime",	php3_filemtime,		NULL},
	{"fileowner",	php3_fileowner,		NULL},
	{"fileperms",	php3_fileperms,		NULL},
	{"filesize",	php3_filesize,		NULL},
	{"filetype",	php3_filetype,		NULL},
	{"file_exists",	php3_fileexists,	NULL},
	{"is_writeable",php3_iswritable,	NULL},
	{"is_readable",	php3_isreadable,	NULL},
	{"is_executable",php3_isexec,		NULL},
	{"is_file",		php3_isfile,		NULL},
	{"is_dir",		php3_isdir,			NULL},
	{"is_link",		php3_islink,		NULL},
	{"stat",		php3_stat,			NULL},
	{"lstat",		php3_lstat,			NULL},
	{"chown",		php3_chown,			NULL},
	{"chgrp",		php3_chgrp,			NULL},
	{"chmod",		php3_chmod,			NULL},
	{"touch",		php3_touch,			NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_filestat_module_entry = {
	"PHP_filestat", php3_filestat_functions, NULL, NULL, php3_init_filestat, php3_shutdown_filestat, NULL, STANDARD_MODULE_PROPERTIES
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
