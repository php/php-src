/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/* IClassFactory Implementation, and DllXXX function implementation */

#define INITGUID
#define DEBUG_CLASS_FACTORY 0
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <initguid.h>

extern "C" {
HINSTANCE module_handle;
}

#include <comcat.h>
#include "TSRM.h"
#include "php4as_classfactory.h"
#include "php4as_scriptengine.h"

volatile LONG TPHPClassFactory::factory_count = 0;
volatile LONG TPHPClassFactory::object_count = 0;

/* {{{ Class Factory Implementation */
TPHPClassFactory::TPHPClassFactory()
{
	m_refcount = 1;
	InterlockedIncrement(const_cast<long*> (&factory_count));
}

TPHPClassFactory::~TPHPClassFactory()
{
	InterlockedDecrement(const_cast<long*> (&factory_count));
}

void TPHPClassFactory::AddToObjectCount(void)
{
	InterlockedIncrement(const_cast<long*> (&object_count));
}

void TPHPClassFactory::RemoveFromObjectCount(void)
{
	InterlockedDecrement(const_cast<long*> (&object_count));
}

STDMETHODIMP_(DWORD) TPHPClassFactory::AddRef(void)
{
	return InterlockedIncrement(const_cast<long*> (&m_refcount));
}

STDMETHODIMP_(DWORD) TPHPClassFactory::Release(void)
{
	DWORD ret = InterlockedDecrement(const_cast<long*> (&m_refcount));
	if (ret == 0)
		delete this;
	return ret;
}

