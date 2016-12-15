/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
#define BROWSCAP_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(browscap, v)

#define DEFAULT_SECTION_NAME "Default Browser Capability Settings"

/* OBJECTS_FIXME: This whole extension needs going through. The use of objects looks pretty broken here */

static void browscap_entry_dtor_request(zval *zvalue) /* {{{ */
{
	if (Z_TYPE_P(zvalue) == IS_ARRAY) {
		zend_hash_destroy(Z_ARRVAL_P(zvalue));
		efree(Z_ARR_P(zvalue));
	} else if (Z_TYPE_P(zvalue) == IS_STRING) {
		zend_string_release(Z_STR_P(zvalue));
	}
}
/* }}} */

static void browscap_entry_dtor_persistent(zval *zvalue) /* {{{ */ {
	if (Z_TYPE_P(zvalue) == IS_ARRAY) {
		zend_hash_destroy(Z_ARRVAL_P(zvalue));
		free(Z_ARR_P(zvalue));
	} else if (Z_TYPE_P(zvalue) == IS_STRING) {
		zend_string_release(Z_STR_P(zvalue));
	}
}
/* }}} */

static size_t browscap_compute_regex_len(zend_string *pattern) {
	size_t i, len = ZSTR_LEN(pattern);
	for (i = 0; i < ZSTR_LEN(pattern); i++) {
		switch (ZSTR_VAL(pattern)[i]) {
			case '*':
			case '.':
			case '\\':
			case '(':
			case ')':
			case '~':
			case '+':
				len++;
				break;
		}
	}
	
	return len + sizeof("~^$~")-1;
}

static void convert_browscap_pattern(zval *pattern, int persistent) /* {{{ */
{
	int i, j=0;
	char *t;
	zend_string *res;
	char *lc_pattern;
	ALLOCA_FLAG(use_heap);

	res = zend_string_alloc(browscap_compute_regex_len(Z_STR_P(pattern)), persistent);
	t = ZSTR_VAL(res);

	lc_pattern = do_alloca(Z_STRLEN_P(pattern) + 1, use_heap);
	zend_str_tolower_copy(lc_pattern, Z_STRVAL_P(pattern), Z_STRLEN_P(pattern));

	t[j++] = '~';
	t[j++] = '^';

	for (i=0; i<Z_STRLEN_P(pattern); i++, j++) {
		switch (lc_pattern[i]) {
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
				t[j] = lc_pattern[i];
				break;
		}
	}

	t[j++] = '$';
	t[j++] = '~';

	t[j]=0;
	ZSTR_LEN(res) = j;
	Z_STR_P(pattern) = res;
	free_alloca(lc_pattern, use_heap);
}
/* }}} */

typedef struct _browscap_parser_ctx {
	browser_data *bdata;
	zval current_section;
	zend_string *current_section_name;
	zend_string *str_empty;
	zend_string *str_one;
	zend_string *str_browser_name_pattern;
	zend_string *str_browser_name_regex;
	HashTable str_interned;
} browscap_parser_ctx;

static zend_string *browscap_intern_str(
		browscap_parser_ctx *ctx, zend_string *str) {
	zend_string *interned = zend_hash_find_ptr(&ctx->str_interned, str);
	if (interned) {
		zend_string_addref(interned);
	} else {
		interned = zend_string_copy(str);
		zend_hash_add_new_ptr(&ctx->str_interned, interned, interned);
	}

	return interned;
}

static zend_string *browscap_intern_str_ci(
		browscap_parser_ctx *ctx, zend_string *str, zend_bool persistent) {
	zend_string *lcname;
	zend_string *interned;
	ALLOCA_FLAG(use_heap);

	ZSTR_ALLOCA_ALLOC(lcname, ZSTR_LEN(str), use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(str), ZSTR_LEN(str));
	interned = zend_hash_find_ptr(&ctx->str_interned, lcname);
	ZSTR_ALLOCA_FREE(lcname, use_heap);

	if (interned) {
		zend_string_addref(interned);
	} else {
		interned = zend_string_dup(lcname, persistent);
		zend_hash_add_new_ptr(&ctx->str_interned, interned, interned);
	}

	return interned;
}

