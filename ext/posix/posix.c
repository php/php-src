/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kristian Koehntopp <kris@koehntopp.de>                      |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_posix.h"

#if HAVE_POSIX

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <unistd.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/times.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>

#define SAFE_STRING(s) ((s)?(s):"")

/* {{{ posix_functions[]
 */
function_entry posix_functions[] = {
    /* POSIX.1, 3.3 */
	PHP_FE(posix_kill,		NULL)

	/* POSIX.1, 4.1 */
	PHP_FE(posix_getpid,	NULL)
	PHP_FE(posix_getppid,	NULL)

	/* POSIX.1,  4.2 */
	PHP_FE(posix_getuid,	NULL)
	PHP_FE(posix_setuid,	NULL)
	PHP_FE(posix_geteuid,	NULL)
	PHP_FE(posix_seteuid,	NULL)
	PHP_FE(posix_getgid,	NULL)
	PHP_FE(posix_setgid,	NULL)
	PHP_FE(posix_getegid,	NULL)
	PHP_FE(posix_setegid,	NULL)
	PHP_FE(posix_getgroups,	NULL)
	PHP_FE(posix_getlogin,	NULL)

	/* POSIX.1, 4.3 */
	PHP_FE(posix_getpgrp,	NULL)
	PHP_FE(posix_setsid,	NULL)
	PHP_FE(posix_setpgid,	NULL)
	/* Non-Posix functions which are common */
	PHP_FE(posix_getpgid,	NULL)
	PHP_FE(posix_getsid,	NULL)

	/* POSIX.1, 4.4 */
	PHP_FE(posix_uname,		NULL)

	/* POSIX.1, 4.5 */
	PHP_FE(posix_times,		NULL)

	/* POSIX.1, 4.7 */
	PHP_FE(posix_ctermid,	NULL)
	PHP_FE(posix_ttyname,	NULL)
	PHP_FE(posix_isatty,	NULL)

    /* POSIX.1, 5.2 */
	PHP_FE(posix_getcwd,	NULL)

	/* POSIX.1, 5.4 */
	PHP_FE(posix_mkfifo,	NULL)

	/* POSIX.1, 9.2 */
	PHP_FE(posix_getgrnam,	NULL)
	PHP_FE(posix_getgrgid,	NULL)
	PHP_FE(posix_getpwnam,	NULL)
	PHP_FE(posix_getpwuid,	NULL)

	PHP_FE(posix_getrlimit,	NULL)

	{NULL, NULL, NULL}
};
/* }}} */

static PHP_MINFO_FUNCTION(posix);

/* {{{ posix_module_entry
 */
