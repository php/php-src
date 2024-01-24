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
   +----------------------------------------------------------------------+
*/

#ifndef DOM_INTERNAL_MACROS
#define DOM_INTERNAL_MACROS

/* We're using the type flags of the zval to store an extra flag. */
#define DOM_Z_OWNED(z, v)	ZVAL_PTR(z, (void *) v)
#define DOM_Z_UNOWNED(z, v)	ZVAL_INDIRECT(z, (void *) v)
#define DOM_Z_IS_OWNED(z)	(Z_TYPE_P(z) == IS_PTR)

#endif
