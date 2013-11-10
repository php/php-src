/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"

ZEND_DECLARE_MODULE_GLOBALS(phpdbg);

void (*zend_execute_old)(zend_execute_data *execute_data TSRMLS_DC);
void (*zend_execute_internal_old)(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC);

static inline void php_phpdbg_globals_ctor(zend_phpdbg_globals *pg) {
    pg->exec = NULL;
    pg->ops = NULL;
    pg->stepping = 0;
    pg->vmret = 0;
}

static PHP_MINIT_FUNCTION(phpdbg) {
    ZEND_INIT_MODULE_GLOBALS(phpdbg, php_phpdbg_globals_ctor, NULL);

    zend_execute_old = zend_execute_ex;
    zend_execute_ex = phpdbg_execute_ex;

    return SUCCESS;
}

static inline void php_phpdbg_destroy_break(void *brake) {

}

static PHP_RINIT_FUNCTION(phpdbg) {
	zend_hash_init(&PHPDBG_G(break_files),   8, NULL, php_phpdbg_destroy_break, 0);
	zend_hash_init(&PHPDBG_G(break_symbols), 8, NULL, php_phpdbg_destroy_break, 0);

	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(phpdbg) {
    zend_hash_destroy(&PHPDBG_G(break_files));
    zend_hash_destroy(&PHPDBG_G(break_symbols));

    if (PHPDBG_G(exec)) {
        efree(PHPDBG_G(exec));
    }

    if (PHPDBG_G(ops)) {
        destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
        efree(PHPDBG_G(ops));
    }
    return SUCCESS;
}

static zend_module_entry sapi_phpdbg_module_entry = {
	STANDARD_MODULE_HEADER,
	"phpdbg",
	NULL,
	PHP_MINIT(phpdbg),
	NULL,
	PHP_RINIT(phpdbg),
	PHP_RSHUTDOWN(phpdbg),
	NULL,
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

static inline int php_sapi_phpdbg_module_startup(sapi_module_struct *module) /* {{{ */
{
    if (php_module_startup(module, &sapi_phpdbg_module_entry, 1) == FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
} /* }}} */

/* {{{ sapi_module_struct phpdbg_sapi_module
 */
static sapi_module_struct phpdbg_sapi_module = {
	"phpdbg",						/* name */
	"phpdbg",					    /* pretty name */

	php_sapi_phpdbg_module_startup,	/* startup */
	php_module_shutdown_wrapper,    /* shutdown */

	NULL,		                    /* activate */
	NULL,		                    /* deactivate */

	NULL,			                /* unbuffered write */
	NULL,				            /* flush */
	NULL,							/* get uid */
	NULL,				            /* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	NULL,			                /* send headers handler */
	NULL,							/* send header handler */

	NULL,				            /* read POST data */
	NULL,			                /* read Cookies */

	NULL,	                        /* register server variables */
	NULL,			                /* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */
	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

int main(int argc, char **argv) /* {{{ */
{
	sapi_module_struct *phpdbg = &phpdbg_sapi_module;
#ifdef ZTS
	void ***tsrm_ls;
	tsrm_startup(1, 1, 0, NULL);

	tsrm_ls = ts_resource(0);
#endif

#ifdef PHP_WIN32
	_fmode = _O_BINARY;                 /* sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);  /* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY); /* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY); /* make the stdio mode be binary */
#endif

	phpdbg->executable_location = argv[0];

	sapi_startup(phpdbg);

	if (phpdbg->startup(phpdbg) == SUCCESS) {
		zend_activate(TSRMLS_C);

#ifdef ZEND_SIGNALS
		zend_try {
			zend_signals_activate(TSRMLS_C);
		} zend_end_try();
#endif

		PG(modules_activated) = 0;

		zend_try {
			zend_activate_modules(TSRMLS_C);
		} zend_end_try();

		zend_try {
			phpdbg_interactive(argc, argv TSRMLS_CC);
		} zend_end_try();

		if (PG(modules_activated)) {
			zend_try {
				zend_deactivate_modules(TSRMLS_C);
			} zend_end_try();
		}

		zend_deactivate(TSRMLS_C);

		zend_try {
			zend_post_deactivate_modules(TSRMLS_C);
		} zend_end_try();

#ifdef ZEND_SIGNALS
		zend_try {
			zend_signal_deactivate(TSRMLS_C);
		} zend_end_try();
#endif

		php_module_shutdown(TSRMLS_C);

		sapi_shutdown();
	}

#ifdef ZTS
	tsrm_shutdown();
#endif

	return 0;
} /* }}} */
