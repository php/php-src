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
   | Authors: Jim Winstead <jimw@php.net>                                 |
   |          Stig Sæther Bakken <ssb@php.net>                            |
   +----------------------------------------------------------------------+
*/

#include "php.h"

/* Define rounding modes (all are round-to-nearest) */
#ifndef PHP_ROUND_HALF_UP
#define PHP_ROUND_HALF_UP        0x01    /* Arithmetic rounding, up == away from zero */
#endif

#ifndef PHP_ROUND_HALF_DOWN
#define PHP_ROUND_HALF_DOWN      0x02    /* Arithmetic rounding, down == towards zero */
#endif

#ifndef PHP_ROUND_HALF_EVEN
#define PHP_ROUND_HALF_EVEN      0x03    /* Banker's rounding */
#endif

#ifndef PHP_ROUND_HALF_ODD
#define PHP_ROUND_HALF_ODD       0x04
#endif

#ifndef PHP_ROUND_CEILING
#define PHP_ROUND_CEILING        0x05
#endif

#ifndef PHP_ROUND_FLOOR
#define PHP_ROUND_FLOOR          0x06
#endif

#ifndef PHP_ROUND_TOWARD_ZERO
#define PHP_ROUND_TOWARD_ZERO    0x07
#endif

#ifndef PHP_ROUND_AWAY_FROM_ZERO
#define PHP_ROUND_AWAY_FROM_ZERO 0x08
#endif

extern PHPAPI zend_class_entry *rounding_mode_ce;

PHPAPI int php_math_round_mode_from_enum(zend_object *mode);
