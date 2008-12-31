/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(com_dotnet)
static PHP_GINIT_FUNCTION(com_dotnet);

TsHashTable php_com_typelibraries;

zend_class_entry
	*php_com_variant_class_entry,
   	*php_com_exception_class_entry,
	*php_com_saproxy_class_entry;

zend_function_entry com_dotnet_functions[] = {
	PHP_FE(variant_set, NULL)
	PHP_FE(variant_add, NULL)
	PHP_FE(variant_cat, NULL)
	PHP_FE(variant_sub, NULL)
	PHP_FE(variant_mul, NULL)
	PHP_FE(variant_and, NULL)
	PHP_FE(variant_div, NULL)
	PHP_FE(variant_eqv, NULL)
	PHP_FE(variant_idiv, NULL)
	PHP_FE(variant_imp, NULL)
	PHP_FE(variant_mod, NULL)
	PHP_FE(variant_or, NULL)
	PHP_FE(variant_pow, NULL)
	PHP_FE(variant_xor, NULL)
	PHP_FE(variant_abs, NULL)
	PHP_FE(variant_fix, NULL)
	PHP_FE(variant_int, NULL)
	PHP_FE(variant_neg, NULL)
	PHP_FE(variant_not, NULL)
	PHP_FE(variant_round, NULL)
	PHP_FE(variant_cmp, NULL)
	PHP_FE(variant_date_to_timestamp, NULL)
	PHP_FE(variant_date_from_timestamp, NULL)
	PHP_FE(variant_get_type, NULL)
	PHP_FE(variant_set_type, NULL)
	PHP_FE(variant_cast, NULL)
	/* com_com.c */
	PHP_FE(com_create_guid, NULL)
	PHP_FE(com_event_sink, NULL)
	PHP_FE(com_print_typeinfo, NULL)
	PHP_FE(com_message_pump, NULL)
	PHP_FE(com_load_typelib, NULL)
	PHP_FE(com_get_active_object, NULL)
	{ NULL, NULL, NULL }
};

/* {{{ com_dotnet_module_entry
 */
