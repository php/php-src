/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_kill, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, sig, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_getpid, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_posix_getppid arginfo_posix_getpid

#define arginfo_posix_getuid arginfo_posix_getpid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setuid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, uid, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_posix_geteuid arginfo_posix_getpid

#if defined(HAVE_SETEUID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_seteuid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, uid, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_posix_getgid arginfo_posix_getpid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setgid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, gid, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_posix_getegid arginfo_posix_getpid

#if defined(HAVE_SETEGID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setegid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, gid, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETGROUPS)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getgroups, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETLOGIN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getlogin, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

#define arginfo_posix_getpgrp arginfo_posix_getpid

#if defined(HAVE_SETSID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setsid, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setpgid, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pgid, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GETPGID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getpgid, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETSID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getsid, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_uname, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_posix_times arginfo_posix_uname

#if defined(HAVE_CTERMID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_ctermid, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_ttyname, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_isatty, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getcwd, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#if defined(HAVE_MKFIFO)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_mkfifo, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pathname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MKNOD)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_mknod, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pathname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, major, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, minor, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_access, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getgrnam, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getgrgid, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, gid, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getpwnam, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getpwuid, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, uid, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GETRLIMIT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getrlimit, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SETRLIMIT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setrlimit, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, resource, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, softlimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hardlimit, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_posix_get_last_error arginfo_posix_getpid

#define arginfo_posix_errno arginfo_posix_getpid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_strerror, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errno, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_INITGROUPS)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_initgroups, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, base_group_id, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif
