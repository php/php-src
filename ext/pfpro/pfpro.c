/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: David Croft <david@infotrek.co.uk>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Note: I've left the globals and ini-file handling stuff in
   as I've yet to decide whether this is a good way to place "default"
   stuff like hosts and proxies */

/* Status: Working. Awaiting comments from Signio as to whether
   PNInit and ProcessPNTransaction have any meaningful return value */

#include "php.h"
#include "php_ini.h"
#include "php_pfpro.h"

#include "pfpro.h"

#if HAVE_PFPRO

#ifdef ZTS
int pfpro_globals_id;
#else
php_pfpro_globals pfpro_globals;
#endif

/* Function table */

function_entry pfpro_functions[] = {
	PHP_FE(pfpro_version, NULL)
	PHP_FE(pfpro_init, NULL)
	PHP_FE(pfpro_cleanup, NULL)
	PHP_FE(pfpro_process_raw, NULL)
	PHP_FE(pfpro_process, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry pfpro_module_entry = {
	"pfpro",
	pfpro_functions,
	PHP_MINIT(pfpro),
	PHP_MSHUTDOWN(pfpro),
	NULL, 					/* request start */
	NULL, 					/* request end */
	PHP_MINFO(pfpro),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PFPRO
ZEND_GET_MODULE(pfpro)
#endif

/*
PHP_INI_BEGIN()
PHP_INI_END()
*/

PHP_MINIT_FUNCTION(pfpro)
{
/*
	REGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pfpro)
{
/*
	UNREGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MINFO_FUNCTION(pfpro)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Signio Payflow Pro support", "enabled");
	php_info_print_table_end();

	/*
	DISPLAY_INI_ENTRIES();
	*/
}



/* {{{ proto string pfpro_version()
   Returns the version of the Payflow Pro library */
PHP_FUNCTION(pfpro_version)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING(PNVersion(), 1);
}
/* }}} */



/* {{{ proto void pfpro_init()
   Initialises the Payflow Pro library */
PHP_FUNCTION(pfpro_init)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	PNInit();

	RETURN_TRUE;
}
/* }}} */



/* {{{ proto void pfpro_cleanup()
   Shuts down the Payflow Pro library */
PHP_FUNCTION(pfpro_cleanup)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	PNCleanup();

	RETURN_TRUE;
}
/* }}} */



/* {{{ proto string pfpro_process_raw(string parmlist [, string hostaddress [, int port, [, int timeout [, string proxyAddress [, int proxyPort [, string proxyLogon [, string proxyPassword]]]]]]])
   Raw Payflow Pro transaction processing */
PHP_FUNCTION(pfpro_process_raw)
{
	pval ***args;

	char *parmlist;
	char *address = NULL;
	int port = 443;
	int timeout = 30;
	char *proxyAddress = NULL;
	int proxyPort = 0;
	char *proxyLogon = NULL;
	char *proxyPassword = NULL;

	int freeaddress = 0;

	/* No, I don't like that Signio tell you to use a
	   fixed length buffer either */

	char response[512];

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 8) {
		WRONG_PARAM_COUNT;
	}

	args = (pval ***) emalloc(sizeof(pval **) * ZEND_NUM_ARGS());

	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
        php_error(E_WARNING, "Unable to read parameters in pfpro_process_raw()");
		efree(args);
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 8:
			convert_to_string_ex(args[7]);
			proxyPassword = (*args[7])->value.str.val;
			/* fall through */

		case 7:
			convert_to_string_ex(args[6]);
			proxyLogon = (*args[6])->value.str.val;
			/* fall through */

		case 6:
			convert_to_long_ex(args[5]);
			proxyPort = (*args[5])->value.lval;
			/* fall through */

		case 5:
			convert_to_string_ex(args[4]);
			proxyAddress = (*args[4])->value.str.val;
			/* fall through */

		case 4:
			convert_to_long_ex(args[3]);
			timeout = (*args[3])->value.lval;
			/* fall through */

		case 3:
			convert_to_long_ex(args[2]);
			port = (*args[2])->value.lval;
			/* fall through */

		case 2:
			convert_to_string_ex(args[1]);
			address = (*args[1])->value.str.val;
	}

	convert_to_string_ex(args[0]);
	parmlist = (*args[0])->value.str.val;

	efree(args);

	/* Default to signio's test server */

	if (address == NULL) {
		address = estrdup("test.signio.com");
		freeaddress = 1;
	}

