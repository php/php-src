/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


#include "php.h"
#include "main.h"
#include "php_ini.h"
#include "internal_functions_registry.h"
#include "php_standard.h"
#include "zend_operators.h"
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <netdb.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#include "safe_mode.h"
#if WIN32|WINNT
#include "win32/unistd.h"
#endif
#include "zend_globals.h"

#include "php_globals.h"
#include "SAPI.h"


#ifdef ZTS
int basic_globals_id;
#else
php_basic_globals basic_globals;
#endif

static unsigned char second_and_third_args_force_ref[] = { 3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
/* uncomment this if/when we actually need it - tired of seeing the warning
static unsigned char third_and_fourth_args_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
*/
;

typedef struct _php_shutdown_function_entry {
	zval **arguments;
	int arg_count;
} php_shutdown_function_entry;

/* some prototypes for local functions */
static int user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry);
pval test_class_get_property(zend_property_reference *property_reference);
int test_class_set_property(zend_property_reference *property_reference, pval *value);
void test_class_call_function(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);

function_entry basic_functions[] = {
	PHP_FE(intval,									NULL)
	PHP_FE(doubleval,								NULL)
	PHP_FE(strval,									NULL)
	PHP_FE(bin2hex, 								NULL)
	PHP_FE(sleep,									NULL)
	PHP_FE(usleep,									NULL)
		
	PHP_FE(time,									NULL)
	PHP_FE(mktime,									NULL)
	PHP_FE(gmmktime,								NULL)
#if HAVE_STRFTIME
	PHP_FE(strftime,								NULL)
#endif
	PHP_FE(strtotime,								NULL)
	PHP_FE(date,									NULL)
	PHP_FE(gmdate,									NULL)
	PHP_FE(getdate,									NULL)
	PHP_FE(checkdate,								NULL)

	PHP_FE(flush,									NULL)
	
	PHP_FE(gettype,									NULL)
	PHP_FE(settype,									first_arg_force_ref)
	
	PHP_FE(getimagesize,							NULL)
	
	PHP_FE(htmlspecialchars,						NULL)
	PHP_FE(htmlentities,							NULL)
	PHP_FE(get_html_translation_table,				NULL)
	
	PHP_NAMED_FE(md5,		php_if_md5,				NULL)

	PHP_FE(iptcparse,								NULL)
	PHP_FE(iptcembed,								NULL)
		
	PHP_FE(phpinfo,									NULL)
	PHP_FE(phpversion,								NULL)
	PHP_FE(phpcredits,								NULL)
	
	PHP_FE(strspn,									NULL)
	PHP_FE(strcspn,									NULL)
	PHP_FE(strtok,									NULL)
	PHP_FE(strtoupper,								NULL)
	PHP_FE(strtolower,								NULL)
	PHP_FE(strpos,									NULL)
	PHP_FE(strrpos,									NULL)
	PHP_FE(strrev,									NULL)
	PHP_FE(hebrev,									NULL)
	PHP_FE(hebrevc,									NULL)
	PHP_FE(nl2br,									NULL)
	PHP_FE(basename,								NULL)
	PHP_FE(dirname,									NULL)
	PHP_FE(stripslashes,							NULL)
	PHP_FE(stripcslashes,							NULL)
	PHP_FE(strstr,									NULL)
	PHP_FE(stristr,									NULL)
	PHP_FE(strrchr,									NULL)
	PHP_FE(substr,									NULL)
	PHP_FE(substr_replace,							NULL)
	PHP_FE(quotemeta,								NULL)
	PHP_FE(ucfirst,									NULL)
	PHP_FE(ucwords,									NULL)
	PHP_FE(strtr,									NULL)
	PHP_FE(addslashes,								NULL)
	PHP_FE(addcslashes,								NULL)
	PHP_FE(chop,									NULL)
	PHP_FE(str_replace,								NULL)
	PHP_FE(str_repeat,								NULL)
	PHP_FE(count_chars,								NULL)
	PHP_FE(chunk_split,								NULL)
	PHP_FE(trim,									NULL)
	PHP_FE(ltrim,									NULL)
	PHP_FE(strip_tags,								NULL)
	PHP_FE(similar_text,							NULL)
	PHP_FE(explode,									NULL)
	PHP_FE(implode,									NULL)
	PHP_FE(setlocale,								NULL)
	PHP_FE(soundex,									NULL)
	PHP_FE(chr,										NULL)
	PHP_FE(ord,										NULL)
	PHP_FE(parse_str,								NULL)
	PHP_FALIAS(rtrim,			chop,				NULL)
	PHP_FALIAS(strchr,			strstr,				NULL)
	PHP_NAMED_FE(sprintf,		PHP_FN(user_sprintf),	NULL)
	PHP_NAMED_FE(printf,		PHP_FN(user_printf),	NULL)
	
	PHP_FE(parse_url,								NULL)
	PHP_FE(urlencode,								NULL)
	PHP_FE(urldecode,								NULL)
	PHP_FE(rawurlencode,							NULL)
	PHP_FE(rawurldecode,							NULL)

	PHP_FE(readlink,								NULL)
	PHP_FE(linkinfo,								NULL)
	PHP_FE(symlink,									NULL)
	PHP_FE(link,									NULL)
	PHP_FE(unlink,									NULL)
	
	PHP_FE(exec, 									second_and_third_args_force_ref)
	PHP_FE(system, 									second_arg_force_ref)
	PHP_FE(escapeshellcmd, 							NULL)
	PHP_FE(passthru, 								second_arg_force_ref)
	PHP_FE(shell_exec, 								NULL)

	PHP_FE(rand,									NULL)
	PHP_FE(srand,									NULL)
	PHP_FE(getrandmax,								NULL)
	PHP_FE(mt_rand,									NULL)
	PHP_FE(mt_srand,								NULL)
	PHP_FE(mt_getrandmax,							NULL)
	PHP_FE(getservbyname, NULL)
	PHP_FE(getservbyport, NULL)	
	PHP_FE(getprotobyname, NULL)
	PHP_FE(getprotobynumber, NULL)
	PHP_FE(gethostbyaddr,							NULL)
	PHP_FE(gethostbyname,							NULL)
	PHP_FE(gethostbynamel,							NULL)
#if !(WIN32|WINNT)||HAVE_BINDLIB
	PHP_FE(checkdnsrr,								NULL)
	PHP_FE(getmxrr,									second_and_third_args_force_ref)
#endif

	PHP_FE(getmyuid,								NULL)
	PHP_FE(getmypid,								NULL)
	PHP_FE(getmyinode,								NULL)
	PHP_FE(getlastmod,								NULL)

	PHP_FE(base64_decode,							NULL)
	PHP_FE(base64_encode,							NULL)

	PHP_FE(abs,										NULL)
	PHP_FE(ceil,									NULL)
	PHP_FE(floor,									NULL)
	PHP_FE(round,									NULL)
	PHP_FE(sin,										NULL)
	PHP_FE(cos,										NULL)
	PHP_FE(tan,										NULL)
	PHP_FE(asin,									NULL)
	PHP_FE(acos,									NULL)
	PHP_FE(atan,									NULL)
	PHP_FE(atan2,									NULL)
	PHP_FE(pi,										NULL)
	PHP_FE(pow,										NULL)
	PHP_FE(exp,										NULL)
	PHP_FE(log,										NULL)
	PHP_FE(log10,									NULL)
	PHP_FE(sqrt,									NULL)
	PHP_FE(deg2rad,									NULL)
	PHP_FE(rad2deg,									NULL)
	PHP_FE(bindec,									NULL)
	PHP_FE(hexdec,									NULL)
	PHP_FE(octdec,									NULL)
	PHP_FE(decbin,									NULL)
	PHP_FE(decoct,									NULL)
	PHP_FE(dechex,									NULL)
	PHP_FE(base_convert,							NULL)
	PHP_FE(number_format,							NULL)

	PHP_FE(getenv,									NULL)
#ifdef HAVE_PUTENV
	PHP_FE(putenv,									NULL)
#endif

	PHP_FE(microtime,								NULL)
	PHP_FE(gettimeofday,							NULL)
	PHP_FE(getrusage,								NULL)
	
	PHP_FE(uniqid,									NULL)
		
	PHP_FE(quoted_printable_decode,					NULL)
	
	PHP_FE(convert_cyr_string,						NULL)
	PHP_FE(get_current_user,						NULL)
	PHP_FE(set_time_limit,							NULL)
	
	PHP_FE(get_cfg_var,								NULL)
	PHP_FALIAS(magic_quotes_runtime, set_magic_quotes_runtime,	NULL)
	PHP_FE(set_magic_quotes_runtime,				NULL)
	PHP_FE(get_magic_quotes_gpc,					NULL)
	PHP_FE(get_magic_quotes_runtime,				NULL)
	
	PHP_FE(is_resource,								first_arg_allow_ref)
	PHP_FE(is_bool,									first_arg_allow_ref)
	PHP_FE(is_long,									first_arg_allow_ref)
	PHP_FALIAS(is_int,			is_long,			first_arg_allow_ref)
	PHP_FALIAS(is_integer,		is_long,			first_arg_allow_ref)
	PHP_FALIAS(is_float,		is_double,			first_arg_allow_ref)
	PHP_FE(is_double,								first_arg_allow_ref)
	PHP_FALIAS(is_real,			is_double,			first_arg_allow_ref)
	PHP_FE(is_string,								first_arg_allow_ref)
	PHP_FE(is_array,								first_arg_allow_ref)
	PHP_FE(is_object,								first_arg_allow_ref)

	PHP_FE(error_log,								NULL)
	PHP_FE(call_user_func,							NULL)
	PHP_FE(call_user_method,						NULL)

	PHP_FE(var_dump,								NULL)
	PHP_FE(serialize,								first_arg_allow_ref)
	PHP_FE(unserialize,								first_arg_allow_ref)
	
	PHP_FE(register_shutdown_function,	NULL)

	PHP_FE(highlight_file,				NULL)	
	PHP_NAMED_FE(show_source, PHP_FN(highlight_file), NULL)
	PHP_FE(highlight_string,			NULL)
	
	PHP_FE(ini_get,						NULL)
	PHP_FE(ini_alter,					NULL)
	PHP_FE(ini_restore,					NULL)

	PHP_FE(print_r,					NULL)
	
	PHP_FE(setcookie,								NULL)
	PHP_NAMED_FE(header,		PHP_FN(Header),							NULL)
	PHP_FE(headers_sent,							NULL)
	
	PHP_FE(connection_aborted,			NULL)
	PHP_FE(connection_timeout,			NULL)
	PHP_FE(connection_status,			NULL)
	PHP_FE(ignore_user_abort,			NULL)

	PHP_FE(get_loaded_extensions,		NULL)
	PHP_FE(extension_loaded,			NULL)
	PHP_FE(get_extension_funcs,			NULL)
	
	{NULL, NULL, NULL}
};


