/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Ard Biesheuvel <a.k.biesheuvel@ewi.tudelft.nl>               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/**
* This UDF library adds the ability to call PHP functions from your SQL
* statement. You will have to declare a different external function for 
* each number of parameters you require. Currently, only string arguments
* are supported as both input arguments as well as the result.
* 
* Declare the functions like this:
* 
*     DECLARE EXTERNAL FUNCTION CALL_PHP1
*         CSTRING(xx),<type> BY DESCRIPTOR, CSTRING(xx)
*         RETURNS PARAMETER 2
*         ENTRY_POINT 'udf_call_php1' MODULE_NAME 'php_ibase_udf'
* 
*     DECLARE EXTERNAL FUNCTION CALL_PHP2
*         CSTRING(xx),<type> BY DESCRIPTOR, CSTRING(xx),CSTRING(xx) 
*         RETURNS PARAMETER 2
*         ENTRY_POINT 'udf_call_php2' MODULE_NAME 'php_ibase_udf'
* 
*     ... and so on.
* 
* The first input parameter contains the function you want to call. The second
* argument is the result, and should not be passed as an argument. Subsequent
* arguments are passed to the called function. The lengths of the input strings can
* have any value >1. The type and length of the output depends on its declaration.
* 
* The declared functions can be called from SQL like:
* 
*     SELECT CALL_PHP1('strtolower',rdb$relation_name) FROM rdb$relations
* 
* THIS UDF WILL ONLY WORK IF YOU ARE RUNNING THE EMBEDDED 'CLASSIC' SERVER
* IN-PROCESS WITH YOUR PHP ENGINE. NETWORK CONNECTIONS TO 'LOCALHOST' ARE 
* NOT SUPPORTED. 
* 
* Compile with:
*     gcc -shared `php-config --includes` `php-config --ldflags` -o php_ibase_udf.so php_ibase_udf.c
* 
* and copy the resulting file to the folder where your database expects to find it.
*/

#include "zend.h"
#include "zend_API.h"
#include "php.h"

#include "stdarg.h"
#include "ibase.h"

#ifdef ZTS
#error This functionality is not available in ZTS mode
#endif

#define min(a,b) ((a)<(b)?(a):(b))

static PARAMDSC *call_php(char *name, PARAMDSC *r, int argc, ...)
{
	zval callback, args[4], *argp[4], return_value;
	va_list va;
	int i;
	PARAMVARY *res = (PARAMVARY*)(r->dsc_address);

	INIT_ZVAL(callback);
	ZVAL_STRING(&callback,name,0);

	do {
		/* check if the requested function exists */
		if (!zend_is_callable(&callback, 0, NULL)) {
			break;
		}
	
		va_start(va, argc);
	
		/* create the argument array */
		for (i = 0; i < argc; ++i) {
			char *arg = va_arg(va, char*);
			
			INIT_ZVAL(args[i]);
			ZVAL_STRING(argp[i] = &args[i], arg, 0);
		}

		va_end(va);
		
		/* now call the function */
		if (FAILURE == call_user_function(EG(function_table), NULL,
				&callback, &return_value, argc, argp)) {
			break;
		}
	
		switch (r->dsc_dtype) {

			case dtype_cstring:
				convert_to_string(&return_value);
				memcpy(r->dsc_address, Z_STRVAL(return_value), min(r->dsc_length,Z_STRLEN(return_value)));
				r->dsc_length = min(r->dsc_length,Z_STRLEN(return_value));
				r->dsc_address[r->dsc_length] = 0;
				break;

			case dtype_text:
				convert_to_string(&return_value);
				memcpy(r->dsc_address, Z_STRVAL(return_value), min(r->dsc_length,Z_STRLEN(return_value)));
				r->dsc_length = min(r->dsc_length,Z_STRLEN(return_value));
				break;

			case dtype_varying:
				convert_to_string(&return_value);
				memcpy(res->vary_string, Z_STRVAL(return_value), min(r->dsc_length-2,Z_STRLEN(return_value)));
				res->vary_length = min(r->dsc_length-2,Z_STRLEN(return_value));
				r->dsc_length = res->vary_length+2;
				break;

			case dtype_short:
				convert_to_long(&return_value);
				*(short*)r->dsc_address = Z_LVAL(return_value);
				break;

			case dtype_long:
				convert_to_long(&return_value);
				*(ISC_LONG*)r->dsc_address = Z_LVAL(return_value);
				break;

			case dtype_int64:
				convert_to_long(&return_value);
				*(ISC_INT64*)r->dsc_address = Z_LVAL(return_value);
				break;

		}
				
	
		zval_dtor(&return_value);

		return;

	} while (0);
	
	/**
	* If we end up here, we should report an error back to the DB engine, but
	* that's not possible. We can however report it back to PHP.
	*/
	php_error_docref(NULL, E_WARNING, "Error calling function '%s' from database", name);
}

PARAMDSC *udf_call_php1(char *name, PARAMDSC *r, char *arg1)
{
	return call_php(name, r, 1, arg1);
}

PARAMDSC *udf_call_php2(char *name, PARAMDSC *r, char *arg1, char *arg2)
{
	return call_php(name, r, 2, arg1, arg2);
}

PARAMDSC *udf_call_php3(char *name, PARAMDSC *r, char *arg1, char *arg2, char *arg3)
{
	return call_php(name, r, 3, arg1, arg2, arg3);
}

PARAMDSC *udf_call_php4(char *name, PARAMDSC *r, char *arg1, char *arg2, char *arg3, char *arg4)
{
	return call_php(name, r, 4, arg1, arg2, arg3, arg4);
}

