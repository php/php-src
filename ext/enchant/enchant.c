/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
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
  +----------------------------------------------------------------------+

  $Id$
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "enchant.h"
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
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
	zval			*rsrc_id;
} _enchant_broker;

typedef struct _dict_struct {
	unsigned int	id;
	EnchantDict		*pdict;
	enchant_broker	*pbroker;
	zval			*rsrc_id;
	enchant_dict	*next;
	enchant_dict	*prev;
} _enchant_dict;


/* True global resources - no need for thread safety here */
static int le_enchant_broker;
static int le_enchant_dict;

/* If you declare any globals in php_enchant.h uncomment this:*/
/*ZEND_DECLARE_MODULE_GLOBALS(enchant)*/

/* {{{ enchant_functions[]
 *
 * Every user visible function must have an entry in enchant_functions[].
 */
function_entry enchant_functions[] = {
	PHP_FE(enchant_broker_init, NULL)
	PHP_FE(enchant_broker_free, NULL)
	PHP_FE(enchant_broker_get_error, NULL)
	PHP_FE(enchant_broker_request_dict,	NULL)
	PHP_FE(enchant_broker_request_pwl_dict, NULL)
	PHP_FE(enchant_broker_free_dict, NULL)
	PHP_FE(enchant_broker_dict_exists, NULL)
	PHP_FE(enchant_broker_set_ordering, NULL)
	PHP_FE(enchant_broker_describe, NULL)
	PHP_FE(enchant_dict_check, NULL)
	PHP_FE(enchant_dict_suggest, NULL)
	PHP_FE(enchant_dict_add_to_personal, NULL)
	PHP_FE(enchant_dict_add_to_session, NULL)
	PHP_FE(enchant_dict_is_in_session, NULL)
	PHP_FE(enchant_dict_store_replacement, NULL)
	PHP_FE(enchant_dict_get_error, NULL)
	PHP_FE(enchant_dict_describe, NULL)

	{NULL, NULL, NULL}	/* Must be the last line in enchant_functions[] */
};
/* }}} */

/* {{{ enchant_module_entry
 */
zend_module_entry enchant_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"enchant",
	enchant_functions,
	PHP_MINIT(enchant),
	PHP_MSHUTDOWN(enchant),
	NULL,	/* Replace with NULL if there's nothing to do at request start */
	NULL,	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(enchant),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
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
                        void * ud)
{
	zval *zdesc = (zval *) ud;
	zval *tmp_array;

	MAKE_STD_ZVAL(tmp_array);
	array_init(tmp_array);

	add_assoc_string(tmp_array, "name", (char *)name, 1);
	add_assoc_string(tmp_array, "desc", (char *)desc, 1);
	add_assoc_string(tmp_array, "file", (char *)file, 1);

	if (Z_TYPE_P(zdesc)!=IS_ARRAY) {
		array_init(zdesc);
	}

	add_next_index_zval(zdesc, tmp_array);
}

static void
describe_dict_fn (const char * const lang,
                  const char * const name,
                  const char * const desc,
                  const char * const file,
                  void * ud)
{
	zval *zdesc = (zval *) ud;
	zval *tmp_array;

	MAKE_STD_ZVAL(tmp_array);
	array_init(tmp_array);

	add_assoc_string(tmp_array, "lang", (char *)lang, 1);
	add_assoc_string(tmp_array, "name", (char *)name, 1);
	add_assoc_string(tmp_array, "desc", (char *)desc, 1);
	add_assoc_string(tmp_array, "file", (char *)file, 1);

	if (Z_TYPE_P(zdesc)!=IS_ARRAY) {
		array_init(zdesc);
	}

	add_next_index_zval(zdesc, tmp_array);
}

