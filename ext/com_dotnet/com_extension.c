/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong  <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <intsafe.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#if SIZEOF_ZEND_LONG == 8
#define PHP_DISP_E_DIVBYZERO ((zend_long) (ULONG) DISP_E_DIVBYZERO)
#define PHP_DISP_E_OVERFLOW ((zend_long) (ULONG) DISP_E_OVERFLOW)
#define PHP_DISP_E_BADINDEX ((zend_long) (ULONG) DISP_E_BADINDEX)
#define PHP_DISP_E_PARAMNOTFOUND ((zend_long) (ULONG) DISP_E_PARAMNOTFOUND)
#define PHP_MK_E_UNAVAILABLE ((zend_long) (ULONG) MK_E_UNAVAILABLE)
#else
#define PHP_DISP_E_DIVBYZERO DISP_E_DIVBYZERO
#define PHP_DISP_E_OVERFLOW DISP_E_OVERFLOW
#define PHP_DISP_E_BADINDEX DISP_E_BADINDEX
#define PHP_DISP_E_PARAMNOTFOUND DISP_E_PARAMNOTFOUND
#define PHP_MK_E_UNAVAILABLE MK_E_UNAVAILABLE
#endif

#include "com_extension_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(com_dotnet)
static PHP_GINIT_FUNCTION(com_dotnet);

zend_class_entry
	*php_com_variant_class_entry,
   	*php_com_exception_class_entry,
	*php_com_saproxy_class_entry;

