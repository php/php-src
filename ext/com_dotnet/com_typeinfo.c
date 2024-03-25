/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   |         Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"

static HashTable php_com_typelibraries;

#ifdef ZTS
static MUTEX_T php_com_typelibraries_mutex;
#endif

PHP_MINIT_FUNCTION(com_typeinfo)
{
	zend_hash_init(&php_com_typelibraries, 0, NULL, php_com_typelibrary_dtor, 1);

#ifdef ZTS
	php_com_typelibraries_mutex = tsrm_mutex_alloc();
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(com_typeinfo)
{
	zend_hash_destroy(&php_com_typelibraries);

#ifdef ZTS
	tsrm_mutex_free(php_com_typelibraries_mutex);
#endif

	return SUCCESS;
}

/* The search string can be either:
 * a) a file name
 * b) a CLSID, major, minor e.g. "{00000200-0000-0010-8000-00AA006D2EA4},2,0"
 * c) a Type Library name e.g. "Microsoft OLE DB ActiveX Data Objects 1.0 Library"
 */
PHP_COM_DOTNET_API ITypeLib *php_com_load_typelib(char *search_string, int codepage)
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

	p = php_com_string_to_olestring(search_string, strlen(search_string), codepage);

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
		 * attempt to get the library via an instance of that class */
		if (FAILED(hr) && (major == NULL || minor == NULL)) {
			IDispatch *disp = NULL;
			ITypeInfo *info = NULL;
			UINT idx;

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
			long libnamelen;

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
								libnamelen = (long)strlen(search_string)+1;
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
										TL = php_com_load_typelib(str, codepage);

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
PHP_COM_DOTNET_API zend_result php_com_import_typelib(ITypeLib *TL, int mode, int codepage)
{
	int i, j, interfaces;
	TYPEKIND pTKind;
	ITypeInfo *TypeInfo;
	VARDESC *pVarDesc;
	UINT NameCount;
	BSTR bstr_ids;
	zend_constant c;
	zval *exists, results, value;

	if (TL == NULL) {
		return FAILURE;
	}

	interfaces = ITypeLib_GetTypeInfoCount(TL);
	for (i = 0; i < interfaces; i++) {
		ITypeLib_GetTypeInfoType(TL, i, &pTKind);
		if (pTKind == TKIND_ENUM) {
			ITypeLib_GetTypeInfo(TL, i, &TypeInfo);
			for (j = 0; ; j++) {
				zend_string *const_name;

				if (FAILED(ITypeInfo_GetVarDesc(TypeInfo, j, &pVarDesc))) {
					break;
				}
				ITypeInfo_GetNames(TypeInfo, pVarDesc->memid, &bstr_ids, 1, &NameCount);
				if (NameCount != 1) {
					ITypeInfo_ReleaseVarDesc(TypeInfo, pVarDesc);
					continue;
				}

				const_name = php_com_olestring_to_string(bstr_ids, codepage);
				SysFreeString(bstr_ids);

				/* sanity check for the case where the constant is already defined */
				php_com_zval_from_variant(&value, pVarDesc->lpvarValue, codepage);
				if ((exists = zend_get_constant(const_name)) != NULL) {
					if (COMG(autoreg_verbose) && !compare_function(&results, &value, exists)) {
						php_error_docref(NULL, E_WARNING, "Type library constant %s is already defined", ZSTR_VAL(const_name));
					}
					zend_string_release_ex(const_name, /* persistent */ false);
					ITypeInfo_ReleaseVarDesc(TypeInfo, pVarDesc);
					continue;
				}

				/* register the constant */
				if (Z_TYPE(value) == IS_LONG) {
					ZEND_CONSTANT_SET_FLAGS(&c, mode, 0);
					ZVAL_LONG(&c.value, Z_LVAL(value));
					if (mode & CONST_PERSISTENT) {
						/* duplicate string in a persistent manner */
						c.name = zend_string_dup(const_name, /* persistent */ true);
						zend_string_release_ex(const_name, /* persistent */ false);
					} else {
						c.name = const_name;
					}
					zend_register_constant(&c);
				}
				ITypeInfo_ReleaseVarDesc(TypeInfo, pVarDesc);
			}
			ITypeInfo_Release(TypeInfo);
		}
	}
	return SUCCESS;
}

/* Type-library stuff */
void php_com_typelibrary_dtor(zval *pDest)
{
	ITypeLib *Lib = (ITypeLib*)Z_PTR_P(pDest);
	ITypeLib_Release(Lib);
}

ITypeLib *php_com_cache_typelib(ITypeLib* TL, char *cache_key, zend_long cache_key_len) {
	ITypeLib* result;
#ifdef ZTS
	tsrm_mutex_lock(php_com_typelibraries_mutex);
#endif

	result = zend_hash_str_add_ptr(&php_com_typelibraries, cache_key, cache_key_len, TL);

#ifdef ZTS
	tsrm_mutex_unlock(php_com_typelibraries_mutex);
#endif

	return result;
}

PHP_COM_DOTNET_API ITypeLib *php_com_load_typelib_via_cache(const char *search_string, int codepage)
{
	ITypeLib *TL;
	char *name_dup;
	zend_string *key = zend_string_init(search_string, strlen(search_string), 1);

#ifdef ZTS
	tsrm_mutex_lock(php_com_typelibraries_mutex);
#endif

	if ((TL = zend_hash_find_ptr(&php_com_typelibraries, key)) != NULL) {
		/* add a reference for the caller */
		ITypeLib_AddRef(TL);

		goto php_com_load_typelib_via_cache_return;
	}

	name_dup = estrndup(ZSTR_VAL(key), ZSTR_LEN(key));
	TL = php_com_load_typelib(name_dup, codepage);
	efree(name_dup);

	if (TL) {
		if (NULL != zend_hash_add_ptr(&php_com_typelibraries, key, TL)) {
			/* add a reference for the hash table */
			ITypeLib_AddRef(TL);
		}
	}

php_com_load_typelib_via_cache_return:
#ifdef ZTS
	tsrm_mutex_unlock(php_com_typelibraries_mutex);
#endif
	zend_string_release(key);

	return TL;
}

ITypeInfo *php_com_locate_typeinfo(zend_string *type_lib_name, php_com_dotnet_object *obj,
	zend_string *dispatch_name, bool sink)
{
	ITypeInfo *typeinfo = NULL;
	ITypeLib *typelib = NULL;
	int gotguid = 0;
	GUID iid;

	if (obj) {
		if (dispatch_name == NULL && sink) {
			if (V_VT(&obj->v) == VT_DISPATCH) {
				IProvideClassInfo2 *pci2;
				IProvideClassInfo *pci;

				if (SUCCEEDED(IDispatch_QueryInterface(V_DISPATCH(&obj->v), &IID_IProvideClassInfo2, (void**)&pci2))) {
					gotguid = SUCCEEDED(IProvideClassInfo2_GetGUID(pci2, GUIDKIND_DEFAULT_SOURCE_DISP_IID, &iid));
					IProvideClassInfo2_Release(pci2);
				}
				if (!gotguid && SUCCEEDED(IDispatch_QueryInterface(V_DISPATCH(&obj->v), &IID_IProvideClassInfo, (void**)&pci))) {
					/* examine the available interfaces */
					/* TODO: write some code here */
					php_error_docref(NULL, E_WARNING, "IProvideClassInfo: this code not yet written!");
					IProvideClassInfo_Release(pci);
				}
			}
		} else if (dispatch_name == NULL) {
			if (obj->typeinfo) {
				ITypeInfo_AddRef(obj->typeinfo);
				return obj->typeinfo;
			} else {
				IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &typeinfo);
				if (typeinfo) {
					return typeinfo;
				}
			}
		} else if (dispatch_name && obj->typeinfo) {
			unsigned int idx;
			/* get the library from the object; the rest will be dealt with later */
			ITypeInfo_GetContainingTypeLib(obj->typeinfo, &typelib, &idx);
		} else if (type_lib_name == NULL) {
			if (V_VT(&obj->v) == VT_DISPATCH) {
				IDispatch_GetTypeInfo(V_DISPATCH(&obj->v), 0, LANG_NEUTRAL, &typeinfo);
				if (dispatch_name) {
					unsigned int idx;
					/* get the library from the object; the rest will be dealt with later */
					ITypeInfo_GetContainingTypeLib(typeinfo, &typelib, &idx);

					if (typelib) {
						ITypeInfo_Release(typeinfo);
						typeinfo = NULL;
					}
				}
			}
		}
	} else if (type_lib_name) {
		/* Fetch the typelibrary and use that to look things up */
		typelib = php_com_load_typelib(ZSTR_VAL(type_lib_name), CP_THREAD_ACP);
	}

	if (!gotguid && dispatch_name && typelib) {
		unsigned short cfound;
		MEMBERID memid;
		OLECHAR *olename = php_com_string_to_olestring(ZSTR_VAL(dispatch_name), ZSTR_LEN(dispatch_name), CP_ACP);

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

static zend_string *php_com_string_from_clsid(const CLSID *clsid, int codepage)
{
	LPOLESTR ole_clsid;
	zend_string *clsid_str;

	StringFromCLSID(clsid, &ole_clsid);
	clsid_str = php_com_olestring_to_string(ole_clsid, codepage);
	LocalFree(ole_clsid);

	return clsid_str;
}


bool php_com_process_typeinfo(ITypeInfo *typeinfo, HashTable *id_to_name, bool printdef, GUID *guid, int codepage)
{
	TYPEATTR *attr;
	FUNCDESC *func;
	int i;
	OLECHAR *olename;
	zend_string *ansi_name = NULL;
	DISPID lastid = 0;	/* for props */

	if (FAILED(ITypeInfo_GetTypeAttr(typeinfo, &attr))) {
		return false;
	}

	/* verify that it is suitable */
	if (id_to_name == NULL || attr->typekind == TKIND_DISPATCH) {

		if (guid) {
			memcpy(guid, &attr->guid, sizeof(GUID));
		}

		if (printdef) {
			zend_string *guid_str;

			ITypeInfo_GetDocumentation(typeinfo, MEMBERID_NIL, &olename, NULL, NULL, NULL);
			ansi_name = php_com_olestring_to_string(olename, codepage);
			SysFreeString(olename);

			guid_str = php_com_string_from_clsid(&attr->guid, codepage);
			php_printf("class %s { /* GUID=%s */\n", ZSTR_VAL(ansi_name), ZSTR_VAL(guid_str));
			zend_string_release_ex(guid_str, /* persistent */ false);
			zend_string_release_ex(ansi_name, /* persistent */ false);
		}

		if (id_to_name) {
			zend_hash_init(id_to_name, 0, NULL, ZVAL_PTR_DTOR, 0);
		}

		/* So we've got the dispatch interface; lets list the event methods */
		for (i = 0; i < attr->cFuncs; i++) {
			zval tmp;
			int isprop;

			if (FAILED(ITypeInfo_GetFuncDesc(typeinfo, i, &func)))
				break;

			isprop = (func->invkind & DISPATCH_PROPERTYGET || func->invkind & DISPATCH_PROPERTYPUT);

			if (!isprop || lastid != func->memid) {

				lastid = func->memid;

				ITypeInfo_GetDocumentation(typeinfo, func->memid, &olename, NULL, NULL, NULL);
				ansi_name = php_com_olestring_to_string(olename, codepage);
				SysFreeString(olename);

				if (printdef) {
					int j;
					zend_string *func_desc;
					unsigned int cnames = 0;
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
							func_desc = php_com_olestring_to_string(olename, codepage);
							SysFreeString(olename);
							php_printf("\t/* %s */\n", ZSTR_VAL(func_desc));
							zend_string_release_ex(func_desc, /* persistent */ false);
						}

						php_printf("\tvar $%s;\n\n", ZSTR_VAL(ansi_name));

					} else {
						/* a function */

						php_printf("\tfunction %s(\n", ZSTR_VAL(ansi_name));

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
								func_desc = php_com_olestring_to_string(names[j+1], codepage);
								SysFreeString(names[j+1]);
								php_printf(" */ %s%s%c\n",
									elem->tdesc.vt == VT_PTR ? "&$" : "$",
									ZSTR_VAL(func_desc),
									j == func->cParams - 1 ? ' ' : ','
								);
							} else {
								php_printf(" */ %s???%c\n",
									elem->tdesc.vt == VT_PTR ? "&$" : "$",
									j == func->cParams - 1 ? ' ' : ','
								);
							}

							if (j+1 < (int)cnames) {
								zend_string_release_ex(func_desc, /* persistent */ false);
							}
						}

						php_printf("\t\t)\n\t{\n");

						ITypeInfo_GetDocumentation(typeinfo, func->memid, NULL, &olename, NULL, NULL);
						if (olename) {
							func_desc = php_com_olestring_to_string(olename, codepage);
							SysFreeString(olename);
							php_printf("\t\t/* %s */\n", ZSTR_VAL(func_desc));
							zend_string_release_ex(func_desc, /* persistent */ false);
						}

						php_printf("\t}\n");
					}

					efree(names);
				}

				if (id_to_name) {
					zend_string *lc_ansi_name = zend_string_tolower(ansi_name);
					ZVAL_STR(&tmp, lc_ansi_name);
					zend_hash_index_update(id_to_name, func->memid, &tmp);
				}
				zend_string_release_ex(ansi_name, /* persistent */ false);
			}
			ITypeInfo_ReleaseFuncDesc(typeinfo, func);
		}

		if (printdef) {
			php_printf("}\n");
		}
	} else {
		zend_throw_error(NULL, "Type kind must be dispatchable, %08x given", attr->typekind);
	}

	ITypeInfo_ReleaseTypeAttr(typeinfo, attr);

	return true;
}
