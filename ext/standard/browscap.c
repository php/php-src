/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_browscap.h"
#include "php_ini.h"
#include "php_string.h"
#include "ext/pcre/php_pcre.h"

#include "zend_ini_scanner.h"
#include "zend_globals.h"

typedef struct {
	HashTable *htab;
	zval *current_section;
	char *current_section_name;
	char filename[MAXPATHLEN];
} browser_data;

/* browser data defined in startup phase, eagerly loaded in MINIT */
static browser_data global_bdata = {0};

/* browser data defined in activation phase, lazily loaded in get_browser.
 * Per request and per thread, if applicable */
ZEND_BEGIN_MODULE_GLOBALS(browscap)
	browser_data activation_bdata;
ZEND_END_MODULE_GLOBALS(browscap)

ZEND_DECLARE_MODULE_GLOBALS(browscap)

#ifdef ZTS
#define BROWSCAP_G(v)	TSRMG(browscap_globals_id, zend_browscap_globals *, v)
#else
#define BROWSCAP_G(v)	(browscap_globals.v)
#endif

#define DEFAULT_SECTION_NAME "Default Browser Capability Settings"

/* OBJECTS_FIXME: This whole extension needs going through. The use of objects looks pretty broken here */

static void browscap_entry_dtor_request(zval **zvalue) /* {{{ */
{
	if (Z_TYPE_PP(zvalue) == IS_ARRAY) {
		zend_hash_destroy(Z_ARRVAL_PP(zvalue));
		efree(Z_ARRVAL_PP(zvalue));
	} else if (Z_TYPE_PP(zvalue) == IS_STRING) {
		if (Z_STRVAL_PP(zvalue)) {
			efree(Z_STRVAL_PP(zvalue));
		}
	}
	efree(*zvalue);
}
/* }}} */

static void browscap_entry_dtor_persistent(zval **zvalue) /* {{{ */ {
	if (Z_TYPE_PP(zvalue) == IS_ARRAY) {
		zend_hash_destroy(Z_ARRVAL_PP(zvalue));
		free(Z_ARRVAL_PP(zvalue));
	} else if (Z_TYPE_PP(zvalue) == IS_STRING) {
		if (Z_STRVAL_PP(zvalue)) {
			free(Z_STRVAL_PP(zvalue));
		}
	}
	free(*zvalue);
}
/* }}} */

static void convert_browscap_pattern(zval *pattern, int persistent) /* {{{ */
{
	int i, j=0;
	char *t;

	php_strtolower(Z_STRVAL_P(pattern), Z_STRLEN_P(pattern));

	t = (char *) safe_pemalloc(Z_STRLEN_P(pattern), 2, 5, persistent);

	t[j++] = '~';
	t[j++] = '^';

	for (i=0; i<Z_STRLEN_P(pattern); i++, j++) {
		switch (Z_STRVAL_P(pattern)[i]) {
			case '?':
				t[j] = '.';
				break;
			case '*':
				t[j++] = '.';
				t[j] = '*';
				break;
			case '.':
				t[j++] = '\\';
				t[j] = '.';
				break;
			case '\\':
				t[j++] = '\\';
				t[j] = '\\';
				break;
			case '(':
				t[j++] = '\\';
				t[j] = '(';
				break;
			case ')':
				t[j++] = '\\';
				t[j] = ')';
				break;
			case '~':
				t[j++] = '\\';
				t[j] = '~';
				break;
			case '+':
				t[j++] = '\\';
				t[j] = '+';
				break;
			default:
				t[j] = Z_STRVAL_P(pattern)[i];
				break;
		}
	}

	t[j++] = '$';
	t[j++] = '~';

	t[j]=0;
	Z_STRVAL_P(pattern) = t;
	Z_STRLEN_P(pattern) = j;
}
/* }}} */

