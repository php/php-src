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

#ifndef _PHP__EXTNAME__H
#define _PHP__EXTNAME__H

#ifdef COMPILE_DL__EXTNAME_
#undef HAVE__EXTNAME_
#define HAVE__EXTNAME_ 1
#endif

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE__EXTNAME_

extern zend_module_entry _extname__module_entry;
#define phpext__extname__ptr &_extname__module_entry

#ifdef PHP_WIN32
#define PHP__EXTNAME__API __declspec(dllexport)
#else
#define PHP__EXTNAME__API
#endif

PHP_MINIT_FUNCTION(_extname_);
PHP_MSHUTDOWN_FUNCTION(_extname_);
PHP_RINIT_FUNCTION(_extname_);
PHP_RSHUTDOWN_FUNCTION(_extname_);
PHP_MINFO_FUNCTION(_extname_);

PHP_FUNCTION(_extname__test);	/* For testing, remove later. */

typedef struct {
	/* Fill in this structure and use entries in it
	   for thread safety instead of using true globals.
	*/
	/* You can use the next one as type if your module registers any
	   resources. Oh, you can of course rename it to something more
	   suitable, add list entry types or remove it if it not needed.
	   It's just an example.
	*/
	int le__extname_;
} php__extname__globals;

/* In every function that needs to use variables in php__extname__globals,
   do call _EXTNAME_LS_FETCH(); after declaring other variables used by
   that function, and always refer to them as _EXTNAME_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define _EXTNAME_G(v) (_extname__globals->v)
#define _EXTNAME_LS_FETCH() php__extname__globals *_extname__globals = ts_resource(gd__extname__id)
#else
#define _EXTNAME_G(v) (_extname__globals.v)
#define _EXTNAME_LS_FETCH()
#endif

#else

#define php__extname__ptr NULL

#endif

#endif	/* _PHP__EXTNAME__H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