STDMETHODIMP TPHPClassFactory::QueryInterface(REFIID iid, void **ppvObject)
{
	*ppvObject = NULL;
	
	if (IsEqualGUID(IID_IClassFactory, iid)) {
		*ppvObject = (IClassFactory*)this;
	} else if (IsEqualGUID(IID_IUnknown, iid)) {
		*ppvObject = this;	
	}
	if (*ppvObject) {
		AddRef();
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

STDMETHODIMP TPHPClassFactory::LockServer(BOOL fLock)
{
	return E_NOTIMPL;
}

STDMETHODIMP TPHPClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID iid, void **ppvObject)
{
	TPHPScriptingEngine *engine = new TPHPScriptingEngine;

	HRESULT ret = engine->QueryInterface(iid, ppvObject);

	engine->Release();
	
	return ret;
}

int TPHPClassFactory::CanUnload(void)
{
	return (object_count + factory_count) == 0 ? 1 : 0;
}

/* }}} */

/* {{{ Registration structures */
struct reg_entry {
	HKEY root_key;
	char *sub_key;
	char *value_name;
	char *data;
};

struct reg_class {
	/* REFIID here causes compilation errors */
	const GUID *class_id;
	char *class_name;
	char *threading;
	const struct reg_entry *reg;
	const GUID **cats;
};

#define MAX_REG_SUBST	8
struct reg_subst {
	int count;
	char *names[MAX_REG_SUBST];
	char *values[MAX_REG_SUBST];
};
/* }}} */

/* {{{ Registration information */
/* This registers the sapi as a scripting engine that can be instantiated using it's progid */
static const struct reg_entry engine_entries[] = {
	{ HKEY_CLASSES_ROOT,	"CLSID\\[CLSID]",	 				NULL,				"[CLASSNAME]" },
	{ HKEY_CLASSES_ROOT,	"CLSID\\[CLSID]\\InprocServer32",	NULL, 				"[MODULENAME]" },
	{ HKEY_CLASSES_ROOT,	"CLSID\\[CLSID]\\InprocServer32",	"ThreadingModel",	"[THREADING]" },
	{ HKEY_CLASSES_ROOT,	"CLSID\\[CLSID]\\OLEScript",		NULL,				NULL },
	{ HKEY_CLASSES_ROOT,	"CLSID\\[CLSID]\\ProgID",			NULL,				"ActivePHP" },
	{ HKEY_CLASSES_ROOT,	"ActivePHP",						NULL,				"ActivePHP" },
	{ HKEY_CLASSES_ROOT,	"ActivePHP\\CLSID",					NULL,				"[CLSID]"},
	{ HKEY_CLASSES_ROOT,	"ActivePHP\\OLEScript",				NULL,				NULL},
	
	{ 0, NULL, NULL, NULL }
};

static const GUID *script_engine_categories[] = {
	&CATID_ActiveScript,
	&CATID_ActiveScriptParse,
	NULL
};

static const struct reg_class classes_to_register[] = {
	{ &CLSID_PHPActiveScriptEngine, "PHP Active Script Engine", "Both", engine_entries, script_engine_categories },
	{ NULL, NULL, NULL, 0, NULL }
};
/* }}} */

/* {{{ Registration code */
static void substitute(struct reg_subst *subst, char *srcstring, char *deststring)
{
	int i;
	char *srcpos = srcstring;
	char *destpos = deststring;
	
	while(1) {
		char *repstart = strchr(srcpos, '[');

		if (repstart == NULL) {
			strcpy(destpos, srcpos);
			break;
		}

		/* copy everything up until that character to the dest */
		strncpy(destpos, srcpos, repstart - srcpos);
		destpos += repstart - srcpos;
		*destpos = 0;

		/* search for replacement strings in the table */
		for (i = 0; i < subst->count; i++) {
			int len = strlen(subst->names[i]);
			if (strncmp(subst->names[i], repstart + 1, len) == 0) {
				/* append the replacement value to the buffer */
				strcpy(destpos, subst->values[i]);
				destpos += strlen(subst->values[i]);
				srcpos = repstart + len + 2;

				break;
			}
		}
	}
#if DEBUG_CLASS_FACTORY
	MessageBox(0, deststring, srcstring, MB_ICONHAND);
#endif
}

static int reg_string(BOOL do_reg, struct reg_subst *subst, const struct reg_entry *entry)
{
	char subbuf[MAX_PATH], valuebuf[MAX_PATH], databuf[MAX_PATH];
	char *sub = NULL, *value = NULL, *data = NULL;
	LRESULT res;
	HKEY hkey;
	DWORD disp;
	
	if (entry->sub_key) {
		substitute(subst, entry->sub_key, subbuf);
		sub = subbuf;
	}

	if (entry->value_name) {
		substitute(subst, entry->value_name, valuebuf);
		value = valuebuf;
	}

	if (entry->data) {
		substitute(subst, entry->data, databuf);
		data = databuf;
	}

	if (do_reg) {
		res = RegCreateKeyEx(entry->root_key, sub, 0, NULL, REG_OPTION_NON_VOLATILE,
				KEY_WRITE, NULL, &hkey, &disp);
		if (res == NOERROR) {
			if (data)
				res = RegSetValueEx(hkey, value, 0, REG_SZ, (LPBYTE)data, strlen(data) + 1);
			RegCloseKey(hkey);
		}
	} else {
		res = RegDeleteKey(entry->root_key, sub);
	}
	return res == NOERROR ? 1 : 0;
}

static int perform_registration(BOOL do_reg)
{
	char module_name[MAX_PATH];
	char classid[40];
	int ret = 0;
	int i, j;
	struct reg_subst subst = {0};
	LPOLESTR classidw;
	ICatRegister *catreg = NULL;
	CATID cats[8];
	int ncats;

	CoInitialize(NULL);
	CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (LPVOID*)&catreg);

	GetModuleFileName(module_handle, module_name, sizeof(module_name));
		
	subst.names[0] = "CLSID";
	subst.values[0] = classid;

	subst.names[1] = "MODULENAME";
	subst.values[1] = module_name;

	subst.names[2] = "CLASSNAME";
	subst.names[3] = "THREADING";

	subst.count = 4;

	for (i = 0; classes_to_register[i].class_id; i++) {
		StringFromCLSID(*classes_to_register[i].class_id, &classidw);
		WideCharToMultiByte(CP_ACP, 0, classidw, -1, classid, sizeof(classid), NULL, NULL);
		classid[39] = 0;
		CoTaskMemFree(classidw);

		subst.values[2] = classes_to_register[i].class_name;
		subst.values[3] = classes_to_register[i].threading;

		if (catreg && classes_to_register[i].cats) {
			ncats = 0;
			while(classes_to_register[i].cats[ncats]) {
				CopyMemory(&cats[ncats], classes_to_register[i].cats[ncats], sizeof(CATID));
				ncats++;
			}
		}
		
		if (do_reg) {
			for (j = 0; classes_to_register[i].reg[j].root_key; j++) {
				if (!reg_string(do_reg, &subst, &classes_to_register[i].reg[j])) {
					ret = 0;
					break;
				}
			}
			
			if (catreg && ncats) {
				catreg->RegisterClassImplCategories(*classes_to_register[i].class_id, ncats, cats);
			}
			
		} else {

			if (catreg && ncats) {
				catreg->UnRegisterClassImplCategories(*classes_to_register[i].class_id, ncats, cats);
			}

			/* do it in reverse order, so count the number of entries first */
			for (j = 0; classes_to_register[i].reg[j].root_key; j++)
				;
			while(j-- > 0) {
				if (!reg_string(do_reg, &subst, &classes_to_register[i].reg[j])) {
					ret = 0;
					break;
				}
			}
		}
	}

	if (catreg)
		catreg->Release();
	CoUninitialize();
	
	return ret;
}
/* }}} */

/* {{{ Standard COM server DllXXX entry points */

STDAPI DllCanUnloadNow(void)
{
	if (TPHPClassFactory::CanUnload())
		return S_OK;
	return S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	HRESULT ret = E_FAIL;

	if (IsEqualCLSID(CLSID_PHPActiveScriptEngine, rclsid)) {
		TPHPClassFactory *factory = new TPHPClassFactory();

		if (factory) {
			ret = factory->QueryInterface(riid, ppv);
			factory->Release();
		}
	}
	return ret;
}



STDAPI DllRegisterServer(void)
{
	return perform_registration(TRUE) ? S_OK : S_FALSE;
}

STDAPI DllUnregisterServer(void)
{
	return perform_registration(FALSE) ? S_OK : S_FALSE;
}

/* }}} */
