/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8742901e9b4fe5ee595a1e7c492474723f95d253 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_fork, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_waitpid, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
	ZEND_ARG_INFO(1, status)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, resource_usage, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_wait, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(1, status)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, resource_usage, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_signal, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, signal, IS_LONG, 0)
	ZEND_ARG_INFO(0, handler)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, restart_syscalls, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pcntl_signal_get_handler, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, signal, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_signal_dispatch, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_SIGPROCMASK)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_sigprocmask, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, signals, IS_ARRAY, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, old_signals, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_STRUCT_SIGINFO_T) && defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_sigwaitinfo, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, signals, IS_ARRAY, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, info, "[]")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_STRUCT_SIGINFO_T) && defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_sigtimedwait, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, signals, IS_ARRAY, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, info, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seconds, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nanoseconds, IS_LONG, 0, "0")
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_wexitstatus, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, status, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pcntl_wtermsig arginfo_pcntl_wexitstatus

#define arginfo_pcntl_wstopsig arginfo_pcntl_wexitstatus

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_exec, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, env_vars, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_alarm, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pcntl_get_last_error arginfo_pcntl_fork

#define arginfo_pcntl_errno arginfo_pcntl_fork

#if defined(HAVE_GETPRIORITY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcntl_getpriority, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, process_id, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PRIO_PROCESS")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SETPRIORITY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_setpriority, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, process_id, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PRIO_PROCESS")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_strerror, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_async_signals, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

#if defined(HAVE_UNSHARE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcntl_unshare, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(pcntl_fork);
ZEND_FUNCTION(pcntl_waitpid);
ZEND_FUNCTION(pcntl_wait);
ZEND_FUNCTION(pcntl_signal);
ZEND_FUNCTION(pcntl_signal_get_handler);
ZEND_FUNCTION(pcntl_signal_dispatch);
#if defined(HAVE_SIGPROCMASK)
ZEND_FUNCTION(pcntl_sigprocmask);
#endif
#if defined(HAVE_STRUCT_SIGINFO_T) && defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
ZEND_FUNCTION(pcntl_sigwaitinfo);
#endif
#if defined(HAVE_STRUCT_SIGINFO_T) && defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
ZEND_FUNCTION(pcntl_sigtimedwait);
#endif
ZEND_FUNCTION(pcntl_wifexited);
ZEND_FUNCTION(pcntl_wifstopped);
#if defined(HAVE_WCONTINUED)
ZEND_FUNCTION(pcntl_wifcontinued);
#endif
ZEND_FUNCTION(pcntl_wifsignaled);
ZEND_FUNCTION(pcntl_wexitstatus);
ZEND_FUNCTION(pcntl_wtermsig);
ZEND_FUNCTION(pcntl_wstopsig);
ZEND_FUNCTION(pcntl_exec);
ZEND_FUNCTION(pcntl_alarm);
ZEND_FUNCTION(pcntl_get_last_error);
#if defined(HAVE_GETPRIORITY)
ZEND_FUNCTION(pcntl_getpriority);
#endif
#if defined(HAVE_SETPRIORITY)
ZEND_FUNCTION(pcntl_setpriority);
#endif
ZEND_FUNCTION(pcntl_strerror);
ZEND_FUNCTION(pcntl_async_signals);
#if defined(HAVE_UNSHARE)
ZEND_FUNCTION(pcntl_unshare);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_FE(pcntl_fork, arginfo_pcntl_fork)
	ZEND_FE(pcntl_waitpid, arginfo_pcntl_waitpid)
	ZEND_FE(pcntl_wait, arginfo_pcntl_wait)
	ZEND_FE(pcntl_signal, arginfo_pcntl_signal)
	ZEND_FE(pcntl_signal_get_handler, arginfo_pcntl_signal_get_handler)
	ZEND_FE(pcntl_signal_dispatch, arginfo_pcntl_signal_dispatch)
#if defined(HAVE_SIGPROCMASK)
	ZEND_FE(pcntl_sigprocmask, arginfo_pcntl_sigprocmask)
#endif
#if defined(HAVE_STRUCT_SIGINFO_T) && defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
	ZEND_FE(pcntl_sigwaitinfo, arginfo_pcntl_sigwaitinfo)
#endif
#if defined(HAVE_STRUCT_SIGINFO_T) && defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
	ZEND_FE(pcntl_sigtimedwait, arginfo_pcntl_sigtimedwait)
#endif
	ZEND_FE(pcntl_wifexited, arginfo_pcntl_wifexited)
	ZEND_FE(pcntl_wifstopped, arginfo_pcntl_wifstopped)
#if defined(HAVE_WCONTINUED)
	ZEND_FE(pcntl_wifcontinued, arginfo_pcntl_wifcontinued)
#endif
	ZEND_FE(pcntl_wifsignaled, arginfo_pcntl_wifsignaled)
	ZEND_FE(pcntl_wexitstatus, arginfo_pcntl_wexitstatus)
	ZEND_FE(pcntl_wtermsig, arginfo_pcntl_wtermsig)
	ZEND_FE(pcntl_wstopsig, arginfo_pcntl_wstopsig)
	ZEND_FE(pcntl_exec, arginfo_pcntl_exec)
	ZEND_FE(pcntl_alarm, arginfo_pcntl_alarm)
	ZEND_FE(pcntl_get_last_error, arginfo_pcntl_get_last_error)
	ZEND_FALIAS(pcntl_errno, pcntl_get_last_error, arginfo_pcntl_errno)
#if defined(HAVE_GETPRIORITY)
	ZEND_FE(pcntl_getpriority, arginfo_pcntl_getpriority)
#endif
#if defined(HAVE_SETPRIORITY)
	ZEND_FE(pcntl_setpriority, arginfo_pcntl_setpriority)
#endif
	ZEND_FE(pcntl_strerror, arginfo_pcntl_strerror)
	ZEND_FE(pcntl_async_signals, arginfo_pcntl_async_signals)
#if defined(HAVE_UNSHARE)
	ZEND_FE(pcntl_unshare, arginfo_pcntl_unshare)
#endif
	ZEND_FE_END
};
