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
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <activscp.h>
#include "zend.h"
#include <setjmp.h>

/* Definitions for thread messages */
enum {
	PHPSE_STATE_CHANGE = WM_USER + 20,
	PHPSE_INIT_NEW,
	PHPSE_PARSE_SCRIPT,
	PHPSE_ADD_SCRIPTLET,
	PHPSE_CLOSE,
	PHPSE_CLONE,
	PHPSE_ENTER,
	PHPSE_LEAVE,
	PHPSE_TERMINATE,
	PHPSE_PARSE_PROC,
	PHPSE_EXEC_PROC,
	PHPSE_ADD_NAMED_ITEM,
	PHPSE_SET_SITE,
	PHPSE_ADD_TYPELIB,
	PHPSE_TRIGGER_ERROR,
	PHPSE_GET_DISPATCH,
	PHPSE_DUMMY_TICK,
};

struct php_active_script_get_dispatch_info {
	LPCOLESTR pstrItemName;
	DWORD dispatch;
};

struct php_active_script_add_named_item_info {
	LPCOLESTR pstrName;
	DWORD dwFlags;
	IUnknown *punk;
	ITypeInfo *ptyp;
	IDispatch *pdisp;
	DWORD marshal;
};

struct php_active_script_add_scriptlet_info {
	/* [in] */ LPCOLESTR pstrDefaultName;
	/* [in] */ LPCOLESTR pstrCode;
	/* [in] */ LPCOLESTR pstrItemName;
	/* [in] */ LPCOLESTR pstrSubItemName;
	/* [in] */ LPCOLESTR pstrEventName;
	/* [in] */ LPCOLESTR pstrDelimiter;
	/* [in] */ DWORD dwSourceContextCookie;
	/* [in] */ ULONG ulStartingLineNumber;
	/* [in] */ DWORD dwFlags;
	/* [out] */ BSTR *pbstrName;
	/* [out] */ EXCEPINFO *pexcepinfo;
};

struct php_active_script_parse_info {
	/* [in] */ LPCOLESTR pstrCode;
	/* [in] */ LPCOLESTR pstrItemName;
	/* [in] */ IUnknown *punkContext;
	/* [in] */ LPCOLESTR pstrDelimiter;
	/* [in] */ DWORD dwSourceContextCookie;
	/* [in] */ ULONG ulStartingLineNumber;
	/* [in] */ DWORD dwFlags;
	/* [out] */ VARIANT *pvarResult;
	/* [out] */ EXCEPINFO *pexcepinfo;
};

struct php_active_script_parse_proc_info {
	/* [in] */ LPCOLESTR pstrCode;
	/* [in] */ LPCOLESTR pstrFormalParams;
	/* [in] */ LPCOLESTR pstrProcedureName;
	/* [in] */ LPCOLESTR pstrItemName;
	/* [in] */ IUnknown *punkContext;
	/* [in] */ LPCOLESTR pstrDelimiter;
	/* [in] */ DWORD dwSourceContextCookie;
	/* [in] */ ULONG ulStartingLineNumber;
	/* [in] */ DWORD dwFlags;
	DWORD dispcookie;
};

struct php_active_script_add_tlb_info {
	/* [in] */ const GUID * rguidTypeLib;
	/* [in] */ DWORD dwMajor;
	/* [in] */ DWORD dwMinor;
	/* [in] */ DWORD dwFlags;
};

class TPHPScriptingEngine:
	public IActiveScript,
	public IActiveScriptParse,
	public IActiveScriptParseProcedure
{
public:
	volatile LONG m_refcount;
	IActiveScriptSite *m_pass;
	SCRIPTSTATE m_scriptstate;
	MUTEX_T		m_mutex;	
	HashTable	m_script_dispatchers;
	HANDLE 		m_engine_thread_handle;

	HANDLE		m_sync_thread_msg;
	HRESULT		m_sync_thread_ret;

	/* This is hacky, but only used when the host queries us for a script dispatch */
	void *** m_tsrm_hack;
	
	void add_to_global_namespace(IDispatch *disp, DWORD flags, char *name TSRMLS_DC);
	
	THREAD_T	m_enginethread, m_basethread;
	HashTable   m_frags;
	ULONG		m_lambda_count;
	IActiveScriptSite *m_pass_eng;

	jmp_buf *m_err_trap;
	int m_in_main, m_stop_main;
		
	HRESULT SendThreadMessage(LONG msg, WPARAM wparam, LPARAM lparam);
	
	void engine_thread_func(void);
	HRESULT engine_thread_handler(LONG msg, WPARAM wParam, LPARAM lParam, int *handled TSRMLS_DC);
	
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
public:
	TPHPScriptingEngine();
	~TPHPScriptingEngine();

};

