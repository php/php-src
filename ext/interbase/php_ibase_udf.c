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
*         CSTRING(xx),CSTRING(xx) RETURNS VARCHAR(4096)
*         ENTRY_POINT 'udf_call_php1' MODULE_NAME 'php_ibase_udf'
* 
*     DECLARE EXTERNAL FUNCTION CALL_PHP2
*         CSTRING(xx),CSTRING(xx),CSTRING(256) RETURNS VARCHAR(4096)
*         ENTRY_POINT 'udf_call_php1' MODULE_NAME 'php_ibase_udf'
* 
*     ... and so on.
* 
* The first input parameter contains the function you want to call. Subsequent
* arguments are passed to the called function. The lengths of the input strings can
* have any value >1. The length of the output is restricted to 4k.
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

#include "stdarg.h"

#ifdef ZTS
#error This functionality is not available in ZTS mode
#endif

/* VARCHAR result ignores first short, but must not be 0 */
static char result[4096] = { 1, 1 };

static void call_php(char *name, int argc, ...)
{
	zval callback, args[4], *argp[4], return_value;
	va_list va;
	int i;

	INIT_ZVAL(callback);
	ZVAL_STRING(&callback,name,0);

	/* check if the requested function exists */
	if (!zend_is_callable(&callback, 0, NULL)) {
		return;
	}

	va_start(va, argc);

	/* create the argument array */
	for (i = 0; i < argc; ++i) {
		char *arg = va_arg(va, char*);
		
		INIT_ZVAL(args[i]);
		argp[i] = &args[i];
		ZVAL_STRING(argp[i] = &args[i], arg, 0);
	}
	
	/* now call the function */
	if (FAILURE == call_user_function(EG(function_table), NULL,
			&callback, &return_value, argc, argp)) {
		return;
	}

	convert_to_string(&return_value);

	memcpy(&result[2], Z_STRVAL(return_value), Z_STRLEN(return_value)+1);
}

char *udf_call_php1(char *name, char *arg1)
{
	call_php(name, 1, arg1);
	return result;
}

char *udf_call_php2(char *name, char *arg1, char *arg2)
{
	call_php(name, 2, arg1, arg2);
	return result;
}

char *udf_call_php3(char *name, char *arg1, char *arg2, char *arg3)
{
	call_php(name, 3, arg1, arg2, arg3);
	return result;
}

char *udf_call_php4(char *name, char *arg1, char *arg2, char *arg3, char *arg4)
{
	call_php(name, 4, arg1, arg2, arg3, arg4);
	return result;
}
