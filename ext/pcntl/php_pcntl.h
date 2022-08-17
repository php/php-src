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
	bool async_signals;
	unsigned num_signals;
ZEND_END_MODULE_GLOBALS(pcntl)

#if defined(ZTS) && defined(COMPILE_DL_PCNTL)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(pcntl)
#define PCNTL_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pcntl, v)

#endif	/* PHP_PCNTL_H */
