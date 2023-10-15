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
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_pcre.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#include "zend_smart_str.h"
#include "SAPI.h"

#include "ext/standard/php_string.h"

#define PREG_PATTERN_ORDER			1
#define PREG_SET_ORDER				2
#define PREG_OFFSET_CAPTURE			(1<<8)
#define PREG_UNMATCHED_AS_NULL		(1<<9)

#define	PREG_SPLIT_NO_EMPTY			(1<<0)
#define PREG_SPLIT_DELIM_CAPTURE	(1<<1)
#define PREG_SPLIT_OFFSET_CAPTURE	(1<<2)

#define PREG_GREP_INVERT			(1<<0)

#define PREG_JIT                    (1<<3)

#define PCRE_CACHE_SIZE 4096

#ifdef HAVE_PCRE_JIT_SUPPORT
#define PHP_PCRE_JIT_SUPPORT 1
#else
#define PHP_PCRE_JIT_SUPPORT 0
#endif

char *php_pcre_version;

#include "php_pcre_arginfo.h"

struct _pcre_cache_entry {
	pcre2_code *re;
	uint32_t preg_options;
	uint32_t capture_count;
	uint32_t name_count;
	uint32_t compile_options;
	uint32_t refcount;
};

PHPAPI ZEND_DECLARE_MODULE_GLOBALS(pcre)

#ifdef HAVE_PCRE_JIT_SUPPORT
#define PCRE_JIT_STACK_MIN_SIZE (32 * 1024)
#define PCRE_JIT_STACK_MAX_SIZE (192 * 1024)
ZEND_TLS pcre2_jit_stack *jit_stack = NULL;
#endif
/* General context using (infallible) system allocator. */
ZEND_TLS pcre2_general_context *gctx = NULL;
/* These two are global per thread for now. Though it is possible to use these
 	per pattern. Either one can copy it and use in pce, or one does no global
	contexts at all, but creates for every pce. */
ZEND_TLS pcre2_compile_context *cctx = NULL;
ZEND_TLS pcre2_match_context   *mctx = NULL;
ZEND_TLS pcre2_match_data      *mdata = NULL;
ZEND_TLS bool              mdata_used = 0;
ZEND_TLS uint8_t pcre2_init_ok = 0;
#if defined(ZTS) && defined(HAVE_PCRE_JIT_SUPPORT)
static MUTEX_T pcre_mt = NULL;
#define php_pcre_mutex_alloc() \
	if (tsrm_is_main_thread() && !pcre_mt) pcre_mt = tsrm_mutex_alloc();
#define php_pcre_mutex_free() \
	if (tsrm_is_main_thread() && pcre_mt) { tsrm_mutex_free(pcre_mt); pcre_mt = NULL; }
#define php_pcre_mutex_lock() tsrm_mutex_lock(pcre_mt);
#define php_pcre_mutex_unlock() tsrm_mutex_unlock(pcre_mt);
#else
#define php_pcre_mutex_alloc()
#define php_pcre_mutex_free()
#define php_pcre_mutex_lock()
#define php_pcre_mutex_unlock()
#endif

ZEND_TLS HashTable char_tables;

static void php_pcre_free_char_table(zval *data)
{/*{{{*/
	void *ptr = Z_PTR_P(data);
	pefree(ptr, 1);
}/*}}}*/

static void pcre_handle_exec_error(int pcre_code) /* {{{ */
{
	int preg_code = 0;

	switch (pcre_code) {
		case PCRE2_ERROR_MATCHLIMIT:
			preg_code = PHP_PCRE_BACKTRACK_LIMIT_ERROR;
			break;

		case PCRE2_ERROR_RECURSIONLIMIT:
			preg_code = PHP_PCRE_RECURSION_LIMIT_ERROR;
			break;

		case PCRE2_ERROR_BADUTFOFFSET:
			preg_code = PHP_PCRE_BAD_UTF8_OFFSET_ERROR;
			break;

#ifdef HAVE_PCRE_JIT_SUPPORT
		case PCRE2_ERROR_JIT_STACKLIMIT:
			preg_code = PHP_PCRE_JIT_STACKLIMIT_ERROR;
			break;
#endif

		default:
			if (pcre_code <= PCRE2_ERROR_UTF8_ERR1 && pcre_code >= PCRE2_ERROR_UTF8_ERR21) {
				preg_code = PHP_PCRE_BAD_UTF8_ERROR;
			} else  {
				preg_code = PHP_PCRE_INTERNAL_ERROR;
			}
			break;
	}

	PCRE_G(error_code) = preg_code;
}
/* }}} */

static const char *php_pcre_get_error_msg(php_pcre_error_code error_code) /* {{{ */
{
	switch (error_code) {
		case PHP_PCRE_NO_ERROR:
			return "No error";
		case PHP_PCRE_INTERNAL_ERROR:
			return "Internal error";
		case PHP_PCRE_BAD_UTF8_ERROR:
			return "Malformed UTF-8 characters, possibly incorrectly encoded";
		case PHP_PCRE_BAD_UTF8_OFFSET_ERROR:
			return "The offset did not correspond to the beginning of a valid UTF-8 code point";
		case PHP_PCRE_BACKTRACK_LIMIT_ERROR:
			return "Backtrack limit exhausted";
		case PHP_PCRE_RECURSION_LIMIT_ERROR:
			return "Recursion limit exhausted";

#ifdef HAVE_PCRE_JIT_SUPPORT
		case PHP_PCRE_JIT_STACKLIMIT_ERROR:
			return "JIT stack limit exhausted";
#endif

		default:
			return "Unknown error";
	}
}
/* }}} */

static void php_free_pcre_cache(zval *data) /* {{{ */
{
	pcre_cache_entry *pce = (pcre_cache_entry *) Z_PTR_P(data);
	if (!pce) return;
	pcre2_code_free(pce->re);
	free(pce);
}
/* }}} */

static void php_efree_pcre_cache(zval *data) /* {{{ */
{
	pcre_cache_entry *pce = (pcre_cache_entry *) Z_PTR_P(data);
	if (!pce) return;
	pcre2_code_free(pce->re);
	efree(pce);
}
/* }}} */

static void *php_pcre_malloc(PCRE2_SIZE size, void *data)
{
	return pemalloc(size, 1);
}

static void php_pcre_free(void *block, void *data)
{
	pefree(block, 1);
}

static void *php_pcre_emalloc(PCRE2_SIZE size, void *data)
{
	return emalloc(size);
}

static void php_pcre_efree(void *block, void *data)
{
	efree(block);
}

#ifdef PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK
	/* pcre 10.38 needs PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK, disabled by default */
#define PHP_PCRE_DEFAULT_EXTRA_COPTIONS PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK
#else
#define PHP_PCRE_DEFAULT_EXTRA_COPTIONS 0
#endif

#define PHP_PCRE_PREALLOC_MDATA_SIZE 32

static void php_pcre_init_pcre2(uint8_t jit)
{/*{{{*/
	if (!gctx) {
		gctx = pcre2_general_context_create(php_pcre_malloc, php_pcre_free, NULL);
		if (!gctx) {
			pcre2_init_ok = 0;
			return;
		}
	}

	if (!cctx) {
		cctx = pcre2_compile_context_create(gctx);
		if (!cctx) {
			pcre2_init_ok = 0;
			return;
		}
	}

	pcre2_set_compile_extra_options(cctx, PHP_PCRE_DEFAULT_EXTRA_COPTIONS);

	if (!mctx) {
		mctx = pcre2_match_context_create(gctx);
		if (!mctx) {
			pcre2_init_ok = 0;
			return;
		}
	}

#ifdef HAVE_PCRE_JIT_SUPPORT
	if (jit && !jit_stack) {
		jit_stack = pcre2_jit_stack_create(PCRE_JIT_STACK_MIN_SIZE, PCRE_JIT_STACK_MAX_SIZE, gctx);
		if (!jit_stack) {
			pcre2_init_ok = 0;
			return;
		}
	}
#endif

	if (!mdata) {
		mdata = pcre2_match_data_create(PHP_PCRE_PREALLOC_MDATA_SIZE, gctx);
		if (!mdata) {
			pcre2_init_ok = 0;
			return;
		}
	}

	pcre2_init_ok = 1;
}/*}}}*/

static void php_pcre_shutdown_pcre2(void)
{/*{{{*/
	if (gctx) {
		pcre2_general_context_free(gctx);
		gctx = NULL;
	}

	if (cctx) {
		pcre2_compile_context_free(cctx);
		cctx = NULL;
	}

	if (mctx) {
		pcre2_match_context_free(mctx);
		mctx = NULL;
	}

#ifdef HAVE_PCRE_JIT_SUPPORT
	/* Stack may only be destroyed when no cached patterns
	 	possibly associated with it do exist. */
	if (jit_stack) {
		pcre2_jit_stack_free(jit_stack);
		jit_stack = NULL;
	}
#endif

	if (mdata) {
		pcre2_match_data_free(mdata);
		mdata = NULL;
	}

	pcre2_init_ok = 0;
}/*}}}*/

static PHP_GINIT_FUNCTION(pcre) /* {{{ */
{
	php_pcre_mutex_alloc();

	/* If we're on the CLI SAPI, there will only be one request, so we don't need the
	 * cache to survive after RSHUTDOWN. */
	pcre_globals->per_request_cache = strcmp(sapi_module.name, "cli") == 0;
	if (!pcre_globals->per_request_cache) {
		zend_hash_init(&pcre_globals->pcre_cache, 0, NULL, php_free_pcre_cache, 1);
	}

	pcre_globals->backtrack_limit = 0;
	pcre_globals->recursion_limit = 0;
	pcre_globals->error_code      = PHP_PCRE_NO_ERROR;
	ZVAL_UNDEF(&pcre_globals->unmatched_null_pair);
	ZVAL_UNDEF(&pcre_globals->unmatched_empty_pair);
#ifdef HAVE_PCRE_JIT_SUPPORT
	pcre_globals->jit = 1;
#endif

	php_pcre_init_pcre2(1);
	zend_hash_init(&char_tables, 1, NULL, php_pcre_free_char_table, 1);
}
/* }}} */

static PHP_GSHUTDOWN_FUNCTION(pcre) /* {{{ */
{
	if (!pcre_globals->per_request_cache) {
		zend_hash_destroy(&pcre_globals->pcre_cache);
	}

	php_pcre_shutdown_pcre2();
	zend_hash_destroy(&char_tables);
	php_pcre_mutex_free();
}
/* }}} */

