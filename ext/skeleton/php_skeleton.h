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
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(extname)
*/

/* In every utility function you add that needs to use variables 
   in php_extname_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as EXTNAME_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define EXTNAME_G(v) TSRMG(extname_globals_id, zend_extname_globals *, v)
#else
#define EXTNAME_G(v) (extname_globals.v)
#endif

#endif	/* PHP_EXTNAME_H */

/* __footer_here__ */
