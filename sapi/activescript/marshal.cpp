/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2004 The PHP Group                                     |
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

/* Fun with threads */

#define _WIN32_DCOM
#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS
#include <winsock2.h>
#include "php5as_scriptengine.h"
#include "php5as_classfactory.h"
#include <objbase.h>
#undef php_win_err

extern "C" char *php_win_err(HRESULT ret);

#define APHPM_IN	1
#define APHPM_OUT	2

#define APHPT_TERM	0
#define APHPT_UNK		1	/* IUnknown *		*/
#define APHPT_DISP	2	/* IDispatch *		*/
#define APHPT_VAR		3	/* PVARIANT 		*/

static inline void trace(char *fmt, ...)
{
	va_list ap;
	char buf[4096];

	sprintf(buf, "T=%08x [MARSHAL] ", tsrm_thread_id());
	OutputDebugString(buf);
	
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);

	OutputDebugString(buf);
	
	va_end(ap);
}
struct marshal_arg {
	int type;
	int argno;
	int direction;
};

static int parse_script_text_mdef[] = {
	APHPT_UNK, 2, APHPM_IN,
	APHPT_VAR, 7, APHPM_OUT,
	APHPT_TERM
};

static int get_script_dispatch_mdef[] = {
	APHPT_DISP, 1, APHPM_OUT,
	APHPT_TERM
};

static int *mdef_by_func[APHP__Max] = {
	parse_script_text_mdef,
	NULL, /* InitNew */
	NULL,	/* AddNamedItem */
	NULL,	/* SetScriptState */
	get_script_dispatch_mdef,
	NULL,	/* Close */
	NULL,	/* AddTypeLib */
	NULL,	/* AddScriptlet */
};

static HRESULT do_marshal_in(int stub, void *args[16], int *mdef, LPSTREAM *ppstm)
{
	int i = 0;
	int want;
	HRESULT ret = S_OK;
	LPSTREAM stm = NULL;

	if (!mdef)
		return S_OK;

	trace("marshalling ... \n");
	
	ret = CreateStreamOnHGlobal(NULL, TRUE, &stm);
	if (FAILED(ret)) {
		trace(" failed to create stm %s", php_win_err(ret));
		return ret;
	}
		
	*ppstm = stm;

	/* if stub is true, we are the stub and are marshaling OUT params,
	 * otherwise, we are the proxy and are marshalling IN params */

	if (stub) {
		want = APHPM_OUT;
	} else {
		want = APHPM_IN;
	}
	
	while (mdef[i] != APHPT_TERM) {
		if ((mdef[i+2] & want) == want) {
			int argno = mdef[i+1];
			int isout = (mdef[i+2] & APHPM_OUT) == APHPM_OUT;

#undef OUT_IFACE
#define OUT_IFACE		(isout ? *(IUnknown**)args[argno] : (IUnknown*)args[argno])
#define IFACE_PRESENT	args[argno] && (!isout || *(IUnknown**)args[argno])
			switch (mdef[i]) {
				case APHPT_UNK:
					if (IFACE_PRESENT) {
						ret = CoMarshalInterface(stm, IID_IUnknown, OUT_IFACE, MSHCTX_INPROC, NULL, MSHLFLAGS_NORMAL);
						trace("   arg=%d IUnknown --> %s", argno, php_win_err(ret));
					} else {
						trace("   arg=%d IUnknown(NULL) - skip\n", argno);
					}
					break;

				case APHPT_DISP:
					if (IFACE_PRESENT) {
						ret = CoMarshalInterface(stm, IID_IDispatch, OUT_IFACE, MSHCTX_INPROC, NULL, MSHLFLAGS_NORMAL);
						trace("   arg=%d IDispatch --> %s", argno, php_win_err(ret));
					} else {
						trace("   arg=%d IDispatch(NULL) - skip\n", argno);
					}
					break;

				case APHPT_VAR:
					if (args[argno])
						ret = E_NOTIMPL;
					break;

				default:
					ret = E_NOTIMPL;
			}

			if (FAILED(ret))
				break;
		} else {
			trace(" -- skipping (this param is not needed in this direction)\n");
		}
		i += 3;
	}

	if (FAILED(ret)) {
		/* TODO: rollback (refcounts are held during marshalling) */
		trace(" rolling back\n");
		stm->Release();
		*ppstm = NULL;
	} else {
		LARGE_INTEGER pos = {0};
		stm->Seek(pos, STREAM_SEEK_SET, NULL);
	}
	
	return ret;
}

