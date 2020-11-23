/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong  <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* Infrastructure for working with persistent COM objects.
 * Implements: IStream* wrapper for PHP streams.
 * TODO: Magic __wakeup and __sleep handlers for serialization
 * (can wait till 5.1) */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"
#include "Zend/zend_exceptions.h"
#include "com_persist_arginfo.h"

/* {{{ expose php_stream as a COM IStream */

typedef struct {
	CONST_VTBL struct IStreamVtbl *lpVtbl;
	DWORD engine_thread;
	LONG refcount;
	php_stream *stream;
	zend_resource *res;
} php_istream;

static int le_istream;
static void istream_destructor(php_istream *stm);

static void istream_dtor(zend_resource *rsrc)
{
	php_istream *stm = (php_istream *)rsrc->ptr;
	istream_destructor(stm);
}

#define FETCH_STM()	\
	php_istream *stm = (php_istream*)This; \
	if (GetCurrentThreadId() != stm->engine_thread) \
		return RPC_E_WRONG_THREAD;

#define FETCH_STM_EX()	\
	php_istream *stm = (php_istream*)This;	\
	if (GetCurrentThreadId() != stm->engine_thread)	\
		return RPC_E_WRONG_THREAD;

static HRESULT STDMETHODCALLTYPE stm_queryinterface(
	IStream *This,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	FETCH_STM_EX();

	if (IsEqualGUID(&IID_IUnknown, riid) ||
			IsEqualGUID(&IID_IStream, riid)) {
		*ppvObject = This;
		InterlockedIncrement(&stm->refcount);
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE stm_addref(IStream *This)
{
	FETCH_STM_EX();

	return InterlockedIncrement(&stm->refcount);
}

static ULONG STDMETHODCALLTYPE stm_release(IStream *This)
{
	ULONG ret;
	FETCH_STM();

	ret = InterlockedDecrement(&stm->refcount);
	if (ret == 0) {
		/* destroy it */
		if (stm->res)
			zend_list_delete(stm->res);
	}
	return ret;
}

static HRESULT STDMETHODCALLTYPE stm_read(IStream *This, void *pv, ULONG cb, ULONG *pcbRead)
{
	ULONG nread;
	FETCH_STM();

	nread = (ULONG)php_stream_read(stm->stream, pv, cb);

	if (pcbRead) {
		*pcbRead = nread > 0 ? nread : 0;
	}
	if (nread > 0) {
		return S_OK;
	}
	return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE stm_write(IStream *This, void const *pv, ULONG cb, ULONG *pcbWritten)
{
	ssize_t nwrote;
	FETCH_STM();

	nwrote = php_stream_write(stm->stream, pv, cb);

	if (pcbWritten) {
		*pcbWritten = nwrote > 0 ? (ULONG)nwrote : 0;
	}
	if (nwrote > 0) {
		return S_OK;
	}
	return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE stm_seek(IStream *This, LARGE_INTEGER dlibMove,
		DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	off_t offset;
	int whence;
	int ret;
	FETCH_STM();

	switch (dwOrigin) {
		case STREAM_SEEK_SET:	whence = SEEK_SET;	break;
		case STREAM_SEEK_CUR:	whence = SEEK_CUR;	break;
		case STREAM_SEEK_END:	whence = SEEK_END;	break;
		default:
			return STG_E_INVALIDFUNCTION;
	}

	if (dlibMove.HighPart) {
		/* we don't support 64-bit offsets */
		return STG_E_INVALIDFUNCTION;
	}

	offset = (off_t) dlibMove.QuadPart;

	ret = php_stream_seek(stm->stream, offset, whence);

	if (plibNewPosition) {
		plibNewPosition->QuadPart = (ULONGLONG)(ret >= 0 ? ret : 0);
	}

	return ret >= 0 ? S_OK : STG_E_INVALIDFUNCTION;
}

static HRESULT STDMETHODCALLTYPE stm_set_size(IStream *This, ULARGE_INTEGER libNewSize)
{
	FETCH_STM();

	if (libNewSize.HighPart) {
		return STG_E_INVALIDFUNCTION;
	}

	if (php_stream_truncate_supported(stm->stream)) {
		int ret = php_stream_truncate_set_size(stm->stream, (size_t)libNewSize.QuadPart);

		if (ret == 0) {
			return S_OK;
		}
	}

	return STG_E_INVALIDFUNCTION;
}

static HRESULT STDMETHODCALLTYPE stm_copy_to(IStream *This, IStream *pstm, ULARGE_INTEGER cb,
		ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	FETCH_STM_EX();

	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE stm_commit(IStream *This, DWORD grfCommitFlags)
{
	FETCH_STM();

	php_stream_flush(stm->stream);

	return S_OK;
}

static HRESULT STDMETHODCALLTYPE stm_revert(IStream *This)
{
	/* NOP */
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE stm_lock_region(IStream *This,
   	ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD lockType)
{
	return STG_E_INVALIDFUNCTION;
}

static HRESULT STDMETHODCALLTYPE stm_unlock_region(IStream *This,
		ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD lockType)
{
	return STG_E_INVALIDFUNCTION;
}

static HRESULT STDMETHODCALLTYPE stm_stat(IStream *This,
		STATSTG *pstatstg, DWORD grfStatFlag)
{
	return STG_E_INVALIDFUNCTION;
}

static HRESULT STDMETHODCALLTYPE stm_clone(IStream *This, IStream **ppstm)
{
	return STG_E_INVALIDFUNCTION;
}

static struct IStreamVtbl php_istream_vtbl = {
	stm_queryinterface,
	stm_addref,
	stm_release,
	stm_read,
	stm_write,
	stm_seek,
	stm_set_size,
	stm_copy_to,
	stm_commit,
	stm_revert,
	stm_lock_region,
	stm_unlock_region,
	stm_stat,
	stm_clone
};

static void istream_destructor(php_istream *stm)
{
	if (stm->refcount > 0) {
		CoDisconnectObject((IUnknown*)stm, 0);
	}

	zend_list_delete(stm->stream->res);

	CoTaskMemFree(stm);
}
/* }}} */

PHP_COM_DOTNET_API IStream *php_com_wrapper_export_stream(php_stream *stream)
{
	php_istream *stm = (php_istream*)CoTaskMemAlloc(sizeof(*stm));

	if (stm == NULL)
		return NULL;

	memset(stm, 0, sizeof(*stm));
	stm->engine_thread = GetCurrentThreadId();
	stm->lpVtbl = &php_istream_vtbl;
	stm->refcount = 1;
	stm->stream = stream;

	GC_ADDREF(stream->res);
	stm->res = zend_register_resource(stm, le_istream);

	return (IStream*)stm;
}

#define CPH_METHOD(fname)		PHP_METHOD(COMPersistHelper, fname)

#define CPH_FETCH()				php_com_persist_helper *helper = (php_com_persist_helper*)Z_OBJ_P(getThis());

#define CPH_NO_OBJ()			if (helper->unk == NULL) { php_com_throw_exception(E_INVALIDARG, "No COM object is associated with this helper instance"); RETURN_THROWS(); }

typedef struct {
	zend_object			std;
	long codepage;
	IUnknown 			*unk;
	IPersistStream 		*ips;
	IPersistStreamInit	*ipsi;
	IPersistFile		*ipf;
} php_com_persist_helper;

static zend_object_handlers helper_handlers;
static zend_class_entry *helper_ce;

static inline HRESULT get_persist_stream(php_com_persist_helper *helper)
{
	if (!helper->ips && helper->unk) {
		return IUnknown_QueryInterface(helper->unk, &IID_IPersistStream, &helper->ips);
	}
	return helper->ips ? S_OK : E_NOTIMPL;
}

static inline HRESULT get_persist_stream_init(php_com_persist_helper *helper)
{
	if (!helper->ipsi && helper->unk) {
		return IUnknown_QueryInterface(helper->unk, &IID_IPersistStreamInit, &helper->ipsi);
	}
	return helper->ipsi ? S_OK : E_NOTIMPL;
}

static inline HRESULT get_persist_file(php_com_persist_helper *helper)
{
	if (!helper->ipf && helper->unk) {
		return IUnknown_QueryInterface(helper->unk, &IID_IPersistFile, &helper->ipf);
	}
	return helper->ipf ? S_OK : E_NOTIMPL;
}


/* {{{ Determines the filename into which an object will be saved, or false if none is set, via IPersistFile::GetCurFile */
CPH_METHOD(GetCurFileName)
{
	HRESULT res;
	OLECHAR *olename = NULL;
	CPH_FETCH();

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CPH_NO_OBJ();

	res = get_persist_file(helper);
	if (helper->ipf) {
		res = IPersistFile_GetCurFile(helper->ipf, &olename);

		if (res == S_OK) {
			size_t len;
			char *str = php_com_olestring_to_string(olename,
				   &len, helper->codepage);
			RETVAL_STRINGL(str, len);
			// TODO: avoid reallocarion???
			efree(str);
			CoTaskMemFree(olename);
			return;
		} else if (res == S_FALSE) {
			CoTaskMemFree(olename);
			RETURN_FALSE;
		}
		php_com_throw_exception(res, NULL);
	} else {
		php_com_throw_exception(res, NULL);
	}
}
/* }}} */


/* {{{ Persist object data to file, via IPersistFile::Save */
CPH_METHOD(SaveToFile)
{
	HRESULT res;
	char *filename, *fullpath = NULL;
	size_t filename_len;
	zend_bool remember = TRUE;
	OLECHAR *olefilename = NULL;
	CPH_FETCH();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "p!|b",
				&filename, &filename_len, &remember)) {
		RETURN_THROWS();
	}

	CPH_NO_OBJ();

	res = get_persist_file(helper);
	if (helper->ipf) {
		if (filename) {
			fullpath = expand_filepath(filename, NULL);
			if (!fullpath) {
				RETURN_FALSE;
			}

			if (php_check_open_basedir(fullpath)) {
				efree(fullpath);
				RETURN_FALSE;
			}

			olefilename = php_com_string_to_olestring(fullpath, strlen(fullpath), helper->codepage);
			efree(fullpath);
		}
		res = IPersistFile_Save(helper->ipf, olefilename, remember);
		if (SUCCEEDED(res)) {
			if (!olefilename) {
				res = IPersistFile_GetCurFile(helper->ipf, &olefilename);
				if (S_OK == res) {
					IPersistFile_SaveCompleted(helper->ipf, olefilename);
					CoTaskMemFree(olefilename);
					olefilename = NULL;
				}
			} else if (remember) {
				IPersistFile_SaveCompleted(helper->ipf, olefilename);
			}
		}

		if (olefilename) {
			efree(olefilename);
		}

		if (FAILED(res)) {
			php_com_throw_exception(res, NULL);
		}

	} else {
		php_com_throw_exception(res, NULL);
	}
}
/* }}} */

/* {{{ Load object data from file, via IPersistFile::Load */
CPH_METHOD(LoadFromFile)
{
	HRESULT res;
	char *filename, *fullpath;
	size_t filename_len;
	zend_long flags = 0;
	OLECHAR *olefilename;
	CPH_FETCH();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "p|l",
				&filename, &filename_len, &flags)) {
		RETURN_THROWS();
	}

	CPH_NO_OBJ();

	res = get_persist_file(helper);
	if (helper->ipf) {
		if (!(fullpath = expand_filepath(filename, NULL))) {
			RETURN_FALSE;
		}

		if (php_check_open_basedir(fullpath)) {
			efree(fullpath);
			RETURN_FALSE;
		}

		olefilename = php_com_string_to_olestring(fullpath, strlen(fullpath), helper->codepage);
		efree(fullpath);

		res = IPersistFile_Load(helper->ipf, olefilename, (DWORD)flags);
		efree(olefilename);

		if (FAILED(res)) {
			php_com_throw_exception(res, NULL);
		}

	} else {
		php_com_throw_exception(res, NULL);
	}
}
/* }}} */

/* {{{ Gets maximum stream size required to store the object data, via IPersistStream::GetSizeMax (or IPersistStreamInit::GetSizeMax) */
CPH_METHOD(GetMaxStreamSize)
{
	HRESULT res;
	ULARGE_INTEGER size;
	CPH_FETCH();

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CPH_NO_OBJ();

	res = get_persist_stream_init(helper);
	if (helper->ipsi) {
		res = IPersistStreamInit_GetSizeMax(helper->ipsi, &size);
	} else {
		res = get_persist_stream(helper);
		if (helper->ips) {
			res = IPersistStream_GetSizeMax(helper->ips, &size);
		} else {
			php_com_throw_exception(res, NULL);
			RETURN_THROWS();
		}
	}

	if (res != S_OK) {
		php_com_throw_exception(res, NULL);
	} else {
		/* TODO: handle 64 bit properly */
		RETURN_LONG((zend_long)size.QuadPart);
	}
}
/* }}} */

/* {{{ Initializes the object to a default state, via IPersistStreamInit::InitNew */
CPH_METHOD(InitNew)
{
	HRESULT res;
	CPH_FETCH();

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CPH_NO_OBJ();

	res = get_persist_stream_init(helper);
	if (helper->ipsi) {
		res = IPersistStreamInit_InitNew(helper->ipsi);

		if (res != S_OK) {
			php_com_throw_exception(res, NULL);
		} else {
			RETURN_TRUE;
		}
	} else {
		php_com_throw_exception(res, NULL);
	}
}
/* }}} */

/* {{{ Initializes an object from the stream where it was previously saved, via IPersistStream::Load or OleLoadFromStream */
CPH_METHOD(LoadFromStream)
{
	zval *zstm;
	php_stream *stream;
	IStream *stm = NULL;
	HRESULT res;
	CPH_FETCH();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zstm)) {
		RETURN_THROWS();
	}

	php_stream_from_zval_no_verify(stream, zstm);

	if (stream == NULL) {
		php_com_throw_exception(E_INVALIDARG, "expected a stream");
		RETURN_THROWS();
	}

	stm = php_com_wrapper_export_stream(stream);
	if (stm == NULL) {
		php_com_throw_exception(E_UNEXPECTED, "failed to wrap stream");
		RETURN_THROWS();
	}

	res = S_OK;
	RETVAL_TRUE;

	if (helper->unk == NULL) {
		IDispatch *disp = NULL;

		/* we need to create an object and load using OleLoadFromStream */
		res = OleLoadFromStream(stm, &IID_IDispatch, &disp);

		if (SUCCEEDED(res)) {
			php_com_wrap_dispatch(return_value, disp, COMG(code_page));
		}
	} else {
		res = get_persist_stream_init(helper);
		if (helper->ipsi) {
			res = IPersistStreamInit_Load(helper->ipsi, stm);
		} else {
			res = get_persist_stream(helper);
			if (helper->ips) {
				res = IPersistStreamInit_Load(helper->ipsi, stm);
			}
		}
	}
	IStream_Release(stm);

	if (FAILED(res)) {
		php_com_throw_exception(res, NULL);
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Saves the object to a stream, via IPersistStream::Save */
CPH_METHOD(SaveToStream)
{
	zval *zstm;
	php_stream *stream;
	IStream *stm = NULL;
	HRESULT res;
	CPH_FETCH();

	CPH_NO_OBJ();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zstm)) {
		RETURN_THROWS();
	}

	php_stream_from_zval_no_verify(stream, zstm);

	if (stream == NULL) {
		php_com_throw_exception(E_INVALIDARG, "expected a stream");
		RETURN_THROWS();
	}

	stm = php_com_wrapper_export_stream(stream);
	if (stm == NULL) {
		php_com_throw_exception(E_UNEXPECTED, "failed to wrap stream");
		RETURN_THROWS();
	}

	res = get_persist_stream_init(helper);
	if (helper->ipsi) {
		res = IPersistStreamInit_Save(helper->ipsi, stm, TRUE);
	} else {
		res = get_persist_stream(helper);
		if (helper->ips) {
			res = IPersistStream_Save(helper->ips, stm, TRUE);
		}
	}

	IStream_Release(stm);

	if (FAILED(res)) {
		php_com_throw_exception(res, NULL);
		RETURN_THROWS();
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Creates a persistence helper object, usually associated with a com_object */
CPH_METHOD(__construct)
{
	php_com_dotnet_object *obj = NULL;
	zval *zobj = NULL;
	CPH_FETCH();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|O!",
				&zobj, php_com_variant_class_entry)) {
		RETURN_THROWS();
	}

	if (!zobj) {
		return;
	}

	obj = CDNO_FETCH(zobj);

	if (V_VT(&obj->v) != VT_DISPATCH || V_DISPATCH(&obj->v) == NULL) {
		php_com_throw_exception(E_INVALIDARG, "parameter must represent an IDispatch COM object");
		RETURN_THROWS();
	}

	/* it is always safe to cast an interface to IUnknown */
	helper->unk = (IUnknown*)V_DISPATCH(&obj->v);
	IUnknown_AddRef(helper->unk);
	helper->codepage = obj->code_page;
}
/* }}} */


