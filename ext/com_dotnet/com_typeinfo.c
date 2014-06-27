/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
   |         Harald Radi <h.radi@nme.at>                                  |
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


/* The search string can be either:
 * a) a file name
 * b) a CLSID, major, minor e.g. "{00000200-0000-0010-8000-00AA006D2EA4},2,0"
 * c) a Type Library name e.g. "Microsoft OLE DB ActiveX Data Objects 1.0 Library"
 */
PHP_COM_DOTNET_API ITypeLib *php_com_load_typelib(char *search_string, int codepage TSRMLS_DC)
{
	ITypeLib *TL = NULL;
	char *strtok_buf, *major, *minor;
	CLSID clsid;
	OLECHAR *p;
	HRESULT hr;

	search_string = php_strtok_r(search_string, ",", &strtok_buf);

	if (search_string == NULL) {
		return NULL;
	}

	major = php_strtok_r(NULL, ",", &strtok_buf);
	minor = php_strtok_r(NULL, ",", &strtok_buf);

	p = php_com_string_to_olestring(search_string, strlen(search_string), codepage TSRMLS_CC);

	if (SUCCEEDED(CLSIDFromString(p, &clsid))) {
		WORD major_i = 1, minor_i = 0;

		/* pick up the major/minor numbers; if none specified, default to 1,0 */
		if (major && minor) {
			major_i = (WORD)atoi(major);
			minor_i = (WORD)atoi(minor);
		}

		/* Load the TypeLib by GUID */
		hr = LoadRegTypeLib((REFGUID)&clsid, major_i, minor_i, LANG_NEUTRAL, &TL);

		/* if that failed, assumed that the GUID is actually a CLSID and
		 * attemp to get the library via an instance of that class */
		if (FAILED(hr) && (major == NULL || minor == NULL)) {
			IDispatch *disp = NULL;
			ITypeInfo *info = NULL;
			int idx;

			if (SUCCEEDED(hr = CoCreateInstance(&clsid, NULL, CLSCTX_SERVER, &IID_IDispatch, (LPVOID*)&disp)) &&
					SUCCEEDED(hr = IDispatch_GetTypeInfo(disp, 0, LANG_NEUTRAL, &info))) {
				hr = ITypeInfo_GetContainingTypeLib(info, &TL, &idx);
			}

			if (info) {
				ITypeInfo_Release(info);
			}
			if (disp) {
				IDispatch_Release(disp);
			}
		}
	} else {
		/* Try to load it from a file; if it fails, do a really painful search of
		 * the registry */
		if (FAILED(LoadTypeLib(p, &TL))) {
			HKEY hkey, hsubkey;
			DWORD SubKeys, MaxSubKeyLength;
			char *keyname;
			unsigned int i, j;
			DWORD VersionCount;
			char version[20];
			char *libname;
			DWORD libnamelen;

			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, "TypeLib", 0, KEY_READ, &hkey) &&
					ERROR_SUCCESS == RegQueryInfoKey(hkey, NULL, NULL, NULL, &SubKeys,
					&MaxSubKeyLength, NULL, NULL, NULL, NULL, NULL, NULL)) {

				MaxSubKeyLength++; /* make room for NUL */
				keyname = emalloc(MaxSubKeyLength);
				libname = emalloc(strlen(search_string) + 1);

				for (i = 0; i < SubKeys && TL == NULL; i++) {
					if (ERROR_SUCCESS == RegEnumKey(hkey, i, keyname, MaxSubKeyLength) &&
							ERROR_SUCCESS == RegOpenKeyEx(hkey, keyname, 0, KEY_READ, &hsubkey)) {
						if (ERROR_SUCCESS == RegQueryInfoKey(hsubkey, NULL, NULL, NULL, &VersionCount,
								NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
							for (j = 0; j < VersionCount; j++) {
								if (ERROR_SUCCESS != RegEnumKey(hsubkey, j, version, sizeof(version))) {
									continue;
								}
								/* get the default value for this key and compare */
								libnamelen = strlen(search_string)+1;
								if (ERROR_SUCCESS == RegQueryValue(hsubkey, version, libname, &libnamelen)) {
									if (0 == stricmp(libname, search_string)) {
										char *str = NULL;
										int major_tmp, minor_tmp;

										/* fetch the GUID and add the version numbers */
										if (2 != sscanf(version, "%d.%d", &major_tmp, &minor_tmp)) {
											major_tmp = 1;
											minor_tmp = 0;
										}
										spprintf(&str, 0, "%s,%d,%d", keyname, major_tmp, minor_tmp);
										/* recurse */
										TL = php_com_load_typelib(str, codepage TSRMLS_CC);

										efree(str);
										break;
									}
								}
							}
						}
						RegCloseKey(hsubkey);
					}
				}
				RegCloseKey(hkey);
				efree(keyname);
				efree(libname);
			}
		}
	}
	
	efree(p);

	return TL;
}