static void php_enchant_broker_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	int total, tofree;
	if (rsrc->ptr) {
		enchant_broker *broker = (enchant_broker *)rsrc->ptr;
		if (broker) {
			if (broker->pbroker) {
				if (broker->dictcnt && broker->dict) {
					if (broker->dict) {
						tofree = total = broker->dictcnt-1;
						do {
							printf("broker free, dict id: %i\n", broker->dict[total]->id);
							zend_list_delete(Z_RESVAL_P(broker->dict[total]->rsrc_id));
							efree(broker->dict[total]);
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

static void php_enchant_dict_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	int prev;

	if (rsrc->ptr) {
		enchant_dict *pdict = (enchant_dict *)rsrc->ptr;
		if (pdict) {
			if (pdict->pdict && pdict->pbroker) {
				enchant_broker_free_dict(pdict->pbroker->pbroker, pdict->pdict);
			}
			prev = -pdict->pbroker->dictcnt;
			printf("id:%i\n",pdict->id);
			if (pdict->id) {
				pdict->pbroker->dict[pdict->id-1]->next = NULL;
			}
			//efree(pdict);
		}
	}
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(enchant)
{
	le_enchant_broker = zend_register_list_destructors_ex(php_enchant_broker_free, NULL, "enchant broker", module_number);
	le_enchant_dict = zend_register_list_destructors_ex(php_enchant_dict_free, NULL, "enchant dict", module_number);

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

/* {{{ PHP_MINFO_FUNCTION
 */
static void __enumerate_providers_fn (const char * const name,
                        const char * const desc,
                        const char * const file,
                        void * ud)
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
	php_info_print_table_header(2, "enchant support", "enabled");
	php_info_print_table_row(2, "Version", "@version@");
	php_info_print_table_row(2, "Revision", "$Revision$");
	php_info_print_table_row(1, "Provides");
	enchant_broker_describe(pbroker, __enumerate_providers_fn, NULL);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource enchant_new([int weekstart])
   create a new broker object capable of requesting */
PHP_FUNCTION(enchant_broker_init)
{
	enchant_broker *broker;
	EnchantBroker *pbroker;

	if ( ZEND_NUM_ARGS() ) {
		ZEND_WRONG_PARAM_COUNT();

	}

	pbroker = enchant_broker_init();

	if (pbroker) {
		broker = (enchant_broker *) safe_emalloc(sizeof(enchant_broker), 1, 0);
		broker->pbroker = pbroker;
		broker->dict = NULL;
		broker->dictcnt = 0;
		ZEND_REGISTER_RESOURCE(return_value, broker, le_enchant_broker);
		broker->rsrc_id = return_value;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean enchant_free(resource broker)
   Destroys the broker object and its dictionnaries */
PHP_FUNCTION(enchant_broker_free)
{
	zval *broker;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(pbroker, enchant_broker *, &broker, -1, "enchant_broker", le_enchant_broker);
	zend_list_delete(Z_RESVAL_P(broker));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string enchant_broker_get_error(resource broker)
   Returns the last error of the broker */
PHP_FUNCTION(enchant_broker_get_error)
{
	zval *broker;
	char *msg=NULL;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pbroker, enchant_broker *, &broker, -1, "enchant broker", le_enchant_broker);

	if (pbroker) {
		if (pbroker->pbroker) {
			msg = enchant_broker_get_error(pbroker->pbroker);
		}
	}
	if (msg) {
		RETURN_STRING((char *)msg, 1);
	}
}
/* }}} */

/* {{{ proto resource enchant_broker_request_dict(resource broker, string tag)
	create a new dict using tag, the non-empty language tag you wish to request
	a dictionary for ("en_US", "de_DE", ...) */
PHP_FUNCTION(enchant_broker_request_dict)
{
	zval *broker;
	enchant_broker *pbroker;
	enchant_dict *dict;
	EnchantDict *d;
	char *tag;
	int taglen;
	int pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &broker, &tag, &taglen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pbroker, enchant_broker *, &broker, -1, "enchant broker", le_enchant_broker);
	if (pbroker) {
		d = enchant_broker_request_dict(pbroker->pbroker, (const char *)tag);
		if (d) {
			if (pbroker->dictcnt) {
				pbroker->dict = (enchant_dict **)erealloc(pbroker->dict, sizeof(enchant_dict *) * pbroker->dictcnt);
				pos = pbroker->dictcnt++;
			} else {
				pbroker->dict = (enchant_dict **)safe_emalloc(sizeof(enchant_dict *), 1, 0);
				pos = 0;
				pbroker->dictcnt++;
			}
			printf("new id: %i\n", pos);
			dict = pbroker->dict[pos] = (enchant_dict *)safe_emalloc(sizeof(enchant_dict), 1, 0);
			dict->id = pos;
			dict->pbroker = pbroker;
			dict->pdict = d;
			dict->rsrc_id = return_value;
			dict->prev = pos?pbroker->dict[pos-1]:NULL;
			dict->next = NULL;
			pbroker->dict[pos] = dict;
			if (pos) {
				pbroker->dict[pos-1]->next = dict;
			}
			ZEND_REGISTER_RESOURCE(return_value, dict, le_enchant_dict);
		} else {
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto resource enchant_broker_request_pwl_dict(resource dict, string tag)
   creates a dictionnary using a PWL file. A PWL file is personal word file one word per line.
   It must exists before the call.*/
PHP_FUNCTION(enchant_broker_request_pwl_dict)
{
	zval *broker;
	enchant_broker *pbroker;
	enchant_dict *dict;
	EnchantDict *d;
	char *pwl;
	long pwllen;
	int pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &broker, &pwl, &pwllen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pbroker, enchant_broker *, &broker, -1, "enchant broker", le_enchant_broker);
	if (pbroker) {
		d = enchant_broker_request_pwl_dict(pbroker->pbroker, (const char *)pwl);
		if (d) {
			if (pbroker->dictcnt) {
				pos = pbroker->dictcnt++;
				pbroker->dict = (enchant_dict **)erealloc(pbroker->dict, sizeof(enchant_dict *) * pbroker->dictcnt);
			} else {
				pbroker->dict = (enchant_dict **)safe_emalloc(sizeof(enchant_dict *), 1, 0);
				pos = 0;
				pbroker->dictcnt++;
			}
			dict = pbroker->dict[pos] = (enchant_dict *)safe_emalloc(sizeof(enchant_dict), 1, 0);
			dict->id = pos;
			dict->pbroker = pbroker;
			dict->pdict = d;
			dict->rsrc_id = return_value;
			dict->prev = pos?pbroker->dict[pos-1]:NULL;
			dict->next = NULL;
			pbroker->dict[pos] = dict;
			if (pos) {
				pbroker->dict[pos-1]->next = dict;
			}
			ZEND_REGISTER_RESOURCE(return_value, dict, le_enchant_dict);
		} else {
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto resource enchant_broker_free_dict(resource dict)
   Free the dictionary resource */
PHP_FUNCTION(enchant_broker_free_dict)
{
	zval *dict;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &dict) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);
	zend_list_delete(Z_RESVAL_P(dict));
}
/* }}} */

/* {{{ proto bool enchant_new_dmy(resource broker, string tag)
   Wether a dictionary exists or not. Using non-empty tag */
PHP_FUNCTION(enchant_broker_dict_exists)
{
	zval *broker;
	char *tag;
	long taglen = 0;
	enchant_broker * pbroker;
	int exists = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &broker, &tag, &taglen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pbroker, enchant_broker *, &broker, -1, "enchant_broker", le_enchant_broker);
	if (pbroker && taglen) {
		if (pbroker->pbroker) {
			exists = enchant_broker_dict_exists(pbroker->pbroker, tag);
			RETURN_TRUE;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ressource broker invalid");
			RETURN_FALSE;
		}
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto resource enchant_broker_set_ordering(resource broker, string tag, string ordering)
	Declares a preference of dictionaries to use for the language
	described/referred to by 'tag'. The ordering is a comma delimited
	list of provider names. As a special exception, the "*" tag can
	be used as a language tag to declare a default ordering for any
	 language that does not explictly declare an ordering. */

PHP_FUNCTION(enchant_broker_set_ordering)
{
	zval *broker;
	char *pordering;
	int porderinglen;
	char *ptag;
	int ptaglen;
	enchant_broker * pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &broker, &ptag, &ptaglen, &pordering, &porderinglen) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(pbroker, enchant_broker *, &broker, -1, "enchant broker", le_enchant_broker);
	if (pbroker->pbroker) {
		enchant_broker_set_ordering(pbroker->pbroker, ptag, pordering);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ressource broker invalid");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array enchant_broker_describe(resource broker)
	Enumerates the Enchant providers and tells
	you some rudimentary information about them. The same info is provided through phpinfo() */
PHP_FUNCTION(enchant_broker_describe)
{
	EnchantBrokerDescribeFn describetozval = enumerate_providers_fn;
	zval *broker;
	enchant_broker * pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(pbroker, enchant_broker *, &broker, -1, "enchant broker", le_enchant_broker);
	if (pbroker->pbroker) {
		enchant_broker_describe(pbroker->pbroker, describetozval, (void *)return_value);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ressource broker invalid");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto long enchant_dict_check(resource broker)
    0 if the word is correctly spelled, positive if not, negative if error */
PHP_FUNCTION(enchant_dict_check)
{
	zval *dict;
	char *word;
	enchant_dict *pdict;
	int results;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &dict, &word, &results) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);
	results = enchant_dict_check( pdict->pdict, word, strlen(word));
	RETURN_LONG((long)results);
}
/* }}} */

/* {{{ proto array enchant_dict_suggest(resource broker, string word)
    Will return an list of values if any of those pre-conditions are not met.*/
PHP_FUNCTION(enchant_dict_suggest)
{
	zval *dict;
	char *word;
	long wordlen;
	char **suggs;
	enchant_dict *pdict;
	int i;
	int n_sugg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);
	if (pdict) {
		if (pdict->pdict) {
			suggs = enchant_dict_suggest(pdict->pdict, word, strlen(word), &n_sugg);
			if (suggs && n_sugg) {
				array_init(return_value);
				for (i=0; i<n_sugg; i++) {
					add_next_index_string(return_value, suggs[i], 1);
				}
			}
			enchant_dict_free_suggestions(pdict->pdict, suggs);
		}
	}
}
/* }}} */

/* {{{ proto resource enchant_dict_add_to_personal(resource broker)
    A list of UTF-8 encoded suggestions, or false */
PHP_FUNCTION(enchant_dict_add_to_personal)
{
	zval *dict;
	char *word;
	long wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);

	if (pdict) {
		if (pdict->pdict) {
			enchant_dict_add_to_personal(pdict->pdict, word, wordlen);
		}
	}
}
/* }}} */

/* {{{ proto bool enchant_dict_add_to_session(resource broker, string word)
   add 'word' to this spell-checking session */
PHP_FUNCTION(enchant_dict_add_to_session)
{
	zval *dict;
	char *word;
	long wordlen;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);

	if (pdict) {
		if (pdict->pdict) {
			enchant_dict_add_to_session(pdict->pdict, word, wordlen);
		}
	}
}
/* }}} */

/* {{{ proto bool enchant_dict_is_in_session(resource broker)
   whether or not 'word' exists in this spelling-session */
PHP_FUNCTION(enchant_dict_is_in_session)
{
	zval *dict;
	char *word;
	long wordlen;
	long result=0;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &dict, &word, &wordlen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);

	if (pdict) {
		if (pdict->pdict) {
			result = (long)enchant_dict_is_in_session(pdict->pdict, word, wordlen);
		}
	}
	RETURN_BOOL(result);
}
/* }}} */

/* {{{ proto resource enchant_dict_store_replacement(resource broker, string mis, string cor)
	add a correction for 'mis' using 'cor'.
	Notes that you replaced @mis with @cor, so it's possibly more likely
	that future occurrences of @mis will be replaced with @cor. So it might
	bump @cor up in the suggestion list.*/
PHP_FUNCTION(enchant_dict_store_replacement)
{
	zval *dict;
	char *mis;
	long mislen;
	char *cor;
	long corlen;

	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &dict, &mis, &mislen, &cor, &corlen) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);

	if (pdict) {
		if (pdict->pdict) {
			enchant_dict_store_replacement(pdict->pdict, mis, mislen, cor, corlen);
		}
	}
}
/* }}} */

/* {{{ proto resource enchant_dict_get_error(resource dict)
   Returns the last error of the current spelling-session */
PHP_FUNCTION(enchant_dict_get_error)
{
	zval *dict;
	char *msg=NULL;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &dict) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pdict, enchant_broker *, &dict, -1, "enchant broker", le_enchant_broker);

	if (pdict) {
		if (pdict->pdict) {
			msg = enchant_dict_get_error(pdict->pdict);
		}
	}
	if (msg) {
		RETURN_STRING((char *)msg, 1);
	}
}
/* }}} */

/* {{{ proto resource enchant_dict_describe(resource dict)
   Describes an individual dictionary 'dict' */
PHP_FUNCTION(enchant_dict_describe)
{
	zval *dict;
	enchant_dict *pdict;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &dict) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(pdict, enchant_dict *, &dict, -1, "enchant dict", le_enchant_dict);
	if (pdict) {
		if (pdict->pdict) {
			enchant_dict_describe(pdict->pdict, describe_dict_fn, (void *)return_value);
		}
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
