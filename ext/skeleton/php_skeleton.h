/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */
 
/* $Id: */

#ifndef _PHP_EXTNAME_H
#define _PHP_EXTNAME_H

#ifdef COMPILE_DL_EXTNAME
#undef HAVE_EXTNAME
#define HAVE_EXTNAME 1
#endif

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
__function_declarations_here__

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

#define php_extname_ptr NULL

#endif

#endif	/* _PHP_EXTNAME_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
