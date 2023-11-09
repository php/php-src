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
   | Author: Zeev Suraski <zeev@php.net>                                  |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_browscap.h"
#include "php_ini.h"
#include "php_string.h"
#include "ext/pcre/php_pcre.h"

#include "zend_ini_scanner.h"
#include "zend_globals.h"

#define BROWSCAP_NUM_CONTAINS 5

typedef struct {
	zend_string *key;
	zend_string *value;
} browscap_kv;

typedef struct {
	zend_string *pattern;
	zend_string *parent;
	uint32_t kv_start;
	uint32_t kv_end;
	/* We ensure that the length fits in 16 bits, so this is fine */
	uint16_t contains_start[BROWSCAP_NUM_CONTAINS];
	uint8_t contains_len[BROWSCAP_NUM_CONTAINS];
	uint8_t prefix_len;
} browscap_entry;

typedef struct {
	HashTable *htab;
	browscap_kv *kv;
	uint32_t kv_used;
	uint32_t kv_size;
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

static void browscap_entry_dtor(zval *zvalue)
{
	browscap_entry *entry = Z_PTR_P(zvalue);
	zend_string_release_ex(entry->pattern, 0);
	if (entry->parent) {
		zend_string_release_ex(entry->parent, 0);
	}
	efree(entry);
}

static void browscap_entry_dtor_persistent(zval *zvalue)
{
	browscap_entry *entry = Z_PTR_P(zvalue);
	zend_string_release_ex(entry->pattern, 1);
	if (entry->parent) {
		zend_string_release_ex(entry->parent, 1);
	}
	pefree(entry, 1);
}

static inline bool is_placeholder(char c) {
	return c == '?' || c == '*';
}

/* Length of prefix not containing any wildcards */
static uint8_t browscap_compute_prefix_len(zend_string *pattern) {
	size_t i;
	for (i = 0; i < ZSTR_LEN(pattern); i++) {
		if (is_placeholder(ZSTR_VAL(pattern)[i])) {
			break;
		}
	}
	return (uint8_t)MIN(i, UINT8_MAX);
}

static size_t browscap_compute_contains(
		zend_string *pattern, size_t start_pos,
		uint16_t *contains_start, uint8_t *contains_len) {
	size_t i = start_pos;
	/* Find first non-placeholder character after prefix */
	for (; i < ZSTR_LEN(pattern); i++) {
		if (!is_placeholder(ZSTR_VAL(pattern)[i])) {
			/* Skip the case of a single non-placeholder character.
			 * Let's try to find something longer instead. */
			if (i + 1 < ZSTR_LEN(pattern) &&
					!is_placeholder(ZSTR_VAL(pattern)[i + 1])) {
				break;
			}
		}
	}
	*contains_start = (uint16_t)i;

	/* Find first placeholder character after that */
	for (; i < ZSTR_LEN(pattern); i++) {
		if (is_placeholder(ZSTR_VAL(pattern)[i])) {
			break;
		}
	}
	*contains_len = (uint8_t)MIN(i - *contains_start, UINT8_MAX);
	return i;
}

/* Length of regex, including escapes, anchors, etc. */
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

static zend_string *browscap_convert_pattern(zend_string *pattern, int persistent) /* {{{ */
{
	size_t i, j=0;
	char *t;
	zend_string *res;
	char *lc_pattern;
	ALLOCA_FLAG(use_heap);

	res = zend_string_alloc(browscap_compute_regex_len(pattern), persistent);
	t = ZSTR_VAL(res);

	lc_pattern = do_alloca(ZSTR_LEN(pattern) + 1, use_heap);
	zend_str_tolower_copy(lc_pattern, ZSTR_VAL(pattern), ZSTR_LEN(pattern));

	t[j++] = '~';
	t[j++] = '^';

	for (i = 0; i < ZSTR_LEN(pattern); i++, j++) {
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
	free_alloca(lc_pattern, use_heap);
	return res;
}
/* }}} */

typedef struct _browscap_parser_ctx {
	browser_data *bdata;
	browscap_entry *current_entry;
	zend_string *current_section_name;
	HashTable str_interned;
} browscap_parser_ctx;

static zend_string *browscap_intern_str(
		browscap_parser_ctx *ctx, zend_string *str, bool persistent) {
	zend_string *interned = zend_hash_find_ptr(&ctx->str_interned, str);
	if (interned) {
		zend_string_addref(interned);
	} else {
		interned = zend_string_copy(str);
		if (persistent) {
			interned = zend_new_interned_string(str);
		}
		zend_hash_add_new_ptr(&ctx->str_interned, interned, interned);
	}

	return interned;
}

static zend_string *browscap_intern_str_ci(
		browscap_parser_ctx *ctx, zend_string *str, bool persistent) {
	zend_string *lcname;
	zend_string *interned;
	ALLOCA_FLAG(use_heap);

	ZSTR_ALLOCA_ALLOC(lcname, ZSTR_LEN(str), use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(str), ZSTR_LEN(str));
	interned = zend_hash_find_ptr(&ctx->str_interned, lcname);

	if (interned) {
		zend_string_addref(interned);
	} else {
		interned = zend_string_init(ZSTR_VAL(lcname), ZSTR_LEN(lcname), persistent);
		if (persistent) {
			interned = zend_new_interned_string(interned);
		}
		zend_hash_add_new_ptr(&ctx->str_interned, interned, interned);
	}

	ZSTR_ALLOCA_FREE(lcname, use_heap);
	return interned;
}

static void browscap_add_kv(
		browser_data *bdata, zend_string *key, zend_string *value, bool persistent) {
	if (bdata->kv_used == bdata->kv_size) {
		bdata->kv_size *= 2;
		bdata->kv = safe_perealloc(bdata->kv, sizeof(browscap_kv), bdata->kv_size, 0, persistent);
	}

	bdata->kv[bdata->kv_used].key = key;
	bdata->kv[bdata->kv_used].value = value;
	bdata->kv_used++;
}

static void browscap_entry_add_kv_to_existing_array(browser_data *bdata, browscap_entry *entry, HashTable *ht) {
	for (uint32_t i = entry->kv_start; i < entry->kv_end; i++) {
		zval tmp;
		ZVAL_STR_COPY(&tmp, bdata->kv[i].value);
		zend_hash_add(ht, bdata->kv[i].key, &tmp);
	}
}

static HashTable *browscap_entry_to_array(browser_data *bdata, browscap_entry *entry) {
	zval tmp;
	HashTable *ht = zend_new_array(8);

	ZVAL_STR(&tmp, browscap_convert_pattern(entry->pattern, 0));
	zend_hash_str_add_new(ht, "browser_name_regex", sizeof("browser_name_regex")-1, &tmp);

	ZVAL_STR_COPY(&tmp, entry->pattern);
	zend_hash_str_add_new(ht, "browser_name_pattern", sizeof("browser_name_pattern")-1, &tmp);

	if (entry->parent) {
		ZVAL_STR_COPY(&tmp, entry->parent);
		zend_hash_str_add_new(ht, "parent", sizeof("parent")-1, &tmp);
	}

	browscap_entry_add_kv_to_existing_array(bdata, entry, ht);

	return ht;
}

static void php_browscap_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg) /* {{{ */
{
	browscap_parser_ctx *ctx = arg;
	browser_data *bdata = ctx->bdata;
	int persistent = GC_FLAGS(bdata->htab) & IS_ARRAY_PERSISTENT;

	if (!arg1) {
		return;
	}

	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			if (ctx->current_entry != NULL && arg2) {
				zend_string *new_key, *new_value;

				/* Set proper value for true/false settings */
				if (zend_string_equals_literal_ci(Z_STR_P(arg2), "on")
					|| zend_string_equals_literal_ci(Z_STR_P(arg2), "yes")
					|| zend_string_equals_literal_ci(Z_STR_P(arg2), "true")
				) {
					new_value = ZSTR_CHAR('1');
				} else if (zend_string_equals_literal_ci(Z_STR_P(arg2), "no")
					|| zend_string_equals_literal_ci(Z_STR_P(arg2), "off")
					|| zend_string_equals_literal_ci(Z_STR_P(arg2), "none")
					|| zend_string_equals_literal_ci(Z_STR_P(arg2), "false")
				) {
					new_value = ZSTR_EMPTY_ALLOC();
				} else { /* Other than true/false setting */
					new_value = browscap_intern_str(ctx, Z_STR_P(arg2), persistent);
				}

				if (zend_string_equals_literal_ci(Z_STR_P(arg1), "parent")) {
					/* parent entry cannot be same as current section -> causes infinite loop! */
					if (ctx->current_section_name != NULL &&
						zend_string_equals_ci(ctx->current_section_name, Z_STR_P(arg2))
					) {
						zend_error(E_CORE_ERROR, "Invalid browscap ini file: "
							"'Parent' value cannot be same as the section name: %s "
							"(in file %s)", ZSTR_VAL(ctx->current_section_name), INI_STR("browscap"));
						return;
					}

					if (ctx->current_entry->parent) {
						zend_string_release(ctx->current_entry->parent);
					}

					ctx->current_entry->parent = new_value;
				} else {
					new_key = browscap_intern_str_ci(ctx, Z_STR_P(arg1), persistent);
					browscap_add_kv(bdata, new_key, new_value, persistent);
					ctx->current_entry->kv_end = bdata->kv_used;
				}
			}
			break;
		case ZEND_INI_PARSER_SECTION:
		{
			browscap_entry *entry;
			zend_string *pattern = Z_STR_P(arg1);
			size_t pos;
			int i;

			if (ZSTR_LEN(pattern) > UINT16_MAX) {
				php_error_docref(NULL, E_WARNING,
					"Skipping excessively long pattern of length %zd", ZSTR_LEN(pattern));
				break;
			}

			if (persistent) {
				pattern = zend_new_interned_string(zend_string_copy(pattern));
				if (ZSTR_IS_INTERNED(pattern)) {
					Z_TYPE_FLAGS_P(arg1) = 0;
				} else {
					zend_string_release(pattern);
				}
			}

			entry = ctx->current_entry
				= pemalloc(sizeof(browscap_entry), persistent);
			zend_hash_update_ptr(bdata->htab, pattern, entry);

			if (ctx->current_section_name) {
				zend_string_release(ctx->current_section_name);
			}
			ctx->current_section_name = zend_string_copy(pattern);

			entry->pattern = zend_string_copy(pattern);
			entry->kv_end = entry->kv_start = bdata->kv_used;
			entry->parent = NULL;

			pos = entry->prefix_len = browscap_compute_prefix_len(pattern);
			for (i = 0; i < BROWSCAP_NUM_CONTAINS; i++) {
				pos = browscap_compute_contains(pattern, pos,
					&entry->contains_start[i], &entry->contains_len[i]);
			}
			break;
		}
	}
}
/* }}} */

