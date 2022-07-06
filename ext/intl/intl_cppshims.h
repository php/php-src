/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