static PHP_INI_MH(OnUpdateBacktrackLimit)
{/*{{{*/
	OnUpdateLong(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	if (mctx) {
		pcre2_set_match_limit(mctx, (uint32_t)PCRE_G(backtrack_limit));
	}

	return SUCCESS;
}/*}}}*/

static PHP_INI_MH(OnUpdateRecursionLimit)
{/*{{{*/
	OnUpdateLong(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	if (mctx) {
		pcre2_set_depth_limit(mctx, (uint32_t)PCRE_G(recursion_limit));
	}

	return SUCCESS;
}/*}}}*/

#ifdef HAVE_PCRE_JIT_SUPPORT
static PHP_INI_MH(OnUpdateJit)
{/*{{{*/
	OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	if (PCRE_G(jit) && jit_stack) {
		pcre2_jit_stack_assign(mctx, NULL, jit_stack);
	} else {
		pcre2_jit_stack_assign(mctx, NULL, NULL);
	}

	return SUCCESS;
}/*}}}*/
#endif

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("pcre.backtrack_limit", "1000000", PHP_INI_ALL, OnUpdateBacktrackLimit, backtrack_limit, zend_pcre_globals, pcre_globals)
	STD_PHP_INI_ENTRY("pcre.recursion_limit", "100000",  PHP_INI_ALL, OnUpdateRecursionLimit, recursion_limit, zend_pcre_globals, pcre_globals)
#ifdef HAVE_PCRE_JIT_SUPPORT
	STD_PHP_INI_BOOLEAN("pcre.jit",           "1",       PHP_INI_ALL, OnUpdateJit,            jit,             zend_pcre_globals, pcre_globals)
#endif
PHP_INI_END()

static char *_pcre2_config_str(uint32_t what)
{/*{{{*/
	int len = pcre2_config(what, NULL);
	char *ret = (char *) malloc(len + 1);

	len = pcre2_config(what, ret);
	if (!len) {
		free(ret);
		return NULL;
	}

	return ret;
}/*}}}*/

/* {{{ PHP_MINFO_FUNCTION(pcre) */
static PHP_MINFO_FUNCTION(pcre)
{
#ifdef HAVE_PCRE_JIT_SUPPORT
	uint32_t flag = 0;
	char *jit_target = _pcre2_config_str(PCRE2_CONFIG_JITTARGET);
#endif
	char *version = _pcre2_config_str(PCRE2_CONFIG_VERSION);
	char *unicode = _pcre2_config_str(PCRE2_CONFIG_UNICODE_VERSION);

	php_info_print_table_start();
	php_info_print_table_row(2, "PCRE (Perl Compatible Regular Expressions) Support", "enabled" );
	php_info_print_table_row(2, "PCRE Library Version", version);
	free(version);
	php_info_print_table_row(2, "PCRE Unicode Version", unicode);
	free(unicode);

#ifdef HAVE_PCRE_JIT_SUPPORT
	if (!pcre2_config(PCRE2_CONFIG_JIT, &flag)) {
		php_info_print_table_row(2, "PCRE JIT Support", flag ? "enabled" : "disabled");
	} else {
		php_info_print_table_row(2, "PCRE JIT Support", "unknown" );
	}
	if (jit_target) {
		php_info_print_table_row(2, "PCRE JIT Target", jit_target);
	}
	free(jit_target);
#else
	php_info_print_table_row(2, "PCRE JIT Support", "not compiled in" );
#endif

#ifdef HAVE_PCRE_VALGRIND_SUPPORT
	php_info_print_table_row(2, "PCRE Valgrind Support", "enabled" );
#endif

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(pcre) */
static PHP_MINIT_FUNCTION(pcre)
{
#ifdef HAVE_PCRE_JIT_SUPPORT
	if (UNEXPECTED(!pcre2_init_ok)) {
		/* Retry. */
		php_pcre_init_pcre2(PCRE_G(jit));
		if (!pcre2_init_ok) {
			return FAILURE;
		}
	}
#endif

	REGISTER_INI_ENTRIES();

	php_pcre_version = _pcre2_config_str(PCRE2_CONFIG_VERSION);

	register_php_pcre_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(pcre) */
static PHP_MSHUTDOWN_FUNCTION(pcre)
{
	UNREGISTER_INI_ENTRIES();

	free(php_pcre_version);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(pcre) */
static PHP_RINIT_FUNCTION(pcre)
{
#ifdef HAVE_PCRE_JIT_SUPPORT
	if (UNEXPECTED(!pcre2_init_ok)) {
		/* Retry. */
		php_pcre_mutex_lock();
		php_pcre_init_pcre2(PCRE_G(jit));
		if (!pcre2_init_ok) {
			php_pcre_mutex_unlock();
			return FAILURE;
		}
		php_pcre_mutex_unlock();
	}

	mdata_used = 0;
#endif

	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;
	PCRE_G(gctx_zmm) = pcre2_general_context_create(php_pcre_emalloc, php_pcre_efree, NULL);
	if (!PCRE_G(gctx_zmm)) {
		return FAILURE;
	}

	if (PCRE_G(per_request_cache)) {
		zend_hash_init(&PCRE_G(pcre_cache), 0, NULL, php_efree_pcre_cache, 0);
	}

	return SUCCESS;
}
/* }}} */

static PHP_RSHUTDOWN_FUNCTION(pcre)
{
	pcre2_general_context_free(PCRE_G(gctx_zmm));
	PCRE_G(gctx_zmm) = NULL;

	if (PCRE_G(per_request_cache)) {
		zend_hash_destroy(&PCRE_G(pcre_cache));
	}

	zval_ptr_dtor(&PCRE_G(unmatched_null_pair));
	zval_ptr_dtor(&PCRE_G(unmatched_empty_pair));
	ZVAL_UNDEF(&PCRE_G(unmatched_null_pair));
	ZVAL_UNDEF(&PCRE_G(unmatched_empty_pair));
	return SUCCESS;
}

/* {{{ static pcre_clean_cache */
static int pcre_clean_cache(zval *data, void *arg)
{
	pcre_cache_entry *pce = (pcre_cache_entry *) Z_PTR_P(data);
	int *num_clean = (int *)arg;

	if (*num_clean > 0 && !pce->refcount) {
		(*num_clean)--;
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

static void free_subpats_table(zend_string **subpat_names, uint32_t num_subpats) {
	uint32_t i;
	for (i = 0; i < num_subpats; i++) {
		if (subpat_names[i]) {
			zend_string_release(subpat_names[i]);
		}
	}
	efree(subpat_names);
}

/* {{{ static make_subpats_table */
static zend_string **make_subpats_table(uint32_t num_subpats, pcre_cache_entry *pce)
{
	uint32_t name_cnt = pce->name_count, name_size, ni = 0;
	char *name_table;
	zend_string **subpat_names;
	int rc1, rc2;

	rc1 = pcre2_pattern_info(pce->re, PCRE2_INFO_NAMETABLE, &name_table);
	rc2 = pcre2_pattern_info(pce->re, PCRE2_INFO_NAMEENTRYSIZE, &name_size);
	if (rc1 < 0 || rc2 < 0) {
		php_error_docref(NULL, E_WARNING, "Internal pcre2_pattern_info() error %d", rc1 < 0 ? rc1 : rc2);
		return NULL;
	}

	subpat_names = ecalloc(num_subpats, sizeof(zend_string *));
	while (ni++ < name_cnt) {
		unsigned short name_idx = 0x100 * (unsigned char)name_table[0] + (unsigned char)name_table[1];
		const char *name = name_table + 2;
		subpat_names[name_idx] = zend_string_init(name, strlen(name), 0);
		if (is_numeric_string(ZSTR_VAL(subpat_names[name_idx]), ZSTR_LEN(subpat_names[name_idx]), NULL, NULL, 0) > 0) {
			php_error_docref(NULL, E_WARNING, "Numeric named subpatterns are not allowed");
			free_subpats_table(subpat_names, num_subpats);
			return NULL;
		}
		name_table += name_size;
	}
	return subpat_names;
}
/* }}} */

/* {{{ static calculate_unit_length */
/* Calculates the byte length of the next character. Assumes valid UTF-8 for PCRE2_UTF. */
static zend_always_inline size_t calculate_unit_length(pcre_cache_entry *pce, const char *start)
{
	size_t unit_len;

	if (pce->compile_options & PCRE2_UTF) {
		const char *end = start;

		/* skip continuation bytes */
		while ((*++end & 0xC0) == 0x80);
		unit_len = end - start;
	} else {
		unit_len = 1;
	}
	return unit_len;
}
/* }}} */

/* {{{ pcre_get_compiled_regex_cache */
PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache_ex(zend_string *regex, int locale_aware)
{
	pcre2_code			*re = NULL;
#if 10 == PCRE2_MAJOR && 37 == PCRE2_MINOR && !HAVE_BUNDLED_PCRE
	uint32_t			 coptions = PCRE2_NO_START_OPTIMIZE;
#else
	uint32_t			 coptions = 0;
#endif
	PCRE2_UCHAR	         error[128];
	PCRE2_SIZE           erroffset;
	int                  errnumber;
	char				 delimiter;
	char				 start_delimiter;
	char				 end_delimiter;
	char				*p, *pp;
	char				*pattern;
	size_t				 pattern_len;
	uint32_t			 poptions = 0;
	const uint8_t       *tables = NULL;
	zval                *zv;
	pcre_cache_entry	 new_entry;
	int					 rc;
	zend_string 		*key;
	pcre_cache_entry	*ret;

	if (locale_aware && BG(ctype_string)) {
		key = zend_string_concat2(
			ZSTR_VAL(BG(ctype_string)), ZSTR_LEN(BG(ctype_string)),
			ZSTR_VAL(regex), ZSTR_LEN(regex));
	} else {
		key = regex;
	}

	/* Try to lookup the cached regex entry, and if successful, just pass
	   back the compiled pattern, otherwise go on and compile it. */
	zv = zend_hash_find(&PCRE_G(pcre_cache), key);
	if (zv) {
		if (key != regex) {
			zend_string_release_ex(key, 0);
		}
		return (pcre_cache_entry*)Z_PTR_P(zv);
	}

	p = ZSTR_VAL(regex);
	const char* end_p = ZSTR_VAL(regex) + ZSTR_LEN(regex);

	/* Parse through the leading whitespace, and display a warning if we
	   get to the end without encountering a delimiter. */
	while (isspace((int)*(unsigned char *)p)) p++;
	if (p >= end_p) {
		if (key != regex) {
			zend_string_release_ex(key, 0);
		}
		php_error_docref(NULL, E_WARNING, "Empty regular expression");
		pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
		return NULL;
	}

	/* Get the delimiter and display a warning if it is alphanumeric
	   or a backslash. */
	delimiter = *p++;
	if (isalnum((int)*(unsigned char *)&delimiter) || delimiter == '\\' || delimiter == '\0') {
		if (key != regex) {
			zend_string_release_ex(key, 0);
		}
		php_error_docref(NULL, E_WARNING, "Delimiter must not be alphanumeric, backslash, or NUL");
		pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
		return NULL;
	}

	start_delimiter = delimiter;
	if ((pp = strchr("([{< )]}> )]}>", delimiter)))
		delimiter = pp[5];
	end_delimiter = delimiter;

	pp = p;

	if (start_delimiter == end_delimiter) {
		/* We need to iterate through the pattern, searching for the ending delimiter,
		   but skipping the backslashed delimiters.  If the ending delimiter is not
		   found, display a warning. */
		while (pp < end_p) {
			if (*pp == '\\' && pp + 1 < end_p) pp++;
			else if (*pp == delimiter)
				break;
			pp++;
		}
	} else {
		/* We iterate through the pattern, searching for the matching ending
		 * delimiter. For each matching starting delimiter, we increment nesting
		 * level, and decrement it for each matching ending delimiter. If we
		 * reach the end of the pattern without matching, display a warning.
		 */
		int brackets = 1; 	/* brackets nesting level */
		while (pp < end_p) {
			if (*pp == '\\' && pp + 1 < end_p) pp++;
			else if (*pp == end_delimiter && --brackets <= 0)
				break;
			else if (*pp == start_delimiter)
				brackets++;
			pp++;
		}
	}

	if (pp >= end_p) {
		if (key != regex) {
			zend_string_release_ex(key, 0);
		}
		if (start_delimiter == end_delimiter) {
			php_error_docref(NULL,E_WARNING, "No ending delimiter '%c' found", delimiter);
		} else {
			php_error_docref(NULL,E_WARNING, "No ending matching delimiter '%c' found", delimiter);
		}
		pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
		return NULL;
	}

	/* Make a copy of the actual pattern. */
	pattern_len = pp - p;
	pattern = estrndup(p, pattern_len);

	/* Move on to the options */
	pp++;

	/* Parse through the options, setting appropriate flags.  Display
	   a warning if we encounter an unknown modifier. */
	while (pp < end_p) {
		switch (*pp++) {
			/* Perl compatible options */
			case 'i':	coptions |= PCRE2_CASELESS;		break;
			case 'm':	coptions |= PCRE2_MULTILINE;		break;
			case 'n':	coptions |= PCRE2_NO_AUTO_CAPTURE;	break;
			case 's':	coptions |= PCRE2_DOTALL;		break;
			case 'x':	coptions |= PCRE2_EXTENDED;		break;

			/* PCRE specific options */
			case 'A':	coptions |= PCRE2_ANCHORED;		break;
			case 'D':	coptions |= PCRE2_DOLLAR_ENDONLY;break;
			case 'S':	/* Pass. */					break;
			case 'X':	/* Pass. */					break;
			case 'U':	coptions |= PCRE2_UNGREEDY;		break;
			case 'u':	coptions |= PCRE2_UTF;
	/* In  PCRE,  by  default, \d, \D, \s, \S, \w, and \W recognize only ASCII
	   characters, even in UTF-8 mode. However, this can be changed by setting
	   the PCRE2_UCP option. */
#ifdef PCRE2_UCP
						coptions |= PCRE2_UCP;
#endif
				break;
			case 'J':	coptions |= PCRE2_DUPNAMES;		break;

			case ' ':
			case '\n':
			case '\r':
				break;

			case 'e': /* legacy eval */
			default:
				if (pp[-1]) {
					php_error_docref(NULL, E_WARNING, "Unknown modifier '%c'", pp[-1]);
				} else {
					php_error_docref(NULL, E_WARNING, "NUL is not a valid modifier");
				}
				pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
				efree(pattern);
				if (key != regex) {
					zend_string_release_ex(key, 0);
				}
				return NULL;
		}
	}

	if (key != regex) {
		tables = (uint8_t *)zend_hash_find_ptr(&char_tables, BG(ctype_string));
		if (!tables) {
			zend_string *_k;
			tables = pcre2_maketables(gctx);
			if (UNEXPECTED(!tables)) {
				php_error_docref(NULL,E_WARNING, "Failed to generate locale character tables");
				pcre_handle_exec_error(PCRE2_ERROR_NOMEMORY);
				zend_string_release_ex(key, 0);
				efree(pattern);
				return NULL;
			}
			_k = zend_string_init(ZSTR_VAL(BG(ctype_string)), ZSTR_LEN(BG(ctype_string)), 1);
			GC_MAKE_PERSISTENT_LOCAL(_k);
			zend_hash_add_ptr(&char_tables, _k, (void *)tables);
			zend_string_release(_k);
		}
	}
	pcre2_set_character_tables(cctx, tables);

	/* Compile pattern and display a warning if compilation failed. */
	re = pcre2_compile((PCRE2_SPTR)pattern, pattern_len, coptions, &errnumber, &erroffset, cctx);

	if (re == NULL) {
		if (key != regex) {
			zend_string_release_ex(key, 0);
		}
		pcre2_get_error_message(errnumber, error, sizeof(error));
		php_error_docref(NULL,E_WARNING, "Compilation failed: %s at offset %zu", error, erroffset);
		pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
		efree(pattern);
		return NULL;
	}

#ifdef HAVE_PCRE_JIT_SUPPORT
	if (PCRE_G(jit)) {
		/* Enable PCRE JIT compiler */
		rc = pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);
		if (EXPECTED(rc >= 0)) {
			size_t jit_size = 0;
			if (!pcre2_pattern_info(re, PCRE2_INFO_JITSIZE, &jit_size) && jit_size > 0) {
				poptions |= PREG_JIT;
			}
		} else if (rc == PCRE2_ERROR_NOMEMORY) {
			php_error_docref(NULL, E_WARNING,
				"Allocation of JIT memory failed, PCRE JIT will be disabled. "
				"This is likely caused by security restrictions. "
				"Either grant PHP permission to allocate executable memory, or set pcre.jit=0");
			PCRE_G(jit) = 0;
		} else {
			pcre2_get_error_message(rc, error, sizeof(error));
			php_error_docref(NULL, E_WARNING, "JIT compilation failed: %s", error);
			pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
		}
	}
#endif
	efree(pattern);

	/*
	 * If we reached cache limit, clean out the items from the head of the list;
	 * these are supposedly the oldest ones (but not necessarily the least used
	 * ones).
	 */
	if (zend_hash_num_elements(&PCRE_G(pcre_cache)) == PCRE_CACHE_SIZE) {
		int num_clean = PCRE_CACHE_SIZE / 8;
		zend_hash_apply_with_argument(&PCRE_G(pcre_cache), pcre_clean_cache, &num_clean);
	}

	/* Store the compiled pattern and extra info in the cache. */
	new_entry.re = re;
	new_entry.preg_options = poptions;
	new_entry.compile_options = coptions;
	new_entry.refcount = 0;

	rc = pcre2_pattern_info(re, PCRE2_INFO_CAPTURECOUNT, &new_entry.capture_count);
	if (rc < 0) {
		if (key != regex) {
			zend_string_release_ex(key, 0);
		}
		php_error_docref(NULL, E_WARNING, "Internal pcre2_pattern_info() error %d", rc);
		pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
		return NULL;
	}

	rc = pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &new_entry.name_count);
	if (rc < 0) {
		if (key != regex) {
			zend_string_release_ex(key, 0);
		}
		php_error_docref(NULL, E_WARNING, "Internal pcre_pattern_info() error %d", rc);
		pcre_handle_exec_error(PCRE2_ERROR_INTERNAL);
		return NULL;
	}

	/*
	 * Interned strings are not duplicated when stored in HashTable,
	 * but all the interned strings created during HTTP request are removed
	 * at end of request. However PCRE_G(pcre_cache) must be consistent
	 * on the next request as well. So we disable usage of interned strings
	 * as hash keys especually for this table.
	 * See bug #63180
	 */
	if (!(GC_FLAGS(key) & IS_STR_PERMANENT) && !PCRE_G(per_request_cache)) {
		zend_string *str = zend_string_init(ZSTR_VAL(key), ZSTR_LEN(key), 1);
		GC_MAKE_PERSISTENT_LOCAL(str);

		ret = zend_hash_add_new_mem(&PCRE_G(pcre_cache), str, &new_entry, sizeof(pcre_cache_entry));
		zend_string_release(str);
	} else {
		ret = zend_hash_add_new_mem(&PCRE_G(pcre_cache), key, &new_entry, sizeof(pcre_cache_entry));
	}

	if (key != regex) {
		zend_string_release_ex(key, 0);
	}

	return ret;
}
/* }}} */

/* {{{ pcre_get_compiled_regex_cache */
PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache(zend_string *regex)
{
	return pcre_get_compiled_regex_cache_ex(regex, 1);
}
/* }}} */

/* {{{ pcre_get_compiled_regex */
PHPAPI pcre2_code *pcre_get_compiled_regex(zend_string *regex, uint32_t *capture_count)
{
	pcre_cache_entry * pce = pcre_get_compiled_regex_cache(regex);

	if (capture_count) {
		*capture_count = pce ? pce->capture_count : 0;
	}

	return pce ? pce->re : NULL;
}
/* }}} */

/* XXX For the cases where it's only about match yes/no and no capture
		required, perhaps just a minimum sized data would suffice. */
PHPAPI pcre2_match_data *php_pcre_create_match_data(uint32_t capture_count, pcre2_code *re)
{/*{{{*/

	assert(NULL != re);

	if (EXPECTED(!mdata_used)) {
		int rc = 0;

		if (!capture_count) {
			/* As we deal with a non cached pattern, no other way to gather this info. */
			rc = pcre2_pattern_info(re, PCRE2_INFO_CAPTURECOUNT, &capture_count);
		}

		if (rc >= 0 && capture_count + 1 <= PHP_PCRE_PREALLOC_MDATA_SIZE) {
			mdata_used = 1;
			return mdata;
		}
	}

	return pcre2_match_data_create_from_pattern(re, gctx);
}/*}}}*/

PHPAPI void php_pcre_free_match_data(pcre2_match_data *match_data)
{/*{{{*/
	if (UNEXPECTED(match_data != mdata)) {
		pcre2_match_data_free(match_data);
	} else {
		mdata_used = 0;
	}
}/*}}}*/

static void init_unmatched_null_pair(void) {
	zval val1, val2;
	ZVAL_NULL(&val1);
	ZVAL_LONG(&val2, -1);
	ZVAL_ARR(&PCRE_G(unmatched_null_pair), zend_new_pair(&val1, &val2));
}

static void init_unmatched_empty_pair(void) {
	zval val1, val2;
	ZVAL_EMPTY_STRING(&val1);
	ZVAL_LONG(&val2, -1);
	ZVAL_ARR(&PCRE_G(unmatched_empty_pair), zend_new_pair(&val1, &val2));
}

static zend_always_inline void populate_match_value_str(
		zval *val, const char *subject, PCRE2_SIZE start_offset, PCRE2_SIZE end_offset) {
	ZVAL_STRINGL_FAST(val, subject + start_offset, end_offset - start_offset);
}

static zend_always_inline void populate_match_value(
		zval *val, const char *subject, PCRE2_SIZE start_offset, PCRE2_SIZE end_offset,
		bool unmatched_as_null) {
	if (PCRE2_UNSET == start_offset) {
		if (unmatched_as_null) {
			ZVAL_NULL(val);
		} else {
			ZVAL_EMPTY_STRING(val);
		}
	} else {
		populate_match_value_str(val, subject, start_offset, end_offset);
	}
}

static inline void add_named(
		HashTable *subpats, zend_string *name, zval *val, bool unmatched) {
	/* If the DUPNAMES option is used, multiple subpatterns might have the same name.
	 * In this case we want to preserve the one that actually has a value. */
	if (!unmatched) {
		zend_hash_update(subpats, name, val);
	} else {
		if (!zend_hash_add(subpats, name, val)) {
			return;
		}
	}
	Z_TRY_ADDREF_P(val);
}

/* {{{ add_offset_pair */
static inline void add_offset_pair(
		HashTable *result, const char *subject, PCRE2_SIZE start_offset, PCRE2_SIZE end_offset,
		zend_string *name, uint32_t unmatched_as_null)
{
	zval match_pair;

	/* Add (match, offset) to the return value */
	if (PCRE2_UNSET == start_offset) {
		if (unmatched_as_null) {
			if (Z_ISUNDEF(PCRE_G(unmatched_null_pair))) {
				init_unmatched_null_pair();
			}
			ZVAL_COPY(&match_pair, &PCRE_G(unmatched_null_pair));
		} else {
			if (Z_ISUNDEF(PCRE_G(unmatched_empty_pair))) {
				init_unmatched_empty_pair();
			}
			ZVAL_COPY(&match_pair, &PCRE_G(unmatched_empty_pair));
		}
	} else {
		zval val1, val2;
		populate_match_value_str(&val1, subject, start_offset, end_offset);
		ZVAL_LONG(&val2, start_offset);
		ZVAL_ARR(&match_pair, zend_new_pair(&val1, &val2));
	}

	if (name) {
		add_named(result, name, &match_pair, start_offset == PCRE2_UNSET);
	}
	zend_hash_next_index_insert_new(result, &match_pair);
}
/* }}} */

static void populate_subpat_array(
		zval *subpats, const char *subject, PCRE2_SIZE *offsets, zend_string **subpat_names,
		uint32_t num_subpats, int count, const PCRE2_SPTR mark, zend_long flags) {
	bool offset_capture = (flags & PREG_OFFSET_CAPTURE) != 0;
	bool unmatched_as_null = (flags & PREG_UNMATCHED_AS_NULL) != 0;
	zval val;
	int i;
	HashTable *subpats_ht = Z_ARRVAL_P(subpats);
	if (subpat_names) {
		if (offset_capture) {
			for (i = 0; i < count; i++) {
				add_offset_pair(
					subpats_ht, subject, offsets[2*i], offsets[2*i+1],
					subpat_names[i], unmatched_as_null);
			}
			if (unmatched_as_null) {
				for (i = count; i < num_subpats; i++) {
					add_offset_pair(subpats_ht, NULL, PCRE2_UNSET, PCRE2_UNSET, subpat_names[i], 1);
				}
			}
		} else {
			for (i = 0; i < count; i++) {
				populate_match_value(
					&val, subject, offsets[2*i], offsets[2*i+1], unmatched_as_null);
				if (subpat_names[i]) {
					add_named(subpats_ht, subpat_names[i], &val, offsets[2*i] == PCRE2_UNSET);
				}
				zend_hash_next_index_insert_new(subpats_ht, &val);
			}
			if (unmatched_as_null) {
				for (i = count; i < num_subpats; i++) {
					ZVAL_NULL(&val);
					if (subpat_names[i]) {
						zend_hash_add(subpats_ht, subpat_names[i], &val);
					}
					zend_hash_next_index_insert_new(subpats_ht, &val);
				}
			}
		}
	} else {
		if (offset_capture) {
			for (i = 0; i < count; i++) {
				add_offset_pair(
					subpats_ht, subject, offsets[2*i], offsets[2*i+1], NULL, unmatched_as_null);
			}
			if (unmatched_as_null) {
				for (i = count; i < num_subpats; i++) {
					add_offset_pair(subpats_ht, NULL, PCRE2_UNSET, PCRE2_UNSET, NULL, 1);
				}
			}
		} else {
			for (i = 0; i < count; i++) {
				populate_match_value(
					&val, subject, offsets[2*i], offsets[2*i+1], unmatched_as_null);
				zend_hash_next_index_insert_new(subpats_ht, &val);
			}
			if (unmatched_as_null) {
				for (i = count; i < num_subpats; i++) {
					add_next_index_null(subpats);
				}
			}
		}
	}
	/* Add MARK, if available */
	if (mark) {
		add_assoc_string_ex(subpats, "MARK", sizeof("MARK") - 1, (char *)mark);
	}
}

static void php_do_pcre_match(INTERNAL_FUNCTION_PARAMETERS, int global) /* {{{ */
{
	/* parameters */
	zend_string		 *regex;			/* Regular expression */
	zend_string		 *subject;			/* String to match against */
	pcre_cache_entry *pce;				/* Compiled regular expression */
	zval			 *subpats = NULL;	/* Array for subpatterns */
	zend_long		  flags = 0;		/* Match control flags */
	zend_long		  start_offset = 0;	/* Where the new search starts */

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR(regex)
		Z_PARAM_STR(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(subpats)
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(start_offset)
	ZEND_PARSE_PARAMETERS_END();

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		RETURN_FALSE;
	}

	pce->refcount++;
	php_pcre_match_impl(pce, subject, return_value, subpats,
		global, ZEND_NUM_ARGS() >= 4, flags, start_offset);
	pce->refcount--;
}
/* }}} */

static zend_always_inline bool is_known_valid_utf8(
		zend_string *subject_str, PCRE2_SIZE start_offset) {
	if (!(GC_FLAGS(subject_str) & IS_STR_VALID_UTF8)) {
		/* We don't know whether the string is valid UTF-8 or not. */
		return 0;
	}

	if (start_offset == ZSTR_LEN(subject_str)) {
		/* Degenerate case: Offset points to end of string. */
		return 1;
	}

	/* Check that the offset does not point to an UTF-8 continuation byte. */
	return (ZSTR_VAL(subject_str)[start_offset] & 0xc0) != 0x80;
}

/* {{{ php_pcre_match_impl() */
PHPAPI void php_pcre_match_impl(pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
	zval *subpats, int global, int use_flags, zend_long flags, zend_off_t start_offset)
{
	zval			 result_set;		/* Holds a set of subpatterns after
										   a global match */
	HashTable	   **match_sets = NULL;	/* An array of sets of matches for each
										   subpattern after a global match */
	uint32_t		 options;			/* Execution options */
	int				 count;				/* Count of matched subpatterns */
	PCRE2_SIZE		*offsets;			/* Array of subpattern offsets */
	uint32_t		 num_subpats;		/* Number of captured subpatterns */
	int				 matched;			/* Has anything matched */
	zend_string	   **subpat_names;		/* Array for named subpatterns */
	size_t			 i;
	uint32_t		 subpats_order;		/* Order of subpattern matches */
	uint32_t		 offset_capture;	/* Capture match offsets: yes/no */
	uint32_t		 unmatched_as_null;	/* Null non-matches: yes/no */
	PCRE2_SPTR       mark = NULL;		/* Target for MARK name */
	zval			 marks;				/* Array of marks for PREG_PATTERN_ORDER */
	pcre2_match_data *match_data;
	PCRE2_SIZE		 start_offset2, orig_start_offset;

	char *subject = ZSTR_VAL(subject_str);
	size_t subject_len = ZSTR_LEN(subject_str);

	ZVAL_UNDEF(&marks);

	/* Overwrite the passed-in value for subpatterns with an empty array. */
	if (subpats != NULL) {
		subpats = zend_try_array_init(subpats);
		if (!subpats) {
			RETURN_THROWS();
		}
	}

	subpats_order = global ? PREG_PATTERN_ORDER : 0;

	if (use_flags) {
		offset_capture = flags & PREG_OFFSET_CAPTURE;
		unmatched_as_null = flags & PREG_UNMATCHED_AS_NULL;

		/*
		 * subpats_order is pre-set to pattern mode so we change it only if
		 * necessary.
		 */
		if (flags & 0xff) {
			subpats_order = flags & 0xff;
		}
		if ((global && (subpats_order < PREG_PATTERN_ORDER || subpats_order > PREG_SET_ORDER)) ||
			(!global && subpats_order != 0)) {
			zend_argument_value_error(4, "must be a PREG_* constant");
			RETURN_THROWS();
		}
	} else {
		offset_capture = 0;
		unmatched_as_null = 0;
	}

	/* Negative offset counts from the end of the string. */
	if (start_offset < 0) {
		if ((PCRE2_SIZE)-start_offset <= subject_len) {
			start_offset2 = subject_len + start_offset;
		} else {
			start_offset2 = 0;
		}
	} else {
		start_offset2 = (PCRE2_SIZE)start_offset;
	}

	if (start_offset2 > subject_len) {
		pcre_handle_exec_error(PCRE2_ERROR_BADOFFSET);
		RETURN_FALSE;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pce->capture_count + 1;

	/*
	 * Build a mapping from subpattern numbers to their names. We will
	 * allocate the table only if there are any named subpatterns.
	 */
	subpat_names = NULL;
	if (subpats && pce->name_count > 0) {
		subpat_names = make_subpats_table(num_subpats, pce);
		if (!subpat_names) {
			RETURN_FALSE;
		}
	}

	matched = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	if (!mdata_used && num_subpats <= PHP_PCRE_PREALLOC_MDATA_SIZE) {
		match_data = mdata;
	} else {
		match_data = pcre2_match_data_create_from_pattern(pce->re, PCRE_G(gctx_zmm));
		if (!match_data) {
			PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
			if (subpat_names) {
				free_subpats_table(subpat_names, num_subpats);
			}
			RETURN_FALSE;
		}
	}

	/* Allocate match sets array and initialize the values. */
	if (global && subpats && subpats_order == PREG_PATTERN_ORDER) {
		match_sets = safe_emalloc(num_subpats, sizeof(HashTable *), 0);
		for (i=0; i<num_subpats; i++) {
			match_sets[i] = zend_new_array(0);
		}
	}

	offsets = pcre2_get_ovector_pointer(match_data);

	orig_start_offset = start_offset2;
	options =
		(pce->compile_options & PCRE2_UTF) && !is_known_valid_utf8(subject_str, orig_start_offset)
			? 0 : PCRE2_NO_UTF_CHECK;

	/* Execute the regular expression. */
#ifdef HAVE_PCRE_JIT_SUPPORT
	if ((pce->preg_options & PREG_JIT) && options) {
		count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset2,
				PCRE2_NO_UTF_CHECK, match_data, mctx);
	} else
#endif
	count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset2,
			options, match_data, mctx);

	while (1) {
		/* If something has matched */
		if (count >= 0) {
			/* Check for too many substrings condition. */
			if (UNEXPECTED(count == 0)) {
				php_error_docref(NULL, E_NOTICE, "Matched, but too many substrings");
				count = num_subpats;
			}

matched:
			matched++;

			/* If subpatterns array has been passed, fill it in with values. */
			if (subpats != NULL) {
				/* Try to get the list of substrings and display a warning if failed. */
				if (UNEXPECTED(offsets[1] < offsets[0])) {
					if (subpat_names) {
						free_subpats_table(subpat_names, num_subpats);
					}
					if (match_sets) efree(match_sets);
					php_error_docref(NULL, E_WARNING, "Get subpatterns list failed");
					RETURN_FALSE;
				}

				if (global) {	/* global pattern matching */
					if (subpats_order == PREG_PATTERN_ORDER) {
						/* For each subpattern, insert it into the appropriate array. */
						if (offset_capture) {
							for (i = 0; i < count; i++) {
								add_offset_pair(
									match_sets[i], subject, offsets[2*i], offsets[2*i+1],
									NULL, unmatched_as_null);
							}
						} else {
							for (i = 0; i < count; i++) {
								zval val;
								populate_match_value(
									&val, subject, offsets[2*i], offsets[2*i+1], unmatched_as_null);
								zend_hash_next_index_insert_new(match_sets[i], &val);
							}
						}
						mark = pcre2_get_mark(match_data);
						/* Add MARK, if available */
						if (mark) {
							if (Z_TYPE(marks) == IS_UNDEF) {
								array_init(&marks);
							}
							add_index_string(&marks, matched - 1, (char *) mark);
						}
						/*
						 * If the number of captured subpatterns on this run is
						 * less than the total possible number, pad the result
						 * arrays with NULLs or empty strings.
						 */
						if (count < num_subpats) {
							for (; i < num_subpats; i++) {
								if (offset_capture) {
									add_offset_pair(
										match_sets[i], NULL, PCRE2_UNSET, PCRE2_UNSET,
										NULL, unmatched_as_null);
								} else if (unmatched_as_null) {
									zval tmp;
									ZVAL_NULL(&tmp);
									zend_hash_next_index_insert_new(match_sets[i], &tmp);
								} else {
									zval tmp;
									ZVAL_EMPTY_STRING(&tmp);
									zend_hash_next_index_insert_new(match_sets[i], &tmp);
								}
							}
						}
					} else {
						/* Allocate and populate the result set array */
						mark = pcre2_get_mark(match_data);
						array_init_size(&result_set, count + (mark ? 1 : 0));
						populate_subpat_array(
							&result_set, subject, offsets, subpat_names,
							num_subpats, count, mark, flags);
						/* And add it to the output array */
						zend_hash_next_index_insert_new(Z_ARRVAL_P(subpats), &result_set);
					}
				} else {			/* single pattern matching */
					/* For each subpattern, insert it into the subpatterns array. */
					mark = pcre2_get_mark(match_data);
					populate_subpat_array(
						subpats, subject, offsets, subpat_names, num_subpats, count, mark, flags);
					break;
				}
			}

			/* Advance to the next piece. */
			start_offset2 = offsets[1];

			/* If we have matched an empty string, mimic what Perl's /g options does.
			   This turns out to be rather cunning. First we set PCRE2_NOTEMPTY_ATSTART and try
			   the match again at the same point. If this fails (picked up above) we
			   advance to the next character. */
			if (start_offset2 == offsets[0]) {
				count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset2,
					PCRE2_NO_UTF_CHECK | PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED, match_data, mctx);
				if (count >= 0) {
					if (global) {
						goto matched;
					} else {
						break;
					}
				} else if (count == PCRE2_ERROR_NOMATCH) {
					/* If we previously set PCRE2_NOTEMPTY_ATSTART after a null match,
					   this is not necessarily the end. We need to advance
					   the start offset, and continue. Fudge the offset values
					   to achieve this, unless we're already at the end of the string. */
					if (start_offset2 < subject_len) {
						size_t unit_len = calculate_unit_length(pce, subject + start_offset2);

						start_offset2 += unit_len;
					} else {
						break;
					}
				} else {
					goto error;
				}
			}
		} else if (count == PCRE2_ERROR_NOMATCH) {
			break;
		} else {
error:
			pcre_handle_exec_error(count);
			break;
		}

		if (!global) {
			break;
		}

		/* Execute the regular expression. */
#ifdef HAVE_PCRE_JIT_SUPPORT
		if ((pce->preg_options & PREG_JIT)) {
			if (PCRE2_UNSET == start_offset2 || start_offset2 > subject_len) {
				pcre_handle_exec_error(PCRE2_ERROR_BADOFFSET);
				break;
			}
			count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset2,
					PCRE2_NO_UTF_CHECK, match_data, mctx);
		} else
#endif
		count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset2,
				PCRE2_NO_UTF_CHECK, match_data, mctx);
	}
	if (match_data != mdata) {
		pcre2_match_data_free(match_data);
	}

	/* Add the match sets to the output array and clean up */
	if (match_sets) {
		if (subpat_names) {
			for (i = 0; i < num_subpats; i++) {
				zval wrapper;
				ZVAL_ARR(&wrapper, match_sets[i]);
				if (subpat_names[i]) {
					zend_hash_update(Z_ARRVAL_P(subpats), subpat_names[i], &wrapper);
					GC_ADDREF(match_sets[i]);
				}
				zend_hash_next_index_insert_new(Z_ARRVAL_P(subpats), &wrapper);
			}
		} else {
			for (i = 0; i < num_subpats; i++) {
				zval wrapper;
				ZVAL_ARR(&wrapper, match_sets[i]);
				zend_hash_next_index_insert_new(Z_ARRVAL_P(subpats), &wrapper);
			}
		}
		efree(match_sets);

		if (Z_TYPE(marks) != IS_UNDEF) {
			add_assoc_zval(subpats, "MARK", &marks);
		}
	}

	if (subpat_names) {
		free_subpats_table(subpat_names, num_subpats);
	}

	if (PCRE_G(error_code) == PHP_PCRE_NO_ERROR) {
		/* If there was no error and we're in /u mode, remember that the string is valid UTF-8. */
		if ((pce->compile_options & PCRE2_UTF)
				&& !ZSTR_IS_INTERNED(subject_str) && orig_start_offset == 0) {
			GC_ADD_FLAGS(subject_str, IS_STR_VALID_UTF8);
		}

		RETVAL_LONG(matched);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ Perform a Perl-style regular expression match */
PHP_FUNCTION(preg_match)
{
	php_do_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Perform a Perl-style global regular expression match */
PHP_FUNCTION(preg_match_all)
{
	php_do_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ preg_get_backref */
static int preg_get_backref(char **str, int *backref)
{
	char in_brace = 0;
	char *walk = *str;

	if (walk[1] == 0)
		return 0;

	if (*walk == '$' && walk[1] == '{') {
		in_brace = 1;
		walk++;
	}
	walk++;

	if (*walk >= '0' && *walk <= '9') {
		*backref = *walk - '0';
		walk++;
	} else
		return 0;

	if (*walk && *walk >= '0' && *walk <= '9') {
		*backref = *backref * 10 + *walk - '0';
		walk++;
	}

	if (in_brace) {
		if (*walk != '}')
			return 0;
		else
			walk++;
	}

	*str = walk;
	return 1;
}
/* }}} */

/* {{{ preg_do_repl_func */
static zend_string *preg_do_repl_func(zend_fcall_info *fci, zend_fcall_info_cache *fcc, const char *subject, PCRE2_SIZE *offsets, zend_string **subpat_names, uint32_t num_subpats, int count, const PCRE2_SPTR mark, zend_long flags)
{
	zend_string *result_str;
	zval		 retval;			/* Function return value */
	zval	     arg;				/* Argument to pass to function */

	array_init_size(&arg, count + (mark ? 1 : 0));
	populate_subpat_array(&arg, subject, offsets, subpat_names, num_subpats, count, mark, flags);

	fci->retval = &retval;
	fci->param_count = 1;
	fci->params = &arg;

	if (zend_call_function(fci, fcc) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (EXPECTED(Z_TYPE(retval) == IS_STRING)) {
			result_str = Z_STR(retval);
		} else {
			result_str = zval_get_string_func(&retval);
			zval_ptr_dtor(&retval);
		}
	} else {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to call custom replacement function");
		}

		result_str = zend_string_init(&subject[offsets[0]], offsets[1] - offsets[0], 0);
	}

	zval_ptr_dtor(&arg);

	return result_str;
}
/* }}} */

/* {{{ php_pcre_replace */
PHPAPI zend_string *php_pcre_replace(zend_string *regex,
							  zend_string *subject_str,
							  const char *subject, size_t subject_len,
							  zend_string *replace_str,
							  size_t limit, size_t *replace_count)
{
	pcre_cache_entry	*pce;			    /* Compiled regular expression */
	zend_string	 		*result;			/* Function result */

	/* Abort on pending exception, e.g. thrown from __toString(). */
	if (UNEXPECTED(EG(exception))) {
		return NULL;
	}

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		return NULL;
	}
	pce->refcount++;
	result = php_pcre_replace_impl(pce, subject_str, subject, subject_len, replace_str,
		limit, replace_count);
	pce->refcount--;

	return result;
}
/* }}} */

/* {{{ php_pcre_replace_impl() */
PHPAPI zend_string *php_pcre_replace_impl(pcre_cache_entry *pce, zend_string *subject_str, const char *subject, size_t subject_len, zend_string *replace_str, size_t limit, size_t *replace_count)
{
	uint32_t		 options;			/* Execution options */
	int				 count;				/* Count of matched subpatterns */
	PCRE2_SIZE		*offsets;			/* Array of subpattern offsets */
	uint32_t		 num_subpats;		/* Number of captured subpatterns */
	size_t			 new_len;			/* Length of needed storage */
	size_t			 alloc_len;			/* Actual allocated length */
	size_t			 match_len;			/* Length of the current match */
	int				 backref;			/* Backreference number */
	PCRE2_SIZE		 start_offset;		/* Where the new search starts */
	size_t			 last_end_offset;	/* Where the last search ended */
	char			*walkbuf,			/* Location of current replacement in the result */
					*walk,				/* Used to walk the replacement string */
					 walk_last;			/* Last walked character */
	const char		*match,				/* The current match */
					*piece,				/* The current piece of subject */
					*replace_end;		/* End of replacement string */
	size_t			result_len; 		/* Length of result */
	zend_string		*result;			/* Result of replacement */
	pcre2_match_data *match_data;

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pce->capture_count + 1;
	alloc_len = 0;
	result = NULL;

	/* Initialize */
	match = NULL;
	start_offset = 0;
	last_end_offset = 0;
	result_len = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	if (!mdata_used && num_subpats <= PHP_PCRE_PREALLOC_MDATA_SIZE) {
		match_data = mdata;
	} else {
		match_data = pcre2_match_data_create_from_pattern(pce->re, PCRE_G(gctx_zmm));
		if (!match_data) {
			PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
			return NULL;
		}
	}

	options = (pce->compile_options & PCRE2_UTF) ? 0 : PCRE2_NO_UTF_CHECK;

	offsets = pcre2_get_ovector_pointer(match_data);

	/* Execute the regular expression. */
#ifdef HAVE_PCRE_JIT_SUPPORT
	if ((pce->preg_options & PREG_JIT) && options) {
		count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
				PCRE2_NO_UTF_CHECK, match_data, mctx);
	} else
#endif
	count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
			options, match_data, mctx);

	while (1) {
		piece = subject + last_end_offset;

		if (count >= 0 && limit > 0) {
			bool simple_string;

			/* Check for too many substrings condition. */
			if (UNEXPECTED(count == 0)) {
				php_error_docref(NULL,E_NOTICE, "Matched, but too many substrings");
				count = num_subpats;
			}

matched:
			if (UNEXPECTED(offsets[1] < offsets[0])) {
				PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
				if (result) {
					zend_string_release_ex(result, 0);
					result = NULL;
				}
				break;
			}

			if (replace_count) {
				++*replace_count;
			}

			/* Set the match location in subject */
			match = subject + offsets[0];

			new_len = result_len + offsets[0] - last_end_offset; /* part before the match */

			walk = ZSTR_VAL(replace_str);
			replace_end = walk + ZSTR_LEN(replace_str);
			walk_last = 0;
			simple_string = 1;
			while (walk < replace_end) {
				if ('\\' == *walk || '$' == *walk) {
					simple_string = 0;
					if (walk_last == '\\') {
						walk++;
						walk_last = 0;
						continue;
					}
					if (preg_get_backref(&walk, &backref)) {
						if (backref < count)
							new_len += offsets[(backref<<1)+1] - offsets[backref<<1];
						continue;
					}
				}
				new_len++;
				walk++;
				walk_last = walk[-1];
			}

			if (new_len >= alloc_len) {
				alloc_len = zend_safe_address_guarded(2, new_len, ZSTR_MAX_OVERHEAD) - ZSTR_MAX_OVERHEAD;
				if (result == NULL) {
					result = zend_string_alloc(alloc_len, 0);
				} else {
					result = zend_string_extend(result, alloc_len, 0);
				}
			}

			if (match-piece > 0) {
				/* copy the part of the string before the match */
				memcpy(&ZSTR_VAL(result)[result_len], piece, match-piece);
				result_len += (match-piece);
			}

			if (simple_string) {
				/* copy replacement */
				memcpy(&ZSTR_VAL(result)[result_len], ZSTR_VAL(replace_str), ZSTR_LEN(replace_str)+1);
				result_len += ZSTR_LEN(replace_str);
			} else {
				/* copy replacement and backrefs */
				walkbuf = ZSTR_VAL(result) + result_len;

				walk = ZSTR_VAL(replace_str);
				walk_last = 0;
				while (walk < replace_end) {
					if ('\\' == *walk || '$' == *walk) {
						if (walk_last == '\\') {
							*(walkbuf-1) = *walk++;
							walk_last = 0;
							continue;
						}
						if (preg_get_backref(&walk, &backref)) {
							if (backref < count) {
								match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
								memcpy(walkbuf, subject + offsets[backref<<1], match_len);
								walkbuf += match_len;
							}
							continue;
						}
					}
					*walkbuf++ = *walk++;
					walk_last = walk[-1];
				}
				*walkbuf = '\0';
				/* increment the result length by how much we've added to the string */
				result_len += (walkbuf - (ZSTR_VAL(result) + result_len));
			}

			limit--;

			/* Advance to the next piece. */
			start_offset = last_end_offset = offsets[1];

			/* If we have matched an empty string, mimic what Perl's /g options does.
			   This turns out to be rather cunning. First we set PCRE2_NOTEMPTY_ATSTART and try
			   the match again at the same point. If this fails (picked up above) we
			   advance to the next character. */
			if (start_offset == offsets[0]) {
				count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
					PCRE2_NO_UTF_CHECK | PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED, match_data, mctx);

				piece = subject + start_offset;
				if (count >= 0 && limit > 0) {
					goto matched;
				} else if (count == PCRE2_ERROR_NOMATCH || limit == 0) {
					/* If we previously set PCRE2_NOTEMPTY_ATSTART after a null match,
					   this is not necessarily the end. We need to advance
					   the start offset, and continue. Fudge the offset values
					   to achieve this, unless we're already at the end of the string. */
					if (start_offset < subject_len) {
						size_t unit_len = calculate_unit_length(pce, piece);
						start_offset += unit_len;
					} else {
						goto not_matched;
					}
				} else {
					goto error;
				}
			}

		} else if (count == PCRE2_ERROR_NOMATCH || limit == 0) {
not_matched:
			if (!result && subject_str) {
				result = zend_string_copy(subject_str);
				break;
			}
			/* now we know exactly how long it is */
			alloc_len = result_len + subject_len - last_end_offset;
			if (NULL != result) {
				result = zend_string_realloc(result, alloc_len, 0);
			} else {
				result = zend_string_alloc(alloc_len, 0);
			}
			/* stick that last bit of string on our output */
			memcpy(ZSTR_VAL(result) + result_len, piece, subject_len - last_end_offset);
			result_len += subject_len - last_end_offset;
			ZSTR_VAL(result)[result_len] = '\0';
			ZSTR_LEN(result) = result_len;
			break;
		} else {
error:
			pcre_handle_exec_error(count);
			if (result) {
				zend_string_release_ex(result, 0);
				result = NULL;
			}
			break;
		}

#ifdef HAVE_PCRE_JIT_SUPPORT
		if (pce->preg_options & PREG_JIT) {
			count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
					PCRE2_NO_UTF_CHECK, match_data, mctx);
		} else
#endif
		count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
					PCRE2_NO_UTF_CHECK, match_data, mctx);
	}
	if (match_data != mdata) {
		pcre2_match_data_free(match_data);
	}

	return result;
}
/* }}} */

