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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_streams.h"
#include "php_main.h"
#include "php_globals.h"
#include "php_ini.h"
#include "internal_functions_registry.h"
#include "php_standard.h"
#include "php_math.h"
#include "php_incomplete_class.h"
#include "ext/standard/info.h"
#include "ext/session/php_session.h"
#include "zend_operators.h"
#include "ext/standard/dns.h"

#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#ifndef NETWARE
#include <netdb.h>
#else
/*#include "netware/env.h"*/    /* Temporary */
#ifdef NEW_LIBC /* Same headers hold good for Winsock and Berkeley sockets */
#include <netinet/in.h>
/*#include <arpa/inet.h>*/
#include <netdb.h>
#else
#include <sys/socket.h>
#endif
#endif

#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if HAVE_LOCALE_H
# include <locale.h>
#endif

#if HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif

#ifdef HARTMUT_0
#include <getopt.h>
#endif

#include "safe_mode.h"

#ifdef PHP_WIN32
# include "win32/unistd.h"
#endif

#include "zend_globals.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_ticks.h"


#ifdef ZTS
int basic_globals_id;
#else
php_basic_globals basic_globals;
#endif


#include "php_fopen_wrappers.h"

static unsigned char first_and_second__args_force_ref[] = { 2, BYREF_FORCE, BYREF_FORCE };
static unsigned char second_and_third_args_force_ref[] = { 3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
static unsigned char second_args_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
static unsigned char third_and_fourth_args_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
static unsigned char third_and_rest_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE_REST };
static unsigned char first_through_third_args_force_ref[] =
{3, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE};


typedef struct _php_shutdown_function_entry {
	zval **arguments;
	int arg_count;
} php_shutdown_function_entry;

typedef struct _user_tick_function_entry {
	zval **arguments;
	int arg_count;
	int calling;
} user_tick_function_entry;

/* some prototypes for local functions */
static void user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry);
static void user_tick_function_dtor(user_tick_function_entry *tick_function_entry);

/* Demo code. Enable only if you need this. */
#define ENABLE_TEST_CLASS 0

#if ENABLE_TEST_CLASS
void test_class_startup(void);
pval test_class_get_property(zend_property_reference *property_reference);
int test_class_set_property(zend_property_reference *property_reference, pval *value);
void test_class_call_function(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);

