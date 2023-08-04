/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
 */

#define PCNTL_DEBUG 0

#if PCNTL_DEBUG
#define DEBUG_OUT printf("DEBUG: ");printf
#define IF_DEBUG(z) z
#else
#define IF_DEBUG(z)
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pcntl.h"
#include "php_signal.h"
#include "php_ticks.h"
#include "zend_fibers.h"

#if defined(HAVE_GETPRIORITY) || defined(HAVE_SETPRIORITY) || defined(HAVE_WAIT3)
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <errno.h>
#ifdef HAVE_UNSHARE
#include <sched.h>
#endif

#ifdef HAVE_FORKX
#include <sys/fork.h>
#endif

#ifndef NSIG
# define NSIG 32
#endif

#define LONG_CONST(c) (zend_long) c

#include "pcntl_arginfo.h"

#include "Zend/zend_max_execution_timer.h"

ZEND_DECLARE_MODULE_GLOBALS(pcntl)
static PHP_GINIT_FUNCTION(pcntl);

zend_module_entry pcntl_module_entry = {
	STANDARD_MODULE_HEADER,
	"pcntl",
	ext_functions,
	PHP_MINIT(pcntl),
	PHP_MSHUTDOWN(pcntl),
	PHP_RINIT(pcntl),
	PHP_RSHUTDOWN(pcntl),
	PHP_MINFO(pcntl),
	PHP_PCNTL_VERSION,
	PHP_MODULE_GLOBALS(pcntl),
	PHP_GINIT(pcntl),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_PCNTL
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pcntl)
#endif

static void (*orig_interrupt_function)(zend_execute_data *execute_data);

#ifdef HAVE_STRUCT_SIGINFO_T
static void pcntl_signal_handler(int, siginfo_t*, void*);
static void pcntl_siginfo_to_zval(int, siginfo_t*, zval*);
#else
static void pcntl_signal_handler(int);
#endif
static void pcntl_signal_dispatch(void);
static void pcntl_signal_dispatch_tick_function(int dummy_int, void *dummy_pointer);
static void pcntl_interrupt_function(zend_execute_data *execute_data);

static PHP_GINIT_FUNCTION(pcntl)
{
#if defined(COMPILE_DL_PCNTL) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(pcntl_globals, 0, sizeof(*pcntl_globals));
}

PHP_RINIT_FUNCTION(pcntl)
{
	php_add_tick_function(pcntl_signal_dispatch_tick_function, NULL);
	zend_hash_init(&PCNTL_G(php_signal_table), 16, NULL, ZVAL_PTR_DTOR, 0);
	PCNTL_G(head) = PCNTL_G(tail) = PCNTL_G(spares) = NULL;
	PCNTL_G(async_signals) = 0;
	PCNTL_G(last_error) = 0;
	PCNTL_G(num_signals) = NSIG;
#ifdef SIGRTMAX
	/* At least FreeBSD reports an incorrecrt NSIG that does not include realtime signals.
	 * As SIGRTMAX may be a dynamic value, adjust the value in INIT. */
	if (NSIG < SIGRTMAX + 1) {
		PCNTL_G(num_signals) = SIGRTMAX + 1;
	}
#endif
	return SUCCESS;
}

PHP_MINIT_FUNCTION(pcntl)
{
	register_pcntl_symbols(module_number);
	orig_interrupt_function = zend_interrupt_function;
	zend_interrupt_function = pcntl_interrupt_function;

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pcntl)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(pcntl)
{
	struct php_pcntl_pending_signal *sig;
	zend_long signo;
	zval *handle;

	/* Reset all signals to their default disposition */
	ZEND_HASH_FOREACH_NUM_KEY_VAL(&PCNTL_G(php_signal_table), signo, handle) {
		if (Z_TYPE_P(handle) != IS_LONG || Z_LVAL_P(handle) != (zend_long)SIG_DFL) {
			php_signal(signo, (Sigfunc *)(zend_long)SIG_DFL, 0);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&PCNTL_G(php_signal_table));

	while (PCNTL_G(head)) {
		sig = PCNTL_G(head);
		PCNTL_G(head) = sig->next;
		efree(sig);
	}
	while (PCNTL_G(spares)) {
		sig = PCNTL_G(spares);
		PCNTL_G(spares) = sig->next;
		efree(sig);
	}

	return SUCCESS;
}

PHP_MINFO_FUNCTION(pcntl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "pcntl support", "enabled");
	php_info_print_table_end();
}

/* {{{ Forks the currently running process following the same behavior as the UNIX fork() system call*/
PHP_FUNCTION(pcntl_fork)
{
	pid_t id;

	ZEND_PARSE_PARAMETERS_NONE();

	id = fork();
	if (id == -1) {
		PCNTL_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Error %d", errno);
	} else if (id == 0) {
		zend_max_execution_timer_init();
	}

	RETURN_LONG((zend_long) id);
}
/* }}} */

/* {{{ Set an alarm clock for delivery of a signal*/
PHP_FUNCTION(pcntl_alarm)
{
	zend_long seconds;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(seconds);
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG((zend_long) alarm(seconds));
}
/* }}} */

