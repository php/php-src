#ifndef COM_H
#define COM_H

#if PHP_WIN32

BEGIN_EXTERN_C()

#include <oleauto.h>

typedef struct comval_ {
#ifdef _DEBUG
	int resourceindex;
#endif	
	BOOL typelib;
	BOOL enumeration;
	int refcount;
	struct {
		IDispatch *dispatch;
		ITypeInfo *typeinfo;
		IEnumVARIANT *enumvariant;
	} i;
	IDispatch *sinkdispatch;
	GUID sinkid;
	DWORD sinkcookie;
} comval;

END_EXTERN_C()

#define ZVAL_COM(z,o) {																\
			zval *handle = NULL;													\
			ZVAL_COM_EX(z,o,handle)													\
		}

#define ZVAL_COM_EX(z,o,handle) {													\
			HashTable *properties;													\
																					\
			ALLOC_HASHTABLE(properties);											\
			zend_hash_init(properties, 0, NULL, ZVAL_PTR_DTOR, 0);					\
																					\
			if (handle == NULL) {													\
				MAKE_STD_ZVAL(handle);												\
			}																		\
			ZVAL_RESOURCE(handle, zend_list_insert((o), IS_COM));						\
																					\
			zval_copy_ctor(handle);													\
			zend_hash_index_update(properties, 0, &handle, sizeof(zval *), NULL);	\
			object_and_properties_init(z, &COM_class_entry, properties);			\
		}

#define RETVAL_COM(o)	ZVAL_COM(&return_value, o);
#define RETURN_COM(o)	RETVAL_COM(o)												\
						return;

#define ALLOC_COM(z)	(z) = (comval *) ecalloc(1, sizeof(comval));					\
						C_REFCOUNT(z) = 0;

#define FREE_COM(z)		php_COM_destruct(z TSRMLS_CC);

#define IS_COM			php_COM_get_le_comval()

#define C_HASTLIB(x)	((x)->typelib)
#define C_HASENUM(x)	((x)->enumeration)
#define C_REFCOUNT(x)	((x)->refcount)
#define C_ISREFD(x)		C_REFCOUNT(x)

#define C_ADDREF(x)		(++((x)->refcount))
#define C_RELEASE(x)	(--((x)->refcount))

#define C_DISPATCH(x)		((x)->i.dispatch)
#define C_TYPEINFO(x)		((x)->i.typeinfo)
#define C_ENUMVARIANT(x)	((x)->i.enumvariant)

#define C_DISPATCH_VT(x)	(C_DISPATCH(x)->lpVtbl)
#define C_TYPEINFO_VT(x)	(C_TYPEINFO(x)->lpVtbl)
#define C_ENUMVARIANT_VT(x)	(C_ENUMVARIANT(x)->lpVtbl)

#endif  /* PHP_WIN32 */

#endif  /* COM_H */