static void str_interned_dtor(zval *zv) {
	zend_string_release(Z_STR_P(zv));
}

static int browscap_read_file(char *filename, browser_data *browdata, int persistent) /* {{{ */
{
	zend_file_handle fh;
	browscap_parser_ctx ctx = {0};
	FILE *fp;

	if (filename == NULL || filename[0] == '\0') {
		return FAILURE;
	}

	fp = VCWD_FOPEN(filename, "r");
	if (!fp) {
		zend_error(E_CORE_WARNING, "Cannot open \"%s\" for reading", filename);
		return FAILURE;
	}
	zend_stream_init_fp(&fh, fp, filename);

	browdata->htab = pemalloc(sizeof *browdata->htab, persistent);
	zend_hash_init(browdata->htab, 0, NULL,
		persistent ? browscap_entry_dtor_persistent : browscap_entry_dtor, persistent);

	browdata->kv_size = 16 * 1024;
	browdata->kv_used = 0;
	browdata->kv = pemalloc(sizeof(browscap_kv) * browdata->kv_size, persistent);

	/* Create parser context */
	ctx.bdata = browdata;
	ctx.current_entry = NULL;
	ctx.current_section_name = NULL;
	zend_hash_init(&ctx.str_interned, 8, NULL, str_interned_dtor, persistent);

	zend_parse_ini_file(&fh, persistent, ZEND_INI_SCANNER_RAW,
			(zend_ini_parser_cb_t) php_browscap_parser_cb, &ctx);

	/* Destroy parser context */
	if (ctx.current_section_name) {
		zend_string_release(ctx.current_section_name);
	}
	zend_hash_destroy(&ctx.str_interned);
	zend_destroy_file_handle(&fh);

	return SUCCESS;
}
/* }}} */

