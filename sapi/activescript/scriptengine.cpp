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
#include "php4activescript.h"
#include "ext/rpc/com/com.h"
#include "ext/rpc/com/php_COM.h"
#include "ext/rpc/com/conversion.h"
}
#include "php_ticks.h"
#include "php4as_scriptengine.h"
#include "php4as_classfactory.h"
#include <objbase.h>

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
		}
		TWideString(LPCOLESTR olestr) {
			m_ole = (LPOLESTR)olestr;
			m_ansi = NULL;
		}

		~TWideString() {
			if (m_ansi) {
				CoTaskMemFree(m_ansi);
			}
			m_ansi = NULL;
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
						m_ansi[m_ansi_strlen] = 0;

					} else {
						trace("conversion failed with return code %08x\n", GetLastError());	
					}
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
static void free_code_fragment(code_frag *frag);
static code_frag *clone_code_fragment(code_frag *frag, TPHPScriptingEngine *engine TSRMLS_DC);

/* }}} */

/* Magic for handling threading correctly */
static inline HRESULT SEND_THREAD_MESSAGE(TPHPScriptingEngine *engine, LONG msg, WPARAM wparam, LPARAM lparam TSRMLS_DC)
{
	if (engine->m_enginethread == 0)
		return E_UNEXPECTED;
	if (tsrm_thread_id() == (engine)->m_enginethread) 
		return (engine)->engine_thread_handler((msg), (wparam), (lparam), NULL TSRMLS_CC);
	return (engine)->SendThreadMessage((msg), (wparam), (lparam));
}

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

/* {{{ This object represents the PHP engine to the scripting host.
 * Although the docs say it's implementation is optional, I found that
 * the Windows Script host would crash if we did not provide it. */
class ScriptDispatch:
	public IDispatchImpl
{
public:
	ScriptDispatch() {
		m_refcount = 1;
	}
};
/* }}} */

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
			SEND_THREAD_MESSAGE(m_engine, PHPSE_EXEC_PROC, 0, (LPARAM)this TSRMLS_CC);
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
		free_code_fragment(frag);

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

static void free_code_fragment(code_frag *frag)
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
	
	if (frag->opcodes)
		destroy_op_array(frag->opcodes);
	if (frag->functionname)
		CoTaskMemFree(frag->functionname);
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

	newfrag->opcodes = compile_string(&pv, "fragment" TSRMLS_CC);

	if (newfrag->opcodes == NULL) {
		free_code_fragment(newfrag);
/*
		if (excepinfo) {
			memset(excepinfo, 0, sizeof(EXCEPINFO));
			excepinfo->wCode = 1000;
			excepinfo->bstrSource = TWideString::bstr_from_ansi("fragment");
			excepinfo->bstrDescription = TWideString::bstr_from_ansi("Problem while parsing/compiling");
		}
*/		
		return NULL;
	}
	
	return newfrag;

}

static int execute_code_fragment(code_frag *frag,
		VARIANT *varResult,
		EXCEPINFO *excepinfo
		TSRMLS_DC)
{
	zval *retval_ptr = NULL;
	jmp_buf	*orig_jmpbuf;
	jmp_buf err_trap;

	if (frag->fragtype == FRAG_MAIN && frag->executed)
		return 1;
	
	orig_jmpbuf = frag->engine->m_err_trap;
	frag->engine->m_err_trap = &err_trap;

	if (setjmp(err_trap) == 0) {
		trace("*** Executing code in thread %08x\n", tsrm_thread_id());

		if (frag->functionname) {

			zval fname;

			fname.type = IS_STRING;
			fname.value.str.val = frag->functionname;
			fname.value.str.len = strlen(frag->functionname);

			call_user_function_ex(CG(function_table), NULL, &fname, &retval_ptr, 0, NULL, 1, NULL TSRMLS_CC);

		} else {
			zend_op_array 		*active_op_array		= EG(active_op_array);
			zend_function_state	*function_state_ptr		= EG(function_state_ptr);
			zval				**return_value_ptr_ptr	= EG(return_value_ptr_ptr);
			zend_op				**opline_ptr			= EG(opline_ptr);

			EG(return_value_ptr_ptr)	= &retval_ptr;
			EG(active_op_array)			= frag->opcodes;
			EG(no_extensions)			= 1;

			zend_execute(frag->opcodes TSRMLS_CC);

			EG(no_extensions)			= 0;
			EG(opline_ptr)				= opline_ptr;
			EG(active_op_array)			= active_op_array;
			EG(function_state_ptr)		= function_state_ptr;
			EG(return_value_ptr_ptr)	= return_value_ptr_ptr;
		}
	} else {
		trace("*** --> caught error while executing\n");
		if (frag->engine->m_in_main)
			frag->engine->m_stop_main = 1;
	}

	frag->engine->m_err_trap = orig_jmpbuf;

	if (frag->fragtype == FRAG_MAIN)
		frag->executed = 1;

	if (varResult)
		VariantInit(varResult);
	
	if (retval_ptr) {
		if (varResult)
			php_pval_to_variant(retval_ptr, varResult, CP_ACP TSRMLS_CC);
		zval_ptr_dtor(&retval_ptr);
	}

	return 1;
}