static void php_browscap_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg) /* {{{ */
{
	browscap_parser_ctx *ctx = arg;
	browser_data *bdata = ctx->bdata;
	int persistent = bdata->htab->u.flags & HASH_FLAG_PERSISTENT;

	if (!arg1) {
		return;
	}

	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			if (Z_TYPE(ctx->current_section) != IS_UNDEF && arg2) {
				zval new_property;
				zend_string *new_key;

				/* parent entry can not be same as current section -> causes infinite loop! */
				if (!strcasecmp(Z_STRVAL_P(arg1), "parent") &&
					ctx->current_section_name != NULL &&
					!strcasecmp(ZSTR_VAL(ctx->current_section_name), Z_STRVAL_P(arg2))
				) {
					zend_error(E_CORE_ERROR, "Invalid browscap ini file: "
						"'Parent' value cannot be same as the section name: %s "
						"(in file %s)", ZSTR_VAL(ctx->current_section_name), INI_STR("browscap"));
					return;
				}

				/* Set proper value for true/false settings */
				if ((Z_STRLEN_P(arg2) == 2 && !strncasecmp(Z_STRVAL_P(arg2), "on", sizeof("on") - 1)) ||
					(Z_STRLEN_P(arg2) == 3 && !strncasecmp(Z_STRVAL_P(arg2), "yes", sizeof("yes") - 1)) ||
					(Z_STRLEN_P(arg2) == 4 && !strncasecmp(Z_STRVAL_P(arg2), "true", sizeof("true") - 1))
				) {
					ZVAL_STR_COPY(&new_property, ctx->str_one);
				} else if (
					(Z_STRLEN_P(arg2) == 2 && !strncasecmp(Z_STRVAL_P(arg2), "no", sizeof("no") - 1)) ||
					(Z_STRLEN_P(arg2) == 3 && !strncasecmp(Z_STRVAL_P(arg2), "off", sizeof("off") - 1)) ||
					(Z_STRLEN_P(arg2) == 4 && !strncasecmp(Z_STRVAL_P(arg2), "none", sizeof("none") - 1)) ||
					(Z_STRLEN_P(arg2) == 5 && !strncasecmp(Z_STRVAL_P(arg2), "false", sizeof("false") - 1))
				) {
					ZVAL_STR_COPY(&new_property, ctx->str_empty);
				} else { /* Other than true/false setting */
					ZVAL_STR(&new_property, browscap_intern_str(ctx, Z_STR_P(arg2)));
				}

				new_key = browscap_intern_str_ci(ctx, Z_STR_P(arg1), persistent);
				zend_hash_update(Z_ARRVAL(ctx->current_section), new_key, &new_property);
				zend_string_release(new_key);
			}
			break;
		case ZEND_INI_PARSER_SECTION: {
				zval processed;
				zval unprocessed;

				if (persistent) {
					ZVAL_NEW_PERSISTENT_ARR(&ctx->current_section);
				} else {
					ZVAL_NEW_ARR(&ctx->current_section);
				}
				zend_hash_init(Z_ARRVAL(ctx->current_section), 0, NULL,
						(dtor_func_t) (persistent?browscap_entry_dtor_persistent
												 :browscap_entry_dtor_request),
						persistent);
				if (ctx->current_section_name) {
					zend_string_release(ctx->current_section_name);
				}
				ctx->current_section_name = zend_string_copy(Z_STR_P(arg1));

				zend_hash_update(bdata->htab, Z_STR_P(arg1), &ctx->current_section);

				ZVAL_STR(&processed, Z_STR_P(arg1));
				convert_browscap_pattern(&processed, persistent);
				zend_hash_update(Z_ARRVAL(ctx->current_section),
					ctx->str_browser_name_regex, &processed);

				ZVAL_STR_COPY(&unprocessed, Z_STR_P(arg1));
				zend_hash_update(Z_ARRVAL(ctx->current_section),
					ctx->str_browser_name_pattern, &unprocessed);
			}
			break;
	}
}
/* }}} */

