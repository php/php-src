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

U_CFUNC PHP_METHOD(BreakIterator, __construct)
{
	zend_throw_exception( NULL,
		"An object of this type cannot be created with the new operator",
		0 TSRMLS_CC );
}

static void _breakiter_factory(const char *func_name,
							   BreakIterator *(*func)(const Locale&, UErrorCode&),
							   INTERNAL_FUNCTION_PARAMETERS)
{
	BreakIterator	*biter;
	const char		*locale_str = NULL;
	int				dummy;
	char			*msg;
	UErrorCode		status = UErrorCode();
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!",
			&locale_str, &dummy) == FAILURE) {
		spprintf(&msg, 0, "%s: bad arguments", func_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1 TSRMLS_CC);
		efree(msg);
		RETURN_NULL();
	}

	if (locale_str == NULL) {
		locale_str = intl_locale_get_default(TSRMLS_C);
	}

	biter = func(Locale::createFromName(locale_str), status);
	intl_error_set_code(NULL, status TSRMLS_CC);
	if (U_FAILURE(status)) {
		spprintf(&msg, 0, "%s: error creating BreakIterator",
				func_name);
		intl_error_set_custom_msg(NULL, msg, 1 TSRMLS_CC);
		efree(msg);
		RETURN_NULL();
	}

	breakiterator_object_create(return_value, biter TSRMLS_CC);
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
	UErrorCode status = UErrorCode();
	intl_error_reset(NULL TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_create_code_point_instance: bad arguments", 0 TSRMLS_CC);
		RETURN_NULL();
	}

	CodePointBreakIterator *cpbi = new CodePointBreakIterator();
	breakiterator_object_create(return_value, cpbi TSRMLS_CC);
}

U_CFUNC PHP_FUNCTION(breakiter_get_text)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_text: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	if (bio->text == NULL) {
		RETURN_NULL();
	} else {
		RETURN_ZVAL(bio->text, 1, 0);
	}
}

U_CFUNC PHP_FUNCTION(breakiter_set_text)
{
	char	*text;
	int		text_len;
	UText	*ut = NULL;
	zval	**textzv;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
			&text, &text_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_set_text: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	int res = zend_get_parameters_ex(1, &textzv);
	assert(res == SUCCESS);

	BREAKITER_METHOD_FETCH_OBJECT;

	/* assert it's safe to use text and text_len because zpp changes the
	 * arguments in the stack */
	assert(text == Z_STRVAL_PP(textzv));

	ut = utext_openUTF8(ut, text, text_len, BREAKITER_ERROR_CODE_P(bio));
	INTL_CTOR_CHECK_STATUS(bio, "breakiter_set_text: error opening UText");

	bio->biter->setText(ut, BREAKITER_ERROR_CODE(bio));
	utext_close(ut); /* ICU shallow clones the UText */
	INTL_CTOR_CHECK_STATUS(bio, "breakiter_set_text: error calling "
		"BreakIterator::setText()");

	/* When ICU clones the UText, it does not copy the buffer, so we have to
	 * keep the string buffer around by holding a reference to its zval. This
	 * also allows a faste implementation of getText() */
	if (bio->text != NULL) {
		zval_ptr_dtor(&bio->text);
	}
	bio->text = *textzv;
	zval_add_ref(&bio->text);

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
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1 TSRMLS_CC);
		efree(msg);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	int32_t res = (bio->biter->*func)();

	RETURN_LONG((long)res);
}

static void _breakiter_int32_ret_int32(
		const char *func_name,
		int32_t (BreakIterator::*func)(int32_t),
		INTERNAL_FUNCTION_PARAMETERS)
{
	char	*msg;
	long	arg;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &arg) == FAILURE) {
		spprintf(&msg, 0, "%s: bad arguments", func_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1 TSRMLS_CC);
		efree(msg);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	if (arg < INT32_MIN || arg > INT32_MAX) {
		spprintf(&msg, 0, "%s: offset argument is outside bounds of "
				"a 32-bit wide integer", func_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, msg, 1 TSRMLS_CC);
		efree(msg);
		RETURN_FALSE;
	}

	int32_t res = (bio->biter->*func)((int32_t)arg);

	RETURN_LONG((long)res);
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
		zval **arg;
		int res = zend_get_parameters_ex(1, &arg);
		assert(res == SUCCESS);
		if (Z_TYPE_PP(arg) == IS_NULL) {
			no_arg_version = true;
			ht = 0; /* pretend we don't have any argument */
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
				"breakiter_current: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	int32_t res = bio->biter->current();

	RETURN_LONG((long)res);
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
	long offset;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
			&offset) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"breakiter_is_boundary: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (offset < INT32_MIN || offset > INT32_MAX) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"breakiter_is_boundary: offset argument is outside bounds of "
				"a 32-bit wide integer", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	UBool res = bio->biter->isBoundary((int32_t)offset);

	RETURN_BOOL((long)res);
}

U_CFUNC PHP_FUNCTION(breakiter_get_locale)
{
	long	locale_type;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &locale_type) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_locale: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (locale_type != ULOC_ACTUAL_LOCALE && locale_type != ULOC_VALID_LOCALE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_locale: invalid locale type", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	Locale locale = bio->biter->getLocale((ULocDataLocaleType)locale_type,
		BREAKITER_ERROR_CODE(bio));
	INTL_METHOD_CHECK_STATUS(bio,
		"breakiter_get_locale: Call to ICU method has failed");

	RETURN_STRING(locale.getName(), 1);
}

U_CFUNC PHP_FUNCTION(breakiter_get_parts_iterator)
{
	long key_type = 0;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &key_type) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_parts_iterator: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	if (key_type != PARTS_ITERATOR_KEY_SEQUENTIAL
			&& key_type != PARTS_ITERATOR_KEY_LEFT
			&& key_type != PARTS_ITERATOR_KEY_RIGHT) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_parts_iterator: bad key type", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	IntlIterator_from_BreakIterator_parts(
		object, return_value, (parts_iter_key_type)key_type TSRMLS_CC);
}

U_CFUNC PHP_FUNCTION(breakiter_get_error_code)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_error_code: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	/* Fetch the object (without resetting its last error code ). */
	bio = (BreakIterator_object*)zend_object_store_get_object(object TSRMLS_CC);
	if (bio == NULL)
		RETURN_FALSE;

	RETURN_LONG((long)BREAKITER_ERROR_CODE(bio));
}

U_CFUNC PHP_FUNCTION(breakiter_get_error_message)
{
	const char* message = NULL;
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"breakiter_get_error_message: bad arguments", 0 TSRMLS_CC );
		RETURN_FALSE;
	}


	/* Fetch the object (without resetting its last error code ). */
	bio = (BreakIterator_object*)zend_object_store_get_object(object TSRMLS_CC);
	if (bio == NULL)
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message(BREAKITER_ERROR_P(bio) TSRMLS_CC);
	RETURN_STRING(message, 0);
}
