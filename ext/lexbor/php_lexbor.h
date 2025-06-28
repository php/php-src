/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