static int browscap_read_file(char *filename, browser_data *browdata, int persistent) /* {{{ */
{
	zend_file_handle fh = {{0}};
	browscap_parser_ctx ctx = {0};

	if (filename == NULL || filename[0] == '\0') {
		return FAILURE;
	}

	browdata->htab = pemalloc(sizeof *browdata->htab, persistent);
	if (browdata->htab == NULL) {
		return FAILURE;
	}

	zend_hash_init_ex(browdata->htab, 0, NULL,
			(dtor_func_t) (persistent?browscap_entry_dtor_persistent
									 :browscap_entry_dtor_request),
			persistent, 0);

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
	fh.type = ZEND_HANDLE_FP;

	/* Create parser context */
	ctx.bdata = browdata;
	ZVAL_UNDEF(&ctx.current_section);
	ctx.current_section_name = NULL;
	// TODO: USE ZSTR_EMPTY_ALLOC()?
	ctx.str_empty = zend_string_init("", sizeof("")-1, persistent);
	ctx.str_one = zend_string_init("1", sizeof("1")-1, persistent);
	ctx.str_browser_name_pattern =
		zend_string_init("browser_name_pattern", sizeof("browser_name_pattern")-1, persistent);
	ctx.str_browser_name_regex =
		zend_string_init("browser_name_regex", sizeof("browser_name_regex")-1, persistent);
	zend_hash_init(&ctx.str_interned, 8, NULL, NULL, persistent);

	zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_RAW,
			(zend_ini_parser_cb_t) php_browscap_parser_cb, &ctx);

	/* Destroy parser context */
	if (ctx.current_section_name) {
		zend_string_release(ctx.current_section_name);
	}
	zend_string_release(ctx.str_one);
	zend_string_release(ctx.str_empty);
	zend_string_release(ctx.str_browser_name_regex);
	zend_string_release(ctx.str_browser_name_pattern);
	zend_hash_destroy(&ctx.str_interned);

	return SUCCESS;
}
/* }}} */

#ifdef ZTS
static void browscap_globals_ctor(zend_browscap_globals *browscap_globals) /* {{{ */
{
	browscap_globals->activation_bdata.htab = NULL;
	browscap_globals->activation_bdata.filename[0] = '\0';
}
/* }}} */
#endif

