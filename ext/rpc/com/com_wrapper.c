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
/* $Id$ */
/*
 * This module implements support for COM components that support the IDispatch
 * interface.  Both local (COM) and remote (DCOM) components can be accessed.
 *
 * Type libraries can be loaded (in order for PHP to recognize automation constants)
 * by specifying a typelib_file in the PHP .ini file.  That file should contain
 * paths to type libraries, one in every line.  By default, constants are registered
 * as case-sensitive.  If you want them to be defined as case-insensitive, add
 * #case_insensitive or #cis at the end of the type library path.
 *
 * This is also the first module to demonstrate Zend's OO syntax overloading
 * capabilities.  CORBA coders are invited to write a CORBA module as well!
 *
 * Zeev
 */

/*
 * 28.12.2000
 * unicode conversion fixed by Harald Radi <h.radi@nme.at>
 *
 * now all these strange '?'s should be disapeared
 */

/*
 * 28.1.2001
 * VARIANT datatype and pass_by_reference support
 */

/*
 * 03.6.2001
 * Enhanced Typelib support to include a search by name
 */

#ifdef PHP_WIN32

#define _WIN32_DCOM

#include <iostream.h>
#include <math.h>
#include <ocidl.h>

#include "php.h"
#include "php_ini.h"

#include "../rpc.h"
#include "../php_rpc.h"
#include "../handler.h"

#include "com.h"
#include "com_wrapper.h"
#include "conversion.h"
#include "variant.h"

#ifdef _DEBUG
int resourcecounter = 1;
#endif

ZEND_API HRESULT php_COM_invoke(comval *obj, DISPID dispIdMember, WORD wFlags,
	DISPPARAMS FAR*  pDispParams, VARIANT FAR*  pVarResult, char **ErrString)
{
	HRESULT hr;
	int failed = FALSE;
	unsigned int ArgErr = 0;
	EXCEPINFO ExceptInfo;

	*ErrString = NULL;
	/* @todo use DispInvoke here ? */
	if (C_HASTLIB(obj)) {
		hr = C_TYPEINFO_VT(obj)->Invoke(C_TYPEINFO(obj), C_DISPATCH(obj),
			dispIdMember, wFlags, pDispParams, pVarResult, &ExceptInfo, &ArgErr);
		if (FAILED(hr)) {
			hr = C_DISPATCH_VT(obj)->Invoke(C_DISPATCH(obj), dispIdMember, &IID_NULL,
				LOCALE_SYSTEM_DEFAULT, wFlags, pDispParams, pVarResult, &ExceptInfo, &ArgErr);
			if (SUCCEEDED(hr)) {
				/*
				 * ITypLib doesn't work
				 * Release ITypeLib and fall back to IDispatch
				 */

				C_TYPEINFO_VT(obj)->Release(C_TYPEINFO(obj));
				C_HASTLIB(obj) = FALSE;
			}
		}
	} else {
		hr = C_DISPATCH_VT(obj)->Invoke(C_DISPATCH(obj), dispIdMember, &IID_NULL,
			LOCALE_SYSTEM_DEFAULT, wFlags, pDispParams, pVarResult, &ExceptInfo, &ArgErr);
	}

	if (FAILED(hr)) {
		switch (hr) {
			case DISP_E_EXCEPTION: {
					
					char *src=estrdup("Unavailable");
					int srclen=strlen(src);
					char *desc=estrdup("Unavailable");
					int desclen=strlen(desc);

					if (ExceptInfo.bstrSource)
					{
						efree(src);
						src = php_OLECHAR_to_char(ExceptInfo.bstrSource, &srclen, C_CODEPAGE(obj), FALSE);
						SysFreeString(ExceptInfo.bstrSource);
					}
					if (ExceptInfo.bstrDescription)
					{
						efree(desc);
						desc = php_OLECHAR_to_char(ExceptInfo.bstrDescription, &desclen, C_CODEPAGE(obj), FALSE);
						SysFreeString(ExceptInfo.bstrDescription);
					}
					
					spprintf(ErrString, 0, "<b>Source</b>: %s <b>Description</b>: %s", src, desc);
					efree(src);
					efree(desc);
					
					if (ExceptInfo.bstrHelpFile)
					{
						SysFreeString(ExceptInfo.bstrHelpFile);
					}
				}
				break;
			case DISP_E_PARAMNOTFOUND:
			case DISP_E_TYPEMISMATCH:
				spprintf(ErrString, 0, "<b>Argument</b>: %d", pDispParams->cArgs - ArgErr);
				break;
		}
	}

	if (pVarResult && (V_VT(pVarResult) == VT_EMPTY)) {
		V_VT(pVarResult) = VT_I4;
		V_I4(pVarResult) = hr;
	}

	return hr;
}