static void helper_free_storage(zend_object *obj)
{
	php_com_persist_helper *object = (php_com_persist_helper*)obj;

	if (object->ipf) {
		IPersistFile_Release(object->ipf);
	}
	if (object->ips) {
		IPersistStream_Release(object->ips);
	}
	if (object->ipsi) {
		IPersistStreamInit_Release(object->ipsi);
	}
	if (object->unk) {
		IUnknown_Release(object->unk);
	}
	zend_object_std_dtor(&object->std);
}


static zend_object* helper_clone(zend_object *obj)
{
	php_com_persist_helper *clone, *object = (php_com_persist_helper*) obj;

	clone = emalloc(sizeof(*object));
	memcpy(clone, object, sizeof(*object));

	zend_object_std_init(&clone->std, object->std.ce);

	if (clone->ipf) {
		IPersistFile_AddRef(clone->ipf);
	}
	if (clone->ips) {
		IPersistStream_AddRef(clone->ips);
	}
	if (clone->ipsi) {
		IPersistStreamInit_AddRef(clone->ipsi);
	}
	if (clone->unk) {
		IUnknown_AddRef(clone->unk);
	}
	return (zend_object*)clone;
}

static zend_object* helper_new(zend_class_entry *ce)
{
	php_com_persist_helper *helper;

	helper = emalloc(sizeof(*helper));
	memset(helper, 0, sizeof(*helper));

	zend_object_std_init(&helper->std, helper_ce);
	helper->std.handlers = &helper_handlers;

	return &helper->std;
}

int php_com_persist_minit(INIT_FUNC_ARGS)
{
	zend_class_entry ce;

	memcpy(&helper_handlers, &std_object_handlers, sizeof(helper_handlers));
	helper_handlers.free_obj = helper_free_storage;
	helper_handlers.clone_obj = helper_clone;

	INIT_CLASS_ENTRY(ce, "COMPersistHelper", class_COMPersistHelper_methods);
	ce.create_object = helper_new;
	helper_ce = zend_register_internal_class(&ce);
	helper_ce->ce_flags |= ZEND_ACC_FINAL;

	le_istream = zend_register_list_destructors_ex(istream_dtor,
			NULL, "com_dotnet_istream_wrapper", module_number);

	return SUCCESS;
}
