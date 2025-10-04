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

#include <type_traits>

template<typename T, typename S = std::size_t, typename std::enable_if<std::is_integral<S>::value && std::is_unsigned<S>::value, int>::type = 0>
T zend_mm_fast_alloc(S len) {
	return static_cast<T>(emalloc(len));
}

// we try to provide enough flexibility for nm and size types as long they re unsigned together
template<typename T, typename N, typename S,
         typename std::enable_if<std::is_integral<N>::value && std::is_unsigned<N>::value && std::is_integral<S>::value && std::is_unsigned<S>::value, int>::type = 0>
T zend_mm_safe_alloc(N num, S len, S offset = 0) {
    return static_cast<T>(safe_emalloc(num, len, offset));
}

template<typename T, typename std::enable_if<std::is_class<T>::value, int>::type = 0>
void zend_mm_destructor(T *inst) {
	if (inst) {
		inst->~T();
		efree(inst);
	}
}

template<typename T, typename std::enable_if<!std::is_class<T>::value, int>::type = 0>
void zend_mm_destructor(T *ptr) {
	if (ptr) {
		efree(ptr);
	}
}
#endif