ZEND_API HRESULT php_COM_get_ids_of_names(comval *obj, OLECHAR FAR* rgszNames, DISPID FAR* rgDispId)
{
	HRESULT hr;

	if (C_HASTLIB(obj)) {
		hr = C_TYPEINFO_VT(obj)->GetIDsOfNames(C_TYPEINFO(obj), &rgszNames, 1, rgDispId);

		if (FAILED(hr)) {
			hr = C_DISPATCH_VT(obj)->GetIDsOfNames(C_DISPATCH(obj), &IID_NULL, &rgszNames, 1, LOCALE_SYSTEM_DEFAULT, rgDispId);

			if (SUCCEEDED(hr)) {
				/*
				 * ITypLib doesn't work
				 * Release ITypeLib and fall back to IDispatch
				 */

				C_TYPEINFO_VT(obj)->Release(C_TYPEINFO(obj));
				C_HASTLIB(obj) = FALSE;
			}
		}
	} else {
		hr = C_DISPATCH_VT(obj)->GetIDsOfNames(C_DISPATCH(obj), &IID_NULL, &rgszNames, 1, LOCALE_SYSTEM_DEFAULT, rgDispId);
	}

	return hr;
}


ZEND_API HRESULT php_COM_release(comval *obj)
{
	return C_DISPATCH_VT(obj)->Release(C_DISPATCH(obj));
}


ZEND_API HRESULT php_COM_addref(comval *obj)
{
	return C_DISPATCH_VT(obj)->AddRef(C_DISPATCH(obj));
}


ZEND_API HRESULT php_COM_set(comval *obj, IDispatch FAR* FAR* ppDisp, int cleanup)
{
	HRESULT hr = 1;
	DISPPARAMS dispparams;
	VARIANT *result;
	IDispatch FAR* pDisp;

	pDisp = *ppDisp;
	if (cleanup) {
		*ppDisp = NULL;
	}

	C_DISPATCH(obj) = pDisp;
	C_HASTLIB(obj) = SUCCEEDED(C_DISPATCH_VT(obj)->GetTypeInfo(C_DISPATCH(obj), 0, LANG_NEUTRAL, &C_TYPEINFO(obj)));
	
	dispparams.rgvarg = NULL;
	dispparams.rgdispidNamedArgs = NULL;
	dispparams.cArgs = 0;
	dispparams.cNamedArgs = 0;

	result = (VARIANT *) emalloc(sizeof(VARIANT));
	VariantInit(result);

	if (C_HASENUM(obj) = SUCCEEDED(C_DISPATCH_VT(obj)->Invoke(C_DISPATCH(obj), DISPID_NEWENUM, &IID_NULL, LOCALE_SYSTEM_DEFAULT,
															  DISPATCH_METHOD|DISPATCH_PROPERTYGET, &dispparams, result, NULL, NULL))) {
		if (V_VT(result) == VT_UNKNOWN) {
			V_UNKNOWN(result)->lpVtbl->AddRef(V_UNKNOWN(result));
			C_HASENUM(obj) = SUCCEEDED(V_UNKNOWN(result)->lpVtbl->QueryInterface(V_UNKNOWN(result), &IID_IEnumVARIANT,
																					 (void**)&C_ENUMVARIANT(obj)));
		} else if (V_VT(result) == VT_DISPATCH) {
			V_DISPATCH(result)->lpVtbl->AddRef(V_DISPATCH(result));
			C_HASENUM(obj) = SUCCEEDED(V_DISPATCH(result)->lpVtbl->QueryInterface(V_DISPATCH(result), &IID_IEnumVARIANT,
																					  (void**)&C_ENUMVARIANT(obj)));
		}
	}

	efree(result);

	if (!cleanup) {
		hr = C_DISPATCH_VT(obj)->AddRef(C_DISPATCH(obj));
	}

#ifdef _DEBUG
	obj->resourceindex = resourcecounter++;
#endif

	return hr;
}