static void frag_dtor(void *pDest)
{
	code_frag *frag = *(code_frag**)pDest;
	free_code_fragment(frag);
}
/* }}} */

/* glue for getting back into the OO land */
static DWORD WINAPI begin_engine_thread(LPVOID param)
{
	TPHPScriptingEngine *engine = (TPHPScriptingEngine*)param;
	engine->engine_thread_func();
	trace("engine thread has really gone away!\n");
	return 0;
}

TPHPScriptingEngine::TPHPScriptingEngine()
{
	m_scriptstate = SCRIPTSTATE_UNINITIALIZED;
	m_pass = NULL;
	m_in_main = 0;
	m_stop_main = 0;
	m_err_trap = NULL;
	m_lambda_count = 0;
	m_pass_eng = NULL;
	m_refcount = 1;
	m_basethread = tsrm_thread_id();
	m_mutex = tsrm_mutex_alloc();
	m_sync_thread_msg = CreateEvent(NULL, TRUE, FALSE, NULL);
	TPHPClassFactory::AddToObjectCount();
	
	m_engine_thread_handle = CreateThread(NULL, 0, begin_engine_thread, this, 0, &m_enginethread);
	CloseHandle(m_engine_thread_handle);
}

void activescript_run_ticks(int count)
{
	MSG msg;
	TSRMLS_FETCH();
	TPHPScriptingEngine *engine;
	
	trace("ticking %d\n", count);
	
	engine = (TPHPScriptingEngine*)SG(server_context);

/*	PostThreadMessage(engine->m_enginethread, PHPSE_DUMMY_TICK, 0, 0); */
	
	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (msg.hwnd) {
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			break;
		}
	}
}

