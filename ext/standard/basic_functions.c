/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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
#include "php_main.h"
#include "php_ini.h"
#include "internal_functions_registry.h"
#include "php_standard.h"
#include "php_math.h"
#include "ext/standard/info.h"
#include "ext/standard/url_scanner.h"
#include "zend_operators.h"
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
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
#ifdef PHP_WIN32
#include "win32/unistd.h"
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

static unsigned char second_and_third_args_force_ref[] = { 3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
static unsigned char second_args_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
static unsigned char third_argument_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
static unsigned char third_and_fourth_args_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
static unsigned char third_and_rest_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE_REST };

typedef struct _php_shutdown_function_entry {
	zval **arguments;
	int arg_count;
} php_shutdown_function_entry;

typedef struct _user_tick_function_entry {
	zval **arguments;
	int arg_count;
} user_tick_function_entry;

/* some prototypes for local functions */
static void user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry);
static void user_tick_function_dtor(user_tick_function_entry *tick_function_entry);
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
	PHP_FE(gmstrftime,								NULL)
#else
	PHP_FALIAS(strftime  , warn_not_available,      NULL)                             
	PHP_FALIAS(gmstrftime, warn_not_available,      NULL)                             
#endif
	PHP_FE(strtotime,								NULL)
	PHP_FE(date,									NULL)
	PHP_FE(gmdate,									NULL)
	PHP_FE(getdate,									NULL)
	PHP_FE(localtime,								NULL)
	PHP_FE(checkdate,								NULL)

	PHP_FE(flush,									NULL)
	
	PHP_FE(gettype,									NULL)
	PHP_FE(settype,									first_arg_force_ref)
	
	PHP_FE(getimagesize,							second_args_force_ref)
	
	PHP_FE(wordwrap,								NULL)
	PHP_FE(htmlspecialchars,						NULL)
	PHP_FE(htmlentities,							NULL)
	PHP_FE(get_html_translation_table,				NULL)
	
	PHP_NAMED_FE(md5,		php_if_md5,				NULL)
	PHP_NAMED_FE(crc32,		php_if_crc32,			NULL)

	PHP_FE(iptcparse,								NULL)
	PHP_FE(iptcembed,								NULL)
		
	PHP_FE(phpinfo,									NULL)
	PHP_FE(phpversion,								NULL)
	PHP_FE(phpcredits,								NULL)
	PHP_FE(php_logo_guid,							NULL)
	PHP_FE(zend_logo_guid,							NULL)
	PHP_FE(php_sapi_name,							NULL)
	PHP_FE(php_uname,								NULL)
	
	PHP_FE(strnatcmp,								NULL)
	PHP_FE(strnatcasecmp,							NULL)
	PHP_FE(substr_count, 							NULL)
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
	PHP_FE(pathinfo,								NULL)
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
	PHP_FE(similar_text,							third_argument_force_ref)
	PHP_FE(explode,									NULL)
	PHP_FE(implode,									NULL)
	PHP_FE(setlocale,								NULL)
	PHP_FE(soundex,									NULL)
	PHP_FE(levenshtein,									NULL)
	PHP_FE(chr,										NULL)
	PHP_FE(ord,										NULL)
	PHP_FE(parse_str,								second_arg_force_ref)
	PHP_FE(str_pad,									NULL)
	PHP_FALIAS(rtrim,			chop,				NULL)
	PHP_FALIAS(strchr,			strstr,				NULL)
	PHP_NAMED_FE(sprintf,		PHP_FN(user_sprintf),	NULL)
	PHP_NAMED_FE(printf,		PHP_FN(user_printf),	NULL)
    PHP_FE(sscanf,                                  third_and_rest_force_ref)
    PHP_FE(fscanf,                                  third_and_rest_force_ref)

	PHP_FE(parse_url,								NULL)
	PHP_FE(urlencode,								NULL)
	PHP_FE(urldecode,								NULL)
	PHP_FE(rawurlencode,							NULL)
	PHP_FE(rawurldecode,							NULL)

#ifdef HAVE_SYMLINK
	PHP_FE(readlink,								NULL)
	PHP_FE(linkinfo,								NULL)
	PHP_FE(symlink,									NULL)
	PHP_FE(link,									NULL)
#else
	PHP_FALIAS(readlink, warn_not_available,		NULL)
	PHP_FALIAS(linkinfo, warn_not_available,		NULL)
	PHP_FALIAS(symlink, warn_not_available,			NULL)
	PHP_FALIAS(link, warn_not_available,			NULL)
#endif
	PHP_FE(unlink,									NULL)
	
	PHP_FE(exec, 									second_and_third_args_force_ref)
	PHP_FE(system, 									second_arg_force_ref)
	PHP_FE(escapeshellcmd, 							NULL)
	PHP_FE(escapeshellarg, 							NULL)
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
#if !defined(PHP_WIN32)||HAVE_BINDLIB
	PHP_FE(checkdnsrr,								NULL)
	PHP_FE(getmxrr,									second_and_third_args_force_ref)
#else
	PHP_FALIAS(checkdnsrr, warn_not_available,      NULL)                             
	PHP_FALIAS(getmxrr,    warn_not_available,      NULL)                             
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
	PHP_FE(ip2long,									NULL)
	PHP_FE(long2ip,									NULL)

	PHP_FE(getenv,									NULL)
