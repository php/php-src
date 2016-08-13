/* __header_here__ */

#ifndef PHP_EXTNAME_H
#define PHP_EXTNAME_H

extern zend_module_entry extname_module_entry;
#define phpext_extname_ptr &extname_module_entry

#define PHP_EXTNAME_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_EXTNAME_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_EXTNAME_API __attribute__ ((visibility("default")))
#else
#	define PHP_EXTNAME_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(extname)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(extname)
*/

/* Always refer to the globals in your function as EXTNAME_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define EXTNAME_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(extname, v)

#if defined(ZTS) && defined(COMPILE_DL_EXTNAME)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_EXTNAME_H */

/* __footer_here__ */
