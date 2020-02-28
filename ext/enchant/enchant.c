/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Pierre-Alain Joye <paj@pearfr.org>                           |
  |         Ilia Alshanetsky <ilia@prohost.org>                          |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include <enchant.h>
#include "php_enchant.h"

typedef EnchantBroker * EnchantBrokerPtr;
typedef struct _broker_struct enchant_broker;
typedef struct _dict_struct enchant_dict;

typedef enchant_broker * enchant_brokerPtr;
typedef enchant_dict * enchant_dictPtr;

typedef struct _broker_struct {
	EnchantBroker	*pbroker;
	enchant_dict	**dict;
	unsigned int	dictcnt;
	zend_resource	*rsrc;
} _enchant_broker;

typedef struct _dict_struct {
	unsigned int	id;
	EnchantDict		*pdict;
	enchant_broker	*pbroker;
	zend_resource	*rsrc;
} _enchant_dict;


/* True global resources - no need for thread safety here */
static int le_enchant_broker;
static int le_enchant_dict;

/* If you declare any globals in php_enchant.h uncomment this:*/
/*ZEND_DECLARE_MODULE_GLOBALS(enchant)*/

#define PHP_ENCHANT_MYSPELL 1
#define PHP_ENCHANT_ISPELL 2

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_enchant_broker_init, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_broker_free, 0, 0, 1)
	ZEND_ARG_INFO(0, broker)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_broker_set_dict_path, 0, 0, 3)
	ZEND_ARG_INFO(0, broker)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_broker_get_dict_path, 0, 0, 2)
	ZEND_ARG_INFO(0, broker)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_broker_request_dict, 0, 0, 2)
	ZEND_ARG_INFO(0, broker)
	ZEND_ARG_INFO(0, tag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_broker_request_pwl_dict, 0, 0, 2)
	ZEND_ARG_INFO(0, broker)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_broker_free_dict, 0, 0, 1)
	ZEND_ARG_INFO(0, dict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_broker_set_ordering, 0, 0, 3)
	ZEND_ARG_INFO(0, broker)
	ZEND_ARG_INFO(0, tag)
	ZEND_ARG_INFO(0, ordering)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_dict_quick_check, 0, 0, 2)
	ZEND_ARG_INFO(0, dict)
	ZEND_ARG_INFO(0, word)
	ZEND_ARG_INFO(1, suggestions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_dict_check, 0, 0, 2)
	ZEND_ARG_INFO(0, dict)
	ZEND_ARG_INFO(0, word)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enchant_dict_store_replacement, 0, 0, 3)
	ZEND_ARG_INFO(0, dict)
	ZEND_ARG_INFO(0, mis)
	ZEND_ARG_INFO(0, cor)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ enchant_functions[]
 *
 * Every user visible function must have an entry in enchant_functions[].
 */
static const zend_function_entry enchant_functions[] = {
	PHP_FE(enchant_broker_init, 			arginfo_enchant_broker_init)
	PHP_FE(enchant_broker_free, 			arginfo_enchant_broker_free)
	PHP_FE(enchant_broker_get_error, 		arginfo_enchant_broker_free)
	PHP_FE(enchant_broker_set_dict_path,	arginfo_enchant_broker_set_dict_path)
	PHP_FE(enchant_broker_get_dict_path,	arginfo_enchant_broker_get_dict_path)
	PHP_FE(enchant_broker_list_dicts, 		arginfo_enchant_broker_free)
	PHP_FE(enchant_broker_request_dict,		arginfo_enchant_broker_request_dict)
	PHP_FE(enchant_broker_request_pwl_dict, arginfo_enchant_broker_request_pwl_dict)
	PHP_FE(enchant_broker_free_dict, 		arginfo_enchant_broker_free_dict)
	PHP_FE(enchant_broker_dict_exists, 		arginfo_enchant_broker_request_dict)
	PHP_FE(enchant_broker_set_ordering, 	arginfo_enchant_broker_set_ordering)
	PHP_FE(enchant_broker_describe, 		arginfo_enchant_broker_free)
	PHP_FE(enchant_dict_check, 				arginfo_enchant_dict_check)
	PHP_FE(enchant_dict_suggest, 			arginfo_enchant_dict_check)
	PHP_FE(enchant_dict_add_to_personal, 	arginfo_enchant_dict_check)
	PHP_FE(enchant_dict_add_to_session, 	arginfo_enchant_dict_check)
	PHP_FE(enchant_dict_is_in_session, 		arginfo_enchant_dict_check)
	PHP_FE(enchant_dict_store_replacement, 	arginfo_enchant_dict_store_replacement)
	PHP_FE(enchant_dict_get_error, 			arginfo_enchant_broker_free_dict)
	PHP_FE(enchant_dict_describe, 			arginfo_enchant_broker_free_dict)
	PHP_FE(enchant_dict_quick_check, 		arginfo_enchant_dict_quick_check)
	PHP_FE_END
};
/* }}} */