/* {{{ php_pcre_replace_func_impl() */
static zend_string *php_pcre_replace_func_impl(pcre_cache_entry *pce, zend_string *subject_str, const char *subject, size_t subject_len, zend_fcall_info *fci, zend_fcall_info_cache *fcc, size_t limit, size_t *replace_count, zend_long flags)
{
	uint32_t		 options;			/* Execution options */
	int				 count;				/* Count of matched subpatterns */
	PCRE2_SIZE		*offsets;			/* Array of subpattern offsets */
	zend_string		**subpat_names;		/* Array for named subpatterns */
	uint32_t		 num_subpats;		/* Number of captured subpatterns */
	size_t			 new_len;			/* Length of needed storage */
	size_t			 alloc_len;			/* Actual allocated length */
	PCRE2_SIZE		 start_offset;		/* Where the new search starts */
	size_t			 last_end_offset;	/* Where the last search ended */
	const char		*match,				/* The current match */
					*piece;				/* The current piece of subject */
	size_t			result_len; 		/* Length of result */
	zend_string		*result;			/* Result of replacement */
	zend_string     *eval_result;		/* Result of custom function */
	pcre2_match_data *match_data;
	bool old_mdata_used;

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pce->capture_count + 1;

	/*
	 * Build a mapping from subpattern numbers to their names. We will
	 * allocate the table only if there are any named subpatterns.
	 */
	subpat_names = NULL;
	if (UNEXPECTED(pce->name_count > 0)) {
		subpat_names = make_subpats_table(num_subpats, pce);
		if (!subpat_names) {
			return NULL;
		}
	}

	alloc_len = 0;
	result = NULL;

	/* Initialize */
	match = NULL;
	start_offset = 0;
	last_end_offset = 0;
	result_len = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	old_mdata_used = mdata_used;
	if (!old_mdata_used && num_subpats <= PHP_PCRE_PREALLOC_MDATA_SIZE) {
		mdata_used = 1;
		match_data = mdata;
	} else {
		match_data = pcre2_match_data_create_from_pattern(pce->re, PCRE_G(gctx_zmm));
		if (!match_data) {
			PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
			if (subpat_names) {
				free_subpats_table(subpat_names, num_subpats);
			}
			mdata_used = old_mdata_used;
			return NULL;
		}
	}

	options = (pce->compile_options & PCRE2_UTF) ? 0 : PCRE2_NO_UTF_CHECK;

	offsets = pcre2_get_ovector_pointer(match_data);

	/* Execute the regular expression. */
#ifdef HAVE_PCRE_JIT_SUPPORT
	if ((pce->preg_options & PREG_JIT) && options) {
		count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
				PCRE2_NO_UTF_CHECK, match_data, mctx);
	} else
