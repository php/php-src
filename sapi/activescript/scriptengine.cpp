/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/* Implementation Notes:
 *
 * PHP stores scripting engine state in thread-local storage. That means
 * that we need to create a dedicated thread per-engine so that a host can
 * use more than one engine object per thread.
 *
 * There are some interesting synchronization issues: Anything to do with
 * running script in the PHP/Zend engine must take place on the engine
 * thread.  Likewise, calling back to the host must take place on the base
 * thread - the thread that set the script site.
 * */

#define _WIN32_DCOM
#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include "php.h"
extern "C" {
#include "php_main.h"
#include "SAPI.h"
#include "zend.h"
#include "zend_execute.h"
#include "zend_compile.h"
#include "php_globals.h"
#include "php_variables.h"
#include "php_ini.h"
#include "php5activescript.h"
#include "ext/com_dotnet/php_com_dotnet.h"
#include "ext/com_dotnet/php_com_dotnet_internal.h"
#include "zend_exceptions.h"
}
#include "php_ticks.h"
#include "php5as_scriptengine.h"
#include "php5as_classfactory.h"
#include <objbase.h>
#undef php_win_err

static int clone_frags(void *pDest, void *arg TSRMLS_DC);

#define ENGINE_THREAD_ONLY(type, method)	\
	if (tsrm_thread_id() != m_enginethread) { \
		trace("WRONG THREAD !! " #type "::" #method "\n"); \
		return RPC_E_WRONG_THREAD; \
	} \
		trace("[direct] " #type "::" #method "\n"); 


#define ASS_CALL(ret, method, args)	\
	if (tsrm_thread_id() == m_basethread) { \
		trace("Calling [direct] m_pass->" #method "\n"); \
		ret = m_pass->method args; \
	} else { \
		IActiveScriptSite *ass; \
		trace("Calling [marshall] m_pass->" #method "\n"); \
		ret = GIT_get(m_asscookie, IID_IActiveScriptSite, (void**)&ass); \
		if (SUCCEEDED(ret)) { \
			ret = ass->method args; \
			ass->Release(); \
		} \
	} \
	trace("--- done calling m_pass->" #method "\n");

/* {{{ trace */
static inline void trace(char *fmt, ...)
{
	va_list ap;
	char buf[4096];

	sprintf(buf, "T=%08x ", tsrm_thread_id());
	OutputDebugString(buf);
	
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);

	OutputDebugString(buf);
	
	va_end(ap);
}
/* }}} */
/* {{{ scriptstate_to_string */
static const char *scriptstate_to_string(SCRIPTSTATE ss)
{
	switch(ss) {
		case SCRIPTSTATE_UNINITIALIZED: return "SCRIPTSTATE_UNINITIALIZED";
		case SCRIPTSTATE_INITIALIZED: return "SCRIPTSTATE_INITIALIZED";
		case SCRIPTSTATE_STARTED: return "SCRIPTSTATE_STARTED";
		case SCRIPTSTATE_CONNECTED: return "SCRIPTSTATE_CONNECTED";
		case SCRIPTSTATE_DISCONNECTED: return "SCRIPTSTATE_DISCONNECTED";
		case SCRIPTSTATE_CLOSED: return "SCRIPTSTATE_CLOSED";
		default:
			return "unknown";
	}
}
/* }}} */
/* {{{ TWideString */
/* This class helps manipulate strings from OLE.
 * It does not use emalloc, so it is better suited for passing pointers
 * between threads. */
class TWideString {
	public:
		LPOLESTR m_ole;
		char *m_ansi;
		int m_ansi_strlen;

		TWideString(LPOLESTR olestr) {
			m_ole = olestr;
			m_ansi = NULL;
			m_ansi_strlen = 0;
		}
		TWideString(LPCOLESTR olestr) {
			m_ole = (LPOLESTR)olestr;
			m_ansi = NULL;
			m_ansi_strlen = 0;
		}

		~TWideString() {
			if (m_ansi) {
				CoTaskMemFree(m_ansi);
			}
			m_ansi = NULL;
			m_ansi_strlen = 0;
		}

		char *safe_ansi_string() {
			char *ret = ansi_string();
			if (ret == NULL)
				return "<NULL>";
			return ret;
		}

		int ansi_len(void) {
			/* force conversion if it has not already occurred */
			if (m_ansi == NULL)
				ansi_string();
			return m_ansi_strlen;
		}

		static BSTR bstr_from_ansi(char *ansi) {
			OLECHAR *ole = NULL;
			BSTR bstr = NULL;

			int req = MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);
			if (req) {
				ole = (OLECHAR*)CoTaskMemAlloc((req + 1) * sizeof(OLECHAR));
				if (ole) {
					req = MultiByteToWideChar(CP_ACP, 0, ansi, -1, ole, req);
					req--;
					ole[req] = 0;

					bstr = SysAllocString(ole);
					CoTaskMemFree(ole);
				}
			}
			return bstr;
		}

		char *ansi_string(void)
		{
			if (m_ansi)
				return m_ansi;

			if (m_ole == NULL)
				return NULL;

			int bufrequired = WideCharToMultiByte(CP_ACP, 0, m_ole, -1, NULL, 0, NULL, NULL);
			if (bufrequired) {

				m_ansi = (char*)CoTaskMemAlloc(bufrequired + 1);
				if (m_ansi) {
					m_ansi_strlen = WideCharToMultiByte(CP_ACP, 0, m_ole, -1, m_ansi, bufrequired + 1, NULL, NULL);

					if (m_ansi_strlen) {
						m_ansi_strlen--;
					} else {
						trace("conversion failed with return code %08x\n", GetLastError());	
					}
				}
				if (m_ansi) {
					m_ansi[m_ansi_strlen] = 0;
				}
			}
			return m_ansi;
		}
};
/* }}} */

/* {{{ code fragment structures */
enum fragtype {
	FRAG_MAIN,
	FRAG_SCRIPTLET,
	FRAG_PROCEDURE
};

typedef struct {
	enum fragtype fragtype;
	zend_op_array *opcodes;
	char *code;
	int persistent;	/* should be retained for Clone */
	int executed;	/* for "main" */
	char *functionname;
	unsigned int codelen;
	unsigned int starting_line;
	TPHPScriptingEngine *engine;
	void *ptr;
} code_frag;

#define FRAG_CREATE_FUNC	(char*)-1
static code_frag *compile_code_fragment(
		enum fragtype fragtype,
		char *functionname,
		LPCOLESTR code,
		ULONG starting_line,
		EXCEPINFO *excepinfo,
		TPHPScriptingEngine *engine
		TSRMLS_DC);

static int execute_code_fragment(code_frag *frag,
		VARIANT *varResult,
		EXCEPINFO *excepinfo
		TSRMLS_DC);
static void free_code_fragment(code_frag *frag TSRMLS_DC);
static code_frag *clone_code_fragment(code_frag *frag, TPHPScriptingEngine *engine TSRMLS_DC);

/* }}} */

/* These functions do some magic so that interfaces can be
 * used across threads without worrying about marshalling
 * or not marshalling, as appropriate.
 * Win95 without DCOM 1.1, and NT SP 2 or lower do not have
 * the GIT; so we emulate the GIT using other means.
 * If you trace problems back to this code, installing the relevant
 * SP should solve them.
 * */
static inline HRESULT GIT_get(DWORD cookie, REFIID riid, void **obj)
{
	IGlobalInterfaceTable *git;
	HRESULT ret;
	
	if (SUCCEEDED(CoCreateInstance(CLSID_StdGlobalInterfaceTable, NULL,
					CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable,
					(void**)&git))) {
		
		ret = git->GetInterfaceFromGlobal(cookie, riid, obj);
		git->Release();
		return ret;
	}
	return CoGetInterfaceAndReleaseStream((LPSTREAM)cookie, riid, obj);
}

static inline HRESULT GIT_put(IUnknown *unk, REFIID riid, DWORD *cookie)
{
	IGlobalInterfaceTable *git;
	HRESULT ret;
	
	if (SUCCEEDED(CoCreateInstance(CLSID_StdGlobalInterfaceTable, NULL,
					CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable,
					(void**)&git))) {
		
		ret = git->RegisterInterfaceInGlobal(unk, riid, cookie);
		git->Release();
		return ret;
	}
	return CoMarshalInterThreadInterfaceInStream(riid, unk, (LPSTREAM*)cookie);
}

static inline HRESULT GIT_revoke(DWORD cookie, IUnknown *unk)
{
	IGlobalInterfaceTable *git;
	HRESULT ret;
	
	if (SUCCEEDED(CoCreateInstance(CLSID_StdGlobalInterfaceTable, NULL,
					CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable,
					(void**)&git))) {
		
		ret = git->RevokeInterfaceFromGlobal(cookie);
		git->Release();
	}
	/* Kill remote clients */
	return CoDisconnectObject(unk, 0);
}



/* {{{ A generic stupid IDispatch implementation */
class IDispatchImpl:
	public IDispatch
{
protected:
	volatile LONG m_refcount;
public:
	/* IUnknown */
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject) {
		*ppvObject = NULL;

		if (IsEqualGUID(IID_IDispatch, iid)) {
			*ppvObject = (IDispatch*)this;
		} else if (IsEqualGUID(IID_IUnknown, iid)) {
			*ppvObject = this;	
		}
		if (*ppvObject) {
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	
	STDMETHODIMP_(DWORD) AddRef(void) {
		return InterlockedIncrement(const_cast<long*> (&m_refcount));
	}
	
	STDMETHODIMP_(DWORD) Release(void) {
		DWORD ret = InterlockedDecrement(const_cast<long*> (&m_refcount));
		trace("%08x: IDispatchImpl: release ref count is now %d\n", this, ret);
		if (ret == 0)
			delete this;
		return ret;
	}
	/* IDispatch */
	STDMETHODIMP GetTypeInfoCount(unsigned int *  pctinfo) {
		*pctinfo = 0;
		trace("%08x: IDispatchImpl: GetTypeInfoCount\n", this);
		return S_OK;
	}
	STDMETHODIMP GetTypeInfo( unsigned int iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
		trace("%08x: IDispatchImpl: GetTypeInfo\n", this);
		return DISP_E_BADINDEX;
	}
	STDMETHODIMP GetIDsOfNames( REFIID  riid, OLECHAR **rgszNames, unsigned int cNames, LCID lcid, DISPID *rgDispId)
	{
		unsigned int i;
		trace("%08x: IDispatchImpl: GetIDsOfNames: \n", this);
		for (i = 0; i < cNames; i++) {
			TWideString name(rgszNames[i]);
			trace("      %s\n", name.ansi_string());
		}
		trace("----\n");
		return DISP_E_UNKNOWNNAME;
	}
	STDMETHODIMP Invoke( DISPID  dispIdMember, REFIID  riid, LCID  lcid, WORD  wFlags,
		DISPPARAMS FAR*  pDispParams, VARIANT FAR*  pVarResult, EXCEPINFO FAR*  pExcepInfo,
		unsigned int FAR*  puArgErr)
	{
		trace("%08x: IDispatchImpl: Invoke dispid %08x\n", this, dispIdMember);
		return S_OK;
	}


	IDispatchImpl() {
		m_refcount = 1;
	}

	virtual ~IDispatchImpl() {
	}
};
/* }}} */
	STDMETHODIMP TPHPScriptingEngine::GetTypeInfoCount(unsigned int *  pctinfo) {
		*pctinfo = 0;
		trace("%08x: ScriptDispatch: GetTypeInfoCount\n", this);
		return S_OK;
	}
	STDMETHODIMP TPHPScriptingEngine::GetTypeInfo( unsigned int iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
		trace("%08x: ScriptDispatch: GetTypeInfo\n", this);
		return DISP_E_BADINDEX;
	}

int TPHPScriptingEngine::create_id(OLECHAR *name, DISPID *dispid TSRMLS_DC) 
{
	int ex = 0;
	char *lcname;
	int l;

	if (m_ids >= 1023) {
		trace("too many ids\n");
		return 0;
	}

	TWideString aname(name);

	l = aname.ansi_len();
	lcname = zend_str_tolower_dup(aname.ansi_string(), l);
	ex = zend_hash_exists(EG(function_table), lcname, l+1);
	efree(lcname);

	if (!ex) {
		trace("no such id %s\n", aname.ansi_string());
		return 0;
	}

	/* do we already have an id for this name? */
	int i;
	for (i = 0; i < m_ids; i++) {
		if (!strcasecmp(m_names[i], aname.ansi_string())) {
			trace("already had this ID\n");
			return i;
		}
	}

	m_lens[m_ids] = aname.ansi_len();
	m_names[m_ids] = aname.ansi_string();
	trace("created ID %d for name %s\n", m_ids, m_names[m_ids]);
	aname.m_ansi = NULL;
	*dispid = m_ids;
	m_ids++;
	return 1;
}

STDMETHODIMP TPHPScriptingEngine::GetIDsOfNames( REFIID  riid, OLECHAR **rgszNames, unsigned int cNames, LCID lcid, DISPID *rgDispId)
{
	unsigned int i;
	HRESULT ret = S_OK;
	TSRMLS_FETCH();

	if (tsrm_thread_id() != m_enginethread) {
		trace("GetIDsOfNames called from wrong thread\n");
		return RPC_E_WRONG_THREAD;
	}
	
	trace("%08x: ScriptDispatch: GetIDsOfNames %d names: \n", this, cNames);
	for (i = 0; i < cNames; i++) {
		if (!create_id(rgszNames[i], &rgDispId[i] TSRMLS_CC)) {
			ret = DISP_E_UNKNOWNNAME;
			break;
		}
	}
	return ret;
}

STDMETHODIMP TPHPScriptingEngine::Invoke( DISPID  dispIdMember, REFIID  riid, LCID  lcid, WORD  wFlags,
		DISPPARAMS FAR*  pdp, VARIANT FAR*  pvarRes, EXCEPINFO FAR*  pei,
		unsigned int FAR*  puArgErr)
{
	UINT i;
	zval *retval = NULL;
	zval ***params = NULL;
	HRESULT ret = DISP_E_MEMBERNOTFOUND;
	char *name;
	int namelen;
	TSRMLS_FETCH();

	if (tsrm_thread_id() != m_enginethread) {
		trace("Invoke called from wrong thread\n");
		return RPC_E_WRONG_THREAD;
	}
	
	name = m_names[dispIdMember];
	namelen = m_lens[dispIdMember];

	trace("%08x: ScriptDispatch: Invoke dispid %08x [%s]\n", this, dispIdMember, name);
	/* this code is similar to that of our com_wrapper InvokeEx implementation */

	/* convert args into zvals.
	 * Args are in reverse order */
	if (pdp->cArgs) {
		params = (zval ***)safe_emalloc(sizeof(zval **), pdp->cArgs, 0);
		for (i = 0; i < pdp->cArgs; i++) {
			VARIANT *arg;
			zval *zarg;

			arg = &pdp->rgvarg[ pdp->cArgs - 1 - i];

			trace("alloc zval for arg %d VT=%08x\n", i, V_VT(arg));

			ALLOC_INIT_ZVAL(zarg);
			php_com_wrap_variant(zarg, arg, CP_ACP TSRMLS_CC);
			params[i] = &zarg;
		}
	}

	trace("arguments processed, prepare to do some work\n");	

	/* TODO: if PHP raises an exception here, we should catch it
	 * and expose it as a COM exception */

#if 0
	if (wFlags & DISPATCH_PROPERTYGET) {
		retval = zend_read_property(Z_OBJCE_P(disp->object), disp->object, m_names[dispIdMember], m_lens[dispIdMember]+1, 1 TSRMLS_CC);
	} else if (wFlags & DISPATCH_PROPERTYPUT) {
		zend_update_property(Z_OBJCE_P(disp->object), disp->object, m_names[dispIdMember], m_lens[dispIdMember]+1, *params[0] TSRMLS_CC);
	} else
#endif
		if (wFlags & DISPATCH_METHOD) {
			zval *zname;
			MAKE_STD_ZVAL(zname);
			ZVAL_STRINGL(zname, (char*)name, namelen, 1);
			trace("invoke function %s\n", Z_STRVAL_P(zname));

			zend_try {

				if (SUCCESS == call_user_function_ex(CG(function_table), NULL, zname,
							&retval, pdp->cArgs, params, 1, NULL TSRMLS_CC)) {
					ret = S_OK;
					trace("we ran it\n");
				} else {
					ret = DISP_E_EXCEPTION;
					trace("no such function\n");
				}

			} zend_catch {
				ret = DISP_E_EXCEPTION;
				/* need to populate the exception here */
				trace("bork\n");
			} zend_end_try();
			
			zval_ptr_dtor(&zname);
		} else {
			trace("Don't know how to handle this invocation %08x\n", wFlags);
			ret = E_UNEXPECTED;
		}

	/* release arguments */
	for (i = 0; i < pdp->cArgs; i++)
		zval_ptr_dtor(params[i]);

	if (params)
		efree(params);

	/* return value */
	if (retval) {
		if (pvarRes) {
			VariantInit(pvarRes);
			trace("setting up return value\n");
			php_com_variant_from_zval(pvarRes, retval, CP_ACP TSRMLS_CC);
		}
		zval_ptr_dtor(&retval);
	} else if (pvarRes) {
		VariantInit(pvarRes);
	}

	trace("Invocation complete\n");

	return ret;
}

/* {{{ This object is used in conjunction with IActiveScriptParseProcedure to
 * allow scriptlets to be bound to events.  IE uses this for declaring
 * event handlers such as onclick="...".
 * The compiled code is stored in this object; IE will call
 * IDispatch::Invoke when the element is clicked.
 * */
class ScriptProcedureDispatch:
	public IDispatchImpl
{
public:
	code_frag 		*m_frag;
	DWORD			m_procflags;
	TPHPScriptingEngine *m_engine;
	DWORD			m_gitcookie;

	STDMETHODIMP Invoke( DISPID  dispIdMember, REFIID  riid, LCID  lcid, WORD  wFlags,
		DISPPARAMS FAR*  pDispParams, VARIANT FAR*  pVarResult, EXCEPINFO FAR*  pExcepInfo,
		unsigned int FAR*  puArgErr)
	{
		TSRMLS_FETCH();

		if (m_frag) {
			trace("%08x: Procedure Dispatch: Invoke dispid %08x\n", this, dispIdMember);
			execute_code_fragment(m_frag, NULL, NULL TSRMLS_CC);
		}
		return S_OK;
	}
	ScriptProcedureDispatch() {
		m_refcount = 1;
		GIT_put((IDispatch*)this, IID_IDispatch, &m_gitcookie);
	}
};
/* }}} */

/* {{{ code fragment management */
static code_frag *compile_code_fragment(
		enum fragtype fragtype,
		char *functionname,
		LPCOLESTR code,
		ULONG starting_line,
		EXCEPINFO *excepinfo,
		TPHPScriptingEngine *engine
		TSRMLS_DC)
{
	zval pv;
	int code_offs = 0;
	char namebuf[256];
	
	code_frag *frag = (code_frag*)CoTaskMemAlloc(sizeof(code_frag));
	memset(frag, 0, sizeof(code_frag));

	frag->engine = engine;
	
	/* handle the function name */
	if (functionname) {
		int namelen;
		if (functionname == FRAG_CREATE_FUNC) {
			ULONG n = ++engine->m_lambda_count;

			sprintf(namebuf, "__frag_%08x_%u", engine, n);
			functionname = namebuf;	
		}

		namelen = strlen(functionname);
		code_offs = namelen + sizeof("function (){");

		frag->functionname = (char*)CoTaskMemAlloc((namelen + 1) * sizeof(char));
		memcpy(frag->functionname, functionname, namelen+1);
	}

	frag->functionname = functionname;
	
trace("%08x: COMPILED FRAG\n", frag);
	
	frag->codelen = WideCharToMultiByte(CP_ACP, 0, code, -1, NULL, 0, NULL, NULL);
	frag->code = (char*)CoTaskMemAlloc(sizeof(char) * (frag->codelen + code_offs + 1));

	if (functionname) {
		sprintf(frag->code, "function %s(){ ", functionname);
	}
	
	frag->codelen = WideCharToMultiByte(CP_ACP, 0, code, -1, frag->code + code_offs, frag->codelen, NULL, NULL) - 1;
	
	if (functionname) {
		frag->codelen += code_offs + 1;
		frag->code[frag->codelen-1] = '}';
		frag->code[frag->codelen] = 0;
	}

trace("code to compile is:\ncode_offs=%d  func=%s\n%s\n", code_offs, functionname, frag->code);	
	
	frag->fragtype = fragtype;
	frag->starting_line = starting_line;
	
	pv.type = IS_STRING;
	pv.value.str.val = frag->code;
	pv.value.str.len = frag->codelen;

	frag->opcodes = compile_string(&pv, "fragment" TSRMLS_CC);

	if (frag->opcodes == NULL) {
		free_code_fragment(frag TSRMLS_CC);

		if (excepinfo) {
			memset(excepinfo, 0, sizeof(EXCEPINFO));
			excepinfo->wCode = 1000;
			excepinfo->bstrSource = TWideString::bstr_from_ansi("fragment");
			excepinfo->bstrDescription = TWideString::bstr_from_ansi("Problem while parsing/compiling");
		}
		
		return NULL;
	}
	
	return frag;
}

static void free_code_fragment(code_frag *frag TSRMLS_DC)
{
	switch(frag->fragtype) {
		case FRAG_PROCEDURE:
			if (frag->ptr) {
				ScriptProcedureDispatch *disp = (ScriptProcedureDispatch*)frag->ptr;
				disp->Release();
				GIT_revoke(disp->m_gitcookie, (IDispatch*)disp);
				frag->ptr = NULL;
			}
			break;
	}
	
	if (frag->opcodes) {
		destroy_op_array(frag->opcodes TSRMLS_CC);
		frag->opcodes = NULL;
	}

	if (frag->functionname) {
		CoTaskMemFree(frag->functionname);
		frag->functionname = NULL;
	}

	CoTaskMemFree(frag->code);
	CoTaskMemFree(frag);
}

static code_frag *clone_code_fragment(code_frag *frag, TPHPScriptingEngine *engine TSRMLS_DC)
{
	zval pv;
	code_frag *newfrag = (code_frag*)CoTaskMemAlloc(sizeof(code_frag));
	memset(newfrag, 0, sizeof(code_frag));

	newfrag->engine = engine;
trace("%08x: CLONED FRAG\n", newfrag);

	newfrag->persistent = frag->persistent;
	newfrag->codelen = frag->codelen;
	newfrag->code = (char*)CoTaskMemAlloc(sizeof(char) * frag->codelen + 1);
	memcpy(newfrag->code, frag->code, frag->codelen + 1);

	if (frag->functionname) {
		int namelen = strlen(frag->functionname);
		newfrag->functionname = (char*)CoTaskMemAlloc(sizeof(char) * (namelen + 1));
		memcpy(newfrag->functionname, frag->functionname, namelen+1);
	} else {
		newfrag->functionname = NULL;
	}
	
	newfrag->fragtype = frag->fragtype;
	newfrag->starting_line = frag->starting_line;
	
	pv.type = IS_STRING;
	pv.value.str.val = newfrag->code;
	pv.value.str.len = newfrag->codelen;

	/* we defer compilation until we are ready to execute,
	 * as we need the host to AddNamedItem certain autoglobals
	 * BEFORE we compile */
	newfrag->opcodes = NULL;

	return newfrag;
}

static int execute_code_fragment(code_frag *frag,
		VARIANT *varResult,
		EXCEPINFO *excepinfo
		TSRMLS_DC)
{
	zval *retval_ptr = NULL;

	if (frag->fragtype == FRAG_MAIN && frag->executed)
		return 1;

	/* compiled cloned fragment, JIT */
	if (frag->persistent && frag->opcodes == NULL) {
		zval pv;
		pv.type = IS_STRING;
		pv.value.str.val = frag->code;
		pv.value.str.len = frag->codelen;

		frag->opcodes = compile_string(&pv, "fragment (JIT)" TSRMLS_CC);

		if (!frag->opcodes) {
			trace("*** JIT compilation of cloned opcodes failed??");
			return 0;
		}
	}
	
	zend_try {
		trace("*** Executing code in thread %08x\n", tsrm_thread_id());

		if (frag->functionname) {

			zval fname;

			fname.type = IS_STRING;
			fname.value.str.val = frag->functionname;
			fname.value.str.len = strlen(frag->functionname);

			call_user_function_ex(CG(function_table), NULL, &fname, &retval_ptr, 0, NULL, 1, NULL TSRMLS_CC);

		} else {
			zend_fcall_info_cache fci_cache;
			zend_fcall_info fci;

			memset(&fci, 0, sizeof(fci));
			memset(&fci_cache, 0, sizeof(fci_cache));
		
			fci.size = sizeof(fci);
			fci.function_table = CG(function_table);
			fci.retval_ptr_ptr = &retval_ptr;
			fci.no_separation = 1;

			fci_cache.initialized = 1;
			fci_cache.function_handler = (zend_function*)frag->opcodes;
			frag->opcodes->type = ZEND_USER_FUNCTION;	// mini hack

			zend_call_function(&fci, &fci_cache TSRMLS_CC);
			
		}
	} zend_catch {
		trace("*** --> caught error while executing\n");
		if (frag->engine->m_in_main)
			frag->engine->m_stop_main = 1;
	} zend_end_try();

	if (frag->fragtype == FRAG_MAIN)
		frag->executed = 1;

	if (varResult)
		VariantInit(varResult);
	
	if (retval_ptr) {
		if (varResult)
			php_com_variant_from_zval(varResult, retval_ptr, CP_ACP TSRMLS_CC);
		zval_ptr_dtor(&retval_ptr);
	}

	return 1;
}

static void frag_dtor(void *pDest)
{
	TSRMLS_FETCH();
	code_frag *frag = *(code_frag**)pDest;
	free_code_fragment(frag TSRMLS_CC);
}
/* }}} */
	
void TPHPScriptingEngine::do_clone(TPHPScriptingEngine *src)
{
	TSRMLS_FETCH();
	zend_hash_apply_with_argument(&src->m_frags, clone_frags, this TSRMLS_CC); 
}

#if ACTIVEPHP_HAS_OWN_THREAD
struct engine_startup {
	HANDLE evt;
	HRESULT ret;
	TPHPScriptingEngine *toclone;
	TPHPScriptingEngine *localref;
};

static WNDCLASS wc = {0};

static LRESULT CALLBACK script_thread_msg_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_ACTIVEPHP_SERIALIZE:
			return marshal_stub(lparam);

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			
		default:
			return DefWindowProc(hwnd, message, wparam, lparam);
	}
}

static DWORD WINAPI script_thread(LPVOID param)
{
	struct engine_startup *su = (struct engine_startup*)param;
	TPHPScriptingEngine *engine;
	IUnknown *punk = NULL;
	MSG msg;

	trace("firing up engine thread/apartment\n");
	
	/* set up COM in this apartment */
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	/* create a window for message queueing */
	wc.lpfnWndProc = script_thread_msg_proc;
	wc.lpszClassName = "ActivePHP Message Window";
	RegisterClass(&wc);
	
	/* create the engine state */
	engine = new TPHPScriptingEngine;

	engine->m_enginethread = tsrm_thread_id();
	engine->m_basethread = 0;
	engine->m_queue = CreateWindow(wc.lpszClassName, wc.lpszClassName,
			0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, NULL, NULL);

	/* marshall it for another apartment */
	engine->QueryInterface(IID_IUnknown, (void**)&punk);
	su->ret = GIT_put(punk, IID_IUnknown, &engine->m_gitcookie);
	punk->Release();
	su->localref = engine;

	/* do we need to clone ? */
	if (su->toclone) {
		engine->do_clone(su->toclone);
	}
	
	/* tell whoever spawned us that we're ready and waiting */
	SetEvent(su->evt);

	/* pump COM messages */
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	trace("terminating engine thread\n");
	
	CoUninitialize();

	return 0;
}
#endif

IUnknown *create_scripting_engine(TPHPScriptingEngine *tobecloned)
{
	IUnknown *punk = NULL;
#if ACTIVEPHP_HAS_OWN_THREAD
	struct engine_startup su;
	HANDLE hthr;
	DWORD thid;

	su.evt = CreateEvent(NULL, TRUE, FALSE, NULL);
	su.toclone = tobecloned;

	hthr = CreateThread(NULL, 0, script_thread, &su, 0, &thid);
	if (hthr)
		CloseHandle(hthr);

	WaitForSingleObject(su.evt, INFINITE);

	if (SUCCEEDED(su.ret)) {
		punk = (IUnknown*)(void*)su.localref;
	}

	CloseHandle(su.evt);
#else
	punk = (IActiveScript*)new TPHPScriptingEngine;
#endif
	return punk;
}

void TPHPScriptingEngine::setup_engine_state(void)
{
	TSRMLS_FETCH();

	m_enginethread = tsrm_thread_id();
	trace("initializing zend engine on this thread\n");
	
	SG(options) |= SAPI_OPTION_NO_CHDIR;
	SG(headers_sent) = 1;
	SG(request_info).no_headers = 1;

	SG(server_context) = this;

	/* override the default PHP error callback */
	zend_error_cb = activescript_error_handler;

	zend_alter_ini_entry("register_argc_argv", 19, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
	zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
	zend_alter_ini_entry("implicit_flush", 15, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
	zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);

	php_request_startup(TSRMLS_C);
	PG(during_request_startup) = 0;
	zend_hash_init(&m_frags, 0, NULL, frag_dtor, 0);

	m_done_init = 1;
	
	trace("---- init done\n");
}

TPHPScriptingEngine::TPHPScriptingEngine()
{
	TSRMLS_FETCH();
	
	/* CTOR */

	trace("*** NEW this=%08x\n", this);

	m_scriptstate = SCRIPTSTATE_UNINITIALIZED;
	m_pass = NULL;
	m_in_main = 0;
	m_done_init = 0;
	m_stop_main = 0;
	m_lambda_count = 0;
	m_refcount = 1;
	m_ids = 0;
	TPHPClassFactory::AddToObjectCount();

#if ACTIVEPHP_HAS_OWN_THREAD
	setup_engine_state();
#endif

}

TPHPScriptingEngine::~TPHPScriptingEngine()
{
	/* DTOR */

	trace("\n\n *** Engine Destructor Called\n\n");

	if (m_done_init && m_scriptstate != SCRIPTSTATE_CLOSED) {
		Close();
	}

	while (m_ids--) {
		CoTaskMemFree(m_names[m_ids]);
	}

	TPHPClassFactory::RemoveFromObjectCount();
}

/* Set some executor globals and execute a zend_op_array.
 * The declaration looks wierd because this can be invoked from
 * zend_hash_apply_with_argument */
static int execute_main(void *pDest, void *arg TSRMLS_DC)
{
	code_frag *frag = *(code_frag**)pDest;

	if (frag->fragtype == FRAG_MAIN && !(frag->engine->m_in_main && frag->engine->m_stop_main))
		execute_code_fragment(frag, NULL, NULL TSRMLS_CC);

	return ZEND_HASH_APPLY_KEEP;
}

static int clone_frags(void *pDest, void *arg TSRMLS_DC)
{
	code_frag *frag, *src = *(code_frag**)pDest;
	TPHPScriptingEngine *engine = (TPHPScriptingEngine*)arg;

	if (src->persistent) {
		frag = clone_code_fragment(src, engine TSRMLS_CC);
		if (frag)
			zend_hash_next_index_insert(&engine->m_frags, &frag, sizeof(code_frag*), NULL);
		else
			trace("WARNING: clone failed!\n");
	}

	return ZEND_HASH_APPLY_KEEP;
}
	
/* Only call this in the context of the engine thread, or you'll be sorry.
 *
 * When SCRIPTITEM_GLOBALMEMBERS is set, we're only adding COM objects to the namespace.
 * We could add *all* properties, but I don't like this idea; what if the value changes
 * while the page is running?  We'd be left with stale data.
 * */

static inline void make_auto_global(char *name, zval *val TSRMLS_DC)
{
	int namelen = strlen(name);
trace("make_auto_global %s\n", name);
	zend_register_auto_global(name, namelen, NULL TSRMLS_CC);
	zend_auto_global_disable_jit(name, namelen TSRMLS_CC);
	ZEND_SET_SYMBOL(&EG(symbol_table), name, val);
}

void TPHPScriptingEngine::add_to_global_namespace(IDispatch *disp, DWORD flags, char *name TSRMLS_DC)
{
	zval *val;
	ITypeInfo *typ;
	int i;
	unsigned int namelen;
	FUNCDESC *func;
	BSTR funcname;
	TYPEATTR *attr;
	DISPPARAMS dispparams;
	VARIANT vres;
	ITypeInfo *rettyp;
	TYPEATTR *retattr;

trace("Add %s to global namespace\n", name);
	
	MAKE_STD_ZVAL(val);
	php_com_wrap_dispatch(val, disp, CP_ACP TSRMLS_CC);
	
	if (val == NULL) {
		disp->Release();
		return;
	}

	make_auto_global(name, val TSRMLS_CC);

	if ((flags & SCRIPTITEM_GLOBALMEMBERS) == 0) {
		disp->Release();
		return;
	}

	/* Enumerate properties and add those too */
	if (FAILED(disp->GetTypeInfo(0, 0, &typ))) {
		disp->Release();
		return;
	}

	if (SUCCEEDED(typ->GetTypeAttr(&attr))) {
		for (i = 0; i < attr->cFuncs; i++) {
			if (FAILED(typ->GetFuncDesc(i, &func)))
				continue;

			/* Look at its type */
			if (func->invkind == INVOKE_PROPERTYGET
					&& VT_PTR == func->elemdescFunc.tdesc.vt
					&& VT_USERDEFINED == func->elemdescFunc.tdesc.lptdesc->vt
					&& SUCCEEDED(typ->GetRefTypeInfo(func->elemdescFunc.tdesc.lptdesc->hreftype, &rettyp)))
			{
				if (SUCCEEDED(rettyp->GetTypeAttr(&retattr))) {
					if (retattr->typekind == TKIND_DISPATCH) {
						/* It's dispatchable */

						/* get the value */
						dispparams.cArgs = 0;
						dispparams.cNamedArgs = 0;
						VariantInit(&vres);

						if (SUCCEEDED(disp->Invoke(func->memid, IID_NULL, 0, func->invkind,
										&dispparams, &vres, NULL, NULL))) {

							/* Get its dispatch */
							IDispatch *sub = NULL;

							if (V_VT(&vres) == VT_UNKNOWN)
								V_UNKNOWN(&vres)->QueryInterface(IID_IDispatch, (void**)&sub);
							else if (V_VT(&vres) == VT_DISPATCH)
								sub = V_DISPATCH(&vres);

							if (sub) {
								/* find out its name */
								typ->GetDocumentation(func->memid, &funcname, NULL, NULL, NULL);
								name = php_com_olestring_to_string(funcname, &namelen, CP_ACP, 0);

								/* add to namespace */
								zval *subval;

								MAKE_STD_ZVAL(subval);
								
								php_com_wrap_dispatch(subval, sub, CP_ACP TSRMLS_CC);
								if (subval) {
									make_auto_global(name, subval TSRMLS_CC);
								}

								efree(name);
								SysFreeString(funcname);
							}
							VariantClear(&vres);
						}
					}
					rettyp->ReleaseTypeAttr(retattr);
				}
				rettyp->Release();
			}
			typ->ReleaseFuncDesc(func);
		}
		typ->ReleaseTypeAttr(attr);
	}
	disp->Release();
}

STDMETHODIMP_(DWORD) TPHPScriptingEngine::AddRef(void)
{
	DWORD ret;
	ret = InterlockedIncrement(const_cast<long*> (&m_refcount));
	trace("AddRef --> %d\n", ret);
	return ret;
}

STDMETHODIMP_(DWORD) TPHPScriptingEngine::Release(void)
{
	DWORD ret = InterlockedDecrement(const_cast<long*> (&m_refcount));
	if (ret == 0) {
		trace("%08x: Release: zero refcount, destroy the engine!\n", this);
		delete this;
	}
	trace("Release --> %d\n", ret);
	return ret;
}

STDMETHODIMP TPHPScriptingEngine::QueryInterface(REFIID iid, void **ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualGUID(IID_IActiveScript, iid)) {
		*ppvObject = (IActiveScript*)this;
	} else if (IsEqualGUID(IID_IActiveScriptParse, iid)) {
		*ppvObject = (IActiveScriptParse*)this;
	} else if (IsEqualGUID(IID_IActiveScriptParseProcedure, iid)) {
		*ppvObject = (IActiveScriptParseProcedure*)this;
	} else if (IsEqualGUID(IID_IObjectSafety, iid)) {
		*ppvObject = (IObjectSafety*)this;
	} else if (IsEqualGUID(IID_IUnknown, iid)) {
		*ppvObject = this;	
	} else if (IsEqualGUID(IID_IDispatch, iid)) {
		*ppvObject = (IDispatch*)this;
	} else {
		LPOLESTR guidw;
		StringFromCLSID(iid, &guidw);
		{
			TWideString guid(guidw);
			trace("%08x: QueryInterface for unsupported %s\n", this, guid.ansi_string());
		}
		CoTaskMemFree(guidw);
	}
	if (*ppvObject) {
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

/* This is called by the host to set the scrite site.
 * It also defines the base thread. */
STDMETHODIMP TPHPScriptingEngine::SetScriptSite(IActiveScriptSite *pass)
{
	HRESULT ret = S_OK;
	TSRMLS_FETCH();

	if (m_pass && pass) {
		trace("SetScriptSite: we're already set\n");
		return E_FAIL;
	}

	trace("%08x: SetScriptSite(%08x) -----> Base thread is %08x\n", this, pass, tsrm_thread_id());

	if (pass) {
		m_basethread = tsrm_thread_id();
#if ACTIVEPHP_HAS_OWN_THREAD
		HRESULT ret = GIT_put(pass, IID_IActiveScriptSite, &m_asscookie);
#endif
	}
	
	if (m_pass) {
#if ACTIVEPHP_HAS_OWN_THREAD
		trace("killing off ass cookie\n");
		GIT_revoke(m_asscookie, m_pass);
#endif
		m_pass->Release();
		m_pass = NULL;
	}

	if (pass == NULL) {
		trace("Closing down site; we should have no references to objects from the host\n"
				"  m_pass=%08x\n  What about named items??\n",
				m_pass);
	}
	
	if (pass) {
		trace("taking a ref on pass\n");
		ret = pass->QueryInterface(IID_IActiveScriptSite, (void**)&m_pass);
		trace("----> %s", php_win_err(ret));
		
		if (SUCCEEDED(ret)) {

#if !ACTIVEPHP_HAS_OWN_THREAD
			setup_engine_state();
#endif
			SetScriptState(SCRIPTSTATE_INITIALIZED);
		}
	}

	return ret;
}

STDMETHODIMP TPHPScriptingEngine::GetScriptSite(REFIID riid, void **ppvObject)
{
	HRESULT ret = S_FALSE;
	
	trace("%08x: GetScriptSite()\n", this);

	if (m_pass) {
		ASS_CALL(ret, QueryInterface, (riid, ppvObject))
	}

	return ret;
}

STDMETHODIMP TPHPScriptingEngine::SetScriptState(SCRIPTSTATE ss)
{
	HRESULT dummy = E_UNEXPECTED;
	int start_running = 0;
	TSRMLS_FETCH();

	if (tsrm_thread_id() != m_enginethread)
		return marshal_call(this, APHP_SetScriptState, 1, ss);
	
	trace("%08x: SetScriptState(current=%s, new=%s)\n",
			this,
			scriptstate_to_string(m_scriptstate),
			scriptstate_to_string(ss));

	if (m_scriptstate == SCRIPTSTATE_INITIALIZED && (ss == SCRIPTSTATE_STARTED || ss == SCRIPTSTATE_CONNECTED))
		start_running = 1;

	m_scriptstate = ss;

	if (start_running) {
		/* run "main()", as described in the docs */	
		if (m_pass) {
			ASS_CALL(dummy, OnEnterScript, ());
		}
		trace("%08x: apply execute main to m_frags\n", this);
		m_in_main = 1;
		m_stop_main = 0;
		zend_hash_apply_with_argument(&m_frags, execute_main, this TSRMLS_CC);
		m_in_main = 0;
		trace("%08x: --- done execute main\n", this);

		if (m_pass) {
			ASS_CALL(dummy, OnLeaveScript, ());
		}
	}

	/* inform host/site of the change */
	if (m_pass) {
#if 0
		if (ss == SCRIPTSTATE_INITIALIZED) {
			VARIANT varRes;
			VariantInit(&varRes);

//			ASS_CALL(dummy, OnScriptTerminate, (&varRes, NULL));
		}
#endif

		ASS_CALL(dummy, OnStateChange, (m_scriptstate));
	}

	
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::GetScriptState(SCRIPTSTATE *pssState)
{
	trace("%08x: GetScriptState(current=%s)\n", this, scriptstate_to_string(m_scriptstate));
	*pssState = m_scriptstate;
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::Close(void)
{
	TSRMLS_FETCH();
	
	trace("Close() refcount = %d\n", m_refcount);

	if (m_scriptstate == SCRIPTSTATE_CLOSED)
		return E_UNEXPECTED;

	if (m_done_init) {

		if (tsrm_thread_id() != m_enginethread)
			return marshal_call(this, APHP_Close, 0);
		
		m_done_init = 0;
		m_scriptstate = SCRIPTSTATE_CLOSED;
		zend_hash_destroy(&m_frags);
		php_request_shutdown(NULL);
		m_enginethread = 0;
	}


	if (m_pass && tsrm_thread_id() == m_basethread) {
		m_pass->OnStateChange(m_scriptstate);
	}
	
	//GIT_revoke(m_asscookie, m_pass);
	trace("%08x: release site \n", this);

	if (m_pass && tsrm_thread_id() == m_basethread) {
		m_pass->Release();
		m_pass = NULL;
	}

	return S_OK;
}

/* The Host uses this to add objects to the global namespace.  Some objects are
 * intended to have their child properties globally visible, so we add those to
 * the global namespace too.  */

STDMETHODIMP TPHPScriptingEngine::AddNamedItem(LPCOLESTR pstrName, DWORD dwFlags)
{
	HRESULT res;
	IUnknown *punk = NULL;
	ITypeInfo *ti = NULL;
	TSRMLS_FETCH();

	if (tsrm_thread_id() != m_enginethread)
		return marshal_call(this, APHP_AddNamedItem, 2, pstrName, dwFlags);
	
	TWideString name(pstrName);
	trace("AddNamedItem: %s (%08x) m_pass=%08x\n", name.ansi_string(), dwFlags, m_pass);
	
	res = m_pass->GetItemInfo(pstrName, SCRIPTINFO_IUNKNOWN, &punk, &ti);

	if (SUCCEEDED(res)) {
		IDispatch *disp = NULL;

		trace("ADD_NAMED_ITEM\n");

		res = punk->QueryInterface(IID_IDispatch, (void**)&disp);
		if (SUCCEEDED(res) && disp) {
			add_to_global_namespace(disp, dwFlags, name.ansi_string() TSRMLS_CC);
			disp->Release();
		} else {
			trace("Ouch: failed to get IDispatch for %s from GIT '%s'\n", name.ansi_string(), php_win_err(res));
		}

	} else {
		trace("failed to get named item, %s", php_win_err(res));
	}
	return res;
}

/* Bind to a type library */
STDMETHODIMP TPHPScriptingEngine::AddTypeLib( 
	/* [in] */ REFGUID rguidTypeLib,
	/* [in] */ DWORD dwMajor,
	/* [in] */ DWORD dwMinor,
	/* [in] */ DWORD dwFlags)
{
	HRESULT ret;
	ITypeLib *TypeLib;
	TSRMLS_FETCH();

	if (tsrm_thread_id() != m_enginethread)
		return marshal_call(this, APHP_AddTypeLib, 4, rguidTypeLib, dwMajor, dwMinor, dwFlags);

	ENGINE_THREAD_ONLY(IActiveScript, AddTypeLib);
	
	trace("AddTypeLib\n");
	ret = LoadRegTypeLib(rguidTypeLib, (USHORT)dwMajor, (USHORT)dwMinor, LANG_NEUTRAL, &TypeLib);

	if (SUCCEEDED(ret)) {
		php_com_import_typelib(TypeLib, CONST_CS, CP_ACP TSRMLS_CC);
		TypeLib->Release();
	}


	return ret;
}

/* Returns an object representing the PHP Scripting Engine.
 * Optionally, a client can request a particular item directly.
 * For the moment, we only do the bare minimum amount of work
 * for the engine to work correctly; we can flesh out this part
 * a little later. */
STDMETHODIMP TPHPScriptingEngine::GetScriptDispatch( 
	/* [in] */ LPCOLESTR pstrItemName,
	/* [out] */ IDispatch **ppdisp)
{
	zend_function *func = NULL;
	TSRMLS_FETCH();

	if (tsrm_thread_id() != m_enginethread) {
		return marshal_call(this, APHP_GetScriptDispatch, 2, pstrItemName, ppdisp);
	}
	
	*ppdisp = NULL;

	if (pstrItemName != NULL) {
		zval **tmp;
		TWideString itemname(pstrItemName);
	trace("GetScriptDispatch %s\n", itemname.ansi_string());

		/* Get that item from the global namespace.
		 * If it is an object, export it as a dispatchable object.
		 * */

		if (zend_hash_find(&EG(symbol_table), itemname.ansi_string(),
					itemname.ansi_len() + 1, (void**)&tmp) == SUCCESS) {
			if (Z_TYPE_PP(tmp) == IS_OBJECT) {
				*ppdisp = php_com_wrapper_export(*tmp TSRMLS_CC);
			}
		} else if (zend_hash_find(EG(function_table), itemname.ansi_string(),
					itemname.ansi_len() + 1, (void**)&func) == SUCCESS) {
			trace("The host wants a function, but we don't have one\n");
		}

	} else {
		trace("GetScriptDispatch NULL\n");
		/* This object represents PHP global namespace */
		*ppdisp = (IDispatch*)this;
	}

	
	if (*ppdisp) {
		return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP TPHPScriptingEngine::GetCurrentScriptThreadID( 
	/* [out] */ SCRIPTTHREADID *pstidThread)
{
	trace("%08x: GetCurrentScriptThreadID()\n", this);
	*pstidThread = GetCurrentThreadId();//m_enginethread;
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::GetScriptThreadID( 
	/* [in] */ DWORD dwWin32ThreadId,
	/* [out] */ SCRIPTTHREADID *pstidThread)
{
	trace("%08x: GetScriptThreadID()\n", this);
	*pstidThread = dwWin32ThreadId;
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::GetScriptThreadState( 
	/* [in] */ SCRIPTTHREADID stidThread,
	/* [out] */ SCRIPTTHREADSTATE *pstsState)
{
	trace("%08x: GetScriptThreadState()\n", this);
	*pstsState = SCRIPTTHREADSTATE_NOTINSCRIPT;
	switch(stidThread) {
		case SCRIPTTHREADID_BASE:
			stidThread = m_basethread;
			break;
		case SCRIPTTHREADID_CURRENT:
			stidThread = m_enginethread;
			break;
	};
	if (stidThread == m_basethread) {
		*pstsState = SCRIPTTHREADSTATE_NOTINSCRIPT;
	} else if (stidThread == m_enginethread) {
		*pstsState = SCRIPTTHREADSTATE_NOTINSCRIPT;
	}
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::InterruptScriptThread( 
	/* [in] */ SCRIPTTHREADID stidThread,
	/* [in] */ const EXCEPINFO *pexcepinfo,
	/* [in] */ DWORD dwFlags)
{
	/* do not serialize this method, or call into the script site */
	trace("%08x: InterruptScriptThread()\n", this);
	return S_OK;
}

/* Clone is essential when running under Windows Script Host.
 * It creates an engine to parse the code.  Once it is parsed,
 * the host clones other engines from the original and runs those.
 * It is intended to be a fast method of running the same script
 * multiple times in multiple threads. */
STDMETHODIMP TPHPScriptingEngine::Clone( 
	/* [out] */ IActiveScript **ppscript)
{
	HRESULT ret = E_FAIL;
	IUnknown *punk;
	
	trace("*************  Clone this[%08x]\n", this);
	
	punk = create_scripting_engine(this);
	
	if (punk) {
		ret = punk->QueryInterface(IID_IActiveScript, (void**)ppscript);
		punk->Release();
	}

	return ret;
}


STDMETHODIMP TPHPScriptingEngine::InitNew( void)
{
	TSRMLS_FETCH();
	trace("InitNew() this=%08x\n", this);
	if (m_done_init) {
		if (tsrm_thread_id() != m_enginethread)
			return marshal_call(this, APHP_InitNew, 0);

		zend_hash_destroy(&m_frags);
		php_request_shutdown(NULL);
		setup_engine_state();
	}
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::AddScriptlet( 
	/* [in] */ LPCOLESTR pstrDefaultName,
	/* [in] */ LPCOLESTR pstrCode,
	/* [in] */ LPCOLESTR pstrItemName,
	/* [in] */ LPCOLESTR pstrSubItemName,
	/* [in] */ LPCOLESTR pstrEventName,
	/* [in] */ LPCOLESTR pstrDelimiter,
	/* [in] */ DWORD dwSourceContextCookie,
	/* [in] */ ULONG ulStartingLineNumber,
	/* [in] */ DWORD dwFlags,
	/* [out] */ BSTR *pbstrName,
	/* [out] */ EXCEPINFO *pexcepinfo)
{
	HRESULT ret;
	TSRMLS_FETCH();

	if (tsrm_thread_id() != m_enginethread)
		return marshal_call(this, APHP_AddScriptlet, 11, pstrDefaultName, pstrCode, pstrItemName, pstrSubItemName, pstrEventName, pstrDelimiter, dwSourceContextCookie, ulStartingLineNumber, dwFlags, pbstrName, pexcepinfo);
	
	ENGINE_THREAD_ONLY(IActiveScriptParse, AddScriptlet);
	
trace("AddScriptlet\n");
	
	/* Parse/compile a chunk of script that will act as an event handler.
	 * If the host supports IActiveScriptParseProcedure, this code will
	 * not be called.
	 * The docs are (typically) vague: AFAICT, once the code has been
	 * compiled, we are supposed to arrange for an IConnectionPoint
	 * advisory connection to the item/subitem, once the script
	 * moves into SCRIPTSTATE_CONNECTED.
	 * That's a lot of work!
	 *
	 * FIXME: this is currently almost useless
	 * */

	TWideString
		default_name(pstrDefaultName),
		code(pstrCode),
		item_name(pstrItemName),
		sub_item_name(pstrSubItemName),
		event_name(pstrEventName),
		delimiter(pstrDelimiter);

	/* lets invent a function name for the scriptlet */
	char sname[256];

	/* should check if the name is already used! */
	if (pstrDefaultName) 
		strcpy(sname, default_name.ansi_string());
	else {
		sname[0] = 0;
		strcat(sname, "__");
		if (pstrItemName) {
			strcat(sname, item_name.ansi_string());
			strcat(sname, "_");
		}
		if (pstrSubItemName) {
			strcat(sname, sub_item_name.ansi_string());
			strcat(sname, "_");
		}
		if (pstrEventName)
			strcat(sname, event_name.ansi_string());
	}


	trace("%08x: AddScriptlet:\n state=%s\n name=%s\n code=%s\n item=%s\n subitem=%s\n event=%s\n delim=%s\n line=%d\n",
			this, scriptstate_to_string(m_scriptstate),
			default_name.safe_ansi_string(), code.safe_ansi_string(), item_name.safe_ansi_string(),
			sub_item_name.safe_ansi_string(), event_name.safe_ansi_string(), delimiter.safe_ansi_string(),
			ulStartingLineNumber);


	code_frag *frag = compile_code_fragment(
			FRAG_SCRIPTLET,
			sname,
			pstrCode,
			ulStartingLineNumber,
			pexcepinfo,
			this
			TSRMLS_CC);

	if (frag) {

		frag->persistent = (dwFlags & SCRIPTTEXT_ISPERSISTENT);

		zend_hash_next_index_insert(&m_frags, &frag, sizeof(code_frag*), NULL);

		/*
			ScriptProcedureDispatch *disp = new ScriptProcedureDispatch;

			disp->AddRef();
			disp->m_frag = frag;
			disp->m_procflags = info->dwFlags;
			disp->m_engine = this;
			frag->ptr = disp;

		 *ppdisp = disp;
		 */
		ret = S_OK;
	} else {
		ret = DISP_E_EXCEPTION;
	}

	*pbstrName = TWideString::bstr_from_ansi(sname);

	trace("%08x: done with scriptlet %s\n", this, sname);

	
	return ret;
}

STDMETHODIMP TPHPScriptingEngine::ParseScriptText( 
	/* [in] */ LPCOLESTR pstrCode,
	/* [in] */ LPCOLESTR pstrItemName,
	/* [in] */ IUnknown *punkContext,
	/* [in] */ LPCOLESTR pstrDelimiter,
	/* [in] */ DWORD dwSourceContextCookie,
	/* [in] */ ULONG ulStartingLineNumber,
	/* [in] */ DWORD dwFlags,
	/* [out] */ VARIANT *pvarResult,
	/* [out] */ EXCEPINFO *pexcepinfo)
{
trace("ParseScriptText\n");
	int doexec;
	TWideString
		code(pstrCode),
		item_name(pstrItemName),
		delimiter(pstrDelimiter);
	HRESULT ret;
	TSRMLS_FETCH();

	trace("pstrCode=%p pstrItemName=%p punkContext=%p pstrDelimiter=%p pvarResult=%p pexcepinfo=%p\n",
			pstrCode, pstrItemName, punkContext, pstrDelimiter, pvarResult, pexcepinfo);

	if (tsrm_thread_id() != m_enginethread) {
		return marshal_call(this, APHP_ParseScriptText, 9,
				pstrCode, pstrItemName, punkContext, pstrDelimiter,
				dwSourceContextCookie, ulStartingLineNumber,
				dwFlags, pvarResult, pexcepinfo);
	}
	
	trace("%08x: ParseScriptText:\n state=%s\ncode=%.*s\n item=%s\n delim=%s\n line=%d\n",
			this, scriptstate_to_string(m_scriptstate),
			code.m_ansi_strlen,
			code.safe_ansi_string(), item_name.safe_ansi_string(), delimiter.safe_ansi_string(),
			ulStartingLineNumber);

	code_frag *frag = compile_code_fragment(
			FRAG_MAIN,
			dwFlags & SCRIPTTEXT_ISEXPRESSION ? FRAG_CREATE_FUNC : NULL,
			pstrCode,
			ulStartingLineNumber,
			pexcepinfo,
			this
			TSRMLS_CC);

	doexec = (dwFlags & SCRIPTTEXT_ISEXPRESSION) ||
		m_scriptstate == SCRIPTSTATE_STARTED ||
		m_scriptstate == SCRIPTSTATE_CONNECTED ||
		m_scriptstate == SCRIPTSTATE_DISCONNECTED;

	if (frag) {
		frag->persistent = (dwFlags & SCRIPTTEXT_ISPERSISTENT);
		ret = S_OK;

		if (dwFlags & SCRIPTTEXT_ISEXPRESSION) {
			if (m_scriptstate == SCRIPTSTATE_INITIALIZED) {
				/* not allowed to execute code in this state */
				ret = E_UNEXPECTED;
				doexec = 0;
			}
		}

		if (doexec) {
			/* execute the code as an expression */
			if (!execute_code_fragment(frag, pvarResult, pexcepinfo TSRMLS_CC))
				ret = DISP_E_EXCEPTION;
		}

		zend_hash_next_index_insert(&m_frags, &frag, sizeof(code_frag*), NULL);
	} else {
		ret = DISP_E_EXCEPTION;
	}

	if (pvarResult) {
		VariantInit(pvarResult);
	}


	return ret;
}

STDMETHODIMP TPHPScriptingEngine::ParseProcedureText( 
		/* [in] */ LPCOLESTR pstrCode,
	   	/* [in] */ LPCOLESTR pstrFormalParams,
   		/* [in] */ LPCOLESTR pstrProcedureName,
		/* [in] */ LPCOLESTR pstrItemName,
		/* [in] */ IUnknown *punkContext,
		/* [in] */ LPCOLESTR pstrDelimiter,
		/* [in] */ DWORD dwSourceContextCookie,
		/* [in] */ ULONG ulStartingLineNumber,
		/* [in] */ DWORD dwFlags,
		/* [out] */ IDispatch **ppdisp)
{
trace("ParseProcedureText\n");
	ENGINE_THREAD_ONLY(IActiveScriptParseProcedure, ParseProcedureText);
	/* This is the IActiveScriptParseProcedure implementation.
	 * IE uses this to request for an IDispatch that it will invoke in
	 * response to some event, and tells us the code that it wants to
	 * run.
	 * We compile the code and pass it back a dispatch object.
	 * The object will then serialize access to the engine thread and
	 * execute the opcodes */
	TWideString
		formal_params(pstrFormalParams),
		procedure_name(pstrProcedureName),
		item_name(pstrItemName),
		delimiter(pstrDelimiter);
	HRESULT ret;
	TSRMLS_FETCH();

	
	trace("%08x: ParseProc:\n state=%s\nparams=%s\nproc=%s\nitem=%s\n delim=%s\n line=%d\n",
			this, scriptstate_to_string(m_scriptstate),
			formal_params.ansi_string(), procedure_name.ansi_string(),
			item_name.safe_ansi_string(), delimiter.safe_ansi_string(),
			ulStartingLineNumber);

	code_frag *frag = compile_code_fragment(
			FRAG_PROCEDURE,
			NULL,
			pstrCode,
			ulStartingLineNumber,
			NULL,
			this
			TSRMLS_CC);

	if (frag) {

		frag->persistent = (dwFlags & SCRIPTTEXT_ISPERSISTENT);
		zend_hash_next_index_insert(&m_frags, &frag, sizeof(code_frag*), NULL);

		ScriptProcedureDispatch *disp = new ScriptProcedureDispatch;

		disp->m_frag = frag;
		disp->m_procflags = dwFlags;
		disp->m_engine = this;
		frag->ptr = disp;

		*ppdisp = (IDispatch*)disp;
	} else {
		ret = DISP_E_EXCEPTION;
	}


	trace("ParseProc: ret=%08x disp=%08x\n", ret, *ppdisp);
	return ret;
}

STDMETHODIMP TPHPScriptingEngine::GetInterfaceSafetyOptions(
		/* [in]  */ REFIID	riid,						// Interface that we want options for
		/* [out] */ DWORD	*pdwSupportedOptions,		// Options meaningful on this interface
		/* [out] */ DWORD	*pdwEnabledOptions)			// current option values on this interface
{
	/* PHP isn't really safe for untrusted anything */
	trace("GetInterfaceSafetyOptions called\n");
	*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_DATA|INTERFACESAFE_FOR_UNTRUSTED_CALLER;
	*pdwEnabledOptions = 0;
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::SetInterfaceSafetyOptions(
		/* [in]  */ REFIID		riid,					// Interface to set options for
		/* [in]  */ DWORD		dwOptionSetMask,		// Options to change
		/* [in]  */ DWORD		dwEnabledOptions)		// New option values
{
	/* PHP isn't really safe for untrusted anything */
	trace("SetInterfaceSafetyOptions mask=%08x enabled=%08x\n", dwOptionSetMask, dwEnabledOptions);
	if (dwEnabledOptions == 0) {
		return S_OK;
	}
	trace("can't set those options; we're not safe enough\n");
	return E_FAIL;
}

#if 0
STDMETHODIMP TPHPScriptingEngine::GetUnmarshalClass(
		REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, CLSID *pCid)
{
	return E_NOTIMPL;
}

STDMETHODIMP TPHPScriptingEngine::GetMarshalSizeMax(
		REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, ULONG *pSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP TPHPScriptingEngine::MarshalInterface(
		IStream *pStm, REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshflags)
{
	return E_NOTIMPL;
}

STDMETHODIMP TPHPScriptingEngine::UnmarshalInterface(
		IStream *pStm, REFIID riid, void **ppv)
{
	return E_NOTIMPL;
}

STDMETHODIMP TPHPScriptingEngine::ReleaseMarshalData(IStream *pStm)
{
	return E_NOTIMPL;
}

STDMETHODIMP TPHPScriptingEngine::DisconnectObject(DWORD dwReserved)
{
	return E_NOTIMPL;
}
#endif	

class TActiveScriptError:
	public IActiveScriptError
{
protected:
	volatile LONG m_refcount;
public:
	/* IUnknown */
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject) {
		*ppvObject = NULL;

		if (IsEqualGUID(IID_IActiveScriptError, iid)) {
			*ppvObject = (IActiveScriptError*)this;
		} else if (IsEqualGUID(IID_IUnknown, iid)) {
			*ppvObject = this;	
		}
		if (*ppvObject) {
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	
	STDMETHODIMP_(DWORD) AddRef(void) {
		return InterlockedIncrement(const_cast<long*> (&m_refcount));
	}
	
	STDMETHODIMP_(DWORD) Release(void) {
		DWORD ret = InterlockedDecrement(const_cast<long*> (&m_refcount));
		trace("Release: errobj refcount=%d\n", ret);
		if (ret == 0)
			delete this;
		return ret;
	}

	HRESULT STDMETHODCALLTYPE GetExceptionInfo( 
			/* [out] */ EXCEPINFO *pexcepinfo)
	{
		memset(pexcepinfo, 0, sizeof(EXCEPINFO));
		pexcepinfo->bstrDescription = SysAllocString(m_message);
		pexcepinfo->bstrSource = SysAllocString(m_filename);
		pexcepinfo->wCode = 1000;	
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetSourcePosition( 
			/* [out] */ DWORD *pdwSourceContext,
			/* [out] */ ULONG *pulLineNumber,
			/* [out] */ LONG *plCharacterPosition)
	{
		if (pdwSourceContext)
			*pdwSourceContext = 0;
		if (pulLineNumber)
			*pulLineNumber = m_lineno;
		if (plCharacterPosition)
			*plCharacterPosition = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetSourceLineText( 
			/* [out] */ BSTR *pbstrSourceLine)
	{
		*pbstrSourceLine = NULL;
		return E_FAIL;
	}

	BSTR m_filename, m_message;
	UINT m_lineno;
	
	TActiveScriptError(const char *filename, const uint lineno, const char *message)
	{
		m_refcount = 0; /* start with zero refcount because this object is passed
						 * directly to the script site; it will call addref */
		m_filename = TWideString::bstr_from_ansi((char*)filename);
		m_message = TWideString::bstr_from_ansi((char*)message);
		m_lineno = lineno;
	}

	~TActiveScriptError()
	{
		trace("%08x: cleaning up error object\n", this);
		SysFreeString(m_filename);
		SysFreeString(m_message);
	}
};

extern "C"
void activescript_error_handler(int type, const char *error_filename,
		const uint error_lineno, const char *format, va_list args)
{
	TSRMLS_FETCH();
	char *buf;
	int buflen;
	TPHPScriptingEngine *engine = (TPHPScriptingEngine*)SG(server_context);
	
	/* ugly way to detect an exception for an eval'd fragment */
	if (type == E_ERROR && EG(exception) && strstr("Exception thrown without a stack frame", format)) {
		zend_exception_error(EG(exception) TSRMLS_CC);
		/* NOTREACHED -- recursive E_ERROR */
	} else {
		buflen = vspprintf(&buf, PG(log_errors_max_len), format, args);
		trace("%08x: PHP Error: %s\n", engine, buf);
	}
	
	TActiveScriptError *eobj = new TActiveScriptError(error_filename, error_lineno, buf);
	trace("raising error object!\n");
	if (engine->m_pass)
		engine->m_pass->OnScriptError(eobj);
	
	switch(type) {
		case E_ERROR:
		case E_CORE_ERROR:
		case E_COMPILE_ERROR:
		case E_USER_ERROR:
		case E_PARSE:
			/* now throw the exception to abort execution */
			zend_bailout();	
			break;
	}
	efree(buf);
}