#ifdef HAVE_PUTENV
	PHP_FE(putenv,									NULL)
#else
	PHP_FALIAS(putenv    , warn_not_available,      NULL)                             
#endif

	PHP_FE(microtime,								NULL)
	PHP_FE(gettimeofday,							NULL)
#ifdef HAVE_GETRUSAGE
	PHP_FE(getrusage,								NULL)
#else
	PHP_FALIAS(getrusage , warn_not_available,      NULL)                             
#endif
	
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
	PHP_FE(is_numeric,								NULL)
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

	PHP_FE(register_tick_function,		NULL)
	PHP_FE(unregister_tick_function,	NULL)

	PHP_FE(highlight_file,				NULL)	
	PHP_FALIAS(show_source, highlight_file , NULL)
	PHP_FE(highlight_string,			NULL)
	
	PHP_FE(ini_get,						NULL)
	PHP_FE(ini_set,						NULL)
	PHP_FALIAS(ini_alter,	ini_set,	NULL)
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

	PHP_FE(parse_ini_file,				NULL)

	PHP_FE(is_uploaded_file,			NULL)
	PHP_FE(move_uploaded_file,			NULL)

	/* functions from reg.c */
	PHP_FE(ereg,									third_argument_force_ref)
	PHP_FE(ereg_replace,							NULL)
	PHP_FE(eregi,									third_argument_force_ref)
	PHP_FE(eregi_replace,							NULL)
	PHP_FE(split,									NULL)
	PHP_FE(spliti,									NULL)
	PHP_FALIAS(join,			implode,			NULL)
	PHP_FE(sql_regcase,								NULL)

	/* functions from dl.c */
	PHP_FE(dl,							NULL)


	/* functions from file.c */
	PHP_FE(pclose,				NULL)
	PHP_FE(popen,				NULL)
	PHP_FE(readfile,			NULL)
	PHP_FE(rewind,				NULL)
	PHP_FE(rmdir,				NULL)
	PHP_FE(umask,				NULL)
	PHP_FE(fclose,				NULL)
	PHP_FE(feof,				NULL)
	PHP_FE(fgetc,				NULL)
	PHP_FE(fgets,				NULL)
	PHP_FE(fgetss,				NULL)
	PHP_FE(fread,				NULL)
	PHP_FE(fopen,				NULL)
	PHP_FE(fpassthru,			NULL)
	PHP_FE(ftruncate,    		NULL)
	PHP_FE(fstat,    			NULL)
	PHP_FE(fseek,				NULL)
	PHP_FE(ftell,				NULL)
	PHP_FE(fflush,				NULL)
	PHP_FE(fwrite,				NULL)
	PHP_FALIAS(fputs,	fwrite,	NULL)
	PHP_FE(mkdir,				NULL)
	PHP_FE(rename,				NULL)
	PHP_FE(copy,				NULL)
	PHP_FE(tempnam,				NULL)
	PHP_FE(tmpfile,				NULL)
	PHP_FE(file,				NULL)
	PHP_FE(fgetcsv,				NULL)
    PHP_FE(flock,				NULL)
	PHP_FE(get_meta_tags,		NULL)
	PHP_FE(set_file_buffer,		NULL)
	/* set_socket_blocking() is deprecated,
	   use socket_set_blocking() instead */
	PHP_FE(set_socket_blocking,	NULL)
	PHP_FE(socket_set_blocking,	NULL)
#if HAVE_SYS_TIME_H
	PHP_FE(socket_set_timeout,	NULL)
#else
	PHP_FALIAS(socket_set_timeout, warn_not_available,      NULL)                             
#endif
	PHP_FE(socket_get_status,	NULL)
#if !PHP_WIN32 || defined(ZTS)
	PHP_FE(realpath,			NULL)
#else
	PHP_FALIAS(realpath,		warn_not_available,		NULL)
#endif

	/* functions from fsock.c */
	PHP_FE(fsockopen, 			third_and_fourth_args_force_ref)
	PHP_FE(pfsockopen, 			third_and_fourth_args_force_ref)

	/* functions from pack.c */
	PHP_FE(pack,				NULL)
	PHP_FE(unpack,				NULL)

	/* functions from browscap.c */
	PHP_FE(get_browser,			NULL)

#if HAVE_CRYPT
	/* functions from crypt.c */
	PHP_FE(crypt,				NULL)
#else
	PHP_FALIAS(crypt  , warn_not_available,      NULL)                             