static void php_browscap_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg TSRMLS_DC) /* {{{ */
{
	browser_data *bdata = arg;
	int persistent = bdata->htab->persistent;
	
	if (!arg1) {
		return;
	}

	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			if (bdata->current_section && arg2) {
				zval *new_property;
				char *new_key;

				/* parent entry can not be same as current section -> causes infinite loop! */
				if (!strcasecmp(Z_STRVAL_P(arg1), "parent") && 
					bdata->current_section_name != NULL &&
					!strcasecmp(bdata->current_section_name, Z_STRVAL_P(arg2))
				) {
					zend_error(E_CORE_ERROR, "Invalid browscap ini file: "
						"'Parent' value cannot be same as the section name: %s "
						"(in file %s)", bdata->current_section_name, INI_STR("browscap"));
					return;
				}

				new_property = (zval *) pemalloc(sizeof(zval), persistent);
				INIT_PZVAL(new_property);
				Z_TYPE_P(new_property) = IS_STRING;

				/* Set proper value for true/false settings */
				if ((Z_STRLEN_P(arg2) == 2 && !strncasecmp(Z_STRVAL_P(arg2), "on", sizeof("on") - 1)) ||
					(Z_STRLEN_P(arg2) == 3 && !strncasecmp(Z_STRVAL_P(arg2), "yes", sizeof("yes") - 1)) ||
					(Z_STRLEN_P(arg2) == 4 && !strncasecmp(Z_STRVAL_P(arg2), "true", sizeof("true") - 1))
				) {
					Z_STRVAL_P(new_property) = pestrndup("1", 1, persistent);
					Z_STRLEN_P(new_property) = 1;
				} else if (
					(Z_STRLEN_P(arg2) == 2 && !strncasecmp(Z_STRVAL_P(arg2), "no", sizeof("no") - 1)) ||
					(Z_STRLEN_P(arg2) == 3 && !strncasecmp(Z_STRVAL_P(arg2), "off", sizeof("off") - 1)) ||
					(Z_STRLEN_P(arg2) == 4 && !strncasecmp(Z_STRVAL_P(arg2), "none", sizeof("none") - 1)) ||
					(Z_STRLEN_P(arg2) == 5 && !strncasecmp(Z_STRVAL_P(arg2), "false", sizeof("false") - 1))
				) {
					Z_STRVAL_P(new_property) = pestrndup("", 0, persistent);
					Z_STRLEN_P(new_property) = 0;
				} else { /* Other than true/false setting */
					Z_STRVAL_P(new_property) = pestrndup(Z_STRVAL_P(arg2),
							Z_STRLEN_P(arg2), persistent);
					Z_STRLEN_P(new_property) = Z_STRLEN_P(arg2);
				}
				new_key = pestrndup(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), persistent);
				zend_str_tolower(new_key, Z_STRLEN_P(arg1));
				zend_hash_update(Z_ARRVAL_P(bdata->current_section), new_key, Z_STRLEN_P(arg1) + 1, &new_property, sizeof(zval *), NULL);
				pefree(new_key, persistent);
			}
			break;
		case ZEND_INI_PARSER_SECTION: {
				zval *processed;
				zval *unprocessed;
				HashTable *section_properties;

				/*printf("'%s' (%d)\n",$1.value.str.val,$1.value.str.len + 1);*/
				bdata->current_section = (zval *) pemalloc(sizeof(zval), persistent);
				INIT_PZVAL(bdata->current_section);
				processed = (zval *) pemalloc(sizeof(zval), persistent);
				INIT_PZVAL(processed);
				unprocessed = (zval *) pemalloc(sizeof(zval), persistent);
				INIT_PZVAL(unprocessed);

				section_properties = (HashTable *) pemalloc(sizeof(HashTable), persistent);
				zend_hash_init(section_properties, 0, NULL,
						(dtor_func_t) (persistent?browscap_entry_dtor_persistent
												 :browscap_entry_dtor_request),
						persistent);
				Z_ARRVAL_P(bdata->current_section) = section_properties;
				Z_TYPE_P(bdata->current_section) = IS_ARRAY;
				if (bdata->current_section_name) {
					pefree(bdata->current_section_name, persistent);
				}
				bdata->current_section_name = pestrndup(Z_STRVAL_P(arg1),
						Z_STRLEN_P(arg1), persistent);

				zend_hash_update(bdata->htab, Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, (void *) &bdata->current_section, sizeof(zval *), NULL);

				Z_STRVAL_P(processed) = Z_STRVAL_P(arg1);
				Z_STRLEN_P(processed) = Z_STRLEN_P(arg1);
				Z_TYPE_P(processed) = IS_STRING;
				Z_STRVAL_P(unprocessed) = Z_STRVAL_P(arg1);
				Z_STRLEN_P(unprocessed) = Z_STRLEN_P(arg1);
				Z_TYPE_P(unprocessed) = IS_STRING;
				Z_STRVAL_P(unprocessed) = pestrndup(Z_STRVAL_P(unprocessed), Z_STRLEN_P(unprocessed), persistent);

				convert_browscap_pattern(processed, persistent);
				zend_hash_update(section_properties, "browser_name_regex", sizeof("browser_name_regex"), (void *) &processed, sizeof(zval *), NULL);
				zend_hash_update(section_properties, "browser_name_pattern", sizeof("browser_name_pattern"), (void *) &unprocessed, sizeof(zval *), NULL);
			}
			break;
	}
}
/* }}} */

