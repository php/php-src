/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui  <laruence@php.net>                              |
  +----------------------------------------------------------------------+
*/
 
/* $Id$ */

#ifndef PHP_YAF_H
#define PHP_YAF_H

extern zend_module_entry yaf_module_entry;
#define phpext_yaf_ptr &yaf_module_entry

#ifdef PHP_WIN32
#define PHP_YAF_API __declspec(dllexport)
#ifndef _MSC_VER
#define _MSC_VER 1600
#endif      
#else
#define PHP_YAF_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define YAF_G(v) TSRMG(yaf_globals_id, zend_yaf_globals *, v)
#else
#define YAF_G(v) (yaf_globals.v)
#endif

#define YAF_STARTUP_FUNCTION(module)   	ZEND_MINIT_FUNCTION(yaf_##module)
#define YAF_RINIT_FUNCTION(modle)		ZEND_RINIT_FUNCTION(yaf_##module)
#define YAF_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define YAF_SHUTDOWN_FUNCTION(module)  	ZEND_MINIT_FUNCTION(yaf_##module)
#define YAF_SHUTDOWN(module)	 	    ZEND_MODULE_SHUTDOWN_N(yaf_##module)(INIT_FUNC_ARGS_PASSTHRU)

#define YAF_VERSION 					"2.1.0"

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
#define YAF_HAVE_NAMESPACE			
#else
#define Z_ADDREF_P 	 ZVAL_ADDREF
#define Z_REFCOUNT_P ZVAL_REFCOUNT
#define Z_DELREF_P 	 ZVAL_DELREF
#endif

#define yaf_application_t	zval
#define yaf_view_t 			zval
#define yaf_controller_t	zval
#define yaf_request_t		zval
#define yaf_router_t		zval
#define yaf_route_t			zval
#define yaf_dispatcher_t	zval
#define yaf_action_t		zval
#define yaf_loader_t		zval
#define yaf_response_t		zval
#define yaf_config_t		zval
#define yaf_registry_t		zval
#define yaf_plugin_t		zval
#define yaf_session_t		zval
#define yaf_exception_t		zval

extern PHPAPI int  php_register_info_logo(char *logo_string, const char *mimetype, const unsigned char *data, int size);
extern PHPAPI void php_var_dump(zval **struc, int level TSRMLS_DC);
extern PHPAPI void php_debug_zval_dump(zval **struc, int level TSRMLS_DC);

#define YAF_STRS	ZEND_STRS
#define YAF_STRL	ZEND_STRL

#define YAF_PSTRS(z)	Z_STRVAL_P(z), Z_STRLEN_P(z) + 1	
#define YAF_PSTRL(z)	Z_STRVAL_P(z), Z_STRLEN_P(z)

#define YAF_PPSTRS(z) \
	Z_STRVAL_PP(z), Z_STRLEN_PP(z) + 1	
#define YAF_PPSTRL(z) \
	Z_STRVAL_PP(z), Z_STRLEN_PP(z)

ZEND_BEGIN_MODULE_GLOBALS(yaf)
	char 		*ext;
	char		*base_uri;
	char 		*environ;
	char 		*directory;
	char 		*library_directory;
	char 		*global_library;
	char 		*view_ext;
	char 		*default_module;
	char 		*default_controller;
	char 		*default_action;
	char 		*bootstrap;
	char 		*name_separator;
	zend_bool 	lowcase_path;
	zend_bool 	use_spl_autoload;
	zend_bool 	throw_exception;
	zend_bool 	cache_config;
	zend_bool   action_prefer;
	zend_bool	name_suffix;
	zend_bool  	autoload_started;
	zend_bool  	running;
	zend_bool  	in_exception;
	zend_bool  	catch_exception;
/* {{{ This only effects internally */
	zend_bool  	st_compatible;
/* }}} */
	long		forward_limit;
	HashTable	*configs;
	zval 		*modules;
	uint 		buf_nesting;
	void		*buffer;
	void 		*owrite_handler;
#ifdef YAF_HAVE_NAMESPACE
	zend_bool	use_namespace;
#endif
ZEND_END_MODULE_GLOBALS(yaf)

PHP_MINIT_FUNCTION(yaf);
PHP_MSHUTDOWN_FUNCTION(yaf);
PHP_RINIT_FUNCTION(yaf);
PHP_RSHUTDOWN_FUNCTION(yaf);
PHP_MINFO_FUNCTION(yaf);

extern ZEND_DECLARE_MODULE_GLOBALS(yaf);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
