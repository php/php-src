/* __header_here__ */

#ifndef PHP_EXTNAME_H
#define PHP_EXTNAME_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_EXTNAME

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

/* Fill in this structure and use entries in it
   for thread safety instead of using true globals.
*/
typedef struct {
	/* You can use the next one as type if your module registers any
	   resources. Oh, you can of course rename it to something more
	   suitable, add list entry types or remove it if it not needed.
	   It's just an example.
	*/
	int le_extname;
} php_extname_globals;

/* In every function that needs to use variables in php_extname_globals,
   do call EXTNAMELS_FETCH(); after declaring other variables used by
   that function, and always refer to them as EXTNAMEG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define EXTNAMEG(v) (extname_globals->v)
#define EXTNAMELS_FETCH() php_extname_globals *extname_globals = ts_resource(gd_extname_id)
#else
#define EXTNAMEG(v) (extname_globals.v)
#define EXTNAMELS_FETCH()
#endif

#else

#define phpext_extname_ptr NULL

#endif

#endif	/* PHP_EXTNAME_H */

/* __footer_here__ */
