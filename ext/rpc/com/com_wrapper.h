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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   |         Harald Radi  <h.radi@nme.at>                                 |
   |         Alan Brown   <abrown@pobox.com>                              |
   |         Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

#ifndef COM_WRAPPER_H
#define COM_WRAPPER_H

#if PHP_WIN32

BEGIN_EXTERN_C()

typedef struct comval_ {
	BOOL typelib;
	BOOL enumeration;
	int refcount;
	int codepage;
	struct {
		IDispatch *dispatch;
		ITypeInfo *typeinfo;
		IEnumVARIANT *enumvariant;
	} i;

#if 1
	IDispatch *sinkdispatch;
	GUID sinkid;
	DWORD sinkcookie;
#endif

#ifdef _DEBUG
	int resourceindex;
#endif	

} comval;

ZEND_API HRESULT	php_COM_invoke(comval *obj, DISPID dispIdMember, WORD wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR* pVarResult, char **ErrString);
ZEND_API HRESULT	php_COM_get_ids_of_names(comval *obj, OLECHAR FAR* rgszNames, DISPID FAR* rgDispId);
ZEND_API HRESULT	php_COM_release(comval *obj);
ZEND_API HRESULT	php_COM_addref(comval *obj);
ZEND_API HRESULT	php_COM_destruct(comval *obj);
ZEND_API HRESULT	php_COM_set(comval *obj, IDispatch FAR* FAR* pDisp, int cleanup);
ZEND_API HRESULT	php_COM_enable_events(comval *obj, int enable);

ZEND_API char*		php_COM_string_from_CLSID(const CLSID *clsid);
ZEND_API char*		php_COM_error_message(HRESULT hr);
ZEND_API int		php_COM_load_typelib(ITypeLib *TypeLib, int mode);
ZEND_API int		php_COM_process_typeinfo(ITypeInfo *typeinfo, HashTable *id_to_name, int printdef, GUID *guid);
ZEND_API ITypeInfo*	php_COM_locate_typeinfo(char *typelibname, comval *obj, char *dispname, int sink);
ZEND_API ITypeLib*	php_COM_find_typelib(char *search_string, int mode);

ZEND_API IDispatch*	php_COM_export_as_sink(zval *val, GUID *sinkid, HashTable *id_to_name);
ZEND_API IDispatch*	php_COM_export_object(zval *val);
ZEND_API zval*		php_COM_object_from_dispatch(IDispatch *disp);

END_EXTERN_C()

#define ZVAL_COM(z,o) {															\
			rpc_internal *intern;												\
			Z_TYPE_P(z) = IS_OBJECT;											\
			(z)->value.obj = rpc_objects_new(com_class_entry TSRMLS_CC);		\
			if (GET_INTERNAL_EX(intern, (z)) != SUCCESS) {						\
				/* TODO: exception */											\
			}																	\
			intern->data = (o);													\
		}

#define RETVAL_COM(o)	ZVAL_COM(&return_value, o);
#define RETURN_COM(o)	RETVAL_COM(o)											\
						return;

#define ALLOC_COM(z)	(z) = (comval *) ecalloc(1, sizeof(comval));			\
						C_CODEPAGE(z) = CP_ACP;

#define FREE_COM(z)		php_COM_destruct(z);

#define C_CODEPAGE(x)	((x)->codepage)

#define C_HASTLIB(x)	((x)->typelib)
#define C_HASENUM(x)	((x)->enumeration)

#define C_DISPATCH(x)		((x)->i.dispatch)
#define C_TYPEINFO(x)		((x)->i.typeinfo)
#define C_ENUMVARIANT(x)	((x)->i.enumvariant)

#define C_DISPATCH_VT(x)	(C_DISPATCH(x)->lpVtbl)
#define C_TYPEINFO_VT(x)	(C_TYPEINFO(x)->lpVtbl)
#define C_ENUMVARIANT_VT(x)	(C_ENUMVARIANT(x)->lpVtbl)

#endif  /* PHP_WIN32 */

#endif  /* COM_H */
