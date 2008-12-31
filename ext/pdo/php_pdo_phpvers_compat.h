/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* macros for backwards compatibility with PHP 5, so that HEAD of PDO and its
 * drivers may still build against PHP 5 */

#ifndef PHP_PDO_PHPVERS_COMPAT_H
# define PHP_PDO_PHPVERS_COMPAT_H
# if PHP_MAJOR_VERSION < 6

typedef char *zstr;
#  define ZSTR(x) x

#  define ZVAL_RT_STRINGL(a, b, c, d)		ZVAL_STRINGL(a, b, c, d)
#  define ZVAL_ASCII_STRINGL(a, b, c, d)	ZVAL_STRINGL(a, b, c, d)
#  define PDO_ZVAL_PP_IS_TEXT(pp)			(Z_TYPE_PP(pp) == IS_STRING)
#  define PDO_ZVAL_P_IS_TEXT(pp)			(Z_TYPE_P(pp) == IS_STRING)
#  define Z_UNIVAL_PP(x)					Z_STRVAL_PP(x)
#  define Z_UNIVAL_P(x)						Z_STRVAL_P(x)
#  define Z_UNIVAL(x)						Z_STRVAL(x)
#  define Z_UNILEN_PP(x)					Z_STRLEN_PP(x)
#  define Z_UNILEN_P(x)						Z_STRLEN_P(x)
#  define Z_UNILEN(x)						Z_STRLEN(x)
#  define zend_u_hash_find(a, b, c, d, e)	zend_hash_find(a, c, d, e)
#  define zend_u_lookup_class(a, b, c, d)	zend_lookup_class(b, c, d)
PDO_API char *php_pdo_str_tolower_dup(const char *src, int len);
#  define zend_u_str_tolower_dup(a, b, c)	php_pdo_str_tolower_dup(b, c)
#  define convert_to_text(x)				convert_to_string(x)
#  define zend_u_fetch_class(a, b, c, d)	zend_fetch_class(b, c, d)
#  define PDO_MEMBER_IS(z, strlit)			((Z_STRLEN_P(z) == sizeof(strlit)-1) && 0 == strcmp(Z_STRVAL_P(z), strlit))
#  define pdo_zstr_efree(x)  efree(x)
#  define pdo_zstr_sval(x)   x
# else

#  define PDO_ZVAL_PP_IS_TEXT(pp)			((Z_TYPE_PP(pp) == IS_STRING) || (Z_TYPE_PP(pp) == IS_UNICODE))
#  define PDO_ZVAL_P_IS_TEXT(pp)			((Z_TYPE_P(pp) == IS_STRING) || (Z_TYPE_P(pp) == IS_UNICODE))

#  define PDO_MEMBER_IS(z, strlit)			((Z_UNILEN_P(z) == sizeof(strlit)-1) && (ZEND_U_EQUAL(Z_TYPE_P(z), Z_UNIVAL_P(z), Z_UNILEN_P(z), strlit, sizeof(strlit)-1)))
#  define pdo_zstr_efree(x)  efree(x.v)
#  define pdo_zstr_sval(x)   x.s

# endif
#endif