zend_module_entry com_dotnet_module_entry = {
	STANDARD_MODULE_HEADER,
	"com_dotnet",
	com_dotnet_functions,
	PHP_MINIT(com_dotnet),
	PHP_MSHUTDOWN(com_dotnet),
	PHP_RINIT(com_dotnet),
	PHP_RSHUTDOWN(com_dotnet),
	PHP_MINFO(com_dotnet),
	"0.1",
	PHP_MODULE_GLOBALS(com_dotnet),
	PHP_GINIT(com_dotnet),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_COM_DOTNET
ZEND_GET_MODULE(com_dotnet)
#endif

/* {{{ PHP_INI
 */

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
	int cached;

	if (!new_value || !new_value[0] || (typelib_file = VCWD_FOPEN(new_value, "r"))==NULL) {
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
				mode &= ~CONST_CS;
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

		if ((pTL = php_com_load_typelib_via_cache(typelib_name, COMG(code_page), &cached TSRMLS_CC)) != NULL) {
			if (!cached) {
				php_com_import_typelib(pTL, mode, COMG(code_page) TSRMLS_CC);
			}
			ITypeLib_Release(pTL);
		}
	}

	efree(typelib_name_buffer);
	fclose(typelib_file);

	return SUCCESS;
}

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("com.allow_dcom",				"0", PHP_INI_SYSTEM, OnUpdateBool, allow_dcom, zend_com_dotnet_globals, com_dotnet_globals)
    STD_PHP_INI_ENTRY("com.autoregister_verbose",	"0", PHP_INI_ALL, OnUpdateBool, autoreg_verbose, zend_com_dotnet_globals, com_dotnet_globals)
    STD_PHP_INI_ENTRY("com.autoregister_typelib",	"0", PHP_INI_ALL, OnUpdateBool, autoreg_on, zend_com_dotnet_globals, com_dotnet_globals)
    STD_PHP_INI_ENTRY("com.autoregister_casesensitive",	"1", PHP_INI_ALL, OnUpdateBool, autoreg_case_sensitive, zend_com_dotnet_globals, com_dotnet_globals)
	STD_PHP_INI_ENTRY("com.code_page", "", PHP_INI_ALL, OnUpdateLong, code_page, zend_com_dotnet_globals, com_dotnet_globals)
	PHP_INI_ENTRY("com.typelib_file", "", PHP_INI_SYSTEM, OnTypeLibFileUpdate)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(com_dotnet)
{
	memset(com_dotnet_globals, 0, sizeof(*com_dotnet_globals));
	com_dotnet_globals->code_page = CP_ACP;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(com_dotnet)
{
	zend_class_entry ce, *tmp;

	php_com_wrapper_minit(INIT_FUNC_ARGS_PASSTHRU);
	php_com_persist_minit(INIT_FUNC_ARGS_PASSTHRU);

	INIT_CLASS_ENTRY(ce, "com_exception", NULL);
	php_com_exception_class_entry = zend_register_internal_class_ex(&ce, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
	php_com_exception_class_entry->ce_flags |= ZEND_ACC_FINAL;
/*	php_com_exception_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */

	INIT_CLASS_ENTRY(ce, "com_safearray_proxy", NULL);
	php_com_saproxy_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	php_com_saproxy_class_entry->ce_flags |= ZEND_ACC_FINAL;
/*	php_com_saproxy_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */
	php_com_saproxy_class_entry->get_iterator = php_com_saproxy_iter_get;
	
	INIT_CLASS_ENTRY(ce, "variant", NULL);
	ce.create_object = php_com_object_new;
	php_com_variant_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	php_com_variant_class_entry->get_iterator = php_com_iter_get;

	INIT_CLASS_ENTRY(ce, "com", NULL);
	ce.create_object = php_com_object_new;
	tmp = zend_register_internal_class_ex(&ce, php_com_variant_class_entry, "variant" TSRMLS_CC);
	tmp->get_iterator = php_com_iter_get;

	zend_ts_hash_init(&php_com_typelibraries, 0, NULL, php_com_typelibrary_dtor, 1);

#if HAVE_MSCOREE_H
	INIT_CLASS_ENTRY(ce, "dotnet", NULL);
	ce.create_object = php_com_object_new;
	tmp = zend_register_internal_class_ex(&ce, php_com_variant_class_entry, "variant" TSRMLS_CC);
	tmp->get_iterator = php_com_iter_get;
#endif

	REGISTER_INI_ENTRIES();

#define COM_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_CS|CONST_PERSISTENT)
	
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

	COM_CONST(NORM_IGNORECASE);
	COM_CONST(NORM_IGNORENONSPACE);
	COM_CONST(NORM_IGNORESYMBOLS);
	COM_CONST(NORM_IGNOREWIDTH);
	COM_CONST(NORM_IGNOREKANATYPE);
#ifdef NORM_IGNOREKASHIDA
	COM_CONST(NORM_IGNOREKASHIDA);
#endif
	COM_CONST(DISP_E_DIVBYZERO);
	COM_CONST(DISP_E_OVERFLOW);
	COM_CONST(DISP_E_BADINDEX);
	COM_CONST(MK_E_UNAVAILABLE);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(com_dotnet)
{
	UNREGISTER_INI_ENTRIES();
#if HAVE_MSCOREE_H
	if (COMG(dotnet_runtime_stuff)) {
		php_com_dotnet_mshutdown(TSRMLS_C);
	}
#endif

	zend_ts_hash_destroy(&php_com_typelibraries);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(com_dotnet)
{
	COMG(rshutdown_started) = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(com_dotnet)
{
#if HAVE_MSCOREE_H
	if (COMG(dotnet_runtime_stuff)) {
		php_com_dotnet_rshutdown(TSRMLS_C);
	}
#endif
	COMG(rshutdown_started) = 1;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