/* {{{ enchant_module_entry
 */
zend_module_entry enchant_module_entry = {
	STANDARD_MODULE_HEADER,
	"enchant",
	enchant_functions,
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

	if (Z_TYPE_P(zdesc)!=IS_ARRAY) {
		array_init(zdesc);
	}

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

	if (Z_TYPE_P(zdesc) != IS_ARRAY) {
		array_init(zdesc);
	}
	add_next_index_zval(zdesc, &tmp_array);

}
/* }}} */

static void php_enchant_broker_free(zend_resource *rsrc) /* {{{ */
{
	if (rsrc->ptr) {
		enchant_broker *broker = (enchant_broker *)rsrc->ptr;
		if (broker) {
			if (broker->pbroker) {
				if (broker->dictcnt && broker->dict) {
					if (broker->dict) {
						int total;
						total = broker->dictcnt-1;
						do {
							if (broker->dict[total]) {
								enchant_dict *pdict = broker->dict[total];
								broker->dict[total] = NULL;
								zend_list_free(pdict->rsrc);
								efree(pdict);
							}
							total--;
						} while (total>=0);
					}
					efree(broker->dict);
					broker->dict = NULL;
				}
				enchant_broker_free(broker->pbroker);
			}
			efree(broker);
		}
	}
}
/* }}} */

static void php_enchant_dict_free(zend_resource *rsrc) /* {{{ */

