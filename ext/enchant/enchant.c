/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Pierre-Alain Joye <paj@pearfr.org>                           |
  |         Ilia Alshanetsky <ilia@prohost.org>                          |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_exceptions.h"
#include "../spl/spl_exceptions.h"
#include <enchant.h>
#include "php_enchant.h"

#define PHP_ENCHANT_MYSPELL 1
#define PHP_ENCHANT_ISPELL 2
#ifdef HAVE_ENCHANT_GET_VERSION
#define PHP_ENCHANT_GET_VERSION enchant_get_version()
#endif

#include "enchant_arginfo.h"

typedef struct _broker_struct {
	EnchantBroker  *pbroker;
	int             nb_dict;
	zend_object     std;
} enchant_broker;

typedef struct _dict_struct {
	EnchantDict	   *pdict;
	zval            zbroker;
	zend_object     std;
} enchant_dict;

zend_class_entry *enchant_broker_ce;
static zend_object_handlers enchant_broker_handlers;

static inline enchant_broker *enchant_broker_from_obj(zend_object *obj) {
	return (enchant_broker *)((char *)(obj) - XtOffsetOf(enchant_broker, std));
}

#define Z_ENCHANT_BROKER_P(zv) enchant_broker_from_obj(Z_OBJ_P(zv))

