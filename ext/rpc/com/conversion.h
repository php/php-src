/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   |         Alan Brown <abrown@pobox.com>                                |
   +----------------------------------------------------------------------+
 */

#ifndef CONVERSION_H
#define CONVERSION_H

/* isn't this defined somewhere else ? */

#define Z_TRUE 1
#define Z_FALSE 0

#define VT_TRUE -1
#define VT_FALSE 0

BEGIN_EXTERN_C()

ZEND_API void php_zval_to_variant(zval *zval_arg, VARIANT *var_arg, int codepage);
ZEND_API void php_zval_to_variant_ex(zval *zval_arg, VARIANT *var_arg, int type, int codepage);
ZEND_API int php_variant_to_zval(VARIANT *var_arg, zval *zval_arg, int codepage);

ZEND_API OLECHAR *php_char_to_OLECHAR(char *C_str, uint strlen, int codepage, int persist);
ZEND_API char *php_OLECHAR_to_char(OLECHAR *unicode_str, uint *out_length, int codepage, int persist);

END_EXTERN_C()

#endif