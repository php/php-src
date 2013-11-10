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

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_modules.h"
#include "php.h"
#include "zend_ini_scanner.h"
#include "zend_globals.h"
#include "zend_stream.h"
#include "SAPI.h"
#include <php_config.h>
#include "php_main.h"

static zend_module_entry sapi_phpdbg_module_entry = {
	STANDARD_MODULE_HEADER,
	"phpdbg",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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

static inline int zend_machine(int argc, char **argv TSRMLS_DC) /* {{{ */
{
    php_printf("Hello World :)\n");
} /* }}} */

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

		/* START: ZEND INITIALIZED */
		{
			/* do the thing */
			zend_machine(argc, argv TSRMLS_CC);
		}
		/* END: ZEND BLOCK */

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
