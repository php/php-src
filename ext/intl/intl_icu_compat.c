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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <php.h>

#include "intl_icu_compat.h"
#include <unicode/ubrk.h>
#include <unicode/ucnv.h>
#include <unicode/uspoof.h>
#include <unicode/uset.h>

UConverter *intl_icu_compat_ucnv_clone(const UConverter *converter, UErrorCode *status)
{
#if INTL_ICU_HAS_UCNV_CLONE
	return ucnv_clone(converter, status);
#else
	return ucnv_safeClone(converter, NULL, NULL, status);
#endif
}

UBreakIterator *intl_icu_compat_ubrk_clone(const UBreakIterator *break_iterator, void *stack_buffer, UErrorCode *status)
{
#if INTL_ICU_HAS_UBRK_CLONE
	return ubrk_clone(break_iterator, status);
#else
	int32_t buffer_size = U_BRK_SAFECLONE_BUFFERSIZE;

	return ubrk_safeClone(break_iterator, stack_buffer, &buffer_size, status);
#endif
}

void intl_icu_compat_uspoof_init_checker(USpoofChecker *checker, IntlIcuSpoofCheckResult **check_result, UErrorCode *status)
{
#if INTL_ICU_HAS_SPOOFCHECKER_CHECK_RESULT
	/* ICU 58 removed WSC/MSC handling in favor of TR39 restriction levels.
	 * Keep the default highly restrictive behavior and allocate the extended
	 * check result used by uspoof_check2*(). */
	uspoof_setRestrictionLevel(checker, USPOOF_HIGHLY_RESTRICTIVE);
	*check_result = uspoof_openCheckResult(status);
#else
	int checks = uspoof_getChecks(checker, status);

	/* Single-script enforcement rejects legitimate mixed-script languages like Japanese. */
	uspoof_setChecks(checker, checks & ~USPOOF_SINGLE_SCRIPT, status);
	*check_result = NULL;
#endif
}

void intl_icu_compat_uspoof_close_check_result(IntlIcuSpoofCheckResult *check_result)
{
#if INTL_ICU_HAS_SPOOFCHECKER_CHECK_RESULT
	if (check_result) {
		uspoof_closeCheckResult(check_result);
	}
#else
	(void) check_result;
#endif
}

int32_t intl_icu_compat_uspoof_check_utf8(const USpoofChecker *checker, const char *text, int32_t length, IntlIcuSpoofCheckResult *check_result, UErrorCode *status)
{
#if INTL_ICU_HAS_SPOOFCHECKER_CHECK_RESULT
	return uspoof_check2UTF8(checker, text, length, check_result, status);
#else
	(void) check_result;
	return uspoof_checkUTF8(checker, text, length, NULL, status);
#endif
}

UBool intl_icu_compat_uspoof_check_result_mismatch(IntlIcuSpoofCheckResult *check_result, int32_t checks, int32_t *result_checks, UErrorCode *status)
{
#if INTL_ICU_HAS_SPOOFCHECKER_CHECK_RESULT
	*result_checks = uspoof_getCheckResultChecks(check_result, status);
	return *result_checks != checks;
#else
	(void) check_result;
	(void) status;
	*result_checks = checks;
	return 0;
#endif
}

UBool intl_icu_compat_uspoof_is_allowed_chars_pattern_option(int64_t pattern_option)
{
	return pattern_option == 0
		|| pattern_option == USET_IGNORE_SPACE
#if INTL_ICU_HAS_USET_SIMPLE_CASE_INSENSITIVE
		|| pattern_option == (USET_IGNORE_SPACE|USET_SIMPLE_CASE_INSENSITIVE)
#endif
		|| pattern_option == (USET_IGNORE_SPACE|USET_CASE_INSENSITIVE)
		|| pattern_option == (USET_IGNORE_SPACE|USET_ADD_CASE_MAPPINGS);
}

const char *intl_icu_compat_uspoof_allowed_chars_pattern_option_error_message(void)
{
#if INTL_ICU_HAS_USET_SIMPLE_CASE_INSENSITIVE
	return "must be a valid pattern option, 0 or (SpoofChecker::IGNORE_SPACE|(<none> or SpoofChecker::CASE_INSENSITIVE or SpoofChecker::ADD_CASE_MAPPINGS or SpoofChecker::SIMPLE_CASE_INSENSITIVE))";
#else
	return "must be a valid pattern option, 0 or (SpoofChecker::IGNORE_SPACE|(<none> or SpoofChecker::CASE_INSENSITIVE or SpoofChecker::ADD_CASE_MAPPINGS))";
#endif
}
