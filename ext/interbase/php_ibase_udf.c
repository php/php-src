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
* This UDF library adds the ability to call PHP functions from SQL
* statements. Because of SQL's strong typing, you will have to declare
* an external function for every combination of input and output parameters 
* your application requires. The types of the input arguments and the result
* type can be either [VAR]CHARs, (un)scaled integers or doubles/floats.
* 
* Declare the functions like this:
* 
*     DECLARE EXTERNAL FUNCTION CALL_PHP1
*         CSTRING(xx),
*         <return type> BY DESCRIPTOR,
*         <arg type> BY DESCRIPTOR
*         RETURNS PARAMETER 2
*         ENTRY_POINT 'udf_call_php1' MODULE_NAME 'php_ibase_udf'
* 
*     DECLARE EXTERNAL FUNCTION CALL_PHP2
*         CSTRING(xx),
*         <return type> BY DESCRIPTOR, 
*         <arg type> BY DESCRIPTOR,
*         <arg type> BY DESCRIPTOR
*         RETURNS PARAMETER 2
*         ENTRY_POINT 'udf_call_php2' MODULE_NAME 'php_ibase_udf'
* 
*     ... and so on. [for up to 8 input arguments]
* 
* The first input parameter contains the function you want to call. The
* second argument is the result, and should not be passed as an argument.
* [the DB will do this for you] Subsequent arguments are passed to the
* function in argument 1. 
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
*     gcc -shared `php-config --includes` `php-config --ldflags` \
*         -o php_ibase_udf.so php_ibase_udf.c
* 
* and copy the resulting file to the folder where your database expects
* to find its UDFs.
*/

#include "zend.h"
#include "zend_API.h"
#include "php.h"

#include "ibase.h"

#define min(a,b) ((a)<(b)?(a):(b))

#ifdef PHP_WIN32
#define LL_LIT(lit) lit ## I64
#else
#define LL_LIT(lit) lit ## ll
#endif

static ISC_INT64 const scales[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 100000000, 1000000000,
	1000000000, LL_LIT(10000000000),LL_LIT(100000000000),LL_LIT(10000000000000),LL_LIT(100000000000000),
	LL_LIT(1000000000000000),LL_LIT(1000000000000000),LL_LIT(1000000000000000000) };