static void browscap_bdata_dtor(browser_data *bdata, int persistent) /* {{{ */
{
	if (bdata->htab != NULL) {
		zend_hash_destroy(bdata->htab);
		pefree(bdata->htab, persistent);
		bdata->htab = NULL;
	}
	bdata->filename[0] = '\0';
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
			browscap_bdata_dtor(bdata, 0);
		}
		if (VCWD_REALPATH(ZSTR_VAL(new_value), bdata->filename) == NULL) {
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
	ts_allocate_id(&browscap_globals_id, sizeof(browser_data), (ts_allocate_ctor) browscap_globals_ctor, NULL);
#endif
	/* ctor call not really needed for non-ZTS */

	if (browscap && browscap[0]) {
		if (browscap_read_file(browscap, &global_bdata, 1) == FAILURE) {
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
		browscap_bdata_dtor(bdata, 0);
	}

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(browscap) /* {{{ */
{
	browscap_bdata_dtor(&global_bdata, 1);

	return SUCCESS;
}
/* }}} */

static int browser_reg_compare(zval *browser, int num_args, va_list args, zend_hash_key *key) /* {{{ */
{
	zval *browser_regex, *previous_match;
	pcre *re;
	int re_options;
	pcre_extra *re_extra;
	char *lookup_browser_name = va_arg(args, char *);
	int lookup_browser_length = va_arg(args, int);
	zval *found_browser_entry = va_arg(args, zval *);

	/* See if we have an exact match, if so, we're done... */
	if (Z_TYPE_P(found_browser_entry) == IS_ARRAY) {
		if ((previous_match = zend_hash_str_find(Z_ARRVAL_P(found_browser_entry), "browser_name_pattern", sizeof("browser_name_pattern")-1)) == NULL) {
			return 0;
		}
		else if (!strcasecmp(Z_STRVAL_P(previous_match), lookup_browser_name)) {
			return 0;
		}
	}

	if ((browser_regex = zend_hash_str_find(Z_ARRVAL_P(browser), "browser_name_regex", sizeof("browser_name_regex")-1)) == NULL) {
		return 0;
	}

	re = pcre_get_compiled_regex(Z_STR_P(browser_regex), &re_extra, &re_options);
	if (re == NULL) {
		return 0;
	}

	if (pcre_exec(re, re_extra, lookup_browser_name, lookup_browser_length, 0, re_options, NULL, 0) == 0) {
		/* If we've found a possible browser, we need to do a comparison of the
		   number of characters changed in the user agent being checked versus
		   the previous match found and the current match. */
		if (Z_TYPE_P(found_browser_entry) == IS_ARRAY) {
			size_t i, prev_len = 0, curr_len = 0;
			zval *current_match = zend_hash_str_find(Z_ARRVAL_P(browser), "browser_name_pattern", sizeof("browser_name_pattern")-1);

			if (!current_match) {
				return 0;
			}

			for (i = 0; i < Z_STRLEN_P(previous_match); i++) {
				switch (Z_STRVAL_P(previous_match)[i]) {
					case '?':
					case '*':
						/* do nothing, ignore these characters in the count */
					break;

					default:
						++prev_len;
				}
			}

			for (i = 0; i < Z_STRLEN_P(current_match); i++) {
				switch (Z_STRVAL_P(current_match)[i]) {
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
			if (prev_len < curr_len) {
				ZVAL_COPY_VALUE(found_browser_entry, browser);
			}
		}
		else {
			ZVAL_COPY_VALUE(found_browser_entry, browser);
		}
	}

	return 0;
}
/* }}} */

static void browscap_zval_copy_ctor(zval *p) /* {{{ */
{
	zval_copy_ctor(p);
}
/* }}} */

/* {{{ proto mixed get_browser([string browser_name [, bool return_array]])
   Get information about the capabilities of a browser. If browser_name is omitted or null, HTTP_USER_AGENT is used. Returns an object by default; if return_array is true, returns an array. */
PHP_FUNCTION(get_browser)
{
	char *agent_name = NULL;
	size_t agent_name_len = 0;
	zend_bool return_array = 0;
	zval *agent, *z_agent_name, *http_user_agent;
	zval found_browser_entry;
	char *lookup_browser_name;
	browser_data *bdata;

	if (BROWSCAP_G(activation_bdata).filename[0] != '\0') {
		bdata = &BROWSCAP_G(activation_bdata);
		if (bdata->htab == NULL) { /* not initialized yet */
			if (browscap_read_file(bdata->filename, bdata, 0) == FAILURE) {
				RETURN_FALSE;
			}
		}
	} else {
		if (!global_bdata.htab) {
			php_error_docref(NULL, E_WARNING, "browscap ini directive not set");
			RETURN_FALSE;
		}
		bdata = &global_bdata;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!b", &agent_name, &agent_name_len, &return_array) == FAILURE) {
		return;
	}

	if (agent_name == NULL) {
		if ((Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY || zend_is_auto_global_str(ZEND_STRL("_SERVER"))) && 
			(http_user_agent = zend_hash_str_find(Z_ARRVAL_P(&PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_USER_AGENT", sizeof("HTTP_USER_AGENT")-1)) == NULL
		) {
			php_error_docref(NULL, E_WARNING, "HTTP_USER_AGENT variable is not set, cannot determine user agent name");
			RETURN_FALSE;
		}
		agent_name = Z_STRVAL_P(http_user_agent);
		agent_name_len = Z_STRLEN_P(http_user_agent);
	}

	lookup_browser_name = estrndup(agent_name, agent_name_len);
	php_strtolower(lookup_browser_name, agent_name_len);

	if ((agent = zend_hash_str_find(bdata->htab, lookup_browser_name, agent_name_len)) == NULL) {
		ZVAL_UNDEF(&found_browser_entry);
		zend_hash_apply_with_arguments(bdata->htab, browser_reg_compare, 3, lookup_browser_name, agent_name_len, &found_browser_entry);

		if (Z_TYPE(found_browser_entry) != IS_UNDEF) {
			agent = &found_browser_entry;
		} else if ((agent = zend_hash_str_find(bdata->htab, DEFAULT_SECTION_NAME, sizeof(DEFAULT_SECTION_NAME)-1)) == NULL) {
			efree(lookup_browser_name);
			RETURN_FALSE;
		}
	}

	if (return_array) {
		RETVAL_ARR(zend_array_dup(Z_ARRVAL_P(agent)));
	}
	else {
		object_init(return_value);
		zend_hash_copy(Z_OBJPROP_P(return_value), Z_ARRVAL_P(agent), (copy_ctor_func_t) browscap_zval_copy_ctor);
	}

	while ((z_agent_name = zend_hash_str_find(Z_ARRVAL_P(agent), "parent", sizeof("parent")-1)) != NULL) {
		if ((agent = zend_hash_find(bdata->htab, Z_STR_P(z_agent_name))) == NULL) {
			break;
		}

		if (return_array) {
			zend_hash_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_P(agent), (copy_ctor_func_t) browscap_zval_copy_ctor, 0);
		}
		else {
			zend_hash_merge(Z_OBJPROP_P(return_value), Z_ARRVAL_P(agent), (copy_ctor_func_t) browscap_zval_copy_ctor, 0);
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