static zend_object *enchant_broker_create_object(zend_class_entry *class_type) {
	enchant_broker *intern = zend_object_alloc(sizeof(enchant_broker), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

zend_class_entry *enchant_dict_ce;
static zend_object_handlers enchant_dict_handlers;

static inline enchant_dict *enchant_dict_from_obj(zend_object *obj) {
	return (enchant_dict *)((char *)(obj) - XtOffsetOf(enchant_dict, std));
}

#define Z_ENCHANT_DICT_P(zv) enchant_dict_from_obj(Z_OBJ_P(zv))

static zend_object *enchant_dict_create_object(zend_class_entry *class_type) {
	enchant_dict *intern = zend_object_alloc(sizeof(enchant_dict), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

/* {{{ enchant_module_entry */
zend_module_entry enchant_module_entry = {
	STANDARD_MODULE_HEADER,
	"enchant",
	ext_functions,
	PHP_MINIT(enchant),
	PHP_MSHUTDOWN(enchant),
	NULL,	/* Replace with NULL if there's nothing to do at request start */
	NULL,	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(enchant),
	PHP_ENCHANT_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ENCHANT
ZEND_GET_MODULE(enchant)
#endif

static void
enumerate_providers_fn (const char * const name,
                        const char * const desc,
                        const char * const file,
                        void * ud) /* {{{ */
{
	zval *zdesc = (zval *) ud;
	zval tmp_array;

	array_init(&tmp_array);

	add_assoc_string(&tmp_array, "name", (char *)name);
	add_assoc_string(&tmp_array, "desc", (char *)desc);
	add_assoc_string(&tmp_array, "file", (char *)file);
	add_next_index_zval(zdesc, &tmp_array);
}
/* }}} */

static void
describe_dict_fn (const char * const lang,
                  const char * const name,
                  const char * const desc,
                  const char * const file,
                  void * ud) /* {{{ */
{
	zval *zdesc = (zval *) ud;
	array_init(zdesc);
	add_assoc_string(zdesc, "lang", (char *)lang);
	add_assoc_string(zdesc, "name", (char *)name);
	add_assoc_string(zdesc, "desc", (char *)desc);
	add_assoc_string(zdesc, "file", (char *)file);
}
/* }}} */

static void php_enchant_list_dicts_fn( const char * const lang_tag,
	   	const char * const provider_name, const char * const provider_desc,
		const char * const provider_file, void * ud) /* {{{ */
{
	zval *zdesc = (zval *) ud;
	zval tmp_array;

	array_init(&tmp_array);
	add_assoc_string(&tmp_array, "lang_tag", (char *)lang_tag);
	add_assoc_string(&tmp_array, "provider_name", (char *)provider_name);
	add_assoc_string(&tmp_array, "provider_desc", (char *)provider_desc);
	add_assoc_string(&tmp_array, "provider_file", (char *)provider_file);
	add_next_index_zval(zdesc, &tmp_array);

}
/* }}} */

static void php_enchant_broker_free(zend_object *object) /* {{{ */
{
	enchant_broker *broker = enchant_broker_from_obj(object);

	if (broker->pbroker) {  /* may have been freed by enchant_broker_free */
		enchant_broker_free(broker->pbroker);
		broker->pbroker = NULL;
	}
	zend_object_std_dtor(object);
}
/* }}} */

static void php_enchant_dict_free(zend_object *object) /* {{{ */

{
	enchant_dict *dict = enchant_dict_from_obj(object);

	if (dict->pdict) { /* may have been freed by enchant_broker_free_dict */
		enchant_broker *broker = Z_ENCHANT_BROKER_P(&dict->zbroker);

		if (broker && broker->pbroker) {
			enchant_broker_free_dict(broker->pbroker, dict->pdict);
			broker->nb_dict--;
			zval_ptr_dtor(&dict->zbroker);
		}
		dict->pdict = NULL;
	}
	zend_object_std_dtor(object);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(enchant)
{
	enchant_broker_ce = register_class_EnchantBroker();
	enchant_broker_ce->create_object = enchant_broker_create_object;
	enchant_broker_ce->default_object_handlers = &enchant_broker_handlers;

	memcpy(&enchant_broker_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	enchant_broker_handlers.offset = XtOffsetOf(enchant_broker, std);
	enchant_broker_handlers.free_obj = php_enchant_broker_free;
	enchant_broker_handlers.clone_obj = NULL;
	enchant_broker_handlers.compare = zend_objects_not_comparable;

	enchant_dict_ce = register_class_EnchantDictionary();
	enchant_dict_ce->create_object = enchant_dict_create_object;
	enchant_dict_ce->default_object_handlers = &enchant_dict_handlers;

	memcpy(&enchant_dict_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	enchant_dict_handlers.offset = XtOffsetOf(enchant_dict, std);
	enchant_dict_handlers.free_obj = php_enchant_dict_free;
	enchant_dict_handlers.clone_obj = NULL;
	enchant_dict_handlers.compare = zend_objects_not_comparable;

	register_enchant_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(enchant)
{
	return SUCCESS;
}
/* }}} */

static void __enumerate_providers_fn (const char * const name,
                        const char * const desc,
                        const char * const file,
                        void * ud) /* {{{ */
{
	php_info_print_table_row(3, name, desc, file);
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(enchant)
{
	EnchantBroker *pbroker;

	pbroker = enchant_broker_init();
	php_info_print_table_start();
	php_info_print_table_row(2, "enchant support", "enabled");
#ifdef HAVE_ENCHANT_GET_VERSION
	php_info_print_table_row(2, "Libenchant Version", enchant_get_version());
#elif defined(HAVE_ENCHANT_BROKER_SET_PARAM)
	php_info_print_table_row(2, "Libenchant Version", "1.5.x");
#endif
	php_info_print_table_end();

	php_info_print_table_start();
	enchant_broker_describe(pbroker, __enumerate_providers_fn, NULL);
	php_info_print_table_end();
	enchant_broker_free(pbroker);
}
/* }}} */

#define PHP_ENCHANT_GET_BROKER	\
	pbroker = Z_ENCHANT_BROKER_P(broker); \
	if (!pbroker->pbroker) {	\
		zend_value_error("Invalid or uninitialized EnchantBroker object"); \
		RETURN_THROWS(); \
	}

#define PHP_ENCHANT_GET_DICT	\
	pdict = Z_ENCHANT_DICT_P(dict); \
	if (!pdict->pdict) {	\
		zend_value_error("Invalid or uninitialized EnchantDictionary object"); \
		RETURN_THROWS(); \
	}

/* {{{ create a new broker object capable of requesting */
PHP_FUNCTION(enchant_broker_init)
{
	enchant_broker *broker;
	EnchantBroker *pbroker;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	pbroker = enchant_broker_init();
	if (pbroker) {
		object_init_ex(return_value, enchant_broker_ce);
		broker = Z_ENCHANT_BROKER_P(return_value);
		broker->pbroker = pbroker;
		broker->nb_dict = 0;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Destroys the broker object and its dictionaries */
PHP_FUNCTION(enchant_broker_free)
{
	zval *broker;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &broker, enchant_broker_ce) == FAILURE) {
		RETURN_THROWS();
	}
	PHP_ENCHANT_GET_BROKER;

	if (pbroker->nb_dict > 0) {
		zend_throw_error(NULL, "Cannot free EnchantBroker object with open EnchantDictionary objects");
		RETURN_THROWS();
	}
	if (pbroker->pbroker) {
		enchant_broker_free(pbroker->pbroker);
		pbroker->pbroker = NULL;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns the last error of the broker */
PHP_FUNCTION(enchant_broker_get_error)
{
	zval *broker;
	enchant_broker *pbroker;
	const char *msg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &broker, enchant_broker_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_BROKER;

	msg = enchant_broker_get_error(pbroker->pbroker);
	if (msg) {
		RETURN_STRING((char *)msg);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Set the directory path for a given backend, works with ispell and myspell */
PHP_FUNCTION(enchant_broker_set_dict_path)
{
	zval *broker;
	zend_long dict_type;
	char *value;
	size_t value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ols", &broker, enchant_broker_ce, &dict_type, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

#if HAVE_ENCHANT_BROKER_SET_PARAM
	enchant_broker *pbroker;
	if (!value_len) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	switch (dict_type) {
		case PHP_ENCHANT_MYSPELL:
			PHP_ENCHANT_GET_BROKER;
			enchant_broker_set_param(pbroker->pbroker, "enchant.myspell.dictionary.path", (const char *)value);
			RETURN_TRUE;
			break;

		case PHP_ENCHANT_ISPELL:
			PHP_ENCHANT_GET_BROKER;
			enchant_broker_set_param(pbroker->pbroker, "enchant.ispell.dictionary.path", (const char *)value);
			RETURN_TRUE;
			break;

		default:
			RETURN_FALSE;
	}
#endif
}
/* }}} */


/* {{{ Get the directory path for a given backend, works with ispell and myspell */
PHP_FUNCTION(enchant_broker_get_dict_path)
{
	zval *broker;
	zend_long dict_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &broker, enchant_broker_ce, &dict_type) == FAILURE) {
		RETURN_THROWS();
	}

#if HAVE_ENCHANT_BROKER_SET_PARAM
	enchant_broker *pbroker;
	char *value;
	PHP_ENCHANT_GET_BROKER;

	switch (dict_type) {
		case PHP_ENCHANT_MYSPELL:
			PHP_ENCHANT_GET_BROKER;
			value = enchant_broker_get_param(pbroker->pbroker, "enchant.myspell.dictionary.path");
			break;

		case PHP_ENCHANT_ISPELL:
			PHP_ENCHANT_GET_BROKER;
			value = enchant_broker_get_param(pbroker->pbroker, "enchant.ispell.dictionary.path");
			break;

		default:
			RETURN_FALSE;
	}

	if (value == NULL) {
		php_error_docref(NULL, E_WARNING, "dict_path not set");
		RETURN_FALSE;
	}

	RETURN_STRING(value);
#endif
}
/* }}} */

/* {{{ Lists the dictionaries available for the given broker */
PHP_FUNCTION(enchant_broker_list_dicts)
{
	zval *broker;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &broker, enchant_broker_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_BROKER;

	array_init(return_value);
	enchant_broker_list_dicts(pbroker->pbroker, php_enchant_list_dicts_fn, (void *)return_value);
}
/* }}} */

/* {{{ create a new dictionary using tag, the non-empty language tag you wish to request
	a dictionary for ("en_US", "de_DE", ...) */
PHP_FUNCTION(enchant_broker_request_dict)
{
	zval *broker;
	enchant_broker *pbroker;
	enchant_dict *dict;
	EnchantDict *pdict;
	char *tag;
	size_t taglen;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &broker, enchant_broker_ce, &tag, &taglen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_BROKER;

	if (taglen == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	pdict = enchant_broker_request_dict(pbroker->pbroker, (const char *)tag);
	if (pdict) {
		pbroker->nb_dict++;

		object_init_ex(return_value, enchant_dict_ce);
		dict = Z_ENCHANT_DICT_P(return_value);
		dict->pdict = pdict;
		ZVAL_COPY(&dict->zbroker, broker);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ creates a dictionary using a PWL file. A PWL file is personal word file one word per line. It must exist before the call.*/
PHP_FUNCTION(enchant_broker_request_pwl_dict)
{
	zval *broker;
	enchant_broker *pbroker;
	enchant_dict *dict;
	EnchantDict *pdict;
	const char *pwl;
	size_t pwllen;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Op", &broker, enchant_broker_ce, &pwl, &pwllen) == FAILURE) {
		RETURN_THROWS();
	}

	if (php_check_open_basedir(pwl)) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	pdict = enchant_broker_request_pwl_dict(pbroker->pbroker, pwl);
	if (pdict) {
		pbroker->nb_dict++;

		object_init_ex(return_value, enchant_dict_ce);
		dict = Z_ENCHANT_DICT_P(return_value);
		dict->pdict = pdict;
		ZVAL_COPY(&dict->zbroker, broker);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Free the dictionary resource */
PHP_FUNCTION(enchant_broker_free_dict)
{
	zval *dict;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &dict, enchant_dict_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	if (pdict->pdict) {
		enchant_broker *broker = Z_ENCHANT_BROKER_P(&pdict->zbroker);

		if (broker && broker->pbroker) {
			enchant_broker_free_dict(broker->pbroker, pdict->pdict);
			broker->nb_dict--;
			zval_ptr_dtor(&pdict->zbroker);
		}
		pdict->pdict = NULL;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Whether a dictionary exists or not. Using non-empty tag */
PHP_FUNCTION(enchant_broker_dict_exists)
{
	zval *broker;
	char *tag;
	size_t taglen;
	enchant_broker * pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &broker, enchant_broker_ce,  &tag, &taglen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_BROKER;

	RETURN_BOOL(enchant_broker_dict_exists(pbroker->pbroker, tag));
}
/* }}} */

/* {{{ Declares a preference of dictionaries to use for the language
	described/referred to by 'tag'. The ordering is a comma delimited
	list of provider names. As a special exception, the "*" tag can
	be used as a language tag to declare a default ordering for any
	language that does not explicitly declare an ordering. */

PHP_FUNCTION(enchant_broker_set_ordering)
{
	zval *broker;
	char *pordering;
	size_t porderinglen;
	char *ptag;
	size_t ptaglen;
	enchant_broker * pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oss", &broker, enchant_broker_ce, &ptag, &ptaglen, &pordering, &porderinglen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_BROKER;

	enchant_broker_set_ordering(pbroker->pbroker, ptag, pordering);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Enumerates the Enchant providers and tells you some rudimentary information about them. The same info is provided through phpinfo() */
PHP_FUNCTION(enchant_broker_describe)
{
	EnchantBrokerDescribeFn describetozval = enumerate_providers_fn;
	zval *broker;
	enchant_broker * pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &broker, enchant_broker_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_BROKER;

	array_init(return_value);
	enchant_broker_describe(pbroker->pbroker, describetozval, (void *)return_value);
}
/* }}} */

/* {{{ If the word is correctly spelled return true, otherwise return false, if suggestions variable
    is provided, fill it with spelling alternatives. */
PHP_FUNCTION(enchant_dict_quick_check)
{
	zval *dict, *sugg = NULL;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os|z", &dict, enchant_dict_ce, &word, &wordlen, &sugg) == FAILURE) {
		RETURN_THROWS();
	}

	if (sugg) {
		sugg = zend_try_array_init(sugg);
		if (!sugg) {
			RETURN_THROWS();
		}
	}

	PHP_ENCHANT_GET_DICT;

	if (enchant_dict_check(pdict->pdict, word, wordlen) > 0) {
		size_t n_sugg;
		char **suggs;

		if (!sugg && ZEND_NUM_ARGS() == 2) {
			RETURN_FALSE;
		}

		suggs = enchant_dict_suggest(pdict->pdict, word, wordlen, &n_sugg);
		if (suggs && n_sugg) {
			size_t i;
			for (i = 0; i < n_sugg; i++) {
				add_next_index_string(sugg, suggs[i]);
			}
			enchant_dict_free_string_list(pdict->pdict, suggs);
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ If the word is correctly spelled return true, otherwise return false */
PHP_FUNCTION(enchant_dict_check)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &dict, enchant_dict_ce, &word, &wordlen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	RETURN_BOOL(!enchant_dict_check(pdict->pdict, word, wordlen));
}
/* }}} */

/* {{{ Will return a list of values if any of those pre-conditions are not met.*/
PHP_FUNCTION(enchant_dict_suggest)
{
	zval *dict;
	char *word;
	size_t wordlen;
	char **suggs;
	enchant_dict *pdict;
	size_t n_sugg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &dict, enchant_dict_ce, &word, &wordlen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;
	array_init(return_value);

	suggs = enchant_dict_suggest(pdict->pdict, word, wordlen, &n_sugg);
	if (suggs && n_sugg) {
		size_t i;

		for (i = 0; i < n_sugg; i++) {
			add_next_index_string(return_value, suggs[i]);
		}

		enchant_dict_free_string_list(pdict->pdict, suggs);
	}
}
/* }}} */

/* {{{ add 'word' to personal word list */
PHP_FUNCTION(enchant_dict_add)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &dict, enchant_dict_ce, &word, &wordlen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_add(pdict->pdict, word, wordlen);
}
/* }}} */

/* {{{ add 'word' to this spell-checking session */
PHP_FUNCTION(enchant_dict_add_to_session)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &dict, enchant_dict_ce, &word, &wordlen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_add_to_session(pdict->pdict, word, wordlen);
}
/* }}} */

/* {{{ whether or not 'word' exists in this spelling-session */
PHP_FUNCTION(enchant_dict_is_added)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &dict, enchant_dict_ce, &word, &wordlen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	RETURN_BOOL(enchant_dict_is_added(pdict->pdict, word, wordlen));
}
/* }}} */

/* {{{ add a correction for 'mis' using 'cor'.
	Notes that you replaced @mis with @cor, so it's possibly more likely
	that future occurrences of @mis will be replaced with @cor. So it might
	bump @cor up in the suggestion list.*/
PHP_FUNCTION(enchant_dict_store_replacement)
{
	zval *dict;
	char *mis, *cor;
	size_t mislen, corlen;

	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oss", &dict, enchant_dict_ce, &mis, &mislen, &cor, &corlen) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_store_replacement(pdict->pdict, mis, mislen, cor, corlen);
}
/* }}} */

/* {{{ Returns the last error of the current spelling-session */
PHP_FUNCTION(enchant_dict_get_error)
{
	zval *dict;
	enchant_dict *pdict;
	const char *msg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &dict, enchant_dict_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	msg = enchant_dict_get_error(pdict->pdict);
	if (msg) {
		RETURN_STRING((char *)msg);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Describes an individual dictionary 'dict' */
PHP_FUNCTION(enchant_dict_describe)
{
	zval *dict;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &dict, enchant_dict_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_describe(pdict->pdict, describe_dict_fn, (void *)return_value);
}
/* }}} */
