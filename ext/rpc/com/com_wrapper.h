#ifndef COM_H
#define COM_H

#if PHP_WIN32

#include "oleauto.h"

typedef struct i_dispatch_ {
	int typelib;
	struct {
		IDispatch *dispatch;
		ITypeInfo *typeinfo;
	} i;
} i_dispatch;

PHPAPI HRESULT php_COM_invoke(i_dispatch *obj, DISPID dispIdMember, WORD wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR*  pVarResult);
PHPAPI HRESULT php_COM_get_ids_of_names(i_dispatch *obj, OLECHAR FAR* FAR* rgszNames, DISPID FAR* rgDispId);
PHPAPI HRESULT php_COM_release(i_dispatch *obj);
PHPAPI HRESULT php_COM_set(i_dispatch *obj, IDispatch FAR* pDisp, int cleanup);
PHPAPI HRESULT php_COM_clone(i_dispatch *obj, i_dispatch *clone, int cleanup);

#endif  /* PHP_WIN32 */

#endif  /* COM_H */