/* Synchronize with the engine thread */
HRESULT TPHPScriptingEngine::SendThreadMessage(LONG msg, WPARAM wparam, LPARAM lparam)
{
	HRESULT ret;

	if (m_enginethread == 0)
		return E_UNEXPECTED;

	trace("I'm waiting for a mutex in SendThreadMessage\n   this=%08x ethread=%08x msg=%08x\n",
			this, m_enginethread, msg);
	
	tsrm_mutex_lock(m_mutex);
	ResetEvent(m_sync_thread_msg);

	/* If we call PostThreadMessage before the thread has created the queue, the message
	 * posting fails.  MSDN docs recommend the following course of action */
	while (!PostThreadMessage(m_enginethread, msg, wparam, lparam)) {
		Sleep(50);
		if (m_enginethread == 0) {
			tsrm_mutex_unlock(m_mutex);
			trace("breaking out of dodgy busy wait\n");
			return E_UNEXPECTED;
		}
	}

	/* Wait for the event object to be signalled.
	 * This is a nice "blocking without blocking" wait; window messages are dispatched
	 * and everything works out quite nicely */
	while(1) {
		DWORD result = MsgWaitForMultipleObjects(1, &m_sync_thread_msg, FALSE, 4000, QS_ALLINPUT);

		if (result == WAIT_OBJECT_0 + 1) {
			/* Dispatch some messages */
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				//trace("dispatching message while waiting\n"); 
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else if (result == WAIT_TIMEOUT) {
			trace("timeout while waiting for thread reply\n");

		} else {
			/* the event was signalled */
			break;
		}
	}
	ret = m_sync_thread_ret;
	ResetEvent(m_sync_thread_msg);
	tsrm_mutex_unlock(m_mutex);
	return ret;
}

TPHPScriptingEngine::~TPHPScriptingEngine()
{
	trace("\n\n *** Engine Destructor Called\n\n");
	if (m_scriptstate != SCRIPTSTATE_UNINITIALIZED && m_scriptstate != SCRIPTSTATE_CLOSED && m_enginethread)
		Close();

	PostThreadMessage(m_enginethread, WM_QUIT, 0, 0);

	TPHPClassFactory::RemoveFromObjectCount();
	tsrm_mutex_free(m_mutex);
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
	
HRESULT TPHPScriptingEngine::engine_thread_handler(LONG msg, WPARAM wparam, LPARAM lParam, int *handled TSRMLS_DC)
{
	HRESULT ret = S_OK;

	trace("engine_thread_handler: running in thread %08x, should be %08x msg=%08x this=%08x\n",
			tsrm_thread_id(), m_enginethread, msg, this);

	if (handled)
		*handled = 1;
	
	if (m_enginethread == 0)
		return E_UNEXPECTED;
	
	switch(msg) {
		case PHPSE_ADD_TYPELIB:
			{
				struct php_active_script_add_tlb_info *info = (struct php_active_script_add_tlb_info*)lParam;
				ITypeLib *TypeLib;

				if (SUCCEEDED(LoadRegTypeLib(*info->rguidTypeLib, (USHORT)info->dwMajor,
								(USHORT)info->dwMinor, LANG_NEUTRAL, &TypeLib))) {
					php_COM_load_typelib(TypeLib, CONST_CS TSRMLS_CC);
					TypeLib->Release();
				}
			}
			break;
		case PHPSE_STATE_CHANGE:
			{
				/* handle the state change here */
				SCRIPTSTATE ss = (SCRIPTSTATE)lParam;
				int start_running = 0;
				trace("%08x: DoSetScriptState(current=%s, new=%s)\n",
						this,
						scriptstate_to_string(m_scriptstate),
						scriptstate_to_string(ss));

				if (m_scriptstate == SCRIPTSTATE_INITIALIZED && (ss == SCRIPTSTATE_STARTED || ss == SCRIPTSTATE_CONNECTED))
					start_running = 1;

				m_scriptstate = ss;

				/* inform host/site of the change */
				if (m_pass_eng)
					m_pass_eng->OnStateChange(m_scriptstate);

				if (start_running) {
					/* run "main()", as described in the docs */	
					if (m_pass_eng)
						m_pass_eng->OnEnterScript();
					trace("%08x: apply execute main to m_frags\n", this);
					m_in_main = 1;
					m_stop_main = 0;
					zend_hash_apply_with_argument(&m_frags, execute_main, this TSRMLS_CC);
					m_in_main = 0;
					trace("%08x: --- done execute main\n", this);
					if (m_pass_eng)
						m_pass_eng->OnLeaveScript();

					/* docs are a bit ambiguous here, but it appears that we should
					 * inform the host that the main script execution has completed,
					 * and also what the return value is */
					VARIANT varRes;

					VariantInit(&varRes);
					if (m_pass_eng)
						m_pass_eng->OnScriptTerminate(&varRes, NULL);

					/*
					   m_scriptstate = SCRIPTSTATE_INITIALIZED;
					   if (m_pass_eng)
					   m_pass_eng->OnStateChange(m_scriptstate);
					   */
				}
			}
			break;
		case PHPSE_INIT_NEW:
			{
				/* Prepare PHP/ZE for use */

				trace("%08x: m_frags : INIT NEW\n", this);
				zend_hash_init(&m_frags, 0, NULL, frag_dtor, TRUE);

				SG(options) |= SAPI_OPTION_NO_CHDIR;
				SG(server_context) = this;
				/* override the default PHP error callback */
				zend_error_cb = activescript_error_handler;

				zend_alter_ini_entry("register_argc_argv", 19, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
				zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
				zend_alter_ini_entry("implicit_flush", 15, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
				zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);

				php_request_startup(TSRMLS_C);
				PG(during_request_startup) = 0;
				trace("\n\n     *** ticks func at %08x %08x ***\n\n\n", activescript_run_ticks, &activescript_run_ticks);
//				php_add_tick_function(activescript_run_ticks);

			}
			break;
		case PHPSE_CLOSE:
			{
				/* Close things down */
				trace("%08x: m_frags : CLOSE/DESTROY\n", this);
				m_scriptstate = SCRIPTSTATE_CLOSED;
				if (m_pass_eng) {
					m_pass_eng->OnStateChange(m_scriptstate);
					trace("%08x: release site from this side\n", this);
					m_pass_eng->Release();
					m_pass_eng = NULL;
				}
				zend_hash_destroy(&m_frags);
				php_request_shutdown(NULL);

				break;
			}
			break;
		case PHPSE_CLONE:
			{
				/* Clone the engine state.  This is semantically equal to serializing all
				 * the parsed code from the source and unserializing it in the dest (this).
				 * IE doesn't appear to use it, but Windows Script Host does. I'd expect
				 * ASP/ASP.NET to do so also.
				 *
				 * FIXME: Probably won't work with IActiveScriptParseProcedure scriplets
				 * */

				TPHPScriptingEngine *src = (TPHPScriptingEngine*)lParam;

				trace("%08x: m_frags : CLONE\n", this);
				zend_hash_apply_with_argument(&src->m_frags, clone_frags, this TSRMLS_CC); 

			}
			break;
		case PHPSE_ADD_SCRIPTLET:
			{
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

				struct php_active_script_add_scriptlet_info *info = (struct php_active_script_add_scriptlet_info*)lParam;

				TWideString
					default_name(info->pstrDefaultName),
				code(info->pstrCode),
				item_name(info->pstrItemName),
				sub_item_name(info->pstrSubItemName),
				event_name(info->pstrEventName),
				delimiter(info->pstrDelimiter);

				/* lets invent a function name for the scriptlet */
				char sname[256];

				/* should check if the name is already used! */
				if (info->pstrDefaultName) 
					strcpy(sname, default_name.ansi_string());
				else {
					sname[0] = 0;
					strcat(sname, "__");
					if (info->pstrItemName) {
						strcat(sname, item_name.ansi_string());
						strcat(sname, "_");
					}
					if (info->pstrSubItemName) {
						strcat(sname, sub_item_name.ansi_string());
						strcat(sname, "_");
					}
					if (info->pstrEventName)
						strcat(sname, event_name.ansi_string());
				}


				trace("%08x: AddScriptlet:\n state=%s\n name=%s\n code=%s\n item=%s\n subitem=%s\n event=%s\n delim=%s\n line=%d\n",
						this, scriptstate_to_string(m_scriptstate),
						default_name.safe_ansi_string(), code.safe_ansi_string(), item_name.safe_ansi_string(),
						sub_item_name.safe_ansi_string(), event_name.safe_ansi_string(), delimiter.safe_ansi_string(),
						info->ulStartingLineNumber);


				code_frag *frag = compile_code_fragment(
						FRAG_SCRIPTLET,
						sname,
						info->pstrCode,
						info->ulStartingLineNumber,
						info->pexcepinfo,
						this
						TSRMLS_CC);

				if (frag) {

					frag->persistent = (info->dwFlags & SCRIPTTEXT_ISPERSISTENT);

					zend_hash_next_index_insert(&m_frags, &frag, sizeof(code_frag*), NULL);

					/*
					   ScriptProcedureDispatch *disp = new ScriptProcedureDispatch;

					   disp->AddRef();
					   disp->m_frag = frag;
					   disp->m_procflags = info->dwFlags;
					   disp->m_engine = this;
					   frag->ptr = disp;

					 *info->ppdisp = disp;
					 */
					ret = S_OK;
				} else {
					ret = DISP_E_EXCEPTION;
				}

				*info->pbstrName = TWideString::bstr_from_ansi(sname);

				trace("%08x: done with scriptlet %s\n", this, sname);

			}
			break;
		case PHPSE_GET_DISPATCH:
			{
				struct php_active_script_get_dispatch_info *info = (struct php_active_script_get_dispatch_info *)lParam;
				IDispatch *disp = NULL;

				if (info->pstrItemName != NULL) {
					zval **tmp;
					/* use this rather than php_OLECHAR_to_char because we want to avoid emalloc here */
					TWideString itemname(info->pstrItemName);
					
					/* Get that item from the global namespace.
					 * If it is an object, export it as a dispatchable object.
					 * */

					if (zend_hash_find(&EG(symbol_table), itemname.ansi_string(),
								itemname.ansi_len() + 1, (void**)&tmp) == SUCCESS) {
						if (Z_TYPE_PP(tmp) == IS_OBJECT) {
							/* FIXME: if this causes an allocation (emalloc) and we are
							 * not in the engine thread, things could get ugly!!! */
							disp = php_COM_export_object(*tmp TSRMLS_CC);
						}
					}

				} else {
					/* This object represents PHP global namespace */
					disp = (IDispatch*) new ScriptDispatch;
				}

				if (disp) {
					ret = GIT_put(disp, IID_IDispatch, &info->dispatch);
					disp->Release();
				} else {
					ret = S_FALSE;
				}
			}
			break;
		case PHPSE_ADD_NAMED_ITEM:
			{
				/* The Host uses this to add objects to the global namespace.
				 * Some objects are intended to have their child properties
				 * globally visible, so we add those to the global namespace too.
				 * */
				struct php_active_script_add_named_item_info *info = (struct php_active_script_add_named_item_info *)lParam;

				TWideString name(info->pstrName);
				IDispatch *disp;
				
				if (SUCCEEDED(GIT_get(info->marshal, IID_IDispatch, (void**)&disp))) 
					add_to_global_namespace(disp, info->dwFlags, name.ansi_string() TSRMLS_CC);

			}
			break;
		case PHPSE_SET_SITE:
			{
				if (m_pass_eng) {
					m_pass_eng->Release();
					m_pass_eng = NULL;
				}

				if (lParam)
					GIT_get(lParam, IID_IActiveScriptSite, (void**)&m_pass_eng);

				trace("%08x: site (engine-side) is now %08x (base=%08x)\n", this, m_pass_eng, m_pass);

			}
			break;
		case PHPSE_EXEC_PROC:
			{
				ScriptProcedureDispatch *disp = (ScriptProcedureDispatch *)lParam;
				execute_code_fragment(disp->m_frag, NULL, NULL TSRMLS_CC);
			}
			break;
		case PHPSE_PARSE_PROC:
			{
				/* This is the IActiveScriptParseProcedure implementation.
				 * IE uses this to request for an IDispatch that it will invoke in
				 * response to some event, and tells us the code that it wants to
				 * run.
				 * We compile the code and pass it back a dispatch object.
				 * The object will then serialize access to the engine thread and
				 * execute the opcodes */
				struct php_active_script_parse_proc_info *info = (struct php_active_script_parse_proc_info*)lParam;
				TWideString
					formal_params(info->pstrFormalParams),
					procedure_name(info->pstrProcedureName),
					item_name(info->pstrItemName),
					delimiter(info->pstrDelimiter);

				trace("%08x: ParseProc:\n state=%s\nparams=%s\nproc=%s\nitem=%s\n delim=%s\n line=%d\n",
						this, scriptstate_to_string(m_scriptstate),
						formal_params.ansi_string(), procedure_name.ansi_string(),
						item_name.safe_ansi_string(), delimiter.safe_ansi_string(),
						info->ulStartingLineNumber);


				code_frag *frag = compile_code_fragment(
						FRAG_PROCEDURE,
						NULL,
						info->pstrCode,
						info->ulStartingLineNumber,
						NULL,
						this
						TSRMLS_CC);

				if (frag) {

					frag->persistent = (info->dwFlags & SCRIPTTEXT_ISPERSISTENT);
					zend_hash_next_index_insert(&m_frags, &frag, sizeof(code_frag*), NULL);

					ScriptProcedureDispatch *disp = new ScriptProcedureDispatch;

					disp->m_frag = frag;
					disp->m_procflags = info->dwFlags;
					disp->m_engine = this;
					frag->ptr = disp;
					info->dispcookie = disp->m_gitcookie;

				} else {
					ret = DISP_E_EXCEPTION;
				}

			}
			break;

		case PHPSE_PARSE_SCRIPT:
			{
				struct php_active_script_parse_info *info = (struct php_active_script_parse_info*)lParam;
				int doexec;

				TWideString
					code(info->pstrCode),
				item_name(info->pstrItemName),
				delimiter(info->pstrDelimiter);

				trace("%08x: ParseScriptText:\n state=%s\ncode=%s\n item=%s\n delim=%s\n line=%d\n",
						this, scriptstate_to_string(m_scriptstate),
						code.safe_ansi_string(), item_name.safe_ansi_string(), delimiter.safe_ansi_string(),
						info->ulStartingLineNumber);

				code_frag *frag = compile_code_fragment(
						FRAG_MAIN,
						info->dwFlags & SCRIPTTEXT_ISEXPRESSION ? FRAG_CREATE_FUNC : NULL,
						info->pstrCode,
						info->ulStartingLineNumber,
						info->pexcepinfo,
						this
						TSRMLS_CC);

				doexec = (info->dwFlags & SCRIPTTEXT_ISEXPRESSION) ||
					m_scriptstate == SCRIPTSTATE_STARTED ||
					m_scriptstate == SCRIPTSTATE_CONNECTED ||
					m_scriptstate == SCRIPTSTATE_DISCONNECTED;

				if (frag) {
					frag->persistent = (info->dwFlags & SCRIPTTEXT_ISPERSISTENT);
					ret = S_OK;

					if (info->dwFlags & SCRIPTTEXT_ISEXPRESSION) {
						if (m_scriptstate == SCRIPTSTATE_INITIALIZED) {
							/* not allowed to execute code in this state */
							ret = E_UNEXPECTED;
							doexec = 0;
						}
					}

					if (doexec) {
						/* execute the code as an expression */
						if (!execute_code_fragment(frag, info->pvarResult, info->pexcepinfo TSRMLS_CC))
							ret = DISP_E_EXCEPTION;
					}

					zend_hash_next_index_insert(&m_frags, &frag, sizeof(code_frag*), NULL);
				} else {
					ret = DISP_E_EXCEPTION;
				}

				if (info->pvarResult) {
					VariantInit(info->pvarResult);
				}


			}
			break;
		default:
			trace("unhandled message type %08x\n", msg);
			if (handled)
				*handled = 0;
	}
	return ret;
}

/* The PHP/Zend state actually lives in this thread */
void TPHPScriptingEngine::engine_thread_func(void)
{
	TSRMLS_FETCH();
	int handled;
	int terminated = 0;
	MSG msg;

	trace("%08x: engine thread started up!\n", this);

	CoInitializeEx(0, COINIT_MULTITHREADED);
	
	m_tsrm_hack = tsrm_ls;

	while(!terminated) {
		DWORD result = MsgWaitForMultipleObjects(0, NULL, FALSE, 4000, QS_ALLINPUT);

		switch(result) {
			case WAIT_OBJECT_0:
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

					if (msg.message == WM_QUIT) {
						terminated = 1;
					} else if (msg.hwnd) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);

					} else {
						handled = 1;
						m_sync_thread_ret = engine_thread_handler(msg.message, msg.wParam, msg.lParam, &handled TSRMLS_CC);
						if (handled)
							SetEvent(m_sync_thread_msg);
					}

				}
				break;
			case WAIT_TIMEOUT:
				trace("thread wait timed out\n");
				break;
			default:
				trace("some strange value\n");
		}
	}
	
#if 0
	while(GetMessage(&msg, NULL, 0, 0)) {

		if (msg.message == WM_QUIT)
			break;
		
		handled = 1;
		m_sync_thread_ret = engine_thread_handler(msg.message, msg.wParam, msg.lParam, &handled TSRMLS_CC);
		if (handled)
			SetEvent(m_sync_thread_msg);
	}
	trace("%08x: engine thread exiting!!!!!\n", this);
#endif
	m_enginethread = 0;

	CoUninitialize();
}

