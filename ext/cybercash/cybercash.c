/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Evan Klinger <evan715@sirius.com>                           |
   |          Timothy Whitfield <timothy@ametro.net>                      |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "cybercash.h"

#if HAVE_MCK
#include "mckcrypt.h"
#include "base64.h"

function_entry cybercash_functions[] = {
	PHP_FE(cybercash_encr, NULL)
	PHP_FE(cybercash_decr, NULL)
	PHP_FE(cybercash_base64_encode, NULL)
	PHP_FE(cybercash_base64_decode, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry cybercash_module_entry = {
	"cybercash",
	cybercash_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(cybercash),
	STANDARD_MODULE_PROPERTIES,
};

#ifdef COMPILE_DL_CYBERCASH
ZEND_GET_MODULE(cybercash)
#endif

PHP_MINFO_FUNCTION(cybercash)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Cybercash Support", "enabled");
	php_info_print_table_end();
}

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
		php_error(E_WARNING, "Return value from cybercash_encr could not be initialized");
		RETURN_FALSE;
	}

	add_assoc_long(return_value, "errcode", errcode);

	if (!errcode) {
		add_assoc_stringl(return_value, "outbuff", outbuff, outLth, 0);
		add_assoc_long(return_value,"outLth",outLth);
		add_assoc_stringl(return_value,"macbuff",macbuff,20,0);
	} else {
		efree(outbuff);
		efree(macbuff);
	}
}

PHP_FUNCTION(cybercash_decr)
{
	zval **wmk,**sk,**inbuff;
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
		php_error(E_WARNING, "Could not initialize Return value from cybercash_decr");
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
#endif
