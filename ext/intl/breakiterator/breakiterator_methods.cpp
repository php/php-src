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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unicode/brkiter.h>
#include "codepointiterator_internal.h"

#include "breakiterator_iterators.h"

extern "C" {
#include "../php_intl.h"
#define USE_BREAKITERATOR_POINTER 1
#include "breakiterator_class.h"
#include "../locale/locale.h"
#include <zend_exceptions.h>
}

using PHP::CodePointBreakIterator;
using icu::BreakIterator;
using icu::Locale;

U_CFUNC PHP_METHOD(BreakIterator, __construct)
{
	zend_throw_exception( NULL,
		"An object of this type cannot be created with the new operator",
		0 );
}

static void _breakiter_factory(const char *func_name,
							   BreakIterator *(*func)(const Locale&, UErrorCode&),
							   INTERNAL_FUNCTION_PARAMETERS)
{
	BreakIterator	*biter;
	const char		*locale_str = NULL;
	size_t				dummy;
	char			*msg;
	UErrorCode		status = UErrorCode();
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!",
			&locale_str, &dummy) == FAILURE) {
		spprintf(&msg, 0, "%s: bad arguments", func_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1);
		efree(msg);
		RETURN_NULL();
	}

	if (locale_str == NULL) {
		locale_str = intl_locale_get_default();
	}

	biter = func(Locale::createFromName(locale_str), status);
	intl_error_set_code(NULL, status);
	if (U_FAILURE(status)) {
		spprintf(&msg, 0, "%s: error creating BreakIterator",
				func_name);
		intl_error_set_custom_msg(NULL, msg, 1);
		efree(msg);
		RETURN_NULL();
	}

	breakiterator_object_create(return_value, biter, 1);
}

U_CFUNC PHP_FUNCTION(breakiter_create_word_instance)
{
	_breakiter_factory("breakiter_create_word_instance",
			&BreakIterator::createWordInstance,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_create_line_instance)
{
	_breakiter_factory("breakiter_create_line_instance",
			&BreakIterator::createLineInstance,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_create_character_instance)
{
	_breakiter_factory("breakiter_create_character_instance",
			&BreakIterator::createCharacterInstance,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_create_sentence_instance)
{
	_breakiter_factory("breakiter_create_sentence_instance",
			&BreakIterator::createSentenceInstance,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_create_title_instance)
{
	_breakiter_factory("breakiter_create_title_instance",
			&BreakIterator::createTitleInstance,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_create_code_point_instance)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_create_code_point_instance: bad arguments", 0);
		RETURN_NULL();
	}

	CodePointBreakIterator *cpbi = new CodePointBreakIterator();
	breakiterator_object_create(return_value, cpbi, 1);
}

U_CFUNC PHP_FUNCTION(breakiter_get_text)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_text: bad arguments", 0);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	if (Z_ISUNDEF(bio->text)) {
		RETURN_NULL();
	} else {
		ZVAL_COPY(return_value, &bio->text);
	}
}

U_CFUNC PHP_FUNCTION(breakiter_set_text)
{
	UText	*ut = NULL;
	zend_string	*text;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_set_text: bad arguments", 0);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	ut = utext_openUTF8(ut, ZSTR_VAL(text), ZSTR_LEN(text), BREAKITER_ERROR_CODE_P(bio));
	INTL_METHOD_CHECK_STATUS_OR_NULL(bio, "breakiter_set_text: error opening UText");

	bio->biter->setText(ut, BREAKITER_ERROR_CODE(bio));
	utext_close(ut); /* ICU shallow clones the UText */
	INTL_METHOD_CHECK_STATUS_OR_NULL(bio, "breakiter_set_text: error calling "
		"BreakIterator::setText()");

	/* When ICU clones the UText, it does not copy the buffer, so we have to
	 * keep the string buffer around by holding a reference to its zval. This
	 * also allows a faste implementation of getText() */
	zval_ptr_dtor(&bio->text);
	ZVAL_STR_COPY(&bio->text, text);

	RETURN_TRUE;
}

static void _breakiter_no_args_ret_int32(
		const char *func_name,
		int32_t (BreakIterator::*func)(),
		INTERNAL_FUNCTION_PARAMETERS)
{
	char	*msg;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		spprintf(&msg, 0, "%s: bad arguments", func_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1);
		efree(msg);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	int32_t res = (bio->biter->*func)();

	RETURN_LONG((zend_long)res);
}

static void _breakiter_int32_ret_int32(
		const char *func_name,
		int32_t (BreakIterator::*func)(int32_t),
		INTERNAL_FUNCTION_PARAMETERS)
{
	char	*msg;
	zend_long	arg;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &arg) == FAILURE) {
		spprintf(&msg, 0, "%s: bad arguments", func_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1);
		efree(msg);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	if (arg < INT32_MIN || arg > INT32_MAX) {
		spprintf(&msg, 0, "%s: offset argument is outside bounds of "
				"a 32-bit wide integer", func_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1);
		efree(msg);
		RETURN_FALSE;
	}

	int32_t res = (bio->biter->*func)((int32_t)arg);

	RETURN_LONG((zend_long)res);
}

