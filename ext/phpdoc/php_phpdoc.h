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
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_PHPDOC_H
#define PHP_PHPDOC_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_PHPDOC

extern zend_module_entry phpdoc_module_entry;
#define phpext_phpdoc_ptr &phpdoc_module_entry

#ifdef PHP_WIN32
#define PHP_PHPDOC_API __declspec(dllexport)
#else
#define PHP_PHPDOC_API
#endif

PHP_MINIT_FUNCTION(phpdoc);
PHP_MSHUTDOWN_FUNCTION(phpdoc);
PHP_RINIT_FUNCTION(phpdoc);
PHP_RSHUTDOWN_FUNCTION(phpdoc);
PHP_MINFO_FUNCTION(phpdoc);

PHP_FUNCTION(confirm_phpdoc_compiled);	/* For testing, remove later. */
PHP_FUNCTION(phpdoc_xml_from_string);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(phpdoc)
	int global_variable;
ZEND_END_MODULE_GLOBALS(phpdoc)
*/

/* In every function that needs to use variables in php_phpdoc_globals,
   do call PHPDOCLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as PHPDOCG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define PHPDOCG(v) (phpdoc_globals->v)
#define PHPDOCLS_FETCH() php_phpdoc_globals *phpdoc_globals = ts_resource(phpdoc_globals_id)
#else
#define PHPDOCG(v) (phpdoc_globals.v)
#define PHPDOCLS_FETCH()
#endif

#else

#define phpext_phpdoc_ptr NULL

#endif

#endif	/* PHP_PHPDOC_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
