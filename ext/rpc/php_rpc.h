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

PHP_MINIT_FUNCTION(rpc);
PHP_MSHUTDOWN_FUNCTION(rpc);
PHP_MINFO_FUNCTION(rpc);

ZEND_BEGIN_MODULE_GLOBALS(rpc)
	zend_object_handle handle;
	HashTable *instance;
	HashTable *handlers;
ZEND_END_MODULE_GLOBALS(rpc)

#ifdef ZTS
#define RPC_G(v) TSRMG(rpc_globals_id, zend_rpc_globals *, v)
#else
#define RPC_G(v) (rpc_globals.v)
#endif

#endif	/* PHP_RPC_H */