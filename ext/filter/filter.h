/*
  $Id$
*/

#ifndef PHP_FILTER_H
#define PHP_FILTER_H

extern zend_module_entry filter_module_entry;
#define phpext_filter_ptr &filter_module_entry

#ifdef PHP_WIN32
#define PHP_FILTER_API __declspec(dllexport)
#else
#define PHP_FILTER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#if PHP_API_VERSION > 20041224
unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len);
#else
unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len);
#endif

PHP_MINIT_FUNCTION(filter);
PHP_MSHUTDOWN_FUNCTION(filter);
PHP_RINIT_FUNCTION(filter);
PHP_RSHUTDOWN_FUNCTION(filter);
PHP_MINFO_FUNCTION(filter);

PHP_FUNCTION(filter);

ZEND_BEGIN_MODULE_GLOBALS(filter)
	zval *post_array;
	zval *get_array;
	zval *cookie_array;
	zval *env_array;
	zval *server_array;
	int default_filter;
ZEND_END_MODULE_GLOBALS(filter)

#ifdef ZTS
#define IF_G(v) TSRMG(filter_globals_id, zend_filter_globals *, v)
#else
#define IF_G(v) (filter_globals.v)
#endif

#endif	/* FILTER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