/* Given a type-library, merge it into the current engine state */
PHP_COM_DOTNET_API int php_com_import_typelib(ITypeLib *TL, int mode, int codepage TSRMLS_DC)
{
	int i, j, interfaces;
	TYPEKIND pTKind;
	ITypeInfo *TypeInfo;
	VARDESC *pVarDesc;
	UINT NameCount;
	BSTR bstr_ids;
	zend_constant c;
	zval exists, results, value;
	char *const_name;

	if (TL == NULL) {
		return FAILURE;
	}

	interfaces = ITypeLib_GetTypeInfoCount(TL);
	for (i = 0; i < interfaces; i++) {
		ITypeLib_GetTypeInfoType(TL, i, &pTKind);
		if (pTKind == TKIND_ENUM) {
			ITypeLib_GetTypeInfo(TL, i, &TypeInfo);
			for (j = 0; ; j++) {
				if (FAILED(ITypeInfo_GetVarDesc(TypeInfo, j, &pVarDesc))) {
					break;
				}
				ITypeInfo_GetNames(TypeInfo, pVarDesc->memid, &bstr_ids, 1, &NameCount);
				if (NameCount != 1) {
					ITypeInfo_ReleaseVarDesc(TypeInfo, pVarDesc);
					continue;
				}

				const_name = php_com_olestring_to_string(bstr_ids, &c.name_len, codepage TSRMLS_CC);
				c.name = zend_strndup(const_name, c.name_len);
				efree(const_name);
				if(c.name == NULL) {
					ITypeInfo_ReleaseVarDesc(TypeInfo, pVarDesc);
					continue;
				}
				c.name_len++; /* include NUL */
				SysFreeString(bstr_ids);

				/* sanity check for the case where the constant is already defined */
				if (zend_get_constant(c.name, c.name_len - 1, &exists TSRMLS_CC)) {
					if (COMG(autoreg_verbose) && !compare_function(&results, &c.value, &exists TSRMLS_CC)) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type library constant %s is already defined", c.name);
					}
					free(c.name);
					ITypeInfo_ReleaseVarDesc(TypeInfo, pVarDesc);
					continue;
				}

				/* register the constant */
				php_com_zval_from_variant(&value, pVarDesc->lpvarValue, codepage TSRMLS_CC);
				if (Z_TYPE(value) == IS_LONG) {
					c.flags = mode;
					c.value.type = IS_LONG;
					c.value.value.lval = Z_LVAL(value);
					c.module_number = 0;
					zend_register_constant(&c TSRMLS_CC);
				}
				ITypeInfo_ReleaseVarDesc(TypeInfo, pVarDesc);
			}
			ITypeInfo_Release(TypeInfo);
		}
	}
	return SUCCESS;
}

/* Type-library stuff */
void php_com_typelibrary_dtor(void *pDest)
{
	ITypeLib **Lib = (ITypeLib**)pDest;
	ITypeLib_Release(*Lib);
}

PHP_COM_DOTNET_API ITypeLib *php_com_load_typelib_via_cache(char *search_string,
	int codepage, int *cached TSRMLS_DC)
{
	ITypeLib **TLp;
	ITypeLib *TL;
	char *name_dup;
	int l;

	l = strlen(search_string);

	if (zend_ts_hash_find(&php_com_typelibraries, search_string, l+1,
			(void**)&TLp) == SUCCESS) {
		*cached = 1;
		/* add a reference for the caller */
		ITypeLib_AddRef(*TLp);
		return *TLp;
	}

	*cached = 0;
	name_dup = estrndup(search_string, l);
	TL = php_com_load_typelib(name_dup, codepage TSRMLS_CC);
	efree(name_dup);

	if (TL) {
		if (SUCCESS == zend_ts_hash_update(&php_com_typelibraries,
				search_string, l+1, (void*)&TL, sizeof(ITypeLib*), NULL)) {
			/* add a reference for the hash table */
			ITypeLib_AddRef(TL);
		}
	}

	return TL;
}