ZEND_API char *php_COM_error_message(HRESULT hr)
{
	void *pMsgBuf = NULL;

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
					   hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &pMsgBuf, 0, NULL)) {
		char error_string[] = "No description available";

		pMsgBuf = LocalAlloc(LMEM_FIXED, sizeof(error_string));
		memcpy(pMsgBuf, error_string, sizeof(error_string));
	}

	return pMsgBuf;
}


ZEND_API char *php_COM_string_from_CLSID(const CLSID *clsid)
{
	LPOLESTR ole_clsid;
	char *clsid_str;

	StringFromCLSID(clsid, &ole_clsid);
	clsid_str = php_OLECHAR_to_char(ole_clsid, NULL, CP_ACP, FALSE);
	CoTaskMemFree(ole_clsid);

	return clsid_str;
}


ZEND_API HRESULT php_COM_destruct(comval *obj)
{
	HRESULT hr = S_OK;

	php_COM_enable_events(obj, FALSE);
	if (obj->sinkdispatch)
		obj->sinkdispatch->lpVtbl->Release(obj->sinkdispatch);
	
	if (C_HASTLIB(obj)) {
		C_TYPEINFO_VT(obj)->Release(C_TYPEINFO(obj));
	}
	if (C_HASENUM(obj)) {
		C_ENUMVARIANT_VT(obj)->Release(C_ENUMVARIANT(obj));
	}

	if (C_DISPATCH(obj)) {
		hr = C_DISPATCH_VT(obj)->Release(C_DISPATCH(obj));
	}
	efree(obj);

	return hr;
}








/* {{{ proto bool com_message_pump([int timeoutms])
   Process COM messages, sleeping for up to timeoutms milliseconds */
PHP_FUNCTION(com_message_pump)
{
	long timeoutms = 0;
	MSG msg;
	DWORD result;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timeoutms) == FAILURE)
		RETURN_FALSE;
	
	result = MsgWaitForMultipleObjects(0, NULL, FALSE, timeoutms, QS_ALLINPUT);

	if (result == WAIT_OBJECT_0) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/* we processed messages */
		RETVAL_TRUE;
	} else {
		/* we did not process messages (timed out) */
		RETVAL_FALSE;
	}
}
/* }}} */

ZEND_API HRESULT php_COM_enable_events(comval *obj, int enable)
{
	if (obj->sinkdispatch) {
		IConnectionPointContainer *cont;
		IConnectionPoint *point;
		
		if (SUCCEEDED(C_DISPATCH_VT(obj)->QueryInterface(C_DISPATCH(obj), &IID_IConnectionPointContainer, (void**)&cont))) {
			if (SUCCEEDED(cont->lpVtbl->FindConnectionPoint(cont, &obj->sinkid, &point))) {
				if (enable) {
					point->lpVtbl->Advise(point, (IUnknown*)obj->sinkdispatch, &obj->sinkcookie);
				} else {
					point->lpVtbl->Unadvise(point, obj->sinkcookie);
				}
				point->lpVtbl->Release(point);
			}
			cont->lpVtbl->Release(cont);
		}
	}

	return S_OK;
}

static const struct {
	VARTYPE vt;
	const char *name;
} vt_names[] = {
	{ VT_NULL,		"VT_NULL" },
	{ VT_EMPTY,		"VT_EMPTY" },
	{ VT_UI1,		"VT_UI1" },
	{ VT_I2,		"VT_I2" },
	{ VT_I4,		"VT_I4" },
	{ VT_R4,		"VT_R4" },
	{ VT_R8,		"VT_R8" },
	{ VT_BOOL,		"VT_BOOL" },
	{ VT_ERROR,		"VT_ERROR" },
	{ VT_CY,		"VT_CY" },
	{ VT_DATE,		"VT_DATE" },
	{ VT_BSTR,		"VT_BSTR" },
	{ VT_DECIMAL,	"VT_DECIMAL" },
	{ VT_UNKNOWN,	"VT_UNKNOWN" },
	{ VT_DISPATCH,	"VT_DISPATCH" },
	{ VT_VARIANT,	"VT_VARIANT" },
	{ VT_I1,		"VT_I1" },
	{ VT_UI2,		"VT_UI2" },
	{ VT_UI4,		"VT_UI4" },
	{ VT_INT,		"VT_INT" },
	{ VT_UINT,		"VT_UINT" },
	{ VT_ARRAY,		"VT_ARRAY" },
	{ VT_BYREF,		"VT_BYREF" },
	{ VT_VOID,		"VT_VOID" },
	{ VT_PTR,		"VT_PTR" },
	{ VT_HRESULT,	"VT_HRESULT" },
	{ 0, NULL }
};