static int browscap_read_file(char *filename, browser_data *browdata, int persistent TSRMLS_DC) /* {{{ */
{
	zend_file_handle fh = {0};
	
	if (filename == NULL || filename[0] == '\0') {
		return FAILURE;
	}
	
	browdata->htab = pemalloc(sizeof *browdata->htab, persistent);
	if (browdata->htab == NULL) {
		return FAILURE;
	}

	if (zend_hash_init_ex(browdata->htab, 0, NULL,
			(dtor_func_t) (persistent?browscap_entry_dtor_persistent
									 :browscap_entry_dtor_request),
			persistent, 0) == FAILURE) {
		pefree(browdata->htab, persistent);
		browdata->htab = NULL;
		return FAILURE;
	}

	fh.handle.fp = VCWD_FOPEN(filename, "r");
	fh.opened_path = NULL;
	fh.free_filename = 0;
	if (!fh.handle.fp) {
		zend_hash_destroy(browdata->htab);
		pefree(browdata->htab, persistent);
		browdata->htab = NULL;
		zend_error(E_CORE_WARNING, "Cannot open '%s' for reading", filename);
		return FAILURE;
	}
	fh.filename = filename;
	Z_TYPE(fh) = ZEND_HANDLE_FP;
	browdata->current_section_name = NULL;
	zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_RAW,
			(zend_ini_parser_cb_t) php_browscap_parser_cb, browdata TSRMLS_CC);
	if (browdata->current_section_name != NULL) {
		pefree(browdata->current_section_name, persistent);
		browdata->current_section_name = NULL;
	}
	
	return SUCCESS;
}
/* }}} */

#ifdef ZTS
static void browscap_globals_ctor(zend_browscap_globals *browscap_globals TSRMLS_DC) /* {{{ */
{
	browscap_globals->activation_bdata.htab = NULL;
	browscap_globals->activation_bdata.current_section = NULL;
	browscap_globals->activation_bdata.current_section_name = NULL;
	browscap_globals->activation_bdata.filename[0] = '\0';
}
/* }}} */
#endif

static void browscap_bdata_dtor(browser_data *bdata, int persistent TSRMLS_DC) /* {{{ */
{
	if (bdata->htab != NULL) {
		zend_hash_destroy(bdata->htab);
		pefree(bdata->htab, persistent);
		bdata->htab = NULL;
	}
	bdata->filename[0] = '\0';
	/* current_section_* are only used during parsing */
}
/* }}} */

/* {{{ PHP_INI_MH
 */
PHP_INI_MH(OnChangeBrowscap)
{
	if (stage == PHP_INI_STAGE_STARTUP) {
		/* value handled in browscap.c's MINIT */
		return SUCCESS;
	} else if (stage == PHP_INI_STAGE_ACTIVATE) {
		browser_data *bdata = &BROWSCAP_G(activation_bdata);
		if (bdata->filename[0] != '\0') {
			browscap_bdata_dtor(bdata, 0 TSRMLS_CC);
		}
		if (VCWD_REALPATH(new_value, bdata->filename) == NULL) {
			return FAILURE;
		}
		return SUCCESS;
	}
	
	return FAILURE;
}
/* }}} */

