/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef NORMALIZER_NORMALIZE_H
#define NORMALIZER_NORMALIZE_H

#include <php.h>

PHP_FUNCTION( normalizer_normalize );
PHP_FUNCTION( normalizer_is_normalized );
#if U_ICU_VERSION_MAJOR_NUM >= 56
PHP_FUNCTION( normalizer_get_raw_decomposition );
#endif

#endif // NORMALIZER_NORMALIZE_H
