/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: David Croft <david@infotrek.co.uk>,                         |
   |          John Donagher <john@webmeta.com>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* {{{ includes */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_pfpro.h"

#include "pfpro.h"

#if HAVE_PFPRO

#include "ext/standard/php_string.h"
/* }}} */

/* {{{ zts */
#ifdef ZTS
int pfpro_globals_id;
#else
php_pfpro_globals pfpro_globals;
#endif
/* }}} */

/* {{{ Function table */
function_entry pfpro_functions[] = {
	PHP_FE(pfpro_version, NULL)
	PHP_FE(pfpro_init, NULL)
	PHP_FE(pfpro_cleanup, NULL)
	PHP_FE(pfpro_process_raw, NULL)
	PHP_FE(pfpro_process, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ Zend module entry */
zend_module_entry pfpro_module_entry = {
	STANDARD_MODULE_HEADER,
	"pfpro",
	pfpro_functions,
	PHP_MINIT(pfpro),
	PHP_MSHUTDOWN(pfpro),
	PHP_RINIT(pfpro),					/* request start */
	PHP_RSHUTDOWN(pfpro),				/* request end */
	PHP_MINFO(pfpro),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ dl() stuff */
#ifdef COMPILE_DL_PFPRO
ZEND_GET_MODULE(pfpro)
#endif
/* }}} */

/* {{{ initialization defaults */
PHP_INI_BEGIN()
#if PFPRO_VERSION < 3
	STD_PHP_INI_ENTRY("pfpro.defaulthost",		"test.signio.com",
	PHP_INI_ALL, OnUpdateString,	defaulthost, php_pfpro_globals,	pfpro_globals) 
#else
	STD_PHP_INI_ENTRY("pfpro.defaulthost",		"test-payflow.verisign.com",
	PHP_INI_ALL, OnUpdateString,	defaulthost, php_pfpro_globals,	pfpro_globals)
#endif
	STD_PHP_INI_ENTRY("pfpro.defaultport",			"443",			PHP_INI_ALL, OnUpdateInt,		defaultport,			php_pfpro_globals,	pfpro_globals)
	STD_PHP_INI_ENTRY("pfpro.defaulttimeout",		"30",			PHP_INI_ALL, OnUpdateInt,		defaulttimeout,			php_pfpro_globals,	pfpro_globals)
	STD_PHP_INI_ENTRY("pfpro.proxyaddress",			"",				PHP_INI_ALL, OnUpdateString,	proxyaddress,			php_pfpro_globals,	pfpro_globals)
	STD_PHP_INI_ENTRY("pfpro.proxyport",			"",				PHP_INI_ALL, OnUpdateInt,		proxyport,			php_pfpro_globals,	pfpro_globals)
	STD_PHP_INI_ENTRY("pfpro.proxylogon",			"",				PHP_INI_ALL, OnUpdateString,	proxylogon,			php_pfpro_globals,	pfpro_globals)
	STD_PHP_INI_ENTRY("pfpro.proxypassword",		"",				PHP_INI_ALL, OnUpdateString,	proxypassword,			php_pfpro_globals,	pfpro_globals)
PHP_INI_END()


PHP_MINIT_FUNCTION(pfpro)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pfpro)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RINIT_FUNCTION(pfpro)
{
	PFPROG(initialized) = 0;

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(pfpro)
{
	if (PFPROG(initialized) == 1) {
		pfproCleanup();
	}

    return SUCCESS;
}
/* }}} */

/* {{{ minfo registration */
PHP_MINFO_FUNCTION(pfpro)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Verisign Payflow Pro support", "enabled");
	php_info_print_table_row(2, "libpfpro version", pfproVersion());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto string pfpro_version()
   Returns the version of the Payflow Pro library */
PHP_FUNCTION(pfpro_version)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING((char *)pfproVersion(), 1);
}
/* }}} */

/* {{{ proto void pfpro_init()
   Initializes the Payflow Pro library */
PHP_FUNCTION(pfpro_init)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	pfproInit();

	PFPROG(initialized) = 1;

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

	pfproCleanup();

	PFPROG(initialized) = 0;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string pfpro_process_raw(string parmlist [, string hostaddress [, int port, [, int timeout [, string proxyAddress [, int proxyPort [, string proxyLogon [, string proxyPassword]]]]]]])
   Raw Payflow Pro transaction processing */
PHP_FUNCTION(pfpro_process_raw)
{
	zval ***args;

	char *parmlist = NULL;
	char *address = NULL;
	int port = PFPROG(defaultport);
	int timeout = PFPROG(defaulttimeout);
	char *proxyAddress = PFPROG(proxyaddress);
	int proxyPort = PFPROG(proxyport);
	char *proxyLogon = PFPROG(proxylogon);
	char *proxyPassword = PFPROG(proxypassword);

	int freeaddress = 0;

#if PFPRO_VERSION < 3
	char response[512] = "";
#else
	int context;
	char *response;
#endif

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 8) {
		WRONG_PARAM_COUNT;
	}

	args = (zval ***) emalloc(sizeof(zval **) * ZEND_NUM_ARGS());

	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
        php_error(E_WARNING, "Unable to read parameters in pfpro_process_raw()");
		efree(args);
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 8:
			convert_to_string_ex(args[7]);
			proxyPassword = Z_STRVAL_PP(args[7]);
			/* fall through */

		case 7:
			convert_to_string_ex(args[6]);
			proxyLogon = Z_STRVAL_PP(args[6]);
			/* fall through */

		case 6:
			convert_to_long_ex(args[5]);
			proxyPort = Z_LVAL_PP(args[5]);
			/* fall through */

		case 5:
			convert_to_string_ex(args[4]);
			proxyAddress = Z_STRVAL_PP(args[4]);
			/* fall through */

		case 4:
			convert_to_long_ex(args[3]);
			timeout = Z_LVAL_PP(args[3]);
			/* fall through */

		case 3:
			convert_to_long_ex(args[2]);
			port = Z_LVAL_PP(args[2]);
			/* fall through */

		case 2:
			convert_to_string_ex(args[1]);
			address = Z_STRVAL_PP(args[1]);
	}

	convert_to_string_ex(args[0]);
	parmlist = Z_STRVAL_PP(args[0]);

	efree(args);

	/* Default to signio's test server */

	if (address == NULL) {
		address = estrdup(PFPROG(defaulthost));
		freeaddress = 1;
	}

#if PFPRO_VERSION < 3
	/* Blank the response buffer */
	memset(response, 0, sizeof(response));
#endif

	/* Initialize the library if needed */

	if (PFPROG(initialized) == 0) {
		pfproInit();
		PFPROG(initialized) = 1;
	}

	/* Perform the transaction */

#if PFPRO_VERSION < 3
	ProcessPNTransaction(address, port, proxyAddress, proxyPort, proxyLogon, proxyPassword, parmlist, strlen(parmlist), timeout, response);
#else
	pfproCreateContext(&context, address, port, timeout, proxyAddress, proxyPort, proxyLogon, proxyPassword);
	pfproSubmitTransaction(context, parmlist, strlen(parmlist), &response);
	pfproDestroyContext(context);
#endif

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
	zval ***args;

	HashTable *target_hash;
	ulong num_key;
	char *string_key;
	zval **entry;
	int pass;

	char *parmlist = NULL;
	char *address = NULL;
	int port = PFPROG(defaultport);
	int timeout = PFPROG(defaulttimeout);
	char *proxyAddress = PFPROG(proxyaddress);
	int proxyPort = PFPROG(proxyport);
	char *proxyLogon = PFPROG(proxylogon);
	char *proxyPassword = PFPROG(proxypassword);

	int parmlength = 0;
	int freeaddress = 0;

#if PFPRO_VERSION < 3
	char response[512] = "";
#else
	int context;
	char *response;
#endif

	char tmpbuf[128];

    char buf[128], sbuf[128];
    char *p1, *p2, *p_end,          /* Pointers for string manipulation */
        *sp1, *sp2,
        *pdelim1="&", *pdelim2="=";

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 8) {
		WRONG_PARAM_COUNT;
	}

	args = (zval ***) emalloc(sizeof(zval **) * ZEND_NUM_ARGS());

	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
        php_error(E_ERROR, "Unable to read parameters in pfpro_process()");
 		efree(args);
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(args[0]) != IS_ARRAY) {
		php_error(E_ERROR, "First parameter to pfpro_process() must be an array");
 		efree(args);
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 8:
			convert_to_string_ex(args[7]);
			proxyPassword = Z_STRVAL_PP(args[7]);
			/* fall through */

		case 7:
			convert_to_string_ex(args[6]);
			proxyLogon = Z_STRVAL_PP(args[6]);
			/* fall through */

		case 6:
			convert_to_long_ex(args[5]);
			proxyPort = Z_LVAL_PP(args[5]);
			/* fall through */

		case 5:
			convert_to_string_ex(args[4]);
			proxyAddress = Z_STRVAL_PP(args[4]);
			/* fall through */

		case 4:
			convert_to_long_ex(args[3]);
			timeout = Z_LVAL_PP(args[3]);
			/* fall through */

		case 3:
			convert_to_long_ex(args[2]);
			port = Z_LVAL_PP(args[2]);
			/* fall through */

		case 2:
			convert_to_string_ex(args[1]);
			address = Z_STRVAL_PP(args[1]);
	}

	/* Concatenate the passed array as specified by Verisign.
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

			if (parmlength > 0) {
				if (pass == 1)
					strcpy(parmlist + parmlength, "&");
				parmlength += 1;
			}

			switch (zend_hash_get_current_key(target_hash, &string_key, &num_key, 0)) {

				case HASH_KEY_IS_STRING:

					if (pass == 1)
						strcpy(parmlist + parmlength, string_key);
					parmlength += strlen(string_key);

					break;

				case HASH_KEY_IS_LONG:

					sprintf(tmpbuf, "%d", num_key);
					if (pass == 1)
						strcpy(parmlist + parmlength, tmpbuf);
					parmlength += strlen(tmpbuf);

					break;

				default:
					php_error(E_ERROR, "pfpro_process() array keys must be strings or integers");
					efree(args);
					RETURN_FALSE;
			}


			switch (Z_TYPE_PP(entry)) {
				case IS_STRING:
					if (strchr(Z_STRVAL_PP(entry), '&')
						|| strchr(Z_STRVAL_PP(entry), '=')) {
						sprintf(tmpbuf, "[%d]=", Z_STRLEN_PP(entry));
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
						strcpy(parmlist + parmlength, Z_STRVAL_PP(entry));
					parmlength += Z_STRLEN_PP(entry);

					break;

				case IS_LONG:
					sprintf(tmpbuf, "=%d", Z_LVAL_PP(entry));
					if (pass == 1)
						strcpy(parmlist + parmlength, tmpbuf);
					parmlength += strlen(tmpbuf);

					break;

				case IS_DOUBLE:
					sprintf(tmpbuf, "=%.2f", Z_DVAL_PP(entry));
					if (pass == 1)
						strcpy(parmlist + parmlength, tmpbuf);
					parmlength += strlen(tmpbuf);

					break;

				default:
					php_error(E_ERROR, "pfpro_process() array values must be strings, ints or floats");
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
		address = estrdup(PFPROG(defaulthost));
		freeaddress = 1;
	}

	/* Allocate the array for the response now - so we catch any errors
	   from this BEFORE we knock it off to the bank */

	if (array_init(return_value) == FAILURE) {
		php_error(E_ERROR, "pfpro_process() unable to create array");
		RETURN_FALSE;
	}

#if PFPRO_VERSION < 3
	/* Blank the response buffer */
	memset(response, 0, sizeof(response));
#endif

	/* Initialize the library if needed */

	if (PFPROG(initialized) == 0) {
		pfproInit();
		PFPROG(initialized) = 1;
	}

	/* Perform the transaction */

#if PFPRO_VERSION < 3
	ProcessPNTransaction(address, port, proxyAddress, proxyPort, proxyLogon, proxyPassword, parmlist, strlen(parmlist), timeout, response);
#else
	pfproCreateContext(&context, address, port, timeout, proxyAddress, proxyPort, proxyLogon, proxyPassword);
	pfproSubmitTransaction(context, parmlist, strlen(parmlist), &response);
	pfproDestroyContext(context);
#endif

	if (freeaddress) {
		efree(address);
	}


	/* This final chunk of code is to walk the string returned by Signio
	   and build a string array to return to the user */

	/* John, I suspect this code will fall over if there are less than
	   3 items in the response string -- david */

	/* Clean our str[n]cpy buffers */
	memset(buf, 0, sizeof(buf));
	memset(sbuf, 0, sizeof(sbuf));

	p_end = response + strlen(response);
	p1 = response;
	p2 = (char*)php_memnstr(response, pdelim1, 1, p_end);

	sp1 = (char*)php_memnstr(response, pdelim2, 1, p2);
	strncpy(buf, p1, sp1-p1);

	sp1++;
	strncpy(sbuf, sp1, p2-sp1);

	add_assoc_string(return_value, &buf[0], &sbuf[0], 1);

	do {
		memset(buf, 0, sizeof(buf));
		memset(sbuf, 0, sizeof(sbuf));

		p1 = p2+1;

		if ((sp2 = (char*)php_memnstr(p1, pdelim1, 1, p_end)) != NULL) {
			sp1 = (char*)php_memnstr(p1, pdelim2, 1, sp2);
			strncpy(buf, p1, sp1-p1);

			sp1++;

			strncpy(sbuf, sp1, sp2-sp1);

			add_assoc_string(return_value, &buf[0], &sbuf[0], 1);
		}


	} while ((p2 = (char*)php_memnstr(p1, pdelim1, 1, p_end)) != NULL);

	if (p1 <= p_end) {
		memset(buf, 0, sizeof(buf));
		memset(sbuf, 0, sizeof(sbuf));

		sp1 = (char*)php_memnstr(p1, pdelim2, 1, p_end);
		strncpy(buf, p1, sp1-p1);

		sp1++;
		strncpy(sbuf, sp1, p_end-sp1);

		add_assoc_string(return_value, &buf[0], &sbuf[0], 1);
	}


}
/* }}} */

#endif	/* HAVE_PFPRO */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
