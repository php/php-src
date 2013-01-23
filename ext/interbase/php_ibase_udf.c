/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
* an external function for every combination { output type, #args } that
* your application requires. 
* 
* Declare the functions like this:
* 
*     DECLARE EXTERNAL FUNCTION CALL_PHP1
*         CSTRING(xx),
*         <return type> BY DESCRIPTOR,
*         INTEGER BY DESCRIPTOR
*         RETURNS PARAMETER 2
*         ENTRY_POINT 'udf_call_php1' MODULE_NAME 'php_ibase_udf'
* 
*     DECLARE EXTERNAL FUNCTION CALL_PHP2
*         CSTRING(xx),
*         <return type> BY DESCRIPTOR, 
*         INTEGER BY DESCRIPTOR,
*         INTEGER BY DESCRIPTOR
*         RETURNS PARAMETER 2
*         ENTRY_POINT 'udf_call_php2' MODULE_NAME 'php_ibase_udf'
* 
*     ... and so on. [for up to 8 input arguments]
* 
* The first input parameter contains the name of the PHP function you want
* to call. The second argument is the result. (omit this argument when calling
* the function) The return type of the function is the declared type of the
* result. The value returned from the PHP function being called will
* automatically be converted if necessary.
* The arguments should have their types declared as well, but you're free
* to pass arguments of other types instead. They will be converted to the
* best matching PHP type before being passed to the PHP function.
* 
* The declared functions can be called from SQL like:
* 
*     SELECT * FROM <table> WHERE CALL_PHP1('soundex',<field>) NOT LIKE ?
* or
*     UPDATE <table> SET <field> = CALL_PHP1('ucwords',<field>)
* 
* Additionally, there's a function 'exec_php' which allows the contents
* of text BLOB fields to be parsed and executed by PHP. This is most useful
* for declaring functions that can then be called with CALL_PHPx.
*
*    DECLARE EXTERNAL FUNCTION EXEC_PHP
*        BLOB,
*        INTEGER BY DESCRIPTOR,
*        SMALLINT
*        RETURNS PARAMETER 2
*        ENTRY_POINT 'exec_php' MODULE_NAME 'php_ibase_udf'
*
* The function will return 1 if execution succeeded and 0 if an error
* occurred. The result that is returned from the executed PHP code is
* ignored. You can pass a non-zero value as second argument to force
* the embedded PHP engine to re-initialise.
*
* There are several ways to build this library, depending on which way the
* database is accessed. If you're using the classic server on a local
* connection, you should compile the library like this:
*
*     gcc -shared `php-config --includes` `php-config --ldflags` \
*         `php-config --libs` -o php_ibase_udf.so php_ibase_udf.c
*
* If you connect to the classic server by TCP/IP, you should build the
* PHP embedded static library and link against that. 
*
*     gcc -shared `php-config --includes` `php-config --ldflags` \
*         `php-config --libs` -o php_ibase_udf.so php_ibase_udf.c \
*         /usr/lib/libphp5.a
*
* If you use the super server, you should also link against the embedded
* library, but be sure to enable thread safety, as the super server is
* multi-threaded. After building, copy the resulting file to the folder
* where your database expects to find its UDFs.
*/

#include "zend.h"
#include "zend_API.h"

#include "php.h"
#include "php_ini.h"

#include "ibase.h"

#define min(a,b) ((a)<(b)?(a):(b))

#ifdef PHP_WIN32
#define LL_LIT(lit) lit ## I64
#else
#define LL_LIT(lit) lit ## ll
#endif

#ifdef ZTS
#  include <pthread.h>

static void ***tsrm_ls;
pthread_mutex_t mtx_res = PTHREAD_MUTEX_INITIALIZER;

#define LOCK() do { pthread_mutex_lock(&mtx_res); } while (0)
#define UNLOCK() do { pthread_mutex_unlock(&mtx_res); } while (0)
#else
#define LOCK()
#define UNLOCK()
#endif

#ifdef PHP_EMBED
# include "php_main.h"
# include "sapi/embed/php_embed.h"

static void __attribute__((constructor)) init()
{
	php_embed_init(0, NULL PTSRMLS_CC);
}

static void __attribute__((destructor)) fini()
{
	php_embed_shutdown(TSRMLS_C);
}

#endif

/**
* Gets the contents of the BLOB b and offers it to Zend for parsing/execution
*/
void exec_php(BLOBCALLBACK b, PARAMDSC *res, ISC_SHORT *init)
{
	int result, remaining = b->blob_total_length, i = 0;
	char *code = pemalloc(remaining+1, 1);
	ISC_USHORT read;

	for (code[remaining] = '\0'; remaining > 0; remaining -= read)
		b->blob_get_segment(b->blob_handle, &code[i++<<16],min(0x10000,remaining), &read); 

	LOCK();

	switch (init && *init) {

		default:
#ifdef PHP_EMBED
			php_request_shutdown(NULL);
			if (FAILURE == (result = php_request_startup(TSRMLS_C))) {
				break;
			}
		case 0:
#endif
			/* feed it to the parser */
			zend_first_try {
				result = zend_eval_stringl(code, b->blob_total_length, NULL, "Firebird Embedded PHP engine" TSRMLS_CC);
			} zend_end_try();
	}
	
	UNLOCK();

	free(code);

	res->dsc_dtype = dtype_long;
	*(ISC_LONG*)res->dsc_address = (result == SUCCESS);
}

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

		LOCK();
		
		/* check if the requested function exists */
		if (!zend_is_callable(&callback, 0, NULL TSRMLS_CC)) {
			break;
		}
		
		UNLOCK();
	
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
				struct tm t;
				char const *fmt;
				char d[64];

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

				case dtype_sql_date:
					isc_decode_sql_date((ISC_DATE*)argv[i]->dsc_address, &t);
					ZVAL_STRINGL(argp[i], d, strftime(d, sizeof(d), INI_STR("ibase.dateformat"), &t),1); 
					break;

				case dtype_sql_time:
					isc_decode_sql_time((ISC_TIME*)argv[i]->dsc_address, &t);
					ZVAL_STRINGL(argp[i], d, strftime(d, sizeof(d), INI_STR("ibase.timeformat"), &t),1); 
					break;

				case dtype_timestamp:
					isc_decode_timestamp((ISC_TIMESTAMP*)argv[i]->dsc_address, &t);
					ZVAL_STRINGL(argp[i], d, strftime(d, sizeof(d), INI_STR("ibase.timestampformat"), &t),1); 
					break;
			}
		}

		LOCK();

		/* now call the function */
		if (FAILURE == call_user_function(EG(function_table), NULL,
				&callback, &return_value, argc, argp TSRMLS_CC)) {
			UNLOCK();
			break;
		}
		
		UNLOCK();

		for (i = 0; i < argc; ++i) {
			switch (argv[i]->dsc_dtype) {
				case dtype_sql_date:
				case dtype_sql_time:
				case dtype_timestamp:
					zval_dtor(argp[i]);
					
			}
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
	LOCK();
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error calling function '%s' from database", name);
	UNLOCK();
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