ITypeInfo *php_com_locate_typeinfo(char *typelibname, php_com_dotnet_object *obj, char *dispname, int sink TSRMLS_DC)
{
	ITypeInfo *typeinfo = NULL;
	ITypeLib *typelib = NULL;
	int gotguid = 0;
	GUID iid;
	
	if (obj) {
		if (dispname == NULL && sink) {
			IProvideClassInfo2 *pci2;
			IProvideClassInfo *pci;

			if (SUCCEEDED(IDispatch_QueryInterface(V_DISPATCH(&obj->v), &IID_IProvideClassInfo2, (void**)&pci2))) {
				gotguid = SUCCEEDED(IProvideClassInfo2_GetGUID(pci2, GUIDKIND_DEFAULT_SOURCE_DISP_IID, &iid));
				IProvideClassInfo2_Release(pci2);
			}
			if (!gotguid && SUCCEEDED(IDispatch_QueryInterface(V_DISPATCH(&obj->v), &IID_IProvideClassInfo, (void**)&pci))) {
				/* examine the available interfaces */
				/* TODO: write some code here */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "IProvideClassInfo: this code not yet written!");
				IProvideClassInfo_Release(pci);
			}
		} else if (dispname == NULL) {
			if (obj->typeinfo) {
				ITypeInfo_AddRef(obj->typeinfo);
				return obj->typeinfo;
			} else {
				IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &typeinfo);
				if (typeinfo) {
					return typeinfo;
				}
			}
		} else if (dispname && obj->typeinfo) {
			unsigned int idx;
			/* get the library from the object; the rest will be dealt with later */
			ITypeInfo_GetContainingTypeLib(obj->typeinfo, &typelib, &idx);
		} else if (typelibname == NULL) {
			IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &typeinfo);
			if (dispname) {
				unsigned int idx;
				/* get the library from the object; the rest will be dealt with later */
				ITypeInfo_GetContainingTypeLib(typeinfo, &typelib, &idx);

				if (typelib) {
					ITypeInfo_Release(typeinfo);
					typeinfo = NULL;
				}
			}
		}
	} else if (typelibname) {
		/* Fetch the typelibrary and use that to look things up */
		typelib = php_com_load_typelib(typelibname, CP_THREAD_ACP TSRMLS_CC);
	} 

	if (!gotguid && dispname && typelib) {
		unsigned short cfound;
		MEMBERID memid;
		OLECHAR *olename = php_com_string_to_olestring(dispname, strlen(dispname), CP_ACP TSRMLS_CC);
			
		cfound = 1;
		if (FAILED(ITypeLib_FindName(typelib, olename, 0, &typeinfo, &memid, &cfound)) || cfound == 0) {
			CLSID coclass;
			ITypeInfo *coinfo;
	
			/* assume that it might be a progid instead */
			if (SUCCEEDED(CLSIDFromProgID(olename, &coclass)) &&
					SUCCEEDED(ITypeLib_GetTypeInfoOfGuid(typelib, &coclass, &coinfo))) {

				/* enumerate implemented interfaces and pick the one as indicated by sink */
				TYPEATTR *attr;
				int i;

				ITypeInfo_GetTypeAttr(coinfo, &attr);

				for (i = 0; i < attr->cImplTypes; i++) {
					HREFTYPE rt;
					int tf;

					if (FAILED(ITypeInfo_GetImplTypeFlags(coinfo, i, &tf))) {
						continue;
					}

					if ((sink && tf == (IMPLTYPEFLAG_FSOURCE|IMPLTYPEFLAG_FDEFAULT)) ||
						(!sink && (tf & IMPLTYPEFLAG_FSOURCE) == 0)) {

						/* flags match what we are looking for */

						if (SUCCEEDED(ITypeInfo_GetRefTypeOfImplType(coinfo, i, &rt)))
							if (SUCCEEDED(ITypeInfo_GetRefTypeInfo(coinfo, rt, &typeinfo)))
								break;
						
					}
				}
				
				ITypeInfo_ReleaseTypeAttr(coinfo, attr);
				ITypeInfo_Release(coinfo);
			}
		}

		
		efree(olename);
	} else if (gotguid) {
		ITypeLib_GetTypeInfoOfGuid(typelib, &iid, &typeinfo);
	}

	if (typelib) {
		ITypeLib_Release(typelib);
	}

	return typeinfo;
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
	{ VT_SAFEARRAY, "VT_SAFEARRAY" },
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