PHP_MINIT_FUNCTION(browscap) /* {{{ */
{
	char *browscap = INI_STR("browscap");

#ifdef ZTS
	ts_allocate_id(&browscap_globals_id, sizeof(browser_data),
		browscap_globals_ctor, NULL);
#endif
	/* ctor call not really needed for non-ZTS */

	if (browscap && browscap[0]) {
		if (browscap_read_file(browscap, &global_bdata, 1 TSRMLS_CC) == FAILURE) {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(browscap) /* {{{ */
{
	browser_data *bdata = &BROWSCAP_G(activation_bdata);
	if (bdata->filename[0] != '\0') {
		browscap_bdata_dtor(bdata, 0 TSRMLS_CC);
	}
	
	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(browscap) /* {{{ */
{
	browscap_bdata_dtor(&global_bdata, 1 TSRMLS_CC);
	
	return SUCCESS;
}
/* }}} */

static int browser_reg_compare(zval **browser TSRMLS_DC, int num_args, va_list args, zend_hash_key *key) /* {{{ */
{
	zval **browser_regex, **previous_match;
	pcre *re;
	int re_options;
	pcre_extra *re_extra;
	char *lookup_browser_name = va_arg(args, char *);
	int lookup_browser_length = va_arg(args, int);
	zval **found_browser_entry = va_arg(args, zval **);

	/* See if we have an exact match, if so, we're done... */
	if (*found_browser_entry) {
		if (zend_hash_find(Z_ARRVAL_PP(found_browser_entry), "browser_name_pattern", sizeof("browser_name_pattern"), (void**) &previous_match) == FAILURE) {
			return 0;
		}
		else if (!strcasecmp(Z_STRVAL_PP(previous_match), lookup_browser_name)) {
			return 0;
		}
	}

	if (zend_hash_find(Z_ARRVAL_PP(browser), "browser_name_regex", sizeof("browser_name_regex"), (void **) &browser_regex) == FAILURE) {
		return 0;
	}

	re = pcre_get_compiled_regex(Z_STRVAL_PP(browser_regex), &re_extra, &re_options TSRMLS_CC);
	if (re == NULL) {
		return 0;
	}

	if (pcre_exec(re, re_extra, lookup_browser_name, lookup_browser_length, 0, re_options, NULL, 0) == 0) {
		/* If we've found a possible browser, we need to do a comparison of the
		   number of characters changed in the user agent being checked versus
		   the previous match found and the current match. */
		if (*found_browser_entry) {
			int i, prev_len = 0, curr_len = 0, ua_len;
			zval **current_match;

			if (zend_hash_find(Z_ARRVAL_PP(browser), "browser_name_pattern", sizeof("browser_name_pattern"), (void**) &current_match) == FAILURE) {
				return 0;
			}

			ua_len = lookup_browser_length;

			for (i = 0; i < Z_STRLEN_PP(previous_match); i++) {
				switch (Z_STRVAL_PP(previous_match)[i]) {
					case '?':
					case '*':
						/* do nothing, ignore these characters in the count */
					break;

					default:
						++prev_len;
				}
			}

			for (i = 0; i < Z_STRLEN_PP(current_match); i++) {
				switch (Z_STRVAL_PP(current_match)[i]) {
					case '?':
					case '*':
						/* do nothing, ignore these characters in the count */
					break;

					default:
						++curr_len;
				}
			}

			/* Pick which browser pattern replaces the least amount of
			   characters when compared to the original user agent string... */
			if (ua_len - prev_len > ua_len - curr_len) {
				*found_browser_entry = *browser;
			}
		}
		else {
			*found_browser_entry = *browser;
		}
	}

	return 0;
}
/* }}} */

static void browscap_zval_copy_ctor(zval **p) /* {{{ */
{
	zval *new;

	ALLOC_ZVAL(new);
	*new = **p;

	zval_copy_ctor(new);

	INIT_PZVAL(new);
	*p = new;
} /* }}} */

/* {{{ proto mixed get_browser([string browser_name [, bool return_array]])
   Get information about the capabilities of a browser. If browser_name is omitted or null, HTTP_USER_AGENT is used. Returns an object by default; if return_array is true, returns an array. */
PHP_FUNCTION(get_browser)
{
	char *agent_name = NULL;
	int agent_name_len = 0;
	zend_bool return_array = 0;
	zval **agent, **z_agent_name, **http_user_agent;
	zval *found_browser_entry, *tmp_copy;
	char *lookup_browser_name;
	browser_data *bdata;

	if (BROWSCAP_G(activation_bdata).filename[0] != '\0') {
		bdata = &BROWSCAP_G(activation_bdata);
		if (bdata->htab == NULL) { /* not initialized yet */
			if (browscap_read_file(bdata->filename, bdata, 0 TSRMLS_CC) == FAILURE) {
				RETURN_FALSE;
			}
		}
	} else {
		if (!global_bdata.htab) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "browscap ini directive not set");
			RETURN_FALSE;
		}
		bdata = &global_bdata;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!b", &agent_name, &agent_name_len, &return_array) == FAILURE) {
		return;
	}

	if (agent_name == NULL) {
		zend_is_auto_global("_SERVER", sizeof("_SERVER") - 1 TSRMLS_CC);
		if (!PG(http_globals)[TRACK_VARS_SERVER] ||
			zend_hash_find(HASH_OF(PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_USER_AGENT", sizeof("HTTP_USER_AGENT"), (void **) &http_user_agent) == FAILURE
		) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "HTTP_USER_AGENT variable is not set, cannot determine user agent name");
			RETURN_FALSE;
		}
		agent_name = Z_STRVAL_PP(http_user_agent);
		agent_name_len = Z_STRLEN_PP(http_user_agent);
	}

	lookup_browser_name = estrndup(agent_name, agent_name_len);
	php_strtolower(lookup_browser_name, agent_name_len);

	if (zend_hash_find(bdata->htab, lookup_browser_name, agent_name_len + 1, (void **) &agent) == FAILURE) {
		found_browser_entry = NULL;
		zend_hash_apply_with_arguments(bdata->htab TSRMLS_CC, (apply_func_args_t) browser_reg_compare, 3, lookup_browser_name, agent_name_len, &found_browser_entry);

		if (found_browser_entry) {
			agent = &found_browser_entry;
		} else if (zend_hash_find(bdata->htab, DEFAULT_SECTION_NAME, sizeof(DEFAULT_SECTION_NAME), (void **) &agent) == FAILURE) {
			efree(lookup_browser_name);
			RETURN_FALSE;
		}
	}

	if (return_array) {
		array_init(return_value);
		zend_hash_copy(Z_ARRVAL_P(return_value), Z_ARRVAL_PP(agent), (copy_ctor_func_t) browscap_zval_copy_ctor, (void *) &tmp_copy, sizeof(zval *));
	}
	else {
		object_init(return_value);
		zend_hash_copy(Z_OBJPROP_P(return_value), Z_ARRVAL_PP(agent), (copy_ctor_func_t) browscap_zval_copy_ctor, (void *) &tmp_copy, sizeof(zval *));
	}

	while (zend_hash_find(Z_ARRVAL_PP(agent), "parent", sizeof("parent"), (void **) &z_agent_name) == SUCCESS) {
		if (zend_hash_find(bdata->htab, Z_STRVAL_PP(z_agent_name), Z_STRLEN_PP(z_agent_name) + 1, (void **)&agent) == FAILURE) {
			break;
		}

		if (return_array) {
			zend_hash_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_PP(agent), (copy_ctor_func_t) browscap_zval_copy_ctor, (void *) &tmp_copy, sizeof(zval *), 0);
		}
		else {
			zend_hash_merge(Z_OBJPROP_P(return_value), Z_ARRVAL_PP(agent), (copy_ctor_func_t) browscap_zval_copy_ctor, (void *) &tmp_copy, sizeof(zval *), 0);
		}
	}

	efree(lookup_browser_name);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