static HRESULT do_marshal_out(int stub, void *args[16], int *mdef, LPSTREAM stm)
{
	int i = 0;
	int want;
	HRESULT ret = S_OK;

	if (!mdef)
		return S_OK;

	trace(" unmarshalling...\n");
	
	/* if stub is true, we are the stub and are unmarshaling IN params,
	 * otherwise, we are the proxy and are unmarshalling OUT params */

	if (!stub) {
		want = APHPM_OUT;
	} else {
		want = APHPM_IN;
	}
	
	while (mdef[i] != APHPT_TERM) {
		if ((mdef[i+2] & want) == want) {
			int argno = mdef[i+1];
			int isout = (mdef[i+2] & APHPM_OUT) == APHPM_OUT;
#undef OUT_IFACE
#define OUT_IFACE		(isout ? (void**)args[argno] : &args[argno])

			switch (mdef[i]) {
				case APHPT_UNK:
					if (IFACE_PRESENT) {
						ret = CoUnmarshalInterface(stm, IID_IUnknown, OUT_IFACE);
						trace("   unmarshal arg=%d IUnknown --> %s", argno, php_win_err(ret));
					} else {
						trace("   unmarshal arg=%d IUnknown(NULL) - skip\n", argno);
					}
					break;

				case APHPT_DISP:
					if (IFACE_PRESENT) {
						trace("   unmarshal dispatch: args[%d]=%p   *args[%d]=%p\n",
								argno, args[argno], argno, args[argno] ? *(void**)args[argno] : NULL);
						ret = CoUnmarshalInterface(stm, IID_IDispatch, OUT_IFACE);
						trace("   unmarshal arg=%d IDispatch --> %s:  args[%d]=%p *args[%d]=%p\n", argno, php_win_err(ret),
								argno, args[argno], argno, args[argno] ? *(void**)args[argno] : NULL);
					} else {
						trace("   unmarshal arg=%d IDispatch(NULL) - skip\n", argno);
					}
					break;

				case APHPT_VAR:
					if (args[argno])
						ret = E_NOTIMPL;
					break;

				default:
					ret = E_NOTIMPL;
			}
			if (FAILED(ret))
				break;
		}
		i += 3;
	}

	return ret;
}


struct activephp_serialize_msg {
	class TPHPScriptingEngine *engine;
	void *args[16];
	int nargs;
	enum activephp_engine_func func;
	int *marshal_defs;
	LPSTREAM instm, outstm;
	
	HANDLE evt;
	HRESULT ret;
};

static const char *func_names[APHP__Max] = {
	"ParseScriptText",
	"InitNew",
	"AddnamedItem",
	"SetScriptState",
	"GetScriptDispatch",
	"Close",
	"AddTypeLib",
	"AddScriptlet",
};

