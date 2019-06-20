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
   | Author: Kristian Koehntopp <kris@koehntopp.de>                       |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include <unistd.h>
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_posix.h"


#if HAVE_POSIX

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/times.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#if HAVE_SYS_MKDEV_H
# include <sys/mkdev.h>
#endif
#if HAVE_SYS_SYSMACROS_H
# include <sys/sysmacros.h>
#endif

ZEND_DECLARE_MODULE_GLOBALS(posix)
static PHP_MINFO_FUNCTION(posix);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_kill, 0, 0, 2)
	ZEND_ARG_INFO(0, pid)
	ZEND_ARG_INFO(0, sig)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_posix_getpid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_posix_getppid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_posix_getuid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_setuid, 0, 0, 1)
	ZEND_ARG_INFO(0, uid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_posix_geteuid, 0)
ZEND_END_ARG_INFO()

#ifdef HAVE_SETEUID
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_seteuid, 0, 0, 1)
	ZEND_ARG_INFO(0, uid)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_posix_getgid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_setgid, 0, 0, 1)
	ZEND_ARG_INFO(0, gid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_posix_getegid, 0)
ZEND_END_ARG_INFO()

#ifdef HAVE_SETEGID
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_setegid, 0, 0, 1)
	ZEND_ARG_INFO(0, gid)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_GETGROUPS
ZEND_BEGIN_ARG_INFO(arginfo_posix_getgroups, 0)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_GETLOGIN
ZEND_BEGIN_ARG_INFO(arginfo_posix_getlogin, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_posix_getpgrp, 0)
ZEND_END_ARG_INFO()

#ifdef HAVE_SETSID
ZEND_BEGIN_ARG_INFO(arginfo_posix_setsid, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_setpgid, 0, 0, 2)
	ZEND_ARG_INFO(0, pid)
	ZEND_ARG_INFO(0, pgid)
ZEND_END_ARG_INFO()

#ifdef HAVE_GETPGID
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_getpgid, 0, 0, 1)
	ZEND_ARG_INFO(0, pid)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_GETSID
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_getsid, 0, 0, 1)
	ZEND_ARG_INFO(0, pid)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_posix_uname, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_posix_times, 0)
ZEND_END_ARG_INFO()

#ifdef HAVE_CTERMID
ZEND_BEGIN_ARG_INFO(arginfo_posix_ctermid, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_ttyname, 0, 0, 1)
	ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_isatty, 0, 0, 1)
	ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_posix_getcwd, 0)
ZEND_END_ARG_INFO()

#ifdef HAVE_MKFIFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_mkfifo, 0, 0, 2)
	ZEND_ARG_INFO(0, pathname)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_MKNOD
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_mknod, 0, 0, 2)
	ZEND_ARG_INFO(0, pathname)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, major)
	ZEND_ARG_INFO(0, minor)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_access, 0, 0, 1)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_getgrnam, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_getgrgid, 0, 0, 1)
	ZEND_ARG_INFO(0, gid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_getpwnam, 0, 0, 1)
	ZEND_ARG_INFO(0, username)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_getpwuid, 0, 0, 1)
	ZEND_ARG_INFO(0, uid)
ZEND_END_ARG_INFO()

#ifdef HAVE_GETRLIMIT
ZEND_BEGIN_ARG_INFO(arginfo_posix_getrlimit, 0)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_SETRLIMIT
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_setrlimit, 0, 0, 3)
	ZEND_ARG_INFO(0, resource)
	ZEND_ARG_INFO(0, softlimit)
	ZEND_ARG_INFO(0, hardlimit)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_posix_get_last_error, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_strerror, 0, 0, 1)
	ZEND_ARG_INFO(0, errno)
ZEND_END_ARG_INFO()

#ifdef HAVE_INITGROUPS
ZEND_BEGIN_ARG_INFO_EX(arginfo_posix_initgroups, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, base_group_id)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/* {{{ posix_functions[]
 */
