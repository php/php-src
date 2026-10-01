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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef INTL_CPPSHIMS_H
#define INTL_CPPSHIMS_H

#ifndef __cplusplus
#error For inclusion form C++ files only
#endif

#ifdef _MSC_VER
//This is only required for old versions of ICU only
#include <stdio.h>

#include <math.h>

/* avoid redefinition of int8_t, also defined in unicode/pwin32.h */
#define _MSC_STDINT_H_ 1
#endif

#endif
