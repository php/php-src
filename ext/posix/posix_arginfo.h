/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e7f501e4ca2c30f6bf52ea5912037c1e59743949 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_kill, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, signal, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_getpid, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_posix_getppid arginfo_posix_getpid

#define arginfo_posix_getuid arginfo_posix_getpid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setuid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, user_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_posix_geteuid arginfo_posix_getpid

#if defined(HAVE_SETEUID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_seteuid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, user_id, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_posix_getgid arginfo_posix_getpid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setgid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, group_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_posix_getegid arginfo_posix_getpid

#if defined(HAVE_SETEGID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setegid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, group_id, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, process_group_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GETPGID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getpgid, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETSID)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getsid, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
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
	ZEND_ARG_INFO(0, file_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_isatty, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, file_descriptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getcwd, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#if defined(HAVE_MKFIFO)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_mkfifo, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, permissions, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MKNOD)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_mknod, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, major, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, minor, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_access, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getgrnam, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getgrgid, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, group_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getpwnam, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getpwuid, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, user_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GETRLIMIT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_posix_getrlimit, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SETRLIMIT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_setrlimit, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, resource, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, soft_limit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hard_limit, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_posix_get_last_error arginfo_posix_getpid

#define arginfo_posix_errno arginfo_posix_getpid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_strerror, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_INITGROUPS)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_initgroups, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, group_id, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(posix_kill);
ZEND_FUNCTION(posix_getpid);
ZEND_FUNCTION(posix_getppid);
ZEND_FUNCTION(posix_getuid);
ZEND_FUNCTION(posix_setuid);
ZEND_FUNCTION(posix_geteuid);
#if defined(HAVE_SETEUID)
ZEND_FUNCTION(posix_seteuid);
#endif
ZEND_FUNCTION(posix_getgid);
ZEND_FUNCTION(posix_setgid);
ZEND_FUNCTION(posix_getegid);
#if defined(HAVE_SETEGID)
ZEND_FUNCTION(posix_setegid);
#endif
#if defined(HAVE_GETGROUPS)
ZEND_FUNCTION(posix_getgroups);
#endif
#if defined(HAVE_GETLOGIN)
ZEND_FUNCTION(posix_getlogin);
#endif
ZEND_FUNCTION(posix_getpgrp);
#if defined(HAVE_SETSID)
ZEND_FUNCTION(posix_setsid);
#endif
ZEND_FUNCTION(posix_setpgid);
#if defined(HAVE_GETPGID)
ZEND_FUNCTION(posix_getpgid);
#endif
#if defined(HAVE_GETSID)
ZEND_FUNCTION(posix_getsid);
#endif
ZEND_FUNCTION(posix_uname);
ZEND_FUNCTION(posix_times);
#if defined(HAVE_CTERMID)
ZEND_FUNCTION(posix_ctermid);
#endif
ZEND_FUNCTION(posix_ttyname);
ZEND_FUNCTION(posix_isatty);
ZEND_FUNCTION(posix_getcwd);
#if defined(HAVE_MKFIFO)
ZEND_FUNCTION(posix_mkfifo);
#endif
#if defined(HAVE_MKNOD)
ZEND_FUNCTION(posix_mknod);
#endif
ZEND_FUNCTION(posix_access);
ZEND_FUNCTION(posix_getgrnam);
ZEND_FUNCTION(posix_getgrgid);
ZEND_FUNCTION(posix_getpwnam);
ZEND_FUNCTION(posix_getpwuid);
#if defined(HAVE_GETRLIMIT)
ZEND_FUNCTION(posix_getrlimit);
#endif
#if defined(HAVE_SETRLIMIT)
ZEND_FUNCTION(posix_setrlimit);
#endif
ZEND_FUNCTION(posix_get_last_error);
ZEND_FUNCTION(posix_strerror);
#if defined(HAVE_INITGROUPS)
ZEND_FUNCTION(posix_initgroups);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_FE(posix_kill, arginfo_posix_kill)
	ZEND_FE(posix_getpid, arginfo_posix_getpid)
	ZEND_FE(posix_getppid, arginfo_posix_getppid)
	ZEND_FE(posix_getuid, arginfo_posix_getuid)
	ZEND_FE(posix_setuid, arginfo_posix_setuid)
	ZEND_FE(posix_geteuid, arginfo_posix_geteuid)
#if defined(HAVE_SETEUID)
	ZEND_FE(posix_seteuid, arginfo_posix_seteuid)
#endif
	ZEND_FE(posix_getgid, arginfo_posix_getgid)
	ZEND_FE(posix_setgid, arginfo_posix_setgid)
	ZEND_FE(posix_getegid, arginfo_posix_getegid)
#if defined(HAVE_SETEGID)
	ZEND_FE(posix_setegid, arginfo_posix_setegid)
#endif
#if defined(HAVE_GETGROUPS)
	ZEND_FE(posix_getgroups, arginfo_posix_getgroups)
#endif
#if defined(HAVE_GETLOGIN)
	ZEND_FE(posix_getlogin, arginfo_posix_getlogin)
#endif
	ZEND_FE(posix_getpgrp, arginfo_posix_getpgrp)
#if defined(HAVE_SETSID)
	ZEND_FE(posix_setsid, arginfo_posix_setsid)
#endif
	ZEND_FE(posix_setpgid, arginfo_posix_setpgid)
#if defined(HAVE_GETPGID)
	ZEND_FE(posix_getpgid, arginfo_posix_getpgid)
#endif
#if defined(HAVE_GETSID)
	ZEND_FE(posix_getsid, arginfo_posix_getsid)
#endif
	ZEND_FE(posix_uname, arginfo_posix_uname)
	ZEND_FE(posix_times, arginfo_posix_times)
#if defined(HAVE_CTERMID)
	ZEND_FE(posix_ctermid, arginfo_posix_ctermid)
#endif
	ZEND_FE(posix_ttyname, arginfo_posix_ttyname)
	ZEND_FE(posix_isatty, arginfo_posix_isatty)
	ZEND_FE(posix_getcwd, arginfo_posix_getcwd)
#if defined(HAVE_MKFIFO)
	ZEND_FE(posix_mkfifo, arginfo_posix_mkfifo)
#endif
#if defined(HAVE_MKNOD)
	ZEND_FE(posix_mknod, arginfo_posix_mknod)
#endif
	ZEND_FE(posix_access, arginfo_posix_access)
	ZEND_FE(posix_getgrnam, arginfo_posix_getgrnam)
	ZEND_FE(posix_getgrgid, arginfo_posix_getgrgid)
	ZEND_FE(posix_getpwnam, arginfo_posix_getpwnam)
	ZEND_FE(posix_getpwuid, arginfo_posix_getpwuid)
#if defined(HAVE_GETRLIMIT)
	ZEND_FE(posix_getrlimit, arginfo_posix_getrlimit)
#endif
#if defined(HAVE_SETRLIMIT)
	ZEND_FE(posix_setrlimit, arginfo_posix_setrlimit)
#endif
	ZEND_FE(posix_get_last_error, arginfo_posix_get_last_error)
	ZEND_FALIAS(posix_errno, posix_get_last_error, arginfo_posix_errno)
	ZEND_FE(posix_strerror, arginfo_posix_strerror)
#if defined(HAVE_INITGROUPS)
	ZEND_FE(posix_initgroups, arginfo_posix_initgroups)
#endif
	ZEND_FE_END
};

