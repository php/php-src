/*
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
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_PCNTL_H
#define PHP_PCNTL_H

#if defined(WCONTINUED) && defined(WIFCONTINUED)
#define HAVE_WCONTINUED 1
#endif

extern zend_module_entry pcntl_module_entry;
#define phpext_pcntl_ptr &pcntl_module_entry

#include "php_version.h"
#define PHP_PCNTL_VERSION PHP_VERSION

PHP_MINIT_FUNCTION(pcntl);
PHP_MSHUTDOWN_FUNCTION(pcntl);
PHP_RINIT_FUNCTION(pcntl);
PHP_RSHUTDOWN_FUNCTION(pcntl);
PHP_MINFO_FUNCTION(pcntl);

PHP_FUNCTION(pcntl_alarm);
PHP_FUNCTION(pcntl_fork);
PHP_FUNCTION(pcntl_waitpid);
PHP_FUNCTION(pcntl_wait);
PHP_FUNCTION(pcntl_wifexited);
PHP_FUNCTION(pcntl_wifstopped);
PHP_FUNCTION(pcntl_wifsignaled);
#ifdef HAVE_WCONTINUED
PHP_FUNCTION(pcntl_wifcontinued);
#endif
PHP_FUNCTION(pcntl_wexitstatus);
PHP_FUNCTION(pcntl_wtermsig);
PHP_FUNCTION(pcntl_wstopsig);
PHP_FUNCTION(pcntl_signal);
PHP_FUNCTION(pcntl_signal_get_handler);
PHP_FUNCTION(pcntl_signal_dispatch);
PHP_FUNCTION(pcntl_get_last_error);
PHP_FUNCTION(pcntl_strerror);
#ifdef HAVE_SIGPROCMASK
PHP_FUNCTION(pcntl_sigprocmask);
#endif
#ifdef HAVE_STRUCT_SIGINFO_T
# if defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
PHP_FUNCTION(pcntl_sigwaitinfo);
PHP_FUNCTION(pcntl_sigtimedwait);
# endif
#endif
PHP_FUNCTION(pcntl_exec);
#ifdef HAVE_GETPRIORITY
PHP_FUNCTION(pcntl_getpriority);
#endif
#ifdef HAVE_SETPRIORITY
PHP_FUNCTION(pcntl_setpriority);
#endif
PHP_FUNCTION(pcntl_async_signals);
#ifdef HAVE_UNSHARE
PHP_FUNCTION(pcntl_unshare);
#endif

struct php_pcntl_pending_signal {
	struct php_pcntl_pending_signal *next;
	zend_long signo;
#ifdef HAVE_STRUCT_SIGINFO_T
	siginfo_t siginfo;
#endif
};

ZEND_BEGIN_MODULE_GLOBALS(pcntl)
	HashTable php_signal_table;
	int processing_signal_queue;
	struct php_pcntl_pending_signal *head, *tail, *spares;
	int last_error;
	volatile char pending_signals;
	zend_bool async_signals;
ZEND_END_MODULE_GLOBALS(pcntl)

#if defined(ZTS) && defined(COMPILE_DL_PCNTL)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(pcntl)
#define PCNTL_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pcntl, v)

#define REGISTER_PCNTL_ERRNO_CONSTANT(name) REGISTER_LONG_CONSTANT("PCNTL_" #name, name, CONST_CS | CONST_PERSISTENT)

#endif	/* PHP_PCNTL_H */
