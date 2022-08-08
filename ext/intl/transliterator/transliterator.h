/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@netcabo.ot>                       |
   +----------------------------------------------------------------------+
 */

#ifndef TRANSLITERATOR_TRANSLITERATOR_H
#define TRANSLITERATOR_TRANSLITERATOR_H

#include <php.h>
#include <unicode/utrans.h>

#include "zend_smart_str.h"

smart_str transliterator_parse_error_to_string( UParseError* pe );

#endif /* #ifndef TRANSLITERATOR_TRANSLITERATOR_H */
