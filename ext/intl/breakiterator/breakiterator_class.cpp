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
#include <unicode/rbbi.h>
#include "codepointiterator_internal.h"

#include "breakiterator_iterators.h"

#include <typeinfo>

extern "C" {
#define USE_BREAKITERATOR_POINTER 1
#include "breakiterator_class.h"
#include "breakiterator_methods.h"
#include "rulebasedbreakiterator_methods.h"
#include "codepointiterator_methods.h"
#include <zend_exceptions.h>
#include <zend_interfaces.h>
#include <assert.h>
}

using PHP::CodePointBreakIterator;
using icu::RuleBasedBreakIterator;

/* {{{ Global variables */
zend_class_entry *BreakIterator_ce_ptr;
zend_class_entry *RuleBasedBreakIterator_ce_ptr;
zend_class_entry *CodePointBreakIterator_ce_ptr;
zend_object_handlers BreakIterator_handlers;
/* }}} */

U_CFUNC	void breakiterator_object_create(zval *object,
										 BreakIterator *biter, int brand_new)
{
	UClassID classId = biter->getDynamicClassID();
	zend_class_entry *ce;

	if (classId == RuleBasedBreakIterator::getStaticClassID()) {
		ce = RuleBasedBreakIterator_ce_ptr;
	} else if (classId == CodePointBreakIterator::getStaticClassID()) {
		ce = CodePointBreakIterator_ce_ptr;
	} else {
		ce = BreakIterator_ce_ptr;
	}

	if (brand_new) {
		object_init_ex(object, ce);
	}
	breakiterator_object_construct(object, biter);
}

U_CFUNC void breakiterator_object_construct(zval *object,
											BreakIterator *biter)
{
	BreakIterator_object *bio;

	BREAKITER_METHOD_FETCH_OBJECT_NO_CHECK; //populate to from object
	assert(bio->biter == NULL);
	bio->biter = biter;
}

/* {{{ compare handler for BreakIterator */
static int BreakIterator_compare_objects(zval *object1,
										 zval *object2)
{
	BreakIterator_object	*bio1,
							*bio2;

	bio1 = Z_INTL_BREAKITERATOR_P(object1);
	bio2 = Z_INTL_BREAKITERATOR_P(object2);

	if (bio1->biter == NULL || bio2->biter == NULL) {
		return bio1->biter == bio2->biter ? 0 : 1;
	}

	return *bio1->biter == *bio2->biter ? 0 : 1;
}
/* }}} */

/* {{{ clone handler for BreakIterator */
static zend_object *BreakIterator_clone_obj(zval *object)
{
	BreakIterator_object	*bio_orig,
							*bio_new;
	zend_object				*ret_val;

	bio_orig = Z_INTL_BREAKITERATOR_P(object);
	intl_errors_reset(INTL_DATA_ERROR_P(bio_orig));

	ret_val = BreakIterator_ce_ptr->create_object(Z_OBJCE_P(object));
	bio_new  = php_intl_breakiterator_fetch_object(ret_val);

	zend_objects_clone_members(&bio_new->zo, &bio_orig->zo);

	if (bio_orig->biter != NULL) {
		BreakIterator *new_biter;

		new_biter = bio_orig->biter->clone();
		if (!new_biter) {
			zend_string *err_msg;
			intl_errors_set_code(BREAKITER_ERROR_P(bio_orig),
				U_MEMORY_ALLOCATION_ERROR);
			intl_errors_set_custom_msg(BREAKITER_ERROR_P(bio_orig),
				"Could not clone BreakIterator", 0);
			err_msg = intl_error_get_message(BREAKITER_ERROR_P(bio_orig));
			zend_throw_exception(NULL, ZSTR_VAL(err_msg), 0);
			zend_string_free(err_msg);
		} else {
			bio_new->biter = new_biter;
			ZVAL_COPY(&bio_new->text, &bio_orig->text);
		}
	} else {
		zend_throw_exception(NULL, "Cannot clone unconstructed BreakIterator", 0);
	}

	return ret_val;
}
/* }}} */

