/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

static inline RuleBasedBreakIterator *fetch_rbbi(BreakIterator_object *bio) {
	return (RuleBasedBreakIterator*)bio->biter;
}

static void _php_intlrbbi_constructor_body(INTERNAL_FUNCTION_PARAMETERS)
{
	zval		*object		= getThis();
	char		*rules;
	int			rules_len;
	zend_bool	compiled	= 0;
	UErrorCode	status		= U_ZERO_ERROR;
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b",
			&rules, &rules_len, &compiled) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"rbbi_create_instance: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	// instantiation of ICU object
	RuleBasedBreakIterator *rbbi;

	if (!compiled) {
		UnicodeString	rulesStr;
		UParseError		parseError = UParseError();
		if (intl_stringFromChar(rulesStr, rules, rules_len, &status)
				== FAILURE) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"rbbi_create_instance: rules were not a valid UTF-8 string",
				0 TSRMLS_CC);
			RETURN_NULL();
		}

		rbbi = new RuleBasedBreakIterator(rulesStr, parseError, status);
		intl_error_set_code(NULL, status TSRMLS_CC);
		if (U_FAILURE(status)) {
			char *msg;
			smart_str parse_error_str;
			parse_error_str = intl_parse_error_to_string(&parseError);
			spprintf(&msg, 0, "rbbi_create_instance: unable to create "
				"RuleBasedBreakIterator from rules (%s)", parse_error_str.c);
			smart_str_free(&parse_error_str);
			intl_error_set_custom_msg(NULL, msg, 1 TSRMLS_CC);
			efree(msg);
			delete rbbi;
			RETURN_NULL();
		}
	} else { // compiled
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
		rbbi = new RuleBasedBreakIterator((uint8_t*)rules, rules_len, status);
		if (U_FAILURE(status)) {
			intl_error_set(NULL, status, "rbbi_create_instance: unable to "
				"create instance from compiled rules", 0 TSRMLS_CC);
			delete rbbi;
			RETURN_NULL();
		}
#else
		intl_error_set(NULL, U_UNSUPPORTED_ERROR, "rbbi_create_instance: "
			"compiled rules require ICU >= 4.8", 0 TSRMLS_CC);
		RETURN_NULL();
#endif
	}

	breakiterator_object_create(return_value, rbbi TSRMLS_CC);
}

U_CFUNC PHP_METHOD(IntlRuleBasedBreakIterator, __construct)
{
	zval	orig_this		= *getThis();

	return_value = getThis();
	//changes this to IS_NULL (without first destroying) if there's an error
	_php_intlrbbi_constructor_body(INTERNAL_FUNCTION_PARAM_PASSTHRU);

	if (Z_TYPE_P(return_value) == IS_NULL) {
		zend_object_store_ctor_failed(&orig_this TSRMLS_CC);
		zval_dtor(&orig_this);
	}
}

U_CFUNC PHP_FUNCTION(rbbi_get_rules)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"rbbi_get_rules: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	const UnicodeString rules = fetch_rbbi(bio)->getRules();

	Z_TYPE_P(return_value) = IS_STRING;
	if (intl_charFromString(rules, &Z_STRVAL_P(return_value),
			&Z_STRLEN_P(return_value), BREAKITER_ERROR_CODE_P(bio)) == FAILURE)
	{
		intl_errors_set(BREAKITER_ERROR_P(bio), BREAKITER_ERROR_CODE(bio),
				"rbbi_hash_code: Error converting result to UTF-8 string",
				0 TSRMLS_CC);
		RETURN_FALSE;
	}
}

U_CFUNC PHP_FUNCTION(rbbi_get_rule_status)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"rbbi_get_rule_status: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	RETURN_LONG(fetch_rbbi(bio)->getRuleStatus());
}

U_CFUNC PHP_FUNCTION(rbbi_get_rule_status_vec)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"rbbi_get_rule_status_vec: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	int32_t num_rules = fetch_rbbi(bio)->getRuleStatusVec(NULL, 0,
			BREAKITER_ERROR_CODE(bio));
	if (BREAKITER_ERROR_CODE(bio) == U_BUFFER_OVERFLOW_ERROR) {
		BREAKITER_ERROR_CODE(bio) = U_ZERO_ERROR;
	} else {
		// should not happen
		INTL_METHOD_CHECK_STATUS(bio, "rbbi_get_rule_status_vec: failed "
				" determining the number of status values");
	}
	int32_t *rules = new int32_t[num_rules];
	num_rules = fetch_rbbi(bio)->getRuleStatusVec(rules, num_rules,
			BREAKITER_ERROR_CODE(bio));
	if (U_FAILURE(BREAKITER_ERROR_CODE(bio))) {
		delete[] rules;
		intl_errors_set(BREAKITER_ERROR_P(bio), BREAKITER_ERROR_CODE(bio),
				"rbbi_get_rule_status_vec: failed obtaining the status values",
				0 TSRMLS_CC);
		RETURN_FALSE;
	}

	array_init_size(return_value, num_rules);
	for (int32_t i = 0; i < num_rules; i++) {
		add_next_index_long(return_value, rules[i]);
	}
	delete[] rules;
}

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
U_CFUNC PHP_FUNCTION(rbbi_get_binary_rules)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"rbbi_get_binary_rules: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	uint32_t		rules_len;
	const uint8_t	*rules = fetch_rbbi(bio)->getBinaryRules(rules_len);

	if (rules_len > INT_MAX - 1) {
		intl_errors_set(BREAKITER_ERROR_P(bio), BREAKITER_ERROR_CODE(bio),
				"rbbi_get_binary_rules: the rules are too large",
				0 TSRMLS_CC);
		RETURN_FALSE;
	}

	char *ret_rules = static_cast<char*>(emalloc(rules_len + 1));
	memcpy(ret_rules, rules, rules_len);
	ret_rules[rules_len] = '\0';

	RETURN_STRINGL(ret_rules, rules_len, 0);
}
#endif