#if 0
	printf("Address: >%s<\n", address);
	printf("Port: >%d<\n", port);
	printf("Parmlist: >%s<\n", parmlist);
	printf("Timeout: >%d<\n", timeout);
	printf("Proxy address: >%s<\n", proxyAddress);
	printf("Proxy port: >%d<\n", proxyPort);
	printf("Proxy logon: >%s<\n", proxyLogon);
	printf("Proxy password: >%s<\n", proxyPassword);
#endif

	/* Blank the response buffer */

	memset(response, 0, sizeof(response));

	/* Perform the transaction */

	ProcessPNTransaction(address, port, proxyAddress, proxyPort, proxyLogon, proxyPassword, parmlist, strlen(parmlist), timeout, response);

	if (freeaddress) {
		efree(address);
	}

	RETURN_STRING(response, 1);
}
/* }}} */



/* {{{ proto array pfpro_process(array parmlist [, string hostaddress [, int port, [, int timeout [, string proxyAddress [, int proxyPort [, string proxyLogon [, string proxyPassword]]]]]]])
   Payflow Pro transaction processing using arrays */
PHP_FUNCTION(pfpro_process)
{
	pval ***args;

	HashTable *target_hash;
	ulong lkey;
	char *varname;
	zval **entry;
	int pass;

	char *address = NULL;
	int port = 443;
	int timeout = 30;
	char *proxyAddress = NULL;
	int proxyPort = 0;
	char *proxyLogon = NULL;
	char *proxyPassword = NULL;
	int freeaddress = 0;

	char *parmlist = NULL;
	int parmlength = 0;

	/* No, I don't like that Signio tell you to use a
	   fixed length buffer either */

	char response[512];

	char tmpbuf[128];
	char *rsppos, *valpos;


	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 8) {
		WRONG_PARAM_COUNT;
	}

	args = (pval ***) emalloc(sizeof(pval **) * ZEND_NUM_ARGS());

	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
        php_error(E_WARNING, "Unable to read parameters in pfpro_process()");
 		efree(args);
		RETURN_FALSE;
	}

	if ((*args[0])->type != IS_ARRAY) {
		php_error(E_WARNING, "First parameter to pfpro_process() must be an array");
 		efree(args);
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 8:
			convert_to_string_ex(args[7]);
			proxyPassword = (*args[7])->value.str.val;
			/* fall through */

		case 7:
			convert_to_string_ex(args[6]);
			proxyLogon = (*args[6])->value.str.val;
			/* fall through */

		case 6:
			convert_to_long_ex(args[5]);
			proxyPort = (*args[5])->value.lval;
			/* fall through */

		case 5:
			convert_to_string_ex(args[4]);
			proxyAddress = (*args[4])->value.str.val;
			/* fall through */

		case 4:
			convert_to_long_ex(args[3]);
			timeout = (*args[3])->value.lval;
			/* fall through */

		case 3:
			convert_to_long_ex(args[2]);
			port = (*args[2])->value.lval;
			/* fall through */

		case 2:
			convert_to_string_ex(args[1]);
			address = (*args[1])->value.str.val;
	}

	/* Concatenate the passed array as specified by signio.
	   Basically it's all key=value&key=value, the only exception
	   being if the value contains = or &, in which case we also
	   encode the length, e.g. key[5]=bl&ah */

	target_hash = HASH_OF(*args[0]);

	for (pass = 0; pass <= 1; pass ++) {

		parmlength = 0;
		/* we go around the array twice. the first time to calculate
		   the string length, the second time to actually store it */

		zend_hash_internal_pointer_reset(target_hash);

		while (zend_hash_get_current_data(target_hash, (void **)&entry) == SUCCESS) {
			if (zend_hash_get_current_key(target_hash, &varname, &lkey) == HASH_KEY_IS_STRING) {
				if (parmlength > 0) {
					if (pass == 1)
						strcpy(parmlist + parmlength, "&");
					parmlength += 1;
				}

				if (pass == 1)
					strcpy(parmlist + parmlength, varname);
				parmlength += strlen(varname);

				if ((*entry)->type == IS_STRING) {
					if (strchr((*entry)->value.str.val, '&') ||
						strchr((*entry)->value.str.val, '=')) {
						sprintf(tmpbuf, "[%d]=", (*entry)->value.str.len);
						if (pass == 1)
							strcpy(parmlist + parmlength, tmpbuf);
						parmlength += strlen(tmpbuf);
					}
					else {
						if (pass == 1)
							strcpy(parmlist + parmlength, "=");
						parmlength += 1;
					}
					if (pass == 1)
						strcpy(parmlist + parmlength, (*entry)->value.str.val);
					parmlength += (*entry)->value.str.len;
				}
				else if ((*entry)->type == IS_LONG) {
					sprintf(tmpbuf, "=%d", (*entry)->value.lval);
					if (pass == 1)
						strcpy(parmlist + parmlength, tmpbuf);
					parmlength += strlen(tmpbuf);
				}
				else if ((*entry)->type == IS_DOUBLE) {
					sprintf(tmpbuf, "=%.2f", (*entry)->value.dval);
					if (pass == 1)
						strcpy(parmlist + parmlength, tmpbuf);
					parmlength += strlen(tmpbuf);
				}
				else {
					php_error(E_WARNING, "pfpro_process() array values should be strings, ints or floats");
				}
				efree(varname);
			}
			else {
				php_error(E_WARNING, "pfpro_process() array keys must be strings");
				efree(args);
				RETURN_FALSE;
			}
			zend_hash_move_forward(target_hash);
		}

		if (pass == 0) {
			parmlist = emalloc(parmlength + 1);
		}
	}

	efree(args);

	/* Default to signio's test server */

	if (address == NULL) {
		/* is it safe to just do address = "test.signio.com"; here? */
		address = estrdup("test.signio.com");
		freeaddress = 1;
	}

