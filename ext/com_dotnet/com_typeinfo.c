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
PHPAPI ITypeLib *php_com_load_typelib(char *search_string, int mode,
		int codepage TSRMLS_DC)
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
									if (0 == ((mode & CONST_CS) ? strcmp(libname, search_string) : stricmp(libname, search_string))) {
										char *str = NULL;
										int major, minor;

										/* fetch the GUID and add the version numbers */
										if (2 != sscanf(version, "%d.%d", &major, &minor)) {
											major = 1;
											minor = 0;
										}
										spprintf(&str, 0, "%s,%d,%d", keyname, major, minor);
										/* recurse */
										TL = php_com_load_typelib(str, mode, codepage TSRMLS_CC);

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
PHPAPI int php_com_import_typelib(ITypeLib *TL, int mode, int codepage TSRMLS_DC)
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
	ITypeLib *Lib = *(ITypeLib**)pDest;
	ITypeLib_Release(Lib);
}

PHPAPI ITypeLib *php_com_load_typelib_via_cache(char *search_string,
	int mode, int codepage, int *cached TSRMLS_DC)
{
	ITypeLib **TL;
	char *name_dup;
	int l;

	l = strlen(search_string);

	if (zend_ts_hash_find(&php_com_typelibraries, search_string, l+1,
			(void**)&TL) == SUCCESS) {
		*cached = 1;
		/* add a reference for the caller */
		ITypeLib_AddRef(*TL);
		return *TL;
	}

	*cached = 0;
	name_dup = estrndup(search_string, l);
	*TL = php_com_load_typelib(name_dup, mode, codepage TSRMLS_CC);
	efree(name_dup);

	if (*TL) {
		if (SUCCESS == zend_ts_hash_update(&php_com_typelibraries,
				search_string, l+1, (void*)*TL, sizeof(ITypeLib*), NULL)) {
			/* add a reference for the hash table */
			ITypeLib_AddRef(*TL);
		}
	}

	return *TL;
}
