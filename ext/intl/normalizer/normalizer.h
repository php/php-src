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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef NORMALIZER_NORMALIZER_H
#define NORMALIZER_NORMALIZER_H

#include <php.h>
#include <unicode/utypes.h>
#include <unicode/unorm2.h>

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

#endif // NORMALIZER_NORMALIZER_H