PHP_INI_BEGIN()
	PHP_INI_ENTRY1("highlight.string",	"#foobar",	PHP_INI_ALL,		NULL,		NULL)
	PHP_INI_ENTRY1("test2",				"testing",	PHP_INI_SYSTEM,		NULL,		NULL)
PHP_INI_END()


zend_module_entry basic_functions_module = {
	"Basic Functions",			/* extension name */
	basic_functions,			/* function list */
	PHP_MINIT(basic),			/* process startup */
	PHP_MSHUTDOWN(basic),		/* process shutdown */
	PHP_RINIT(basic),			/* request startup */
	PHP_RSHUTDOWN(basic),		/* request shutdown */
	NULL,						/* extension info */
	STANDARD_MODULE_PROPERTIES
};

#if defined(HAVE_PUTENV)

static int php_putenv_destructor(putenv_entry *pe)
{
	if (pe->previous_value) {
		putenv(pe->previous_value);
	} else {
# if HAVE_UNSETENV
		unsetenv(pe->key);
# else
		char **env;
		
		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env,pe->key,pe->key_len) && (*env)[pe->key_len]=='=') {	/* found it */
				*env = "";
				break;
			}
		}
# endif
	}
	efree(pe->putenv_string);
	efree(pe->key);
	return 1;
}
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void test_class_startup(void);