static const zend_function_entry posix_functions[] = {
    /* POSIX.1, 3.3 */
	PHP_FE(posix_kill,		arginfo_posix_kill)

	/* POSIX.1, 4.1 */
	PHP_FE(posix_getpid,	arginfo_posix_getpid)
	PHP_FE(posix_getppid,	arginfo_posix_getppid)

	/* POSIX.1,  4.2 */
	PHP_FE(posix_getuid,	arginfo_posix_getuid)
	PHP_FE(posix_setuid,	arginfo_posix_setuid)
	PHP_FE(posix_geteuid,	arginfo_posix_geteuid)
#ifdef HAVE_SETEUID
	PHP_FE(posix_seteuid,	arginfo_posix_seteuid)
#endif
	PHP_FE(posix_getgid,	arginfo_posix_getgid)
	PHP_FE(posix_setgid,	arginfo_posix_setgid)
	PHP_FE(posix_getegid,	arginfo_posix_getegid)
#ifdef HAVE_SETEGID
	PHP_FE(posix_setegid,	arginfo_posix_setegid)
#endif
#ifdef HAVE_GETGROUPS
	PHP_FE(posix_getgroups,	arginfo_posix_getgroups)
#endif
#ifdef HAVE_GETLOGIN
	PHP_FE(posix_getlogin,	arginfo_posix_getlogin)
#endif

	/* POSIX.1, 4.3 */
	PHP_FE(posix_getpgrp,	arginfo_posix_getpgrp)
#ifdef HAVE_SETSID
	PHP_FE(posix_setsid,	arginfo_posix_setsid)
#endif
	PHP_FE(posix_setpgid,	arginfo_posix_setpgid)
	/* Non-Posix functions which are common */
#ifdef HAVE_GETPGID
	PHP_FE(posix_getpgid,	arginfo_posix_getpgid)
#endif /* HAVE_GETPGID */
#ifdef HAVE_GETSID
	PHP_FE(posix_getsid,	arginfo_posix_getsid)
#endif /* HAVE_GETSID */

	/* POSIX.1, 4.4 */
	PHP_FE(posix_uname,		arginfo_posix_uname)

	/* POSIX.1, 4.5 */
	PHP_FE(posix_times,		arginfo_posix_times)

	/* POSIX.1, 4.7 */
#ifdef HAVE_CTERMID
	PHP_FE(posix_ctermid,	arginfo_posix_ctermid)
#endif
	PHP_FE(posix_ttyname,	arginfo_posix_ttyname)
	PHP_FE(posix_isatty,	arginfo_posix_isatty)

    /* POSIX.1, 5.2 */
	PHP_FE(posix_getcwd,	arginfo_posix_getcwd)

	/* POSIX.1, 5.4 */
#ifdef HAVE_MKFIFO
	PHP_FE(posix_mkfifo,	arginfo_posix_mkfifo)
#endif
#ifdef HAVE_MKNOD
	PHP_FE(posix_mknod,		arginfo_posix_mknod)
#endif

	/* POSIX.1, 5.6 */
	PHP_FE(posix_access,	arginfo_posix_access)
	/* POSIX.1, 9.2 */
	PHP_FE(posix_getgrnam,	arginfo_posix_getgrnam)
	PHP_FE(posix_getgrgid,	arginfo_posix_getgrgid)
	PHP_FE(posix_getpwnam,	arginfo_posix_getpwnam)
	PHP_FE(posix_getpwuid,	arginfo_posix_getpwuid)

#ifdef HAVE_GETRLIMIT
	PHP_FE(posix_getrlimit,	arginfo_posix_getrlimit)
#endif
#ifdef HAVE_SETRLIMIT
	PHP_FE(posix_setrlimit, arginfo_posix_setrlimit)
#endif

	PHP_FE(posix_get_last_error,					arginfo_posix_get_last_error)
	PHP_FALIAS(posix_errno, posix_get_last_error,	arginfo_posix_get_last_error)
	PHP_FE(posix_strerror,							arginfo_posix_strerror)
#ifdef HAVE_INITGROUPS
	PHP_FE(posix_initgroups,	arginfo_posix_initgroups)
#endif

	PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(posix)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "POSIX support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static PHP_GINIT_FUNCTION(posix) /* {{{ */
{
#if defined(COMPILE_DL_POSIX) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	posix_globals->last_error = 0;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(posix)
 */
static PHP_MINIT_FUNCTION(posix)
{
	REGISTER_LONG_CONSTANT("POSIX_F_OK", F_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POSIX_X_OK", X_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POSIX_W_OK", W_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POSIX_R_OK", R_OK, CONST_CS | CONST_PERSISTENT);
#ifdef S_IFREG
	REGISTER_LONG_CONSTANT("POSIX_S_IFREG", S_IFREG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef S_IFCHR
	REGISTER_LONG_CONSTANT("POSIX_S_IFCHR", S_IFCHR, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef S_IFBLK
	REGISTER_LONG_CONSTANT("POSIX_S_IFBLK", S_IFBLK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef S_IFIFO
	REGISTER_LONG_CONSTANT("POSIX_S_IFIFO", S_IFIFO, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef S_IFSOCK
	REGISTER_LONG_CONSTANT("POSIX_S_IFSOCK", S_IFSOCK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_AS
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_AS", RLIMIT_AS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_CORE
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_CORE", RLIMIT_CORE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_CPU
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_CPU", RLIMIT_CPU, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_DATA
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_DATA", RLIMIT_DATA, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_FSIZE
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_FSIZE", RLIMIT_FSIZE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_LOCKS
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_LOCKS", RLIMIT_LOCKS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_MEMLOCK
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_MEMLOCK", RLIMIT_MEMLOCK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_MSGQUEUE
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_MSGQUEUE", RLIMIT_MSGQUEUE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_NICE
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_NICE", RLIMIT_NICE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_NOFILE
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_NOFILE", RLIMIT_NOFILE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_NPROC
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_NPROC", RLIMIT_NPROC, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_RSS
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_RSS", RLIMIT_RSS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_RTPRIO
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_RTPRIO", RLIMIT_RTPRIO, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_RTTIME
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_RTTIME", RLIMIT_RTTIME, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_SIGPENDING
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_SIGPENDING", RLIMIT_SIGPENDING, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef RLIMIT_STACK
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_STACK", RLIMIT_STACK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef HAVE_SETRLIMIT
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_INFINITY", RLIM_INFINITY, CONST_CS | CONST_PERSISTENT);
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ posix_module_entry
 */
zend_module_entry posix_module_entry = {
	STANDARD_MODULE_HEADER,
	"posix",
	posix_functions,
	PHP_MINIT(posix),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(posix),
	PHP_POSIX_VERSION,
	PHP_MODULE_GLOBALS(posix),
	PHP_GINIT(posix),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_POSIX
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(posix)
#endif

#define PHP_POSIX_NO_ARGS	if (zend_parse_parameters_none() == FAILURE) return;

#define PHP_POSIX_RETURN_LONG_FUNC(func_name)	\
	PHP_POSIX_NO_ARGS	\
	RETURN_LONG(func_name());

#define PHP_POSIX_SINGLE_ARG_FUNC(func_name)	\
	zend_long val;	\
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(val) \
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE); \
	if (func_name(val) < 0) {	\
		POSIX_G(last_error) = errno;	\
		RETURN_FALSE;	\
	}	\
	RETURN_TRUE;

/* {{{ proto bool posix_kill(int pid, int sig)
   Send a signal to a process (POSIX.1, 3.3.2) */

PHP_FUNCTION(posix_kill)
{
	zend_long pid, sig;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pid)
		Z_PARAM_LONG(sig)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (kill(pid, sig) < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
  	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int posix_getpid(void)
   Get the current process id (POSIX.1, 4.1.1) */
PHP_FUNCTION(posix_getpid)
{
	PHP_POSIX_RETURN_LONG_FUNC(getpid);
}
/* }}} */

/* {{{ proto int posix_getppid(void)
   Get the parent process id (POSIX.1, 4.1.1) */
PHP_FUNCTION(posix_getppid)
{
	PHP_POSIX_RETURN_LONG_FUNC(getppid);
}
/* }}} */

/* {{{ proto int posix_getuid(void)
   Get the current user id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_getuid)
{
	PHP_POSIX_RETURN_LONG_FUNC(getuid);
}
/* }}} */

/* {{{ proto int posix_getgid(void)
   Get the current group id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_getgid)
{
	PHP_POSIX_RETURN_LONG_FUNC(getgid);
}
/* }}} */

/* {{{ proto int posix_geteuid(void)
   Get the current effective user id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_geteuid)
{
	PHP_POSIX_RETURN_LONG_FUNC(geteuid);
}
/* }}} */

/* {{{ proto int posix_getegid(void)
   Get the current effective group id (POSIX.1, 4.2.1) */
PHP_FUNCTION(posix_getegid)
{
	PHP_POSIX_RETURN_LONG_FUNC(getegid);
}
/* }}} */

/* {{{ proto bool posix_setuid(int uid)
   Set user id (POSIX.1, 4.2.2) */
PHP_FUNCTION(posix_setuid)
{
	PHP_POSIX_SINGLE_ARG_FUNC(setuid);
}
/* }}} */

/* {{{ proto bool posix_setgid(int uid)
   Set group id (POSIX.1, 4.2.2) */
PHP_FUNCTION(posix_setgid)
{
	PHP_POSIX_SINGLE_ARG_FUNC(setgid);
}
/* }}} */

/* {{{ proto bool posix_seteuid(int uid)
   Set effective user id */
#ifdef HAVE_SETEUID
PHP_FUNCTION(posix_seteuid)
{
	PHP_POSIX_SINGLE_ARG_FUNC(seteuid);
}
#endif
/* }}} */

/* {{{ proto bool posix_setegid(int uid)
   Set effective group id */
#ifdef HAVE_SETEGID
PHP_FUNCTION(posix_setegid)
{
	PHP_POSIX_SINGLE_ARG_FUNC(setegid);
}
#endif
/* }}} */

/* {{{ proto array posix_getgroups(void)
   Get supplementary group id's (POSIX.1, 4.2.3) */
#ifdef HAVE_GETGROUPS
PHP_FUNCTION(posix_getgroups)
{
	gid_t  gidlist[NGROUPS_MAX];
	int    result;
	int    i;

	PHP_POSIX_NO_ARGS;

	if ((result = getgroups(NGROUPS_MAX, gidlist)) < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i=0; i<result; i++) {
		add_next_index_long(return_value, gidlist[i]);
	}
}
#endif
/* }}} */

/* {{{ proto string posix_getlogin(void)
   Get user name (POSIX.1, 4.2.4) */
#ifdef HAVE_GETLOGIN
PHP_FUNCTION(posix_getlogin)
{
	char *p;

	PHP_POSIX_NO_ARGS;

	if (NULL == (p = getlogin())) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_STRING(p);
}
#endif
/* }}} */

/* {{{ proto int posix_getpgrp(void)
   Get current process group id (POSIX.1, 4.3.1) */
PHP_FUNCTION(posix_getpgrp)
{
	PHP_POSIX_RETURN_LONG_FUNC(getpgrp);
}
/* }}} */

/* {{{ proto int posix_setsid(void)
   Create session and set process group id (POSIX.1, 4.3.2) */
#ifdef HAVE_SETSID
PHP_FUNCTION(posix_setsid)
{
	PHP_POSIX_RETURN_LONG_FUNC(setsid);
}
#endif
/* }}} */

/* {{{ proto bool posix_setpgid(int pid, int pgid)
   Set process group id for job control (POSIX.1, 4.3.3) */
PHP_FUNCTION(posix_setpgid)
{
	zend_long pid, pgid;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pid)
		Z_PARAM_LONG(pgid)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (setpgid(pid, pgid) < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int posix_getpgid(void)
   Get the process group id of the specified process (This is not a POSIX function, but a SVR4ism, so we compile conditionally) */
#ifdef HAVE_GETPGID
PHP_FUNCTION(posix_getpgid)
{
	zend_long val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(val)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if ((val = getpgid(val)) < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
	RETURN_LONG(val);
}
#endif
/* }}} */

/* {{{ proto int posix_getsid(void)
   Get process group id of session leader (This is not a POSIX function, but a SVR4ism, so be compile conditionally) */
#ifdef HAVE_GETSID
PHP_FUNCTION(posix_getsid)
{
	zend_long val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(val)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if ((val = getsid(val)) < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
	RETURN_LONG(val);
}
#endif
/* }}} */

/* {{{ proto array posix_uname(void)
   Get system name (POSIX.1, 4.4.1) */
PHP_FUNCTION(posix_uname)
{
	struct utsname u;

	PHP_POSIX_NO_ARGS;

	if (uname(&u) < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_string(return_value, "sysname",  u.sysname);
	add_assoc_string(return_value, "nodename", u.nodename);
	add_assoc_string(return_value, "release",  u.release);
	add_assoc_string(return_value, "version",  u.version);
	add_assoc_string(return_value, "machine",  u.machine);

#if defined(_GNU_SOURCE) && !defined(DARWIN) && defined(HAVE_UTSNAME_DOMAINNAME)
	add_assoc_string(return_value, "domainname", u.domainname);
#endif
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

	PHP_POSIX_NO_ARGS;

	if ((ticks = times(&t)) == -1) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_long(return_value, "ticks",	ticks);			/* clock ticks */
	add_assoc_long(return_value, "utime",	t.tms_utime);	/* user time */
	add_assoc_long(return_value, "stime",	t.tms_stime);	/* system time */
	add_assoc_long(return_value, "cutime",	t.tms_cutime);	/* user time of children */
	add_assoc_long(return_value, "cstime",	t.tms_cstime);	/* system time of children */
}
/* }}} */

/* POSIX.1, 4.6.1 getenv() - Environment Access
							already covered by PHP
*/

/* {{{ proto string posix_ctermid(void)
   Generate terminal path name (POSIX.1, 4.7.1) */
#ifdef HAVE_CTERMID
PHP_FUNCTION(posix_ctermid)
{
	char  buffer[L_ctermid];

	PHP_POSIX_NO_ARGS;

	if (NULL == ctermid(buffer)) {
		/* Found no documentation how the defined behaviour is when this
		 * function fails
		 */
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_STRING(buffer);
}
#endif
/* }}} */

/* Checks if the provides resource is a stream and if it provides a file descriptor */
static int php_posix_stream_get_fd(zval *zfp, int *fd) /* {{{ */
{
	php_stream *stream;

	php_stream_from_zval_no_verify(stream, zfp);

	if (stream == NULL) {
		php_error_docref(NULL, E_WARNING, "expects argument 1 to be a valid stream resource");
		return 0;
	}
	if (php_stream_can_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT) == SUCCESS) {
		php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT, (void*)fd, 0);
	} else if (php_stream_can_cast(stream, PHP_STREAM_AS_FD) == SUCCESS) {
		php_stream_cast(stream, PHP_STREAM_AS_FD, (void*)fd, 0);
	} else {
		php_error_docref(NULL, E_WARNING, "could not use stream of type '%s'",
				stream->ops->label);
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ proto string posix_ttyname(int fd)
   Determine terminal device name (POSIX.1, 4.7.2) */
PHP_FUNCTION(posix_ttyname)
{
	zval *z_fd;
	char *p;
	int fd;
#if defined(ZTS) && defined(HAVE_TTYNAME_R) && defined(_SC_TTY_NAME_MAX)
	zend_long buflen;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(z_fd)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	switch (Z_TYPE_P(z_fd)) {
		case IS_RESOURCE:
			if (!php_posix_stream_get_fd(z_fd, &fd)) {
				RETURN_FALSE;
			}
			break;
		default:
			fd = zval_get_long(z_fd);
	}
#if defined(ZTS) && defined(HAVE_TTYNAME_R) && defined(_SC_TTY_NAME_MAX)
	buflen = sysconf(_SC_TTY_NAME_MAX);
	if (buflen < 1) {
		RETURN_FALSE;
	}
	p = emalloc(buflen);

	if (ttyname_r(fd, p, buflen)) {
		POSIX_G(last_error) = errno;
		efree(p);
		RETURN_FALSE;
	}
	RETURN_STRING(p);
	efree(p);
#else
	if (NULL == (p = ttyname(fd))) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
#endif
	RETURN_STRING(p);
}
/* }}} */

/* {{{ proto bool posix_isatty(int fd)
   Determine if filedesc is a tty (POSIX.1, 4.7.1) */
PHP_FUNCTION(posix_isatty)
{
	zval *z_fd;
	int fd;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(z_fd)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	switch (Z_TYPE_P(z_fd)) {
		case IS_RESOURCE:
			if (!php_posix_stream_get_fd(z_fd, &fd)) {
				RETURN_FALSE;
			}
			break;
		default:
			fd = zval_get_long(z_fd);
	}

	if (isatty(fd)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
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

	PHP_POSIX_NO_ARGS;

	p = VCWD_GETCWD(buffer, MAXPATHLEN);
	if (!p) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_STRING(buffer);
}
/* }}} */

/*
	POSIX.1, 5.3.x open(), creat(), umask()
	POSIX.1, 5.4.1 link()
		already supported by PHP.
 */

/* {{{ proto bool posix_mkfifo(string pathname, int mode)
   Make a FIFO special file (POSIX.1, 5.4.2) */
#ifdef HAVE_MKFIFO
PHP_FUNCTION(posix_mkfifo)
{
	zend_string *path;
	zend_long mode;
	int     result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH_STR(path)
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (php_check_open_basedir_ex(ZSTR_VAL(path), 0)) {
		RETURN_FALSE;
	}

	result = mkfifo(ZSTR_VAL(path), mode);
	if (result < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
#endif
/* }}} */

/* {{{ proto bool posix_mknod(string pathname, int mode [, int major [, int minor]])
   Make a special or ordinary file (POSIX.1) */
#ifdef HAVE_MKNOD
PHP_FUNCTION(posix_mknod)
{
	zend_string *path;
	zend_long mode;
	zend_long major = 0, minor = 0;
	int result;
	dev_t php_dev = 0;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_PATH_STR(path)
		Z_PARAM_LONG(mode)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(major)
		Z_PARAM_LONG(minor)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (php_check_open_basedir_ex(ZSTR_VAL(path), 0)) {
		RETURN_FALSE;
	}

	if ((mode & S_IFCHR) || (mode & S_IFBLK)) {
		if (ZEND_NUM_ARGS() == 2) {
			php_error_docref(NULL, E_WARNING, "For S_IFCHR and S_IFBLK you need to pass a major device kernel identifier");
			RETURN_FALSE;
		}
		if (major == 0) {
			php_error_docref(NULL, E_WARNING,
				"Expects argument 3 to be non-zero for POSIX_S_IFCHR and POSIX_S_IFBLK");
			RETURN_FALSE;
		} else {
#if defined(HAVE_MAKEDEV) || defined(makedev)
			php_dev = makedev(major, minor);
#else
			php_error_docref(NULL, E_WARNING, "Cannot create a block or character device, creating a normal file instead");
#endif
		}
	}

	result = mknod(ZSTR_VAL(path), mode, php_dev);
	if (result < 0) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
#endif
/* }}} */

/* Takes a pointer to posix group and a pointer to an already initialized ZVAL
 * array container and fills the array with the posix group member data. */
int php_posix_group_to_array(struct group *g, zval *array_group) /* {{{ */
{
	zval array_members;
	int count;

	if (NULL == g)
		return 0;

	if (array_group == NULL || Z_TYPE_P(array_group) != IS_ARRAY)
		return 0;

	array_init(&array_members);

	add_assoc_string(array_group, "name", g->gr_name);
	if (g->gr_passwd) {
		add_assoc_string(array_group, "passwd", g->gr_passwd);
	} else {
		add_assoc_null(array_group, "passwd");
	}
	for (count = 0;; count++) {
		/* gr_mem entries may be misaligned on macos. */
		char *gr_mem;
		memcpy(&gr_mem, &g->gr_mem[count], sizeof(char *));
		if (!gr_mem) {
			break;
		}

		add_next_index_string(&array_members, gr_mem);
	}
	zend_hash_str_update(Z_ARRVAL_P(array_group), "members", sizeof("members")-1, &array_members);
	add_assoc_long(array_group, "gid", g->gr_gid);
	return 1;
}
/* }}} */

/*
	POSIX.1, 5.5.1 unlink()
	POSIX.1, 5.5.2 rmdir()
	POSIX.1, 5.5.3 rename()
	POSIX.1, 5.6.x stat(), chmod(), utime() already supported by PHP.
*/

/* {{{ proto bool posix_access(string file [, int mode])
   Determine accessibility of a file (POSIX.1 5.6.3) */
PHP_FUNCTION(posix_access)
{
	zend_long mode = 0;
	size_t filename_len, ret;
	char *filename, *path;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	path = expand_filepath(filename, NULL);
	if (!path) {
		POSIX_G(last_error) = EIO;
		RETURN_FALSE;
	}

	if (php_check_open_basedir_ex(path, 0)) {
		efree(path);
		POSIX_G(last_error) = EPERM;
		RETURN_FALSE;
	}

	ret = access(path, mode);
	efree(path);

	if (ret) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/*
	POSIX.1, 6.x most I/O functions already supported by PHP.
	POSIX.1, 7.x tty functions, TODO
	POSIX.1, 8.x interactions with other C language functions
	POSIX.1, 9.x system database access
*/

/* {{{ proto array posix_getgrnam(string groupname)
   Group database access (POSIX.1, 9.2.1) */
PHP_FUNCTION(posix_getgrnam)
{
	char *name;
	struct group *g;
	size_t name_len;
#if defined(ZTS) && defined(HAVE_GETGRNAM_R) && defined(_SC_GETGR_R_SIZE_MAX)
	struct group gbuf;
	long buflen;
	char *buf;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

#if defined(ZTS) && defined(HAVE_GETGRNAM_R) && defined(_SC_GETGR_R_SIZE_MAX)
	buflen = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (buflen < 1) {
		RETURN_FALSE;
	}
	buf = emalloc(buflen);
try_again:
	g = &gbuf;

	if (getgrnam_r(name, g, buf, buflen, &g) || g == NULL) {
		if (errno == ERANGE) {
			buflen *= 2;
			buf = erealloc(buf, buflen);
			goto try_again;
		}
		POSIX_G(last_error) = errno;
		efree(buf);
		RETURN_FALSE;
	}
#else
	if (NULL == (g = getgrnam(name))) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
#endif
	array_init(return_value);

	if (!php_posix_group_to_array(g, return_value)) {
		zend_array_destroy(Z_ARR_P(return_value));
		php_error_docref(NULL, E_WARNING, "unable to convert posix group to array");
		RETVAL_FALSE;
	}
#if defined(ZTS) && defined(HAVE_GETGRNAM_R) && defined(_SC_GETGR_R_SIZE_MAX)
	efree(buf);
#endif
}
/* }}} */

/* {{{ proto array posix_getgrgid(int gid)
   Group database access (POSIX.1, 9.2.1) */
PHP_FUNCTION(posix_getgrgid)
{
	zend_long gid;
#if defined(ZTS) && defined(HAVE_GETGRGID_R) && defined(_SC_GETGR_R_SIZE_MAX)
	int ret;
	struct group _g;
	struct group *retgrptr = NULL;
	long grbuflen;
	char *grbuf;
#endif
	struct group *g;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(gid)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

#if defined(ZTS) && defined(HAVE_GETGRGID_R) && defined(_SC_GETGR_R_SIZE_MAX)

	grbuflen = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (grbuflen < 1) {
		RETURN_FALSE;
	}

	grbuf = emalloc(grbuflen);

	ret = getgrgid_r(gid, &_g, grbuf, grbuflen, &retgrptr);
	if (ret || retgrptr == NULL) {
		POSIX_G(last_error) = ret;
		efree(grbuf);
		RETURN_FALSE;
	}
	g = &_g;
#else
	if (NULL == (g = getgrgid(gid))) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
#endif
	array_init(return_value);

	if (!php_posix_group_to_array(g, return_value)) {
		zend_array_destroy(Z_ARR_P(return_value));
		php_error_docref(NULL, E_WARNING, "unable to convert posix group struct to array");
		RETVAL_FALSE;
	}
#if defined(ZTS) && defined(HAVE_GETGRGID_R) && defined(_SC_GETGR_R_SIZE_MAX)
	efree(grbuf);
#endif
}
/* }}} */

int php_posix_passwd_to_array(struct passwd *pw, zval *return_value) /* {{{ */
{
	if (NULL == pw)
		return 0;
	if (NULL == return_value || Z_TYPE_P(return_value) != IS_ARRAY)
		return 0;

	add_assoc_string(return_value, "name",      pw->pw_name);
	add_assoc_string(return_value, "passwd",    pw->pw_passwd);
	add_assoc_long  (return_value, "uid",       pw->pw_uid);
	add_assoc_long  (return_value, "gid",		pw->pw_gid);
	add_assoc_string(return_value, "gecos",     pw->pw_gecos);
	add_assoc_string(return_value, "dir",       pw->pw_dir);
	add_assoc_string(return_value, "shell",     pw->pw_shell);
	return 1;
}
/* }}} */

/* {{{ proto array posix_getpwnam(string groupname)
   User database access (POSIX.1, 9.2.2) */
PHP_FUNCTION(posix_getpwnam)
{
	struct passwd *pw;
	char *name;
	size_t name_len;
#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWNAM_R)
	struct passwd pwbuf;
	long buflen;
	char *buf;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWNAM_R)
	buflen = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (buflen < 1) {
		RETURN_FALSE;
	}
	buf = emalloc(buflen);
	pw = &pwbuf;

	if (getpwnam_r(name, pw, buf, buflen, &pw) || pw == NULL) {
		efree(buf);
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
#else
	if (NULL == (pw = getpwnam(name))) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
#endif
	array_init(return_value);

	if (!php_posix_passwd_to_array(pw, return_value)) {
		zend_array_destroy(Z_ARR_P(return_value));
		php_error_docref(NULL, E_WARNING, "unable to convert posix passwd struct to array");
		RETVAL_FALSE;
	}
#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWNAM_R)
	efree(buf);
#endif
}
/* }}} */

/* {{{ proto array posix_getpwuid(int uid)
   User database access (POSIX.1, 9.2.2) */
PHP_FUNCTION(posix_getpwuid)
{
	zend_long uid;
#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWUID_R)
	struct passwd _pw;
	struct passwd *retpwptr = NULL;
	long pwbuflen;
	char *pwbuf;
	int ret;
#endif
	struct passwd *pw;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(uid)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWUID_R)
	pwbuflen = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (pwbuflen < 1) {
		RETURN_FALSE;
	}
	pwbuf = emalloc(pwbuflen);

	ret = getpwuid_r(uid, &_pw, pwbuf, pwbuflen, &retpwptr);
	if (ret || retpwptr == NULL) {
		POSIX_G(last_error) = ret;
		efree(pwbuf);
		RETURN_FALSE;
	}
	pw = &_pw;
#else
	if (NULL == (pw = getpwuid(uid))) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}
#endif
	array_init(return_value);

	if (!php_posix_passwd_to_array(pw, return_value)) {
		zend_array_destroy(Z_ARR_P(return_value));
		php_error_docref(NULL, E_WARNING, "unable to convert posix passwd struct to array");
		RETVAL_FALSE;
	}
#if defined(ZTS) && defined(_SC_GETPW_R_SIZE_MAX) && defined(HAVE_GETPWUID_R)
	efree(pwbuf);
#endif
}
/* }}} */


#ifdef HAVE_GETRLIMIT

#define UNLIMITED_STRING "unlimited"

/* {{{ posix_addlimit
 */
static int posix_addlimit(int limit, const char *name, zval *return_value) {
	int result;
	struct rlimit rl;
	char hard[80];
	char soft[80];

	snprintf(hard, 80, "hard %s", name);
	snprintf(soft, 80, "soft %s", name);

	result = getrlimit(limit, &rl);
	if (result < 0) {
		POSIX_G(last_error) = errno;
		return FAILURE;
	}

	if (rl.rlim_cur == RLIM_INFINITY) {
		add_assoc_stringl(return_value, soft, UNLIMITED_STRING, sizeof(UNLIMITED_STRING)-1);
	} else {
		add_assoc_long(return_value, soft, rl.rlim_cur);
	}

	if (rl.rlim_max == RLIM_INFINITY) {
		add_assoc_stringl(return_value, hard, UNLIMITED_STRING, sizeof(UNLIMITED_STRING)-1);
	} else {
		add_assoc_long(return_value, hard, rl.rlim_max);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ limits[]
 */
static const struct limitlist {
	int limit;
	const char *name;
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


/* {{{ proto array posix_getrlimit(void)
   Get system resource consumption limits (This is not a POSIX function, but a BSDism and a SVR4ism. We compile conditionally) */
PHP_FUNCTION(posix_getrlimit)
{
	const struct limitlist *l = NULL;

	PHP_POSIX_NO_ARGS;

	array_init(return_value);

	for (l=limits; l->name; l++) {
		if (posix_addlimit(l->limit, l->name, return_value) == FAILURE) {
			zend_array_destroy(Z_ARR_P(return_value));
			RETURN_FALSE;
		}
	}
}
/* }}} */

#endif /* HAVE_GETRLIMIT */

#ifdef HAVE_SETRLIMIT
/* {{{ proto bool posix_setrlimit(int resource, int softlimit, int hardlimit)
   Set system resource consumption limits (POSIX.1-2001) */
PHP_FUNCTION(posix_setrlimit)
{
	struct rlimit rl;
	zend_long res, cur, max;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(res)
		Z_PARAM_LONG(cur)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	rl.rlim_cur = cur;
	rl.rlim_max = max;

	if (setrlimit(res, &rl) == -1) {
		POSIX_G(last_error) = errno;
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_SETRLIMIT */


/* {{{ proto int posix_get_last_error(void)
   Retrieve the error number set by the last posix function which failed. */
PHP_FUNCTION(posix_get_last_error)
{
	PHP_POSIX_NO_ARGS;

	RETURN_LONG(POSIX_G(last_error));
}
/* }}} */

/* {{{ proto string posix_strerror(int errno)
   Retrieve the system error message associated with the given errno. */
PHP_FUNCTION(posix_strerror)
{
	zend_long error;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(error)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	RETURN_STRING(strerror(error));
}
/* }}} */

#endif

#ifdef HAVE_INITGROUPS
/* {{{ proto bool posix_initgroups(string name, int base_group_id)
   Calculate the group access list for the user specified in name. */
PHP_FUNCTION(posix_initgroups)
{
	zend_long basegid;
	char *name;
	size_t name_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(name, name_len)
		Z_PARAM_LONG(basegid)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (name_len == 0) {
		RETURN_FALSE;
	}

	RETURN_BOOL(!initgroups((const char *)name, basegid));
}
/* }}} */
#endif