static void call_php(char *name, PARAMDSC *r, int argc, PARAMDSC **argv)
{

	do {
		zval callback, args[4], *argp[4], return_value;
		PARAMVARY *res = (PARAMVARY*)r->dsc_address;
		int i;

		INIT_ZVAL(callback);
		ZVAL_STRING(&callback,name,0);

		/* check if the requested function exists */
		if (!zend_is_callable(&callback, 0, NULL)) {
			break;
		}
	
		/* create the argument array */
		for (i = 0; i < argc; ++i) {

			INIT_ZVAL(args[i]);
			argp[i] = &args[i];
			
			/* test arg for null */
			if (argv[i]->dsc_flags & DSC_null) {
				ZVAL_NULL(argp[i]);
				continue;
			}

			switch (argv[i]->dsc_dtype) {
				ISC_INT64 l;

				case dtype_cstring:
					ZVAL_STRING(argp[i], (char*)argv[i]->dsc_address,0);
					break;

				case dtype_text:
					ZVAL_STRINGL(argp[i], (char*)argv[i]->dsc_address, argv[i]->dsc_length,0);
					break;

				case dtype_varying:
					ZVAL_STRINGL(argp[i], ((PARAMVARY*)argv[i]->dsc_address)->vary_string,
						((PARAMVARY*)argv[i]->dsc_address)->vary_length,0);
					break;

				case dtype_short:
					if (argv[i]->dsc_scale == 0) {
						ZVAL_LONG(argp[i], *(short*)argv[i]->dsc_address);
					} else {
						ZVAL_DOUBLE(argp[i],
							((double)*(short*)argv[i]->dsc_address)/scales[-argv[i]->dsc_scale]);
					}
					break;

				case dtype_long:
					if (argv[i]->dsc_scale == 0) {
						ZVAL_LONG(argp[i], *(ISC_LONG*)argv[i]->dsc_address);
					} else {
						ZVAL_DOUBLE(argp[i],
							((double)*(ISC_LONG*)argv[i]->dsc_address)/scales[-argv[i]->dsc_scale]);
					}
					break;

				case dtype_int64:
					l = *(ISC_INT64*)argv[i]->dsc_address;

					if (argv[i]->dsc_scale == 0 && l <= LONG_MAX && l >= LONG_MIN) {
						ZVAL_LONG(argp[i], (long)l);
					} else {
						ZVAL_DOUBLE(argp[i], ((double)l)/scales[-argv[i]->dsc_scale]);
					}
					break;

				case dtype_real:
					ZVAL_DOUBLE(argp[i], *(float*)argv[i]->dsc_address);
					break;

				case dtype_double:
					ZVAL_DOUBLE(argp[i], *(double*)argv[i]->dsc_address);
					break;
					
			}
		}
		
		/* now call the function */
		if (FAILURE == call_user_function(EG(function_table), NULL,
				&callback, &return_value, argc, argp)) {
			break;
		}
	
		/* return whatever type we got back from the callback: let DB handle conversion */
		switch (Z_TYPE(return_value)) {

			case IS_LONG:
				r->dsc_dtype = dtype_long;
				*(long*)r->dsc_address = Z_LVAL(return_value);
				r->dsc_length = sizeof(long);
				break;

			case IS_DOUBLE:
				r->dsc_dtype = dtype_double;
				*(double*)r->dsc_address = Z_DVAL(return_value);
				r->dsc_length = sizeof(double);
				break;

			case IS_NULL:
				r->dsc_flags |= DSC_null;
				break;

			default:
				convert_to_string(&return_value);

			case IS_STRING:
				r->dsc_dtype = dtype_varying;
				memcpy(res->vary_string, Z_STRVAL(return_value),
					(res->vary_length = min(r->dsc_length-2,Z_STRLEN(return_value))));
				r->dsc_length = res->vary_length+2;
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


/* Entry points for the DB engine */

void udf_call_php1(char *name, PARAMDSC *r, PARAMDSC *arg1)
{
	PARAMDSC *args[1] = { arg1 };
	call_php(name, r, 1, args);
}

void udf_call_php2(char *name, PARAMDSC *r, PARAMDSC *arg1, PARAMDSC *arg2)
{
	PARAMDSC *args[2] = { arg1, arg2 };
	call_php(name, r, 2, args);
}

void udf_call_php3(char *name, PARAMDSC *r, PARAMDSC *arg1, PARAMDSC *arg2, PARAMDSC *arg3)
{
	PARAMDSC *args[3] = { arg1, arg2, arg3 };
	call_php(name, r, 3, args);
}

void udf_call_php4(char *name, PARAMDSC *r, PARAMDSC *arg1, PARAMDSC *arg2, PARAMDSC *arg3, 
	PARAMDSC *arg4)
{
	PARAMDSC *args[4] = { arg1, arg2, arg3, arg4 };
	call_php(name, r, 4, args);
}

void udf_call_php5(char *name, PARAMDSC *r, PARAMDSC *arg1, PARAMDSC *arg2, PARAMDSC *arg3, 
	PARAMDSC *arg4, PARAMDSC *arg5)
{
	PARAMDSC *args[5] = { arg1, arg2, arg3, arg4, arg5 };
	call_php(name, r, 5, args);
}

void udf_call_php6(char *name, PARAMDSC *r, PARAMDSC *arg1, PARAMDSC *arg2, PARAMDSC *arg3, 
	PARAMDSC *arg4, PARAMDSC *arg5, PARAMDSC *arg6)
{
	PARAMDSC *args[6] = { arg1, arg2, arg3, arg4, arg5, arg6 };
	call_php(name, r, 6, args);
}

void udf_call_php7(char *name, PARAMDSC *r, PARAMDSC *arg1, PARAMDSC *arg2, PARAMDSC *arg3, 
	PARAMDSC *arg4, PARAMDSC *arg5, PARAMDSC *arg6, PARAMDSC *arg7)
{
	PARAMDSC *args[7] = { arg1, arg2, arg3, arg4, arg5, arg6, arg7 };
	call_php(name, r, 7, args);
}

void udf_call_php8(char *name, PARAMDSC *r, PARAMDSC *arg1, PARAMDSC *arg2, PARAMDSC *arg3, 
	PARAMDSC *arg4, PARAMDSC *arg5, PARAMDSC *arg6, PARAMDSC *arg7, PARAMDSC *arg8)
{
	PARAMDSC *args[8] = { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
	call_php(name, r, 8, args);
}

