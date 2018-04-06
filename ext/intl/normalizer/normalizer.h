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

#ifndef NORMALIZER_NORMALIZER_H
#define NORMALIZER_NORMALIZER_H

#include <php.h>
#include <unicode/utypes.h>
#if U_ICU_VERSION_MAJOR_NUM < 56
#include <unicode/unorm.h>

#define NORMALIZER_NONE UNORM_NONE
#define NORMALIZER_FORM_D UNORM_NFD
#define NORMALIZER_NFD UNORM_NFD
#define NORMALIZER_FORM_KD UNORM_NFKD
#define NORMALIZER_NFKD UNORM_NFKD
#define NORMALIZER_FORM_C UNORM_NFC
#define NORMALIZER_NFC UNORM_NFC
#define NORMALIZER_FORM_KC UNORM_NFKC
#define NORMALIZER_NFKC UNORM_NFKC
#define NORMALIZER_DEFAULT UNORM_DEFAULT
#else
#include <unicode/unorm2.h>

#define NORMALIZER_NONE 0x2
#define NORMALIZER_FORM_D 0x4
#define NORMALIZER_NFD NORMALIZER_FORM_D
#define NORMALIZER_FORM_KD 0x8
#define NORMALIZER_NFKD NORMALIZER_FORM_KD
#define NORMALIZER_FORM_C 0x10
#define NORMALIZER_NFC NORMALIZER_FORM_C
#define NORMALIZER_FORM_KC 0x20
#define NORMALIZER_NFKC NORMALIZER_FORM_KC
#define NORMALIZER_FORM_KC_CF 0x30
#define NORMALIZER_NFKC_CF NORMALIZER_FORM_KC_CF
#define NORMALIZER_DEFAULT NORMALIZER_FORM_C
#endif

void normalizer_register_constants( INIT_FUNC_ARGS );

#endif // NORMALIZER_NORMALIZER_H
