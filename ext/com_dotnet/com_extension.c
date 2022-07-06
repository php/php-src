/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
	zend_class_entry ce, *tmp;

	php_com_wrapper_minit(INIT_FUNC_ARGS_PASSTHRU);
	php_com_persist_minit(INIT_FUNC_ARGS_PASSTHRU);

	INIT_CLASS_ENTRY(ce, "com_exception", NULL);
	php_com_exception_class_entry = zend_register_internal_class_ex(&ce, zend_ce_exception);
	php_com_exception_class_entry->ce_flags |= ZEND_ACC_FINAL;
/*	php_com_exception_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */

	INIT_CLASS_ENTRY(ce, "com_safearray_proxy", NULL);
	php_com_saproxy_class_entry = zend_register_internal_class(&ce);
	php_com_saproxy_class_entry->ce_flags |= ZEND_ACC_FINAL;
/*	php_com_saproxy_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */
	php_com_saproxy_class_entry->get_iterator = php_com_saproxy_iter_get;

	INIT_CLASS_ENTRY(ce, "variant", class_variant_methods);
	ce.create_object = php_com_object_new;
	php_com_variant_class_entry = zend_register_internal_class(&ce);
	php_com_variant_class_entry->get_iterator = php_com_iter_get;
	php_com_variant_class_entry->serialize = zend_class_serialize_deny;
	php_com_variant_class_entry->unserialize = zend_class_unserialize_deny;

	INIT_CLASS_ENTRY(ce, "com", class_com_methods);
	ce.create_object = php_com_object_new;
	tmp = zend_register_internal_class_ex(&ce, php_com_variant_class_entry);
	tmp->get_iterator = php_com_iter_get;
	tmp->serialize = zend_class_serialize_deny;
	tmp->unserialize = zend_class_unserialize_deny;

#if HAVE_MSCOREE_H
	INIT_CLASS_ENTRY(ce, "dotnet", class_dotnet_methods);
	ce.create_object = php_com_object_new;
	tmp = zend_register_internal_class_ex(&ce, php_com_variant_class_entry);
	tmp->get_iterator = php_com_iter_get;
	tmp->serialize = zend_class_serialize_deny;
	tmp->unserialize = zend_class_unserialize_deny;
#endif

	REGISTER_INI_ENTRIES();

#define COM_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_CS|CONST_PERSISTENT)

#if SIZEOF_ZEND_LONG == 8
# define COM_ERR_CONST(x) REGISTER_LONG_CONSTANT(#x, (zend_long) (ULONG) (x), CONST_CS|CONST_PERSISTENT)
#else
# define COM_ERR_CONST COM_CONST
#endif

	COM_CONST(CLSCTX_INPROC_SERVER);
	COM_CONST(CLSCTX_INPROC_HANDLER);
	COM_CONST(CLSCTX_LOCAL_SERVER);
	COM_CONST(CLSCTX_REMOTE_SERVER);
	COM_CONST(CLSCTX_SERVER);
	COM_CONST(CLSCTX_ALL);

#if 0
	COM_CONST(DISPATCH_METHOD);
	COM_CONST(DISPATCH_PROPERTYGET);
	COM_CONST(DISPATCH_PROPERTYPUT);
#endif

	COM_CONST(VT_NULL);
	COM_CONST(VT_EMPTY);
	COM_CONST(VT_UI1);
	COM_CONST(VT_I1);
	COM_CONST(VT_UI2);
	COM_CONST(VT_I2);
	COM_CONST(VT_UI4);
	COM_CONST(VT_I4);
	COM_CONST(VT_R4);
	COM_CONST(VT_R8);
	COM_CONST(VT_BOOL);
	COM_CONST(VT_ERROR);
	COM_CONST(VT_CY);
	COM_CONST(VT_DATE);
	COM_CONST(VT_BSTR);
	COM_CONST(VT_DECIMAL);
	COM_CONST(VT_UNKNOWN);
	COM_CONST(VT_DISPATCH);
	COM_CONST(VT_VARIANT);
	COM_CONST(VT_INT);
	COM_CONST(VT_UINT);
	COM_CONST(VT_ARRAY);
	COM_CONST(VT_BYREF);

	COM_CONST(CP_ACP);
	COM_CONST(CP_MACCP);
	COM_CONST(CP_OEMCP);
	COM_CONST(CP_UTF7);
	COM_CONST(CP_UTF8);
	COM_CONST(CP_SYMBOL);
	COM_CONST(CP_THREAD_ACP);

	COM_CONST(VARCMP_LT);
	COM_CONST(VARCMP_EQ);
	COM_CONST(VARCMP_GT);
	COM_CONST(VARCMP_NULL);
	COM_CONST(LOCALE_SYSTEM_DEFAULT);

	COM_CONST(NORM_IGNORECASE);
	COM_CONST(NORM_IGNORENONSPACE);
	COM_CONST(NORM_IGNORESYMBOLS);
	COM_CONST(NORM_IGNOREWIDTH);
	COM_CONST(NORM_IGNOREKANATYPE);
#ifdef NORM_IGNOREKASHIDA
	COM_CONST(NORM_IGNOREKASHIDA);
#endif
	COM_ERR_CONST(DISP_E_DIVBYZERO);
	COM_ERR_CONST(DISP_E_OVERFLOW);
	COM_ERR_CONST(DISP_E_BADINDEX);
	COM_ERR_CONST(MK_E_UNAVAILABLE);

#if SIZEOF_ZEND_LONG == 8
	COM_CONST(VT_UI8);
	COM_CONST(VT_I8);
#endif

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
