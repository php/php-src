/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   |          Mate Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_LEXBOR_H
#define PHP_LEXBOR_H

#ifdef HAVE_LEXBOR
extern zend_module_entry lexbor_module_entry;
#define phpext_lexbor_ptr &lexbor_module_entry

#if defined(ZTS) && defined(COMPILE_DL_LEXBOR)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif
#endif /* PHP_LEXBOR_H */