pval test_class_get_property(zend_property_reference *property_reference)
{
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Reading a property from a OverloadedTestClass object:\n");

	for (element = property_reference->elements_list->head; element; element = element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
	
		switch (Z_TYPE_P(overloaded_property)) {
	
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;

			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
	
		switch (Z_TYPE(overloaded_property->element)) {
	
			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;

			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	Z_STRVAL(result) = estrndup("testing", 7);
	Z_STRLEN(result) = 7;
	Z_TYPE(result) = IS_STRING;
	return result;
}

int test_class_set_property(zend_property_reference *property_reference, pval * value)
{
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Writing to a property from a OverloadedTestClass object:\n");
	printf("Writing '");
	zend_print_variable(value);
	printf("'\n");

	for (element = property_reference->elements_list->head; element; element = element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		
		switch (Z_TYPE_P(overloaded_property)) {
	
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
	
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}

		switch (Z_TYPE(overloaded_property->element)) {

			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
	
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}
	return 0;
}

void test_class_call_function(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Invoking a method on OverloadedTestClass object:\n");

	for (element = property_reference->elements_list->head; element; element = element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;

		switch (Z_TYPE_P(overloaded_property)) {
	
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
	
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
	
			case OE_IS_METHOD:
				printf("Overloaded method:  ");
		}

		switch (Z_TYPE(overloaded_property->element)) {

			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
	
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	printf("%d arguments\n", ZEND_NUM_ARGS());
	RETVAL_STRING("testing", 1);
}

void test_class_startup(void)
{
	zend_class_entry test_class_entry;
	TSRMLS_FETCH();

	INIT_OVERLOADED_CLASS_ENTRY(test_class_entry,
								"OverloadedTestClass",
								NULL, test_class_call_function,
								test_class_get_property,
								test_class_set_property);

	zend_register_internal_class(&test_class_entry TSRMLS_CC);
}
#endif


function_entry basic_functions[] = {
	PHP_FE(constant,														NULL)
	PHP_FE(bin2hex,															NULL)
	PHP_FE(sleep,															NULL)
	PHP_FE(usleep,															NULL)
	PHP_FE(time,															NULL)
	PHP_FE(mktime,															NULL)
	PHP_FE(gmmktime,														NULL)

#if HAVE_STRFTIME
	PHP_FE(strftime,														NULL)
	PHP_FE(gmstrftime,														NULL)
#endif

	PHP_FE(strtotime,														NULL)
	PHP_FE(date,															NULL)
	PHP_FE(idate,															NULL)
	PHP_FE(gmdate,															NULL)
	PHP_FE(getdate,															NULL)
	PHP_FE(localtime,														NULL)
	PHP_FE(checkdate,														NULL)

	PHP_FE(flush,															NULL)
	PHP_FE(wordwrap,														NULL)
	PHP_FE(htmlspecialchars,												NULL)
	PHP_FE(htmlentities,													NULL)
	PHP_FE(html_entity_decode,												NULL)
	PHP_FE(get_html_translation_table,										NULL)
	PHP_FE(sha1,															NULL)
	PHP_FE(sha1_file,														NULL)
	PHP_NAMED_FE(md5,php_if_md5,											NULL)
	PHP_NAMED_FE(md5_file,php_if_md5_file,									NULL)
	PHP_NAMED_FE(crc32,php_if_crc32,										NULL)

	PHP_FE(iptcparse,														NULL)															
	PHP_FE(iptcembed,														NULL)
	PHP_FE(getimagesize,			second_args_force_ref)
	PHP_FE(image_type_to_mime_type,											NULL)

	PHP_FE(phpinfo,															NULL)
	PHP_FE(phpversion,														NULL)
	PHP_FE(phpcredits,														NULL)
	PHP_FE(php_logo_guid,													NULL)
	PHP_FE(zend_logo_guid,													NULL)
	PHP_FE(php_sapi_name,													NULL)
	PHP_FE(php_uname,														NULL)
	PHP_FE(php_ini_scanned_files,											NULL)

	PHP_FE(strnatcmp,														NULL)
	PHP_FE(strnatcasecmp,													NULL)
	PHP_FE(substr_count,													NULL)
	PHP_FE(strspn,															NULL)
	PHP_FE(strcspn,															NULL)
	PHP_FE(strtok,															NULL)
	PHP_FE(strtoupper,														NULL)
	PHP_FE(strtolower,														NULL)
	PHP_FE(strpos,															NULL)
	PHP_FE(stripos,															NULL)
	PHP_FE(strrpos,															NULL)
	PHP_FE(strripos,														NULL)
	PHP_FE(strrev,															NULL)
	PHP_FE(hebrev,															NULL)
	PHP_FE(hebrevc,															NULL)
	PHP_FE(nl2br,															NULL)
	PHP_FE(basename,														NULL)
	PHP_FE(dirname,															NULL)
	PHP_FE(pathinfo,														NULL)
	PHP_FE(stripslashes,													NULL)
	PHP_FE(stripcslashes,													NULL)
	PHP_FE(strstr,															NULL)
	PHP_FE(stristr,															NULL)
	PHP_FE(strrchr,															NULL)
	PHP_FE(str_shuffle,															NULL)
	PHP_FE(str_word_count,														NULL)
	PHP_FE(str_split,														NULL)
	PHP_FE(strpbrk,															NULL)

#ifdef HAVE_STRCOLL
	PHP_FE(strcoll,															NULL)
#endif

#ifdef HAVE_STRFMON
	PHP_FE(money_format,                                                    NULL)
#endif

	PHP_FE(substr,															NULL)
	PHP_FE(substr_replace,													NULL)
	PHP_FE(quotemeta,														NULL)
	PHP_FE(ucfirst,															NULL)
	PHP_FE(ucwords,															NULL)
	PHP_FE(strtr,															NULL)
	PHP_FE(addslashes,														NULL)
	PHP_FE(addcslashes,														NULL)
	PHP_FE(rtrim,															NULL)
	PHP_FE(str_replace,														NULL)
	PHP_FE(str_ireplace,													NULL)
	PHP_FE(str_repeat,														NULL)
	PHP_FE(count_chars,														NULL)
	PHP_FE(chunk_split,														NULL)
	PHP_FE(trim,															NULL)
	PHP_FE(ltrim,															NULL)
	PHP_FE(strip_tags,														NULL)
	PHP_FE(similar_text,			third_arg_force_ref)
	PHP_FE(explode,															NULL)
	PHP_FE(implode,															NULL)
	PHP_FE(setlocale,														NULL)
	PHP_FE(localeconv,														NULL)

#if HAVE_NL_LANGINFO
	PHP_FE(nl_langinfo,														NULL)
#endif

	PHP_FE(soundex,															NULL)
	PHP_FE(levenshtein,														NULL)
	PHP_FE(chr,																NULL)
	PHP_FE(ord,																NULL)
	PHP_FE(parse_str,				second_arg_force_ref)
	PHP_FE(str_pad,															NULL)
	PHP_FALIAS(chop,				rtrim,									NULL)
	PHP_FALIAS(strchr,				strstr,									NULL)
	PHP_NAMED_FE(sprintf,			PHP_FN(user_sprintf),					NULL)
	PHP_NAMED_FE(printf,			PHP_FN(user_printf),					NULL)
	PHP_FE(vprintf,															NULL)
	PHP_FE(vsprintf,														NULL)
	PHP_FE(fprintf,															NULL)
	PHP_FE(vfprintf,														NULL)
	PHP_FE(sscanf,					third_and_rest_force_ref)
	PHP_FE(fscanf,					third_and_rest_force_ref)
	PHP_FE(parse_url,														NULL)
	PHP_FE(urlencode,														NULL)
	PHP_FE(urldecode,														NULL)
	PHP_FE(rawurlencode,													NULL)
	PHP_FE(rawurldecode,													NULL)

#ifdef HAVE_SYMLINK
	PHP_FE(readlink,														NULL)
	PHP_FE(linkinfo,														NULL)
	PHP_FE(symlink,															NULL)
	PHP_FE(link,															NULL)
#endif

	PHP_FE(unlink,															NULL)
	PHP_FE(exec,					second_and_third_args_force_ref)
	PHP_FE(system,					second_arg_force_ref)
	PHP_FE(escapeshellcmd,													NULL)
	PHP_FE(escapeshellarg,													NULL)
	PHP_FE(passthru,				second_arg_force_ref)
	PHP_FE(shell_exec,														NULL)
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_FE(proc_open,				third_arg_force_ref)
	PHP_FE(proc_close,														NULL)
	PHP_FE(proc_terminate,													NULL)
	PHP_FE(proc_get_status,													NULL)
#endif

#ifdef HAVE_NICE
	PHP_FE(proc_nice,														NULL)	
#endif

	PHP_FE(rand,															NULL)
	PHP_FE(srand,															NULL)
	PHP_FE(getrandmax,														NULL)
	PHP_FE(mt_rand,															NULL)
	PHP_FE(mt_srand,														NULL)
	PHP_FE(mt_getrandmax,													NULL)

#if HAVE_GETSERVBYNAME
	PHP_FE(getservbyname,													NULL)
#endif

#if HAVE_GETSERVBYPORT
	PHP_FE(getservbyport,													NULL)
#endif

#if HAVE_GETPROTOBYNAME
	PHP_FE(getprotobyname,													NULL)
#endif

#if HAVE_GETPROTOBYNUMBER
	PHP_FE(getprotobynumber,												NULL)
#endif

	PHP_FE(getmyuid,														NULL)
	PHP_FE(getmygid,														NULL)
	PHP_FE(getmypid,														NULL)
	PHP_FE(getmyinode,														NULL)
	PHP_FE(getlastmod,														NULL)

	PHP_FE(base64_decode,													NULL)
	PHP_FE(base64_encode,													NULL)

	PHP_FE(abs,																NULL)
	PHP_FE(ceil,															NULL)
	PHP_FE(floor,															NULL)
	PHP_FE(round,															NULL)
	PHP_FE(sin,																NULL)
	PHP_FE(cos,																NULL)
	PHP_FE(tan,																NULL)
	PHP_FE(asin,															NULL)
	PHP_FE(acos,															NULL)
	PHP_FE(atan,															NULL)
	PHP_FE(atan2,															NULL)
	PHP_FE(sinh,															NULL)
	PHP_FE(cosh,															NULL)
	PHP_FE(tanh,															NULL)

#ifdef HAVE_ASINH 
	PHP_FE(asinh,															NULL)
#endif
#ifdef HAVE_ACOSH
	PHP_FE(acosh,															NULL)
#endif
#ifdef HAVE_ATANH
	PHP_FE(atanh,															NULL)
#endif
#if !defined(PHP_WIN32) && !defined(NETWARE)
	PHP_FE(expm1,															NULL)
	PHP_FE(log1p,															NULL)
#endif

	PHP_FE(pi,																NULL)
	PHP_FE(is_finite,														NULL)
	PHP_FE(is_nan,															NULL)
	PHP_FE(is_infinite,														NULL)
	PHP_FE(pow,																NULL)
	PHP_FE(exp,																NULL)
	PHP_FE(log,																NULL)
	PHP_FE(log10,															NULL)
	PHP_FE(sqrt,															NULL)
	PHP_FE(hypot,															NULL)
	PHP_FE(deg2rad,															NULL)
	PHP_FE(rad2deg,															NULL)
	PHP_FE(bindec,															NULL)
	PHP_FE(hexdec,															NULL)
	PHP_FE(octdec,															NULL)
	PHP_FE(decbin,															NULL)
	PHP_FE(decoct,															NULL)
	PHP_FE(dechex,															NULL)
	PHP_FE(base_convert,													NULL)
	PHP_FE(number_format,													NULL)
	PHP_FE(fmod,															NULL)
	PHP_FE(ip2long,															NULL)
	PHP_FE(long2ip,															NULL)

	PHP_FE(getenv,															NULL)
#ifdef HAVE_PUTENV
	PHP_FE(putenv,															NULL)
#endif

#ifdef HAVE_GETOPT
	PHP_FE(getopt,															NULL)
#endif

#ifdef HAVE_GETTIMEOFDAY
	PHP_FE(microtime,														NULL)
	PHP_FE(gettimeofday,													NULL)
#endif

#ifdef HAVE_GETRUSAGE
	PHP_FE(getrusage,														NULL)
#endif

#ifdef HAVE_GETTIMEOFDAY
	PHP_FE(uniqid,															NULL)
#endif

	PHP_FE(quoted_printable_decode,											NULL)
	PHP_FE(convert_cyr_string,												NULL)
	PHP_FE(get_current_user,												NULL)
	PHP_FE(set_time_limit,													NULL)
	PHP_FE(get_cfg_var,														NULL)
	PHP_FALIAS(magic_quotes_runtime, set_magic_quotes_runtime,				NULL)
	PHP_FE(set_magic_quotes_runtime,										NULL)
	PHP_FE(get_magic_quotes_gpc,											NULL)
	PHP_FE(get_magic_quotes_runtime,										NULL)

	PHP_FE(import_request_variables,										NULL)
	PHP_FE(error_log,														NULL)
	PHP_FE(call_user_func,													NULL)
	PHP_FE(call_user_func_array,											NULL)
	PHP_FE(call_user_method,		second_arg_force_ref)
	PHP_FE(call_user_method_array,	second_arg_force_ref)
	PHP_FE(serialize,														NULL)															
	PHP_FE(unserialize,														NULL)

	PHP_FE(var_dump,														NULL)
	PHP_FE(var_export,														NULL)
	PHP_FE(debug_zval_dump,														NULL)
	PHP_FE(print_r,															NULL)
#if MEMORY_LIMIT 
	PHP_FE(memory_get_usage,												NULL)
#endif

	PHP_FE(register_shutdown_function,										NULL)
	PHP_FE(register_tick_function,											NULL)
	PHP_FE(unregister_tick_function,										NULL)

	PHP_FE(highlight_file,													NULL)
	PHP_FALIAS(show_source, 		highlight_file,							NULL)
	PHP_FE(highlight_string,												NULL)

	PHP_FE(ini_get,															NULL)
	PHP_FE(ini_get_all,														NULL)
	PHP_FE(ini_set,															NULL)
	PHP_FALIAS(ini_alter,			ini_set,								NULL)
	PHP_FE(ini_restore,														NULL)
	PHP_FE(get_include_path,												NULL)
	PHP_FE(set_include_path,												NULL)
	PHP_FE(restore_include_path,											NULL)

	PHP_FE(setcookie,														NULL)
	PHP_FE(header,															NULL)
	PHP_FE(headers_sent,  first_and_second__args_force_ref)

	PHP_FE(connection_aborted,												NULL)
	PHP_FE(connection_status,												NULL)
	PHP_FE(ignore_user_abort,												NULL)
	PHP_FE(parse_ini_file,													NULL)
	PHP_FE(is_uploaded_file,												NULL)
	PHP_FE(move_uploaded_file,												NULL)

	/* functions from dns.c */
	PHP_FE(gethostbyaddr,													NULL)
	PHP_FE(gethostbyname,													NULL)
	PHP_FE(gethostbynamel,													NULL)

#if HAVE_RES_SEARCH && !(defined(__BEOS__) || defined(PHP_WIN32) || defined(NETWARE))
	PHP_FE(dns_check_record,												NULL)
	PHP_FALIAS(checkdnsrr,			dns_check_record,						NULL)
# if HAVE_DN_SKIPNAME && HAVE_DN_EXPAND
	PHP_FE(dns_get_mx,				second_and_third_args_force_ref)
	PHP_FALIAS(getmxrr, 			dns_get_mx, second_and_third_args_force_ref)
# endif
# if HAVE_DNS_FUNCS
	PHP_FE(dns_get_record,			third_and_rest_force_ref)
# endif
#endif

	/* functions from type.c */
	PHP_FE(intval,															NULL)
	PHP_FE(floatval,														NULL)
	PHP_FALIAS(doubleval,          floatval,				                NULL)
	PHP_FE(strval,															NULL)
	PHP_FE(gettype,															NULL)
	PHP_FE(settype,					first_arg_force_ref)
	PHP_FE(is_null,															NULL)
	PHP_FE(is_resource,														NULL)
	PHP_FE(is_bool,															NULL)
	PHP_FE(is_long,															NULL)
	PHP_FE(is_float,														NULL)
	PHP_FALIAS(is_int,				is_long,								NULL)
	PHP_FALIAS(is_integer,			is_long,								NULL)
	PHP_FALIAS(is_double,			is_float,								NULL)
	PHP_FALIAS(is_real,				is_float,								NULL)
	PHP_FE(is_numeric,														NULL)
	PHP_FE(is_string,														NULL)
	PHP_FE(is_array,														NULL)
	PHP_FE(is_object,														NULL)
	PHP_FE(is_scalar,														NULL)
	PHP_FE(is_callable,				third_arg_force_ref)

	/* functions from reg.c */
	PHP_FE(ereg,					third_arg_force_ref)
	PHP_FE(ereg_replace,													NULL)
	PHP_FE(eregi,					third_arg_force_ref)
	PHP_FE(eregi_replace,													NULL)
	PHP_FE(split,															NULL)
	PHP_FE(spliti,															NULL)
	PHP_FALIAS(join,				implode,								NULL)
	PHP_FE(sql_regcase,														NULL)

	/* functions from dl.c */
	PHP_FE(dl,																NULL)

	/* functions from file.c */
	PHP_FE(pclose,															NULL)
	PHP_FE(popen,															NULL)
	PHP_FE(readfile,														NULL)
	PHP_FE(rewind,															NULL)
	PHP_FE(rmdir,															NULL)
	PHP_FE(umask,															NULL)
	PHP_FE(fclose,															NULL)
	PHP_FE(feof,															NULL)
	PHP_FE(fgetc,															NULL)
	PHP_FE(fgets,															NULL)
	PHP_FE(fgetss,															NULL)
	PHP_FE(fread,															NULL)
	PHP_STATIC_FE("fopen",			php_if_fopen,							NULL)
	PHP_FE(fpassthru,														NULL)
	PHP_STATIC_FE("ftruncate",		php_if_ftruncate,						NULL)
	PHP_STATIC_FE("fstat",			php_if_fstat,							NULL)
	PHP_FE(fseek,															NULL)
	PHP_FE(ftell,															NULL)
	PHP_FE(fflush,															NULL)
	PHP_FE(fwrite,															NULL)
	PHP_FALIAS(fputs,				fwrite,									NULL)
	PHP_FE(mkdir,															NULL)
	PHP_FE(rename,															NULL)
	PHP_FE(copy,															NULL)
	PHP_FE(tempnam,															NULL)
	PHP_STATIC_FE("tmpfile",		php_if_tmpfile,							NULL)
	PHP_FE(file,															NULL)
	PHP_FE(file_get_contents,												NULL)
	PHP_FE(stream_select,					  first_through_third_args_force_ref)
	PHP_FE(stream_context_create,											NULL)
	PHP_FE(stream_context_set_params,										NULL)
	PHP_FE(stream_context_set_option,										NULL)
	PHP_FE(stream_context_get_options,										NULL)
	PHP_FE(stream_filter_prepend,											NULL)
	PHP_FE(stream_filter_append,											NULL)
	PHP_FE(fgetcsv,															NULL)
	PHP_FE(flock,															NULL)
	PHP_FE(get_meta_tags,													NULL)
	PHP_FE(stream_set_write_buffer,											NULL)
	PHP_FALIAS(set_file_buffer, stream_set_write_buffer,					NULL)

	PHP_FE(set_socket_blocking,												NULL)
	PHP_FE(stream_set_blocking,												NULL)
	PHP_FALIAS(socket_set_blocking, stream_set_blocking,					NULL)

	PHP_FE(stream_get_meta_data,											NULL)
	PHP_FE(stream_get_line,												NULL)
	PHP_FE(stream_register_wrapper,											NULL)
	PHP_FE(stream_get_wrappers,												NULL)
	PHP_FE(get_headers,													NULL)

#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
	PHP_FE(stream_set_timeout,												NULL)
	PHP_FALIAS(socket_set_timeout, stream_set_timeout,						NULL)
#endif

	PHP_FALIAS(socket_get_status, stream_get_meta_data,						NULL)

#if (!defined(PHP_WIN32) && !defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
	PHP_FE(realpath,														NULL)
#endif

#ifdef HAVE_FNMATCH
	PHP_FE(fnmatch,														    NULL)
#endif

	/* functions from fsock.c */
	PHP_FE(fsockopen,				third_and_fourth_args_force_ref)
	PHP_FE(pfsockopen,				third_and_fourth_args_force_ref)

	/* functions from pack.c */
	PHP_FE(pack,															NULL)
	PHP_FE(unpack,															NULL)

	/* functions from browscap.c */
	PHP_FE(get_browser,														NULL)

#if HAVE_CRYPT
	/* functions from crypt.c */
	PHP_FE(crypt,															NULL)
#endif

	/* functions from dir.c */
	PHP_FE(opendir,															NULL)
	PHP_FE(closedir,														NULL)
	PHP_FE(chdir,															NULL)

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
	PHP_FE(chroot,															NULL)
#endif

	PHP_FE(getcwd,															NULL)
	PHP_FE(rewinddir,														NULL)
	PHP_STATIC_FE("readdir",		php_if_readdir,							NULL)
	PHP_FALIAS(dir,					getdir,									NULL)
	PHP_FE(scandir,															NULL)
#ifdef HAVE_GLOB
	PHP_FE(glob,															NULL)
#endif
	/* functions from filestat.c */
	PHP_FE(fileatime,														NULL)
	PHP_FE(filectime,														NULL)
	PHP_FE(filegroup,														NULL)
	PHP_FE(fileinode,														NULL)
	PHP_FE(filemtime,														NULL)
	PHP_FE(fileowner,														NULL)
	PHP_FE(fileperms,														NULL)
	PHP_FE(filesize,														NULL)
	PHP_FE(filetype,														NULL)
	PHP_FE(file_exists,														NULL)
	PHP_FE(is_writable,														NULL)
	PHP_FALIAS(is_writeable,		is_writable,							NULL)
	PHP_FE(is_readable,														NULL)
	PHP_FE(is_executable,													NULL)
	PHP_FE(is_file,															NULL)
	PHP_FE(is_dir,															NULL)
	PHP_FE(is_link,															NULL)
	PHP_STATIC_FE("stat",			php_if_stat,							NULL)
	PHP_STATIC_FE("lstat",			php_if_lstat,							NULL)
	PHP_FE(chown,															NULL)
	PHP_FE(chgrp,															NULL)
	PHP_FE(chmod,															NULL)
#if HAVE_UTIME
	PHP_FE(touch,															NULL)
#endif	
	PHP_FE(clearstatcache,													NULL)
	PHP_FE(disk_total_space,												NULL)
	PHP_FE(disk_free_space,													NULL)
	PHP_FALIAS(diskfreespace,		disk_free_space,						NULL)

	/* functions from mail.c */
#ifdef HAVE_SENDMAIL
	PHP_FE(mail,															NULL)
	PHP_FE(ezmlm_hash,														NULL)
#endif

	/* functions from syslog.c */
#ifdef HAVE_SYSLOG_H
	PHP_FE(openlog,															NULL)
	PHP_FE(syslog,															NULL)
	PHP_FE(closelog,														NULL)
	PHP_FE(define_syslog_variables,											NULL)
#endif

	/* functions from lcg.c */
	PHP_FE(lcg_value,														NULL)

	/* functions from metaphone.c */
	PHP_FE(metaphone,														NULL)

	/* functions from output.c */
	PHP_FE(ob_start,														NULL)
	PHP_FE(ob_flush,														NULL)
	PHP_FE(ob_clean,														NULL)
	PHP_FE(ob_end_flush,													NULL)
	PHP_FE(ob_end_clean,													NULL)
	PHP_FE(ob_get_flush,													NULL)
	PHP_FE(ob_get_clean,													NULL)
	PHP_FE(ob_get_length,													NULL)
	PHP_FE(ob_get_level,													NULL)
	PHP_FE(ob_get_status,													NULL)
	PHP_FE(ob_get_contents,													NULL)
	PHP_FE(ob_implicit_flush,												NULL)
	PHP_FE(ob_list_handlers,												NULL)

	/* functions from array.c */
	PHP_FE(ksort,					first_arg_force_ref)
	PHP_FE(krsort,					first_arg_force_ref)
	PHP_FE(natsort,					first_arg_force_ref)
	PHP_FE(natcasesort,				first_arg_force_ref)
	PHP_FE(asort,					first_arg_force_ref)
	PHP_FE(arsort,					first_arg_force_ref)
	PHP_FE(sort,					first_arg_force_ref)
	PHP_FE(rsort,					first_arg_force_ref)
	PHP_FE(usort,					first_arg_force_ref)
	PHP_FE(uasort,					first_arg_force_ref)
	PHP_FE(uksort,					first_arg_force_ref)
	PHP_FE(shuffle,					first_arg_force_ref)
	PHP_FE(array_walk,				first_arg_force_ref)
	PHP_FE(array_walk_recursive,			first_arg_force_ref)
	PHP_FE(count,															NULL)
	PHP_FE(end,						first_arg_force_ref)
	PHP_FE(prev,					first_arg_force_ref)
	PHP_FE(next,					first_arg_force_ref)
	PHP_FE(reset,					first_arg_force_ref)
	PHP_FE(current,					first_arg_force_ref)
	PHP_FE(key,						first_arg_force_ref)
	PHP_FE(min,																NULL)
	PHP_FE(max,																NULL)
	PHP_FE(in_array,														NULL)
	PHP_FE(array_search,													NULL)
	PHP_FE(extract,															NULL)
	PHP_FE(compact,															NULL)
	PHP_FE(array_fill,														NULL)
	PHP_FE(range,															NULL)
	PHP_FE(array_multisort,													NULL)
	PHP_FE(array_push,				first_arg_force_ref)
	PHP_FE(array_pop,				first_arg_force_ref)
	PHP_FE(array_shift,				first_arg_force_ref)
	PHP_FE(array_unshift,			first_arg_force_ref)
	PHP_FE(array_splice,			first_arg_force_ref)
	PHP_FE(array_slice,														NULL)
	PHP_FE(array_merge,														NULL)
	PHP_FE(array_merge_recursive,											NULL)
	PHP_FE(array_keys,														NULL)
	PHP_FE(array_values,													NULL)
	PHP_FE(array_count_values,												NULL)
	PHP_FE(array_reverse,													NULL)
	PHP_FE(array_reduce,													NULL)
	PHP_FE(array_pad,														NULL)
	PHP_FE(array_flip,														NULL)
	PHP_FE(array_change_key_case,											NULL)
	PHP_FE(array_rand,														NULL)
	PHP_FE(array_unique,													NULL)
	PHP_FE(array_intersect,													NULL)
	PHP_FE(array_intersect_assoc,													NULL)
	PHP_FE(array_diff,														NULL)
	PHP_FE(array_diff_assoc,														NULL)
	PHP_FE(array_sum,														NULL)
	PHP_FE(array_filter,													NULL)
	PHP_FE(array_map,														NULL)
	PHP_FE(array_chunk,														NULL)
	PHP_FE(array_combine,														NULL)
	PHP_FE(array_key_exists,														NULL)

	/* aliases from array.c */
	PHP_FALIAS(pos, 				current, 				 first_arg_force_ref)
	PHP_FALIAS(sizeof, 				count, 									NULL)
	PHP_FALIAS(key_exists,			array_key_exists,						NULL)

	/* functions from assert.c */
	PHP_FE(assert,															NULL)
	PHP_FE(assert_options,													NULL)

    /* functions from versioning.c */
    PHP_FE(version_compare,													NULL)

	/* functions from ftok.c*/
#if HAVE_FTOK
	PHP_FE(ftok,	NULL)
#endif

	PHP_FE(str_rot13, NULL)
	PHP_FE(stream_get_filters, NULL)
	PHP_FE(stream_register_filter, NULL)

	/* functions from aggregate.c */
	PHP_FE(aggregate,						first_arg_force_ref)
	PHP_FE(aggregate_methods,				first_arg_force_ref)
	PHP_FE(aggregate_methods_by_list,		first_arg_force_ref)
	PHP_FE(aggregate_properties,			first_arg_force_ref)
	PHP_FE(aggregate_properties_by_list,	first_arg_force_ref)
#if (HAVE_PCRE || HAVE_BUNDLED_PCRE) && !defined(COMPILE_DL_PCRE)
	PHP_FE(aggregate_methods_by_regexp,		first_arg_force_ref)
	PHP_FE(aggregate_properties_by_regexp,	first_arg_force_ref)
#endif
	PHP_FE(deaggregate,						first_arg_force_ref)
	PHP_FE(aggregate_info,				first_arg_force_ref)
	PHP_FALIAS(aggregation_info,			aggregate_info,				first_arg_force_ref)

	PHP_FE(output_add_rewrite_var,											NULL)
	PHP_FE(output_reset_rewrite_vars,										NULL)
	PHP_FE(date_sunrise,														NULL)
	PHP_FE(date_sunset,															NULL)

	{NULL, NULL, NULL}
};


static PHP_INI_MH(OnUpdateSafeModeProtectedEnvVars)
{
	char *protected_vars, *protected_var;
	char *token_buf;
	int dummy = 1;

	protected_vars = estrndup(new_value, new_value_length);
	zend_hash_clean(&BG(sm_protected_env_vars));

	protected_var = php_strtok_r(protected_vars, ", ", &token_buf);
	while (protected_var) {
		zend_hash_update(&BG(sm_protected_env_vars), protected_var, strlen(protected_var), &dummy, sizeof(int), NULL);
		protected_var = php_strtok_r(NULL, ", ", &token_buf);
	}
	efree(protected_vars);
	return SUCCESS;
}


static PHP_INI_MH(OnUpdateSafeModeAllowedEnvVars)
{
	if (BG(sm_allowed_env_vars)) {
		free(BG(sm_allowed_env_vars));
	}
	BG(sm_allowed_env_vars) = zend_strndup(new_value, new_value_length);
	return SUCCESS;
}


PHP_INI_BEGIN()
	PHP_INI_ENTRY_EX("safe_mode_protected_env_vars", SAFE_MODE_PROTECTED_ENV_VARS, PHP_INI_SYSTEM, OnUpdateSafeModeProtectedEnvVars, NULL)
	PHP_INI_ENTRY_EX("safe_mode_allowed_env_vars",   SAFE_MODE_ALLOWED_ENV_VARS,   PHP_INI_SYSTEM, OnUpdateSafeModeAllowedEnvVars,   NULL)
	PHP_INI_ENTRY("date.default_latitude",  DATE_DEFAULT_LATITUDE, PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.default_longitude",  DATE_DEFAULT_LONGITUDE, PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunset_zenith",DATE_SUNSET_ZENITH, PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunrise_zenith",DATE_SUNRISE_ZENITH, PHP_INI_ALL, NULL)
PHP_INI_END()


zend_module_entry basic_functions_module = {
    STANDARD_MODULE_HEADER,
	"standard",					/* extension name */
	basic_functions,			/* function list */
	PHP_MINIT(basic),			/* process startup */
	PHP_MSHUTDOWN(basic),		/* process shutdown */
	PHP_RINIT(basic),			/* request startup */
	PHP_RSHUTDOWN(basic),		/* request shutdown */
	PHP_MINFO(basic),			/* extension info */
    PHP_VERSION,				/* extension version */
	STANDARD_MODULE_PROPERTIES
};


#if defined(HAVE_PUTENV)
static void php_putenv_destructor(putenv_entry *pe)
{
	if (pe->previous_value) {
		putenv(pe->previous_value);
	} else {
# if HAVE_UNSETENV
		unsetenv(pe->key);
# else
		char **env;

		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env, pe->key, pe->key_len) && (*env)[pe->key_len] == '=') {	/* found it */
				*env = "";
				break;
			}
		}
# endif
	}
	efree(pe->putenv_string);
	efree(pe->key);
}
#endif


static void basic_globals_ctor(php_basic_globals *basic_globals_p TSRMLS_DC)
{
	BG(next) = NULL;
	BG(left) = -1;
	BG(user_tick_functions) = NULL;
	BG(aggregation_table) = NULL;
	BG(user_filter_map) = NULL;
	zend_hash_init(&BG(sm_protected_env_vars), 5, NULL, NULL, 1);
	BG(sm_allowed_env_vars) = NULL;

	memset(&BG(url_adapt_state), 0, sizeof(BG(url_adapt_state)));
	memset(&BG(url_adapt_state_ex), 0, sizeof(BG(url_adapt_state_ex)));

	BG(incomplete_class) = php_create_incomplete_class(TSRMLS_C);
}


static void basic_globals_dtor(php_basic_globals *basic_globals_p TSRMLS_DC)
{
	zend_hash_destroy(&BG(sm_protected_env_vars));
	if (BG(sm_allowed_env_vars)) {
		free(BG(sm_allowed_env_vars));
	}
}


PHP_MINIT_FUNCTION(basic)
{
#ifdef ZTS
	ts_allocate_id(&basic_globals_id, sizeof(php_basic_globals), (ts_allocate_ctor) basic_globals_ctor, (ts_allocate_dtor) basic_globals_dtor);
#else
	basic_globals_ctor(&basic_globals TSRMLS_CC);
#endif

	REGISTER_LONG_CONSTANT("CONNECTION_ABORTED", PHP_CONNECTION_ABORTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_NORMAL",  PHP_CONNECTION_NORMAL,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_TIMEOUT", PHP_CONNECTION_TIMEOUT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("INI_USER",   ZEND_INI_USER,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_PERDIR", ZEND_INI_PERDIR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SYSTEM", ZEND_INI_SYSTEM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_ALL",    ZEND_INI_ALL,    CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_TIMESTAMP", SUNFUNCS_RET_TIMESTAMP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_STRING", SUNFUNCS_RET_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_DOUBLE", SUNFUNCS_RET_DOUBLE, CONST_CS | CONST_PERSISTENT);

#define REGISTER_MATH_CONSTANT(x)  REGISTER_DOUBLE_CONSTANT(#x, x, CONST_CS | CONST_PERSISTENT)
	REGISTER_MATH_CONSTANT(M_E);
	REGISTER_MATH_CONSTANT(M_LOG2E);
	REGISTER_MATH_CONSTANT(M_LOG10E);
	REGISTER_MATH_CONSTANT(M_LN2);
	REGISTER_MATH_CONSTANT(M_LN10);
	REGISTER_MATH_CONSTANT(M_PI);
	REGISTER_MATH_CONSTANT(M_PI_2);
	REGISTER_MATH_CONSTANT(M_PI_4);
	REGISTER_MATH_CONSTANT(M_1_PI);
	REGISTER_MATH_CONSTANT(M_2_PI);
	REGISTER_MATH_CONSTANT(M_2_SQRTPI);
	REGISTER_MATH_CONSTANT(M_SQRT2);
	REGISTER_MATH_CONSTANT(M_SQRT1_2);

#if ENABLE_TEST_CLASS
	test_class_startup();
#endif

	REGISTER_INI_ENTRIES();

	register_phpinfo_constants(INIT_FUNC_ARGS_PASSTHRU);
	register_html_constants(INIT_FUNC_ARGS_PASSTHRU);
	register_string_constants(INIT_FUNC_ARGS_PASSTHRU);

	PHP_MINIT(regex)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(file)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(pack)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(browscap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(standard_filters)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(user_filters)(INIT_FUNC_ARGS_PASSTHRU);

#if defined(HAVE_LOCALECONV) && defined(ZTS)
	PHP_MINIT(localeconv)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#if defined(HAVE_NL_LANGINFO)
	PHP_MINIT(nl_langinfo)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#if HAVE_CRYPT
	PHP_MINIT(crypt)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	PHP_MINIT(lcg)(INIT_FUNC_ARGS_PASSTHRU);

	PHP_MINIT(dir)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
	PHP_MINIT(syslog)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	PHP_MINIT(array)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(assert)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(url_scanner_ex)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_MINIT(proc_open)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	PHP_MINIT(user_streams)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(imagetypes)(INIT_FUNC_ARGS_PASSTHRU);

	php_register_url_stream_wrapper("php", &php_stream_php_wrapper TSRMLS_CC);
#ifndef PHP_CURL_URL_WRAPPERS
	php_register_url_stream_wrapper("http", &php_stream_http_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("ftp", &php_stream_ftp_wrapper TSRMLS_CC);
# if HAVE_OPENSSL_EXT
	php_register_url_stream_wrapper("https", &php_stream_http_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("ftps", &php_stream_ftp_wrapper TSRMLS_CC);
# endif
#endif

#if HAVE_RES_SEARCH && !(defined(__BEOS__)||defined(PHP_WIN32) || defined(NETWARE))
# if HAVE_DNS_FUNCS
	PHP_MINIT(dns)(INIT_FUNC_ARGS_PASSTHRU);
# endif
#endif

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(basic)
{
#ifdef ZTS
	ts_free_id(basic_globals_id);
#else
	basic_globals_dtor(&basic_globals TSRMLS_CC);
#endif

	php_unregister_url_stream_wrapper("php" TSRMLS_CC);
#ifndef PHP_CURL_URL_WRAPPERS
	php_unregister_url_stream_wrapper("http" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftp" TSRMLS_CC);
# if HAVE_OPENSSL_EXT
	php_unregister_url_stream_wrapper("https" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftps" TSRMLS_CC);
# endif
#endif

	UNREGISTER_INI_ENTRIES();

	PHP_MSHUTDOWN(regex)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(browscap)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(array)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(assert)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(url_scanner_ex)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(file)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(standard_filters)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#if defined(HAVE_LOCALECONV) && defined(ZTS)
	PHP_MSHUTDOWN(localeconv)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}


PHP_RINIT_FUNCTION(basic)
{
	memset(BG(strtok_table), 0, 256);
	BG(strtok_string) = NULL;
	BG(strtok_zval) = NULL;
	BG(locale_string) = NULL;
	BG(user_compare_func_name) = NULL;
	BG(array_walk_func_name) = NULL;
#ifdef HAVE_MMAP
	BG(mmap_file) = NULL;
#endif
	BG(page_uid) = -1;
	BG(page_gid) = -1;
	BG(page_inode) = -1;
	BG(page_mtime) = -1;
#ifdef HAVE_PUTENV
	if (zend_hash_init(&BG(putenv_ht), 1, NULL, (void (*)(void *)) php_putenv_destructor, 0) == FAILURE) {
		return FAILURE;
	}
#endif
	BG(user_shutdown_function_names) = NULL;

#if HAVE_CRYPT
	PHP_RINIT(crypt)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	PHP_RINIT(lcg)(INIT_FUNC_ARGS_PASSTHRU);

	PHP_RINIT(filestat)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
	PHP_RINIT(syslog)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	PHP_RINIT(dir)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_RINIT(url_scanner_ex)(INIT_FUNC_ARGS_PASSTHRU);

	/* Reset magic_quotes_runtime */
	PG(magic_quotes_runtime) = INI_BOOL("magic_quotes_runtime");

	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(basic)
{
	if (BG(strtok_zval)) {
		zval_ptr_dtor(&BG(strtok_zval));
	}
	BG(strtok_string) = NULL;
	BG(strtok_zval) = NULL;
#ifdef HAVE_PUTENV
	zend_hash_destroy(&BG(putenv_ht));
#endif

	/* Check if locale was changed and change it back
	   to the value in startup environment */
	if (BG(locale_string) != NULL) {
		setlocale(LC_ALL, "C");
		setlocale(LC_CTYPE, "");
	}
	STR_FREE(BG(locale_string));

	PHP_RSHUTDOWN(fsock)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(filestat)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
	PHP_RSHUTDOWN(syslog)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif
	PHP_RSHUTDOWN(assert)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(url_scanner_ex)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(streams)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

	if (BG(user_tick_functions)) {
		zend_llist_destroy(BG(user_tick_functions));
		efree(BG(user_tick_functions));
		BG(user_tick_functions) = NULL;
	}

	if (BG(aggregation_table)) {
		zend_hash_destroy(BG(aggregation_table));
		efree(BG(aggregation_table));
		BG(aggregation_table) = NULL;
	}

	if (BG(user_filter_map)) {
		zend_hash_destroy(BG(user_filter_map));
		efree(BG(user_filter_map));
		BG(user_filter_map) = NULL;
	}
	
#ifdef HAVE_MMAP
	if (BG(mmap_file)) {
		munmap(BG(mmap_file), BG(mmap_len));
	}
#endif

	return SUCCESS;
}


PHP_MINFO_FUNCTION(basic)
{
	php_info_print_table_start();
	PHP_MINFO(regex)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
	PHP_MINFO(dl)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
	PHP_MINFO(mail)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
	php_info_print_table_end();
	PHP_MINFO(assert)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
}


/* {{{ proto mixed constant(string const_name)
   Given the name of a constant this function will return the constants associated value */
PHP_FUNCTION(constant)
{
	zval **const_name;

	if (ZEND_NUM_ARGS() != 1 ||
		zend_get_parameters_ex(1, &const_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(const_name);

	if (!zend_get_constant(Z_STRVAL_PP(const_name), Z_STRLEN_PP(const_name), return_value TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't find constant %s", Z_STRVAL_PP(const_name));
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto int ip2long(string ip_address)
   Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address */
PHP_FUNCTION(ip2long)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(str);

	RETURN_LONG(ntohl(inet_addr(Z_STRVAL_PP(str))));
}
/* }}} */

/* {{{ proto string long2ip(int proper_address)
   Converts an (IPv4) Internet network address into a string in Internet standard dotted format */
PHP_FUNCTION(long2ip)
{
	zval **num;
	unsigned long n;
	struct in_addr myaddr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(num);
	
	n = strtoul(Z_STRVAL_PP(num), NULL, 0);

	myaddr.s_addr = htonl(n);
	RETURN_STRING(inet_ntoa(myaddr), 1);
}
/* }}} */


/********************
 * System Functions *
 ********************/

/* {{{ proto string getenv(string varname)
   Get the value of an environment variable */
PHP_FUNCTION(getenv)
{
	char *ptr, *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	ptr = sapi_getenv(str, str_len TSRMLS_CC);
	if (! ptr) {
		ptr = getenv(str);
	}
	if (ptr) {
		RETURN_STRING(ptr, 1);
	}
	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_PUTENV
/* {{{ proto bool putenv(string setting)
   Set the value of an environment variable */
PHP_FUNCTION(putenv)
{
	pval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if (Z_STRVAL_PP(str) && *(Z_STRVAL_PP(str))) {
		char *p, **env;
		putenv_entry pe;

		pe.putenv_string = estrndup(Z_STRVAL_PP(str), Z_STRLEN_PP(str));
		pe.key = estrndup(Z_STRVAL_PP(str), Z_STRLEN_PP(str));
		if ((p = strchr(pe.key, '='))) {	/* nullify the '=' if there is one */
			*p = '\0';
		}
		pe.key_len = strlen(pe.key);

		if (PG(safe_mode)) {
			/* Check the protected list */
			if (zend_hash_exists(&BG(sm_protected_env_vars), pe.key, pe.key_len)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Safe Mode warning: Cannot override protected environment variable '%s'", pe.key);
				efree(pe.putenv_string);
				efree(pe.key);
				RETURN_FALSE;
			}

			/* Check the allowed list */
			if (BG(sm_allowed_env_vars) && *BG(sm_allowed_env_vars)) {
				char *allowed_env_vars = estrdup(BG(sm_allowed_env_vars));
				char *allowed_prefix = strtok(allowed_env_vars, ", ");
				zend_bool allowed = 0;

				while (allowed_prefix) {
					if (!strncmp(allowed_prefix, pe.key, strlen(allowed_prefix))) {
						allowed = 1;
						break;
					}
					allowed_prefix = strtok(NULL, ", ");
				}
				efree(allowed_env_vars);
				if (!allowed) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Safe Mode warning: Cannot set environment variable '%s' - it's not in the allowed list", pe.key);
					efree(pe.putenv_string);
					efree(pe.key);
					RETURN_FALSE;
				}
			}
		}

		zend_hash_del(&BG(putenv_ht), pe.key, pe.key_len+1);

		/* find previous value */
		pe.previous_value = NULL;
		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env, pe.key, pe.key_len) && (*env)[pe.key_len] == '=') {	/* found it */
				pe.previous_value = *env;
				break;
			}
		}

		if (putenv(pe.putenv_string) == 0) {	/* success */
			zend_hash_add(&BG(putenv_ht), pe.key, pe.key_len+1, (void **) &pe, sizeof(putenv_entry), NULL);
#ifdef HAVE_TZSET
			if (!strncmp(pe.key, "TZ", 2)) {
				tzset();
			}
#endif
			RETURN_TRUE;
		} else {
			efree(pe.putenv_string);
			efree(pe.key);
			RETURN_FALSE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter syntax.");
	RETURN_FALSE;
}
/* }}} */
#endif

#ifdef HAVE_GETOPT
/* {{{ free_argv
   Free the memory allocated to an argv array. */
static void free_argv(char **argv, int argc)
{
	int i;

	if (argv) {
		for (i = 0; i < argc; i++) {
			if (argv[i]) {
				efree(argv[i]);
			}
		}
		efree(argv);
	}
}
/* }}} */

#ifdef HARTMUT_0
/* {{{ free_longopts
   Free the memory allocated to an longopt array. */
static void free_longopts(struct option *longopts)
{
	struct option *p;

	if(longopts) {
		for(p=longopts; p->name; p++) {
			efree((char *)(p->name));
		}
		
		efree(longopts);
	}
}
/* }}} */
#endif

/* {{{ proto array getopt(string options [, array longopts])
   Get options from the command line argument list */
PHP_FUNCTION(getopt)
{
	char *options = NULL, **argv = NULL;
	char opt[2] = { '\0' };
	char *optname;
	int argc = 0, options_len = 0, o;
	zval *val, **args = NULL, *p_longopts = NULL;
#ifdef HARTMUT_0
	struct option *longopts = NULL;
	int longindex = 0;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a",
							  &options, &options_len, &p_longopts) == FAILURE) {
		RETURN_FALSE;
	}

	/*
	 * Get argv from the global symbol table.  We calculate argc ourselves
	 * in order to be on the safe side, even though it is also available
	 * from the symbol table.
	 */
	if (zend_hash_find(HASH_OF(PG(http_globals)[TRACK_VARS_SERVER]), "argv", sizeof("argv"),
					   (void **) &args) != FAILURE) {
		int pos = 0;
		zval **arg;

		argc = zend_hash_num_elements(Z_ARRVAL_PP(args));

		/* 
		 * Attempt to allocate enough memory to hold all of the arguments
		 * and a trailing NULL 
		 */
		if ((argv = (char **) emalloc((argc + 1) * sizeof(char *))) == NULL) {
			RETURN_FALSE;
		}

		/* Reset the array indexes. */
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(args));

		/* Iterate over the hash to construct the argv array. */
		while (zend_hash_get_current_data(Z_ARRVAL_PP(args),
										  (void **)&arg) == SUCCESS) {
			argv[pos++] = estrdup(Z_STRVAL_PP(arg));
			zend_hash_move_forward(Z_ARRVAL_PP(args));
		}

		/* 
		 * The C Standard requires argv[argc] to be NULL - this might
		 * keep some getopt implementations happy. 
		 */
		argv[argc] = NULL;
	} else {
		/* Return false if we can't find argv. */
		RETURN_FALSE;
	}

	if(p_longopts) {
#ifdef HARTMUT_0
		int len, c = zend_hash_num_elements(Z_ARRVAL_P(p_longopts));
		struct option *p;
		zval **arg;
		char *name;

		longopts = (struct option *)ecalloc(c+1, sizeof(struct option));

		if(!longopts) RETURN_FALSE;

		/* Reset the array indexes. */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(p_longopts));
		p = longopts;

		/* Iterate over the hash to construct the argv array. */
		while (zend_hash_get_current_data(Z_ARRVAL_P(p_longopts),
										  (void **)&arg) == SUCCESS) {

			p->has_arg = 0;
			name = estrdup(Z_STRVAL_PP(arg));
			len = strlen(name);
			if((len > 0) && (name[len-1] == ':')) {
				p->has_arg++;
				name[len-1] = '\0';
				if((len > 1) && (name[len-2] == ':')) {
					p->has_arg++;
					name[len-2] = '\0';
				}
			}
				
			p->name = name; 
			p->flag = NULL;
			p->val = 0;

			zend_hash_move_forward(Z_ARRVAL_P(p_longopts));
			p++;
		}
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No support for long options in this build");
#endif	
	}

	/* Initialize the return value as an array. */
	array_init(return_value);

	/* Disable getopt()'s error messages. */
	opterr = 0;

	/* Invoke getopt(3) on the argument array. */
#ifdef HARTMUT_0
	while ((o = getopt_long(argc, argv, options, longopts, &longindex)) != -1) {
#else
	while ((o = getopt(argc, argv, options)) != -1) {
#endif
		/* Skip unknown arguments. */
		if (o == '?') {
			continue;
		}

		/* Prepare the option character and the argument string. */
		if(o == 0) {
#ifdef HARTMUT_0
			optname = (char *)longopts[longindex].name;
#else                      
			/* o == 0 shall never happen so this only fixes a compiler warning */
			optname = NULL;
#endif
		} else {		
			if(o == 1) o = '-';
			opt[0] = o;
			optname = opt;
		}

		MAKE_STD_ZVAL(val);
		if (optarg != NULL) {
			ZVAL_STRING(val, optarg, 1);
		} else {
			ZVAL_FALSE(val);
		}

		/* Add this option / argument pair to the result hash. */
		if(zend_hash_find(HASH_OF(return_value), optname, strlen(optname)+1, (void **)&args) != FAILURE) {
			if(Z_TYPE_PP(args) != IS_ARRAY) {
				convert_to_array_ex(args);
			} 
 			zend_hash_next_index_insert(HASH_OF(*args),  (void *)&val, sizeof(zval *), NULL);
		} else {
			zend_hash_add(HASH_OF(return_value), optname, strlen(optname)+1, (void *)&val,
						  sizeof(zval *), NULL);
		}
	}

	free_argv(argv, argc);
#ifdef HARTMUT_0
	free_longopts(longopts);
#endif
}
/* }}} */
#endif

/* {{{ proto void flush(void)
   Flush the output buffer */
PHP_FUNCTION(flush)
{
	sapi_flush(TSRMLS_C);
}
/* }}} */

/* {{{ proto void sleep(int seconds)
   Delay for a given number of seconds */
PHP_FUNCTION(sleep)
{
	pval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(num);
	php_sleep(Z_LVAL_PP(num));
}
/* }}} */

/* {{{ proto void usleep(int micro_seconds)
   Delay for a given number of micro seconds */
PHP_FUNCTION(usleep)
{
#if HAVE_USLEEP
	pval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	usleep(Z_LVAL_PP(num));
#endif
}
/* }}} */

/* {{{ proto string get_current_user(void)
   Get the name of the owner of the current PHP script */
PHP_FUNCTION(get_current_user)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING(php_get_current_user(), 1);
}
/* }}} */

/* {{{ proto string get_cfg_var(string option_name)
   Get the value of a PHP configuration option */
PHP_FUNCTION(get_cfg_var)
{
	pval **varname;
	char *value;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &varname) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);

	if (cfg_get_string(Z_STRVAL_PP(varname), &value) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto bool set_magic_quotes_runtime(int new_setting)
   Set the current active configuration setting of magic_quotes_runtime and return previous */
PHP_FUNCTION(set_magic_quotes_runtime)
{
	pval **new_setting;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &new_setting) == FAILURE) {
		RETURN_FALSE;
	}
	convert_to_boolean_ex(new_setting);

	PG(magic_quotes_runtime) = (zend_bool) Z_LVAL_PP(new_setting);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int get_magic_quotes_runtime(void)
   Get the current active configuration setting of magic_quotes_runtime */
PHP_FUNCTION(get_magic_quotes_runtime)
{
	RETURN_LONG(PG(magic_quotes_runtime));
}

/* }}} */

/* {{{ proto int get_magic_quotes_gpc(void)
   Get the current active configuration setting of magic_quotes_gpc */
PHP_FUNCTION(get_magic_quotes_gpc)
{
	RETURN_LONG(PG(magic_quotes_gpc));
}
/* }}} */

/*
	1st arg = error message
	2nd arg = error option
	3rd arg = optional parameters (email address or tcp address)
	4th arg = used for additional headers if email

error options:
    0 = send to php_error_log (uses syslog or file depending on ini setting)
	1 = send via email to 3rd parameter 4th option = additional headers
	2 = send via tcp/ip to 3rd parameter (name or ip:port)
	3 = save to file in 3rd parameter
*/

/* {{{ proto bool error_log(string message [, int message_type [, string destination [, string extra_headers]]])
   Send an error message somewhere */
PHP_FUNCTION(error_log)
{
	pval **string, **erropt = NULL, **option = NULL, **emailhead = NULL;
	int opt_err = 0;
	char *message, *opt = NULL, *headers = NULL;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &string) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument 1 invalid");
				RETURN_FALSE;
			}
			break;

		case 2:
			if (zend_get_parameters_ex(2, &string, &erropt) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid arguments");
				RETURN_FALSE;
			}
			convert_to_long_ex(erropt);
			opt_err = Z_LVAL_PP(erropt);
			break;
	
		case 3:
			if (zend_get_parameters_ex(3, &string, &erropt, &option) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid arguments");
				RETURN_FALSE;
			}
			convert_to_long_ex(erropt);
			opt_err = Z_LVAL_PP(erropt);
			convert_to_string_ex(option);
			opt = Z_STRVAL_PP(option);
			break;
		
		case 4:
			if (zend_get_parameters_ex (4, &string, &erropt, &option, &emailhead) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid arguments");
				RETURN_FALSE;
			}
			break;
	
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(string);
	message = Z_STRVAL_PP(string);

	if (erropt != NULL) {
		convert_to_long_ex(erropt);
		opt_err = Z_LVAL_PP(erropt);
	}

	if (option != NULL) {
		convert_to_string_ex(option);
		opt = Z_STRVAL_PP(option);
	}

	if (emailhead != NULL) {
		convert_to_string_ex(emailhead);
		headers = Z_STRVAL_PP(emailhead);
	}

	if (_php_error_log(opt_err, message, opt, headers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */


PHPAPI int _php_error_log(int opt_err, char *message, char *opt, char *headers TSRMLS_DC)
{
	php_stream *stream = NULL;

	switch (opt_err) {

		case 1:		/*send an email */
			{
#if HAVE_SENDMAIL
				if (!php_mail(opt, "PHP error_log message", message, headers, NULL TSRMLS_CC)) {
					return FAILURE;
				}
#else
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Mail option not available!");
				return FAILURE;
#endif
			}
			break;

		case 2:		/*send to an address */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "TCP/IP option not available!");
			return FAILURE;
			break;

		case 3:		/*save to a file */
			stream = php_stream_open_wrapper(opt, "a", IGNORE_URL | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL);
			if (!stream)
				return FAILURE;
			php_stream_write(stream, message, strlen(message));
			php_stream_close(stream);
			break;

		default:
			php_log_err(message TSRMLS_CC);
			break;
	}
	return SUCCESS;
}

/* {{{ proto mixed call_user_func(string function_name [, mixed parmeter] [, mixed ...])
   Call a user function which is the first parameter */
PHP_FUNCTION(call_user_func)
{
	zval ***params;
	zval *retval_ptr;
	char *name;
	int argc = ZEND_NUM_ARGS();

	if (argc < 1) {
		WRONG_PARAM_COUNT;
	}

	params = emalloc(sizeof(zval **) * argc);

	if (zend_get_parameters_array_ex(argc, params) == FAILURE) {
		efree(params);
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(params[0]) != IS_STRING && Z_TYPE_PP(params[0]) != IS_ARRAY) {
		SEPARATE_ZVAL(params[0]);
		convert_to_string_ex(params[0]);
	}

	if (!zend_is_callable(*params[0], 0, &name)) {
		php_error_docref1(NULL TSRMLS_CC, name, E_WARNING, "First argument is expected to be a valid callback");
		efree(name);
		efree(params);
		RETURN_NULL();
	}

	if (call_user_function_ex(EG(function_table), NULL, *params[0], &retval_ptr, argc-1, params+1, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		if (argc > 1) {
			SEPARATE_ZVAL(params[1]);
			convert_to_string_ex(params[1]);
			if (argc > 2) {
				SEPARATE_ZVAL(params[2]);
				convert_to_string_ex(params[2]);
				php_error_docref1(NULL TSRMLS_CC, name, E_WARNING, "Unable to call %s(%s,%s)", name, Z_STRVAL_PP(params[1]), Z_STRVAL_PP(params[2]));
			} else {
				php_error_docref1(NULL TSRMLS_CC, name, E_WARNING, "Unable to call %s(%s)", name, Z_STRVAL_PP(params[1]));
			}
		} else {
			php_error_docref1(NULL TSRMLS_CC, name, E_WARNING, "Unable to call %s()", name);
		}
	}

	efree(name);
	efree(params);
}
/* }}} */

/* {{{ proto mixed call_user_func_array(string function_name, array parameters)
   Call a user function which is the first parameter with the arguments contained in array */
PHP_FUNCTION(call_user_func_array)
{
	zval ***func_params, **func, **params;
	zval *retval_ptr;
	HashTable *func_params_ht;
	char *name;
	int count;
	int current = 0;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &func, &params) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	SEPARATE_ZVAL(params);
	convert_to_array_ex(params);

	if (Z_TYPE_PP(func) != IS_STRING && Z_TYPE_PP(func) != IS_ARRAY) {
		SEPARATE_ZVAL(func);
		convert_to_string_ex(func);
	}

	if (!zend_is_callable(*func, 0, &name)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "First argumented is expected to be a valid callback, '%s' was given", name);
		efree(name);
		RETURN_NULL();
	}

	func_params_ht = Z_ARRVAL_PP(params);

	count = zend_hash_num_elements(func_params_ht);
	func_params = emalloc(sizeof(zval **) * count);

	for (zend_hash_internal_pointer_reset(func_params_ht);
		 zend_hash_get_current_data(func_params_ht, (void **) &func_params[current]) == SUCCESS;
		 zend_hash_move_forward(func_params_ht)
		) {
		current++;
	}

	if (call_user_function_ex(EG(function_table), NULL, *func, &retval_ptr, count, func_params, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s()", name);
	}

	efree(name);
	efree(func_params);
}
/* }}} */

#define _CUM_DEPREC "This function is deprecated, use the call_user_func variety with the array(&$obj, \"method\") syntax instead"

/* {{{ proto mixed call_user_method(string method_name, mixed object [, mixed parameter] [, mixed ...])
   Call a user method on a specific object or class */
PHP_FUNCTION(call_user_method)
{
	zval ***params;
	zval *retval_ptr;
	int arg_count = ZEND_NUM_ARGS();

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, _CUM_DEPREC);

	if (arg_count < 2) {
		WRONG_PARAM_COUNT;
	}
	params = (zval ***) emalloc(sizeof(zval **) * arg_count);

	if (zend_get_parameters_array_ex(arg_count, params) == FAILURE) {
		efree(params);
		RETURN_FALSE;
	}
	if (Z_TYPE_PP(params[1]) != IS_OBJECT && Z_TYPE_PP(params[1]) != IS_STRING) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument is not an object or class name");
		efree(params);
		RETURN_FALSE;
	}

	SEPARATE_ZVAL(params[0]);
	convert_to_string(*params[0]);

	if (call_user_function_ex(EG(function_table), params[1], *params[0], &retval_ptr, arg_count-2, params+2, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s()", Z_STRVAL_PP(params[0]));
	}
	efree(params);
}
/* }}} */

/* {{{ proto mixed call_user_method_array(string method_name, mixed object, array params)
   Call a user method on a specific object or class using a parameter array */
PHP_FUNCTION(call_user_method_array)
{
	zval **method_name,	**obj, **params, ***method_args = NULL, *retval_ptr;
	HashTable *params_ar;
	int num_elems, element = 0;

	php_error_docref(NULL TSRMLS_CC, E_NOTICE, _CUM_DEPREC);

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &method_name, &obj, &params) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(obj) != IS_OBJECT && Z_TYPE_PP(obj) != IS_STRING) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument is not an object or class name");
		RETURN_FALSE;
	}

	SEPARATE_ZVAL(method_name);
	SEPARATE_ZVAL(params);
	convert_to_string_ex(method_name);
	convert_to_array_ex(params);

	params_ar = HASH_OF(*params);
	num_elems = zend_hash_num_elements(params_ar);
	method_args = (zval ***) emalloc(sizeof(zval **) *num_elems);

	for (zend_hash_internal_pointer_reset(params_ar);
		 zend_hash_get_current_data(params_ar, (void **) &(method_args[element])) == SUCCESS;
		 zend_hash_move_forward(params_ar)
		) {
		element++;
	}
	
	if (call_user_function_ex(EG(function_table), obj, *method_name, &retval_ptr, num_elems, method_args, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s()", Z_STRVAL_PP(method_name));
	}

	efree(method_args);
}
/* }}} */


void user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry)
{
	int i;

	for (i = 0; i < shutdown_function_entry->arg_count; i++) {
		zval_ptr_dtor(&shutdown_function_entry->arguments[i]);
	}
	efree(shutdown_function_entry->arguments);
}

void user_tick_function_dtor(user_tick_function_entry *tick_function_entry)
{
	int i;

	for (i = 0; i < tick_function_entry->arg_count; i++) {
		zval_ptr_dtor(&tick_function_entry->arguments[i]);
	}
	efree(tick_function_entry->arguments);
}

static int user_shutdown_function_call(php_shutdown_function_entry *shutdown_function_entry TSRMLS_DC)
{
	zval retval;

	if (call_user_function(	EG(function_table), NULL,
							shutdown_function_entry->arguments[0],
							&retval, 
							shutdown_function_entry->arg_count - 1,
							shutdown_function_entry->arguments + 1 
							TSRMLS_CC ) == SUCCESS ) {
		zval_dtor(&retval);

	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s() - function does not exist", Z_STRVAL_P(shutdown_function_entry->arguments[0]));
	}
	return 0;
}

static void user_tick_function_call(user_tick_function_entry *tick_fe TSRMLS_DC)
{
	zval retval;
	zval *function = tick_fe->arguments[0];
	
	/* Prevent reentrant calls to the same user ticks function */
	if (! tick_fe->calling) {
		tick_fe->calling = 1;

		if (call_user_function(	EG(function_table), NULL, 
								function, 
								&retval,
								tick_fe->arg_count - 1,
								tick_fe->arguments+1
								TSRMLS_CC) == SUCCESS) {
			zval_dtor(&retval);

		} else {
			zval **obj, **method;

			if (Z_TYPE_P(function) == IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s() - function does not exist", Z_STRVAL_P(function));
			} else if (	Z_TYPE_P(function) == IS_ARRAY 
						&& zend_hash_index_find(Z_ARRVAL_P(function), 0, (void **) &obj) == SUCCESS
						&& zend_hash_index_find(Z_ARRVAL_P(function), 1, (void **) &method) == SUCCESS
						&& Z_TYPE_PP(obj) == IS_OBJECT
						&& Z_TYPE_PP(method) == IS_STRING ) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s::%s() - function does not exist", Z_OBJCE_PP(obj)->name, Z_STRVAL_PP(method));
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call tick function");
			}
		}
	
		tick_fe->calling = 0;
	}
}

static void run_user_tick_functions(int tick_count)
{
	TSRMLS_FETCH();

	zend_llist_apply(BG(user_tick_functions), (llist_apply_func_t) user_tick_function_call TSRMLS_CC);
}

static int user_tick_function_compare(user_tick_function_entry * tick_fe1, user_tick_function_entry * tick_fe2)
{
	zval *func1 = tick_fe1->arguments[0];
	zval *func2 = tick_fe2->arguments[0];
	TSRMLS_FETCH();

	if (Z_TYPE_P(func1) == IS_STRING && Z_TYPE_P(func2) == IS_STRING) {
		return (zend_binary_zval_strcmp(func1, func2) == 0);
	} else if (Z_TYPE_P(func1) == IS_ARRAY && Z_TYPE_P(func2) == IS_ARRAY) {
		zval result;
		zend_compare_arrays(&result, func1, func2 TSRMLS_CC);
		return (Z_LVAL(result) == 0);
	} else {
		return 0;
	}
}

void php_call_shutdown_functions(void)
{
	TSRMLS_FETCH();

	if (BG(user_shutdown_function_names))
		zend_try {
			zend_hash_apply(BG(user_shutdown_function_names), (apply_func_t) user_shutdown_function_call TSRMLS_CC);
			memcpy(&EG(bailout), &orig_bailout, sizeof(jmp_buf));
			zend_hash_destroy(BG(user_shutdown_function_names));
			efree(BG(user_shutdown_function_names));
		}
		zend_end_try();
}

/* {{{ proto void register_shutdown_function(string function_name)
   Register a user-level function to be called on request termination */
PHP_FUNCTION(register_shutdown_function)
{
	php_shutdown_function_entry shutdown_function_entry;
	int i;

	shutdown_function_entry.arg_count = ZEND_NUM_ARGS();

	if (shutdown_function_entry.arg_count < 1) {
		WRONG_PARAM_COUNT;
	}

	shutdown_function_entry.arguments = (pval **) emalloc(sizeof(pval *) *shutdown_function_entry.arg_count);

	if (zend_get_parameters_array(ht, shutdown_function_entry.arg_count, shutdown_function_entry.arguments) == FAILURE) {
		RETURN_FALSE;
	}
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, (void (*)(void *)) user_shutdown_function_dtor, 0);
	}

	for (i = 0; i < shutdown_function_entry.arg_count; i++) {
		shutdown_function_entry.arguments[i]->refcount++;
	}
	zend_hash_next_index_insert(BG(user_shutdown_function_names), &shutdown_function_entry, sizeof(php_shutdown_function_entry), NULL);
}
/* }}} */


ZEND_API void php_get_highlight_struct(zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	syntax_highlighter_ini->highlight_comment = INI_STR("highlight.comment");
	syntax_highlighter_ini->highlight_default = INI_STR("highlight.default");
	syntax_highlighter_ini->highlight_html    = INI_STR("highlight.html");
	syntax_highlighter_ini->highlight_keyword = INI_STR("highlight.keyword");
	syntax_highlighter_ini->highlight_string  = INI_STR("highlight.string");
}

/* {{{ proto bool highlight_file(string file_name [, bool return] )
   Syntax highlight a source file */
PHP_FUNCTION(highlight_file)
{
	zval *filename;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	zend_bool i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &filename, &i) == FAILURE) {
		RETURN_FALSE;
	}
	convert_to_string(filename);

	if (PG(safe_mode) && (!php_checkuid(Z_STRVAL_P(filename), NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_P(filename) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (i) {
		php_start_ob_buffer (NULL, 0, 1 TSRMLS_CC);
	}

	php_get_highlight_struct(&syntax_highlighter_ini);

	if (highlight_file(Z_STRVAL_P(filename), &syntax_highlighter_ini TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}

	if (i) {
		php_ob_get_buffer (return_value TSRMLS_CC);
		php_end_ob_buffer (0, 0 TSRMLS_CC);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto bool highlight_string(string string [, bool return] )
   Syntax highlight a string or optionally return it */
PHP_FUNCTION(highlight_string)
{
	zval *expr;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	char *hicompiled_string_description;
	zend_bool  i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &expr, &i) == FAILURE) {
		RETURN_FALSE;
	}
	convert_to_string(expr);

	if (i) {
		php_start_ob_buffer (NULL, 0, 1 TSRMLS_CC);
	}

	php_get_highlight_struct(&syntax_highlighter_ini);

	hicompiled_string_description = zend_make_compiled_string_description("highlighted code" TSRMLS_CC);

	if (highlight_string(expr, &syntax_highlighter_ini, hicompiled_string_description TSRMLS_CC) == FAILURE) {
		efree(hicompiled_string_description);
		RETURN_FALSE;
	}
	efree(hicompiled_string_description);

	if (i) {
		php_ob_get_buffer (return_value TSRMLS_CC);
		php_end_ob_buffer (0, 0 TSRMLS_CC);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string ini_get(string varname)
   Get a configuration option */
PHP_FUNCTION(ini_get)
{
	pval **varname;
	char *str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &varname) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);

	str = zend_ini_string(Z_STRVAL_PP(varname), Z_STRLEN_PP(varname)+1, 0);

	if (!str) {
		RETURN_FALSE;
	}

	RETURN_STRING(str, 1);
}
/* }}} */


static int php_ini_get_option(zend_ini_entry *ini_entry, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *ini_array = va_arg(args, zval *);
	int module_number = va_arg(args, int);
	zval *option;

	if (module_number != 0 && ini_entry->module_number != module_number) {
		return 0;
	}

	if (hash_key->nKeyLength == 0 || hash_key->arKey[0] != 0) {

		MAKE_STD_ZVAL(option);
		array_init(option);

		if (ini_entry->orig_value) {
			add_assoc_stringl(option, "global_value", ini_entry->orig_value, ini_entry->orig_value_length, 1);
		} else if (ini_entry->value) {
			add_assoc_stringl(option, "global_value", ini_entry->value, ini_entry->value_length, 1);
		} else {
			add_assoc_null(option, "global_value");
		}

		if (ini_entry->value) {
			add_assoc_stringl(option, "local_value", ini_entry->value, ini_entry->value_length, 1);
		} else {
			add_assoc_null(option, "local_value");
		}

		add_assoc_long(option, "access", ini_entry->modifyable);

		add_assoc_zval_ex(ini_array, ini_entry->name, ini_entry->name_length, option);
	}
	return 0;
}

/* {{{ proto array ini_get_all([string extension])
   Get all configuration options */
PHP_FUNCTION(ini_get_all)
{
	char *extname = NULL;
	int extname_len = 0, extnumber = 0;
	zend_module_entry *module;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &extname, &extname_len) == FAILURE) {
		RETURN_FALSE;
	}

	zend_ini_sort_entries(TSRMLS_C);

	if (extname) {
		if (zend_hash_find(&module_registry, extname, extname_len+1, (void **) &module) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find extension '%s'", extname);
			RETURN_FALSE;
		}
		extnumber = module->module_number;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(ini_directives), (apply_func_args_t) php_ini_get_option, 2, return_value, extnumber TSRMLS_CC);
}
/* }}} */

static int php_ini_check_path(char *option_name, int option_len, char *new_option_name, int new_option_len)
{
	if ( option_len != (new_option_len-1) ) {
		return 0;
	}
	
	return strncmp(option_name, new_option_name, option_len);
}

/* {{{ proto string ini_set(string varname, string newvalue)
   Set a configuration option, returns false on error and the old value of the configuration option on success */
PHP_FUNCTION(ini_set)
{
	pval **varname, **new_value;
	char *old_value;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &varname, &new_value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);
	convert_to_string_ex(new_value);

	old_value = zend_ini_string(Z_STRVAL_PP(varname), Z_STRLEN_PP(varname)+1, 0);

	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value, 1);
	} else {
		RETVAL_FALSE;
	}

#define _CHECK_PATH(var, ini) php_ini_check_path(Z_STRVAL_PP(var), Z_STRLEN_PP(var), ini, sizeof(ini))
	
	/* safe_mode & basedir check */
	if (PG(safe_mode) || PG(open_basedir)) {
		if (_CHECK_PATH(varname, "error_log") ||
			_CHECK_PATH(varname, "java.class.path") ||
			_CHECK_PATH(varname, "java.home") ||
			_CHECK_PATH(varname, "java.library.path") ||
			_CHECK_PATH(varname, "session.save_path") ||
			_CHECK_PATH(varname, "vpopmail.directory")) {
			if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(new_value), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
				zval_dtor(return_value);
				RETURN_FALSE;
			}

			if (php_check_open_basedir(Z_STRVAL_PP(new_value) TSRMLS_CC)) {
				zval_dtor(return_value);
				RETURN_FALSE;
			}
		}
	}	
		
	/* checks that ensure the user does not overwrite certain ini settings when safe_mode is enabled */
	if (PG(safe_mode)) {
		if (!strncmp("max_execution_time", Z_STRVAL_PP(varname), sizeof("max_execution_time")) ||
			!strncmp("memory_limit", Z_STRVAL_PP(varname), sizeof("memory_limit")) ||
			!strncmp("child_terminate", Z_STRVAL_PP(varname), sizeof("child_terminate"))) {
			zval_dtor(return_value);
			RETURN_FALSE;
		}	
	}	
		
	if (zend_alter_ini_entry(Z_STRVAL_PP(varname), Z_STRLEN_PP(varname)+1, Z_STRVAL_PP(new_value), Z_STRLEN_PP(new_value),
								PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string ini_restore(string varname)
   Restore the value of a configuration option specified by varname */
PHP_FUNCTION(ini_restore)
{
	pval **varname;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &varname) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);

	zend_restore_ini_entry(Z_STRVAL_PP(varname), Z_STRLEN_PP(varname)+1, PHP_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto string set_include_path(string varname, string newvalue)
   Sets the include_path configuration option */

PHP_FUNCTION(set_include_path)
{
	pval **new_value;
	char *old_value;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &new_value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(new_value);
	old_value = zend_ini_string("include_path", sizeof("include_path"), 0);
	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value, 1);
	} else {
		RETVAL_FALSE;
	}
	if (zend_alter_ini_entry("include_path", sizeof("include_path"),
                             Z_STRVAL_PP(new_value), Z_STRLEN_PP(new_value),
                             PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto string get_include_path()
   Get the current include_path configuration option */

PHP_FUNCTION(get_include_path)
{
    char *str;
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	str = zend_ini_string("include_path", sizeof("include_path"), 0);
	if (str == NULL) {
		RETURN_FALSE;
	}
	RETURN_STRING(str, 1);
}

/* }}} */

/* {{{ proto string restore_include_path()
   Restore the value of the include_path configuration option */

PHP_FUNCTION(restore_include_path)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	zend_restore_ini_entry("include_path", sizeof("include_path"),
                           PHP_INI_STAGE_RUNTIME);
}

/* }}} */

/* {{{ proto bool print_r(mixed var [, bool return])
   Prints out or returns information about the specified variable */
PHP_FUNCTION(print_r)
{
	zval *var;
	zend_bool i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &var, &i) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (i) {
		php_start_ob_buffer (NULL, 0, 1 TSRMLS_CC);
	}

	zend_print_pval_r(var, 0 TSRMLS_CC);

	if (i) {
		php_ob_get_buffer (return_value TSRMLS_CC);
		php_end_ob_buffer (0, 0 TSRMLS_CC);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* This should go back to PHP */

/* {{{ proto int connection_aborted(void)
   Returns true if client disconnected */
PHP_FUNCTION(connection_aborted)
{
	RETURN_LONG(PG(connection_status) & PHP_CONNECTION_ABORTED);
}
/* }}} */

/* {{{ proto int connection_status(void)
   Returns the connection status bitfield */
PHP_FUNCTION(connection_status)
{
	RETURN_LONG(PG(connection_status));
}
/* }}} */

/* {{{ proto int ignore_user_abort(bool value)
   Set whether we want to ignore a user abort event or not */
PHP_FUNCTION(ignore_user_abort)
{
	pval **arg;
	int old_setting;

	old_setting = PG(ignore_user_abort);
	switch (ZEND_NUM_ARGS()) {

		case 0:
			break;
	
		case 1:
			if (zend_get_parameters_ex(1, &arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(arg);
			zend_alter_ini_entry("ignore_user_abort", sizeof("ignore_user_abort"), Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			break;
	
		default:	
			WRONG_PARAM_COUNT;
			break;
	}
	RETURN_LONG(old_setting);
}
/* }}} */

#if HAVE_GETSERVBYNAME
/* {{{ proto int getservbyname(string service, string protocol)
   Returns port associated with service. Protocol must be "tcp" or "udp" */
PHP_FUNCTION(getservbyname)
{
	pval **name, **proto;
	struct servent *serv;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &name, &proto) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	convert_to_string_ex(proto);

	serv = getservbyname(Z_STRVAL_PP(name), Z_STRVAL_PP(proto));

	if (serv == NULL) {
		RETURN_FALSE;
	}
	
	RETURN_LONG(ntohs(serv->s_port));
}
/* }}} */
#endif

#if HAVE_GETSERVBYPORT
/* {{{ proto string getservbyport(int port, string protocol)
   Returns service name associated with port. Protocol must be "tcp" or "udp" */
PHP_FUNCTION(getservbyport)
{
	pval **port, **proto;
	struct servent *serv;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &port, &proto) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(port);
	convert_to_string_ex(proto);

	serv = getservbyport(htons((unsigned short) Z_LVAL_PP(port)), Z_STRVAL_PP(proto));

	if (serv == NULL) {
		RETURN_FALSE;
	}
	
	RETURN_STRING(serv->s_name, 1);
}
/* }}} */
#endif

#if HAVE_GETPROTOBYNAME
/* {{{ proto int getprotobyname(string name)
   Returns protocol number associated with name as per /etc/protocols */
PHP_FUNCTION(getprotobyname)
{
	pval **name;
	struct protoent *ent;

	if (ZEND_NUM_ARGS() != 1
		|| zend_get_parameters_ex(1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(name);

	ent = getprotobyname(Z_STRVAL_PP(name));

	if (ent == NULL) {
		Z_LVAL_P(return_value) = -1;
		Z_TYPE_P(return_value) = IS_LONG;
		RETURN_FALSE;
	}

	RETURN_LONG(ent->p_proto);
}
/* }}} */
#endif

#if HAVE_GETPROTOBYNUMBER
/* {{{ proto string getprotobynumber(int proto)
   Returns protocol name associated with protocol number proto */
PHP_FUNCTION(getprotobynumber)
{
	pval **proto;
	struct protoent *ent;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &proto) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(proto);

	ent = getprotobynumber(Z_LVAL_PP(proto));

	if (ent == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(ent->p_name, 1);
}
/* }}} */
#endif

/* {{{ proto bool register_tick_function(string function_name [, mixed arg [, mixed ... ]])
   Registers a tick callback function */
PHP_FUNCTION(register_tick_function)
{
	user_tick_function_entry tick_fe;
	int i;

	tick_fe.calling = 0;
	tick_fe.arg_count = ZEND_NUM_ARGS();
	if (tick_fe.arg_count < 1) {
		WRONG_PARAM_COUNT;
	}

	tick_fe.arguments = (zval **) emalloc(sizeof(zval *) * tick_fe.arg_count);

	if (zend_get_parameters_array(ht, tick_fe.arg_count, tick_fe.arguments) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(tick_fe.arguments[0]) != IS_ARRAY)
		convert_to_string_ex(&tick_fe.arguments[0]);

	if (!BG(user_tick_functions)) {
		BG(user_tick_functions) = (zend_llist *) emalloc(sizeof(zend_llist));
		zend_llist_init(BG(user_tick_functions),
						sizeof(user_tick_function_entry),
						(llist_dtor_func_t) user_tick_function_dtor, 0);
		php_add_tick_function(run_user_tick_functions);
	}

	for (i = 0; i < tick_fe.arg_count; i++) {
		tick_fe.arguments[i]->refcount++;
	}

	zend_llist_add_element(BG(user_tick_functions), &tick_fe);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void unregister_tick_function(string function_name)
   Unregisters a tick callback function */
PHP_FUNCTION(unregister_tick_function)
{
	zval **function;
	user_tick_function_entry tick_fe;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &function)) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(function) != IS_ARRAY) {
		convert_to_string_ex(function);
	}
	
	tick_fe.arguments = (zval **) emalloc(sizeof(zval *));
	tick_fe.arguments[0] = *function;
	tick_fe.arg_count = 1;
	zend_llist_del_element(BG(user_tick_functions), &tick_fe, (int (*)(void *, void *)) user_tick_function_compare);
	efree(tick_fe.arguments);
}
/* }}} */

/* {{{ proto bool is_uploaded_file(string path)
   Check if file was created by rfc1867 upload  */
PHP_FUNCTION(is_uploaded_file)
{
	zval **path;

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &path) != SUCCESS) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(path);

	if (zend_hash_exists(SG(rfc1867_uploaded_files), Z_STRVAL_PP(path), Z_STRLEN_PP(path)+1)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool move_uploaded_file(string path, string new_path)
   Move a file if and only if it was created by an upload */
PHP_FUNCTION(move_uploaded_file)
{
	zval **path, **new_path;
	zend_bool successful = 0;

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &path, &new_path) != SUCCESS) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(path);
	convert_to_string_ex(new_path);

	if (!zend_hash_exists(SG(rfc1867_uploaded_files), Z_STRVAL_PP(path), Z_STRLEN_PP(path)+1)) {
		RETURN_FALSE;
	}

	if (PG(safe_mode) && (!php_checkuid(Z_STRVAL_PP(new_path), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(new_path) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	VCWD_UNLINK(Z_STRVAL_PP(new_path));
	if (rename(Z_STRVAL_PP(path), Z_STRVAL_PP(new_path)) == 0) {
		successful = 1;
	} else
		if (php_copy_file(Z_STRVAL_PP(path), Z_STRVAL_PP(new_path) TSRMLS_CC) == SUCCESS) {
		VCWD_UNLINK(Z_STRVAL_PP(path));
		successful = 1;
	}

	if (successful) {
		zend_hash_del(SG(rfc1867_uploaded_files), Z_STRVAL_PP(path), Z_STRLEN_PP(path)+1);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to move '%s' to '%s'", Z_STRVAL_PP(path), Z_STRVAL_PP(new_path));
	}
	RETURN_BOOL(successful);
}
/* }}} */


static void php_simple_ini_parser_cb(zval *arg1, zval *arg2, int callback_type, zval *arr)
{
	zval *element;

	switch (callback_type) {
	
		case ZEND_INI_PARSER_ENTRY:
			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}
			ALLOC_ZVAL(element);
			*element = *arg2;
			zval_copy_ctor(element);
			INIT_PZVAL(element);
			zend_hash_update(Z_ARRVAL_P(arr), Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)+1, &element, sizeof(zval *), NULL);
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
		{
			zval *hash, **find_hash;

			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}

			if (zend_hash_find(Z_ARRVAL_P(arr), Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)+1, (void **) &find_hash) == FAILURE) {
				ALLOC_ZVAL(hash);
				INIT_PZVAL(hash);
				array_init(hash);

				zend_hash_update(Z_ARRVAL_P(arr), Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)+1, &hash, sizeof(zval *), NULL);
			} else {
				hash = *find_hash;
			}

			ALLOC_ZVAL(element);
			*element = *arg2;
			zval_copy_ctor(element);
			INIT_PZVAL(element);
			add_next_index_zval(hash, element);			
		}
		break;

		case ZEND_INI_PARSER_SECTION:
			break;
	}
}

