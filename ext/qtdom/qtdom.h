/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jan Borsodi                                                  |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_QTDOM_H
#define PHP_QTDOM_H

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
PHP_FUNCTION(qdom_error);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(qtdom)
	int global_variable;
ZEND_END_MODULE_GLOBALS(qtdom)
*/

#ifdef ZTS
# define QTDOMG(v) TSRMG(qtdom_globals_id, zend_qtdom_globals *, v)
#else
# define QTDOMG(v) (qtdom_globals.v)
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