#endif
	count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
			options, match_data, mctx);

	while (1) {
		piece = subject + last_end_offset;

		if (count >= 0 && limit) {
			/* Check for too many substrings condition. */
			if (UNEXPECTED(count == 0)) {
				php_error_docref(NULL,E_NOTICE, "Matched, but too many substrings");
				count = num_subpats;
			}

matched:
			if (UNEXPECTED(offsets[1] < offsets[0])) {
				PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
				if (result) {
					zend_string_release_ex(result, 0);
					result = NULL;
				}
				break;
			}

			if (replace_count) {
				++*replace_count;
			}

			/* Set the match location in subject */
			match = subject + offsets[0];

			new_len = result_len + offsets[0] - last_end_offset; /* part before the match */

			/* Use custom function to get replacement string and its length. */
			eval_result = preg_do_repl_func(
				fci, fcc, subject, offsets, subpat_names, num_subpats, count,
				pcre2_get_mark(match_data), flags);

			ZEND_ASSERT(eval_result);
			new_len = zend_safe_address_guarded(1, ZSTR_LEN(eval_result) + ZSTR_MAX_OVERHEAD, new_len) -ZSTR_MAX_OVERHEAD;
			if (new_len >= alloc_len) {
				alloc_len = zend_safe_address_guarded(2, new_len, ZSTR_MAX_OVERHEAD) - ZSTR_MAX_OVERHEAD;
				if (result == NULL) {
					result = zend_string_alloc(alloc_len, 0);
				} else {
					result = zend_string_extend(result, alloc_len, 0);
				}
			}

			if (match-piece > 0) {
				/* copy the part of the string before the match */
				memcpy(ZSTR_VAL(result) + result_len, piece, match-piece);
				result_len += (match-piece);
			}

			/* If using custom function, copy result to the buffer and clean up. */
			memcpy(ZSTR_VAL(result) + result_len, ZSTR_VAL(eval_result), ZSTR_LEN(eval_result));
			result_len += ZSTR_LEN(eval_result);
			zend_string_release_ex(eval_result, 0);

			limit--;

			/* Advance to the next piece. */
			start_offset = last_end_offset = offsets[1];

			/* If we have matched an empty string, mimic what Perl's /g options does.
			   This turns out to be rather cunning. First we set PCRE2_NOTEMPTY_ATSTART and try
			   the match again at the same point. If this fails (picked up above) we
			   advance to the next character. */
			if (start_offset == offsets[0]) {
				count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
					PCRE2_NO_UTF_CHECK | PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED, match_data, mctx);

				piece = subject + start_offset;
				if (count >= 0 && limit) {
					goto matched;
				} else if (count == PCRE2_ERROR_NOMATCH || limit == 0) {
					/* If we previously set PCRE2_NOTEMPTY_ATSTART after a null match,
					   this is not necessarily the end. We need to advance
					   the start offset, and continue. Fudge the offset values
					   to achieve this, unless we're already at the end of the string. */
					if (start_offset < subject_len) {
						size_t unit_len = calculate_unit_length(pce, piece);
						start_offset += unit_len;
					} else {
						goto not_matched;
					}
				} else {
					goto error;
				}
			}

		} else if (count == PCRE2_ERROR_NOMATCH || limit == 0) {
not_matched:
			if (!result && subject_str) {
				result = zend_string_copy(subject_str);
				break;
			}
			/* now we know exactly how long it is */
			alloc_len = result_len + subject_len - last_end_offset;
			if (NULL != result) {
				result = zend_string_realloc(result, alloc_len, 0);
			} else {
				result = zend_string_alloc(alloc_len, 0);
			}
			/* stick that last bit of string on our output */
			memcpy(ZSTR_VAL(result) + result_len, piece, subject_len - last_end_offset);
			result_len += subject_len - last_end_offset;
			ZSTR_VAL(result)[result_len] = '\0';
			ZSTR_LEN(result) = result_len;
			break;
		} else {
error:
			pcre_handle_exec_error(count);
			if (result) {
				zend_string_release_ex(result, 0);
				result = NULL;
			}
			break;
		}