/* {{{ get_debug_info handler for BreakIterator */
static HashTable *BreakIterator_get_debug_info(zval *object, int *is_temp)
{
	zval val;
	HashTable *debug_info;
	BreakIterator_object	*bio;
	const BreakIterator		*biter;

	*is_temp = 1;

	debug_info = zend_new_array(8);

	bio  = Z_INTL_BREAKITERATOR_P(object);
	biter = bio->biter;

	if (biter == NULL) {
		ZVAL_FALSE(&val);
		zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &val);
		return debug_info;
	}
	ZVAL_TRUE(&val);
	zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &val);

	if (Z_ISUNDEF(bio->text)) {
		ZVAL_NULL(&val);
		zend_hash_str_update(debug_info, "text", sizeof("text") - 1, &val);
	} else {
		Z_TRY_ADDREF(bio->text);
		zend_hash_str_update(debug_info, "text", sizeof("text") - 1, &bio->text);
	}

	ZVAL_STRING(&val, const_cast<char*>(typeid(*biter).name()));
	zend_hash_str_update(debug_info, "type", sizeof("type") - 1, &val);

	return debug_info;
}
/* }}} */

/* {{{ void breakiterator_object_init(BreakIterator_object* to)
 * Initialize internals of BreakIterator_object not specific to zend standard objects.
 */
static void breakiterator_object_init(BreakIterator_object *bio)
{
	intl_error_init(BREAKITER_ERROR_P(bio));
	bio->biter = NULL;
	ZVAL_UNDEF(&bio->text);
}
/* }}} */

/* {{{ BreakIterator_objects_free */
static void BreakIterator_objects_free(zend_object *object)
{
	BreakIterator_object* bio = php_intl_breakiterator_fetch_object(object);

	zval_ptr_dtor(&bio->text);
	if (bio->biter) {
		delete bio->biter;
		bio->biter = NULL;
	}
	intl_error_reset(BREAKITER_ERROR_P(bio));

	zend_object_std_dtor(&bio->zo);
}
/* }}} */

/* {{{ BreakIterator_object_create */
static zend_object *BreakIterator_object_create(zend_class_entry *ce)
{
	BreakIterator_object*	intern;

	intern = (BreakIterator_object*)ecalloc(1, sizeof(BreakIterator_object) + sizeof(zval) * (ce->default_properties_count - 1));

	zend_object_std_init(&intern->zo, ce);
    object_properties_init((zend_object*) intern, ce);
	breakiterator_object_init(intern);

	intern->zo.handlers = &BreakIterator_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ BreakIterator/RuleBasedBreakIterator methods arguments info */

ZEND_BEGIN_ARG_INFO_EX(ainfo_biter_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_biter_locale, 0, 0, 0)
	ZEND_ARG_INFO(0, locale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_biter_setText, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_biter_next, 0, 0, 0)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_biter_offset, 0, 0, 1)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_biter_get_locale, 0, 0, 1)
	ZEND_ARG_INFO(0, locale_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_biter_getPartsIterator, 0, 0, 0)
	ZEND_ARG_INFO(0, key_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_rbbi___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, rules)
	ZEND_ARG_INFO(0, areCompiled)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ BreakIterator_class_functions
 * Every 'BreakIterator' class method has an entry in this table
 */
