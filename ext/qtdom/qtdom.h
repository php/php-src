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

#ifndef PHP_QTDOM_H
#define PHP_QTDOM_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_QTDOM

extern zend_module_entry qtdom_module_entry;
#define phpext_qtdom_ptr &qtdom_module_entry

#ifdef PHP_WIN32
#define PHP_QTDOM_API __declspec(dllexport)
#else
#define PHP_QTDOM_API
#endif

static zend_class_entry *qdomdoc_class_entry_ptr;
static zend_class_entry *qdomnode_class_entry_ptr;

PHP_MINIT_FUNCTION(qtdom);
PHP_MSHUTDOWN_FUNCTION(qtdom);
PHP_RINIT_FUNCTION(qtdom);
PHP_RSHUTDOWN_FUNCTION(qtdom);
PHP_MINFO_FUNCTION(qtdom);

PHP_FUNCTION(qdom_tree);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(qtdom)
	int global_variable;
ZEND_END_MODULE_GLOBALS(qtdom)
*/

/* In every function that needs to use variables in php_qtdom_globals,
   do call QTDOMLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as QTDOMG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define QTDOMG(v) (qtdom_globals->v)
#define QTDOMLS_FETCH() php_qtdom_globals *qtdom_globals = ts_resource(qtdom_globals_id)
#else
#define QTDOMG(v) (qtdom_globals.v)
#define QTDOMLS_FETCH()
#endif

#else

#define phpext_qtdom_ptr NULL

#endif

#endif	/* PHP_QTDOM_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
