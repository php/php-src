/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Weilin Du <weilindu@php.net>                                |
   +----------------------------------------------------------------------+
 */

#ifndef INTL_ICU_COMPAT_H
#define INTL_ICU_COMPAT_H

#include <unicode/ubrk.h>
#include <unicode/ucnv.h>
#include <unicode/uspoof.h>
#include <unicode/utypes.h>
#include <unicode/uversion.h>

#define INTL_ICU_VERSION_AT_LEAST(major, minor) \
	(U_ICU_VERSION_MAJOR_NUM > (major) || \
		(U_ICU_VERSION_MAJOR_NUM == (major) && U_ICU_VERSION_MINOR_NUM >= (minor)))

#define INTL_ICU_HAS_UBRK_CLONE INTL_ICU_VERSION_AT_LEAST(69, 0)
#define INTL_ICU_HAS_UCNV_CLONE INTL_ICU_VERSION_AT_LEAST(71, 0)
#define INTL_ICU_HAS_SPOOFCHECKER_CHECK_RESULT INTL_ICU_VERSION_AT_LEAST(58, 0)
#define INTL_ICU_HAS_USET_SIMPLE_CASE_INSENSITIVE INTL_ICU_VERSION_AT_LEAST(73, 0)

#if INTL_ICU_HAS_SPOOFCHECKER_CHECK_RESULT
typedef USpoofCheckResult IntlIcuSpoofCheckResult;
#else
typedef void IntlIcuSpoofCheckResult;
#endif

#ifdef __cplusplus
extern "C" {
#endif

UConverter *intl_icu_compat_ucnv_clone(const UConverter *converter, UErrorCode *status);
UBreakIterator *intl_icu_compat_ubrk_clone(const UBreakIterator *break_iterator, void *stack_buffer, UErrorCode *status);
void intl_icu_compat_uspoof_init_checker(USpoofChecker *checker, IntlIcuSpoofCheckResult **check_result, UErrorCode *status);
void intl_icu_compat_uspoof_close_check_result(IntlIcuSpoofCheckResult *check_result);
int32_t intl_icu_compat_uspoof_check_utf8(const USpoofChecker *checker, const char *text, int32_t length, IntlIcuSpoofCheckResult *check_result, UErrorCode *status);
UBool intl_icu_compat_uspoof_check_result_mismatch(IntlIcuSpoofCheckResult *check_result, int32_t checks, int32_t *result_checks, UErrorCode *status);
UBool intl_icu_compat_uspoof_is_allowed_chars_pattern_option(int64_t pattern_option);
const char *intl_icu_compat_uspoof_allowed_chars_pattern_option_error_message(void);

#ifdef __cplusplus
}
#endif

#endif
