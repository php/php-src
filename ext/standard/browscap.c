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

	res = zend_string_alloc(browscap_compute_regex_len(pattern), persistent);
	t = ZSTR_VAL(res);

	t[j++] = '~';
	t[j++] = '^';

	for (i = 0; i < ZSTR_LEN(pattern); i++, j++) {
		char c = ZSTR_VAL(pattern)[i];
		switch (c) {
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
				t[j] = zend_tolower_ascii(c);
				break;
		}
	}

	t[j++] = '$';
	t[j++] = '~';
	t[j]=0;

	ZSTR_LEN(res) = j;
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
			interned = zend_new_interned_string(interned);
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
	HashTable *ht = zend_new_array(2 + (entry->parent ? 1 : 0) + (entry->kv_end - entry->kv_start));

	ZVAL_STR(&tmp, browscap_convert_pattern(entry->pattern, 0));
	zend_string *key = ZSTR_INIT_LITERAL("browser_name_regex", 0);
	ZSTR_H(key) = zend_inline_hash_func("browser_name_regex", sizeof("browser_name_regex")-1);
	zend_hash_add_new(ht, key, &tmp);
	zend_string_release_ex(key, false);

	ZVAL_STR_COPY(&tmp, entry->pattern);
	key = ZSTR_INIT_LITERAL("browser_name_pattern", 0);
	ZSTR_H(key) = zend_inline_hash_func("browser_name_pattern", sizeof("browser_name_pattern")-1);
	zend_hash_add_new(ht, key, &tmp);
	zend_string_release_ex(key, false);

	if (entry->parent) {
		ZVAL_STR_COPY(&tmp, entry->parent);
		key = ZSTR_INIT_LITERAL("parent", 0);
		ZSTR_H(key) = zend_inline_hash_func("parent", sizeof("parent")-1);
		zend_hash_add_new(ht, key, &tmp);
		zend_string_release_ex(key, false);
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
	/* No dtor because we don't inc the refcount for the reference stored within the hash table's entry value
	 * as the hash table is only temporary anyway. */
	zend_hash_init(&ctx.str_interned, 8, NULL, NULL, persistent);

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

static bool browscap_match_string_wildcard(const char *s, const char *s_end, const char *pattern, const char *pattern_end)
{
	const char *pattern_current = pattern;
	const char *s_current = s;

	const char *wildcard_pattern_restore_pos = NULL;
	const char *wildcard_s_restore_pos = NULL;

	while (s_current < s_end) {
		char pattern_char = *pattern_current;
		char s_char = *s_current;

		if (pattern_char == '*') {
			/* Collapse wildcards */
			pattern_current++;
			while (pattern_current < pattern_end && *pattern_current == '*') {
				pattern_current++;
			}

			/* If we're at the end of the pattern, it means that the ending was just '*', so this is a trivial match */
			if (pattern_current == pattern_end) {
				return true;
			}

			/* Optimization: if there is a non-wildcard character X after a *, then we can immediately jump to the first
			 * character X in s starting from s_current because it is the only way to match beyond the *. */
			if (*pattern_current != '?') {
				while (s_current < s_end && *s_current != *pattern_current) {
					s_current++;
				}
			}

			/* We will first assume the skipped part by * is a 0-length string (or n-length if the optimization above skipped n characters).
			 * When a mismatch happens we will backtrack and move s one position to assume * skipped a 1-length string.
			 * Then 2, 3, 4, ... */
			wildcard_pattern_restore_pos = pattern_current;
			wildcard_s_restore_pos = s_current;

			continue;
		} else if (pattern_char == s_char || pattern_char == '?') {
			/* Match */
			pattern_current++;
			s_current++;

			/* If this was the last character of the pattern, we either fully matched s, or we have a mismatch */
			if (pattern_current == pattern_end) {
				if (s_current == s_end) {
					return true;
				}
				/* Fallthrough to mismatch */
			} else {
				continue;
			}
		}

		/* Mismatch */
		if (wildcard_pattern_restore_pos) {
			pattern_current = wildcard_pattern_restore_pos;
			wildcard_s_restore_pos++;
			s_current = wildcard_s_restore_pos;
		} else {
			/* No wildcard is active, so it is impossible to match */
			return false;
		}
	}

	/* Skip remaining * wildcards, they match nothing here as we are at the end of s */
	while (pattern_current < pattern_end && *pattern_current == '*') {
		pattern_current++;
	}

	ZEND_ASSERT(s_current == s_end);
	return pattern_current == pattern_end;
}

static int browser_reg_compare(browscap_entry *entry, zend_string *agent_name, browscap_entry **found_entry_ptr, size_t *cached_prev_len) /* {{{ */
{
	browscap_entry *found_entry = *found_entry_ptr;
	ALLOCA_FLAG(use_heap)
	zend_string *pattern_lc;
	const char *cur;
	int i;

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
		/* cached_prev_len doesn't matter here because we end the search when an exact match is found. */
		ZSTR_ALLOCA_FREE(pattern_lc, use_heap);
		return 1;
	}

	if (browscap_match_string_wildcard(
		ZSTR_VAL(agent_name) + entry->prefix_len,
		ZSTR_VAL(agent_name) + ZSTR_LEN(agent_name),
		ZSTR_VAL(pattern_lc) + entry->prefix_len,
		ZSTR_VAL(pattern_lc) + ZSTR_LEN(pattern_lc)
	)) {
		/* If we've found a possible browser, we need to do a comparison of the
		   number of characters changed in the user agent being checked versus
		   the previous match found and the current match. */
		size_t curr_len = entry->prefix_len; /* Start from the prefix because the prefix is free of wildcards */
		zend_string *current_match = entry->pattern;
		for (size_t i = curr_len; i < ZSTR_LEN(current_match); i++) {
			switch (ZSTR_VAL(current_match)[i]) {
				case '?':
				case '*':
					/* do nothing, ignore these characters in the count */
				break;

				default:
					++curr_len;
			}
		}

		if (found_entry) {
			/* Pick which browser pattern replaces the least amount of
			   characters when compared to the original user agent string... */
			if (*cached_prev_len < curr_len) {
				*found_entry_ptr = entry;
				*cached_prev_len = curr_len;
			}
		} else {
			*found_entry_ptr = entry;
			*cached_prev_len = curr_len;
		}
	}

	ZSTR_ALLOCA_FREE(pattern_lc, use_heap);
	return 0;
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
		size_t cached_prev_len = 0; /* silence compiler warning */

		ZEND_HASH_MAP_FOREACH_PTR(bdata->htab, entry) {
			/* The following two early-skip checks are inside this loop instead of inside browser_reg_compare().
			 * That's because we want to avoid the call frame overhead, especially as browser_reg_compare() is
			 * a function that uses alloca(). */

			/* Agent name too short */
			if (ZSTR_LEN(lookup_browser_name) < browscap_get_minimum_length(entry)) {
				continue;
			}

			/* Quickly discard patterns where the prefix doesn't match. */
			bool prefix_matches = true;
			for (size_t i = 0; i < entry->prefix_len; i++) {
				if (ZSTR_VAL(lookup_browser_name)[i] != zend_tolower_ascii(ZSTR_VAL(entry->pattern)[i])) {
					prefix_matches = false;
					break;
				}
			}
			if (!prefix_matches) {
				continue;
			}

			if (browser_reg_compare(entry, lookup_browser_name, &found_entry, &cached_prev_len)) {
				break;
			}
		} ZEND_HASH_FOREACH_END();

		if (found_entry == NULL) {
			found_entry = zend_hash_str_find_ptr(bdata->htab,
				DEFAULT_SECTION_NAME, sizeof(DEFAULT_SECTION_NAME)-1);
			if (found_entry == NULL) {
				zend_string_release_ex(lookup_browser_name, false);
				RETURN_FALSE;
			}
		}
	}

	zend_string_release_ex(lookup_browser_name, false);

	agent_ht = browscap_entry_to_array(bdata, found_entry);

	if (return_array) {
		RETVAL_ARR(agent_ht);
	} else {
		object_and_properties_init(return_value, zend_standard_class_def, agent_ht);
	}

	HashTable *target_ht = return_array ? Z_ARRVAL_P(return_value) : Z_OBJPROP_P(return_value);

	while (found_entry->parent) {
		found_entry = zend_hash_find_ptr(bdata->htab, found_entry->parent);
		if (found_entry == NULL) {
			break;
		}

		browscap_entry_add_kv_to_existing_array(bdata, found_entry, target_ht);
	}
}
/* }}} */