static char *php_com_string_from_clsid(const CLSID *clsid, int codepage TSRMLS_DC)
{
	LPOLESTR ole_clsid;
	char *clsid_str;

	StringFromCLSID(clsid, &ole_clsid);
	clsid_str = php_com_olestring_to_string(ole_clsid, NULL, codepage TSRMLS_CC);
	LocalFree(ole_clsid);

	return clsid_str;
}


int php_com_process_typeinfo(ITypeInfo *typeinfo, HashTable *id_to_name, int printdef, GUID *guid, int codepage TSRMLS_DC)
{
	TYPEATTR *attr;
	FUNCDESC *func;
	int i;
	OLECHAR *olename;
	char *ansiname = NULL;
	unsigned int ansinamelen;
	int ret = 0;

	if (FAILED(ITypeInfo_GetTypeAttr(typeinfo, &attr))) {
		return 0;
	}

	/* verify that it is suitable */
	if (id_to_name == NULL || attr->typekind == TKIND_DISPATCH) {

		if (guid) {
			memcpy(guid, &attr->guid, sizeof(GUID));
		}
		
		if (printdef) {
			char *guidstring;

			ITypeInfo_GetDocumentation(typeinfo, MEMBERID_NIL, &olename, NULL, NULL, NULL);
			ansiname = php_com_olestring_to_string(olename, &ansinamelen, codepage TSRMLS_CC);
			SysFreeString(olename);

			guidstring = php_com_string_from_clsid(&attr->guid, codepage TSRMLS_CC);
			php_printf("class %s { /* GUID=%s */\n", ansiname, guidstring);
			efree(guidstring);

			efree(ansiname);
		}

		if (id_to_name) {
			zend_hash_init(id_to_name, 0, NULL, ZVAL_PTR_DTOR, 0);
		}

		/* So we've got the dispatch interface; lets list the event methods */
		for (i = 0; i < attr->cFuncs; i++) {
			zval *tmp;
			DISPID lastid = 0;	/* for props */
			int isprop;

			if (FAILED(ITypeInfo_GetFuncDesc(typeinfo, i, &func)))
				break;

			isprop = (func->invkind & DISPATCH_PROPERTYGET || func->invkind & DISPATCH_PROPERTYPUT);

			if (!isprop || lastid != func->memid) {

				lastid = func->memid;
				
				ITypeInfo_GetDocumentation(typeinfo, func->memid, &olename, NULL, NULL, NULL);
				ansiname = php_com_olestring_to_string(olename, &ansinamelen, codepage TSRMLS_CC);
				SysFreeString(olename);

				if (printdef) {
					int j;
					char *funcdesc;
					unsigned int funcdesclen, cnames = 0;
					BSTR *names;

					names = (BSTR*)safe_emalloc((func->cParams + 1), sizeof(BSTR), 0);

					ITypeInfo_GetNames(typeinfo, func->memid, names, func->cParams + 1, &cnames);
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

						ITypeInfo_GetDocumentation(typeinfo, func->memid, NULL, &olename, NULL, NULL);
						if (olename) {
							funcdesc = php_com_olestring_to_string(olename, &funcdesclen, codepage TSRMLS_CC);
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
								funcdesc = php_com_olestring_to_string(names[j+1], &funcdesclen, codepage TSRMLS_CC);
								SysFreeString(names[j+1]);
							} else {
								funcdesc = "???";
							}

							php_printf(" */ %s%s%c\n",
									elem->tdesc.vt == VT_PTR ? "&$" : "$",
									funcdesc,
									j == func->cParams - 1 ? ' ' : ','
									);

							if (j+1 < (int)cnames) {
								efree(funcdesc);
							}
						}

						php_printf("\t\t)\n\t{\n");

						ITypeInfo_GetDocumentation(typeinfo, func->memid, NULL, &olename, NULL, NULL);
						if (olename) {
							funcdesc = php_com_olestring_to_string(olename, &funcdesclen, codepage TSRMLS_CC);
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
			ITypeInfo_ReleaseFuncDesc(typeinfo, func);
		}

		if (printdef) {
			php_printf("}\n");
		}

		ret = 1;
	} else {
		zend_error(E_WARNING, "That's not a dispatchable interface!! type kind = %08x", attr->typekind);
	}

	ITypeInfo_ReleaseTypeAttr(typeinfo, attr);

	return ret;
}


