/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jason Greene <jason@inetgurus.net>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_pcntl.h"

ZEND_DECLARE_MODULE_GLOBALS(pcntl)

static int le_pcntl;

function_entry pcntl_functions[] = {
	PHP_FE(pcntl_fork,	NULL)	
	PHP_FE(pcntl_waitpid,	NULL)	
       	PHP_FE(pcntl_signal,	NULL)	
	{NULL, NULL, NULL}	
};

zend_module_entry pcntl_module_entry = {
	"pcntl",	pcntl_functions,
	PHP_MINIT(pcntl),
	PHP_MSHUTDOWN(pcntl),
	NULL,
	NULL,
	PHP_MINFO(pcntl),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PCNTL
ZEND_GET_MODULE(pcntl)
#endif

void php_register_signal_constants(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("SIG_IGN",  (long) SIG_IGN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIG_DFL",  (long) SIG_DFL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIG_ERR",  (long) SIG_ERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGHUP",   (long) SIGHUP,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGINT",   (long) SIGINT,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGQUIT",  (long) SIGQUIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGILL",   (long) SIGILL,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGTRAP",  (long) SIGTRAP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGABRT",  (long) SIGABRT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGIOT",   (long) SIGIOT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGBUS",   (long) SIGBUS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGFPE",   (long) SIGFPE, CONST_CS | CONST_PERSISTENT);
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
	REGISTER_LONG_CONSTANT("SIGPOLL",  (long) SIGPOLL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGIO",    (long) SIGIO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGPWR",   (long) SIGPWR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIGSYS",   (long) SIGSYS, CONST_CS | CONST_PERSISTENT);
}
   
  
PHP_MINIT_FUNCTION(pcntl)
{
 	PCNTL_LS_FETCH();

   	php_register_signal_constants(INIT_FUNC_ARGS_PASSTHRU);
	zend_hash_init(&PCNTL_G(php_signal_table), 16, NULL, NULL, 1);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pcntl)
{
	PCNTL_LS_FETCH();
   
	zend_hash_destroy(&PCNTL_G(php_signal_table));
	return SUCCESS;
}

PHP_MINFO_FUNCTION(pcntl)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pcntl support", "enabled");
	php_info_print_table_end();
}
/* {{{ proto long pcntl_fork()
   Forks the currently running process following the same behavior as the UNIX fork() system call*/
PHP_FUNCTION(pcntl_fork)
{
	pid_t id;

	id=fork();
	if (id == -1) {
		php_error(E_ERROR, "Error %d in %s", errno, get_active_function_name());
	}
	
	RETURN_LONG((long) id);
}
/* }}} */

/* {{{ proto long pcntl_waitpid(long pid, long status, long options)
      Waits on or returns the status of a forked child as defined by the waitpid() system call */
PHP_FUNCTION(pcntl_waitpid)
{
	zval **pid, **status, **options;
	int temp_options, temp_status=0;
	pid_t temp_id;

	if (ZEND_NUM_ARGS() > 3 || ZEND_NUM_ARGS() < 2 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &pid, &status, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(pid);
	convert_to_long_ex(options);
   
   	SEPARATE_ZVAL(status);
	convert_to_long_ex(status);

        if (ZEND_NUM_ARGS()==2) temp_options=0;
		else temp_options=Z_LVAL_PP(options);
		
	temp_id = waitpid((pid_t) Z_LVAL_PP(pid), &temp_status, temp_options);
	Z_LVAL_PP(status)=temp_status;
	RETURN_LONG((long) temp_id);
}
/* }}} */

/* {{{ proto bool pcntl_signal(long signo, mixed handle)
      Assigns a system signal handler to a php function  */
PHP_FUNCTION(pcntl_signal)
{
	zval **signo, **handle;
   	char *func_name;
	PCNTL_LS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &signo, &handle) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(signo);

	/* Special long value case for SIG_DFL and SIG_IGN */
	if (Z_TYPE_PP(handle)==IS_LONG) {
 		if (Z_LVAL_PP(handle)!= (long) SIG_DFL && Z_LVAL_PP(handle) != (long) SIG_IGN) {
 			php_error(E_WARNING, "Invalid value for handle argument specifEied in %s", get_active_function_name());
		}
		if (php_signal(Z_LVAL_PP(signo), (Sigfunc *) Z_LVAL_PP(handle))==SIG_ERR) {
			php_error(E_WARNING, "Error assigning singal in %s", get_active_function_name());
		   	RETURN_FALSE;
		}
		RETURN_TRUE;
	} 
	
        if (Z_TYPE_PP(handle)!=IS_STRING) {
		php_error(E_WARNING, "Invalid type specified for handle argument in %s", get_active_function_name());
		RETURN_FALSE;
	}

	convert_to_string_ex(handle); /* Just in case */
	if (!zend_is_callable(*handle, 0, &func_name)) {
		php_error(E_WARNING, "%s: %s is not a callable function name error", get_active_function_name(), func_name);
	   	efree(func_name);
		RETURN_FALSE;
	}
   	efree(func_name);

        /* Add the function name to our signal table */
	zend_hash_index_update(&PCNTL_G(php_signal_table), Z_LVAL_PP(signo), Z_STRVAL_PP(handle),  (Z_STRLEN_PP(handle) + 1) * sizeof(char), NULL);
	
	if (php_signal(Z_LVAL_PP(signo), pcntl_signal_handler)==SIG_ERR) {
 			php_error(E_WARNING, "Error assigning singal in %s", get_active_function_name());
		   	RETURN_FALSE;
	}
	RETURN_TRUE;	
}
/* }}} */

/* Our custom signal handler that calls the appropriate php_function */
static void pcntl_signal_handler(int signo)
{
	char *func_name;
	zval *param, *call_name, *retval;
   
	PCNTL_LS_FETCH();
	/* printf("Caught signal: %d\n", signo); */
        if (zend_hash_index_find(&PCNTL_G(php_signal_table), (long) signo, (void *) &func_name)==FAILURE) {
		/* printf("Signl handler not fount"); */
		return;
	}
	/* printf("Signal handler found, Calling %s\n", func_name); */
	MAKE_STD_ZVAL(param);
	MAKE_STD_ZVAL(call_name);
	MAKE_STD_ZVAL(retval);
	ZVAL_LONG(param, signo);
 	ZVAL_STRING(call_name, func_name, 1);

	/* Call php singal handler - Note that we do not report errors, and we ignore the return value */ 
	call_user_function(EG(function_table), NULL, call_name, retval, 1, &param);
	
 	zval_dtor(call_name);
	efree(call_name);
	efree(param);

	return;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