#endif

	/* functions from dir.c */
	PHP_FE(opendir,				NULL)
	PHP_FE(closedir,			NULL)
	PHP_FE(chdir,				NULL)
	PHP_FE(getcwd,				NULL)
	PHP_FE(rewinddir,			NULL)
	PHP_FE(readdir,				NULL)
	PHP_FALIAS(dir,		getdir,	NULL)

	/* functions from filestat.c */
	PHP_FE(fileatime,			NULL)
	PHP_FE(filectime,			NULL)
	PHP_FE(filegroup,			NULL)
	PHP_FE(fileinode,			NULL)
	PHP_FE(filemtime,			NULL)
	PHP_FE(fileowner,			NULL)
	PHP_FE(fileperms,			NULL)
	PHP_FE(filesize,			NULL)
	PHP_FE(filetype,			NULL)
	PHP_FE(file_exists,			NULL)
	PHP_FE(is_writable,			NULL)
	PHP_FALIAS(is_writeable,	is_writable,		NULL)
	PHP_FE(is_readable,				NULL)
	PHP_FE(is_executable,			NULL)
	PHP_FE(is_file,					NULL)
	PHP_FE(is_dir,					NULL)
	PHP_FE(is_link,					NULL)
	PHP_FE(stat,					NULL)
	PHP_FE(lstat,					NULL)
	PHP_FE(chown,					NULL)
	PHP_FE(chgrp,					NULL)
	PHP_FE(chmod,					NULL)
	PHP_FE(touch,					NULL)
	PHP_FE(clearstatcache,			NULL)
	PHP_FE(diskfreespace,			NULL)

	/* functions from mail.c */
	PHP_FE(mail,					NULL)
	PHP_FE(ezmlm_hash,				NULL)

	/* functions from syslog.c */
	PHP_FE(openlog,					NULL)
	PHP_FE(syslog,					NULL)
	PHP_FE(closelog,				NULL)
	PHP_FE(define_syslog_variables,	NULL)

	/* functions from lcg.c */
	PHP_FE(lcg_value, NULL)

	/* functions from metaphone.c */
	PHP_FE(metaphone, NULL)

	/* functions from output.c */
	PHP_FE(ob_start,					NULL)
	PHP_FE(ob_end_flush,				NULL)
	PHP_FE(ob_end_clean,				NULL)
	PHP_FE(ob_get_length,				NULL)
	PHP_FE(ob_get_contents,				NULL)
	PHP_FE(ob_implicit_flush,			NULL)

	/* functions from array.c */
	PHP_FE(ksort,									first_arg_force_ref)
	PHP_FE(krsort,									first_arg_force_ref)
	PHP_FE(natsort,									first_arg_force_ref)
	PHP_FE(natcasesort,								first_arg_force_ref)
	PHP_FE(asort,									first_arg_force_ref)
	PHP_FE(arsort,									first_arg_force_ref)
	PHP_FE(sort,									first_arg_force_ref)
	PHP_FE(rsort,									first_arg_force_ref)
	PHP_FE(usort,									first_arg_force_ref)
	PHP_FE(uasort,									first_arg_force_ref)
	PHP_FE(uksort,									first_arg_force_ref)
	PHP_FE(shuffle,									first_arg_force_ref)
	PHP_FE(array_walk,								first_arg_force_ref)
	PHP_FE(count,									first_arg_allow_ref)
	PHP_FE(end, 									first_arg_force_ref)
	PHP_FE(prev, 									first_arg_force_ref)
	PHP_FE(next, 									first_arg_force_ref)
	PHP_FE(reset, 									first_arg_force_ref)
	PHP_FE(current, 								first_arg_force_ref)
	PHP_FE(key, 									first_arg_force_ref)
	PHP_FE(min,										NULL)
	PHP_FE(max,										NULL)
	PHP_FE(in_array,								NULL)
	PHP_FE(extract,									NULL)
	PHP_FE(compact,									NULL)
	PHP_FE(range,									NULL)
	PHP_FE(array_multisort,							NULL)
	PHP_FE(array_push,								first_arg_force_ref)
	PHP_FE(array_pop,								first_arg_force_ref)
	PHP_FE(array_shift,								first_arg_force_ref)
	PHP_FE(array_unshift,							first_arg_force_ref)
	PHP_FE(array_splice,							first_arg_force_ref)
	PHP_FE(array_slice,								NULL)
	PHP_FE(array_merge,								NULL)
	PHP_FE(array_merge_recursive,					NULL)
	PHP_FE(array_keys,								NULL)
	PHP_FE(array_values,							NULL)
	PHP_FE(array_count_values,		 			  	NULL)
	PHP_FE(array_reverse,							NULL)
	PHP_FE(array_pad,								NULL)
	PHP_FE(array_flip,								NULL)
	PHP_FE(array_rand,								NULL)
	PHP_FE(array_unique,							NULL)
	PHP_FE(array_intersect,							NULL)
	PHP_FE(array_diff,							    NULL)

	/* aliases from array.c */
	PHP_FALIAS(pos,				current,			first_arg_force_ref)
	PHP_FALIAS(sizeof,			count,				first_arg_allow_ref)
	
	/* functions from assert.c */
	PHP_FE(assert,          NULL)
	PHP_FE(assert_options,	NULL)

	{NULL, NULL, NULL}
};


static PHP_INI_MH(OnUpdateSafeModeProtectedEnvVars)
{
	char *protected_vars, *protected_var;
	int dummy=1;
	BLS_FETCH();

	protected_vars = estrndup(new_value, new_value_length);
	zend_hash_clean(&BG(sm_protected_env_vars));

	protected_var=strtok(protected_vars, ", ");
	while (protected_var) {
		zend_hash_update(&BG(sm_protected_env_vars), protected_var, strlen(protected_var), &dummy, sizeof(int), NULL);
		protected_var=strtok(NULL, ", ");
	}
	efree(protected_vars);
	return SUCCESS;
}


static PHP_INI_MH(OnUpdateSafeModeAllowedEnvVars)
{
	BLS_FETCH();

	if (BG(sm_allowed_env_vars)) {
		free(BG(sm_allowed_env_vars));
	}
	BG(sm_allowed_env_vars) = zend_strndup(new_value, new_value_length);
	return SUCCESS;
}