{
	if (rsrc->ptr) {
		enchant_dict *pdict = (enchant_dict *)rsrc->ptr;
		if (pdict) {
			enchant_broker *pbroker = pdict->pbroker;

			if (pdict->pdict && pbroker) {
				enchant_broker_free_dict(pbroker->pbroker, pdict->pdict);
			}

			pbroker->dict[pdict->id] = NULL;
			efree(pdict);
			zend_list_delete(pbroker->rsrc);
		}
	}
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(enchant)
{
	le_enchant_broker = zend_register_list_destructors_ex(php_enchant_broker_free, NULL, "enchant_broker", module_number);
	le_enchant_dict = zend_register_list_destructors_ex(php_enchant_dict_free, NULL, "enchant_dict", module_number);
	REGISTER_LONG_CONSTANT("ENCHANT_MYSPELL", PHP_ENCHANT_MYSPELL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENCHANT_ISPELL", PHP_ENCHANT_ISPELL, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
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

/* {{{ PHP_MINFO_FUNCTION
 */
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
	pbroker = (enchant_broker *)zend_fetch_resource(Z_RES_P(broker), "enchant_broker", le_enchant_broker); \
	if (!pbroker || !pbroker->pbroker) {	\
		php_error_docref(NULL, E_WARNING, "Resource broker invalid");	\
		RETURN_FALSE;	\
	}

#define PHP_ENCHANT_GET_DICT	\
	pdict = (enchant_dict *)zend_fetch_resource(Z_RES_P(dict), "enchant_dict", le_enchant_dict); \
	if (!pdict || !pdict->pdict) {	\
		php_error_docref(NULL, E_WARNING, "Invalid dictionary resource.");	\
		RETURN_FALSE;	\
	}

/* {{{ proto resource enchant_broker_init()
   create a new broker object capable of requesting */
PHP_FUNCTION(enchant_broker_init)
{
	enchant_broker *broker;
	EnchantBroker *pbroker;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	pbroker = enchant_broker_init();

	if (pbroker) {
		broker = (enchant_broker *) emalloc(sizeof(enchant_broker));
		broker->pbroker = pbroker;
		broker->dict = NULL;
		broker->dictcnt = 0;
		broker->rsrc = zend_register_resource(broker, le_enchant_broker);
		RETURN_RES(broker->rsrc);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool enchant_broker_free(resource broker)
   Destroys the broker object and its dictionnaries */
PHP_FUNCTION(enchant_broker_free)
{
	zval *broker;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}
	PHP_ENCHANT_GET_BROKER;

	zend_list_close(Z_RES_P(broker));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string enchant_broker_get_error(resource broker)
   Returns the last error of the broker */
PHP_FUNCTION(enchant_broker_get_error)
{
	zval *broker;
	enchant_broker *pbroker;
	char *msg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	msg = enchant_broker_get_error(pbroker->pbroker);
	if (msg) {
		RETURN_STRING((char *)msg);
	}
	RETURN_FALSE;
}
/* }}} */

#if HAVE_ENCHANT_BROKER_SET_PARAM
/* {{{ proto bool enchant_broker_set_dict_path(resource broker, int dict_type, string value)
	Set the directory path for a given backend, works with ispell and myspell */
PHP_FUNCTION(enchant_broker_set_dict_path)
{
	zval *broker;
	enchant_broker *pbroker;
	zend_long dict_type;
	char *value;
	size_t value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rls", &broker, &dict_type, &value, &value_len) == FAILURE) {
		RETURN_FALSE;
	}

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
}
/* }}} */


/* {{{ proto string enchant_broker_get_dict_path(resource broker, int dict_type)
	Get the directory path for a given backend, works with ispell and myspell */
PHP_FUNCTION(enchant_broker_get_dict_path)
{
	zval *broker;
	enchant_broker *pbroker;
	zend_long dict_type;
	char *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &broker, &dict_type) == FAILURE) {
		RETURN_FALSE;
	}

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
}
/* }}} */
#else
/* {{{ proto bool enchant_broker_set_dict_path(resource broker, int dict_type, string value)
	Set the directory path for a given backend, works with ispell and myspell */
PHP_FUNCTION(enchant_broker_set_dict_path)
{
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto string enchant_broker_get_dict_path(resource broker, int dict_type)
	Get the directory path for a given backend, works with ispell and myspell */
PHP_FUNCTION(enchant_broker_get_dict_path)
{
	RETURN_FALSE;
}
/* }}} */
#endif

/* {{{ proto array enchant_broker_list_dicts(resource broker)
   Lists the dictionaries available for the given broker */
PHP_FUNCTION(enchant_broker_list_dicts)
{
	zval *broker;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	enchant_broker_list_dicts(pbroker->pbroker, php_enchant_list_dicts_fn, (void *)return_value);
}
/* }}} */

/* {{{ proto resource enchant_broker_request_dict(resource broker, string tag)
	create a new dictionary using tag, the non-empty language tag you wish to request
	a dictionary for ("en_US", "de_DE", ...) */
PHP_FUNCTION(enchant_broker_request_dict)
{
	zval *broker;
	enchant_broker *pbroker;
	enchant_dict *dict;
	EnchantDict *d;
	char *tag;
	size_t taglen;
	int pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &broker, &tag, &taglen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	if (taglen == 0) {
		php_error_docref(NULL, E_WARNING, "Tag cannot be empty");
		RETURN_FALSE;
	}

	d = enchant_broker_request_dict(pbroker->pbroker, (const char *)tag);
	if (d) {
		pos = pbroker->dictcnt++;
		if (pbroker->dictcnt) {
			pbroker->dict = (enchant_dict **)erealloc(pbroker->dict, sizeof(enchant_dict *) * pbroker->dictcnt);
		} else {
			pbroker->dict = (enchant_dict **)emalloc(sizeof(enchant_dict *));
			pos = 0;
		}

		dict = pbroker->dict[pos] = (enchant_dict *)emalloc(sizeof(enchant_dict));
		dict->id = pos;
		dict->pbroker = pbroker;
		dict->pdict = d;
		pbroker->dict[pos] = dict;

		dict->rsrc = zend_register_resource(dict, le_enchant_dict);
		GC_ADDREF(pbroker->rsrc);
		RETURN_RES(dict->rsrc);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource enchant_broker_request_pwl_dict(resource broker, string filename)
   creates a dictionary using a PWL file. A PWL file is personal word file one word per line. It must exist before the call.*/
PHP_FUNCTION(enchant_broker_request_pwl_dict)
{
	zval *broker;
	enchant_broker *pbroker;
	enchant_dict *dict;
	EnchantDict *d;
	char *pwl;
	size_t pwllen;
	int pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rp", &broker, &pwl, &pwllen) == FAILURE) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(pwl)) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	d = enchant_broker_request_pwl_dict(pbroker->pbroker, (const char *)pwl);
	if (d) {
		pos = pbroker->dictcnt++;
		if (pbroker->dictcnt) {
			pbroker->dict = (enchant_dict **)erealloc(pbroker->dict, sizeof(enchant_dict *) * pbroker->dictcnt);
		} else {
			pbroker->dict = (enchant_dict **)emalloc(sizeof(enchant_dict *));
			pos = 0;
		}

		dict = pbroker->dict[pos] = (enchant_dict *)emalloc(sizeof(enchant_dict));
		dict->id = pos;
		dict->pbroker = pbroker;
		dict->pdict = d;
		pbroker->dict[pos] = dict;

		dict->rsrc = zend_register_resource(dict, le_enchant_dict);
		GC_ADDREF(pbroker->rsrc);
		RETURN_RES(dict->rsrc);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource enchant_broker_free_dict(resource dict)
   Free the dictionary resource */
PHP_FUNCTION(enchant_broker_free_dict)
{
	zval *dict;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &dict) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	zend_list_close(Z_RES_P(dict));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool enchant_broker_dict_exists(resource broker, string tag)
   Whether a dictionary exists or not. Using non-empty tag */
PHP_FUNCTION(enchant_broker_dict_exists)
{
	zval *broker;
	char *tag;
	size_t taglen;
	enchant_broker * pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &broker, &tag, &taglen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	RETURN_BOOL(enchant_broker_dict_exists(pbroker->pbroker, tag));
}
/* }}} */

/* {{{ proto bool enchant_broker_set_ordering(resource broker, string tag, string ordering)
	Declares a preference of dictionaries to use for the language
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &broker, &ptag, &ptaglen, &pordering, &porderinglen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	enchant_broker_set_ordering(pbroker->pbroker, ptag, pordering);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array enchant_broker_describe(resource broker)
	Enumerates the Enchant providers and tells you some rudimentary information about them. The same info is provided through phpinfo() */
PHP_FUNCTION(enchant_broker_describe)
{
	EnchantBrokerDescribeFn describetozval = enumerate_providers_fn;
	zval *broker;
	enchant_broker * pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	enchant_broker_describe(pbroker->pbroker, describetozval, (void *)return_value);
}
/* }}} */

/* {{{ proto bool enchant_dict_quick_check(resource dict, string word [, array &suggestions])
    If the word is correctly spelled return true, otherwise return false, if suggestions variable
    is provided, fill it with spelling alternatives. */
PHP_FUNCTION(enchant_dict_quick_check)
{
	zval *dict, *sugg = NULL;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|z", &dict, &word, &wordlen, &sugg) == FAILURE) {
		RETURN_FALSE;
	}

	if (sugg) {
		sugg = zend_try_array_init(sugg);
		if (!sugg) {
			return;
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
			enchant_dict_free_suggestions(pdict->pdict, suggs);
		}


		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool enchant_dict_check(resource dict, string word)
    If the word is correctly spelled return true, otherwise return false */
PHP_FUNCTION(enchant_dict_check)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	RETURN_BOOL(!enchant_dict_check(pdict->pdict, word, wordlen));
}
/* }}} */

/* {{{ proto array enchant_dict_suggest(resource dict, string word)
    Will return a list of values if any of those pre-conditions are not met.*/
PHP_FUNCTION(enchant_dict_suggest)
{
	zval *dict;
	char *word;
	size_t wordlen;
	char **suggs;
	enchant_dict *pdict;
	size_t n_sugg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	suggs = enchant_dict_suggest(pdict->pdict, word, wordlen, &n_sugg);
	if (suggs && n_sugg) {
		size_t i;

		array_init(return_value);
		for (i = 0; i < n_sugg; i++) {
			add_next_index_string(return_value, suggs[i]);
		}

		enchant_dict_free_suggestions(pdict->pdict, suggs);
	}
}
/* }}} */

/* {{{ proto void enchant_dict_add_to_personal(resource dict, string word)
     add 'word' to personal word list */
PHP_FUNCTION(enchant_dict_add_to_personal)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_add_to_personal(pdict->pdict, word, wordlen);
}
/* }}} */

/* {{{ proto void enchant_dict_add_to_session(resource dict, string word)
   add 'word' to this spell-checking session */
PHP_FUNCTION(enchant_dict_add_to_session)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_add_to_session(pdict->pdict, word, wordlen);
}
/* }}} */

/* {{{ proto bool enchant_dict_is_in_session(resource dict, string word)
   whether or not 'word' exists in this spelling-session */
PHP_FUNCTION(enchant_dict_is_in_session)
{
	zval *dict;
	char *word;
	size_t wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	RETURN_BOOL(enchant_dict_is_in_session(pdict->pdict, word, wordlen));
}
/* }}} */

/* {{{ proto void enchant_dict_store_replacement(resource dict, string mis, string cor)
	add a correction for 'mis' using 'cor'.
	Notes that you replaced @mis with @cor, so it's possibly more likely
	that future occurrences of @mis will be replaced with @cor. So it might
	bump @cor up in the suggestion list.*/
PHP_FUNCTION(enchant_dict_store_replacement)
{
	zval *dict;
	char *mis, *cor;
	size_t mislen, corlen;

	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &dict, &mis, &mislen, &cor, &corlen) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_store_replacement(pdict->pdict, mis, mislen, cor, corlen);
}
/* }}} */

/* {{{ proto string enchant_dict_get_error(resource dict)
   Returns the last error of the current spelling-session */
PHP_FUNCTION(enchant_dict_get_error)
{
	zval *dict;
	enchant_dict *pdict;
	char *msg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &dict) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	msg = enchant_dict_get_error(pdict->pdict);
	if (msg) {
		RETURN_STRING((char *)msg);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto array enchant_dict_describe(resource dict)
   Describes an individual dictionary 'dict' */
PHP_FUNCTION(enchant_dict_describe)
{
	zval *dict;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &dict) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_DICT;

	enchant_dict_describe(pdict->pdict, describe_dict_fn, (void *)return_value);
}
/* }}} */
