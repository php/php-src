#ifndef PHP_RPC_H
#define PHP_RPC_H

#include "zend.h"

extern zend_module_entry rpc_module_entry;
#define phpext_rpc_ptr &rpc_module_entry

#ifdef PHP_WIN32
#define PHP_RPC_API __declspec(dllexport)
#else
#define PHP_RPC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_MINIT_FUNCTION(rpc);
ZEND_MSHUTDOWN_FUNCTION(rpc);
ZEND_RINIT_FUNCTION(rpc);
ZEND_RSHUTDOWN_FUNCTION(rpc);
ZEND_MINFO_FUNCTION(rpc);

ZEND_FUNCTION(rpc_load);
ZEND_FUNCTION(rpc_call);
ZEND_FUNCTION(rpc_set);
ZEND_FUNCTION(rpc_get);
ZEND_FUNCTION(rpc_singleton);
ZEND_FUNCTION(rpc_poolable);

ZEND_BEGIN_MODULE_GLOBALS(rpc)
	TsHashTable *proxy;
ZEND_END_MODULE_GLOBALS(rpc)

#ifdef ZTS
#define RPC_G(v) TSRMG(rpc_globals_id, zend_rpc_globals *, v)
#else
#define RPC_G(v) (rpc_globals.v)
#endif
	
#define phpext_rpc_ptr &rpc_module_entry

#endif	/* PHP_RPC_H */