static void basic_globals_ctor(BLS_D)
{
	BG(next) = NULL;
	BG(left) = -1;
}

PHP_MINIT_FUNCTION(basic)
{
	ELS_FETCH();
#ifdef ZTS
	basic_globals_id = ts_allocate_id(sizeof(php_basic_globals), (ts_allocate_ctor) basic_globals_ctor, NULL);
#else
	basic_globals_ctor(BLS_C);
#endif
	
	REGISTER_DOUBLE_CONSTANT("M_PI", M_PI, CONST_CS | CONST_PERSISTENT);
	
	test_class_startup();
	REGISTER_INI_ENTRIES();

	register_phpinfo_constants(INIT_FUNC_ARGS_PASSTHRU);
	register_html_constants(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(basic)
{
#ifdef ZTS
	ts_free_id(basic_globals_id);
#endif
	
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;	
}


PHP_RINIT_FUNCTION(basic)
{
	BLS_FETCH();

	BG(strtok_string) = NULL;
	BG(locale_string) = NULL;
	BG(user_compare_func_name) = NULL;
	BG(array_walk_func_name) = NULL;
	BG(page_uid) = -1;
	BG(page_inode) = -1;
	BG(page_mtime) = -1;
#ifdef HAVE_PUTENV
	if (zend_hash_init(&BG(putenv_ht), 1, NULL, (int (*)(void *)) php_putenv_destructor, 0) == FAILURE) {
		return FAILURE;
	}
#endif
	BG(user_shutdown_function_names)=NULL;
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(basic)
{
	BLS_FETCH();

	STR_FREE(BG(strtok_string));
#ifdef HAVE_PUTENV
	zend_hash_destroy(&BG(putenv_ht));
#endif
	/* Check if locale was changed and change it back
	   to the value in startup environment */
	if (BG(locale_string) != NULL) {
		setlocale(LC_ALL, "");
	}
	STR_FREE(BG(locale_string));

	return SUCCESS;
}

/********************
 * System Functions *
 ********************/

PHP_FUNCTION(getenv)
{
#if FHTTPD
	int i;
#endif
	pval **str;
	char *ptr;
#if APACHE
	SLS_FETCH();
#endif

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

#if FHTTPD
	ptr=NULL;
	if ((*str)->type == IS_STRING && req){
		for(i=0;i<req->nlines;i++){
			if (req->lines[i].paramc>1){
				if (req->lines[i].params[0]){
					if (!strcmp(req->lines[i].params[0],
								(*str)->value.str.val)){
						ptr=req->lines[i].params[1];
						i=req->nlines;
					}
				}
			}
		}
	}
	if (!ptr) ptr = getenv((*str)->value.str.val);
	if (ptr
#else

	if ((*str)->type == IS_STRING &&
#if APACHE
		((ptr = (char *)table_get(((request_rec *) SG(server_context))->subprocess_env, (*str)->value.str.val)) || (ptr = getenv((*str)->value.str.val)))
#endif
#if CGI_BINARY
		(ptr = getenv((*str)->value.str.val))
#endif

#if USE_SAPI
		(ptr = sapi_rqst->getenv(sapi_rqst->scid,(*str)->value.str.val))
#endif
#endif
		) {
		RETURN_STRING(ptr,1);
	}
	RETURN_FALSE;
}


#ifdef HAVE_PUTENV
PHP_FUNCTION(putenv)
{
	pval **str;
	BLS_FETCH();

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if ((*str)->value.str.val && *((*str)->value.str.val)) {
		int ret;
		char *p,**env;
		putenv_entry pe;
		
		pe.putenv_string = estrndup((*str)->value.str.val,(*str)->value.str.len);
		pe.key = (*str)->value.str.val;
		if ((p=strchr(pe.key,'='))) { /* nullify the '=' if there is one */
			*p='\0';
		}
		pe.key_len = strlen(pe.key);
		pe.key = estrndup(pe.key,pe.key_len);
		
		zend_hash_del(&BG(putenv_ht),pe.key,pe.key_len+1);
		
		/* find previous value */
		pe.previous_value = NULL;
		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env,pe.key,pe.key_len) && (*env)[pe.key_len]=='=') {	/* found it */
				pe.previous_value = *env;
				break;
			}
		}

		if ((ret=putenv(pe.putenv_string))==0) { /* success */
			zend_hash_add(&BG(putenv_ht),pe.key,pe.key_len+1,(void **) &pe,sizeof(putenv_entry),NULL);
			RETURN_TRUE;
		} else {
			efree(pe.putenv_string);
			efree(pe.key);
			RETURN_FALSE;
		}
	}
}
#endif


/*******************
 * Basic Functions *
 *******************/

PHP_FUNCTION(intval)
{
	pval **num, **arg_base;
	int base;
	
	switch(ARG_COUNT(ht)) {
	case 1:
		if (zend_get_parameters_ex(1, &num) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		base = 10;
		break;
	case 2:
		if (zend_get_parameters_ex(2, &num, &arg_base) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg_base);
		base = (*arg_base)->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	*return_value=**num;
	zval_copy_ctor(return_value);
	convert_to_long_base(return_value,base);
}


PHP_FUNCTION(doubleval)
{
	pval **num;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	*return_value=**num;
	zval_copy_ctor(return_value);
	convert_to_double(return_value);
}

 
PHP_FUNCTION(strval)
{
	pval **num;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	*return_value=**num;
	zval_copy_ctor(return_value);
	convert_to_string(return_value);
}

PHP_FUNCTION(flush)
{
#if APACHE 
	SLS_FETCH();
#endif
	
#if APACHE
#  if MODULE_MAGIC_NUMBER > 19970110
	rflush(((request_rec *) SG(server_context)));
#  else
	bflush(((request_rec *) SG(server_context))->connection->client);
#  endif
#endif
#if FHTTPD
       /*FIXME -- what does it flush really? the whole response?*/
#endif
#if CGI_BINARY
	fflush(stdout);
#endif
#if USE_SAPI
	sapi_rqst->flush(sapi_rqst->scid);
#endif
}


PHP_FUNCTION(sleep)
{
	pval **num;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	sleep((*num)->value.lval);
}

PHP_FUNCTION(usleep)
{
#if HAVE_USLEEP
	pval **num;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	usleep((*num)->value.lval);
#endif
}

PHP_FUNCTION(gettype)
{
	pval **arg;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch ((*arg)->type) {
		case IS_BOOL:
			RETVAL_STRING("boolean",1);
			break;
		case IS_LONG:
			RETVAL_STRING("integer",1);
			break;
		case IS_RESOURCE:
			RETVAL_STRING("resource",1);
			break;
		case IS_DOUBLE:
			RETVAL_STRING("double",1);
			break;
		case IS_STRING:
			RETVAL_STRING("string",1);
			break;
		case IS_ARRAY:
			RETVAL_STRING("array",1);
			break;
		case IS_OBJECT:
			RETVAL_STRING("object",1);
			break;
			/*
			{
				char *result;
				int res_len;
				
				res_len = sizeof("object of type ")-1 + arg->value.obj.ce->name_length;
				result = (char *) emalloc(res_len+1);
				sprintf(result, "object of type %s", arg->value.obj.ce->name);
				RETVAL_STRINGL(result, res_len, 0);
			}
			*/
			break;
		default:
			RETVAL_STRING("unknown type",1);
	}
}


PHP_FUNCTION(settype)
{
	pval **var, **type;
	char *new_type;

	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &var, &type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(type);
	new_type = (*type)->value.str.val;

	if (!strcasecmp(new_type, "integer")) {
		convert_to_long(*var);
	} else if (!strcasecmp(new_type, "double")) {
		convert_to_double(*var);
	} else if (!strcasecmp(new_type, "string")) {
		convert_to_string(*var);
	} else if (!strcasecmp(new_type, "array")) {
		convert_to_array(*var);
	} else if (!strcasecmp(new_type, "object")) {
		convert_to_object(*var);
	} else if (!strcasecmp(new_type, "boolean")) {
		convert_to_boolean(*var);
	} else if (!strcasecmp(new_type, "resource")) {
		php_error(E_WARNING, "settype: cannot convert to resource type");
		RETURN_FALSE;
	} else {
		php_error(E_WARNING, "settype: invalid type");
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}


PHP_FUNCTION(get_current_user)
{
	RETURN_STRING(php_get_current_user(),1);
}


PHP_FUNCTION(get_cfg_var)
{
	pval **varname;
	char *value;
	
	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &varname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(varname);
	
	if (cfg_get_string((*varname)->value.str.val,&value)==FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRING(value,1);
}

PHP_FUNCTION(set_magic_quotes_runtime)
{
	pval **new_setting;
	PLS_FETCH();
	
	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &new_setting)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_boolean_ex(new_setting);
	
	PG(magic_quotes_runtime) = (zend_bool) (*new_setting)->value.lval;
	RETURN_TRUE;
}
	
PHP_FUNCTION(get_magic_quotes_runtime)
{
	PLS_FETCH();

	RETURN_LONG(PG(magic_quotes_runtime));
}

PHP_FUNCTION(get_magic_quotes_gpc)
{
	PLS_FETCH();

	RETURN_LONG(PG(magic_quotes_gpc));
}


void php_is_type(INTERNAL_FUNCTION_PARAMETERS,int type)
{
	pval **arg;
	
	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &arg)==FAILURE) {
		RETURN_FALSE;
	}
	if ((*arg)->type == type) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}


PHP_FUNCTION(is_resource) 
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_RESOURCE);
}

