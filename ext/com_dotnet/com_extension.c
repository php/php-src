/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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
#include "Zend/zend_default_classes.h"

ZEND_DECLARE_MODULE_GLOBALS(com_dotnet)
TsHashTable php_com_typelibraries;
zend_class_entry *php_com_variant_class_entry;

function_entry com_dotnet_functions[] = {
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
	PHP_FE(com_create_guid, NULL)
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
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_COM_DOTNET
ZEND_GET_MODULE(com_dotnet)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("com.allow_dcom",				"0", PHP_INI_SYSTEM, OnUpdateBool, allow_dcom, zend_com_dotnet_globals, com_dotnet_globals)
    STD_PHP_INI_ENTRY("com.autoregister_verbose",	"0", PHP_INI_ALL, OnUpdateBool, autoreg_verbose, zend_com_dotnet_globals, com_dotnet_globals)
    STD_PHP_INI_ENTRY("com.autoregister_typelib",	"0", PHP_INI_ALL, OnUpdateBool, autoreg_on, zend_com_dotnet_globals, com_dotnet_globals)
    STD_PHP_INI_ENTRY("com.autoregister_casesensitive",	"0", PHP_INI_ALL, OnUpdateBool, autoreg_case_sensitive, zend_com_dotnet_globals, com_dotnet_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_com_dotnet_init_globals
 */
static void php_com_dotnet_init_globals(zend_com_dotnet_globals *com_dotnet_globals)
{
	memset(com_dotnet_globals, 0, sizeof(*com_dotnet_globals));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(com_dotnet)
{
	zend_class_entry ce;

	ZEND_INIT_MODULE_GLOBALS(com_dotnet, php_com_dotnet_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	INIT_CLASS_ENTRY(ce, "variant", NULL);
	ce.create_object = php_com_object_new;
	ce.get_iterator = php_com_iter_get;
	php_com_variant_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "com", NULL);
	ce.create_object = php_com_object_new;
	ce.get_iterator = php_com_iter_get;
	zend_register_internal_class_ex(&ce, php_com_variant_class_entry, "variant" TSRMLS_CC);

	zend_ts_hash_init(&php_com_typelibraries, 0, NULL, php_com_typelibrary_dtor, 1);

#if HAVE_MSCOREE_H
	INIT_CLASS_ENTRY(ce, "dotnet", NULL);
	ce.create_object = php_com_object_new;
	ce.get_iterator = php_com_iter_get;
	zend_register_internal_class_ex(&ce, php_com_variant_class_entry, "variant" TSRMLS_CC);
#endif

#define COM_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_CS|CONST_PERSISTENT)
	
	COM_CONST(CLSCTX_INPROC_SERVER);
	COM_CONST(CLSCTX_INPROC_HANDLER);
	COM_CONST(CLSCTX_LOCAL_SERVER);
	COM_CONST(CLSCTX_REMOTE_SERVER);
	COM_CONST(CLSCTX_SERVER);
	COM_CONST(CLSCTX_ALL);

	COM_CONST(DISPATCH_METHOD);
	COM_CONST(DISPATCH_PROPERTYGET);
	COM_CONST(DISPATCH_PROPERTYPUT);

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

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(com_dotnet)
{
	UNREGISTER_INI_ENTRIES();
	zend_ts_hash_destroy(&php_com_typelibraries);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(com_dotnet)
{
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