static inline const char *vt_to_string(VARTYPE vt)
{
	int i;
	for (i = 0; vt_names[i].name != NULL; i++) {
		if (vt_names[i].vt == vt)
			return vt_names[i].name;
	}
	return "?";
}

ZEND_API int php_COM_process_typeinfo(ITypeInfo *typeinfo, HashTable *id_to_name, int printdef, GUID *guid)
{
	TYPEATTR *attr;
	FUNCDESC *func;
	int i;
	OLECHAR *olename;
	char *ansiname = NULL;
	unsigned int ansinamelen;
	int ret = 0;

	if (FAILED(typeinfo->lpVtbl->GetTypeAttr(typeinfo, &attr)))
		return 0;

	/* verify that it is suitable */
	if (id_to_name == NULL || attr->typekind == TKIND_DISPATCH) {

		if (guid)
			memcpy(guid, &attr->guid, sizeof(GUID));
		
		if (printdef) {
			char *guidstring;

			typeinfo->lpVtbl->GetDocumentation(typeinfo, MEMBERID_NIL, &olename, NULL, NULL, NULL);
			ansiname = php_OLECHAR_to_char(olename, &ansinamelen, CP_ACP, FALSE);
			SysFreeString(olename);

			guidstring = php_COM_string_from_CLSID(&attr->guid);
			php_printf("class %s { /* GUID=%s */\n", ansiname, guidstring);
			efree(guidstring);

			efree(ansiname);
		}

		if (id_to_name)
			zend_hash_init(id_to_name, 0, NULL, ZVAL_PTR_DTOR, 0);

		/* So we've got the dispatch interface; lets list the event methods */
		for (i = 0; i < attr->cFuncs; i++) {
			zval *tmp;
			DISPID lastid = 0;	/* for props */
			int isprop;

			if (FAILED(typeinfo->lpVtbl->GetFuncDesc(typeinfo, i, &func)))
				break;

			isprop = (func->invkind & DISPATCH_PROPERTYGET || func->invkind & DISPATCH_PROPERTYPUT);

			if (!isprop || lastid != func->memid) {

				lastid = func->memid;
				
				typeinfo->lpVtbl->GetDocumentation(typeinfo, func->memid, &olename, NULL, NULL, NULL);
				ansiname = php_OLECHAR_to_char(olename, &ansinamelen, CP_ACP, FALSE);
				SysFreeString(olename);

				if (printdef) {
					int j;
					char *funcdesc;
					unsigned int funcdesclen, cnames = 0;
					BSTR *names;

					names = (BSTR*)emalloc((func->cParams + 1) * sizeof(BSTR));

					typeinfo->lpVtbl->GetNames(typeinfo, func->memid, names, func->cParams + 1, &cnames);
					/* first element is the function name */
					SysFreeString(names[0]);

					php_printf("\t/* DISPID=%d */\n", func->memid);

					if (func->elemdescFunc.tdesc.vt != VT_VOID) {
						php_printf("\t/* %s [%d] */\n",
								vt_to_string(func->elemdescFunc.tdesc.vt),
								func->elemdescFunc.tdesc.vt
								);
					}

					if (isprop) {

						typeinfo->lpVtbl->GetDocumentation(typeinfo, func->memid, NULL, &olename, NULL, NULL);
						if (olename) {
							funcdesc = php_OLECHAR_to_char(olename, &funcdesclen, CP_ACP, FALSE);
							SysFreeString(olename);
							php_printf("\t/* %s */\n", funcdesc);
							efree(funcdesc);
						}

						php_printf("\tvar $%s;\n\n", ansiname);

					} else {
						/* a function */

						php_printf("\tfunction %s(\n", ansiname);

						for (j = 0; j < func->cParams; j++) {
							ELEMDESC *elem = &func->lprgelemdescParam[j];

							php_printf("\t\t/* %s [%d] ", vt_to_string(elem->tdesc.vt), elem->tdesc.vt);

							if (elem->paramdesc.wParamFlags & PARAMFLAG_FIN)
								php_printf("[in]");
							if (elem->paramdesc.wParamFlags & PARAMFLAG_FOUT)
								php_printf("[out]");

							if (elem->tdesc.vt == VT_PTR) {
								/* what does it point to ? */
								php_printf(" --> %s [%d] ",
										vt_to_string(elem->tdesc.lptdesc->vt),
										elem->tdesc.lptdesc->vt
										);
							}

							/* when we handle prop put and get, this will look nicer */
							if (j+1 < (int)cnames) {
								funcdesc = php_OLECHAR_to_char(names[j+1], &funcdesclen, CP_ACP, FALSE);
								SysFreeString(names[j+1]);
							} else {
								funcdesc = "???";
							}

							php_printf(" */ %s%s%c\n",
									elem->tdesc.vt == VT_PTR ? "&$" : "$",
									funcdesc,
									j == func->cParams - 1 ? ' ' : ','
									);

							if (j+1 < (int)cnames)
								efree(funcdesc);
						}

						php_printf("\t\t)\n\t{\n");

						typeinfo->lpVtbl->GetDocumentation(typeinfo, func->memid, NULL, &olename, NULL, NULL);
						if (olename) {
							funcdesc = php_OLECHAR_to_char(olename, &funcdesclen, CP_ACP, FALSE);
							SysFreeString(olename);
							php_printf("\t\t/* %s */\n", funcdesc);
							efree(funcdesc);
						}

						php_printf("\t}\n");
					}

					efree(names);
				}

				if (id_to_name) {
					zend_str_tolower(ansiname, ansinamelen);
					MAKE_STD_ZVAL(tmp);
					ZVAL_STRINGL(tmp, ansiname, ansinamelen, 0);
					zend_hash_index_update(id_to_name, func->memid, (void*)&tmp, sizeof(zval *), NULL);
				}
			}
			typeinfo->lpVtbl->ReleaseFuncDesc(typeinfo, func);

		}

		if (printdef)
			php_printf("}\n");

		ret = 1;
	} else {
		zend_error(E_WARNING, "That's not a dispatchable interface!! type kind = %08x", attr->typekind);
	}

	typeinfo->lpVtbl->ReleaseTypeAttr(typeinfo, attr);

	return ret;
}

