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
   | Author: Onn Ben-Zvi <onn@zend.com>, onnb@mercury.co.il               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_fribidi.h"
#include "fribidi.h"

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_FRIBIDI

/* If you declare any globals in php_fribidi.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(fribidi)
*/

/* True global resources - no need for thread safety here */
/* static int le_fribidi; */

/* Every user visible function must have an entry in fribidi_functions[].
*/
function_entry fribidi_functions[] = {
	PHP_FE(fribidi_log2vis,	NULL)		
	{NULL, NULL, NULL}	/* Must be the last line in fribidi_functions[] */
};

zend_module_entry fribidi_module_entry = {
    STANDARD_MODULE_HEADER,
	"fribidi",
	fribidi_functions,
	PHP_MINIT(fribidi),
	PHP_MSHUTDOWN(fribidi),
	NULL,
	NULL,
	PHP_MINFO(fribidi),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FRIBIDI
ZEND_GET_MODULE(fribidi)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fribidi)
{
	REGISTER_LONG_CONSTANT("FRIBIDI_CHARSET_UTF8", FRIBIDI_CHARSET_UTF8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FRIBIDI_CHARSET_8859_6", FRIBIDI_CHARSET_ISO8859_6, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FRIBIDI_CHARSET_8859_8", FRIBIDI_CHARSET_ISO8859_8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FRIBIDI_CHARSET_CP1255", FRIBIDI_CHARSET_CP1255, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FRIBIDI_CHARSET_CP1256", FRIBIDI_CHARSET_CP1256, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FRIBIDI_CHARSET_ISIRI_3342", FRIBIDI_CHARSET_ISIRI_3342, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(fribidi)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fribidi)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "fribidi support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/*--------------------------------------------------------------*/
/* Name: fribidi_log2vis                                        */
/* Purpose: convert a logical string to a visual one            */
/* Input: 1) The logical string.                                */
/*        2) Base direction -                                   */
/*             Possible values:                                 */
/*                   a) "L" - base language is left to right.   */
/*                   b) "R" - base language is right  to left.  */
/*                   c) empty - base language is determined     */
/*                      automatically by the FriBiDi algorithm  */
/*        3) Character code being used -                        */
/*             Possible values (i.e., char sets supported)      */
/*              FRIBIDI_CHARSET_UTF8                            */
/*              FRIBIDI_CHARSET_8859_6                          */  
/*				FRIBIDI_CHARSET_8859_8                          */
/*				FRIBIDI_CHARSET_CP1255                          */
/*				FRIBIDI_CHARSET_CP1256                          */
/*				FRIBIDI_CHARSET_ISIRI_3342                      */
/*                                                              */
/* Output: on success: The visual string.                       */
/*         on failure:                                          */ 
/*--------------------------------------------------------------*/           

/* {{{ proto string fribidi_log2vis(string str, string direction, int charset)
   Convert a logical string to a visual one */
PHP_FUNCTION(fribidi_log2vis)
{
	zval **parameter1, **parameter2, **parameter3;


	FriBidiChar *u_logical_str, *u_visual_str;  /* unicode strings .... */
	char *inString;
	guchar *outString;
	int len, alloc_len, utf8_len;


	FriBidiCharType base_dir;

	guint16 *position_L_to_V_list;
	guint16 *position_V_to_L_list;
	guint8  *embedding_level_list;
				   
	/* get parameters from input */
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &parameter1, &parameter2, &parameter3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* convert input to expected type.... */

	convert_to_string_ex(parameter1);
	convert_to_string_ex(parameter2);
	convert_to_long_ex(parameter3);
	
	/* allocate space and prepare all local variables */


	len = Z_STRLEN_PP(parameter1);
	
	inString = estrndup(Z_STRVAL_PP(parameter1), len);

	alloc_len = len+1;

	u_logical_str = (FriBidiChar*) emalloc(sizeof(FriBidiChar)*alloc_len);
	u_visual_str = (FriBidiChar*) emalloc(sizeof(FriBidiChar)*alloc_len);
	
	position_L_to_V_list = (guint16*) emalloc(sizeof(guint16)*alloc_len);
	position_V_to_L_list = (guint16*) emalloc(sizeof(guint16)*alloc_len);
	embedding_level_list = (guint8*) emalloc(sizeof(guint8)*alloc_len);

	outString = (guchar*)emalloc(sizeof(guchar)*alloc_len);
	
	if(inString[len-1] == '\n') {
		inString[len-1] = '\0';
	}

	switch(Z_LVAL_PP(parameter3)) {
		case FRIBIDI_CHARSET_UTF8:
			utf8_len=fribidi_utf8_to_unicode(inString, u_logical_str);
			break;
		case FRIBIDI_CHARSET_ISO8859_6:
			fribidi_iso8859_6_to_unicode(inString, u_logical_str);
			break;
		case FRIBIDI_CHARSET_ISO8859_8:
			fribidi_iso8859_8_to_unicode(inString, u_logical_str);
			break;
		case FRIBIDI_CHARSET_CP1255:
			fribidi_cp1255_to_unicode(inString, u_logical_str);
			break;
		case FRIBIDI_CHARSET_CP1256:
			fribidi_cp1256_to_unicode(inString, u_logical_str);
			break;
		case FRIBIDI_CHARSET_ISIRI_3342:
			fribidi_isiri_3342_to_unicode(inString, u_logical_str);
			break;
		default:
			zend_error(E_ERROR,"unknown character set %d<br />", Z_LVAL_PP(parameter3));
	}
	
	
	/* visualize the logical.... */

	if ((Z_STRVAL_PP(parameter2))[0] == 'R') {
		base_dir = FRIBIDI_TYPE_RTL;
	} else if (Z_STRVAL_PP(parameter2)[0] == 'L')
		base_dir = FRIBIDI_TYPE_LTR;
	else
		base_dir = FRIBIDI_TYPE_N;

	fribidi_log2vis(u_logical_str, len, &base_dir, u_visual_str, position_L_to_V_list, position_V_to_L_list, embedding_level_list);
	
	/* convert back to original char set */

	switch(Z_LVAL_PP(parameter3)) {
		case FRIBIDI_CHARSET_UTF8:
			fribidi_unicode_to_utf8(u_visual_str, utf8_len , outString);
			break;
		case FRIBIDI_CHARSET_ISO8859_6:
			fribidi_unicode_to_iso8859_6(u_visual_str, len , outString);
			break;
		case FRIBIDI_CHARSET_ISO8859_8:
			fribidi_unicode_to_iso8859_8(u_visual_str, len , outString);
			break;
		case FRIBIDI_CHARSET_CP1255:
			fribidi_unicode_to_cp1255(u_visual_str, len , outString);
			break;
		case FRIBIDI_CHARSET_CP1256:
			fribidi_unicode_to_cp1256(u_visual_str, len , outString);
			break;
		case FRIBIDI_CHARSET_ISIRI_3342:
			fribidi_unicode_to_isiri_3342(u_visual_str, len , outString);
			break;
		default:
			zend_error(E_ERROR,"unknown character set %d<br />", Z_LVAL_PP(parameter3));
	}
	

	efree(u_logical_str);
	efree(u_visual_str);
	
	efree(position_L_to_V_list);
	efree(position_V_to_L_list);
	efree(embedding_level_list);
	
	RETURN_STRING(outString, 1);
}
/* }}} */


#endif	/* HAVE_FRIBIDI */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