PHP_INI_BEGIN()
	PHP_INI_ENTRY_EX("safe_mode_protected_env_vars",	SAFE_MODE_PROTECTED_ENV_VARS,	PHP_INI_SYSTEM,		OnUpdateSafeModeProtectedEnvVars,		NULL)
	PHP_INI_ENTRY_EX("safe_mode_allowed_env_vars",		SAFE_MODE_ALLOWED_ENV_VARS,		PHP_INI_SYSTEM,		OnUpdateSafeModeAllowedEnvVars,			NULL)
	STD_PHP_INI_ENTRY("session.use_trans_sid",          "1",							PHP_INI_ALL,			OnUpdateBool,			use_trans_sid,			php_basic_globals,			basic_globals)
PHP_INI_END()


zend_module_entry basic_functions_module = {
	"standard",					/* extension name */
	basic_functions,			/* function list */
	PHP_MINIT(basic),			/* process startup */
	PHP_MSHUTDOWN(basic),		/* process shutdown */
	PHP_RINIT(basic),			/* request startup */
	PHP_RSHUTDOWN(basic),		/* request shutdown */
	PHP_MINFO(basic),			/* extension info */
	PHP_GINIT(basic),			/* global startup function */
	NULL,						/* global shutdown function */
	STANDARD_MODULE_PROPERTIES_EX
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
			if (!strncmp(*env,pe->key,pe->key_len) && (*env)[pe->key_len]=='=') {	/* found it */
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


void test_class_startup(void);

static void basic_globals_ctor(BLS_D)
{
	BG(next) = NULL;
	BG(left) = -1;
	BG(user_tick_functions) = NULL;
	zend_hash_init(&BG(sm_protected_env_vars), 5, NULL, NULL, 1);
	BG(sm_allowed_env_vars) = NULL;

#ifdef TRANS_SID
	memset(&BG(url_adapt_state), 0, sizeof(BG(url_adapt_state)));
#endif

#ifdef PHP_WIN32
	CoInitialize(NULL);
#endif
}

static void basic_globals_dtor(BLS_D)
{
	zend_hash_destroy(&BG(sm_protected_env_vars));
	if (BG(sm_allowed_env_vars)) {
		free(BG(sm_allowed_env_vars));
	}
#ifdef PHP_WIN32
	CoUninitialize();
#endif
}


PHP_MINIT_FUNCTION(basic)
{
#ifdef ZTS
	basic_globals_id = ts_allocate_id(sizeof(php_basic_globals), (ts_allocate_ctor) basic_globals_ctor, (ts_allocate_dtor) basic_globals_dtor);
#else
	basic_globals_ctor(BLS_C);
#endif
	
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


	test_class_startup();
	REGISTER_INI_ENTRIES();

	register_phpinfo_constants(INIT_FUNC_ARGS_PASSTHRU);
	register_html_constants(INIT_FUNC_ARGS_PASSTHRU);
	register_string_constants(INIT_FUNC_ARGS_PASSTHRU);

	PHP_MINIT(regex)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(file)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(fsock)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(pack)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(browscap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(lcg)(INIT_FUNC_ARGS_PASSTHRU);

#if HAVE_CRYPT
	PHP_MINIT(crypt)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	PHP_MINIT(dir)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(syslog)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(array)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(assert)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(basic)
{
	BLS_FETCH();

	basic_globals_dtor(BLS_C);

#ifdef ZTS
	ts_free_id(basic_globals_id);
#endif

	UNREGISTER_INI_ENTRIES();

	PHP_MSHUTDOWN(regex)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(fsock)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(browscap)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(array)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(assert)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

	return SUCCESS;	
}


PHP_RINIT_FUNCTION(basic)
{
	BLS_FETCH();

	BG(strtok_string) = NULL;
	BG(locale_string) = NULL;
	BG(user_compare_func_name) = NULL;
	BG(array_walk_func_name) = NULL;
	BG(incomplete_class) = NULL;
	BG(page_uid) = -1;
	BG(page_inode) = -1;
	BG(page_mtime) = -1;
#ifdef HAVE_PUTENV
	if (zend_hash_init(&BG(putenv_ht), 1, NULL, (void (*)(void *)) php_putenv_destructor, 0) == FAILURE) {
		return FAILURE;
	}
#endif
	BG(user_shutdown_function_names)=NULL;

	PHP_RINIT(head)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_RINIT(filestat)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_RINIT(syslog)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_RINIT(assert)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_RINIT(dir)(INIT_FUNC_ARGS_PASSTHRU);

#ifdef TRANS_SID
	if (BG(use_trans_sid)) {
		PHP_RINIT(url_scanner)(INIT_FUNC_ARGS_PASSTHRU);
		PHP_RINIT(url_scanner_ex)(INIT_FUNC_ARGS_PASSTHRU);
	}
#endif
	
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
		setlocale(LC_ALL, "C");
		setlocale(LC_CTYPE, "");
	}
	STR_FREE(BG(locale_string));

	PHP_RSHUTDOWN(fsock)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(filestat)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(syslog)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(assert)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

#ifdef TRANS_SID
	PHP_RSHUTDOWN(url_scanner_ex)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(url_scanner)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif

	if (BG(user_tick_functions)) {
		zend_llist_destroy(BG(user_tick_functions));
		efree(BG(user_tick_functions));
		BG(user_tick_functions) = NULL;
	}
	
	return SUCCESS;
}


PHP_GINIT_FUNCTION(basic)
{
	PHP_GINIT(output)(GINIT_FUNC_ARGS_PASSTHRU);

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

/* {{{ proto int ip2long(string ip_address)
   Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address */
PHP_FUNCTION(ip2long)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(str);

	RETURN_LONG(ntohl(inet_addr((*str)->value.str.val)));
}
/* }}} */

/* {{{ proto string long2ip(int proper_address)
   Converts an (IPv4) Internet network address into a string in Internet standard dotted format */
PHP_FUNCTION(long2ip)
{
	zval **num;
	struct in_addr myaddr;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(num);
	myaddr.s_addr = htonl((unsigned long)(*num)->value.lval);

	RETURN_STRING (inet_ntoa(myaddr), 1);
}
/* }}} */


/********************
 * System Functions *
 ********************/

/* {{{ proto string getenv(string varname)
   Get the value of an environment variable */
PHP_FUNCTION(getenv)
{
	pval **str;
	char *ptr;


	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if ((*str)->type != IS_STRING) {
		RETURN_FALSE;
	}
	
	
	ptr = sapi_getenv((*str)->value.str.val, (*str)->value.str.len);
	if (!ptr) {
		ptr = getenv((*str)->value.str.val);
	}
	if (ptr) {
		RETURN_STRING(ptr, 1);
	}
	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_PUTENV

/* {{{ proto void putenv(string setting)
   Set the value of an environment variable */
PHP_FUNCTION(putenv)
{
	pval **str;
	BLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if ((*str)->value.str.val && *((*str)->value.str.val)) {
		int ret;
		char *p,**env;
		putenv_entry pe;
		PLS_FETCH();
		
		pe.putenv_string = estrndup((*str)->value.str.val,(*str)->value.str.len);
		pe.key = estrndup((*str)->value.str.val, (*str)->value.str.len);
		if ((p=strchr(pe.key,'='))) { /* nullify the '=' if there is one */
			*p='\0';
		}
		pe.key_len = strlen(pe.key);
		
		if (PG(safe_mode)) {
			/* Check the protected list */
			if (zend_hash_exists(&BG(sm_protected_env_vars), pe.key, pe.key_len)) {
				php_error(E_WARNING, "Safe Mode:  Cannot override protected environment variable '%s'", pe.key);
				efree(pe.putenv_string);
				efree(pe.key);
				RETURN_FALSE;
			}

			/* Check the allowed list */
			if (BG(sm_allowed_env_vars) && *BG(sm_allowed_env_vars)) {
				char *allowed_env_vars = estrdup(BG(sm_allowed_env_vars));
				char *allowed_prefix = strtok(allowed_env_vars, ", ");
				zend_bool allowed=0;

				while (allowed_prefix) {
					if (!strncmp(allowed_prefix, pe.key, strlen(allowed_prefix))) {
						allowed=1;
						break;
					}
					allowed_prefix = strtok(NULL, ", ");
				}
				efree(allowed_env_vars);
				if (!allowed) {
					php_error(E_WARNING, "Safe Mode:  Cannot set environment variable '%s' - it's not in the allowed list", pe.key);
					efree(pe.putenv_string);
					efree(pe.key);
					RETURN_FALSE;
				}
			}
		}

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
/* }}} */
#endif


/*******************
 * Basic Functions *
 *******************/
/* {{{ proto int intval(mixed var [, int base])
   Get the integer value of a variable using the optional base for the conversion */
PHP_FUNCTION(intval)
{
	pval **num, **arg_base;
	int base;
	
	switch(ZEND_NUM_ARGS()) {
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
/* }}} */

/* {{{ proto double doubleval(mixed var)
   Get the double-precision value of a variable */
PHP_FUNCTION(doubleval)
{
	pval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	*return_value=**num;
	zval_copy_ctor(return_value);
	convert_to_double(return_value);
}
/* }}} */

/* {{{ proto string strval(mixed var) 
   Get the string value of a variable */
PHP_FUNCTION(strval)
{
	pval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	*return_value=**num;
	zval_copy_ctor(return_value);
	convert_to_string(return_value);
}
/* }}} */

/* {{{ proto void flush(void)
   Flush the output buffer */
PHP_FUNCTION(flush)
{
	sapi_flush();
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
	php_sleep((*num)->value.lval);
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
	usleep((*num)->value.lval);
#endif
}
/* }}} */

/* {{{ proto string gettype(mixed var)
   Returns the type of the variable */
PHP_FUNCTION(gettype)
{
	pval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch ((*arg)->type) {
		case IS_NULL:
			RETVAL_STRING("NULL",1);
			break;
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
/* }}} */

/* {{{ proto int settype(string var, string type)
   Set the type of the variable */
PHP_FUNCTION(settype)
{
	pval **var, **type;
	char *new_type;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &var, &type) == FAILURE) {
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
/* }}} */

/* {{{ proto string get_current_user(void)
   Get the name of the owner of the current PHP script */
PHP_FUNCTION(get_current_user)
{
	RETURN_STRING(php_get_current_user(),1);
}
/* }}} */

/* {{{ proto string get_cfg_var(string option_name)
   Get the value of a PHP configuration option */
PHP_FUNCTION(get_cfg_var)
{
	pval **varname;
	char *value;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &varname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(varname);
	
	if (cfg_get_string((*varname)->value.str.val,&value)==FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRING(value,1);
}
/* }}} */


/* {{{ proto int set_magic_quotes_runtime(int new_setting)
   Set the current active configuration setting of magic_quotes_runtime and return previous */
PHP_FUNCTION(set_magic_quotes_runtime)
{
	pval **new_setting;
	PLS_FETCH();
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &new_setting)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_boolean_ex(new_setting);
	
	PG(magic_quotes_runtime) = (zend_bool) (*new_setting)->value.lval;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int get_magic_quotes_runtime(void)
   Get the current active configuration setting of magic_quotes_runtime */	
PHP_FUNCTION(get_magic_quotes_runtime)
{
	PLS_FETCH();

	RETURN_LONG(PG(magic_quotes_runtime));
}
/* }}} */

/* {{{ proto int get_magic_quotes_gpc(void)
   Get the current active configuration setting of magic_quotes_gpc */
PHP_FUNCTION(get_magic_quotes_gpc)
{
	PLS_FETCH();

	RETURN_LONG(PG(magic_quotes_gpc));
}
/* }}} */


void php_is_type(INTERNAL_FUNCTION_PARAMETERS,int type)
{
	pval **arg;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &arg)==FAILURE) {
		RETURN_FALSE;
	}
	if ((*arg)->type == type) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* {{{ proto bool is_resource(mixed var)
   Returns true if variable is a resource */ 
PHP_FUNCTION(is_resource) 
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_RESOURCE);
}
/* }}} */

/* {{{ proto bool is_bool(mixed var)
   Returns true if variable is a boolean */
PHP_FUNCTION(is_bool) 
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_BOOL);
}
/* }}} */

/* {{{ proto bool is_long(mixed var)
   Returns true if variable is a long (integer) */
PHP_FUNCTION(is_long) 
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_LONG);
}
/* }}} */

/* {{{ proto bool is_double(mixed var)
   Returns true if variable is a double */
PHP_FUNCTION(is_double)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_DOUBLE);
}
/* }}} */

/* {{{ proto bool is_string(mixed var)
   Returns true if variable is a string */
PHP_FUNCTION(is_string)
{ 
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_STRING);
}
/* }}} */

/* {{{ proto bool is_array(mixed var)
   Returns true if variable is an array */
PHP_FUNCTION(is_array)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_ARRAY);
}
/* }}} */

/* {{{ proto bool is_object(mixed var)
   Returns true if variable is an object */
PHP_FUNCTION(is_object)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_OBJECT);
}
/* }}} */

