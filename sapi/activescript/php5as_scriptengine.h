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

#include <activscp.h>
#include <objsafe.h>
#include "zend.h"

#if 0
#define ACTIVEPHP_THREADING_MODE	COINIT_MULTITHREADED
#else
#define ACTIVEPHP_THREADING_MODE	COINIT_APARTMENTTHREADED
#endif

#define ACTIVEPHP_HAS_OWN_THREAD	1

#define WM_ACTIVEPHP_SERIALIZE	WM_USER + 200

enum activephp_engine_func { /* if you change the order, change marshal.cpp too */
	APHP_ParseScriptText,
	APHP_InitNew,
	APHP_AddNamedItem,
	APHP_SetScriptState,
	APHP_GetScriptDispatch,
	APHP_Close,
	APHP_AddTypeLib,
	APHP_AddScriptlet,
	APHP__Max
};

HRESULT marshal_call(class TPHPScriptingEngine *engine, enum activephp_engine_func func, int nargs, ...);
HRESULT marshal_stub(LPARAM lparam);

class TPHPScriptingEngine:
	public IActiveScript,
	public IActiveScriptParse,
	public IActiveScriptParseProcedure,
	public IObjectSafety,
	public IDispatch
#if 0
	, public IMarshal
#endif
{
public:
	volatile LONG m_refcount;
	IActiveScriptSite *m_pass;
	SCRIPTSTATE m_scriptstate;

	void add_to_global_namespace(IDispatch *disp, DWORD flags, char *name TSRMLS_DC);
	
	THREAD_T	m_enginethread, m_basethread;
	HashTable   m_frags;
	ULONG		m_lambda_count;
	DWORD		m_gitcookie, m_asscookie;
	HWND		m_queue;

	int m_done_init;

	int m_in_main, m_stop_main;
	
	void do_clone(TPHPScriptingEngine *src);
void setup_engine_state(void);
	int create_id(OLECHAR *name, DISPID *dispid TSRMLS_DC);

	char *m_names[1024];
	int m_lens[1024];
	int m_ids;
	
public: /* IUnknown */
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(DWORD) AddRef(void);
	STDMETHODIMP_(DWORD) Release(void);
public: /* IActiveScript */
	STDMETHODIMP SetScriptSite( 
		/* [in] */ IActiveScriptSite *pass);
	
	STDMETHODIMP GetScriptSite( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject);
	
	STDMETHODIMP SetScriptState( 
		/* [in] */ SCRIPTSTATE ss);
	
	STDMETHODIMP GetScriptState( 
		/* [out] */ SCRIPTSTATE *pssState);
	
	STDMETHODIMP Close( void);
	
	STDMETHODIMP AddNamedItem( 
		/* [in] */ LPCOLESTR pstrName,
		/* [in] */ DWORD dwFlags);
	
	STDMETHODIMP AddTypeLib( 
		/* [in] */ REFGUID rguidTypeLib,
		/* [in] */ DWORD dwMajor,
		/* [in] */ DWORD dwMinor,
		/* [in] */ DWORD dwFlags);
	
	STDMETHODIMP GetScriptDispatch( 
		/* [in] */ LPCOLESTR pstrItemName,
		/* [out] */ IDispatch **ppdisp);
	
	STDMETHODIMP GetCurrentScriptThreadID( 
		/* [out] */ SCRIPTTHREADID *pstidThread);
	
	STDMETHODIMP GetScriptThreadID( 
		/* [in] */ DWORD dwWin32ThreadId,
		/* [out] */ SCRIPTTHREADID *pstidThread);
	
	STDMETHODIMP GetScriptThreadState( 
		/* [in] */ SCRIPTTHREADID stidThread,
		/* [out] */ SCRIPTTHREADSTATE *pstsState);
	
	STDMETHODIMP InterruptScriptThread( 
		/* [in] */ SCRIPTTHREADID stidThread,
		/* [in] */ const EXCEPINFO *pexcepinfo,
		/* [in] */ DWORD dwFlags);
	
	STDMETHODIMP Clone( 
		/* [out] */ IActiveScript **ppscript);
	
public: /* IActiveScriptParse */
	STDMETHODIMP InitNew( void);
	
	STDMETHODIMP AddScriptlet( 
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
		/* [out] */ EXCEPINFO *pexcepinfo);
	
	STDMETHODIMP ParseScriptText( 
		/* [in] */ LPCOLESTR pstrCode,
		/* [in] */ LPCOLESTR pstrItemName,
		/* [in] */ IUnknown *punkContext,
		/* [in] */ LPCOLESTR pstrDelimiter,
		/* [in] */ DWORD dwSourceContextCookie,
		/* [in] */ ULONG ulStartingLineNumber,
		/* [in] */ DWORD dwFlags,
		/* [out] */ VARIANT *pvarResult,
		/* [out] */ EXCEPINFO *pexcepinfo);
public: /* IActiveScriptParseProcedure */
	STDMETHODIMP ParseProcedureText( 
		/* [in] */ LPCOLESTR pstrCode,
	   	/* [in] */ LPCOLESTR pstrFormalParams,
   		/* [in] */ LPCOLESTR pstrProcedureName,
		/* [in] */ LPCOLESTR pstrItemName,
		/* [in] */ IUnknown *punkContext,
		/* [in] */ LPCOLESTR pstrDelimiter,
		/* [in] */ DWORD dwSourceContextCookie,
		/* [in] */ ULONG ulStartingLineNumber,
		/* [in] */ DWORD dwFlags,
		/* [out] */ IDispatch **ppdisp);

public: /* IObjectSafety */
	STDMETHODIMP GetInterfaceSafetyOptions(
		/* [in]  */ REFIID	riid,						// Interface that we want options for
		/* [out] */ DWORD	*pdwSupportedOptions,		// Options meaningful on this interface
		/* [out] */ DWORD	*pdwEnabledOptions);		// current option values on this interface

	STDMETHODIMP SetInterfaceSafetyOptions(
		/* [in]  */ REFIID		riid,					// Interface to set options for
		/* [in]  */ DWORD		dwOptionSetMask,		// Options to change
		/* [in]  */ DWORD		dwEnabledOptions);		// New option values
#if 0
public: /* IMarshal */
	STDMETHODIMP GetUnmarshalClass(
			REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, CLSID *pCid);
	STDMETHODIMP GetMarshalSizeMax(
			REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, ULONG *pSize);
	STDMETHODIMP MarshalInterface(
			IStream *pStm, REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshflags);
	STDMETHODIMP UnmarshalInterface(
			IStream *pStm, REFIID riid, void **ppv);
	STDMETHODIMP ReleaseMarshalData(IStream *pStm);
	STDMETHODIMP DisconnectObject(DWORD dwReserved);
#endif

public:	/* IDispatch */
	STDMETHODIMP GetIDsOfNames( REFIID  riid, OLECHAR **rgszNames, unsigned int cNames, LCID lcid, DISPID *rgDispId);
	STDMETHODIMP Invoke( DISPID  dispIdMember, REFIID  riid, LCID  lcid, WORD  wFlags,
		DISPPARAMS FAR*  pdp, VARIANT FAR*  pvarRes, EXCEPINFO FAR*  pei,
		unsigned int FAR*  puArgErr);
	STDMETHODIMP GetTypeInfoCount(unsigned int *  pctinfo);
	STDMETHODIMP GetTypeInfo( unsigned int iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	
public:
	TPHPScriptingEngine();
	~TPHPScriptingEngine();
	
};

IUnknown *create_scripting_engine(TPHPScriptingEngine *tobecloned);