static const zend_function_entry BreakIterator_class_functions[] = {
	PHP_ME(BreakIterator,					__construct,							ainfo_biter_void,					ZEND_ACC_PRIVATE)
	PHP_ME_MAPPING(createWordInstance,		breakiter_create_word_instance,			ainfo_biter_locale,					ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(createLineInstance,		breakiter_create_line_instance,			ainfo_biter_locale,					ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(createCharacterInstance,	breakiter_create_character_instance,	ainfo_biter_locale,					ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(createSentenceInstance,	breakiter_create_sentence_instance,		ainfo_biter_locale,					ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(createTitleInstance,		breakiter_create_title_instance,		ainfo_biter_locale,					ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(createCodePointInstance,	breakiter_create_code_point_instance,	ainfo_biter_void,					ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getText,					breakiter_get_text,						ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setText,					breakiter_set_text,						ainfo_biter_setText,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(first,					breakiter_first,						ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(last,					breakiter_last,							ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(previous,				breakiter_previous,						ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(next,					breakiter_next,							ainfo_biter_next,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(current,					breakiter_current,						ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(following,				breakiter_following,					ainfo_biter_offset,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(preceding,				breakiter_preceding,					ainfo_biter_offset,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(isBoundary,				breakiter_is_boundary,					ainfo_biter_offset,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getLocale,				breakiter_get_locale,					ainfo_biter_get_locale,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getPartsIterator,		breakiter_get_parts_iterator,			ainfo_biter_getPartsIterator,		ZEND_ACC_PUBLIC)

	PHP_ME_MAPPING(getErrorCode,			breakiter_get_error_code,				ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getErrorMessage,			breakiter_get_error_message,			ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ RuleBasedBreakIterator_class_functions
 */
static const zend_function_entry RuleBasedBreakIterator_class_functions[] = {
	PHP_ME(IntlRuleBasedBreakIterator,		__construct,							ainfo_rbbi___construct,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getRules,				rbbi_get_rules,							ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getRuleStatus,			rbbi_get_rule_status,					ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getRuleStatusVec,		rbbi_get_rule_status_vec,				ainfo_biter_void,					ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
	PHP_ME_MAPPING(getBinaryRules,			rbbi_get_binary_rules,					ainfo_biter_void,					ZEND_ACC_PUBLIC)
#endif
	PHP_FE_END
};
/* }}} */

/* {{{ CodePointBreakIterator_class_functions
 */
static const zend_function_entry CodePointBreakIterator_class_functions[] = {
	PHP_ME_MAPPING(getLastCodePoint,		cpbi_get_last_code_point,				ainfo_biter_void,					ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */


/* {{{ breakiterator_register_BreakIterator_class
 * Initialize 'BreakIterator' class
 */
U_CFUNC void breakiterator_register_BreakIterator_class(void)
{
	zend_class_entry ce;

	/* Create and register 'BreakIterator' class. */
	INIT_CLASS_ENTRY(ce, "IntlBreakIterator", BreakIterator_class_functions);
	ce.create_object = BreakIterator_object_create;
	ce.get_iterator = _breakiterator_get_iterator;
	BreakIterator_ce_ptr = zend_register_internal_class(&ce);

	memcpy(&BreakIterator_handlers, &std_object_handlers,
		sizeof BreakIterator_handlers);
	BreakIterator_handlers.offset = XtOffsetOf(BreakIterator_object, zo);
	BreakIterator_handlers.compare_objects = BreakIterator_compare_objects;
	BreakIterator_handlers.clone_obj = BreakIterator_clone_obj;
	BreakIterator_handlers.get_debug_info = BreakIterator_get_debug_info;
	BreakIterator_handlers.free_obj = BreakIterator_objects_free;

	zend_class_implements(BreakIterator_ce_ptr, 1,
			zend_ce_traversable);

	zend_declare_class_constant_long(BreakIterator_ce_ptr,
		"DONE", sizeof("DONE") - 1, BreakIterator::DONE );

	/* Declare constants that are defined in the C header */
#define BREAKITER_DECL_LONG_CONST(name) \
	zend_declare_class_constant_long(BreakIterator_ce_ptr, #name, \
		sizeof(#name) - 1, UBRK_ ## name)

	BREAKITER_DECL_LONG_CONST(WORD_NONE);
	BREAKITER_DECL_LONG_CONST(WORD_NONE_LIMIT);
	BREAKITER_DECL_LONG_CONST(WORD_NUMBER);
	BREAKITER_DECL_LONG_CONST(WORD_NUMBER_LIMIT);
	BREAKITER_DECL_LONG_CONST(WORD_LETTER);
	BREAKITER_DECL_LONG_CONST(WORD_LETTER_LIMIT);
	BREAKITER_DECL_LONG_CONST(WORD_KANA);
	BREAKITER_DECL_LONG_CONST(WORD_KANA_LIMIT);
	BREAKITER_DECL_LONG_CONST(WORD_IDEO);
	BREAKITER_DECL_LONG_CONST(WORD_IDEO_LIMIT);

	BREAKITER_DECL_LONG_CONST(LINE_SOFT);
	BREAKITER_DECL_LONG_CONST(LINE_SOFT_LIMIT);
	BREAKITER_DECL_LONG_CONST(LINE_HARD);
	BREAKITER_DECL_LONG_CONST(LINE_HARD_LIMIT);

	BREAKITER_DECL_LONG_CONST(SENTENCE_TERM);
	BREAKITER_DECL_LONG_CONST(SENTENCE_TERM_LIMIT);
	BREAKITER_DECL_LONG_CONST(SENTENCE_SEP);
	BREAKITER_DECL_LONG_CONST(SENTENCE_SEP_LIMIT);

#undef BREAKITER_DECL_LONG_CONST


	/* Create and register 'RuleBasedBreakIterator' class. */
	INIT_CLASS_ENTRY(ce, "IntlRuleBasedBreakIterator",
			RuleBasedBreakIterator_class_functions);
	RuleBasedBreakIterator_ce_ptr = zend_register_internal_class_ex(&ce,
			BreakIterator_ce_ptr);

	/* Create and register 'CodePointBreakIterator' class. */
	INIT_CLASS_ENTRY(ce, "IntlCodePointBreakIterator",
			CodePointBreakIterator_class_functions);
	CodePointBreakIterator_ce_ptr = zend_register_internal_class_ex(&ce,
			BreakIterator_ce_ptr);
}
/* }}} */
