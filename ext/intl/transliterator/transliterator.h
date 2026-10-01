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
   | Authors: Gustavo Lopes <cataphract@netcabo.ot>                       |
   +----------------------------------------------------------------------+
 */

#ifndef TRANSLITERATOR_TRANSLITERATOR_H
#define TRANSLITERATOR_TRANSLITERATOR_H

#include <php.h>
#include <unicode/utrans.h>

smart_str transliterator_parse_error_to_string( UParseError* pe );

#endif /* #ifndef TRANSLITERATOR_TRANSLITERATOR_H */