#ifdef ZTS
static void browscap_globals_ctor(zend_browscap_globals *browscap_globals) /* {{{ */
{
	browscap_globals->activation_bdata.htab = NULL;
	browscap_globals->activation_bdata.kv = NULL;
	browscap_globals->activation_bdata.filename[0] = '\0';
}
/* }}} */
#endif

static void browscap_bdata_dtor(browser_data *bdata, int persistent) /* {{{ */
{
	if (bdata->htab != NULL) {
		uint32_t i;

		zend_hash_destroy(bdata->htab);
		pefree(bdata->htab, persistent);
		bdata->htab = NULL;

		for (i = 0; i < bdata->kv_used; i++) {
			zend_string_release(bdata->kv[i].key);
			zend_string_release(bdata->kv[i].value);
		}
		pefree(bdata->kv, persistent);
		bdata->kv = NULL;
	}
	bdata->filename[0] = '\0';
}
/* }}} */

/* {{{ PHP_INI_MH */
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

static inline size_t browscap_get_minimum_length(browscap_entry *entry) {
	size_t len = entry->prefix_len;
	int i;
	for (i = 0; i < BROWSCAP_NUM_CONTAINS; i++) {
		len += entry->contains_len[i];
	}
	return len;
}

static int browser_reg_compare(browscap_entry *entry, zend_string *agent_name, browscap_entry **found_entry_ptr) /* {{{ */
{
	browscap_entry *found_entry = *found_entry_ptr;
	ALLOCA_FLAG(use_heap)
	zend_string *pattern_lc, *regex;
	const char *cur;
	int i;

	pcre2_code *re;
	pcre2_match_data *match_data;
	uint32_t capture_count;
	int rc;

	/* Agent name too short */
	if (ZSTR_LEN(agent_name) < browscap_get_minimum_length(entry)) {
		return 0;
	}

	/* Quickly discard patterns where the prefix doesn't match. */
	if (zend_binary_strcasecmp(
			ZSTR_VAL(agent_name), entry->prefix_len,
			ZSTR_VAL(entry->pattern), entry->prefix_len) != 0) {
		return 0;
	}

	/* Lowercase the pattern, the agent name is already lowercase */
	ZSTR_ALLOCA_ALLOC(pattern_lc, ZSTR_LEN(entry->pattern), use_heap);
	zend_str_tolower_copy(ZSTR_VAL(pattern_lc), ZSTR_VAL(entry->pattern), ZSTR_LEN(entry->pattern));

	/* Check if the agent contains the "contains" portions */
	cur = ZSTR_VAL(agent_name) + entry->prefix_len;
	for (i = 0; i < BROWSCAP_NUM_CONTAINS; i++) {
		if (entry->contains_len[i] != 0) {
			cur = zend_memnstr(cur,
				ZSTR_VAL(pattern_lc) + entry->contains_start[i],
				entry->contains_len[i],
				ZSTR_VAL(agent_name) + ZSTR_LEN(agent_name));
			if (!cur) {
				ZSTR_ALLOCA_FREE(pattern_lc, use_heap);
				return 0;
			}
			cur += entry->contains_len[i];
		}
	}

	/* See if we have an exact match, if so, we're done... */
	if (zend_string_equals(agent_name, pattern_lc)) {
		*found_entry_ptr = entry;
		ZSTR_ALLOCA_FREE(pattern_lc, use_heap);
		return 1;
	}

	regex = browscap_convert_pattern(entry->pattern, 0);
	re = pcre_get_compiled_regex(regex, &capture_count);
	if (re == NULL) {
		ZSTR_ALLOCA_FREE(pattern_lc, use_heap);
		zend_string_release(regex);
		return 0;
	}

	match_data = php_pcre_create_match_data(capture_count, re);
	if (!match_data) {
		ZSTR_ALLOCA_FREE(pattern_lc, use_heap);
		zend_string_release(regex);
		return 0;
	}
	rc = pcre2_match(re, (PCRE2_SPTR)ZSTR_VAL(agent_name), ZSTR_LEN(agent_name), 0, 0, match_data, php_pcre_mctx());
	php_pcre_free_match_data(match_data);
	if (rc >= 0) {
		/* If we've found a possible browser, we need to do a comparison of the
		   number of characters changed in the user agent being checked versus
		   the previous match found and the current match. */
		if (found_entry) {
			size_t i, prev_len = 0, curr_len = 0;
			zend_string *previous_match = found_entry->pattern;
			zend_string *current_match = entry->pattern;

			for (i = 0; i < ZSTR_LEN(previous_match); i++) {
				switch (ZSTR_VAL(previous_match)[i]) {
					case '?':
					case '*':
						/* do nothing, ignore these characters in the count */
					break;

					default:
						++prev_len;
				}
			}

			for (i = 0; i < ZSTR_LEN(current_match); i++) {
				switch (ZSTR_VAL(current_match)[i]) {
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
				*found_entry_ptr = entry;
			}
		} else {
			*found_entry_ptr = entry;
		}
	}

	ZSTR_ALLOCA_FREE(pattern_lc, use_heap);
	zend_string_release(regex);
	return 0;
}
/* }}} */

static void browscap_zval_copy_ctor(zval *p) /* {{{ */
{
	if (Z_REFCOUNTED_P(p)) {
		zend_string *str;

		ZEND_ASSERT(Z_TYPE_P(p) == IS_STRING);
		str = Z_STR_P(p);
		if (!(GC_FLAGS(str) & GC_PERSISTENT)) {
			GC_ADDREF(str);
		} else {
			ZVAL_NEW_STR(p, zend_string_init(ZSTR_VAL(str), ZSTR_LEN(str), 0));
		}
	}
}
/* }}} */

/* {{{ Get information about the capabilities of a browser. If browser_name is omitted or null, HTTP_USER_AGENT is used. Returns an object by default; if return_array is true, returns an array. */
PHP_FUNCTION(get_browser)
{
	zend_string *agent_name = NULL, *lookup_browser_name;
	bool return_array = 0;
	browser_data *bdata;
	browscap_entry *found_entry = NULL;
	HashTable *agent_ht;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(agent_name)
		Z_PARAM_BOOL(return_array)
	ZEND_PARSE_PARAMETERS_END();

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

	if (agent_name == NULL) {
		zval *http_user_agent = NULL;
		if (Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY
				|| zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER))) {
			http_user_agent = zend_hash_str_find(
				Z_ARRVAL_P(&PG(http_globals)[TRACK_VARS_SERVER]),
				"HTTP_USER_AGENT", sizeof("HTTP_USER_AGENT")-1);
		}
		if (http_user_agent == NULL) {
			php_error_docref(NULL, E_WARNING, "HTTP_USER_AGENT variable is not set, cannot determine user agent name");
			RETURN_FALSE;
		}
		agent_name = Z_STR_P(http_user_agent);
	}

	lookup_browser_name = zend_string_tolower(agent_name);
	found_entry = zend_hash_find_ptr(bdata->htab, lookup_browser_name);
	if (found_entry == NULL) {
		browscap_entry *entry;

		ZEND_HASH_FOREACH_PTR(bdata->htab, entry) {
			if (browser_reg_compare(entry, lookup_browser_name, &found_entry)) {
				break;
			}
		} ZEND_HASH_FOREACH_END();

		if (found_entry == NULL) {
			found_entry = zend_hash_str_find_ptr(bdata->htab,
				DEFAULT_SECTION_NAME, sizeof(DEFAULT_SECTION_NAME)-1);
			if (found_entry == NULL) {
				zend_string_release(lookup_browser_name);
				RETURN_FALSE;
			}
		}
	}

	agent_ht = browscap_entry_to_array(bdata, found_entry);

	if (return_array) {
		RETVAL_ARR(agent_ht);
	} else {
		object_and_properties_init(return_value, zend_standard_class_def, agent_ht);
	}

	while (found_entry->parent) {
		found_entry = zend_hash_find_ptr(bdata->htab, found_entry->parent);
		if (found_entry == NULL) {
			break;
		}

		agent_ht = browscap_entry_to_array(bdata, found_entry);
		if (return_array) {
			zend_hash_merge(Z_ARRVAL_P(return_value), agent_ht, (copy_ctor_func_t) browscap_zval_copy_ctor, 0);
		} else {
			zend_hash_merge(Z_OBJPROP_P(return_value), agent_ht, (copy_ctor_func_t) browscap_zval_copy_ctor, 0);
		}

		zend_hash_destroy(agent_ht);
		efree(agent_ht);
	}

	zend_string_release_ex(lookup_browser_name, 0);
}
/* }}} */