zend_module_entry posix_module_entry = {
	"posix", 
	posix_functions, 
	NULL,
	NULL,
	NULL,
	NULL, 
	PHP_MINFO(posix),
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_POSIX
ZEND_GET_MODULE(posix)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(posix)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Revision", "$Revision$");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto int posix_kill(int pid, int sig)
   Send a signal to a process (POSIX.1, 3.3.2) */

PHP_FUNCTION(posix_kill)
{
	pval   *pid;
	pval   *sig;
	int     result;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters(ht, 2, &pid, &sig)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(pid);                        
	convert_to_long(sig);
  
	result = kill(Z_LVAL_P(pid), Z_LVAL_P(sig));
	if (result< 0) {
		php_error(E_WARNING, "posix_kill(%d, %d) failed with '%s'",
    		Z_LVAL_P(pid),
			Z_LVAL_P(sig),
			strerror(errno));
		RETURN_FALSE;
  	}
  	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto long posix_getpid(void) 
   Get the current process id (POSIX.1, 4.1.1) */
PHP_FUNCTION(posix_getpid)
{
	pid_t  pid;

	pid = getpid();
	RETURN_LONG(pid);
}
/* }}} */

/* {{{ proto long posix_getppid(void) 
   Get the parent process id (POSIX.1, 4.1.1) */
PHP_FUNCTION(posix_getppid)
{
	pid_t  ppid;

	ppid = getppid();
	RETURN_LONG(ppid);
}
/* }}} */

/* {{{ proto long posix_getuid(void) 
   Get the current user id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_getuid)
{
	uid_t  uid;

	uid = getuid();
	RETURN_LONG(uid);
}
/* }}} */

/* {{{ proto long posix_getgid(void) 
   Get the current group id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_getgid)
{
	gid_t  gid;

	gid = getgid();
	RETURN_LONG(gid);
}
/* }}} */

/* {{{ proto long posix_geteuid(void) 
   Get the current effective user id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_geteuid)
{
	uid_t  uid;

	uid = geteuid();
	RETURN_LONG(uid);
}
/* }}} */

/* {{{ proto long posix_getegid(void) 
   Get the current effective group id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_getegid)
{
	gid_t  gid;

	gid = getegid();
	RETURN_LONG(gid);
}
/* }}} */

/* {{{ proto long posix_setuid(long uid)
   Set user id (POSIX.1, 4.2.2) */
PHP_FUNCTION(posix_setuid)
{
	pval *uid;
	int   result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &uid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(uid);
  
	result = setuid(Z_LVAL_P(uid));
	if (result < 0) {
		php_error(E_WARNING, "posix_setuid(%d) failed with '%s'. Must be root",
	    	Z_LVAL_P(uid),
			strerror(errno));
			RETURN_FALSE;
	}
	
	RETURN_TRUE;                                  
}
/* }}} */

/* {{{ proto long posix_setgid(long uid)
   Set group id (POSIX.1, 4.2.2) */
PHP_FUNCTION(posix_setgid)
{
	pval *gid;
	int   result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &gid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(gid);
  
	result = setgid(Z_LVAL_P(gid));
	if (result < 0) {
		php_error(E_WARNING, "posix_setgid(%d) failed with '%s'. Must be root",
	    	Z_LVAL_P(gid),
			strerror(errno));
			RETURN_FALSE;
	}
	
	RETURN_TRUE;                                  
}
/* }}} */

/* {{{ proto long posix_seteuid(long uid)
   Set effective user id */
PHP_FUNCTION(posix_seteuid)
{
#ifdef HAVE_SETEUID
	pval *uid;
	int   result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &uid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(uid);
  
	result = seteuid(Z_LVAL_P(uid));
	if (result < 0) {
		php_error(E_WARNING, "posix_setuid(%d) failed with '%s'.",
			Z_LVAL_P(uid),
			strerror(errno));
			RETURN_FALSE;
	}
	
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto long posix_setegid(long uid)
   Set effective group id */
PHP_FUNCTION(posix_setegid)
{
#ifdef HAVE_SETEGID
	pval *gid;
	int   result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &gid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(gid);
  
	result = setegid(Z_LVAL_P(gid));
	if (result < 0) {
		php_error(E_WARNING, "posix_setgid(%d) failed with '%s'.",
	    	Z_LVAL_P(gid),
			strerror(errno));
			RETURN_FALSE;
	}
	
	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto long posix_getgroups(void) 
   Get supplementary group id's (POSIX.1, 4.2.3) */
PHP_FUNCTION(posix_getgroups)
{
	gid_t  gidlist[NGROUPS_MAX];
	int    result;
	int    i;

	result = getgroups(NGROUPS_MAX, gidlist);
	if (result < 0) {
		php_error(E_WARNING, "posix_getgroups() failed with '%s'",
			strerror(errno));
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	for (i=0; i<result; i++) {
		add_next_index_long(return_value,gidlist[i]);
	}
}
/* }}} */

/* {{{ proto string posix_getlogin(void) 
   Get user name (POSIX.1, 4.2.4) */
PHP_FUNCTION(posix_getlogin)
{
	char *p;
	
	p = getlogin();
	if (p == NULL) {
		php_error(E_WARNING, "Cannot determine your login name. Something is really wrong here.");
		RETURN_FALSE;
	}
	
	RETURN_STRING(p, 1);
}
/* }}} */

/* {{{ proto long posix_getpgrp(void) 
   Get current process group id (POSIX.1, 4.3.1) */
PHP_FUNCTION(posix_getpgrp)
{
	pid_t  pgrp;

	pgrp = getpgrp();
	RETURN_LONG(pgrp);
}
/* }}} */

/* {{{ proto long posix_setsid(void) 
   Create session and set process group id (POSIX.1, 4.3.2) */
PHP_FUNCTION(posix_setsid)
{
#ifdef HAVE_SETSID
	pid_t  sid;

	sid = setsid();
	RETURN_LONG(sid);
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto long posix_setpgid(long pid, long pgid) 
   Set process group id for job control (POSIX.1, 4.3.3) */
PHP_FUNCTION(posix_setpgid)
{
	pval   *pid;
	pval   *pgid;
	int     result;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters(ht, 2, &pid, &pgid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(pid);
	convert_to_long(pgid);
  
	result = setpgid(Z_LVAL_P(pid), Z_LVAL_P(pgid));
	if (result< 0) {
		php_error(E_WARNING, "posix_setpgid(%d, %d) failed with '%s'",
    		Z_LVAL_P(pid),
			Z_LVAL_P(pgid),
			strerror(errno));
		RETURN_FALSE;
  	}
  	
	RETURN_LONG(result);
}
/* }}} */

/* {{{ proto long posix_getpgid(void) 
   Get the process group id of the specified process (This is not a POSIX function, but a SVR4ism, so we compile conditionally) */
PHP_FUNCTION(posix_getpgid)
{
#ifdef HAVE_GETPGID
	pid_t  pgid;
	pval  *pid;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &pid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(pid);
	pgid = getpgid(Z_LVAL_P(pid));
	if (pgid < 0) {
		php_error(E_WARNING, "posix_getpgid(%d) failed with '%s'", 
			Z_LVAL_P(pid),
			strerror(errno));
		RETURN_FALSE;
	}

	Z_TYPE_P(return_value)= IS_LONG;
	Z_LVAL_P(return_value) = pgid;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto long posix_getsid(void) 
   Get process group id of session leader (This is not a POSIX function, but a SVR4ism, so be compile conditionally) */
PHP_FUNCTION(posix_getsid)
{
#ifdef HAVE_GETSID
	pid_t  sid;
	pval  *pid;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &pid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(pid);
	sid = getsid(Z_LVAL_P(pid));
	if (sid < 0) {
		php_error(E_WARNING, "posix_getsid(%d) failed with '%s'", 
			Z_LVAL_P(pid),
			strerror(errno));
		RETURN_FALSE;
	}

	Z_TYPE_P(return_value)= IS_LONG;
	Z_LVAL_P(return_value) = sid;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto array posix_uname(void) 
   Get system name (POSIX.1, 4.4.1) */
PHP_FUNCTION(posix_uname)
{
	struct utsname u;

	uname(&u);

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "sysname",  u.sysname,  1);
	add_assoc_string(return_value, "nodename", u.nodename, 1);
    add_assoc_string(return_value, "release",  u.release, 1);
    add_assoc_string(return_value, "version",  u.version, 1);
    add_assoc_string(return_value, "machine",  u.machine, 1);
}
/* }}} */

/* POSIX.1, 4.5.1 time() - Get System Time
							already covered by PHP
 */

/* {{{ proto array posix_times(void) 
   Get process times (POSIX.1, 4.5.2) */
PHP_FUNCTION(posix_times)
{
	struct tms t;
	clock_t    ticks;

	ticks = times(&t);
	if (ticks < 0) {
		php_error(E_WARNING, "posix_times failed with '%s'",
			strerror(errno));
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_long(return_value, "ticks",		ticks);
	add_assoc_long(return_value, "utime",	t.tms_utime);
    add_assoc_long(return_value, "stime",	t.tms_stime);
    add_assoc_long(return_value, "cutime",	t.tms_cutime);
    add_assoc_long(return_value, "cstime",	t.tms_cstime);
}
/* }}} */

/* POSIX.1, 4.6.1 getenv() - Environment Access
							already covered by PHP
*/

/* {{{ proto string posix_ctermid(void) 
   Generate terminal path name (POSIX.1, 4.7.1) */
PHP_FUNCTION(posix_ctermid)
{
#ifdef HAVE_CTERMID
	char  buffer[L_ctermid];
	char *p;
	
	p = ctermid(buffer);
	if (p == NULL) {
		php_error(E_WARNING, "posix_ctermid() failed with '%s'",
			strerror(errno));
		RETURN_FALSE;
	}
	
	RETURN_STRING(buffer, 1);
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto string posix_ttyname(int fd) 
   Determine terminal device name (POSIX.1, 4.7.2) */
PHP_FUNCTION(posix_ttyname)
{
	pval *fd;
	char *p;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &fd)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(fd);

	p = ttyname(Z_LVAL_P(fd));
	if (p == NULL) {
		php_error(E_WARNING, "posix_ttyname(%d) failed with '%s'",
			Z_LVAL_P(fd),
			strerror(errno));
		RETURN_FALSE;
	}
	
	RETURN_STRING(p, 1);
}
/* }}} */

/* {{{ proto bool posix_isatty(int fd) 
   Determine if filedesc is a tty (POSIX.1, 4.7.1) */
PHP_FUNCTION(posix_isatty)
{
	pval *fd;
	int   result;
	
    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &fd)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(fd);

	result = isatty(Z_LVAL_P(fd));
	if (!result)
		RETURN_FALSE;

	RETURN_TRUE;	
}
/* }}} */

/*
	POSIX.1, 4.8.1 sysconf() - TODO
	POSIX.1, 5.7.1 pathconf(), fpathconf() - TODO

	POSIX.1, 5.1.2 opendir(), readdir(), rewinddir(), closedir()
	POSIX.1, 5.2.1 chdir()
				already supported by PHP
 */

/* {{{ proto string posix_getcwd(void) 
   Get working directory pathname (POSIX.1, 5.2.2) */
PHP_FUNCTION(posix_getcwd)
{
	char  buffer[MAXPATHLEN];
	char *p;

	p = VCWD_GETCWD(buffer, MAXPATHLEN);
	if (!p) {
		php_error(E_WARNING, "posix_getcwd() failed with '%s'",
			strerror(errno));
		RETURN_FALSE;
	}

	RETURN_STRING(buffer, 1);
}
/* }}} */

/*
	POSIX.1, 5.3.x open(), creat(), umask()
	POSIX.1, 5.4.1 link()
		already supported by PHP.
 */

/* {{{ proto string posix_mkfifo(void)
   Make a FIFO special file (POSIX.1, 5.4.2) */
PHP_FUNCTION(posix_mkfifo)
{
#ifdef HAVE_MKFIFO
	pval   *path;
	pval   *mode;
	int     result;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters(ht, 2, &path, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(path);
	convert_to_long(mode);

	if (PG(safe_mode) && (!php_checkuid(Z_STRVAL_P(path), NULL, CHECKUID_ALLOW_ONLY_DIR))) {
		RETURN_FALSE;
	}
	result = mkfifo(Z_STRVAL_P(path), Z_LVAL_P(mode));
	if (result < 0) {
		php_error(E_WARNING, "posix_mkfifo(%s) failed with '%s'",
			Z_STRVAL_P(path),
			strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
#else
	RETURN_FALSE;
#endif
}    
/* }}} */

/*
	POSIX.1, 5.5.1 unlink()
	POSIX.1, 5.5.2 rmdir()
	POSIX.1, 5.5.3 rename()
	POSIX.1, 5.6.x stat(), access(), chmod(), utime()
		already supported by PHP (access() not supported, because it is
		braindead and dangerous and gives outdated results).

	POSIX.1, 6.x most I/O functions already supported by PHP.
	POSIX.1, 7.x tty functions, TODO
	POSIX.1, 8.x interactions with other C language functions
	POSIX.1, 9.x system database access	
 */

/* {{{ proto array posix_getgrnam(string groupname) 
   Group database access (POSIX.1, 9.2.1) */
PHP_FUNCTION(posix_getgrnam)
{
	pval          *name;
	char           buffer[10];
	struct group  *g;
	char         **p;
	int            count;
	
    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &name)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(name);

	g = getgrnam(Z_STRVAL_P(name));
	if (!g) {
		php_error(E_WARNING, "posix_getgrnam(%s) failed with '%s'",
			Z_STRVAL_P(name),
			strerror(errno));
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "name",		g->gr_name,  1);
	add_assoc_long  (return_value, "gid",		g->gr_gid);
	for (count=0, p=g->gr_mem; p[count] != NULL; count++) {
		snprintf(buffer, 10, "%d", count);
		add_assoc_string(return_value, buffer, p[count], 1);
	}
	add_assoc_long(return_value, "members", count);
}
/* }}} */

/* {{{ proto array posix_getgrgid(long gid) 
   Group database access (POSIX.1, 9.2.1) */
PHP_FUNCTION(posix_getgrgid)
{
	pval          *gid;
	char           buffer[10];
	struct group  *g;
	char         **p;
	int            count;
	
    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &gid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(gid);

	g = getgrgid(Z_LVAL_P(gid));
	if (!g) {
		php_error(E_WARNING, "posix_getgrgid(%d) failed with '%s'",
			Z_LVAL_P(gid),
			strerror(errno));
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "name",		g->gr_name, 1);
	add_assoc_long  (return_value, "gid",		g->gr_gid);
	for (count=0, p=g->gr_mem; p[count] != NULL; count++) {
		snprintf(buffer, 10, "%d", count);
		add_assoc_string(return_value, buffer, p[count], 1);
	}
	add_assoc_long(return_value, "members", count);
}
/* }}} */

/* {{{ proto array posix_getpwnam(string groupname) 
   User database access (POSIX.1, 9.2.2) */
PHP_FUNCTION(posix_getpwnam)
{
	pval          *name;
	struct passwd *pw;
	
    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &name)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(name);

	pw = getpwnam(Z_STRVAL_P(name));
	if (!pw) {
		php_error(E_WARNING, "posix_getpwnam(%s) failed with '%s'",
			Z_STRVAL_P(name),
			strerror(errno));
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "name",      pw->pw_name, 1);
	add_assoc_string(return_value, "passwd",    pw->pw_passwd, 1);
	add_assoc_long  (return_value, "uid",       pw->pw_uid);
	add_assoc_long  (return_value, "gid",		pw->pw_gid);
	add_assoc_string(return_value, "gecos",     pw->pw_gecos, 1);
	add_assoc_string(return_value, "dir",       pw->pw_dir, 1);
	add_assoc_string(return_value, "shell",     pw->pw_shell, 1);
}
/* }}} */

/* {{{ proto array posix_getpwuid(long uid) 
   User database access (POSIX.1, 9.2.2) */
PHP_FUNCTION(posix_getpwuid)
{
	pval          *uid;
	struct passwd *pw;
	
    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters(ht, 1, &uid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(uid);

	pw = getpwuid(Z_LVAL_P(uid));
	if (!pw) {
		php_error(E_WARNING, "posix_getpwuid(%d) failed with '%s'",
			Z_LVAL_P(uid),
			strerror(errno));
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "name",      pw->pw_name, 1);
	add_assoc_string(return_value, "passwd",    pw->pw_passwd, 1);
	add_assoc_long  (return_value, "uid",       pw->pw_uid);
	add_assoc_long  (return_value, "gid",		pw->pw_gid);
	add_assoc_string(return_value, "gecos",     pw->pw_gecos, 1);
	add_assoc_string(return_value, "dir",       pw->pw_dir, 1);
	add_assoc_string(return_value, "shell",     pw->pw_shell, 1);
}
/* }}} */


#ifdef HAVE_GETRLIMIT

#define UNLIMITED_STRING "unlimited"

/* {{{ posix_addlimit
 */
static int posix_addlimit(int limit, char *name, pval *return_value) {
	int result;
	struct rlimit rl;
	char hard[80];
	char soft[80];

	snprintf(hard, 80, "hard %s", name);
	snprintf(soft, 80, "soft %s", name);

	result = getrlimit(limit, &rl);
	if (result < 0) {
		php_error(E_WARNING, "posix_getrlimit failed to getrlimit(RLIMIT_CORE with '%s'", strerror(errno));
		return FAILURE;
	}

	if (rl.rlim_cur == RLIM_INFINITY) {
		add_assoc_stringl(return_value, soft, UNLIMITED_STRING, sizeof(UNLIMITED_STRING)-1, 1);
	} else {
		add_assoc_long(return_value, soft, rl.rlim_cur);
	}

	if (rl.rlim_max == RLIM_INFINITY) {
		add_assoc_stringl(return_value, hard, UNLIMITED_STRING, sizeof(UNLIMITED_STRING)-1, 1);
	} else {
		add_assoc_long(return_value, hard, rl.rlim_max);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ limits[]
 */
struct limitlist {
	int limit;
	char *name;
} limits[] = {
#ifdef RLIMIT_CORE
	{ RLIMIT_CORE,	"core" },
#endif

#ifdef RLIMIT_DATA
	{ RLIMIT_DATA,	"data" },
#endif

#ifdef RLIMIT_STACK
	{ RLIMIT_STACK,	"stack" },
#endif

#ifdef RLIMIT_VMEM
	{ RLIMIT_VMEM, "virtualmem" },
#endif

#ifdef RLIMIT_AS
	{ RLIMIT_AS, "totalmem" },
#endif

#ifdef RLIMIT_RSS
	{ RLIMIT_RSS, "rss" },
#endif

#ifdef RLIMIT_NPROC
	{ RLIMIT_NPROC, "maxproc" },
#endif

#ifdef RLIMIT_MEMLOCK
	{ RLIMIT_MEMLOCK, "memlock" },
#endif

#ifdef RLIMIT_CPU
	{ RLIMIT_CPU,	"cpu" },
#endif

#ifdef RLIMIT_FSIZE
	{ RLIMIT_FSIZE, "filesize" },
#endif

#ifdef RLIMIT_NOFILE
	{ RLIMIT_NOFILE, "openfiles" },
#endif

#ifdef RLIMIT_OFILE
	{ RLIMIT_OFILE, "openfiles" },
#endif

	{ 0, NULL }
};
/* }}} */

#endif /* HAVE_GETRLIMIT */

/* {{{ proto long posix_getrlimit(void) 
   Get system resource consumption limits (This is not a POSIX function, but a BSDism and a SVR4ism. We compile conditionally) */
PHP_FUNCTION(posix_getrlimit)
{
#ifdef HAVE_GETRLIMIT
	struct limitlist *l = NULL;

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	for (l=limits; l->name; l++) {
		if (posix_addlimit(l->limit, l->name, return_value) == FAILURE)
			RETURN_FALSE;
	}
#else
	RETURN_FALSE;
#endif
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