HRESULT marshal_call(class TPHPScriptingEngine *engine, enum activephp_engine_func func, int nargs, ...)
{
	va_list ap;
	struct activephp_serialize_msg msg ;
	HRESULT ret;

	memset(&msg, 0, sizeof(msg));
	
	msg.engine = engine;
	msg.func = func;
	msg.marshal_defs = mdef_by_func[func];

	trace(" prepping for function code %d %s, %d args, marshal defs at %p\n", func, func_names[func], nargs, msg.marshal_defs);

	va_start(ap, nargs);
	for (msg.nargs = 0; msg.nargs < nargs; msg.nargs++) {
		msg.args[msg.nargs] = va_arg(ap, void*);
	}
	va_end(ap);

	ret = do_marshal_in(0, msg.args, msg.marshal_defs, &msg.instm);

	if (FAILED(ret)) {
		return ret;
	}
	
#if 1
	msg.evt = CreateEvent(NULL, TRUE, FALSE, NULL);
	PostMessage(engine->m_queue, WM_ACTIVEPHP_SERIALIZE, 0, (LPARAM)&msg);

	while (WAIT_OBJECT_0 != WaitForSingleObject(msg.evt, 0)) {
		DWORD status = MsgWaitForMultipleObjects(1, &msg.evt, FALSE, INFINITE, QS_ALLEVENTS|QS_ALLINPUT|QS_ALLPOSTMESSAGE|QS_SENDMESSAGE|QS_POSTMESSAGE);

		if (status == WAIT_OBJECT_0)
			break;

		MSG msg;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	CloseHandle(msg.evt);
#else
	ret = SendMessage(engine->m_queue, WM_ACTIVEPHP_SERIALIZE, 0, (LPARAM)&msg);
#endif
	
	if (msg.outstm) {
		ret = do_marshal_out(0, msg.args, msg.marshal_defs, msg.outstm);
		msg.outstm->Release();
	}

	if (msg.instm)
		msg.instm->Release();
	
	trace("marshall call to %s completed %s", func_names[func], php_win_err(ret));
	
	return ret;
}

HRESULT marshal_stub(LPARAM lparam)
{
	struct activephp_serialize_msg *msg = (struct activephp_serialize_msg*)lparam;

	if (msg->instm) {
		msg->ret = do_marshal_out(1, msg->args, msg->marshal_defs, msg->instm);

		if (FAILED(msg->ret)) {
			SetEvent(msg->evt);
			return msg->ret;
		}
	}
	
	switch (msg->func) {
		case APHP_ParseScriptText:
			msg->ret = msg->engine->ParseScriptText(
					(LPCOLESTR)msg->args[0],
					(LPCOLESTR)msg->args[1],
					(IUnknown*)msg->args[2],
					(LPCOLESTR)msg->args[3],
					(DWORD)msg->args[4],
					(ULONG)msg->args[5],
					(DWORD)msg->args[6],
					(VARIANT*)msg->args[7],
					(EXCEPINFO*)msg->args[8]);
			break;

		case APHP_InitNew:
			msg->ret = msg->engine->InitNew();
			break;

		case APHP_AddNamedItem:
			msg->ret = msg->engine->AddNamedItem(
					(LPCOLESTR)msg->args[0],
					(DWORD)msg->args[1]);
			break;

		case APHP_SetScriptState:
			msg->ret = msg->engine->SetScriptState((SCRIPTSTATE)(LONG)msg->args[0]);
			break;

		case APHP_GetScriptDispatch:
			msg->ret = msg->engine->GetScriptDispatch(
					(LPCOLESTR)msg->args[0],
				 	(IDispatch**)msg->args[1]);
			break;

		case APHP_Close:
			msg->ret = msg->engine->Close();
			break;

		case APHP_AddTypeLib:
			msg->ret = msg->engine->AddTypeLib(
					(REFGUID)msg->args[0],
					(DWORD)msg->args[1],
					(DWORD)msg->args[2],
					(DWORD)msg->args[3]);
			break;
				
		case APHP_AddScriptlet:
			msg->ret = msg->engine->AddScriptlet(
					(LPCOLESTR)msg->args[0],
					(LPCOLESTR)msg->args[1],
					(LPCOLESTR)msg->args[2],
					(LPCOLESTR)msg->args[3],
					(LPCOLESTR)msg->args[4],
					(LPCOLESTR)msg->args[5],
					(DWORD)msg->args[6],
					(ULONG)msg->args[7],
					(DWORD)msg->args[8],
					(BSTR*)msg->args[9],
					(EXCEPINFO*)msg->args[10]);
			break;

		default:
			msg->ret = E_NOTIMPL;
	}

	if (SUCCEEDED(msg->ret)) {
		msg->ret = do_marshal_in(1, msg->args, msg->marshal_defs, &msg->outstm);
	}

	SetEvent(msg->evt);
	
	return msg->ret;
}

