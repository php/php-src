#ifndef COM_H
#define COM_H

#if PHP_WIN32

#include "oleauto.h"

typedef struct comval_ {
	BOOL typelib;
	BOOL enumeration;
	int refcount;
	struct {
		IDispatch *dispatch;
		ITypeInfo *typeinfo;
		IEnumVARIANT *enumvariant;
	} i;
} comval;

PHPAPI HRESULT php_COM_invoke(comval *obj, DISPID dispIdMember, WORD wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR* pVarResult);
PHPAPI HRESULT php_COM_get_ids_of_names(comval *obj, OLECHAR FAR* FAR* rgszNames, DISPID FAR* rgDispId);
PHPAPI HRESULT php_COM_release(comval *obj);
PHPAPI HRESULT php_COM_addref(comval *obj);
PHPAPI HRESULT php_COM_set(comval *obj, IDispatch FAR* pDisp, int cleanup);
PHPAPI HRESULT php_COM_clone(comval *obj, comval *clone, int cleanup);

#define C_HASTLIB(x)	((x)->typelib)
#define C_HASENUM(x)	((x)->enumeration)
#define C_ISREFD(x)	((x)->refcount)

#define C_DISPATCH(x)	((x)->i.dispatch)
#define C_TYPEINFO(x)	((x)->i.typeinfo)
#define C_ENUMVARIANT(x)	((x)->i.enumvariant)

#define C_DISPATCH_VT(x)	(C_DISPATCH(x)->lpVtbl)
#define C_TYPEINFO_VT(x)	(C_TYPEINFO(x)->lpVtbl)
#define C_ENUMVARIANT_VT(x)	(C_ENUMVARIANT(x)->lpVtbl)

#define C_ADDREF(x)	(++((x)->refcount))
#define C_RELEASE(x)	(--((x)->refcount))

#endif  /* PHP_WIN32 */

#endif  /* COM_H */
