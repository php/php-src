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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_COM_DOTNET_INTERNAL_H
#define PHP_COM_DOTNET_INTERNAL_H

#define _WIN32_DCOM
#define COBJMACROS
#include <ocidl.h>
#include <oleauto.h>
#include <unknwn.h>
#include <dispex.h>
#include "win32/winutil.h"

typedef struct _php_com_dotnet_object {
	zend_object zo;

	VARIANT v;
	int modified;

	int code_page;

	ITypeInfo *typeinfo;

	zend_class_entry *ce;

   	/* associated event sink */
	IDispatch *sink_dispatch;
	GUID sink_id;
	DWORD sink_cookie;

	/* cache for method signatures */
	HashTable *method_cache;
	/* cache for name -> DISPID */
	HashTable *id_of_name_cache;
} php_com_dotnet_object;

static inline int php_com_is_valid_object(zval *zv)
{
	zend_class_entry *ce = Z_OBJCE_P(zv);
	return strcmp("com", ce->name->val) == 0 ||
		strcmp("dotnet", ce->name->val) == 0 ||
		strcmp("variant", ce->name->val) == 0;
}

#define CDNO_FETCH(zv)			(php_com_dotnet_object*)Z_OBJ_P(zv)
#define CDNO_FETCH_VERIFY(obj, zv)	do { \
	if (!php_com_is_valid_object(zv)) { \
		php_com_throw_exception(E_UNEXPECTED, "expected a variant object"); \
		return; \
	} \
	obj = (php_com_dotnet_object*)Z_OBJ_P(zv); \
} while(0)

/* com_extension.c */
zend_class_entry *php_com_variant_class_entry, *php_com_exception_class_entry, *php_com_saproxy_class_entry;

/* com_handlers.c */
zend_object* php_com_object_new(zend_class_entry *ce);
zend_object* php_com_object_clone(zend_object *object);
void php_com_object_free_storage(zend_object *object);
zend_object_handlers php_com_object_handlers;
void php_com_object_enable_event_sink(php_com_dotnet_object *obj, int enable);

/* com_saproxy.c */
zend_object_iterator *php_com_saproxy_iter_get(zend_class_entry *ce, zval *object, int by_ref);
int php_com_saproxy_create(zend_object *com_object, zval *proxy_out, zval *index);

/* com_olechar.c */
PHP_COM_DOTNET_API char *php_com_olestring_to_string(OLECHAR *olestring,
		size_t *string_len, int codepage);
PHP_COM_DOTNET_API OLECHAR *php_com_string_to_olestring(const char *string,
		size_t string_len, int codepage);
BSTR php_com_string_to_bstr(zend_string *string, int codepage);
zend_string *php_com_bstr_to_string(BSTR bstr, int codepage);


/* com_com.c */
PHP_METHOD(com, __construct);

HRESULT php_com_invoke_helper(php_com_dotnet_object *obj, DISPID id_member,
		WORD flags, DISPPARAMS *disp_params, VARIANT *v, int silent, int allow_noarg);
HRESULT php_com_get_id_of_name(php_com_dotnet_object *obj, char *name,
		size_t namelen, DISPID *dispid);
int php_com_do_invoke_by_id(php_com_dotnet_object *obj, DISPID dispid,
		WORD flags,	VARIANT *v, int nargs, zval *args, int silent, int allow_noarg);
int php_com_do_invoke(php_com_dotnet_object *obj, char *name, size_t namelen,
		WORD flags,	VARIANT *v, int nargs, zval *args, int allow_noarg);
int php_com_do_invoke_byref(php_com_dotnet_object *obj, zend_internal_function *f,
		WORD flags,	VARIANT *v, int nargs, zval *args);

/* com_wrapper.c */
int php_com_wrapper_minit(INIT_FUNC_ARGS);
PHP_COM_DOTNET_API IDispatch *php_com_wrapper_export_as_sink(zval *val, GUID *sinkid, HashTable *id_to_name);
PHP_COM_DOTNET_API IDispatch *php_com_wrapper_export(zval *val);

/* com_persist.c */
int php_com_persist_minit(INIT_FUNC_ARGS);

/* com_variant.c */
PHP_METHOD(variant, __construct);

PHP_COM_DOTNET_API void php_com_variant_from_zval_with_type(VARIANT *v, zval *z, VARTYPE type, int codepage);
PHP_COM_DOTNET_API void php_com_variant_from_zval(VARIANT *v, zval *z, int codepage);
PHP_COM_DOTNET_API int php_com_zval_from_variant(zval *z, VARIANT *v, int codepage);
PHP_COM_DOTNET_API int php_com_copy_variant(VARIANT *dst, VARIANT *src);

/* com_dotnet.c */
PHP_METHOD(dotnet, __construct);
void php_com_dotnet_rshutdown(void);
void php_com_dotnet_mshutdown(void);

/* com_misc.c */
void php_com_throw_exception(HRESULT code, char *message);
PHP_COM_DOTNET_API void php_com_wrap_dispatch(zval *z, IDispatch *disp,
		int codepage);
PHP_COM_DOTNET_API void php_com_wrap_variant(zval *z, VARIANT *v,
		int codepage);
PHP_COM_DOTNET_API int php_com_safearray_get_elem(VARIANT *array, VARIANT *dest, LONG dim1);

/* com_typeinfo.c */
PHP_COM_DOTNET_API ITypeLib *php_com_load_typelib_via_cache(const char *search_string, int codepage);
PHP_COM_DOTNET_API ITypeLib *php_com_load_typelib(char *search_string, int codepage);
PHP_COM_DOTNET_API int php_com_import_typelib(ITypeLib *TL, int mode,
		int codepage);
void php_com_typelibrary_dtor(zval *pDest);
ITypeInfo *php_com_locate_typeinfo(char *typelibname, php_com_dotnet_object *obj, char *dispname, int sink);
int php_com_process_typeinfo(ITypeInfo *typeinfo, HashTable *id_to_name, int printdef, GUID *guid, int codepage);
ITypeLib *php_com_cache_typelib(ITypeLib* TL, char *cache_key, zend_long cache_key_len);
PHP_MINIT_FUNCTION(com_typeinfo);
PHP_MSHUTDOWN_FUNCTION(com_typeinfo);

/* com_iterator.c */
zend_object_iterator *php_com_iter_get(zend_class_entry *ce, zval *object, int by_ref);


#endif