static void php_ini_parser_cb_with_sections(zval *arg1, zval *arg2, int callback_type, zval *arr)
{
	TSRMLS_FETCH();

	if (callback_type == ZEND_INI_PARSER_SECTION) {
		MAKE_STD_ZVAL(BG(active_ini_file_section));
		array_init(BG(active_ini_file_section));
		zend_hash_update(	Z_ARRVAL_P(arr),
							Z_STRVAL_P(arg1),
							Z_STRLEN_P(arg1)+1,
							&BG(active_ini_file_section),
							sizeof(zval *), NULL);
	} else if (arg2) {
		zval *active_arr;

		if (BG(active_ini_file_section)) {
			active_arr = BG(active_ini_file_section);
		} else {
			active_arr = arr;
		}

		php_simple_ini_parser_cb(arg1, arg2, callback_type, active_arr);
	}
}


/* {{{ proto array parse_ini_file(string filename [, bool process_sections])
   Parse configuration file */
PHP_FUNCTION(parse_ini_file)
{
	zval **filename, **process_sections;
	zend_file_handle fh;
	zend_ini_parser_cb_t ini_parser_cb;

	switch (ZEND_NUM_ARGS()) {

		case 1:
			if (zend_get_parameters_ex(1, &filename) == FAILURE) {
				RETURN_FALSE;
			}
			ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
			break;

		case 2:
			if (zend_get_parameters_ex(2, &filename, &process_sections) == FAILURE) {
				RETURN_FALSE;
			}
	
			convert_to_boolean_ex(process_sections);
		
			if (Z_BVAL_PP(process_sections)) {
				TSRMLS_FETCH();

				BG(active_ini_file_section) = NULL;
				ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
			} else {
				ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
			}
			break;
	
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}

	convert_to_string_ex(filename);

	memset(&fh, 0, sizeof(fh));
	fh.filename = Z_STRVAL_PP(filename);
	Z_TYPE(fh) = ZEND_HANDLE_FILENAME;
	
	array_init(return_value);
	zend_parse_ini_file(&fh, 0, ini_parser_cb, return_value);
}
/* }}} */

