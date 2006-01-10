/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PHAR_H
#define PHP_PHAR_H

#include "ext/standard/basic_functions.h"
extern zend_module_entry phar_module_entry;
#define phpext_phar_ptr &phar_module_entry

#ifdef PHP_WIN32
#define PHP_PHAR_API __declspec(dllexport)
#else
#define PHP_PHAR_API
#endif

/* In every utility function you add that needs to use variables 
   in php_phar_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PHAR_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#	include "TSRM.h"
#	define PHAR_G(v) TSRMG(phar_globals_id, zend_phar_globals *, v)
#	define PHAR_GLOBALS ((zend_phar_globals *) (*((void ***) tsrm_ls))[TSRM_UNSHUFFLE_RSRC_ID(phar_globals_id)])
#else
#	define PHAR_G(v) (phar_globals.v)
#	define PHAR_GLOBALS (&phar_globals)
#endif

#endif	/* PHP_PHAR_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