/* {{{ proto bool is_numeric(mixed value)
   Returns true if value is a number or a numeric string */
PHP_FUNCTION(is_numeric)
{
	zval **arg;
	int result;
	
	if (ZEND_NUM_ARGS() !=1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch ((*arg)->type) {
		case IS_LONG:
		case IS_DOUBLE:
			RETURN_TRUE;
			break;

		case IS_STRING:
			result = is_numeric_string((*arg)->value.str.val, (*arg)->value.str.len, NULL, NULL);
			if (result == IS_LONG || result == IS_DOUBLE) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */

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

/* {{{ proto int error_log(string message, int message_type [, string destination] [, string extra_headers])
   Send an error message somewhere */
PHP_FUNCTION(error_log)
{
	pval **string, **erropt = NULL, **option = NULL, **emailhead = NULL;
	int opt_err = 0;
	char *message, *opt=NULL, *headers=NULL;

	switch(ZEND_NUM_ARGS()) {
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
/* }}} */

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

/* {{{ proto mixed call_user_func(string function_name [, mixed parmeter] [, mixed ...])
   Call a user function which is the first parameter */
PHP_FUNCTION(call_user_func)
{
	pval ***params;
	pval *retval_ptr;
	int arg_count=ZEND_NUM_ARGS();
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
	if (call_user_function_ex(CG(function_table), NULL, *params[0], &retval_ptr, arg_count-1, params+1, 1, NULL)==SUCCESS
		&& retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		php_error(E_WARNING,"Unable to call %s() - function does not exist", (*params[0])->value.str.val);
	}
	efree(params);
}
/* }}} */

/* {{{ proto mixed call_user_method(string method_name, object object [, mixed parameter] [, mixed ...])
   Call a user method, on a specific object where the first argument is the method name, the second argument is the object and the subsequent arguments are the parameters */
PHP_FUNCTION(call_user_method)
{
	pval ***params;
	pval *retval_ptr;
	int arg_count=ZEND_NUM_ARGS();
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
	if (call_user_function_ex(CG(function_table), *params[1], *params[0], &retval_ptr, arg_count-2, params+2, 1, NULL)==SUCCESS
		&& retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	} else {
		php_error(E_WARNING,"Unable to call %s() - function does not exist", (*params[0])->value.str.val);
	}
	efree(params);
}
/* }}} */

void user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry)
{
	int i;

	for (i=0; i<shutdown_function_entry->arg_count; i++) {
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

static int user_shutdown_function_call(php_shutdown_function_entry *shutdown_function_entry)
{
	zval retval;
	CLS_FETCH();

	if (call_user_function(CG(function_table), NULL, shutdown_function_entry->arguments[0], &retval, shutdown_function_entry->arg_count-1, shutdown_function_entry->arguments+1)==SUCCESS) {
		zval_dtor(&retval);
	} else {
		php_error(E_WARNING,"Unable to call %s() - function does not exist",
				  shutdown_function_entry->arguments[0]->value.str.val);
	}
	return 0;
}

static void user_tick_function_call(user_tick_function_entry *tick_fe)
{
	zval retval;
	zval *function = tick_fe->arguments[0];
	CLS_FETCH();

	if (call_user_function(CG(function_table), NULL, function, &retval,
						   tick_fe->arg_count - 1, tick_fe->arguments + 1) == SUCCESS) {
		zval_dtor(&retval);
	} else {
		zval **obj, **method;

		if (Z_TYPE_P(function) == IS_STRING) {
			php_error(E_WARNING, "Unable to call %s() - function does not exist",
					  Z_STRVAL_P(function));
		} else if (Z_TYPE_P(function) == IS_ARRAY &&
				   zend_hash_index_find(function->value.ht, 0, (void **) &obj) == SUCCESS &&
				   zend_hash_index_find(function->value.ht, 1, (void **) &method) == SUCCESS &&
				   Z_TYPE_PP(obj) == IS_OBJECT &&
				   Z_TYPE_PP(method) == IS_STRING) {
			php_error(E_WARNING, "Unable to call %s::%s() - function does not exist",
					  (*obj)->value.obj.ce->name, Z_STRVAL_PP(method));
		} else
			php_error(E_WARNING, "Unable to call tick function");
	}
}

static void run_user_tick_functions(int tick_count)
{
	BLS_FETCH();

	zend_llist_apply(BG(user_tick_functions), (llist_apply_func_t)user_tick_function_call);
}

static int user_tick_function_compare(user_tick_function_entry *tick_fe1,
									  user_tick_function_entry *tick_fe2)
{
	zval *func1 = tick_fe1->arguments[0];
	zval *func2 = tick_fe2->arguments[0];
	
	if (Z_TYPE_P(func1) == IS_STRING && Z_TYPE_P(func2) == IS_STRING) {
		return (zend_binary_zval_strcmp(func1, func2) == 0);
	} else if (Z_TYPE_P(func1) == IS_ARRAY && Z_TYPE_P(func2) == IS_ARRAY) {
		zval result;
		zend_compare_arrays(&result, func1, func2);
		return (Z_LVAL(result) == 0);
	} else
		return 0;
}

void php_call_shutdown_functions(void)
{
	BLS_FETCH();
	ELS_FETCH();

	if (BG(user_shutdown_function_names)) {
		jmp_buf orig_bailout;

		memcpy(&orig_bailout, &EG(bailout), sizeof(jmp_buf));
		if (setjmp(EG(bailout))!=0) {
			/* one of the shutdown functions bailed out */
			memcpy(&EG(bailout), &orig_bailout, sizeof(jmp_buf));
			return;
		}
		zend_hash_apply(BG(user_shutdown_function_names),
						(apply_func_t)user_shutdown_function_call);
		memcpy(&EG(bailout), &orig_bailout, sizeof(jmp_buf));
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

	shutdown_function_entry.arg_count = ZEND_NUM_ARGS();

	if (shutdown_function_entry.arg_count<1) {
		WRONG_PARAM_COUNT;
	}
	shutdown_function_entry.arguments = (pval **) emalloc(sizeof(pval *)*shutdown_function_entry.arg_count);

	if (zend_get_parameters_array(ht, shutdown_function_entry.arg_count, shutdown_function_entry.arguments)==FAILURE) {
		RETURN_FALSE;
	}
	if (!BG(user_shutdown_function_names)) {
		BG(user_shutdown_function_names) = (HashTable *) emalloc(sizeof(HashTable));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, (void (*)(void *))user_shutdown_function_dtor, 0);
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

	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &filename)==FAILURE) {
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
	char *hicompiled_string_description;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &expr)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(expr);

	php_get_highlight_struct(&syntax_highlighter_ini);
	
	hicompiled_string_description = zend_make_compiled_string_description("highlighted code");

	if (highlight_string(*expr, &syntax_highlighter_ini, hicompiled_string_description)==FAILURE) {
		efree(hicompiled_string_description);
		RETURN_FALSE;
	}
	efree(hicompiled_string_description);
	RETURN_TRUE;
}
/* }}} */


pval test_class_get_property(zend_property_reference *property_reference)
{
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;


	printf("Reading a property from a OverloadedTestClass object:\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
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

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
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

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
			case OE_IS_METHOD:
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
		
	printf("%d arguments\n", ZEND_NUM_ARGS());
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

	zend_register_internal_class(&test_class_entry);
}

/* {{{ proto string ini_get(string varname)
   Get a configuration option */
PHP_FUNCTION(ini_get)
{
	pval **varname;
	char *str;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &varname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);

	str = php_ini_string((*varname)->value.str.val, (*varname)->value.str.len+1, 0);

	if (!str) {
		RETURN_FALSE;
	}

	RETURN_STRING(str,1);
}
/* }}} */

/* {{{ proto string ini_set(string varname, string newvalue)
   Set a configuration option, returns false on error and the old value of the configuration option on success */
PHP_FUNCTION(ini_set)
{
	pval **varname, **new_value;
	char *old_value;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &varname, &new_value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);
	convert_to_string_ex(new_value);

	old_value = php_ini_string((*varname)->value.str.val, (*varname)->value.str.len+1, 0);
	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value, 1);
	} else {
		RETVAL_FALSE;
	}

	if (php_alter_ini_entry((*varname)->value.str.val, (*varname)->value.str.len+1, (*new_value)->value.str.val, (*new_value)->value.str.len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME)==FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string ini_restore(string varname)
   Restore the value of a configuration option specified by varname */
PHP_FUNCTION(ini_restore)
{
	pval **varname;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &varname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(varname);

	php_restore_ini_entry((*varname)->value.str.val, (*varname)->value.str.len+1, PHP_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto string print_r(mixed var)
   Prints out information about the specified variable */
PHP_FUNCTION(print_r)
{
	pval **expr;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &expr)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	zend_print_pval_r(*expr, 0);

	RETURN_TRUE;
}
/* }}} */


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
    switch (ZEND_NUM_ARGS()) {
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
   Returns port associated with service. Protocol must be "tcp" or "udp". */
PHP_FUNCTION(getservbyname)
{
	pval **name,**proto;  
	struct servent *serv;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2,&name,&proto) == FAILURE) {
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

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2,&port,&proto) == FAILURE) {
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
   Returns protocol number associated with name as per /etc/protocols */
PHP_FUNCTION(getprotobyname)
{
	pval **name;
	struct protoent *ent;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE) {
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
   Returns protocol name associated with protocol number proto */
PHP_FUNCTION(getprotobynumber)
{
	pval **proto;
	struct protoent *ent;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &proto) == FAILURE) {
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
	if (ZEND_NUM_ARGS() != 0) {
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

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &extension_name)) {
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


/* {{{ proto array get_extension_funcs(string extension_name)
   Returns an array with the names of functions belonging to the named extension */
PHP_FUNCTION(get_extension_funcs)
{
	zval **extension_name;
	zend_module_entry *module;
	zend_function_entry *func;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &extension_name)) {
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

/* {{{ proto void register_tick_function(string function_name [, mixed arg [, ... ]])
   Registers a tick callback function */
PHP_FUNCTION(register_tick_function)
{
	user_tick_function_entry tick_fe;
	int i;
	BLS_FETCH();

	tick_fe.arg_count = ZEND_NUM_ARGS();
	if (tick_fe.arg_count < 1) {
		WRONG_PARAM_COUNT;
	}

	tick_fe.arguments = (zval **)emalloc(sizeof(zval *) * tick_fe.arg_count);
	if (zend_get_parameters_array(ht, tick_fe.arg_count, tick_fe.arguments) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (Z_TYPE_P(tick_fe.arguments[0]) != IS_ARRAY)
		convert_to_string_ex(&tick_fe.arguments[0]);
	
	if (!BG(user_tick_functions)) {
		BG(user_tick_functions) = (zend_llist *)emalloc(sizeof(zend_llist));
		zend_llist_init(BG(user_tick_functions), sizeof(user_tick_function_entry),
						(void (*)(void *))user_tick_function_dtor, 0);
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
	BLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &function)) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(function) != IS_ARRAY)
		convert_to_string_ex(function);

	tick_fe.arguments = (zval **)emalloc(sizeof(zval *));
	tick_fe.arguments[0] = *function;
	tick_fe.arg_count = 1;
	zend_llist_del_element(BG(user_tick_functions), &tick_fe,
						   (int(*)(void*,void*))user_tick_function_compare);
	efree(tick_fe.arguments);
}
/* }}} */


/* This function is not directly accessible to end users */
PHPAPI PHP_FUNCTION(warn_not_available)
{
	php_error(E_WARNING, "%s() is  not supported in this PHP build", get_active_function_name());
    RETURN_FALSE;
}


PHP_FUNCTION(is_uploaded_file)
{
	zval **path;
	SLS_FETCH();

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &path)!=SUCCESS) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(path);

	if (zend_hash_exists(SG(rfc1867_uploaded_files), Z_STRVAL_PP(path), Z_STRLEN_PP(path)+1)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}


PHP_FUNCTION(move_uploaded_file)
{
	zval **path, **new_path;
	zend_bool successful=0;
	SLS_FETCH();

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &path, &new_path)!=SUCCESS) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(path);
	convert_to_string_ex(new_path);

	if (!zend_hash_exists(SG(rfc1867_uploaded_files), Z_STRVAL_PP(path), Z_STRLEN_PP(path)+1)) {
		RETURN_FALSE;
	}

	V_UNLINK(Z_STRVAL_PP(new_path));
	if (rename(Z_STRVAL_PP(path), Z_STRVAL_PP(new_path))==0) {
		successful=1;
	} else if (php_copy_file(Z_STRVAL_PP(path), Z_STRVAL_PP(new_path))==SUCCESS) {
		V_UNLINK(Z_STRVAL_PP(path));
		successful=1;
	}

	if (successful) {
		zend_hash_del(SG(rfc1867_uploaded_files), Z_STRVAL_PP(path), Z_STRLEN_PP(path)+1);
	} else {
		php_error(E_WARNING, "Unable to move '%s' to '%s'", Z_STRVAL_PP(path), Z_STRVAL_PP(new_path));
	}
	RETURN_BOOL(successful);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */ 