#if 0
	printf("Address: >%s<\n", address);
	printf("Port: >%d<\n", port);
	printf("Parmlist: >%s<\n", parmlist);
	printf("Timeout: >%d<\n", timeout);
	printf("Proxy address: >%s<\n", proxyAddress);
	printf("Proxy port: >%d<\n", proxyPort);
	printf("Proxy logon: >%s<\n", proxyLogon);
	printf("Proxy password: >%s<\n", proxyPassword);
#endif

	/* Allocate the array for the response now - so we catch any errors
	   from this BEFORE we knock it off to the bank */

	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "pfpro_process() unable to create array");
		RETURN_FALSE;
	}

	/* Blank the response buffer */

	memset(response, 0, sizeof(response));

	/* Perform the transaction */

	ProcessPNTransaction(address, port, proxyAddress, proxyPort, proxyLogon, proxyPassword, parmlist, parmlength, timeout, response);

	if (freeaddress) {
		efree(address);
	}

	/* Decode the response back into a PHP array */

	rsppos = strtok(response, "&");

	do {
		valpos = strchr(rsppos, '=');
		if (valpos) {
			strncpy(tmpbuf, rsppos, valpos - rsppos);
			tmpbuf[valpos - rsppos] = 0;
			add_assoc_string(return_value, tmpbuf, valpos + 1, 1);
		}
		
	} while (rsppos = strtok(NULL, "&"));
}
/* }}} */



#endif	/* HAVE_PFPRO */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