ZEND_API ITypeInfo *php_COM_locate_typeinfo(char *typelibname, comval *obj, char *dispname, int sink)
{
	ITypeInfo *typeinfo = NULL;
	ITypeLib *typelib = NULL;
	int gotguid = 0;
	GUID iid;

	if (obj) {
		if (dispname == NULL && sink) {
			IProvideClassInfo2 *pci2;
			IProvideClassInfo *pci;

			if (SUCCEEDED(C_DISPATCH_VT(obj)->QueryInterface(C_DISPATCH(obj), &IID_IProvideClassInfo2, (void**)&pci2))) {
				gotguid = SUCCEEDED(pci2->lpVtbl->GetGUID(pci2, GUIDKIND_DEFAULT_SOURCE_DISP_IID, &iid));
				pci2->lpVtbl->Release(pci2);
			}
			if (!gotguid && SUCCEEDED(C_DISPATCH_VT(obj)->QueryInterface(C_DISPATCH(obj), &IID_IProvideClassInfo, (void**)&pci))) {
				/* examine the available interfaces */
				/* TODO: write some code here */
				pci->lpVtbl->Release(pci);
			}
		} else if (dispname && C_HASTLIB(obj)) {
			unsigned int idx;
			/* get the library from the object; the rest will be dealt with later */
			C_TYPEINFO_VT(obj)->GetContainingTypeLib(C_TYPEINFO(obj), &typelib, &idx);	
		} else if (typelibname == NULL) {
			C_DISPATCH_VT(obj)->GetTypeInfo(C_DISPATCH(obj), 0, LANG_NEUTRAL, &typeinfo);
		}
	} else if (typelibname) {
		/* Fetch the typelibrary and use that to look things up */
		typelib = php_COM_find_typelib(typelibname, CONST_CS);
	} 

	if (!gotguid && dispname && typelib) {
		unsigned short cfound;
		MEMBERID memid;
		OLECHAR *olename = php_char_to_OLECHAR(dispname, strlen(dispname), CP_ACP, FALSE);
		
		cfound = 1;
		if (FAILED(typelib->lpVtbl->FindName(typelib, olename, 0, &typeinfo, &memid, &cfound)) || cfound == 0) {
			CLSID coclass;
			ITypeInfo *coinfo;
			
			/* assume that it might be a progid instead */
			if (SUCCEEDED(CLSIDFromProgID(olename, &coclass)) &&
					SUCCEEDED(typelib->lpVtbl->GetTypeInfoOfGuid(typelib, &coclass, &coinfo))) {

				/* enumerate implemented interfaces and pick the one as indicated by sink */
				TYPEATTR *attr;
				int i;

				coinfo->lpVtbl->GetTypeAttr(coinfo, &attr);

				for (i = 0; i < attr->cImplTypes; i++) {
					HREFTYPE rt;
					int tf;

					if (FAILED(coinfo->lpVtbl->GetImplTypeFlags(coinfo, i, &tf)))
						continue;

					if ((sink && tf == (IMPLTYPEFLAG_FSOURCE|IMPLTYPEFLAG_FDEFAULT)) ||
						(!sink && (tf & IMPLTYPEFLAG_FSOURCE) == 0)) {

						/* flags match what we are looking for */

						if (SUCCEEDED(coinfo->lpVtbl->GetRefTypeOfImplType(coinfo, i, &rt)))
							if (SUCCEEDED(coinfo->lpVtbl->GetRefTypeInfo(coinfo, rt, &typeinfo)))
								break;
						
					}
				}
				
				coinfo->lpVtbl->ReleaseTypeAttr(coinfo, attr);
				coinfo->lpVtbl->Release(coinfo);
			}
		}

		
		efree(olename);
	} else if (gotguid) {
		typelib->lpVtbl->GetTypeInfoOfGuid(typelib, &iid, &typeinfo);
	}

	if (typelib)
		typelib->lpVtbl->Release(typelib);

	return typeinfo;
}