#define PHP_RUSAGE_PARA(from, to, field) \
	add_assoc_long(to, #field, from.field)
#ifndef _OSD_POSIX
	#define PHP_RUSAGE_SPECIAL(from, to) \
		PHP_RUSAGE_PARA(from, to, ru_oublock); \
		PHP_RUSAGE_PARA(from, to, ru_inblock); \
		PHP_RUSAGE_PARA(from, to, ru_msgsnd); \
		PHP_RUSAGE_PARA(from, to, ru_msgrcv); \
		PHP_RUSAGE_PARA(from, to, ru_maxrss); \
		PHP_RUSAGE_PARA(from, to, ru_ixrss); \
		PHP_RUSAGE_PARA(from, to, ru_idrss); \
		PHP_RUSAGE_PARA(from, to, ru_minflt); \
		PHP_RUSAGE_PARA(from, to, ru_majflt); \
		PHP_RUSAGE_PARA(from, to, ru_nsignals); \
		PHP_RUSAGE_PARA(from, to, ru_nvcsw); \
		PHP_RUSAGE_PARA(from, to, ru_nivcsw); \
		PHP_RUSAGE_PARA(from, to, ru_nswap);
#else /*_OSD_POSIX*/
	#define PHP_RUSAGE_SPECIAL(from, to)
#endif

#define PHP_RUSAGE_COMMON(from ,to) \
	PHP_RUSAGE_PARA(from, to, ru_utime.tv_usec); \
	PHP_RUSAGE_PARA(from, to, ru_utime.tv_sec); \
	PHP_RUSAGE_PARA(from, to, ru_stime.tv_usec); \
	PHP_RUSAGE_PARA(from, to, ru_stime.tv_sec);

#define PHP_RUSAGE_TO_ARRAY(from, to) \
	if (to) { \
		PHP_RUSAGE_SPECIAL(from, to) \
		PHP_RUSAGE_COMMON(from, to); \
	}

/* {{{ Waits on or returns the status of a forked child as defined by the waitpid() system call */
PHP_FUNCTION(pcntl_waitpid)
{
	zend_long pid, options = 0;
	zval *z_status = NULL, *z_rusage = NULL;
	int status;
	pid_t child_id;
#ifdef HAVE_WAIT4
	struct rusage rusage;
#endif

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(pid)
		Z_PARAM_ZVAL(z_status)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(z_rusage)
	ZEND_PARSE_PARAMETERS_END();

	status = zval_get_long(z_status);

#ifdef HAVE_WAIT4
	if (z_rusage) {
		z_rusage = zend_try_array_init(z_rusage);
		if (!z_rusage) {
			RETURN_THROWS();
		}

		memset(&rusage, 0, sizeof(struct rusage));
		child_id = wait4((pid_t) pid, &status, options, &rusage);
	} else {
		child_id = waitpid((pid_t) pid, &status, options);
	}
#else
	child_id = waitpid((pid_t) pid, &status, options);
#endif

	if (child_id < 0) {
		PCNTL_G(last_error) = errno;
	}

#ifdef HAVE_WAIT4
	if (child_id > 0) {
		PHP_RUSAGE_TO_ARRAY(rusage, z_rusage);
	}
#endif

	ZEND_TRY_ASSIGN_REF_LONG(z_status, status);

	RETURN_LONG((zend_long) child_id);
}
/* }}} */

/* {{{ Waits on or returns the status of a forked child as defined by the waitpid() system call */
PHP_FUNCTION(pcntl_wait)
{
	zend_long options = 0;
	zval *z_status = NULL, *z_rusage = NULL;
	int status;
	pid_t child_id;
#ifdef HAVE_WAIT3
	struct rusage rusage;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ZVAL(z_status)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(z_rusage)
	ZEND_PARSE_PARAMETERS_END();

	status = zval_get_long(z_status);
#ifdef HAVE_WAIT3
	if (z_rusage) {
		z_rusage = zend_try_array_init(z_rusage);
		if (!z_rusage) {
			RETURN_THROWS();
		}

		memset(&rusage, 0, sizeof(struct rusage));
		child_id = wait3(&status, options, &rusage);
	} else if (options) {
		child_id = wait3(&status, options, NULL);
	} else {
		child_id = wait(&status);
	}
#else
	child_id = wait(&status);
#endif
	if (child_id < 0) {
		PCNTL_G(last_error) = errno;
	}

#ifdef HAVE_WAIT3
	if (child_id > 0) {
		PHP_RUSAGE_TO_ARRAY(rusage, z_rusage);
	}
#endif

	ZEND_TRY_ASSIGN_REF_LONG(z_status, status);

	RETURN_LONG((zend_long) child_id);
}
/* }}} */

#undef PHP_RUSAGE_PARA
#undef PHP_RUSAGE_SPECIAL
#undef PHP_RUSAGE_COMMON
#undef PHP_RUSAGE_TO_ARRAY

/* {{{ Returns true if the child status code represents a successful exit */
PHP_FUNCTION(pcntl_wifexited)
{
	zend_long status_word;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(status_word)
	ZEND_PARSE_PARAMETERS_END();

#ifdef WIFEXITED
	int int_status_word = (int) status_word;
	if (WIFEXITED(int_status_word)) {
		RETURN_TRUE;
	}
#endif

	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns true if the child status code represents a stopped process (WUNTRACED must have been used with waitpid) */
PHP_FUNCTION(pcntl_wifstopped)
{
	zend_long status_word;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(status_word)
	ZEND_PARSE_PARAMETERS_END();

#ifdef WIFSTOPPED
	int int_status_word = (int) status_word;
	if (WIFSTOPPED(int_status_word)) {
		RETURN_TRUE;
	}
#endif

	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns true if the child status code represents a process that was terminated due to a signal */
PHP_FUNCTION(pcntl_wifsignaled)
{
	zend_long status_word;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(status_word)
	ZEND_PARSE_PARAMETERS_END();

#ifdef WIFSIGNALED
	int int_status_word = (int) status_word;
	if (WIFSIGNALED(int_status_word)) {
		RETURN_TRUE;
	}
#endif

	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns true if the child status code represents a process that was resumed due to a SIGCONT signal */
PHP_FUNCTION(pcntl_wifcontinued)
{
	zend_long status_word;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(status_word)
	ZEND_PARSE_PARAMETERS_END();

#ifdef HAVE_WCONTINUED
	int int_status_word = (int) status_word;
	if (WIFCONTINUED(int_status_word)) {
		RETURN_TRUE;
	}
#endif
	RETURN_FALSE;
}
/* }}} */


/* {{{ Returns the status code of a child's exit */
PHP_FUNCTION(pcntl_wexitstatus)
{
	zend_long status_word;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(status_word)
	ZEND_PARSE_PARAMETERS_END();

#ifdef WEXITSTATUS
	int int_status_word = (int) status_word;
	RETURN_LONG(WEXITSTATUS(int_status_word));
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ Returns the number of the signal that terminated the process who's status code is passed  */
PHP_FUNCTION(pcntl_wtermsig)
{
	zend_long status_word;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(status_word)
	ZEND_PARSE_PARAMETERS_END();

#ifdef WTERMSIG
	int int_status_word = (int) status_word;
	RETURN_LONG(WTERMSIG(int_status_word));
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ Returns the number of the signal that caused the process to stop who's status code is passed */
PHP_FUNCTION(pcntl_wstopsig)
{
	zend_long status_word;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(status_word)
	ZEND_PARSE_PARAMETERS_END();

#ifdef WSTOPSIG
	int int_status_word = (int) status_word;
	RETURN_LONG(WSTOPSIG(int_status_word));
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ Executes specified program in current process space as defined by exec(2) */
PHP_FUNCTION(pcntl_exec)
{
	zval *args = NULL, *envs = NULL;
	zval *element;
	HashTable *args_hash, *envs_hash;
	int argc = 0, argi = 0;
	int envc = 0, envi = 0;
	char **argv = NULL, **envp = NULL;
	char **current_arg, **pair;
	size_t pair_length;
	zend_string *key;
	char *path;
	size_t path_len;
	zend_ulong key_num;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH(path, path_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(args)
		Z_PARAM_ARRAY(envs)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() > 1) {
		/* Build argument list */
		SEPARATE_ARRAY(args);
		args_hash = Z_ARRVAL_P(args);
		argc = zend_hash_num_elements(args_hash);

		argv = safe_emalloc((argc + 2), sizeof(char *), 0);
		*argv = path;
		current_arg = argv+1;
		ZEND_HASH_FOREACH_VAL(args_hash, element) {
			if (argi >= argc) break;
			if (!try_convert_to_string(element)) {
				efree(argv);
				RETURN_THROWS();
			}

			*current_arg = Z_STRVAL_P(element);
			argi++;
			current_arg++;
		} ZEND_HASH_FOREACH_END();
		*current_arg = NULL;
	} else {
		argv = emalloc(2 * sizeof(char *));
		argv[0] = path;
		argv[1] = NULL;
	}

	if ( ZEND_NUM_ARGS() == 3 ) {
		/* Build environment pair list */
		SEPARATE_ARRAY(envs);
		envs_hash = Z_ARRVAL_P(envs);
		envc = zend_hash_num_elements(envs_hash);

		pair = envp = safe_emalloc((envc + 1), sizeof(char *), 0);
		ZEND_HASH_FOREACH_KEY_VAL(envs_hash, key_num, key, element) {
			if (envi >= envc) break;
			if (!key) {
				key = zend_long_to_str(key_num);
			} else {
				zend_string_addref(key);
			}

			if (!try_convert_to_string(element)) {
				zend_string_release(key);
				efree(argv);
				efree(envp);
				RETURN_THROWS();
			}

			/* Length of element + equal sign + length of key + null */
			ZEND_ASSERT(Z_STRLEN_P(element) < SIZE_MAX && ZSTR_LEN(key) < SIZE_MAX);
			*pair = safe_emalloc(Z_STRLEN_P(element) + 1, sizeof(char), ZSTR_LEN(key) + 1);
			pair_length = Z_STRLEN_P(element) + ZSTR_LEN(key) + 2;
			strlcpy(*pair, ZSTR_VAL(key), ZSTR_LEN(key) + 1);
			strlcat(*pair, "=", pair_length);
			strlcat(*pair, Z_STRVAL_P(element), pair_length);

			/* Cleanup */
			zend_string_release_ex(key, 0);
			envi++;
			pair++;
		} ZEND_HASH_FOREACH_END();
		*(pair) = NULL;

		if (execve(path, argv, envp) == -1) {
			PCNTL_G(last_error) = errno;
			php_error_docref(NULL, E_WARNING, "Error has occurred: (errno %d) %s", errno, strerror(errno));
		}

		/* Cleanup */
		for (pair = envp; *pair != NULL; pair++) efree(*pair);
		efree(envp);
	} else {

		if (execv(path, argv) == -1) {
			PCNTL_G(last_error) = errno;
			php_error_docref(NULL, E_WARNING, "Error has occurred: (errno %d) %s", errno, strerror(errno));
		}
	}

	efree(argv);

	RETURN_FALSE;
}
/* }}} */

/* {{{ Assigns a system signal handler to a PHP function */
PHP_FUNCTION(pcntl_signal)
{
	zval *handle;
	zend_long signo;
	bool restart_syscalls = 1;
	bool restart_syscalls_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(signo)
		Z_PARAM_ZVAL(handle)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(restart_syscalls, restart_syscalls_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (signo < 1) {
		zend_argument_value_error(1, "must be greater than or equal to 1");
		RETURN_THROWS();
	}

	if (signo >= PCNTL_G(num_signals)) {
		zend_argument_value_error(1, "must be less than %d", PCNTL_G(num_signals));
		RETURN_THROWS();
	}

	if (!PCNTL_G(spares)) {
		/* since calling malloc() from within a signal handler is not portable,
		 * pre-allocate a few records for recording signals */
		int i;
		for (i = 0; i < PCNTL_G(num_signals); i++) {
			struct php_pcntl_pending_signal *psig;

			psig = emalloc(sizeof(*psig));
			psig->next = PCNTL_G(spares);
			PCNTL_G(spares) = psig;
		}
	}

	/* If restart_syscalls was not explicitly specified and the signal is SIGALRM, then default
	 * restart_syscalls to false. PHP used to enforce that restart_syscalls is false for SIGALRM,
	 * so we keep this differing default to reduce the degree of BC breakage. */
	if (restart_syscalls_is_null && signo == SIGALRM) {
		restart_syscalls = 0;
	}

	/* Special long value case for SIG_DFL and SIG_IGN */
	if (Z_TYPE_P(handle) == IS_LONG) {
		if (Z_LVAL_P(handle) != (zend_long) SIG_DFL && Z_LVAL_P(handle) != (zend_long) SIG_IGN) {
			zend_argument_value_error(2, "must be either SIG_DFL or SIG_IGN when an integer value is given");
			RETURN_THROWS();
		}
		if (php_signal(signo, (Sigfunc *) Z_LVAL_P(handle), (int) restart_syscalls) == (void *)SIG_ERR) {
			PCNTL_G(last_error) = errno;
			php_error_docref(NULL, E_WARNING, "Error assigning signal");
			RETURN_FALSE;
		}
		zend_hash_index_update(&PCNTL_G(php_signal_table), signo, handle);
		RETURN_TRUE;
	}

	if (!zend_is_callable_ex(handle, NULL, 0, NULL, NULL, NULL)) {
		PCNTL_G(last_error) = EINVAL;

		zend_argument_type_error(2, "must be of type callable|int, %s given", zend_zval_value_name(handle));
		RETURN_THROWS();
	}

	/* Add the function name to our signal table */
	handle = zend_hash_index_update(&PCNTL_G(php_signal_table), signo, handle);
	Z_TRY_ADDREF_P(handle);

	if (php_signal4(signo, pcntl_signal_handler, (int) restart_syscalls, 1) == (void *)SIG_ERR) {
		PCNTL_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Error assigning signal");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Gets signal handler */
PHP_FUNCTION(pcntl_signal_get_handler)
{
	zval *prev_handle;
	zend_long signo;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(signo)
	ZEND_PARSE_PARAMETERS_END();

	if (signo < 1 || signo > 32) {
		zend_argument_value_error(1, "must be between 1 and 32");
		RETURN_THROWS();
	}

	if ((prev_handle = zend_hash_index_find(&PCNTL_G(php_signal_table), signo)) != NULL) {
		RETURN_COPY(prev_handle);
	} else {
		RETURN_LONG((zend_long)SIG_DFL);
	}
}

/* {{{ Dispatch signals to signal handlers */
PHP_FUNCTION(pcntl_signal_dispatch)
{
	ZEND_PARSE_PARAMETERS_NONE();

	pcntl_signal_dispatch();
	RETURN_TRUE;
}
/* }}} */

/* Common helper function for these 3 wrapper functions */
#if defined(HAVE_SIGWAITINFO) || defined(HAVE_SIGTIMEDWAIT) || defined(HAVE_SIGPROCMASK)
static bool php_pcntl_set_user_signal_infos(
	/* const */ HashTable *const user_signals,
	sigset_t *const set,
	size_t arg_num
) {
	if (zend_hash_num_elements(user_signals) == 0) {
		zend_argument_value_error(arg_num, "cannot be empty");
		return false;
	}

	errno = 0;
	if (sigemptyset(set) != 0) {
		PCNTL_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		return false;
	}

	zval *user_signal_no;
	ZEND_HASH_FOREACH_VAL(user_signals, user_signal_no) {
		bool failed = true;
		zend_long tmp = zval_try_get_long(user_signal_no, &failed);

		if (failed) {
			zend_argument_type_error(arg_num, "signals must be of type int, %s given", zend_zval_value_name(user_signal_no));
			return false;
		}
		/* Signals are positive integers */
		if (tmp < 1 || tmp >= PCNTL_G(num_signals)) {
			/* PCNTL_G(num_signals) stores +1 from the last valid signal */
			zend_argument_value_error(arg_num, "signals must be between 1 and %d", PCNTL_G(num_signals)-1);
			return false;
		}

		int signal_no = (int) tmp;
		errno = 0;
		if (sigaddset(set, signal_no) != 0) {
			PCNTL_G(last_error) = errno;
			php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
			return false;
		}
	} ZEND_HASH_FOREACH_END();
	return true;
}
#endif

#ifdef HAVE_SIGPROCMASK
/* {{{ Examine and change blocked signals */
PHP_FUNCTION(pcntl_sigprocmask)
{
	zend_long how;
	HashTable *user_set;
	/* Optional by-ref out-param array of old signals */
	zval *user_old_set = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(how)
		Z_PARAM_ARRAY_HT(user_set)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(user_old_set)
	ZEND_PARSE_PARAMETERS_END();

	if (how != SIG_BLOCK && how != SIG_UNBLOCK && how != SIG_SETMASK) {
		zend_argument_value_error(1, "must be one of SIG_BLOCK, SIG_UNBLOCK, or SIG_SETMASK");
		RETURN_THROWS();
	}

	errno = 0;
	sigset_t old_set;
	if (sigemptyset(&old_set) != 0) {
		PCNTL_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	sigset_t set;
	/* Can set an empty array of signals for SIG_SETMASK */
	if (how == SIG_SETMASK && zend_hash_num_elements(user_set) == 0) {
		if (sigemptyset(&set) != 0) {
			PCNTL_G(last_error) = errno;
			php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
			RETURN_FALSE;
		}
		goto skip_setting_user_signals;
	}
	bool status = php_pcntl_set_user_signal_infos(user_set, &set, 2);
	/* Some error occurred */
	if (!status) {
		RETURN_FALSE;
	}
	skip_setting_user_signals:

	if (sigprocmask(how, &set, &old_set) != 0) {
		PCNTL_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	if (user_old_set != NULL) {
		user_old_set = zend_try_array_init(user_old_set);
		if (!user_old_set) {
			RETURN_THROWS();
		}

		for (int signal_no = 1; signal_no < PCNTL_G(num_signals); ++signal_no) {
			if (sigismember(&old_set, signal_no) != 1) {
				continue;
			}
			add_next_index_long(user_old_set, signal_no);
		}
	}

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_STRUCT_SIGINFO_T
# ifdef HAVE_SIGWAITINFO

/* {{{ Synchronously wait for queued signals */
PHP_FUNCTION(pcntl_sigwaitinfo)
{
	HashTable *user_set;
	/* Optional by-ref array of ints */
	zval *user_siginfo = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_HT(user_set)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(user_siginfo)
	ZEND_PARSE_PARAMETERS_END();

	sigset_t set;
	bool status = php_pcntl_set_user_signal_infos(user_set, &set, 1);
	/* Some error occurred */
	if (!status) {
		RETURN_FALSE;
	}

	errno = 0;
	siginfo_t siginfo;
	int signal_no = sigwaitinfo(&set, &siginfo);
	/* sigwaitinfo() never sets errno to EAGAIN according to POSIX */
	if (signal_no == -1) {
		PCNTL_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	/* sigwaitinfo can return 0 on success on some platforms, e.g. NetBSD */
	if (!signal_no && siginfo.si_signo) {
		signal_no = siginfo.si_signo;
	}

	pcntl_siginfo_to_zval(signal_no, &siginfo, user_siginfo);

	RETURN_LONG(signal_no);
}
/* }}} */
# endif
# ifdef HAVE_SIGTIMEDWAIT
/* {{{ Wait for queued signals */
PHP_FUNCTION(pcntl_sigtimedwait)
{
	HashTable *user_set;
	/* Optional by-ref array of ints */
	zval *user_siginfo = NULL;
	zend_long tv_sec = 0;
	zend_long tv_nsec = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_ARRAY_HT(user_set)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(user_siginfo)
		Z_PARAM_LONG(tv_sec)
		Z_PARAM_LONG(tv_nsec)
	ZEND_PARSE_PARAMETERS_END();

	sigset_t set;
	bool status = php_pcntl_set_user_signal_infos(user_set, &set, 1);
	/* Some error occurred */
	if (!status) {
		RETURN_FALSE;
	}
	if (tv_sec < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}
	/* Nanosecond between 0 and 1e9 */
	if (tv_nsec < 0 || tv_nsec > 1000000000) {
		zend_argument_value_error(4, "must be between 0 and 1e9");
		RETURN_THROWS();
	}
	if (UNEXPECTED(tv_sec == 0 && tv_nsec == 0)) {
		zend_value_error("pcntl_sigtimedwait(): At least one of argument #3 ($seconds) or argument #4 ($nanoseconds) must be greater than 0");
		RETURN_THROWS();
	}

	errno = 0;
	siginfo_t siginfo;
	struct timespec timeout;
	timeout.tv_sec  = (time_t) tv_sec;
	timeout.tv_nsec = tv_nsec;
	int signal_no = sigtimedwait(&set, &siginfo, &timeout);
	// TODO Drop check for EAGAIN as it will return -1?
	if (signal_no == -1 && errno != EAGAIN) {
		PCNTL_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	/* sigtimedwait can return 0 on success on some platforms, e.g. NetBSD */
	if (!signal_no && siginfo.si_signo) {
		signal_no = siginfo.si_signo;
	}

	pcntl_siginfo_to_zval(signal_no, &siginfo, user_siginfo);

	RETURN_LONG(signal_no);
}
/* }}} */
# endif

static void pcntl_siginfo_to_zval(int signo, siginfo_t *siginfo, zval *user_siginfo) /* {{{ */
{
	if (signo > 0 && user_siginfo) {
		user_siginfo = zend_try_array_init(user_siginfo);
		if (!user_siginfo) {
			return;
		}

		add_assoc_long_ex(user_siginfo, "signo", sizeof("signo")-1, siginfo->si_signo);
		add_assoc_long_ex(user_siginfo, "errno", sizeof("errno")-1, siginfo->si_errno);
		add_assoc_long_ex(user_siginfo, "code",  sizeof("code")-1,  siginfo->si_code);
		switch(signo) {
#ifdef SIGCHLD
			case SIGCHLD:
				add_assoc_long_ex(user_siginfo,   "status", sizeof("status")-1, siginfo->si_status);
# ifdef si_utime
				add_assoc_double_ex(user_siginfo, "utime",  sizeof("utime")-1,  siginfo->si_utime);
# endif
# ifdef si_stime
				add_assoc_double_ex(user_siginfo, "stime",  sizeof("stime")-1,  siginfo->si_stime);
# endif
				add_assoc_long_ex(user_siginfo,   "pid",    sizeof("pid")-1,    siginfo->si_pid);
				add_assoc_long_ex(user_siginfo,   "uid",    sizeof("uid")-1,    siginfo->si_uid);
				break;
			case SIGUSR1:
			case SIGUSR2:
				add_assoc_long_ex(user_siginfo,   "pid",    sizeof("pid")-1,    siginfo->si_pid);
				add_assoc_long_ex(user_siginfo,   "uid",    sizeof("uid")-1,    siginfo->si_uid);
				break;
#endif
			case SIGILL:
			case SIGFPE:
			case SIGSEGV:
			case SIGBUS:
				add_assoc_double_ex(user_siginfo, "addr", sizeof("addr")-1, (zend_long)siginfo->si_addr);
				break;
#if defined(SIGPOLL) && !defined(__CYGWIN__)
			case SIGPOLL:
				add_assoc_long_ex(user_siginfo, "band", sizeof("band")-1, siginfo->si_band);
# ifdef si_fd
				add_assoc_long_ex(user_siginfo, "fd",   sizeof("fd")-1,   siginfo->si_fd);
# endif
				break;
#endif
		}
#if defined(SIGRTMIN) && defined(SIGRTMAX)
		if (SIGRTMIN <= signo && signo <= SIGRTMAX) {
			add_assoc_long_ex(user_siginfo, "pid", sizeof("pid")-1, siginfo->si_pid);
			add_assoc_long_ex(user_siginfo, "uid", sizeof("uid")-1, siginfo->si_uid);
		}
#endif
	}
}
/* }}} */
#endif

#ifdef HAVE_GETPRIORITY
/* {{{ Get the priority of any process */
PHP_FUNCTION(pcntl_getpriority)
{
	zend_long who = PRIO_PROCESS;
	zend_long pid;
	bool pid_is_null = 1;
	int pri;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(pid, pid_is_null)
		Z_PARAM_LONG(who)
	ZEND_PARSE_PARAMETERS_END();

	/* needs to be cleared, since any returned value is valid */
	errno = 0;

	pid = pid_is_null ? getpid() : pid;
	pri = getpriority(who, pid);

	if (errno) {
		PCNTL_G(last_error) = errno;
		switch (errno) {
			case ESRCH:
				php_error_docref(NULL, E_WARNING, "Error %d: No process was located using the given parameters", errno);
				break;
			case EINVAL:
#ifdef PRIO_DARWIN_BG
				if (who != PRIO_PGRP && who != PRIO_USER && who != PRIO_PROCESS && who != PRIO_DARWIN_THREAD) {
					zend_argument_value_error(2, "must be one of PRIO_PGRP, PRIO_USER, PRIO_PROCESS or PRIO_DARWIN_THREAD");
					RETURN_THROWS();
				} else if (who == PRIO_DARWIN_THREAD && pid != 0) {
					zend_argument_value_error(1, "must be 0 (zero) if PRIO_DARWIN_THREAD is provided as second parameter");
					RETURN_THROWS();
				} else {
					zend_argument_value_error(1, "is not a valid process, process group, or user ID");
					RETURN_THROWS();
				}
#else
				zend_argument_value_error(2, "must be one of PRIO_PGRP, PRIO_USER, or PRIO_PROCESS");
				RETURN_THROWS();
#endif

			default:
				php_error_docref(NULL, E_WARNING, "Unknown error %d has occurred", errno);
				break;
		}
		RETURN_FALSE;
	}

	RETURN_LONG(pri);
}
/* }}} */
#endif

#ifdef HAVE_SETPRIORITY
/* {{{ Change the priority of any process */
PHP_FUNCTION(pcntl_setpriority)
{
	zend_long who = PRIO_PROCESS;
	zend_long pid;
	bool pid_is_null = 1;
	zend_long pri;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pri)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(pid, pid_is_null)
		Z_PARAM_LONG(who)
	ZEND_PARSE_PARAMETERS_END();

	pid = pid_is_null ? getpid() : pid;

	if (setpriority(who, pid, pri)) {
		PCNTL_G(last_error) = errno;
		switch (errno) {
			case ESRCH:
				php_error_docref(NULL, E_WARNING, "Error %d: No process was located using the given parameters", errno);
				break;
			case EINVAL:
#ifdef PRIO_DARWIN_BG
				if (who != PRIO_PGRP && who != PRIO_USER && who != PRIO_PROCESS && who != PRIO_DARWIN_THREAD) {
					zend_argument_value_error(3, "must be one of PRIO_PGRP, PRIO_USER, PRIO_PROCESS or PRIO_DARWIN_THREAD");
					RETURN_THROWS();
				} else if (who == PRIO_DARWIN_THREAD && pid != 0) {
					zend_argument_value_error(2, "must be 0 (zero) if PRIO_DARWIN_THREAD is provided as second parameter");
					RETURN_THROWS();
				} else if (who == PRIO_DARWIN_THREAD && pid == 0 && (pri != 0 && pri != PRIO_DARWIN_BG)) {
					zend_argument_value_error(1, "must be either 0 (zero) or PRIO_DARWIN_BG, for mode PRIO_DARWIN_THREAD");
					RETURN_THROWS();
				} else {
					zend_argument_value_error(2, "is not a valid process, process group, or user ID");
					RETURN_THROWS();
				}
#else
				zend_argument_value_error(3, "must be one of PRIO_PGRP, PRIO_USER, or PRIO_PROCESS");
				RETURN_THROWS();
#endif
			case EPERM:
				php_error_docref(NULL, E_WARNING, "Error %d: A process was located, but neither its effective nor real user ID matched the effective user ID of the caller", errno);
				break;
			case EACCES:
				php_error_docref(NULL, E_WARNING, "Error %d: Only a super user may attempt to increase the process priority", errno);
				break;
			default:
				php_error_docref(NULL, E_WARNING, "Unknown error %d has occurred", errno);
				break;
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Retrieve the error number set by the last pcntl function which failed. */
PHP_FUNCTION(pcntl_get_last_error)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(PCNTL_G(last_error));
}
/* }}} */

/* {{{ Retrieve the system error message associated with the given errno. */
PHP_FUNCTION(pcntl_strerror)
{
	zend_long error;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(error)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STRING(strerror(error));
}
/* }}} */

/* Our custom signal handler that calls the appropriate php_function */
#ifdef HAVE_STRUCT_SIGINFO_T
static void pcntl_signal_handler(int signo, siginfo_t *siginfo, void *context)
#else
static void pcntl_signal_handler(int signo)
#endif
{
	struct php_pcntl_pending_signal *psig = PCNTL_G(spares);
	if (!psig) {
		/* oops, too many signals for us to track, so we'll forget about this one */
		return;
	}
	PCNTL_G(spares) = psig->next;

	psig->signo = signo;
	psig->next = NULL;

#ifdef HAVE_STRUCT_SIGINFO_T
	psig->siginfo = *siginfo;
#endif

	/* the head check is important, as the tick handler cannot atomically clear both
	 * the head and tail */
	if (PCNTL_G(head) && PCNTL_G(tail)) {
		PCNTL_G(tail)->next = psig;
	} else {
		PCNTL_G(head) = psig;
	}
	PCNTL_G(tail) = psig;
	PCNTL_G(pending_signals) = 1;
	if (PCNTL_G(async_signals)) {
		zend_atomic_bool_store_ex(&EG(vm_interrupt), true);
	}
}

void pcntl_signal_dispatch(void)
{
	zval params[2], *handle, retval;
	struct php_pcntl_pending_signal *queue, *next;
	sigset_t mask;
	sigset_t old_mask;

	if(!PCNTL_G(pending_signals)) {
		return;
	}

	/* Mask all signals */
	sigfillset(&mask);
	sigprocmask(SIG_BLOCK, &mask, &old_mask);

	/* Bail if the queue is empty or if we are already playing the queue */
	if (!PCNTL_G(head) || PCNTL_G(processing_signal_queue)) {
		sigprocmask(SIG_SETMASK, &old_mask, NULL);
		return;
	}

	/* Prevent switching fibers when handling signals */
	zend_fiber_switch_block();

	/* Prevent reentrant handler calls */
	PCNTL_G(processing_signal_queue) = 1;

	queue = PCNTL_G(head);
	PCNTL_G(head) = NULL; /* simple stores are atomic */

	/* Allocate */
	while (queue) {
		if ((handle = zend_hash_index_find(&PCNTL_G(php_signal_table), queue->signo)) != NULL) {
			if (Z_TYPE_P(handle) != IS_LONG) {
				ZVAL_NULL(&retval);
				ZVAL_LONG(&params[0], queue->signo);
#ifdef HAVE_STRUCT_SIGINFO_T
				array_init(&params[1]);
				pcntl_siginfo_to_zval(queue->signo, &queue->siginfo, &params[1]);
#else
				ZVAL_NULL(&params[1]);
#endif

				/* Call php signal handler - Note that we do not report errors, and we ignore the return value */
				/* FIXME: this is probably broken when multiple signals are handled in this while loop (retval) */
				call_user_function(NULL, NULL, handle, &retval, 2, params);
				zval_ptr_dtor(&retval);
#ifdef HAVE_STRUCT_SIGINFO_T
				zval_ptr_dtor(&params[1]);
#endif
			}
		}

		next = queue->next;
		queue->next = PCNTL_G(spares);
		PCNTL_G(spares) = queue;
		queue = next;
	}

	PCNTL_G(pending_signals) = 0;

	/* Re-enable queue */
	PCNTL_G(processing_signal_queue) = 0;

	/* Re-enable fiber switching */
	zend_fiber_switch_unblock();

	/* return signal mask to previous state */
	sigprocmask(SIG_SETMASK, &old_mask, NULL);
}

static void pcntl_signal_dispatch_tick_function(int dummy_int, void *dummy_pointer)
{
	return pcntl_signal_dispatch();
}

/* {{{ Enable/disable asynchronous signal handling and return the old setting. */
PHP_FUNCTION(pcntl_async_signals)
{
	bool on, on_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(on, on_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (on_is_null) {
		RETURN_BOOL(PCNTL_G(async_signals));
	}

	RETVAL_BOOL(PCNTL_G(async_signals));
	PCNTL_G(async_signals) = on;
}
/* }}} */

#ifdef HAVE_UNSHARE
/* {{{ disassociate parts of the process execution context */
PHP_FUNCTION(pcntl_unshare)
{
	zend_long flags;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	if (unshare(flags) == -1) {
		PCNTL_G(last_error) = errno;
		switch (errno) {
#ifdef EINVAL
			case EINVAL:
				zend_argument_value_error(1, "must be a combination of CLONE_* flags");
				RETURN_THROWS();
				break;
#endif
#ifdef ENOMEM
			case ENOMEM:
				php_error_docref(NULL, E_WARNING, "Error %d: Insufficient memory for unshare", errno);
				break;
#endif
#ifdef EPERM
			case EPERM:
				php_error_docref(NULL, E_WARNING, "Error %d: No privilege to use these flags", errno);
				break;
#endif
#ifdef ENOSPC
			case ENOSPC:
				php_error_docref(NULL, E_WARNING, "Error %d: Reached the maximum nesting limit for one of the specified namespaces", errno);
				break;
#endif
#ifdef EUSERS
			case EUSERS:
				php_error_docref(NULL, E_WARNING, "Error %d: Reached the maximum nesting limit for the user namespace", errno);
				break;
#endif
			default:
				php_error_docref(NULL, E_WARNING, "Unknown error %d has occurred", errno);
				break;
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_RFORK
/* {{{ proto bool pcntl_rfork(int flags [, int signal])
   More control over the process creation is given over fork/vfork. */
PHP_FUNCTION(pcntl_rfork)
{
	zend_long flags;
	zend_long csignal = 0;
	pid_t pid;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(flags)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(csignal)
	ZEND_PARSE_PARAMETERS_END();

	/* This is a flag to use with great caution in general, preferably not within PHP */
	if ((flags & RFMEM) != 0) {
		zend_argument_value_error(1, "must not include RFMEM value, not allowed within this context");
		RETURN_THROWS();
	}

	if ((flags & RFSIGSHARE) != 0) {
		zend_argument_value_error(1, "must not include RFSIGSHARE value, not allowed within this context");
		RETURN_THROWS();
	}

	if ((flags & (RFFDG | RFCFDG)) == (RFFDG | RFCFDG)) {
		zend_argument_value_error(1, "must not include both RFFDG and RFCFDG, because these flags are mutually exclusive");
		RETURN_THROWS();
	}

	/* A new pid is required */
	if (!(flags & (RFPROC))) {
		flags |= RFPROC;
	}

#ifdef RFTSIGZMB
	if ((flags & RFTSIGZMB) != 0) {
		flags |= RFTSIGFLAGS(csignal);
	}
#endif

	pid = rfork(flags);

	if (pid == -1) {
		PCNTL_G(last_error) = errno;
		switch (errno) {
			case EAGAIN:
			php_error_docref(NULL, E_WARNING, "Maximum process creations limit reached\n");
		break;

		default:
			php_error_docref(NULL, E_WARNING, "Error %d", errno);
		}
	}

	RETURN_LONG((zend_long) pid);
}
#endif
/* }}} */

#ifdef HAVE_FORKX
/* {{{ proto bool pcntl_forkx(int flags)
   More elaborated version of fork with the following settings.
   FORK_WAITPID: forbid the parent process to wait for multiple pid but one only
   FORK_NOSIGCHLD: SIGCHLD signal ignored when the child terminates */
PHP_FUNCTION(pcntl_forkx)
{
	zend_long flags;
	pid_t pid;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	if (flags < FORK_NOSIGCHLD || flags > FORK_WAITPID) {
		zend_argument_value_error(1, "must be FORK_NOSIGCHLD or FORK_WAITPID");
		RETURN_THROWS();
	}

	pid = forkx(flags);

	if (pid == -1) {
		PCNTL_G(last_error) = errno;
		switch (errno) {
			case EAGAIN:
			php_error_docref(NULL, E_WARNING, "Maximum process creations limit reached\n");
		break;
			case EPERM:
			php_error_docref(NULL, E_WARNING, "Calling process not having the proper privileges\n");
		break;
			case ENOMEM:
			php_error_docref(NULL, E_WARNING, "No swap space left\n");
		break;
		default:
			php_error_docref(NULL, E_WARNING, "Error %d", errno);
		}
	}

	RETURN_LONG((zend_long) pid);
}
#endif
/* }}} */

static void pcntl_interrupt_function(zend_execute_data *execute_data)
{
	pcntl_signal_dispatch();
	if (orig_interrupt_function) {
		orig_interrupt_function(execute_data);
	}
}