PHP_FUNCTION(is_bool) 
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_BOOL);
}

PHP_FUNCTION(is_long) 
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_LONG);
}

PHP_FUNCTION(is_double)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_DOUBLE);
}

PHP_FUNCTION(is_string)
{ 
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_STRING);
}

PHP_FUNCTION(is_array)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_ARRAY);
}

PHP_FUNCTION(is_object)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_OBJECT);
}


/* 
	1st arg = error message
	2nd arg = error option
	3rd arg = optional parameters (email address or tcp address)
	4th arg = used for additional headers if email

  error options
    0 = send to php_error_log (uses syslog or file depending on ini setting)
	1 = send via email to 3rd parameter 4th option = additional headers
	2 = send via tcp/ip to 3rd parameter (name or ip:port)
	3 = save to file in 3rd parameter
*/

PHP_FUNCTION(error_log)
{
	pval **string, **erropt = NULL, **option = NULL, **emailhead = NULL;
	int opt_err = 0;
	char *message, *opt=NULL, *headers=NULL;

	switch(ARG_COUNT(ht)) {
	case 1:
		if (zend_get_parameters_ex(1,&string) == FAILURE) {
			php_error(E_WARNING,"Invalid argument 1 in error_log");
			RETURN_FALSE;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2,&string,&erropt) == FAILURE) {
			php_error(E_WARNING,"Invalid arguments in error_log");
			RETURN_FALSE;
		}
		convert_to_long_ex(erropt);
		opt_err=(*erropt)->value.lval;
		break;
	case 3:
		if (zend_get_parameters_ex(3,&string,&erropt,&option) == FAILURE){
			php_error(E_WARNING,"Invalid arguments in error_log");
			RETURN_FALSE;
		}
		convert_to_long_ex(erropt);
		opt_err=(*erropt)->value.lval;
		convert_to_string_ex(option);
		opt=(*option)->value.str.val;
		break;
	case 4:
		if (zend_get_parameters_ex(4,&string,&erropt,&option,&emailhead) == FAILURE){
			php_error(E_WARNING,"Invalid arguments in error_log");
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(string);
	message=(*string)->value.str.val;
	if (erropt != NULL) {
		convert_to_long_ex(erropt);
		opt_err=(*erropt)->value.lval;
	}
	if (option != NULL) {
		convert_to_string_ex(option);
		opt=(*option)->value.str.val;
	}
	if (emailhead != NULL) {
		convert_to_string_ex(emailhead);
		headers=(*emailhead)->value.str.val;
	}

	if (_php_error_log(opt_err,message,opt,headers)==FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

PHPAPI int _php_error_log(int opt_err,char *message,char *opt,char *headers){
	FILE *logfile;
	int issock=0, socketd=0;;

	switch(opt_err){
	case 1: /*send an email*/
		{
#if HAVE_SENDMAIL
		if (!php_mail(opt,"PHP error_log message",message,headers)){
			return FAILURE;
		}
#else
		php_error(E_WARNING,"Mail option not available!");
		return FAILURE;
#endif
		}
		break;
	case 2: /*send to an address */
		php_error(E_WARNING,"TCP/IP option not available!");
		return FAILURE;
		break;
	case 3: /*save to a file*/
		logfile=php_fopen_wrapper(opt,"a", (IGNORE_URL|ENFORCE_SAFE_MODE), &issock, &socketd, NULL);
		if(!logfile) {
			php_error(E_WARNING,"error_log: Unable to write to %s",opt);
			return FAILURE;
		}
		fwrite(message,strlen(message),1,logfile);
		fclose(logfile);
		break;
	default:
		php_log_err(message);
		break;
	}
	return SUCCESS;
}


PHP_FUNCTION(call_user_func)
{
	pval ***params;
	pval *retval_ptr;
	int arg_count=ARG_COUNT(ht);
	CLS_FETCH();
	
	if (arg_count<1) {
		WRONG_PARAM_COUNT;
	}
	params = (pval ***) emalloc(sizeof(pval **)*arg_count);
	
	if (zend_get_parameters_array_ex(arg_count, params)==FAILURE) {
		efree(params);
		RETURN_FALSE;
	}
	SEPARATE_ZVAL(params[0]);
	convert_to_string(*params[0]);
	if (call_user_function_ex(CG(function_table), NULL, *params[0], &retval_ptr, arg_count-1, params+1, 1)==SUCCESS
		&& retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		php_error(E_WARNING,"Unable to call %s() - function does not exist", (*params[0])->value.str.val);
	}
	efree(params);
}


PHP_FUNCTION(call_user_method)
{
	pval ***params;
	pval *retval_ptr;
	int arg_count=ARG_COUNT(ht);
	CLS_FETCH();
	
	if (arg_count<2) {
		WRONG_PARAM_COUNT;
	}
	params = (pval ***) emalloc(sizeof(pval **)*arg_count);
	
	if (zend_get_parameters_array_ex(arg_count, params)==FAILURE) {
		efree(params);
		RETURN_FALSE;
	}
	if ((*params[1])->type != IS_OBJECT) {
		php_error(E_WARNING,"2nd argument is not an object\n");
		efree(params);
		RETURN_FALSE;
	}
	SEPARATE_ZVAL(params[0]);
	SEPARATE_ZVAL(params[1]);
	convert_to_string(*params[0]);
	if (call_user_function_ex(CG(function_table), *params[1], *params[0], &retval_ptr, arg_count-2, params+2, 1)==SUCCESS
		&& retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		php_error(E_WARNING,"Unable to call %s() - function does not exist", (*params[0])->value.str.val);
	}
	efree(params);
}


int user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry)
{
	pval retval;
	int i;
	CLS_FETCH();

	if (call_user_function(CG(function_table), NULL, shutdown_function_entry->arguments[0], &retval, shutdown_function_entry->arg_count-1, shutdown_function_entry->arguments+1)==SUCCESS) {
		pval_destructor(&retval);
	}
	for (i=0; i<shutdown_function_entry->arg_count; i++) {
		zval_ptr_dtor(&shutdown_function_entry->arguments[i]);
	}
	efree(shutdown_function_entry->arguments);
	return 1;
}


void php_call_shutdown_functions(void)
{
	BLS_FETCH();
	
	if (BG(user_shutdown_function_names)) {
		zend_hash_destroy(BG(user_shutdown_function_names));
		efree(BG(user_shutdown_function_names));
	}
}

/* {{{ proto void register_shutdown_function(string function_name)
   Register a user-level function to be called on request termination */
PHP_FUNCTION(register_shutdown_function)
{
	php_shutdown_function_entry shutdown_function_entry;
	int i;
	BLS_FETCH();

	shutdown_function_entry.arg_count = ARG_COUNT(ht);

	if (shutdown_function_entry.arg_count<1) {
		WRONG_PARAM_COUNT;
	}
	shutdown_function_entry.arguments = (pval **) emalloc(sizeof(pval *)*shutdown_function_entry.arg_count);

	if (zend_get_parameters_array(ht, shutdown_function_entry.arg_count, shutdown_function_entry.arguments)==FAILURE) {
		RETURN_FALSE;
	}	
	convert_to_string(shutdown_function_entry.arguments[0]);
	if (!BG(user_shutdown_function_names)) {
		BG(user_shutdown_function_names) = (HashTable *) emalloc(sizeof(HashTable));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, (int (*)(void *))user_shutdown_function_dtor, 0);
	}

	for (i=0; i<shutdown_function_entry.arg_count; i++) {
		shutdown_function_entry.arguments[i]->refcount++;
	}
	zend_hash_next_index_insert(BG(user_shutdown_function_names), &shutdown_function_entry, sizeof(php_shutdown_function_entry), NULL);
}
/* }}} */


ZEND_API void php_get_highlight_struct(zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	syntax_highlighter_ini->highlight_comment = INI_STR("highlight.comment");
	syntax_highlighter_ini->highlight_default = INI_STR("highlight.default");
	syntax_highlighter_ini->highlight_html = INI_STR("highlight.html");
	syntax_highlighter_ini->highlight_keyword = INI_STR("highlight.keyword");
	syntax_highlighter_ini->highlight_string = INI_STR("highlight.string");
}


/* {{{ proto void highlight_file(string file_name)
   Syntax highlight a source file */
PHP_FUNCTION(highlight_file)
{
	pval **filename;
	zend_syntax_highlighter_ini syntax_highlighter_ini;

	
	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &filename)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	php_get_highlight_struct(&syntax_highlighter_ini);

	if (highlight_file((*filename)->value.str.val, &syntax_highlighter_ini)==FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto void highlight_string(string string)
   Syntax highlight a string */
PHP_FUNCTION(highlight_string)
{
	pval **expr;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	
	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &expr)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(expr);

	php_get_highlight_struct(&syntax_highlighter_ini);

	if (highlight_string(*expr, &syntax_highlighter_ini)==FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


pval test_class_get_property(zend_property_reference *property_reference)
{
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;


	printf("Reading a property from a OverloadedTestClass object:\n");

	for (element=property_reference->elements_list.head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case IS_ARRAY:
				printf("Array offset:  ");
				break;
			case IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%ld (numeric)\n", overloaded_property->element.value.lval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
				break;
		}
		pval_destructor(&overloaded_property->element);
	}
		
	result.value.str.val = estrndup("testing", 7);
	result.value.str.len = 7;
	result.type = IS_STRING;
	return result;
}


int test_class_set_property(zend_property_reference *property_reference, pval *value)
{
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Writing to a property from a OverloadedTestClass object:\n");
	printf("Writing '");
	zend_print_variable(value);
	printf("'\n");

	for (element=property_reference->elements_list.head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case IS_ARRAY:
				printf("Array offset:  ");
				break;
			case IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%ld (numeric)\n", overloaded_property->element.value.lval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
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

	for (element=property_reference->elements_list.head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case IS_ARRAY:
				printf("Array offset:  ");
				break;
			case IS_OBJECT:
				printf("Object property:  ");
				break;
			case IS_METHOD:
				printf("Overloaded method:  ");
		}
		switch (overloaded_property->element.type) {
			case IS_LONG:
				printf("%ld (numeric)\n", overloaded_property->element.value.lval);
				break;
			case IS_STRING:
				printf("'%s'\n", overloaded_property->element.value.str.val);
				break;
		}
		pval_destructor(&overloaded_property->element);
	}
		
	printf("%d arguments\n", ARG_COUNT(ht));
	return_value->value.str.val = estrndup("testing", 7);
	return_value->value.str.len = 7;
	return_value->type = IS_STRING;
}


void test_class_startup()
{
	zend_class_entry test_class_entry;

	INIT_OVERLOADED_CLASS_ENTRY(test_class_entry, "OverloadedTestClass", NULL,
								test_class_call_function,
								test_class_get_property,
								test_class_set_property);

	register_internal_class(&test_class_entry);
}


PHP_FUNCTION(ini_get)
{
	pval **varname;

	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &varname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);

	return_value->value.str.val = php_ini_string((*varname)->value.str.val, (*varname)->value.str.len+1, 0);

	if (!return_value->value.str.val) {
		RETURN_FALSE;
	}

	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
	pval_copy_constructor(return_value);
}


PHP_FUNCTION(ini_alter)
{
	pval **varname, **new_value;
	char *old_value;

	if (ARG_COUNT(ht)!=2 || zend_get_parameters_ex(2, &varname, &new_value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);
	convert_to_string_ex(new_value);

	old_value = php_ini_string((*varname)->value.str.val, (*varname)->value.str.len+1, 0);

	if (php_alter_ini_entry((*varname)->value.str.val, (*varname)->value.str.len+1, (*new_value)->value.str.val, (*new_value)->value.str.len, PHP_INI_USER)==FAILURE) {
		RETURN_FALSE;
	}
	if (old_value) {
		RETURN_STRING(old_value, 1);
	} else {
		RETURN_FALSE;
	}
}



PHP_FUNCTION(ini_restore)
{
	pval **varname;

	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &varname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);

	php_restore_ini_entry((*varname)->value.str.val, (*varname)->value.str.len);
}


PHP_FUNCTION(print_r)
{
	pval **expr;

	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &expr)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	zend_print_pval_r(*expr, 0);

	RETURN_TRUE;
}