static int copy_request_variable(void *pDest, int num_args, va_list args, zend_hash_key *hash_key)
{
	char *prefix, *new_key;
	uint prefix_len, new_key_len;
	zval **var = (zval **) pDest;
	TSRMLS_FETCH();

	if (num_args != 2) {
		return 0;
	}

	prefix = va_arg(args, char *);
	prefix_len = va_arg(args, uint);

	new_key_len = prefix_len + hash_key->nKeyLength;
	new_key = (char *) emalloc(new_key_len);

	memcpy(new_key, prefix, prefix_len);
	memcpy(new_key+prefix_len, hash_key->arKey, hash_key->nKeyLength);

	zend_hash_del(&EG(symbol_table), new_key, new_key_len);
	ZEND_SET_SYMBOL_WITH_LENGTH(&EG(symbol_table), new_key, new_key_len, *var, (*var)->refcount+1, 0);

	efree(new_key);
	return 0;
}

/* {{{ proto bool import_request_variables(string types [, string prefix])
   Import GET/POST/Cookie variables into the global scope */
PHP_FUNCTION(import_request_variables)
{
	zval **z_types, **z_prefix;
	char *types, *prefix;
	uint prefix_len;
	char *p;

	switch (ZEND_NUM_ARGS()) {

		case 1:
			if (zend_get_parameters_ex(1, &z_types) == FAILURE) {
				RETURN_FALSE;
			}
			prefix = "";
			prefix_len = 0;
			break;

		case 2:
			if (zend_get_parameters_ex(2, &z_types, &z_prefix) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(z_prefix);
			prefix = Z_STRVAL_PP(z_prefix);
			prefix_len = Z_STRLEN_PP(z_prefix);
			break;
	
		default:
			ZEND_WRONG_PARAM_COUNT();
	}

	if (prefix_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "No prefix specified - possible security hazard");
	}

	convert_to_string_ex(z_types);
	types = Z_STRVAL_PP(z_types);

	for (p = types; p && *p; p++) {
		switch (*p) {
	
			case 'g':
			case 'G':
				zend_hash_apply_with_arguments(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_GET]), (apply_func_args_t) copy_request_variable, 2, prefix, prefix_len);
				break;
	
			case 'p':
			case 'P':
				zend_hash_apply_with_arguments(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_POST]), (apply_func_args_t) copy_request_variable, 2, prefix, prefix_len);
				zend_hash_apply_with_arguments(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_FILES]), (apply_func_args_t) copy_request_variable, 2, prefix, prefix_len);
				break;

			case 'c':
			case 'C':
				zend_hash_apply_with_arguments(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_COOKIE]), (apply_func_args_t) copy_request_variable, 2, prefix, prefix_len);
				break;
		}
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