static void register_posix_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("POSIX_F_OK", F_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POSIX_X_OK", X_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POSIX_W_OK", W_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POSIX_R_OK", R_OK, CONST_PERSISTENT);
#if defined(S_IFREG)
	REGISTER_LONG_CONSTANT("POSIX_S_IFREG", S_IFREG, CONST_PERSISTENT);
#endif
#if defined(S_IFCHR)
	REGISTER_LONG_CONSTANT("POSIX_S_IFCHR", S_IFCHR, CONST_PERSISTENT);
#endif
#if defined(S_IFBLK)
	REGISTER_LONG_CONSTANT("POSIX_S_IFBLK", S_IFBLK, CONST_PERSISTENT);
#endif
#if defined(S_IFIFO)
	REGISTER_LONG_CONSTANT("POSIX_S_IFIFO", S_IFIFO, CONST_PERSISTENT);
#endif
#if defined(S_IFSOCK)
	REGISTER_LONG_CONSTANT("POSIX_S_IFSOCK", S_IFSOCK, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_AS)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_AS", RLIMIT_AS, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_CORE)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_CORE", RLIMIT_CORE, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_CPU)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_CPU", RLIMIT_CPU, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_DATA)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_DATA", RLIMIT_DATA, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_FSIZE)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_FSIZE", RLIMIT_FSIZE, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_LOCKS)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_LOCKS", RLIMIT_LOCKS, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_MEMLOCK)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_MEMLOCK", RLIMIT_MEMLOCK, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_MSGQUEUE)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_MSGQUEUE", RLIMIT_MSGQUEUE, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_NICE)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_NICE", RLIMIT_NICE, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_NOFILE)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_NOFILE", RLIMIT_NOFILE, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_NPROC)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_NPROC", RLIMIT_NPROC, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_RSS)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_RSS", RLIMIT_RSS, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_RTPRIO)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_RTPRIO", RLIMIT_RTPRIO, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_RTTIME)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_RTTIME", RLIMIT_RTTIME, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_SIGPENDING)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_SIGPENDING", RLIMIT_SIGPENDING, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_STACK)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_STACK", RLIMIT_STACK, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_KQUEUES)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_KQUEUES", RLIMIT_KQUEUES, CONST_PERSISTENT);
#endif
#if defined(RLIMIT_NPTS)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_NPTS", RLIMIT_NPTS, CONST_PERSISTENT);
#endif
#if defined(HAVE_SETRLIMIT)
	REGISTER_LONG_CONSTANT("POSIX_RLIMIT_INFINITY", RLIM_INFINITY, CONST_PERSISTENT);
#endif
}