/* {{{ com_dotnet_module_entry */
zend_module_entry com_dotnet_module_entry = {
	STANDARD_MODULE_HEADER,
	"com_dotnet",
	ext_functions,
	PHP_MINIT(com_dotnet),
	PHP_MSHUTDOWN(com_dotnet),
	PHP_RINIT(com_dotnet),
	PHP_RSHUTDOWN(com_dotnet),
	PHP_MINFO(com_dotnet),
	PHP_COM_DOTNET_VERSION,
	PHP_MODULE_GLOBALS(com_dotnet),
	PHP_GINIT(com_dotnet),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_COM_DOTNET
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(com_dotnet)
#endif

/* {{{ PHP_INI */

/* com.typelib_file is the path to a file containing a
 * list of typelibraries to register *persistently*.
 * lines starting with ; are comments
 * append #cis to end of typelib name to cause its constants
 * to be loaded case insensitively */
static PHP_INI_MH(OnTypeLibFileUpdate)
{
	FILE *typelib_file;
	char *typelib_name_buffer;
	char *strtok_buf = NULL;

	if (NULL == new_value || !new_value->val[0] || (typelib_file = VCWD_FOPEN(new_value->val, "r"))==NULL) {
		return FAILURE;
	}

	typelib_name_buffer = (char *) emalloc(sizeof(char)*1024);

	while (fgets(typelib_name_buffer, 1024, typelib_file)) {
		ITypeLib *pTL;
		char *typelib_name;
		char *modifier, *ptr;
		int mode = CONST_CS | CONST_PERSISTENT;	/* CONST_PERSISTENT is ok here */

		if (typelib_name_buffer[0]==';') {
			continue;
		}
		typelib_name = php_strtok_r(typelib_name_buffer, "\r\n", &strtok_buf); /* get rid of newlines */
		if (typelib_name == NULL) {
			continue;
		}
		typelib_name = php_strtok_r(typelib_name, "#", &strtok_buf);
		modifier = php_strtok_r(NULL, "#", &strtok_buf);
		if (modifier != NULL) {
			if (!strcmp(modifier, "cis") || !strcmp(modifier, "case_insensitive")) {
				php_error_docref("com.configuration", E_WARNING, "Declaration of case-insensitive constants is no longer supported; #cis modifier ignored");
			}
		}

		/* Remove leading/training white spaces on search_string */
		while (isspace(*typelib_name)) {/* Ends on '\0' in worst case */
			typelib_name ++;
		}
		ptr = typelib_name + strlen(typelib_name) - 1;
		while ((ptr != typelib_name) && isspace(*ptr)) {
			*ptr = '\0';
			ptr--;
		}

		if ((pTL = php_com_load_typelib_via_cache(typelib_name, COMG(code_page))) != NULL) {
			php_com_import_typelib(pTL, mode, COMG(code_page));
			ITypeLib_Release(pTL);
		}
	}

	efree(typelib_name_buffer);
	fclose(typelib_file);

	return SUCCESS;
}

static ZEND_INI_MH(OnAutoregisterCasesensitive)
{
	if (!zend_ini_parse_bool(new_value)) {
		php_error_docref("com.configuration", E_WARNING, "Declaration of case-insensitive constants is no longer supported");
		return FAILURE;
	}
	return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("com.allow_dcom",				"0", PHP_INI_SYSTEM, OnUpdateBool, allow_dcom, zend_com_dotnet_globals, com_dotnet_globals)
	STD_PHP_INI_BOOLEAN("com.autoregister_verbose",	"0", PHP_INI_ALL, OnUpdateBool, autoreg_verbose, zend_com_dotnet_globals, com_dotnet_globals)
	STD_PHP_INI_BOOLEAN("com.autoregister_typelib",	"0", PHP_INI_ALL, OnUpdateBool, autoreg_on, zend_com_dotnet_globals, com_dotnet_globals)
	STD_PHP_INI_ENTRY("com.autoregister_casesensitive",	"1", PHP_INI_ALL, OnAutoregisterCasesensitive, autoreg_case_sensitive, zend_com_dotnet_globals, com_dotnet_globals)
	STD_PHP_INI_ENTRY("com.code_page", "", PHP_INI_ALL, OnUpdateLong, code_page, zend_com_dotnet_globals, com_dotnet_globals)
	PHP_INI_ENTRY("com.typelib_file", "", PHP_INI_SYSTEM, OnTypeLibFileUpdate)
	PHP_INI_ENTRY("com.dotnet_version", NULL, PHP_INI_SYSTEM, NULL)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(com_dotnet)
{
#if defined(COMPILE_DL_COM_DOTNET) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(com_dotnet_globals, 0, sizeof(*com_dotnet_globals));
	com_dotnet_globals->code_page = CP_ACP;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(com_dotnet)
{
	zend_class_entry *tmp;

	php_com_wrapper_minit(INIT_FUNC_ARGS_PASSTHRU);
	php_com_persist_minit(INIT_FUNC_ARGS_PASSTHRU);

	php_com_exception_class_entry = register_class_com_exception(zend_ce_exception);
/*	php_com_exception_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */

	php_com_saproxy_class_entry = register_class_com_safearray_proxy();
/*	php_com_saproxy_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */
	php_com_saproxy_class_entry->get_iterator = php_com_saproxy_iter_get;

	php_com_variant_class_entry = register_class_variant();
	php_com_variant_class_entry->create_object = php_com_object_new;
	php_com_variant_class_entry->get_iterator = php_com_iter_get;

	tmp = register_class_com(php_com_variant_class_entry);
	tmp->create_object = php_com_object_new;
	tmp->get_iterator = php_com_iter_get;

#if HAVE_MSCOREE_H
	tmp = register_class_dotnet(php_com_variant_class_entry);
	tmp->create_object = php_com_object_new;
	tmp->get_iterator = php_com_iter_get;
#endif

	REGISTER_INI_ENTRIES();

	register_com_extension_symbols(module_number);

	PHP_MINIT(com_typeinfo)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(com_dotnet)
{
	UNREGISTER_INI_ENTRIES();
#if HAVE_MSCOREE_H
	if (COMG(dotnet_runtime_stuff)) {
		php_com_dotnet_mshutdown();
	}
#endif

	PHP_MSHUTDOWN(com_typeinfo)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(com_dotnet)
{
	COMG(rshutdown_started) = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(com_dotnet)
{
#if HAVE_MSCOREE_H
	if (COMG(dotnet_runtime_stuff)) {
		php_com_dotnet_rshutdown();
	}
#endif
	COMG(rshutdown_started) = 1;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(com_dotnet)
{
	php_info_print_table_start();

	php_info_print_table_header(2, "COM support", "enabled");
	php_info_print_table_header(2, "DCOM support", COMG(allow_dcom) ? "enabled" : "disabled");

#if HAVE_MSCOREE_H
	php_info_print_table_header(2, ".Net support", "enabled");
#else
	php_info_print_table_header(2, ".Net support", "not present in this build");
#endif

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */
