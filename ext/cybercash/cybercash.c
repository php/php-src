/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Evan Klinger <evan715@sirius.com>                           |
   |          Timothy Whitfield <timothy@ametro.net>                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "cybercash.h"

#if HAVE_MCK
#include "mckcrypt.h"
#include "base64.h"

/* {{{ cybercash_functions[]
 */
function_entry cybercash_functions[] = {
	PHP_FE(cybercash_encr, NULL)
	PHP_FE(cybercash_decr, NULL)
	PHP_FE(cybercash_base64_encode, NULL)
	PHP_FE(cybercash_base64_decode, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ cybercash_module_entry
 */
zend_module_entry cybercash_module_entry = {
    STANDARD_MODULE_HEADER,
	"cybercash",
	cybercash_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(cybercash),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES,
};
/* }}} */

#ifdef COMPILE_DL_CYBERCASH
ZEND_GET_MODULE(cybercash)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cybercash)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Cybercash Support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto array cybercash_encr(string wmk, string sk, string data)
   Cybercash encrypt */
PHP_FUNCTION(cybercash_encr)
{
	zval **wmk, **sk, **inbuff;
	unsigned char *outbuff, *macbuff;
	unsigned int outAlloc, outLth;
	long errcode;

	if (ZEND_NUM_ARGS() != 3 || 
	    zend_get_parameters_ex(3, &wmk, &sk, &inbuff) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(wmk);
	convert_to_string_ex(sk);
	convert_to_string_ex(inbuff);

	outAlloc = Z_STRLEN_PP(inbuff) + 10;

	outbuff = (unsigned char *)emalloc(sizeof(unsigned char) * outAlloc);
	macbuff = (unsigned char *)emalloc(sizeof(unsigned char) * 20);

	errcode = mck_encr(Z_STRVAL_PP(wmk),
	                   Z_STRVAL_PP(sk),
	                   Z_STRLEN_PP(inbuff) + 1,
	                   Z_STRVAL_PP(inbuff),
	                   outAlloc,
	                   outbuff,
	                   &outLth,
	                   macbuff);

	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "%s(): Return value could not be initialized", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	add_assoc_long(return_value, "errcode", errcode);

	if (!errcode) {
		add_assoc_stringl(return_value, "outbuff", outbuff, outLth, 0);
		add_assoc_long(return_value,"outLth", outLth);
		add_assoc_stringl(return_value,"macbuff", macbuff, 20, 0);
	} else {
		efree(outbuff);
		efree(macbuff);
	}
}
/* }}} */

/* {{{ proto array cybercash_decr(string wmp, string sk, string data)
   Cybercash decrypt */
PHP_FUNCTION(cybercash_decr)
{
	zval **wmk, **sk, **inbuff;
	unsigned char *outbuff, *macbuff;
	unsigned int outAlloc, outLth;
	long errcode;
  

	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3, &wmk, &sk, &inbuff) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(wmk);
	convert_to_string_ex(sk);
	convert_to_string_ex(inbuff);

	outAlloc = Z_STRLEN_PP(inbuff);

	outbuff = (unsigned char *)emalloc(sizeof(unsigned char) * outAlloc);
	macbuff = (unsigned char *)emalloc(sizeof(unsigned char) * 20);

	errcode = mck_decr(Z_STRVAL_PP(wmk),
	                   Z_STRVAL_PP(sk),
	                   Z_STRLEN_PP(inbuff),
	                   Z_STRVAL_PP(inbuff),
	                   outAlloc,
	                   outbuff,
	                   &outLth,
	                   macbuff);

	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "%s(): Could not initialize return value", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	add_assoc_long(return_value, "errcode", errcode);

	if (!errcode) {
		add_assoc_stringl(return_value, "outbuff", outbuff, outLth, 0);
		add_assoc_long(return_value, "outLth", outLth);
		add_assoc_stringl(return_value, "macbuff", macbuff, 20, 0);
	} else {
		efree(outbuff);
		efree(macbuff);
	}
}
/* }}} */

/* {{{ proto string cybercash_base64_encode(string data)
   base64 encode data for cybercash */
PHP_FUNCTION(cybercash_base64_encode)
{
	zval **inbuff;
	char *outbuff;
	long ret_length;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &inbuff) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(inbuff);

	outbuff = (char *)emalloc(base64_enc_size((unsigned int)Z_STRLEN_PP(inbuff)));
  
	ret_length = base64_encode(outbuff, 
	                           Z_STRVAL_PP(inbuff),
	                           Z_STRLEN_PP(inbuff));

	RETURN_STRINGL(outbuff, ret_length, 0);
}
/* }}} */

/* {{{ proto string cybercash_base64_decode(string data)
   base64 decode data for cybercash */
PHP_FUNCTION(cybercash_base64_decode)
{
	zval **inbuff;
	char *outbuff;
	long ret_length;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &inbuff) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(inbuff);

	outbuff = (char *)emalloc(base64_dec_size((unsigned int)Z_STRLEN_PP(inbuff)));
  
	ret_length = base64_decode(outbuff, Z_STRVAL_PP(inbuff), Z_STRLEN_PP(inbuff));

	RETURN_STRINGL(outbuff, ret_length, 0);
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