static ITypeLib *php_COM_find_typelib(char *search_string, int mode)
{
	ITypeLib *TypeLib = NULL;
	char *strtok_buf, *major, *minor;
	CLSID clsid;
	OLECHAR *p;

	/* Type Libraries:
	 * The string we have is either:
	 * a) a file name
	 * b) a CLSID, major, minor e.g. "{00000200-0000-0010-8000-00AA006D2EA4},2,0"
	 * c) a Type Library name e.g. "Microsoft OLE DB ActiveX Data Objects 1.0 Library"
	 * Searching for the name will be more expensive that the
	 * other two, so we will do that when both other attempts
	 * fail.
	 */

	search_string = php_strtok_r(search_string, ",", &strtok_buf);

	if (search_string == NULL)
		return NULL;
	
	major = php_strtok_r(NULL, ",", &strtok_buf);
	minor = php_strtok_r(NULL, ",", &strtok_buf);

	p = php_char_to_OLECHAR(search_string, strlen(search_string), CP_ACP, FALSE);
	/* Is the string a GUID ? */

	if (!FAILED(CLSIDFromString(p, &clsid))) {
		HRESULT hr;
		WORD major_i = 1;
		WORD minor_i = 0;

		/* We have a valid GUID, check to see if a major/minor */
		/* version was specified otherwise assume 1,0 */
		if ((major != NULL) && (minor != NULL)) {
			major_i = (WORD) atoi(major);
			minor_i = (WORD) atoi(minor);
		}

		/* The GUID will either be a typelibrary or a CLSID */
		hr = LoadRegTypeLib((REFGUID) &clsid, major_i, minor_i, LANG_NEUTRAL, &TypeLib);

		/* If the LoadRegTypeLib fails, let's try to instantiate */
		/* the class itself and then QI for the TypeInfo and */
		/* retrieve the type info from that interface */
		if (FAILED(hr) && (!major || !minor)) {
			IDispatch *Dispatch;
			ITypeInfo *TypeInfo;
			int idx;

			if (FAILED(CoCreateInstance(&clsid, NULL, CLSCTX_SERVER, &IID_IDispatch, (LPVOID *) &Dispatch))) {
				efree(p);
				return NULL;
			}
			if (FAILED(Dispatch->lpVtbl->GetTypeInfo(Dispatch, 0, LANG_NEUTRAL, &TypeInfo))) {
				Dispatch->lpVtbl->Release(Dispatch);
				efree(p);
				return NULL;
			}
			Dispatch->lpVtbl->Release(Dispatch);
			if (FAILED(TypeInfo->lpVtbl->GetContainingTypeLib(TypeInfo, &TypeLib, &idx))) {
				TypeInfo->lpVtbl->Release(TypeInfo);
				efree(p);
				return NULL;
			}
			TypeInfo->lpVtbl->Release(TypeInfo);
		}
	} else {
		if (FAILED(LoadTypeLib(p, &TypeLib))) {
			/* Walk HKCR/TypeLib looking for the string */
			/* If that succeeds, call ourself recursively */
			/* using the CLSID found, else give up and bail */
			HKEY hkey, hsubkey;
			DWORD SubKeys, MaxSubKeyLength;
			char *keyname;
			register unsigned int ii, jj;
			DWORD VersionCount;
			char version[20]; /* All the version keys are 1.0, 4.6, ... */
			char *libname;
			DWORD libnamelen;

			/* No Need for Unicode version any more */
			efree(p);

			/* Starting at HKEY_CLASSES_ROOT/TypeLib */
			/* Walk all subkeys (Typelib GUIDs) looking */
			/* at each version for a string match to the */
			/* supplied argument */

			if (ERROR_SUCCESS != RegOpenKey(HKEY_CLASSES_ROOT, "TypeLib",&hkey)) {
				/* This is pretty bad - better bail */
				return NULL;
			}
			if (ERROR_SUCCESS != RegQueryInfoKey(hkey, NULL, NULL, NULL, &SubKeys, &MaxSubKeyLength, NULL, NULL, NULL, NULL, NULL, NULL)) {
				RegCloseKey(hkey);
				return NULL;
			}
			MaxSubKeyLength++; /* \0 is not counted */
			keyname = emalloc(MaxSubKeyLength);
			libname = emalloc(strlen(search_string)+1);
			for (ii=0;ii<SubKeys;ii++) {
				if (ERROR_SUCCESS != RegEnumKey(hkey, ii, keyname, MaxSubKeyLength)) {
					/* Failed - who cares */
					continue;
				}
				if (ERROR_SUCCESS != RegOpenKey(hkey, keyname, &hsubkey)) {
					/* Failed - who cares */
					continue;
				}
				if (ERROR_SUCCESS != RegQueryInfoKey(hsubkey, NULL, NULL, NULL, &VersionCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
					/* Failed - who cares */
					RegCloseKey(hsubkey);
					continue;
				}
				for (jj=0;jj<VersionCount;jj++) {
					if (ERROR_SUCCESS != RegEnumKey(hsubkey, jj, version, sizeof(version))) {
						/* Failed - who cares */
						continue;
					}
					/* OK we just need to retrieve the default */
					/* value for this key and see if it matches */
					libnamelen = strlen(search_string)+1;
					if (ERROR_SUCCESS == RegQueryValue(hsubkey, version, libname, &libnamelen)) {
						if ((mode & CONST_CS) ? (strcmp(libname, search_string) == 0) : (stricmp(libname, search_string) == 0)) {
							char *str;
							int major, minor;

							/* Found it */
							RegCloseKey(hkey);
							RegCloseKey(hsubkey);

							efree(libname);
							/* We can either open up the "win32" key and find the DLL name */
							/* Or just parse the version string and pass that in */
							/* The version string seems like a more portable solution */
							/* Given that there is a COM on Unix */
							if (2 != sscanf(version, "%d.%d", &major, &minor)) {
								major = 1;
								minor = 0;
							}
							str = emalloc(strlen(keyname)+strlen(version)+20); /* 18 == safety, 2 == extra comma and \0 */
							sprintf(str, "%s,%d,%d", keyname, major, minor);
							efree(keyname);
							TypeLib = php_COM_find_typelib(str, mode);
							efree(str);
							/* This is probbaly much harder to read and follow */
							/* But it is MUCH more effiecient than trying to */
							/* test for errors and leave through a single "return" */
							return TypeLib;
						}
					} else {
						/* Failed - perhaps too small abuffer */
						/* But if too small, then the name does not match */
					}
				}
				RegCloseKey(hsubkey);
			}
			efree(keyname);
			efree(libname);
			return NULL;
		}
	}
	efree(p);
	return TypeLib;
}


ZEND_API int php_COM_load_typelib(ITypeLib *TypeLib, int mode)
{
	ITypeComp *TypeComp;
	int i;
	int interfaces;
	TSRMLS_FETCH();

	if (NULL == TypeLib) {
		return FAILURE;
	}

	interfaces = TypeLib->lpVtbl->GetTypeInfoCount(TypeLib);

	TypeLib->lpVtbl->GetTypeComp(TypeLib, &TypeComp);
	for (i=0; i<interfaces; i++) {
		TYPEKIND pTKind;

		TypeLib->lpVtbl->GetTypeInfoType(TypeLib, i, &pTKind);
		if (pTKind==TKIND_ENUM) {
			ITypeInfo *TypeInfo;
			VARDESC *pVarDesc;
			UINT NameCount;
			int j;
			BSTR bstr_EnumId;
			char *EnumId;

			TypeLib->lpVtbl->GetDocumentation(TypeLib, i, &bstr_EnumId, NULL, NULL, NULL);
			EnumId = php_OLECHAR_to_char(bstr_EnumId, NULL, CP_ACP, FALSE);
			printf("Enumeration %d - %s:\n", i, EnumId);
			efree(EnumId);

			TypeLib->lpVtbl->GetTypeInfo(TypeLib, i, &TypeInfo);

			j=0;
			while (SUCCEEDED(TypeInfo->lpVtbl->GetVarDesc(TypeInfo, j, &pVarDesc))) {
				BSTR bstr_ids;
				zend_constant c;
				zval exists, results, value;
				char *const_name;

				TypeInfo->lpVtbl->GetNames(TypeInfo, pVarDesc->memid, &bstr_ids, 1, &NameCount);
				if (NameCount!=1) {
					j++;
					continue;
				}
				const_name = php_OLECHAR_to_char(bstr_ids, &c.name_len, CP_ACP, FALSE);
				c.name = zend_strndup(const_name, c.name_len);
				efree(const_name);
				c.name_len++; /* length should include the NULL */
				SysFreeString(bstr_ids);

				/* Before registering the contsnt, let's see if we can find it */
				if (zend_get_constant(c.name, c.name_len-1, &exists TSRMLS_CC)) {
					/* Oops, it already exists. No problem if it is defined as the same value */
					/* Check to see if they are the same */
					if (!compare_function(&results, &c.value, &exists TSRMLS_CC) && INI_INT("com.autoregister_verbose")) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type library value %s is already defined and has a different value", c.name);
					}
					free(c.name);
					j++;
					continue;
				}

				php_variant_to_zval(pVarDesc->lpvarValue, &value, CP_ACP);
				 /* we only import enumerations (=int) */
				if (Z_TYPE(value) == IS_LONG) {
					c.flags = mode;
					c.value.type = IS_LONG;
					c.value.value.lval = Z_LVAL(value);
					c.module_number = 0; /* the module number is not available here */

					zend_register_constant(&c TSRMLS_CC);
				}

				j++;
			}
			TypeInfo->lpVtbl->Release(TypeInfo);
		}
	}

	return SUCCESS;
}


/* create an overloaded COM object from a dispatch pointer */
PHPAPI zval *php_COM_object_from_dispatch(IDispatch *disp)
{
	comval *obj;
	TSRMLS_FETCH();
	
	ALLOC_COM(obj);
	php_COM_set(obj, &disp, FALSE);
	
	return rpc_object_from_data(NULL, com, obj, NULL);

}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