/* This should go back to PHP */

/* {{{ proto int connection_aborted(void)
   Returns true if client disconnected */
PHP_FUNCTION(connection_aborted)
{
	PLS_FETCH();

    RETURN_LONG(PG(connection_status)&PHP_CONNECTION_ABORTED);
}
/* }}} */

/* {{{ proto int connection_timeout(void)
   Returns true if script timed out */
PHP_FUNCTION(connection_timeout)
{
	PLS_FETCH();

    RETURN_LONG(PG(connection_status)&PHP_CONNECTION_TIMEOUT);
}
/* }}} */

/* {{{ proto int connection_status(void)
   Returns the connection status bitfield */
PHP_FUNCTION(connection_status)
{
	PLS_FETCH();

    RETURN_LONG(PG(connection_status));
}
/* }}} */

/* {{{ proto int ignore_user_abort(boolean value)
   Set whether we want to ignore a user abort event or not */
PHP_FUNCTION(ignore_user_abort)
{
    pval **arg;
    int old_setting;
	PLS_FETCH();

    old_setting = PG(ignore_user_abort);
    switch (ARG_COUNT(ht)) {
        case 0:
            break;
        case 1:
            if (zend_get_parameters_ex(1, &arg) == FAILURE) {
                RETURN_FALSE;
            }
            convert_to_boolean_ex(arg);
            PG(ignore_user_abort) = (zend_bool) (*arg)->value.lval;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    RETURN_LONG(old_setting);
}
/* }}} */

/* {{{ proto int getservbyname(string service, string protocol)
   Returns port associated with service. protocol must be "tcp" or "udp". */
PHP_FUNCTION(getservbyname)
{
	pval **name,**proto;  
	struct servent *serv;

	if(ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2,&name,&proto) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	convert_to_string_ex(proto);

	serv = getservbyname((*name)->value.str.val,(*proto)->value.str.val);

	if(serv == NULL)
		RETURN_FALSE;

	RETURN_LONG(ntohs(serv->s_port));
}
/* }}} */


/* {{{ proto string getservbyport(int port, string protocol)
   Returns service name associated with port. Protocol must be "tcp" or "udp". */
PHP_FUNCTION(getservbyport)
{
	pval **port,**proto;
	struct servent *serv;

	if(ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2,&port,&proto) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(port);
	convert_to_string_ex(proto);

	serv = getservbyport(htons((unsigned short) (*port)->value.lval),(*proto)->value.str.val);

	if(serv == NULL)
		RETURN_FALSE;

	RETURN_STRING(serv->s_name,1);
}
/* }}} */


/* {{{ proto int getprotobyname(string name)
   Returns protocol number associated with name as per /etc/protocols. */
PHP_FUNCTION(getprotobyname)
{
	pval **name;
	struct protoent *ent;

	if(ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(name);
	
	ent = getprotobyname((*name)->value.str.val);

	if(ent == NULL) {
		return_value->value.lval = -1;
		return_value->type = IS_LONG;
		return;
	}

	RETURN_LONG(ent->p_proto);
}
/* }}} */
		

/* {{{ proto string getprotobynumber(int proto)
   Returns protocol name associated with protocol number proto. */
PHP_FUNCTION(getprotobynumber)
{
	pval **proto;
	struct protoent *ent;

	if(ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &proto) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(proto);

	ent = getprotobynumber((*proto)->value.lval);
	
	if(ent == NULL)
		RETURN_FALSE;

	RETURN_STRING(ent->p_name,1);
}
/* }}} */


static int php_add_extension_info(zend_module_entry *module, void *arg)
{
	zval *name_array = (zval *)arg;
	add_next_index_string(name_array, module->name, 1);
	return 0;
}

/* {{{ proto array get_loaded_extensions(void)
   Return an array containing names of loaded extensions */
PHP_FUNCTION(get_loaded_extensions)
{
	if (ARG_COUNT(ht) != 0) {
		WRONG_PARAM_COUNT;
	}

	array_init(return_value);
	zend_hash_apply_with_argument(&module_registry, (int (*)(void *, void*)) php_add_extension_info, return_value);
}
/* }}} */


/* {{{ proto bool extension_loaded(string extension_name)
   Returns true if the named extension is loaded */
PHP_FUNCTION(extension_loaded)
{
	zval **extension_name;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &extension_name)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(extension_name);
	if (zend_hash_exists(&module_registry, (*extension_name)->value.str.val, (*extension_name)->value.str.len+1)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* proto array get_extension_funcs(string extension_name)
   Returns an array with the names of functions belonging to the named extension */
PHP_FUNCTION(get_extension_funcs)
{
	zval **extension_name;
	zend_module_entry *module;
	zend_function_entry *func;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &extension_name)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(extension_name);
	if (zend_hash_find(&module_registry, (*extension_name)->value.str.val,
				       (*extension_name)->value.str.len+1, (void**)&module) == FAILURE) {
		return;
	}

	array_init(return_value);
	func = module->functions;
	while(func->fname) {
		add_next_index_string(return_value, func->fname, 1);
		func++;
	}
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */ 
