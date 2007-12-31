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
   | Author: Hartmut Holzgraefe <hartmut@six.de>                          |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_CTYPE_H
#define PHP_CTYPE_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_CTYPE

extern zend_module_entry ctype_module_entry;
#define phpext_ctype_ptr &ctype_module_entry

#ifdef PHP_WIN32
#define PHP_CTYPE_API __declspec(dllexport)
#else
#define PHP_CTYPE_API
#endif

PHP_MINIT_FUNCTION(ctype);
PHP_MSHUTDOWN_FUNCTION(ctype);
PHP_RINIT_FUNCTION(ctype);
PHP_RSHUTDOWN_FUNCTION(ctype);
PHP_MINFO_FUNCTION(ctype);

PHP_FUNCTION(ctype_alnum);
PHP_FUNCTION(ctype_alpha);
PHP_FUNCTION(ctype_cntrl);
PHP_FUNCTION(ctype_digit);
PHP_FUNCTION(ctype_lower);
PHP_FUNCTION(ctype_graph);
PHP_FUNCTION(ctype_print);
PHP_FUNCTION(ctype_punct);
PHP_FUNCTION(ctype_space);
PHP_FUNCTION(ctype_upper);
PHP_FUNCTION(ctype_xdigit);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(ctype)
	int global_variable;
ZEND_END_MODULE_GLOBALS(ctype)
*/

#ifdef ZTS
#define CTYPEG(v) TSRMG(ctype_globals_id, php_ctype_globals *, v)
#else
#define CTYPEG(v) (ctype_globals.v)
#endif

#else

#define phpext_ctype_ptr NULL

#endif

#endif	/* PHP_CTYPE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
