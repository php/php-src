/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

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

#if HAVE_GETPRIORITY || HAVE_SETPRIORITY
#include <sys/time.h>
#include <sys/resource.h>
#endif

ZEND_DECLARE_MODULE_GLOBALS(pcntl)

function_entry pcntl_functions[] = {
	PHP_FE(pcntl_fork,			NULL)
	PHP_FE(pcntl_waitpid,		second_arg_force_ref)
	PHP_FE(pcntl_signal,		NULL)
	PHP_FE(pcntl_wifexited,		NULL)
	PHP_FE(pcntl_wifstopped,	NULL)
	PHP_FE(pcntl_wifsignaled,	NULL)
	PHP_FE(pcntl_wexitstatus,	NULL)
	PHP_FE(pcntl_wtermsig,		NULL)
	PHP_FE(pcntl_wstopsig,		NULL)
	PHP_FE(pcntl_exec,			NULL)
	PHP_FE(pcntl_alarm,			NULL)
	PHP_FE(pcntl_getpriority,	NULL)
	PHP_FE(pcntl_setpriority,	NULL)
	{NULL, NULL, NULL}	
};

zend_module_entry pcntl_module_entry = {
	STANDARD_MODULE_HEADER,
	"pcntl",
	pcntl_functions,
	PHP_MINIT(pcntl),
	PHP_MSHUTDOWN(pcntl),
	PHP_RINIT(pcntl),
	PHP_RSHUTDOWN(pcntl),
	PHP_MINFO(pcntl),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PCNTL
ZEND_GET_MODULE(pcntl)
#endif
  
void php_register_signal_constants(INIT_FUNC_ARGS)
{

	/* Wait Constants */
#ifdef WNOHANG
	REGISTER_LONG_CONSTANT("WNOHANG",  (long) WNOHANG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef WUNTRACED
	REGISTER_LONG_CONSTANT("WUNTRACED",  (long) WUNTRACED, CONST_CS | CONST_PERSISTENT);
#endif

	/* Signal Constants */
	REGISTER_LONG_CONSTANT("SIG_IGN",  (long) SIG_IGN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIG_DFL",  (long) SIG_DFL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIG_ERR",  (long) SIG_ERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGHUP",   (long) SIGHUP,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGINT",   (long) SIGINT,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGQUIT",  (long) SIGQUIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGILL",   (long) SIGILL,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGTRAP",  (long) SIGTRAP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGABRT",  (long) SIGABRT, CONST_CS | CONST_PERSISTENT);
#ifdef SIGIOT
	REGISTER_LONG_CONSTANT("SIGIOT",   (long) SIGIOT,  CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SIGBUS",   (long) SIGBUS,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGFPE",   (long) SIGFPE,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGKILL",  (long) SIGKILL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGUSR1",  (long) SIGUSR1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGSEGV",  (long) SIGSEGV, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGUSR2",  (long) SIGUSR2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGPIPE",  (long) SIGPIPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGALRM",  (long) SIGALRM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGTERM",  (long) SIGTERM, CONST_CS | CONST_PERSISTENT);
#ifdef SIGSTKFLT
	REGISTER_LONG_CONSTANT("SIGSTKFLT",(long) SIGSTKFLT, CONST_CS | CONST_PERSISTENT);
#endif 
#ifdef SIGCLD
	REGISTER_LONG_CONSTANT("SIGCLD",   (long) SIGCLD, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef SIGCHLD
	REGISTER_LONG_CONSTANT("SIGCHLD",  (long) SIGCHLD, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SIGCONT",  (long) SIGCONT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGSTOP",  (long) SIGSTOP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGTSTP",  (long) SIGTSTP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGTTIN",  (long) SIGTTIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGTTOU",  (long) SIGTTOU, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGURG",   (long) SIGURG , CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGXCPU",  (long) SIGXCPU, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGXFSZ",  (long) SIGXFSZ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGVTALRM",(long) SIGVTALRM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGPROF",  (long) SIGPROF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGWINCH", (long) SIGWINCH, CONST_CS | CONST_PERSISTENT);
#ifdef SIGPOLL
	REGISTER_LONG_CONSTANT("SIGPOLL",  (long) SIGPOLL, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SIGIO",    (long) SIGIO, CONST_CS | CONST_PERSISTENT);
#ifdef SIGPWR
	REGISTER_LONG_CONSTANT("SIGPWR",   (long) SIGPWR, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SIGSYS",   (long) SIGSYS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGBABY",  (long) SIGSYS, CONST_CS | CONST_PERSISTENT);

#if HAVE_GETPRIORITY || HAVE_SETPRIORITY
	REGISTER_LONG_CONSTANT("PRIO_PGRP", PRIO_PGRP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PRIO_USER", PRIO_USER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PRIO_PROCESS", PRIO_PROCESS, CONST_CS | CONST_PERSISTENT);
#endif
}

static void php_pcntl_init_globals(zend_pcntl_globals *pcntl_globals)
{ 
	/* Just in case ... */
	memset(&pcntl_globals->php_signal_queue,0,sizeof(pcntl_globals->php_signal_queue));
   
	zend_llist_init(&pcntl_globals->php_signal_queue, sizeof (long),  NULL, 1);
	pcntl_globals->signal_queue_ready = 0;
	pcntl_globals->processing_signal_queue = 0;
}

PHP_RINIT_FUNCTION(pcntl)
{
	zend_hash_init(&PCNTL_G(php_signal_table), 16, NULL, ZVAL_PTR_DTOR, 1);
	return SUCCESS;
}

PHP_MINIT_FUNCTION(pcntl)
{
	php_register_signal_constants(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_INIT_MODULE_GLOBALS(pcntl, php_pcntl_init_globals, NULL);
	php_add_tick_function(pcntl_tick_handler);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pcntl)
{
	zend_llist_destroy(&PCNTL_G(php_signal_queue));
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(pcntl)
{
	zend_hash_destroy(&PCNTL_G(php_signal_table));
	return SUCCESS;
}

PHP_MINFO_FUNCTION(pcntl)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pcntl support", "enabled");
	php_info_print_table_end();
}

/* {{{ proto int pcntl_fork(void)
   Forks the currently running process following the same behavior as the UNIX fork() system call*/
PHP_FUNCTION(pcntl_fork)
{
	pid_t id;

	id = fork();
	if (id == -1) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error %d", errno);
	}
	
	RETURN_LONG((long) id);
}
/* }}} */

/* {{{ proto int pcntl_alarm(int seconds)
   Set an alarm clock for delivery of a signal*/
PHP_FUNCTION(pcntl_alarm)
{
	long seconds;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &seconds) == FAILURE)
		return;
	
	RETURN_LONG ((long) alarm(seconds));
}
/* }}} */

/* {{{ proto int pcntl_waitpid(long pid, long status, long options)
   Waits on or returns the status of a forked child as defined by the waitpid() system call */
PHP_FUNCTION(pcntl_waitpid)
{
	long pid, options = 0;
	zval *z_status = NULL;
	int status;
	pid_t child_id;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|l", &pid, &z_status, &options) == FAILURE)
		return;
	
	convert_to_long_ex(&z_status);

	status = Z_LVAL_P(z_status);

	child_id = waitpid((pid_t) pid, &status, options);

	Z_LVAL_P(z_status) = status;

	RETURN_LONG((long) child_id);
}
/* }}} */

/* {{{ proto bool pcntl_wifexited(long status) 
   Returns true if the child status code represents a successful exit */
PHP_FUNCTION(pcntl_wifexited)
{
#ifdef WIFEXITED
	zval **status;
	int status_word;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	status_word = (int) Z_LVAL_PP(status);
	
	if (WIFEXITED(status_word)) RETURN_TRUE;
#endif
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool pcntl_wifstopped(long status) 
   Returns true if the child status code represents a stopped process (WUNTRACED must have been used with waitpid) */
PHP_FUNCTION(pcntl_wifstopped)
{
#ifdef WIFSTOPPED
	zval **status;
	int status_word;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	status_word = (int) Z_LVAL_PP(status);
	
	if (WIFSTOPPED(status_word)) RETURN_TRUE;
#endif
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool pcntl_wifsignaled(long status) 
   Returns true if the child status code represents a process that was terminated due to a signal */
PHP_FUNCTION(pcntl_wifsignaled)
{
#ifdef WIFSIGNALED
	zval **status;
	int status_word;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	status_word = (int) Z_LVAL_PP(status);
	
	if (WIFSIGNALED(status_word)) RETURN_TRUE;
#endif
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int pcntl_wexitstatus(long status) 
   Returns the status code of a child's exit */
PHP_FUNCTION(pcntl_wexitstatus)
{
#ifdef WEXITSTATUS
	zval **status;
	int status_word;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	status_word = (int) Z_LVAL_PP(status);

	/* WEXITSTATUS only returns 8 bits so we *MUST* cast this to signed char
	   if you want to have valid negative exit codes */
	RETURN_LONG((signed char) WEXITSTATUS(status_word));
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto int pcntl_wtermsig(long status) 
   Returns the number of the signal that terminated the process who's status code is passed  */
PHP_FUNCTION(pcntl_wtermsig)
{
#ifdef WTERMSIG
	zval **status;
	int status_word;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	status_word = (int) Z_LVAL_PP(status);
	
	RETURN_LONG(WTERMSIG(status_word));
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto int pcntl_wstopsig(long status) 
   Returns the number of the signal that caused the process to stop who's status code is passed */
PHP_FUNCTION(pcntl_wstopsig)
{
#ifdef WSTOPSIG
	zval **status;
	int status_word;
   
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
   
	status_word = (int) Z_LVAL_PP(status);

 	RETURN_LONG(WSTOPSIG(status_word));
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool pcntl_exec(string path [, array args [, array envs]])
   Executes specified program in current process space as defined by exec(2) */
PHP_FUNCTION(pcntl_exec)
{
	zval *args, *envs;
	zval **element;
	HashTable *args_hash, *envs_hash;
	int argc = 0, argi = 0;
	int envc = 0, envi = 0;
	int return_val = 0;
	char **argv = NULL, **envp = NULL;
	char **current_arg, **pair;
	int pair_length;
	char *key;
	int key_length;
	char *path;
	int path_len;
	long key_num;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|aa", &path, &path_len, &args, &envs) == FAILURE) {
		return;
	}
	
	if (ZEND_NUM_ARGS() > 1) {
		/* Build argumnent list */
		args_hash = HASH_OF(args);
		argc = zend_hash_num_elements(args_hash);
		
		argv = alloca((argc+2) * sizeof(char *));
		*argv = path;
		for ( zend_hash_internal_pointer_reset(args_hash), current_arg = argv+1; 
			(argi < argc && (zend_hash_get_current_data(args_hash, (void **) &element) == SUCCESS));
			(argi++, current_arg++, zend_hash_move_forward(args_hash)) ) {

			convert_to_string_ex(element);
			*current_arg = Z_STRVAL_PP(element);
		}
		*(current_arg) = NULL;
	} else {
		argv = alloca(2 * sizeof(char *));
		*argv = path;
		*(argv+1) = NULL;
	}

	if ( ZEND_NUM_ARGS() == 3 ) {
		/* Build environment pair list */
		envs_hash = HASH_OF(envs);
		envc = zend_hash_num_elements(envs_hash);
		
		envp = alloca((envc+1) * sizeof(char *));
		for ( zend_hash_internal_pointer_reset(envs_hash), pair = envp; 
			(envi < envc && (zend_hash_get_current_data(envs_hash, (void **) &element) == SUCCESS));
			(envi++, pair++, zend_hash_move_forward(envs_hash)) ) {
			switch (return_val = zend_hash_get_current_key_ex(envs_hash, &key, &key_length, &key_num, 0, NULL)) {
				case HASH_KEY_IS_LONG:
					key = alloca(101);
					snprintf(key, 100, "%ld", key_num);
					key_length = strlen(key);
					break;
				case HASH_KEY_NON_EXISTANT:
					pair--;
					continue;
			}

			convert_to_string_ex(element);

			/* Length of element + equal sign + length of key + null */ 
			pair_length = Z_STRLEN_PP(element) + key_length + 2;
			*pair = emalloc(pair_length);
			strlcpy(*pair, key, key_length); 
			strlcat(*pair, "=", pair_length);
			strlcat(*pair, Z_STRVAL_PP(element), pair_length);
			
			/* Cleanup */
			if (return_val == HASH_KEY_IS_LONG) free_alloca(key);
		}
		*(pair) = NULL;
	}
	
	if (execve(path, argv, envp) == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error has occured: (errno %d) %s", errno, strerror(errno));
	}
	
	/* Cleanup */
	if (envp != NULL) {
		for (pair = envp; *pair != NULL; pair++) efree(*pair);
		free_alloca(envp);
	}

	free_alloca(argv);
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool pcntl_signal(long signo, mixed handle, [bool restart_syscalls])
   Assigns a system signal handler to a PHP function */
PHP_FUNCTION(pcntl_signal)
{
	zval *handle, **dest_handle = NULL;
	char *func_name;
	long signo;
	zend_bool restart_syscalls = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|b", &signo, &handle, &restart_syscalls) == FAILURE) {
		return;
	}

	/* Special long value case for SIG_DFL and SIG_IGN */
	if (Z_TYPE_P(handle)==IS_LONG) {
		if (Z_LVAL_P(handle)!= (long) SIG_DFL && Z_LVAL_P(handle) != (long) SIG_IGN) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid value for handle argument specifEied");
		}
		if (php_signal(signo, (Sigfunc *) Z_LVAL_P(handle), (int) restart_syscalls) == SIG_ERR) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error assigning singal");
			RETURN_FALSE;
		}
		RETURN_TRUE;
	}
	
	if (!zend_is_callable(handle, 0, &func_name)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not a callable function name error", func_name);
		efree(func_name);
		RETURN_FALSE;
	}
	efree(func_name);
	
	/* Add the function name to our signal table */
	zend_hash_index_update(&PCNTL_G(php_signal_table), signo, (void **) &handle, sizeof(zval *), (void **) &dest_handle);
	if (dest_handle) zval_add_ref(dest_handle);
	
	if (php_signal(signo, pcntl_signal_handler, (int) restart_syscalls) == SIG_ERR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error assigning singal");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_GETPRIORITY
/* {{{ proto int pcntl_getpriority(int pid, [int process_identifier]])
   Get the priority of any process */
PHP_FUNCTION(pcntl_getpriority)
{
	long who = PRIO_PROCESS;
	long pid = getpid();
	int pri;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &pid, &who) == FAILURE) {
		RETURN_FALSE;
	}

	/* needs to be cleared, since any returned value is valid */ 
	errno = 0;

	pri = getpriority(who, pid);

	if (errno) {
		switch (errno) {
			case ESRCH:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%l: No process was located using the given parameters.", errno);
				break;
			case EINVAL:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%l: Invalid identifier flag.", errno);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error %l has occured.", errno);
				break;
		}
		RETURN_FALSE;
	}

	RETURN_LONG(pri);
}
/* }}} */
#endif

#ifdef HAVE_SETPRIORITY
/* {{{ proto bool pcntl_setpriority(int priority, [int pid, [int process_identifier]])
   Change the priority of any process */
PHP_FUNCTION(pcntl_setpriority)
{
	long who = PRIO_PROCESS;
	long pid = getpid();
	long pri;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &pri, &pid, &who) == FAILURE) {
		RETURN_FALSE;
	}

	if (setpriority(who, pid, pri)) {
		switch (errno) {
			case ESRCH:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%l: No process was located using the given parameters.", errno);
				break;
			case EINVAL:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%l: Invalid identifier flag.", errno);
				break;
			case EPERM:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%l: A process was located, but neither its effective nor real user ID matched the effective user ID of the caller.", errno);
				break;
			case EACCES:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%l: Only a super user may attempt to increase the process priority.", errno);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error %l has occured.", errno);
				break;
		}
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */
#endif

/* Our custom signal handler that calls the appropriate php_function */
static void pcntl_signal_handler(int signo)
{
	long signal_num = signo;
	TSRMLS_FETCH();
	
	IF_DEBUG(DEBUG_OUT("Caught signo %d\n", signo));
	if (! PCNTL_G(processing_signal_queue)) {
		zend_llist_add_element(&PCNTL_G(php_signal_queue), &signal_num);
		PCNTL_G(signal_queue_ready) = 1;
		IF_DEBUG(DEBUG_OUT("Added queue entry\n"));
	}
	return;
}

void pcntl_tick_handler()
{
	zend_llist_element *element;
	zval *param, **handle, *retval;
	TSRMLS_FETCH();

	/* Bail if the queue is empty or if we are already playing the queue*/
	if (! PCNTL_G(signal_queue_ready) || PCNTL_G(processing_signal_queue))
		return;

	/* Mark our queue empty */
	PCNTL_G(signal_queue_ready) = 0;
	
	/* If for some reason our signal queue is empty then return */
	if (zend_llist_count(&PCNTL_G(php_signal_queue)) <= 0) {
		return;
	}
	
	/* Prevent reentrant handler calls */
	PCNTL_G(processing_signal_queue) = 1;

	/* Allocate */
	MAKE_STD_ZVAL(param);
	MAKE_STD_ZVAL(retval);

	/* Traverse through our signal queue and call the appropriate php functions */
	for (element = (&PCNTL_G(php_signal_queue))->head; element; element = element->next) {
		long *signal_num = (long *)&element->data;
		if (zend_hash_index_find(&PCNTL_G(php_signal_table), *signal_num, (void **) &handle)==FAILURE) {
			continue;
		}

		ZVAL_LONG(param, *signal_num);
		
		/* Call php singal handler - Note that we do not report errors, and we ignore the eturn value */
		call_user_function(EG(function_table), NULL, *handle, retval, 1, &param TSRMLS_CC);
	}
	/* Clear */
	zend_llist_clean(&PCNTL_G(php_signal_queue));

	/* Re-enable queue */
	PCNTL_G(processing_signal_queue) = 0;

	/* Clean up */
	efree(param);
	efree(retval);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
