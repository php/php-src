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
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "php_main.h"
#include "SAPI.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "php_variables.h"
#include "php_ini.h"
#include "php4activescript.h"

/* SAPI definitions and DllMain */

static int php_activescript_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_activescript_module, 1) == FAILURE) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}

static int sapi_activescript_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	/* In theory, this is a blackhole.  In practice, I wan't to see the output
	 * in the debugger! */

	char buf[1024];
	uint l, a = str_length;

	while(a) {
		l = a;
		if (l > sizeof(buf) - 1)
			l = sizeof(buf) - 1;
		memcpy(buf, str, l);
		buf[l] = 0;
		OutputDebugString(buf);
		a -= l;
	}

	return str_length;
}

static void sapi_activescript_register_server_variables(zval *track_vars_array TSRMLS_DC)
{
}

static char *sapi_activescript_read_cookies(TSRMLS_D)
{
	return NULL;
}

static int sapi_activescript_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	return SAPI_HEADER_ADD;
}

static int sapi_activescript_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

zend_module_entry php_activescript_module = {
    STANDARD_MODULE_HEADER,
	"ActiveScript",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
    NULL,
	STANDARD_MODULE_PROPERTIES
};


sapi_module_struct activescript_sapi_module = {
	"activescript",						/* name */
	"Active Script",					/* pretty name */
									
	php_activescript_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	NULL,							/* deactivate */

	sapi_activescript_ub_write,			/* unbuffered write */
	NULL,							/* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	zend_error,						/* error handler */

	sapi_activescript_header_handler,		/* header handler */
	sapi_activescript_send_headers,		/* send headers handler */
	NULL,							/* send header handler */

	NULL,			/* read POST data */
	sapi_activescript_read_cookies,		/* read Cookies */

	sapi_activescript_register_server_variables,	/* register server variables */
	NULL,									/* Log message */

	STANDARD_SAPI_MODULE_PROPERTIES
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			module_handle = hinstDLL;

			tsrm_startup(128, 1, TSRM_ERROR_LEVEL_CORE, "C:\\TSRM.log");

			sapi_startup(&activescript_sapi_module);
			if (activescript_sapi_module.startup) {
				activescript_sapi_module.startup(&sapi_module);
			}
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			ts_free_thread();
			break;
		case DLL_PROCESS_DETACH:
			if (activescript_sapi_module.shutdown) {
				activescript_sapi_module.shutdown(&sapi_module);
			}
			tsrm_shutdown();
			break;
	}
	return TRUE;
}


