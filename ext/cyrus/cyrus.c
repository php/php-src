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
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_cyrus.h"

#if HAVE_CYRUS

#include <cyrus/imclient.h>

#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

static int le_cyrus;
#define le_cyrus_name "Cyrus IMAP connection"

function_entry cyrus_functions[] = {
	PHP_FE(cyrus_connect,		NULL)
	PHP_FE(cyrus_authenticate,	NULL)
	PHP_FE(cyrus_bind,			NULL)
	PHP_FE(cyrus_unbind,		NULL)
	PHP_FE(cyrus_query,			NULL)
	PHP_FE(cyrus_close,			NULL)
	{NULL, NULL, NULL}
};

zend_module_entry cyrus_module_entry = {
	STANDARD_MODULE_HEADER,
	"cyrus",
	cyrus_functions,
	PHP_MINIT(cyrus),
	NULL,
	NULL,	
	NULL,
	PHP_MINFO(cyrus),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CYRUS
ZEND_GET_MODULE(cyrus)
#endif

static void cyrus_free(zend_rsrc_list_entry *rsrc)
{
	php_cyrus *conn = (php_cyrus *) rsrc->ptr;

	if (conn->client)
		imclient_close(conn->client);

	if (conn->host) 
		efree(conn->host);

	if (conn->port)
		efree(conn->port);

	efree(conn);
}
	
PHP_MINIT_FUNCTION(cyrus)
{
	le_cyrus = zend_register_list_destructors_ex(cyrus_free, NULL, 
	                                             le_cyrus_name, module_number);

	REGISTER_LONG_CONSTANT("CYRUS_CONN_NONSYNCLITERAL", 
	                       IMCLIENT_CONN_NONSYNCLITERAL,
	                       CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CYRUS_CONN_INITIALRESPONSE", 
	                       IMCLIENT_CONN_INITIALRESPONSE,
	                       CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CYRUS_CALLBACK_NUMBERED", CALLBACK_NUMBERED,
	                       CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CYRUS_CALLBACK_NOLITERAL", CALLBACK_NOLITERAL,
	                       CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MINFO_FUNCTION(cyrus)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Cyrus IMAP support", "enabled");
	php_info_print_table_end();
}

extern void fatal(char *s, int exit)
{
	TSRMLS_FETCH();

	php_error_docref(NULL TSRMLS_CC, E_ERROR, s);
}

/* {{{ proto resource cyrus_connect([ string host [, string port [, int flags]]])
   Connect to a Cyrus IMAP server */
PHP_FUNCTION(cyrus_connect)
{
	zval            **z_host;
	zval            **z_port;
	zval            **z_flags;
	php_cyrus        *conn;
	struct imclient  *client;
	char             *host;
	char             *port = NULL;
	int               flags = 0;
	int               error;
	int               argc = ZEND_NUM_ARGS();
	
	if (argc < 0 || argc > 3 ||
	    zend_get_parameters_ex(argc, &z_host, &z_port, &z_flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc > 0) {
		convert_to_string_ex(z_host);
		host = estrndup(Z_STRVAL_PP(z_host), Z_STRLEN_PP(z_host));
	}
	else {
		host = estrndup("localhost", sizeof("localhost") - 1);
	}

	if (argc > 1) {
		convert_to_string_ex(z_port);
		port = estrndup(Z_STRVAL_PP(z_port), Z_STRLEN_PP(z_port));
	}

	if (argc > 2) {
		convert_to_long_ex(z_flags);
		flags = Z_LVAL_PP(z_flags);
	}

	error = imclient_connect(&client, host, port, NULL);
	switch (error) {
	case 0:
		if (client) {
			conn = ecalloc(1, sizeof *conn);
			conn->client = client;
			conn->host = host;
			conn->port = port;

			if (flags) {
				imclient_setflags(conn->client, flags);
				conn->flags = flags;
			}
		}

		break;

	case -1:
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid hostname: %s", host);
		RETURN_FALSE;
	
	case -2:
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid port: %d", port);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, conn, le_cyrus);
	conn->id = Z_LVAL_P(return_value);
}
/* }}} */


static void cyrus_capable_callback(struct imclient *client, void *rock, 
                                   struct imclient_reply *reply)
{
	char *token = NULL;
	char *token_buf;
	char *mechanism = rock;

	
	/* We need to split the reply up by the whitespace */
	token = php_strtok_r(reply->text, " ", &token_buf);
	while (token != NULL) {
		if (! strncmp(token, "AUTH=", 5)) {
			memcpy(mechanism, token + 5, strlen(token) - 5);
			break;
		}

		token = php_strtok_r(NULL, " ", &token_buf);
	}
}


/* {{{ proto bool cyrus_authenticate( resource connection [, string mechlist [, string service [, string user [, int minssf [, int maxssf]]]]])
   Authenticate agaings a Cyrus IMAP server */
PHP_FUNCTION(cyrus_authenticate)
{
	zval        **z_conn;
	zval        **z_mechlist;
	zval        **z_service;
	zval        **z_user;
	zval        **z_minssf;
	zval        **z_maxssf;
	php_cyrus    *conn;
	char         *mechlist;
	char         *service;
	char         *user;
	int           minssf;
	int           maxssf;
	int           argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 6 ||
	    zend_get_parameters_ex(argc, &z_conn, &z_mechlist, &z_service, &z_user, &z_minssf, &z_maxssf) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(conn, php_cyrus *, z_conn, -1, le_cyrus_name, le_cyrus);

	/* Determine support mechanisms */
	if (argc > 1 && Z_TYPE_PP(z_mechlist) != IS_NULL) {
		convert_to_string_ex(z_mechlist);

		mechlist = estrndup(Z_STRVAL_PP(z_mechlist), Z_STRLEN_PP(z_mechlist));
	}
	else {
		char tmp_mechlist[100];
		int  pos = 0;
		
		/* NULL out the buffer, ensures it has a safe ending and allows us to
		 * test properly for the end of the buffer
		 */
		memset(tmp_mechlist, 0, sizeof tmp_mechlist);

		/* We'll be calling the "CAPABILITY" command, which will give us a list
		 * of the types of authorization the server is capable of 
		 */
		imclient_addcallback(conn->client, "CAPABILITY", 0, 
		                     cyrus_capable_callback, (void *) tmp_mechlist, 0);
		imclient_send(conn->client, NULL, NULL, "CAPABILITY");

		/* Grab the end of string position into pos */
		while (tmp_mechlist[pos++] != 0) 
			;

		/* Tack on PLAIN to whatever the auth string is */
		memcpy(tmp_mechlist + pos, " PLAIN", 6);
		
		/* Copy it onto the main buffer */
		mechlist = estrndup(tmp_mechlist, pos + 6);
	}

	/* Determine the service type */
	if (argc > 2 && Z_TYPE_PP(z_service) != IS_NULL) {
		convert_to_string_ex(z_service);
		service = estrndup(Z_STRVAL_PP(z_service), Z_STRLEN_PP(z_service));
	}
	else {
		service = estrndup("imap", 4);
	}

	/* Determine the user */
	if (argc > 3 && Z_TYPE_PP(z_user) != IS_NULL) {
		convert_to_string_ex(z_user);
		user = estrndup(Z_STRVAL_PP(z_user), Z_STRLEN_PP(z_user));
	}
	else {
		/* XXX: UGLY, but works, determines the username to use */
		user = (char *) sapi_getenv("USER", 4);
		if (! user) {
		user = (char *) getenv("USER");
		if (! user) {
			user = (char *) sapi_getenv("LOGNAME", 7);
			if (! user) {
			user = (char *) getenv("LOGNAME");
			if (! user) {
				struct passwd *pwd = getpwuid(getuid());
				if (! pwd) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't determine user id");
				RETURN_FALSE;
				}
			
				user = estrdup(pwd->pw_name);
			}
			}
		}
		}
	}

	/* Determine the minssf */
	if (argc > 4 && Z_TYPE_PP(z_minssf) != NULL) {
		convert_to_long_ex(z_minssf);
		minssf = Z_LVAL_PP(z_minssf);
	}
	else {
		minssf = 0;
	}

	/* Determine the maxssf */
	if (argc > 5 && Z_TYPE_PP(z_maxssf) != NULL) {
		convert_to_long_ex(z_maxssf);
		maxssf = Z_LVAL_PP(z_maxssf);
	}
	else {
		maxssf = 1000;
	}

	imclient_authenticate(conn->client, mechlist, service, 
	                      user, minssf, maxssf);

	efree(mechlist);
	efree(service);
	efree(user);
}
/* }}} */


static void cyrus_generic_callback(struct imclient *client, 
                                   void *rock, 
                                   struct imclient_reply *reply)
{
	php_cyrus_callback *callback = rock;

	if (client) {
		zval **argv[4];
		zval  *retval;
		zval  *cyrus;
		zval  *keyword;
		zval  *text;
		zval  *msgno;
		TSRMLS_FETCH();

		MAKE_STD_ZVAL(cyrus);
		MAKE_STD_ZVAL(keyword);
		MAKE_STD_ZVAL(text);
		MAKE_STD_ZVAL(msgno);

		ZVAL_RESOURCE(cyrus, callback->le);
		zend_list_addref(callback->le);

		ZVAL_STRING(keyword, reply->keyword, 1);
		ZVAL_STRING(text, reply->text, 1);
		ZVAL_LONG(msgno, reply->msgno);

		argv[0] = &cyrus;
		argv[1] = &keyword;
		argv[2] = &text;
		argv[3] = &msgno;

		if (call_user_function_ex(EG(function_table), NULL, callback->function, 
                                  &retval, 4, argv, 0, NULL TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't call the %s handler", callback->trigger);
		}

		zval_ptr_dtor(argv[0]);
		zval_ptr_dtor(argv[1]);
		zval_ptr_dtor(argv[2]);
		zval_ptr_dtor(argv[3]);

		efree(argv);
	}
	else {
		return;
	}
}

	
/* {{{ proto bool cyrus_bind( resource connection, array callbacks) 
 Bind callbacks to a Cyrus IMAP connection */
PHP_FUNCTION(cyrus_bind)
{
	zval                  **z_conn;
	zval                  **z_callback;
	zval                  **tmp;
	HashTable              *hash;
	php_cyrus             *conn;
	php_cyrus_callback     callback;
	char                  *string_key;
	ulong                  num_key;

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &z_conn, &z_callback) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(conn, php_cyrus *, z_conn, -1, le_cyrus_name, le_cyrus);

	hash = HASH_OF(*z_callback);
	if (! hash) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument must be an array or object");
		RETURN_FALSE;
	}

	for (zend_hash_internal_pointer_reset(hash);
	     zend_hash_get_current_data(hash, (void **) &tmp) == SUCCESS;
		 zend_hash_move_forward(hash)) {
		SEPARATE_ZVAL(tmp);
		zend_hash_get_current_key(hash, &string_key, &num_key, 0);
		if (! string_key) 
			continue;
		
		if (! strcasecmp(string_key, "trigger")) {
			convert_to_string_ex(tmp);
			callback.trigger = estrndup(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
		}
		else if (! strcasecmp(string_key, "function")) {
			callback.function = *tmp;
			zval_add_ref(&callback.function);
		}
		else if (! strcasecmp(string_key, "flags")) {
			convert_to_long_ex(tmp);
			callback.flags |= Z_LVAL_PP(tmp);
		}
	}

	if (! callback.trigger) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must specify a trigger in your callback");
		RETURN_FALSE;
	}

	if (! callback.function) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must specify a function in your callback");
		RETURN_FALSE;
	}

	callback.le = conn->id;

	imclient_addcallback(conn->client, callback.trigger, callback.flags, 
	                     cyrus_generic_callback, (void **) &callback, 0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool cyrus_unbind( resource connection, string trigger_name)
   Unbind ... */
PHP_FUNCTION(cyrus_unbind)
{
	zval        **z_conn;
	zval        **trigger_name;
	php_cyrus    *conn;

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &z_conn, &trigger_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(conn, php_cyrus *, z_conn, -1, le_cyrus_name, le_cyrus);
	convert_to_string_ex(trigger_name);

	imclient_addcallback(conn->client, Z_STRVAL_PP(trigger_name), 0, 
	                     NULL, NULL, 0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool cyrus_query( resource connection, string query) 
   Send a query to a Cyrus IMAP server */
PHP_FUNCTION(cyrus_query)
{
	zval               **z_conn;
	zval               **query;
	php_cyrus           *conn;

	if (ZEND_NUM_ARGS() != 2 ||
	    zend_get_parameters_ex(2, &z_conn, &query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(conn, php_cyrus *, z_conn, -1, le_cyrus_name, le_cyrus);
	convert_to_string_ex(query);

	imclient_send(conn->client, NULL, NULL, Z_STRVAL_PP(query)); 

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool cyrus_close( resource connection)
   Close connection to a cyrus server */
PHP_FUNCTION(cyrus_close)
{
	zval      **z_conn;
	php_cyrus  *conn;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &z_conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(conn, php_cyrus *, z_conn, -1, le_cyrus_name, le_cyrus);

	zend_list_delete(Z_LVAL_PP(z_conn));

	RETURN_TRUE;
}
/* }}} */
#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