#ifdef HAVE_PCRE_JIT_SUPPORT
		if ((pce->preg_options & PREG_JIT)) {
			count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
					PCRE2_NO_UTF_CHECK, match_data, mctx);
		} else
#endif
		count = pcre2_match(pce->re, (PCRE2_SPTR)subject, subject_len, start_offset,
				PCRE2_NO_UTF_CHECK, match_data, mctx);
	}
	if (match_data != mdata) {
		pcre2_match_data_free(match_data);
	}
	mdata_used = old_mdata_used;

	if (UNEXPECTED(subpat_names)) {
		free_subpats_table(subpat_names, num_subpats);
	}

	return result;
}
/* }}} */

/* {{{ php_pcre_replace_func */
static zend_always_inline zend_string *php_pcre_replace_func(zend_string *regex,
							  zend_string *subject_str,
							  zend_fcall_info *fci, zend_fcall_info_cache *fcc,
							  size_t limit, size_t *replace_count, zend_long flags)
{
	pcre_cache_entry	*pce;			    /* Compiled regular expression */
	zend_string	 		*result;			/* Function result */

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		return NULL;
	}
	pce->refcount++;
	result = php_pcre_replace_func_impl(
		pce, subject_str, ZSTR_VAL(subject_str), ZSTR_LEN(subject_str), fci, fcc,
		limit, replace_count, flags);
	pce->refcount--;

	return result;
}
/* }}} */