/* Only call this in the context of the engine thread, or you'll be sorry.
 *
 * When SCRIPTITEM_GLOBALMEMBERS is set, we're only adding COM objects to the namespace.
 * We could add *all* properties, but I don't like this idea; what if the value changes
 * while the page is running?  We'd be left with stale data.
 * */
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
	
	val = php_COM_object_from_dispatch(disp, NULL TSRMLS_CC);
	
	if (val == NULL) {
		disp->Release();
		return;
	}

	ZEND_SET_SYMBOL(&EG(symbol_table), name, val);

	if (flags & SCRIPTITEM_GLOBALMEMBERS == 0) {
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

			/* Look at it's type */
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

							/* Get it's dispatch */
							IDispatch *sub = NULL;

							if (V_VT(&vres) == VT_UNKNOWN)
								V_UNKNOWN(&vres)->QueryInterface(IID_IDispatch, (void**)&sub);
							else if (V_VT(&vres) == VT_DISPATCH)
								sub = V_DISPATCH(&vres);

							if (sub) {
								/* find out it's name */
								typ->GetDocumentation(func->memid, &funcname, NULL, NULL, NULL);
								name = php_OLECHAR_to_char(funcname, &namelen, CP_ACP TSRMLS_CC);

								/* add to namespace */
								zval *subval = php_COM_object_from_dispatch(sub, NULL TSRMLS_CC);
								if (subval) {
									ZEND_SET_SYMBOL(&EG(symbol_table), name, subval);	
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
	return InterlockedIncrement(const_cast<long*> (&m_refcount));
}

STDMETHODIMP_(DWORD) TPHPScriptingEngine::Release(void)
{
	DWORD ret = InterlockedDecrement(const_cast<long*> (&m_refcount));
	if (ret == 0) {
		trace("%08x: Release: zero refcount, destroy the engine!\n", this);
		delete this;
	}
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
	} else if (IsEqualGUID(IID_IUnknown, iid)) {
		*ppvObject = this;	
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
	TSRMLS_FETCH();

	tsrm_mutex_lock(m_mutex);

	trace("%08x: -----> Base thread is %08x\n", this, tsrm_thread_id());
	
	if (m_pass) {
		m_pass->Release();
		m_pass = NULL;
		SEND_THREAD_MESSAGE(this, PHPSE_SET_SITE, 0, 0 TSRMLS_CC);
	}

	if (pass == NULL) {
		trace("Closing down site; we should have no references to objects from the host\n"
				"  m_pass=%08x\n  m_pass_eng=%08x\n   What about named items??\n",
				m_pass, m_pass_eng);
	}
	
	m_pass = pass;
	if (m_pass) {
		m_pass->AddRef();

		DWORD cookie;
		if (SUCCEEDED(GIT_put(m_pass, IID_IActiveScriptSite, &cookie)))
			SEND_THREAD_MESSAGE(this, PHPSE_SET_SITE, 0, cookie TSRMLS_CC);
		
		if (m_scriptstate == SCRIPTSTATE_UNINITIALIZED)
			SEND_THREAD_MESSAGE(this, PHPSE_STATE_CHANGE, 0, SCRIPTSTATE_INITIALIZED TSRMLS_CC);
	}

	tsrm_mutex_unlock(m_mutex);
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::GetScriptSite(REFIID riid, void **ppvObject)
{
	HRESULT ret = S_FALSE;
	
	trace("%08x: GetScriptSite()\n", this);
	tsrm_mutex_lock(m_mutex);


	if (m_pass)
		ret = m_pass->QueryInterface(riid, ppvObject);

	tsrm_mutex_unlock(m_mutex);
	return ret;
}

STDMETHODIMP TPHPScriptingEngine::SetScriptState(SCRIPTSTATE ss)
{
	TSRMLS_FETCH();
	trace("%08x: SetScriptState(%s)\n", this, scriptstate_to_string(ss));
	return SEND_THREAD_MESSAGE(this, PHPSE_STATE_CHANGE, 0, ss TSRMLS_CC);
}

STDMETHODIMP TPHPScriptingEngine::GetScriptState(SCRIPTSTATE *pssState)
{
	trace("%08x: GetScriptState(current=%s)\n", this, scriptstate_to_string(m_scriptstate));
	tsrm_mutex_lock(m_mutex);
	*pssState = m_scriptstate;
	tsrm_mutex_unlock(m_mutex);
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::Close(void)
{
	TSRMLS_FETCH();

	if (m_pass) {
		m_pass->Release();
		m_pass = NULL;
	}
	SEND_THREAD_MESSAGE(this, PHPSE_CLOSE, 0, 0 TSRMLS_CC);
	return S_OK;
}

/* Add an item to global namespace.
 * This is called in the context of the base thread (or perhaps some other thread).
 * We want to be able to work with the object in the engine thread, so we marshal
 * it into a stream and let the engine thread deal with it.
 * This works quite nicely when PHP scripts call into the object; threading is
 * handled correctly. */
STDMETHODIMP TPHPScriptingEngine::AddNamedItem(LPCOLESTR pstrName, DWORD dwFlags)
{
	struct php_active_script_add_named_item_info info;
	TSRMLS_FETCH();

	info.pstrName = pstrName;
	info.dwFlags = dwFlags;

	m_pass->GetItemInfo(pstrName, SCRIPTINFO_IUNKNOWN, &info.punk, NULL);
	if (SUCCEEDED(GIT_put(info.punk, IID_IDispatch, &info.marshal))) {
		SEND_THREAD_MESSAGE(this, PHPSE_ADD_NAMED_ITEM, 0, (LPARAM)&info TSRMLS_CC);
	}
	info.punk->Release();
	
	return S_OK;
}

/* Bind to a type library */
STDMETHODIMP TPHPScriptingEngine::AddTypeLib( 
	/* [in] */ REFGUID rguidTypeLib,
	/* [in] */ DWORD dwMajor,
	/* [in] */ DWORD dwMinor,
	/* [in] */ DWORD dwFlags)
{
	struct php_active_script_add_tlb_info info;
	TSRMLS_FETCH();

	info.rguidTypeLib = &rguidTypeLib;
	info.dwMajor = dwMajor;
	info.dwMinor = dwMinor;
	info.dwFlags = dwFlags;

	SEND_THREAD_MESSAGE(this, PHPSE_ADD_TYPELIB, 0, (LPARAM)&info TSRMLS_CC);

	return S_OK;
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
	TSRMLS_FETCH();
	*ppdisp = NULL;
	struct php_active_script_get_dispatch_info info;

	info.pstrItemName = pstrItemName;
	info.dispatch = NULL;
	
	/* This hack is required because the host is likely to query us
	 * for a dispatch if we use any of it's objects from PHP script.
	 * Since the engine thread will be waiting for the return from
	 * a COM call, we need to deliberately poke a hole in thread
	 * safety so that it is possible to read the symbol table from
	 * outside the engine thread and give it a valid return value.
	 * This is "safe" only in this instance, since we are not modifying
	 * the engine state by looking up the dispatch (I hope).
	 * The scripting engine rules pretty much guarantee that this
	 * method is only called in the base thread. */

	if (tsrm_thread_id() != m_enginethread) {
		tsrm_ls = m_tsrm_hack;
		trace("HEY: hacking thread safety!\n");
	}

	if (S_OK == engine_thread_handler(PHPSE_GET_DISPATCH, 0, (LPARAM)&info, NULL TSRMLS_CC)) {
		GIT_get(info.dispatch, IID_IDispatch, (void**)ppdisp);
	}
	
	if (*ppdisp) {
		return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP TPHPScriptingEngine::GetCurrentScriptThreadID( 
	/* [out] */ SCRIPTTHREADID *pstidThread)
{
//	tsrm_mutex_lock(m_mutex);
	trace("%08x: GetCurrentScriptThreadID()\n", this);
	*pstidThread = m_enginethread;
//	tsrm_mutex_unlock(m_mutex);
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::GetScriptThreadID( 
	/* [in] */ DWORD dwWin32ThreadId,
	/* [out] */ SCRIPTTHREADID *pstidThread)
{
//	tsrm_mutex_lock(m_mutex);
	trace("%08x: GetScriptThreadID()\n", this);
	*pstidThread = dwWin32ThreadId;
//	tsrm_mutex_unlock(m_mutex);
	return S_OK;
}

STDMETHODIMP TPHPScriptingEngine::GetScriptThreadState( 
	/* [in] */ SCRIPTTHREADID stidThread,
	/* [out] */ SCRIPTTHREADSTATE *pstsState)
{
//	tsrm_mutex_lock(m_mutex);

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
//	tsrm_mutex_unlock(m_mutex);
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
	TPHPScriptingEngine *cloned = new TPHPScriptingEngine;
	TSRMLS_FETCH();
	
	trace("%08x: Clone()\n", this);

	if (ppscript)
		*ppscript = NULL;

	if (cloned) {
		cloned->InitNew();
		SEND_THREAD_MESSAGE(cloned, PHPSE_CLONE, 0, (LPARAM)this TSRMLS_CC);
		trace("%08x: Cloned OK, returning cloned object ptr %08x\n", this, cloned);	
		*ppscript = (IActiveScript*)cloned;
		return S_OK;

	}
		
	return E_FAIL;
}


STDMETHODIMP TPHPScriptingEngine::InitNew( void)
{
	TSRMLS_FETCH();
	SEND_THREAD_MESSAGE(this, PHPSE_INIT_NEW, 0, 0 TSRMLS_CC);
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
	struct php_active_script_add_scriptlet_info info;
	TSRMLS_FETCH();

	info.pstrDefaultName = pstrDefaultName;
	info.pstrCode = pstrCode;
	info.pstrItemName = pstrItemName;
	info.pstrSubItemName = pstrSubItemName;
	info.pstrEventName = pstrEventName;
	info.pstrDelimiter = pstrDelimiter;
	info.dwSourceContextCookie = dwSourceContextCookie;
	info.ulStartingLineNumber = ulStartingLineNumber;
	info.dwFlags = dwFlags;
	info.pbstrName = pbstrName;
	info.pexcepinfo = pexcepinfo;

	return SEND_THREAD_MESSAGE(this, PHPSE_ADD_SCRIPTLET, 0, (LPARAM)&info TSRMLS_CC);
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
	struct php_active_script_parse_info info;
	TSRMLS_FETCH();

	info.pstrCode = pstrCode;
	info.pstrItemName = pstrItemName;
	info.punkContext = punkContext;
	info.pstrDelimiter = pstrDelimiter;
	info.dwSourceContextCookie = dwSourceContextCookie;
	info.ulStartingLineNumber = ulStartingLineNumber;
	info.dwFlags = dwFlags;
	info.pvarResult = pvarResult;
	info.pexcepinfo = pexcepinfo;

	return SEND_THREAD_MESSAGE(this, PHPSE_PARSE_SCRIPT, 0, (LPARAM)&info TSRMLS_CC);
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
	struct php_active_script_parse_proc_info info;
	HRESULT ret;
	TSRMLS_FETCH();

	info.pstrCode = pstrCode;
	info.pstrFormalParams = pstrFormalParams;
	info.pstrProcedureName = pstrProcedureName;
	info.pstrItemName = pstrItemName;
	info.punkContext = punkContext;
	info.pstrDelimiter = pstrDelimiter;
	info.dwSourceContextCookie = dwSourceContextCookie;
	info.ulStartingLineNumber = ulStartingLineNumber;
	info.dwFlags = dwFlags;

	ret = SEND_THREAD_MESSAGE(this, PHPSE_PARSE_PROC, 0, (LPARAM)&info TSRMLS_CC);

	if (ret == S_OK)
		ret = GIT_get(info.dispcookie, IID_IDispatch, (void**)ppdisp);
	
	trace("ParseProc: ret=%08x disp=%08x\n", ret, *ppdisp);
	return ret;
}

extern "C" 
void activescript_error_func(int type, const char *error_msg, ...)
{
	TSRMLS_FETCH();

	TPHPScriptingEngine *engine = (TPHPScriptingEngine*)SG(server_context);

	
}

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
		*pdwSourceContext = 0;
		*pulLineNumber = m_lineno;
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
	
	buflen = vspprintf(&buf, PG(log_errors_max_len), format, args);
	trace("%08x: Error: %s\n", engine, buf);

	/* if it's a fatal error, report it using IActiveScriptError. */
	
	switch(type) {
		case E_ERROR:
		case E_CORE_ERROR:
		case E_COMPILE_ERROR:
		case E_USER_ERROR:
		case E_PARSE:
			/* trigger an error in the host */
			TActiveScriptError *eobj = new TActiveScriptError(error_filename, error_lineno, buf);
			trace("raising error object!\n");
			if (engine->m_pass_eng)
				engine->m_pass_eng->OnScriptError(eobj);

			/* now throw the exception to abort execution */
			if (engine->m_err_trap)
				longjmp(*engine->m_err_trap, 1);
			
			break;
	}
	efree(buf);
}