U_CFUNC PHP_FUNCTION(breakiter_first)
{
	_breakiter_no_args_ret_int32("breakiter_first",
			&BreakIterator::first,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_last)
{
	_breakiter_no_args_ret_int32("breakiter_last",
			&BreakIterator::last,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_previous)
{
	_breakiter_no_args_ret_int32("breakiter_previous",
			&BreakIterator::previous,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_next)
{
	bool no_arg_version = false;

	if (ZEND_NUM_ARGS() == 0) {
		no_arg_version = true;
	} else if (ZEND_NUM_ARGS() == 1) {
		zval *arg;
		int res = zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg);
		assert(res == SUCCESS);
		if (Z_TYPE_P(arg) == IS_NULL) {
			no_arg_version = true;
			ZEND_NUM_ARGS() = 0; /* pretend we don't have any argument */
		} else {
			no_arg_version = false;
		}
	}

	if (no_arg_version) {
		_breakiter_no_args_ret_int32("breakiter_next",
				&BreakIterator::next,
				INTERNAL_FUNCTION_PARAM_PASSTHRU);
	} else {
		_breakiter_int32_ret_int32("breakiter_next",
				&BreakIterator::next,
				INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}

U_CFUNC PHP_FUNCTION(breakiter_current)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"breakiter_current: bad arguments", 0);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	int32_t res = bio->biter->current();

	RETURN_LONG((zend_long)res);
}

U_CFUNC PHP_FUNCTION(breakiter_following)
{
	_breakiter_int32_ret_int32("breakiter_following",
			&BreakIterator::following,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_preceding)
{
	_breakiter_int32_ret_int32("breakiter_preceding",
			&BreakIterator::preceding,
			INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(breakiter_is_boundary)
{
	zend_long offset;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l",
			&offset) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"breakiter_is_boundary: bad arguments", 0);
		RETURN_FALSE;
	}

	if (offset < INT32_MIN || offset > INT32_MAX) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"breakiter_is_boundary: offset argument is outside bounds of "
				"a 32-bit wide integer", 0);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	UBool res = bio->biter->isBoundary((int32_t)offset);

	RETURN_BOOL((zend_long)res);
}

U_CFUNC PHP_FUNCTION(breakiter_get_locale)
{
	zend_long	locale_type;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &locale_type) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_locale: bad arguments", 0);
		RETURN_FALSE;
	}

	if (locale_type != ULOC_ACTUAL_LOCALE && locale_type != ULOC_VALID_LOCALE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_locale: invalid locale type", 0);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	Locale locale = bio->biter->getLocale((ULocDataLocaleType)locale_type,
		BREAKITER_ERROR_CODE(bio));
	INTL_METHOD_CHECK_STATUS(bio,
		"breakiter_get_locale: Call to ICU method has failed");

	RETURN_STRING(locale.getName());
}

U_CFUNC PHP_FUNCTION(breakiter_get_parts_iterator)
{
	zend_long key_type = 0;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &key_type) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_parts_iterator: bad arguments", 0);
		RETURN_FALSE;
	}

	if (key_type != PARTS_ITERATOR_KEY_SEQUENTIAL
			&& key_type != PARTS_ITERATOR_KEY_LEFT
			&& key_type != PARTS_ITERATOR_KEY_RIGHT) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_parts_iterator: bad key type", 0);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	IntlIterator_from_BreakIterator_parts(
		object, return_value, (parts_iter_key_type)key_type);
}

U_CFUNC PHP_FUNCTION(breakiter_get_error_code)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_error_code: bad arguments", 0);
		RETURN_FALSE;
	}

	/* Fetch the object (without resetting its last error code ). */
	bio = Z_INTL_BREAKITERATOR_P(object);
	if (bio == NULL)
		RETURN_FALSE;

	RETURN_LONG((zend_long)BREAKITER_ERROR_CODE(bio));
}

U_CFUNC PHP_FUNCTION(breakiter_get_error_message)
{
	zend_string* message = NULL;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_error_message: bad arguments", 0 );
		RETURN_FALSE;
	}


	/* Fetch the object (without resetting its last error code ). */
	bio = Z_INTL_BREAKITERATOR_P(object);
	if (bio == NULL)
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message(BREAKITER_ERROR_P(bio));
	RETURN_STR(message);
}
