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

#include <unicode/rbbi.h>

extern "C" {
#define USE_BREAKITERATOR_POINTER 1
#include "breakiterator_class.h"
#include <zend_exceptions.h>
#include <limits.h>
}

#include "../intl_convertcpp.h"
#include "../intl_common.h"

using icu::RuleBasedBreakIterator;
using icu::Locale;

static inline RuleBasedBreakIterator *fetch_rbbi(BreakIterator_object *bio) {
	return (RuleBasedBreakIterator*)bio->biter;
}

static void _php_intlrbbi_constructor_body(INTERNAL_FUNCTION_PARAMETERS)
{
	char		*rules;
	size_t		rules_len;
	zend_bool	compiled	= 0;
	UErrorCode	status		= U_ZERO_ERROR;
	BREAKITER_METHOD_INIT_VARS;
	object = ZEND_THIS;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|b",
			&rules, &rules_len, &compiled) == FAILURE) {
		RETURN_THROWS();
	}

	BREAKITER_METHOD_FETCH_OBJECT_NO_CHECK;
	if (bio->biter) {
		zend_throw_error(NULL, "IntlRuleBasedBreakIterator object is already constructed");
		RETURN_THROWS();
	}

	// instantiation of ICU object
	RuleBasedBreakIterator *rbbi;

	if (!compiled) {
		UnicodeString	rulesStr;
		UParseError		parseError = UParseError();
		if (intl_stringFromChar(rulesStr, rules, rules_len, &status)
				== FAILURE) {
			zend_throw_exception(IntlException_ce_ptr,
				"IntlRuleBasedBreakIterator::__construct(): "
				"rules were not a valid UTF-8 string", 0);
			RETURN_THROWS();
		}

		rbbi = new RuleBasedBreakIterator(rulesStr, parseError, status);
		intl_error_set_code(NULL, status);
		if (U_FAILURE(status)) {
			smart_str parse_error_str;
			parse_error_str = intl_parse_error_to_string(&parseError);
			zend_throw_exception_ex(IntlException_ce_ptr, 0,
				"IntlRuleBasedBreakIterator::__construct(): "
				"unable to create RuleBasedBreakIterator from rules (%s)",
				parse_error_str.s ? ZSTR_VAL(parse_error_str.s) : "");
			smart_str_free(&parse_error_str);
			delete rbbi;
			RETURN_THROWS();
		}
	} else { // compiled
		rbbi = new RuleBasedBreakIterator((uint8_t*)rules, rules_len, status);
		if (U_FAILURE(status)) {
			zend_throw_exception(IntlException_ce_ptr,
				"IntlRuleBasedBreakIterator::__construct(): "
				"unable to create instance from compiled rules", 0);
			delete rbbi;
			RETURN_THROWS();
		}
	}

	breakiterator_object_create(return_value, rbbi, 0);
}

U_CFUNC PHP_METHOD(IntlRuleBasedBreakIterator, __construct)
{
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, IntlException_ce_ptr, &error_handling);
	return_value = ZEND_THIS;
	_php_intlrbbi_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	zend_restore_error_handling(&error_handling);
}

U_CFUNC PHP_METHOD(IntlRuleBasedBreakIterator, getRules)
{
	BREAKITER_METHOD_INIT_VARS;
	object = ZEND_THIS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	zend_string *u8str;
	const UnicodeString rules = fetch_rbbi(bio)->getRules();

	u8str = intl_charFromString(rules, BREAKITER_ERROR_CODE_P(bio));
	if (!u8str)
	{
		intl_errors_set(BREAKITER_ERROR_P(bio), BREAKITER_ERROR_CODE(bio),
				"rbbi_hash_code: Error converting result to UTF-8 string",
				0);
		RETURN_FALSE;
	}
	RETVAL_STR(u8str);
}

U_CFUNC PHP_METHOD(IntlRuleBasedBreakIterator, getRuleStatus)
{
	BREAKITER_METHOD_INIT_VARS;
	object = ZEND_THIS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	RETURN_LONG(fetch_rbbi(bio)->getRuleStatus());
}

U_CFUNC PHP_METHOD(IntlRuleBasedBreakIterator, getRuleStatusVec)
{
	BREAKITER_METHOD_INIT_VARS;
	object = ZEND_THIS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	int32_t num_rules = fetch_rbbi(bio)->getRuleStatusVec(NULL, 0,
			BREAKITER_ERROR_CODE(bio));

	ZEND_ASSERT(BREAKITER_ERROR_CODE(bio) == U_BUFFER_OVERFLOW_ERROR);
	BREAKITER_ERROR_CODE(bio) = U_ZERO_ERROR;

	int32_t *rules = new int32_t[num_rules];
	num_rules = fetch_rbbi(bio)->getRuleStatusVec(rules, num_rules,
			BREAKITER_ERROR_CODE(bio));
	if (U_FAILURE(BREAKITER_ERROR_CODE(bio))) {
		delete[] rules;
		intl_errors_set(BREAKITER_ERROR_P(bio), BREAKITER_ERROR_CODE(bio),
				"rbbi_get_rule_status_vec: failed obtaining the status values",
				0);
		RETURN_FALSE;
	}

	array_init_size(return_value, num_rules);
	for (int32_t i = 0; i < num_rules; i++) {
		add_next_index_long(return_value, rules[i]);
	}
	delete[] rules;
}

U_CFUNC PHP_METHOD(IntlRuleBasedBreakIterator, getBinaryRules)
{
	BREAKITER_METHOD_INIT_VARS;
	object = ZEND_THIS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	uint32_t		rules_len;
	const uint8_t	*rules = fetch_rbbi(bio)->getBinaryRules(rules_len);

	if (rules_len > INT_MAX - 1) {
		intl_errors_set(BREAKITER_ERROR_P(bio), BREAKITER_ERROR_CODE(bio),
				"rbbi_get_binary_rules: the rules are too large",
				0);
		RETURN_FALSE;
	}

	zend_string *ret_rules = zend_string_alloc(rules_len, 0);
	memcpy(ZSTR_VAL(ret_rules), rules, rules_len);
	ZSTR_VAL(ret_rules)[rules_len] = '\0';

	RETURN_STR(ret_rules);
}
