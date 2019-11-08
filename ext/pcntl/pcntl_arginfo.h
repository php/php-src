/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_fork, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_waitpid, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
	ZEND_ARG_INFO(1, status)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_INFO(1, rusage)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_wait, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(1, status)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_INFO(1, rusage)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_signal, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, signo, IS_LONG, 0)
	ZEND_ARG_INFO(0, handler)
	ZEND_ARG_TYPE_INFO(0, restart_syscalls, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pcntl_signal_get_handler, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, signo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_signal_dispatch, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_SIGPROCMASK)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_sigprocmask, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, how, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, set, IS_ARRAY, 0)
	ZEND_ARG_INFO(1, oldset)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_STRUCT_SIGINFO_T) && HAVE_SIGWAITINFO && HAVE_SIGTIMEDWAIT
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_sigwaitinfo, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, set, IS_ARRAY, 0)
	ZEND_ARG_INFO(1, info)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_STRUCT_SIGINFO_T) && HAVE_SIGWAITINFO && HAVE_SIGTIMEDWAIT
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_sigtimedwait, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, set, IS_ARRAY, 0)
	ZEND_ARG_INFO(1, info)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nanoseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_wifexited, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, status, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pcntl_wifstopped arginfo_pcntl_wifexited

#if defined(HAVE_WCONTINUED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_wifcontinued, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, status, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_pcntl_wifsignaled arginfo_pcntl_wifexited

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_wifexitstatus, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, status, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pcntl_wtermsig arginfo_pcntl_wifexitstatus

#define arginfo_pcntl_wstopsig arginfo_pcntl_wifexitstatus

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_exec, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, args, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, envs, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_alarm, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pcntl_get_last_error arginfo_pcntl_fork

#if defined(HAVE_GETPRIORITY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_getpriority, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, process_identifier, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SETPRIORITY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_setpriority, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, process_identifier, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_strerror, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errno, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_async_signals, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, on, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_UNSHARE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_unshare, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif
