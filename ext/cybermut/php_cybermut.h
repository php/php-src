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
   | Authors: Sylvain PAGES <spages@free.fr>                              |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_CYBERMUT_H
#define PHP_CYBERMUT_H

extern zend_module_entry cybermut_module_entry;
#define phpext_cybermut_ptr &cybermut_module_entry

#ifdef PHP_WIN32
#define PHP_CYBERMUT_API __declspec(dllexport)
#else
#define PHP_CYBERMUT_API
#endif

PHP_MINIT_FUNCTION(cybermut);
PHP_MSHUTDOWN_FUNCTION(cybermut);
PHP_RINIT_FUNCTION(cybermut);
PHP_RSHUTDOWN_FUNCTION(cybermut);
PHP_MINFO_FUNCTION(cybermut);

PHP_FUNCTION(confirm_cybermut_compiled);	/* For testing, remove later. */
PHP_FUNCTION(cybermut_creerformulairecm);
PHP_FUNCTION(cybermut_testmac);
PHP_FUNCTION(cybermut_creerreponsecm);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(cybermut)
	int global_variable;
ZEND_END_MODULE_GLOBALS(cybermut)
*/

/* In every function that needs to use variables in php_cybermut_globals,
   do call CYBERMUTLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as CYBERMUTG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define CYBERMUTG(v) (cybermut_globals->v)
#define CYBERMUTLS_FETCH() php_cybermut_globals *cybermut_globals = ts_resource(cybermut_globals_id)
#else
#define CYBERMUTG(v) (cybermut_globals.v)
#define CYBERMUTLS_FETCH()
#endif

#endif	/* PHP_CYBERMUT_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