/* {{{ php_pcre_replace_array */
static zend_string *php_pcre_replace_array(HashTable *regex,
	zend_string *replace_str, HashTable *replace_ht,
	zend_string *subject_str, size_t limit, size_t *replace_count)
{
	zval		*regex_entry;
	zend_string *result;

	zend_string_addref(subject_str);

	if (replace_ht) {
		uint32_t replace_idx = 0;

		/* For each entry in the regex array, get the entry */
		ZEND_HASH_FOREACH_VAL(regex, regex_entry) {
			/* Make sure we're dealing with strings. */
			zend_string *tmp_regex_str;
			zend_string *regex_str = zval_get_tmp_string(regex_entry, &tmp_regex_str);
			zend_string *replace_entry_str, *tmp_replace_entry_str;
			zval *zv;

			/* Get current entry */
			while (1) {
				if (replace_idx == replace_ht->nNumUsed) {
					replace_entry_str = ZSTR_EMPTY_ALLOC();
					tmp_replace_entry_str = NULL;
					break;
				}
				zv = ZEND_HASH_ELEMENT(replace_ht, replace_idx);
				replace_idx++;
				if (Z_TYPE_P(zv) != IS_UNDEF) {
					replace_entry_str = zval_get_tmp_string(zv, &tmp_replace_entry_str);
					break;
				}
			}

			/* Do the actual replacement and put the result back into subject_str
			   for further replacements. */
			result = php_pcre_replace(regex_str, subject_str, ZSTR_VAL(subject_str),
				ZSTR_LEN(subject_str), replace_entry_str, limit, replace_count);
			zend_tmp_string_release(tmp_replace_entry_str);
			zend_tmp_string_release(tmp_regex_str);
			zend_string_release_ex(subject_str, 0);
			subject_str = result;
			if (UNEXPECTED(result == NULL)) {
				break;
			}
		} ZEND_HASH_FOREACH_END();

	} else {
		ZEND_ASSERT(replace_str != NULL);

		/* For each entry in the regex array, get the entry */
		ZEND_HASH_FOREACH_VAL(regex, regex_entry) {
			/* Make sure we're dealing with strings. */
			zend_string *tmp_regex_str;
			zend_string *regex_str = zval_get_tmp_string(regex_entry, &tmp_regex_str);

			/* Do the actual replacement and put the result back into subject_str
			   for further replacements. */
			result = php_pcre_replace(regex_str, subject_str, ZSTR_VAL(subject_str),
				ZSTR_LEN(subject_str), replace_str, limit, replace_count);
			zend_tmp_string_release(tmp_regex_str);
			zend_string_release_ex(subject_str, 0);
			subject_str = result;

			if (UNEXPECTED(result == NULL)) {
				break;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return subject_str;
}
/* }}} */

/* {{{ php_replace_in_subject */
static zend_always_inline zend_string *php_replace_in_subject(
	zend_string *regex_str, HashTable *regex_ht,
	zend_string *replace_str, HashTable *replace_ht,
	zend_string *subject, size_t limit, size_t *replace_count)
{
	zend_string *result;

	if (regex_str) {
		ZEND_ASSERT(replace_str != NULL);
		result = php_pcre_replace(regex_str, subject, ZSTR_VAL(subject), ZSTR_LEN(subject),
			replace_str, limit, replace_count);
	} else {
		ZEND_ASSERT(regex_ht != NULL);
		result = php_pcre_replace_array(regex_ht, replace_str, replace_ht, subject,
			limit, replace_count);
	}
	return result;
}
/* }}} */

/* {{{ php_replace_in_subject_func */
static zend_string *php_replace_in_subject_func(zend_string *regex_str, HashTable *regex_ht,
	zend_fcall_info *fci, zend_fcall_info_cache *fcc,
	zend_string *subject, size_t limit, size_t *replace_count, zend_long flags)
{
	zend_string *result;

	if (regex_str) {
		result = php_pcre_replace_func(
			regex_str, subject, fci, fcc, limit, replace_count, flags);
		return result;
	} else {
		/* If regex is an array */
		zval		*regex_entry;

		ZEND_ASSERT(regex_ht != NULL);

		zend_string_addref(subject);

		/* For each entry in the regex array, get the entry */
		ZEND_HASH_FOREACH_VAL(regex_ht, regex_entry) {
			/* Make sure we're dealing with strings. */
			zend_string *tmp_regex_entry_str;
			zend_string *regex_entry_str = zval_get_tmp_string(regex_entry, &tmp_regex_entry_str);

			/* Do the actual replacement and put the result back into subject
			   for further replacements. */
			result = php_pcre_replace_func(
				regex_entry_str, subject, fci, fcc, limit, replace_count, flags);
			zend_tmp_string_release(tmp_regex_entry_str);
			zend_string_release(subject);
			subject = result;
			if (UNEXPECTED(result == NULL)) {
				break;
			}
		} ZEND_HASH_FOREACH_END();

		return subject;
	}
}
/* }}} */

/* {{{ preg_replace_func_impl */
static size_t preg_replace_func_impl(zval *return_value,
	zend_string *regex_str, HashTable *regex_ht,
	zend_fcall_info *fci, zend_fcall_info_cache *fcc,
	zend_string *subject_str, HashTable *subject_ht, zend_long limit_val, zend_long flags)
{
	zend_string	*result;
	size_t replace_count = 0;

	if (subject_str) {
		result = php_replace_in_subject_func(
			regex_str, regex_ht, fci, fcc, subject_str, limit_val, &replace_count, flags);
		if (result != NULL) {
			RETVAL_STR(result);
		} else {
			RETVAL_NULL();
		}
	} else {
		/* if subject is an array */
		zval		*subject_entry, zv;
		zend_string	*string_key;
		zend_ulong	 num_key;

		ZEND_ASSERT(subject_ht != NULL);

		array_init_size(return_value, zend_hash_num_elements(subject_ht));
		HashTable *return_value_ht = Z_ARRVAL_P(return_value);

		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		ZEND_HASH_FOREACH_KEY_VAL(subject_ht, num_key, string_key, subject_entry) {
			zend_string *tmp_subject_entry_str;
			zend_string *subject_entry_str = zval_get_tmp_string(subject_entry, &tmp_subject_entry_str);

			result = php_replace_in_subject_func(
				regex_str, regex_ht, fci, fcc, subject_entry_str, limit_val, &replace_count, flags);
			if (result != NULL) {
				/* Add to return array */
				ZVAL_STR(&zv, result);
				if (string_key) {
					zend_hash_add_new(return_value_ht, string_key, &zv);
				} else {
					zend_hash_index_add_new(return_value_ht, num_key, &zv);
				}
			}
			zend_tmp_string_release(tmp_subject_entry_str);
		} ZEND_HASH_FOREACH_END();
	}

	return replace_count;
}
/* }}} */

/* {{{ preg_replace_common */
static void preg_replace_common(INTERNAL_FUNCTION_PARAMETERS, bool is_filter)
{
	zval *zcount = NULL;
	zend_string *regex_str;
	HashTable *regex_ht;
	zend_string *replace_str;
	HashTable *replace_ht;
	zend_string *subject_str;
	HashTable *subject_ht;
	zend_long limit = -1;
	size_t replace_count = 0;
	zend_string	*result;
	size_t old_replace_count;

	/* Get function parameters and do error-checking. */
	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_ARRAY_HT_OR_STR(regex_ht, regex_str)
		Z_PARAM_ARRAY_HT_OR_STR(replace_ht, replace_str)
		Z_PARAM_ARRAY_HT_OR_STR(subject_ht, subject_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
		Z_PARAM_ZVAL(zcount)
	ZEND_PARSE_PARAMETERS_END();

	/* If replace is an array then the regex argument needs to also be an array */
	if (replace_ht && !regex_ht) {
		zend_argument_type_error(1, "must be of type array when argument #2 ($replacement) is an array, string given");
		RETURN_THROWS();
	}

	if (subject_str) {
		old_replace_count = replace_count;
		result = php_replace_in_subject(regex_str, regex_ht, replace_str, replace_ht,
			subject_str, limit, &replace_count);
		if (result != NULL) {
			if (!is_filter || replace_count > old_replace_count) {
				RETVAL_STR(result);
			} else {
				zend_string_release_ex(result, 0);
				RETVAL_NULL();
			}
		} else {
			RETVAL_NULL();
		}
	} else {
		/* if subject is an array */
		zval		*subject_entry, zv;
		zend_string	*string_key;
		zend_ulong	 num_key;

		ZEND_ASSERT(subject_ht != NULL);

		array_init_size(return_value, zend_hash_num_elements(subject_ht));
		HashTable *return_value_ht = Z_ARRVAL_P(return_value);

		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		ZEND_HASH_FOREACH_KEY_VAL(subject_ht, num_key, string_key, subject_entry) {
			old_replace_count = replace_count;
			zend_string *tmp_subject_entry_str;
			zend_string *subject_entry_str = zval_get_tmp_string(subject_entry, &tmp_subject_entry_str);
			result = php_replace_in_subject(regex_str, regex_ht, replace_str, replace_ht,
				subject_entry_str, limit, &replace_count);

			if (result != NULL) {
				if (!is_filter || replace_count > old_replace_count) {
					/* Add to return array */
					ZVAL_STR(&zv, result);
					if (string_key) {
						zend_hash_add_new(return_value_ht, string_key, &zv);
					} else {
						zend_hash_index_add_new(return_value_ht, num_key, &zv);
					}
				} else {
					zend_string_release_ex(result, 0);
				}
			}
			zend_tmp_string_release(tmp_subject_entry_str);
		} ZEND_HASH_FOREACH_END();
	}

	if (zcount) {
		ZEND_TRY_ASSIGN_REF_LONG(zcount, replace_count);
	}
}
/* }}} */

/* {{{ Perform Perl-style regular expression replacement. */
PHP_FUNCTION(preg_replace)
{
	preg_replace_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} */

/* {{{ Perform Perl-style regular expression replacement using replacement callback. */
PHP_FUNCTION(preg_replace_callback)
{
	zval *zcount = NULL;
	zend_string *regex_str;
	HashTable *regex_ht;
	zend_string *subject_str;
	HashTable *subject_ht;
	zend_long limit = -1, flags = 0;
	size_t replace_count;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	/* Get function parameters and do error-checking. */
	ZEND_PARSE_PARAMETERS_START(3, 6)
		Z_PARAM_ARRAY_HT_OR_STR(regex_ht, regex_str)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_ARRAY_HT_OR_STR(subject_ht, subject_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
		Z_PARAM_ZVAL(zcount)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	replace_count = preg_replace_func_impl(return_value, regex_str, regex_ht,
		&fci, &fcc,
		subject_str, subject_ht, limit, flags);
	if (zcount) {
		ZEND_TRY_ASSIGN_REF_LONG(zcount, replace_count);
	}
}
/* }}} */

/* {{{ Perform Perl-style regular expression replacement using replacement callback. */
PHP_FUNCTION(preg_replace_callback_array)
{
	zval zv, *replace, *zcount = NULL;
	HashTable *pattern, *subject_ht;
	zend_string *subject_str, *str_idx_regex;
	zend_long limit = -1, flags = 0;
	size_t replace_count = 0;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	/* Get function parameters and do error-checking. */
	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_ARRAY_HT(pattern)
		Z_PARAM_ARRAY_HT_OR_STR(subject_ht, subject_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
		Z_PARAM_ZVAL(zcount)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	fci.size = sizeof(fci);
	fci.object = NULL;
	fci.named_params = NULL;

	if (subject_ht) {
		GC_TRY_ADDREF(subject_ht);
	} else {
		GC_TRY_ADDREF(subject_str);
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(pattern, str_idx_regex, replace) {
		if (!zend_is_callable_ex(replace, NULL, 0, NULL, &fcc, NULL)) {
			zend_argument_type_error(1, "must contain only valid callbacks");
			goto error;
		}
		if (!str_idx_regex) {
			zend_argument_type_error(1, "must contain only string patterns as keys");
			goto error;
		}

		ZVAL_COPY_VALUE(&fci.function_name, replace);

		replace_count += preg_replace_func_impl(&zv, str_idx_regex, /* regex_ht */ NULL, &fci, &fcc,
			subject_str, subject_ht, limit, flags);
		switch (Z_TYPE(zv)) {
			case IS_ARRAY:
				ZEND_ASSERT(subject_ht);
				zend_array_release(subject_ht);
				subject_ht = Z_ARR(zv);
				break;
			case IS_STRING:
				ZEND_ASSERT(subject_str);
				zend_string_release(subject_str);
				subject_str = Z_STR(zv);
				break;
			case IS_NULL:
				RETVAL_NULL();
				goto error;
			EMPTY_SWITCH_DEFAULT_CASE()
		}

		if (EG(exception)) {
			goto error;
		}
	} ZEND_HASH_FOREACH_END();

	if (zcount) {
		ZEND_TRY_ASSIGN_REF_LONG(zcount, replace_count);
	}

	if (subject_ht) {
		RETVAL_ARR(subject_ht);
		// Unset the type_flags of immutable arrays to prevent the VM from performing refcounting
		if (GC_FLAGS(subject_ht) & IS_ARRAY_IMMUTABLE) {
			Z_TYPE_FLAGS_P(return_value) = 0;
		}
		return;
	} else {
		RETURN_STR(subject_str);
	}

error:
	if (subject_ht) {
		zend_array_release(subject_ht);
	} else {
		zend_string_release(subject_str);
	}
}
/* }}} */

/* {{{ Perform Perl-style regular expression replacement and only return matches. */
PHP_FUNCTION(preg_filter)
{
	preg_replace_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} */

/* {{{ Split string into an array using a perl-style regular expression as a delimiter */
PHP_FUNCTION(preg_split)
{
	zend_string			*regex;			/* Regular expression */
	zend_string			*subject;		/* String to match against */
	zend_long			 limit_val = -1;/* Integer value of limit */
	zend_long			 flags = 0;		/* Match control flags */
	pcre_cache_entry	*pce;			/* Compiled regular expression */

	/* Get function parameters and do error checking */
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(regex)
		Z_PARAM_STR(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit_val)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		RETURN_FALSE;
	}

	pce->refcount++;
	php_pcre_split_impl(pce, subject, return_value, limit_val, flags);
	pce->refcount--;
}
/* }}} */

/* {{{ php_pcre_split */
PHPAPI void php_pcre_split_impl(pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
	zend_long limit_val, zend_long flags)
{
	PCRE2_SIZE		*offsets;			/* Array of subpattern offsets */
	uint32_t		 options;			/* Execution options */
	int				 count;				/* Count of matched subpatterns */
	PCRE2_SIZE		 start_offset;		/* Where the new search starts */
	PCRE2_SIZE		 last_match_offset;	/* Location of last match */
	uint32_t		 no_empty;			/* If NO_EMPTY flag is set */
	uint32_t		 delim_capture; 	/* If delimiters should be captured */
	uint32_t		 offset_capture;	/* If offsets should be captured */
	uint32_t		 num_subpats;		/* Number of captured subpatterns */
	zval			 tmp;
	pcre2_match_data *match_data;
	char *subject = ZSTR_VAL(subject_str);

	no_empty = flags & PREG_SPLIT_NO_EMPTY;
	delim_capture = flags & PREG_SPLIT_DELIM_CAPTURE;
	offset_capture = flags & PREG_SPLIT_OFFSET_CAPTURE;

	/* Initialize return value */
	array_init(return_value);
	HashTable *return_value_ht = Z_ARRVAL_P(return_value);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pce->capture_count + 1;

	/* Start at the beginning of the string */
	start_offset = 0;
	last_match_offset = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	if (limit_val == -1) {
		/* pass */
	} else if (limit_val == 0) {
		limit_val = -1;
	} else if (limit_val <= 1) {
		goto last;
	}

	if (!mdata_used && num_subpats <= PHP_PCRE_PREALLOC_MDATA_SIZE) {
		match_data = mdata;
	} else {
		match_data = pcre2_match_data_create_from_pattern(pce->re, PCRE_G(gctx_zmm));
		if (!match_data) {
			PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}
	}

	options = (pce->compile_options & PCRE2_UTF) ? 0 : PCRE2_NO_UTF_CHECK;

	offsets = pcre2_get_ovector_pointer(match_data);

#ifdef HAVE_PCRE_JIT_SUPPORT
	if ((pce->preg_options & PREG_JIT) && options) {
		count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, ZSTR_LEN(subject_str), start_offset,
				PCRE2_NO_UTF_CHECK, match_data, mctx);
	} else
#endif
	count = pcre2_match(pce->re, (PCRE2_SPTR)subject, ZSTR_LEN(subject_str), start_offset,
			options, match_data, mctx);

	while (1) {
		/* If something matched */
		if (count >= 0) {
			/* Check for too many substrings condition. */
			if (UNEXPECTED(count == 0)) {
				php_error_docref(NULL,E_NOTICE, "Matched, but too many substrings");
				count = num_subpats;
			}

matched:
			if (UNEXPECTED(offsets[1] < offsets[0])) {
				PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
				break;
			}

			if (!no_empty || offsets[0] != last_match_offset) {
				if (offset_capture) {
					/* Add (match, offset) pair to the return value */
					add_offset_pair(
						return_value_ht, subject, last_match_offset, offsets[0],
						NULL, 0);
				} else {
					/* Add the piece to the return value */
					populate_match_value_str(&tmp, subject, last_match_offset, offsets[0]);
					zend_hash_next_index_insert_new(return_value_ht, &tmp);
				}

				/* One less left to do */
				if (limit_val != -1)
					limit_val--;
			}

			if (delim_capture) {
				size_t i;
				for (i = 1; i < count; i++) {
					/* If we have matched a delimiter */
					if (!no_empty || offsets[2*i] != offsets[2*i+1]) {
						if (offset_capture) {
							add_offset_pair(
								return_value_ht, subject, offsets[2*i], offsets[2*i+1], NULL, 0);
						} else {
							populate_match_value_str(&tmp, subject, offsets[2*i], offsets[2*i+1]);
							zend_hash_next_index_insert_new(return_value_ht, &tmp);
						}
					}
				}
			}

			/* Advance to the position right after the last full match */
			start_offset = last_match_offset = offsets[1];

			/* If we have matched an empty string, mimic what Perl's /g options does.
			   This turns out to be rather cunning. First we set PCRE2_NOTEMPTY_ATSTART and try
			   the match again at the same point. If this fails (picked up above) we
			   advance to the next character. */
			if (start_offset == offsets[0]) {
				/* Get next piece if no limit or limit not yet reached and something matched*/
				if (limit_val != -1 && limit_val <= 1) {
					break;
				}
				count = pcre2_match(pce->re, (PCRE2_SPTR)subject, ZSTR_LEN(subject_str), start_offset,
					PCRE2_NO_UTF_CHECK | PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED, match_data, mctx);
				if (count >= 0) {
					goto matched;
				} else if (count == PCRE2_ERROR_NOMATCH) {
					/* If we previously set PCRE2_NOTEMPTY_ATSTART after a null match,
					   this is not necessarily the end. We need to advance
					   the start offset, and continue. Fudge the offset values
					   to achieve this, unless we're already at the end of the string. */
					if (start_offset < ZSTR_LEN(subject_str)) {
						start_offset += calculate_unit_length(pce, subject + start_offset);
					} else {
						break;
					}
				} else {
					goto error;
				}
			}

		} else if (count == PCRE2_ERROR_NOMATCH) {
			break;
		} else {
error:
			pcre_handle_exec_error(count);
			break;
		}

		/* Get next piece if no limit or limit not yet reached and something matched*/
		if (limit_val != -1 && limit_val <= 1) {
			break;
		}

#ifdef HAVE_PCRE_JIT_SUPPORT
		if (pce->preg_options & PREG_JIT) {
			count = pcre2_jit_match(pce->re, (PCRE2_SPTR)subject, ZSTR_LEN(subject_str), start_offset,
					PCRE2_NO_UTF_CHECK, match_data, mctx);
		} else
#endif
		count = pcre2_match(pce->re, (PCRE2_SPTR)subject, ZSTR_LEN(subject_str), start_offset,
				PCRE2_NO_UTF_CHECK, match_data, mctx);
	}
	if (match_data != mdata) {
		pcre2_match_data_free(match_data);
	}

	if (PCRE_G(error_code) != PHP_PCRE_NO_ERROR) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

last:
	start_offset = last_match_offset; /* the offset might have been incremented, but without further successful matches */

	if (!no_empty || start_offset < ZSTR_LEN(subject_str)) {
		if (offset_capture) {
			/* Add the last (match, offset) pair to the return value */
			add_offset_pair(return_value_ht, subject, start_offset, ZSTR_LEN(subject_str), NULL, 0);
		} else {
			/* Add the last piece to the return value */
			if (start_offset == 0) {
				ZVAL_STR_COPY(&tmp, subject_str);
			} else {
				populate_match_value_str(&tmp, subject, start_offset, ZSTR_LEN(subject_str));
			}
			zend_hash_next_index_insert_new(return_value_ht, &tmp);
		}
	}
}
/* }}} */

/* {{{ Quote regular expression characters plus an optional character */
PHP_FUNCTION(preg_quote)
{
	zend_string *str;       		/* Input string argument */
	zend_string	*delim = NULL;		/* Additional delimiter argument */
	char		*in_str;			/* Input string */
	char		*in_str_end;    	/* End of the input string */
	zend_string	*out_str;			/* Output string with quoted characters */
	size_t       extra_len;         /* Number of additional characters */
	char 		*p,					/* Iterator for input string */
				*q,					/* Iterator for output string */
				 delim_char = '\0',	/* Delimiter character to be quoted */
				 c;					/* Current character */

	/* Get the arguments and check for errors */
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(delim)
	ZEND_PARSE_PARAMETERS_END();

	/* Nothing to do if we got an empty string */
	if (ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_STRING();
	}

	in_str = ZSTR_VAL(str);
	in_str_end = in_str + ZSTR_LEN(str);

	if (delim) {
		delim_char = ZSTR_VAL(delim)[0];
	}

	/* Go through the string and quote necessary characters */
	extra_len = 0;
	p = in_str;
	do {
		c = *p;
		switch(c) {
			case '.':
			case '\\':
			case '+':
			case '*':
			case '?':
			case '[':
			case '^':
			case ']':
			case '$':
			case '(':
			case ')':
			case '{':
			case '}':
			case '=':
			case '!':
			case '>':
			case '<':
			case '|':
			case ':':
			case '-':
			case '#':
				extra_len++;
				break;

			case '\0':
				extra_len+=3;
				break;

			default:
				if (c == delim_char) {
					extra_len++;
				}
				break;
		}
		p++;
	} while (p != in_str_end);

	if (extra_len == 0) {
		RETURN_STR_COPY(str);
	}

	/* Allocate enough memory so that even if each character
	   is quoted, we won't run out of room */
	out_str = zend_string_safe_alloc(1, ZSTR_LEN(str), extra_len, 0);
	q = ZSTR_VAL(out_str);
	p = in_str;

	do {
		c = *p;
		switch(c) {
			case '.':
			case '\\':
			case '+':
			case '*':
			case '?':
			case '[':
			case '^':
			case ']':
			case '$':
			case '(':
			case ')':
			case '{':
			case '}':
			case '=':
			case '!':
			case '>':
			case '<':
			case '|':
			case ':':
			case '-':
			case '#':
				*q++ = '\\';
				*q++ = c;
				break;

			case '\0':
				*q++ = '\\';
				*q++ = '0';
				*q++ = '0';
				*q++ = '0';
				break;

			default:
				if (c == delim_char) {
					*q++ = '\\';
				}
				*q++ = c;
				break;
		}
		p++;
	} while (p != in_str_end);
	*q = '\0';

	RETURN_NEW_STR(out_str);
}
/* }}} */

/* {{{ Searches array and returns entries which match regex */
PHP_FUNCTION(preg_grep)
{
	zend_string			*regex;			/* Regular expression */
	zval				*input;			/* Input array */
	zend_long			 flags = 0;		/* Match control flags */
	pcre_cache_entry	*pce;			/* Compiled regular expression */

	/* Get arguments and do error checking */
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(regex)
		Z_PARAM_ARRAY(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		RETURN_FALSE;
	}

	pce->refcount++;
	php_pcre_grep_impl(pce, input, return_value, flags);
	pce->refcount--;
}
/* }}} */

PHPAPI void  php_pcre_grep_impl(pcre_cache_entry *pce, zval *input, zval *return_value, zend_long flags) /* {{{ */
{
	zval            *entry;             /* An entry in the input array */
	uint32_t		 num_subpats;		/* Number of captured subpatterns */
	int				 count;				/* Count of matched subpatterns */
	uint32_t		 options;			/* Execution options */
	zend_string		*string_key;
	zend_ulong		 num_key;
	bool		 invert;			/* Whether to return non-matching
										   entries */
	pcre2_match_data *match_data;
	invert = flags & PREG_GREP_INVERT ? 1 : 0;

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pce->capture_count + 1;

	/* Initialize return array */
	array_init(return_value);
	HashTable *return_value_ht = Z_ARRVAL_P(return_value);

	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	if (!mdata_used && num_subpats <= PHP_PCRE_PREALLOC_MDATA_SIZE) {
		match_data = mdata;
	} else {
		match_data = pcre2_match_data_create_from_pattern(pce->re, PCRE_G(gctx_zmm));
		if (!match_data) {
			PCRE_G(error_code) = PHP_PCRE_INTERNAL_ERROR;
			return;
		}
	}

	options = (pce->compile_options & PCRE2_UTF) ? 0 : PCRE2_NO_UTF_CHECK;

	/* Go through the input array */
	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(input), num_key, string_key, entry) {
		zend_string *tmp_subject_str;
		zend_string *subject_str = zval_get_tmp_string(entry, &tmp_subject_str);

		/* Perform the match */
#ifdef HAVE_PCRE_JIT_SUPPORT
		if ((pce->preg_options & PREG_JIT) && options) {
			count = pcre2_jit_match(pce->re, (PCRE2_SPTR)ZSTR_VAL(subject_str), ZSTR_LEN(subject_str), 0,
					PCRE2_NO_UTF_CHECK, match_data, mctx);
		} else
#endif
		count = pcre2_match(pce->re, (PCRE2_SPTR)ZSTR_VAL(subject_str), ZSTR_LEN(subject_str), 0,
				options, match_data, mctx);

		/* If the entry fits our requirements */
		if (count >= 0) {
			/* Check for too many substrings condition. */
			if (UNEXPECTED(count == 0)) {
				php_error_docref(NULL, E_NOTICE, "Matched, but too many substrings");
			}
			if (!invert) {
				Z_TRY_ADDREF_P(entry);

				/* Add to return array */
				if (string_key) {
					zend_hash_update(return_value_ht, string_key, entry);
				} else {
					zend_hash_index_update(return_value_ht, num_key, entry);
				}
			}
		} else if (count == PCRE2_ERROR_NOMATCH) {
			if (invert) {
				Z_TRY_ADDREF_P(entry);

				/* Add to return array */
				if (string_key) {
					zend_hash_update(return_value_ht, string_key, entry);
				} else {
					zend_hash_index_update(return_value_ht, num_key, entry);
				}
			}
		} else {
			pcre_handle_exec_error(count);
			zend_tmp_string_release(tmp_subject_str);
			break;
		}

		zend_tmp_string_release(tmp_subject_str);
	} ZEND_HASH_FOREACH_END();
	if (match_data != mdata) {
		pcre2_match_data_free(match_data);
	}
}
/* }}} */

/* {{{ Returns the error code of the last regexp execution. */
PHP_FUNCTION(preg_last_error)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(PCRE_G(error_code));
}
/* }}} */

/* {{{ Returns the error message of the last regexp execution. */
PHP_FUNCTION(preg_last_error_msg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRING(php_pcre_get_error_msg(PCRE_G(error_code)));
}
/* }}} */

/* {{{ module definition structures */

zend_module_entry pcre_module_entry = {
	STANDARD_MODULE_HEADER,
	"pcre",
	ext_functions,
	PHP_MINIT(pcre),
	PHP_MSHUTDOWN(pcre),
	PHP_RINIT(pcre),
	PHP_RSHUTDOWN(pcre),
	PHP_MINFO(pcre),
	PHP_PCRE_VERSION,
	PHP_MODULE_GLOBALS(pcre),
	PHP_GINIT(pcre),
	PHP_GSHUTDOWN(pcre),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_PCRE
ZEND_GET_MODULE(pcre)
#endif

/* }}} */

PHPAPI pcre2_match_context *php_pcre_mctx(void)
{/*{{{*/
	return mctx;
}/*}}}*/

PHPAPI pcre2_general_context *php_pcre_gctx(void)
{/*{{{*/
	return gctx;
}/*}}}*/

PHPAPI pcre2_compile_context *php_pcre_cctx(void)
{/*{{{*/
	return cctx;
}/*}}}*/

PHPAPI void php_pcre_pce_incref(pcre_cache_entry *pce)
{/*{{{*/
	assert(NULL != pce);
	pce->refcount++;
}/*}}}*/

PHPAPI void php_pcre_pce_decref(pcre_cache_entry *pce)
{/*{{{*/
	assert(NULL != pce);
	assert(0 != pce->refcount);
	pce->refcount--;
}/*}}}*/

PHPAPI pcre2_code *php_pcre_pce_re(pcre_cache_entry *pce)
{/*{{{*/
	assert(NULL != pce);
	return pce->re;
}/*}}}*/
