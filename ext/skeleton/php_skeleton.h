/* __header_here__ */

#ifndef PHP_EXTNAME_H
#define PHP_EXTNAME_H

extern zend_module_entry extname_module_entry;
#define phpext_extname_ptr &extname_module_entry

#ifdef PHP_WIN32
#define PHP_EXTNAME_API __declspec(dllexport)
#else
#define PHP_EXTNAME_API
#endif

PHP_MINIT_FUNCTION(extname);
PHP_MSHUTDOWN_FUNCTION(extname);
PHP_RINIT_FUNCTION(extname);
PHP_RSHUTDOWN_FUNCTION(extname);
PHP_MINFO_FUNCTION(extname);

PHP_FUNCTION(confirm_extname_compiled);	/* For testing, remove later. */
/* __function_declarations_here__ */

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(extname)
	int global_variable;
ZEND_END_MODULE_GLOBALS(extname)
*/

/* In every function that needs to use variables in php_extname_globals,
   do call EXTNAMELS_FETCH(); after declaring other variables used by
   that function, and always refer to them as EXTNAMEG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define EXTNAMEG(v) (extname_globals->v)
#define EXTNAMELS_FETCH() php_extname_globals *extname_globals = ts_resource(extname_globals_id)
#else
#define EXTNAMEG(v) (extname_globals.v)
#define EXTNAMELS_FETCH()
#endif

#endif	/* PHP_EXTNAME_H */

/* __footer_here__ */
