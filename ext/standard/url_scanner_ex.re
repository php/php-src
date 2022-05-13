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
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  |         Yasuo Ohgaki <yohgaki@ohgaki.net>                            |
  +----------------------------------------------------------------------+
*/

#include "php.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SAPI.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_string.h"
#define STATE_TAG SOME_OTHER_STATE_TAG
#include "basic_functions.h"
#include "url.h"
#include "html.h"
#undef STATE_TAG

#define url_scanner url_scanner_ex

#include "zend_smart_str.h"

static void tag_dtor(zval *zv)
{
	free(Z_PTR_P(zv));
}

static int php_ini_on_update_tags(zend_ini_entry *entry, zend_string *new_value, void *mh_arg1, void *mh_arg2, void *mh_arg3, int stage, int type)
{
	url_adapt_state_ex_t *ctx;
	char *key;
	char *tmp;
	char *lasts = NULL;

	if (type) {
		ctx = &BG(url_adapt_session_ex);
	} else {
		ctx = &BG(url_adapt_output_ex);
	}

	tmp = estrndup(ZSTR_VAL(new_value), ZSTR_LEN(new_value));

	if (ctx->tags)
		zend_hash_destroy(ctx->tags);
	else {
		ctx->tags = malloc(sizeof(HashTable));
		if (!ctx->tags) {
			efree(tmp);
			return FAILURE;
		}
	}

	zend_hash_init(ctx->tags, 0, NULL, tag_dtor, 1);

	for (key = php_strtok_r(tmp, ",", &lasts);
		 key;
		 key = php_strtok_r(NULL, ",", &lasts)) {
		char *val;

		val = strchr(key, '=');
		if (val) {
			char *q;
			size_t keylen;
			zend_string *str;

			*val++ = '\0';
			for (q = key; *q; q++) {
				*q = tolower(*q);
			}
			keylen = q - key;
			str = zend_string_init(key, keylen, 1);
			GC_MAKE_PERSISTENT_LOCAL(str);
			zend_hash_add_mem(ctx->tags, str, val, strlen(val)+1);
			zend_string_release_ex(str, 1);
		}
	}

	efree(tmp);

	return SUCCESS;
}

static PHP_INI_MH(OnUpdateSessionTags)
{
	return php_ini_on_update_tags(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage, 1);
}

static PHP_INI_MH(OnUpdateOutputTags)
{
	return php_ini_on_update_tags(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage, 0);
}

static int php_ini_on_update_hosts(zend_ini_entry *entry, zend_string *new_value, void *mh_arg1, void *mh_arg2, void *mh_arg3, int stage, int type)
{
	HashTable *hosts;
	char *key;
	char *tmp;
	char *lasts = NULL;

	if (type) {
		hosts = &BG(url_adapt_session_hosts_ht);
	} else {
		hosts = &BG(url_adapt_output_hosts_ht);
	}
	zend_hash_clean(hosts);

	/* Use user supplied host whitelist */
	tmp = estrndup(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
	for (key = php_strtok_r(tmp, ",", &lasts);
		 key;
		 key = php_strtok_r(NULL, ",", &lasts)) {
		size_t keylen;
		zend_string *tmp_key;
		char *q;

		for (q = key; *q; q++) {
			*q = tolower(*q);
		}
		keylen = q - key;
		if (keylen > 0) {
			tmp_key = zend_string_init(key, keylen, 0);
			zend_hash_add_empty_element(hosts, tmp_key);
			zend_string_release_ex(tmp_key, 0);
		}
	}
	efree(tmp);

	return SUCCESS;
}

static PHP_INI_MH(OnUpdateSessionHosts)
{
	return php_ini_on_update_hosts(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage, 1);
}

static PHP_INI_MH(OnUpdateOutputHosts)
{
	return php_ini_on_update_hosts(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage, 0);
}

/* FIXME: OnUpdate*Hosts cannot set default to $_SERVER['HTTP_HOST'] at startup */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("session.trans_sid_tags", "a=href,area=href,frame=src,form=", PHP_INI_ALL, OnUpdateSessionTags, url_adapt_session_ex, php_basic_globals, basic_globals)
	STD_PHP_INI_ENTRY("session.trans_sid_hosts", "", PHP_INI_ALL, OnUpdateSessionHosts, url_adapt_session_hosts_ht, php_basic_globals, basic_globals)
	STD_PHP_INI_ENTRY("url_rewriter.tags", "form=", PHP_INI_ALL, OnUpdateOutputTags, url_adapt_session_ex, php_basic_globals, basic_globals)
	STD_PHP_INI_ENTRY("url_rewriter.hosts", "", PHP_INI_ALL, OnUpdateOutputHosts, url_adapt_session_hosts_ht, php_basic_globals, basic_globals)
PHP_INI_END()

/*!re2c
any = [\000-\377];
N = (any\[<]);
alpha = [a-zA-Z];
alphanamespace = [a-zA-Z:];
alphadash = ([a-zA-Z] | "-");
*/

#define YYFILL(n) goto done
#define YYCTYPE unsigned char
#define YYCURSOR p
#define YYLIMIT q
#define YYMARKER r

static inline void append_modified_url(smart_str *url, smart_str *dest, smart_str *url_app, const char *separator)
{
	php_url *url_parts;

	smart_str_0(url); /* FIXME: Bug #70480 php_url_parse_ex() crashes by processing chars exceed len */
	url_parts = php_url_parse_ex(ZSTR_VAL(url->s), ZSTR_LEN(url->s));

	/* Ignore malformed URLs */
	if (!url_parts) {
		smart_str_append_smart_str(dest, url);
		return;
	}

	/* Don't modify URLs of the format "#mark" */
	if (url_parts->fragment && '#' == ZSTR_VAL(url->s)[0]) {
		smart_str_append_smart_str(dest, url);
		php_url_free(url_parts);
		return;
	}

	/* Check protocol. Only http/https is allowed. */
	if (url_parts->scheme
		&& !zend_string_equals_literal_ci(url_parts->scheme, "http")
		&& !zend_string_equals_literal_ci(url_parts->scheme, "https")) {
		smart_str_append_smart_str(dest, url);
		php_url_free(url_parts);
		return;
	}

	/* Check host whitelist. If it's not listed, do nothing. */
	if (url_parts->host) {
		zend_string *tmp = zend_string_tolower(url_parts->host);
		if (!zend_hash_exists(&BG(url_adapt_session_hosts_ht), tmp)) {
			zend_string_release_ex(tmp, 0);
			smart_str_append_smart_str(dest, url);
			php_url_free(url_parts);
			return;
		}
		zend_string_release_ex(tmp, 0);
	}

	/*
	 * When URL does not have path and query string add "/?".
	 * i.e. If URL is only "?foo=bar", should not add "/?".
	 */
	if (!url_parts->path && !url_parts->query && !url_parts->fragment) {
		/* URL is http://php.net or like */
		smart_str_append_smart_str(dest, url);
		smart_str_appendc(dest, '/');
		smart_str_appendc(dest, '?');
		smart_str_append_smart_str(dest, url_app);
		php_url_free(url_parts);
		return;
	}

	if (url_parts->scheme) {
		smart_str_appends(dest, ZSTR_VAL(url_parts->scheme));
		smart_str_appends(dest, "://");
	} else if (*(ZSTR_VAL(url->s)) == '/' && *(ZSTR_VAL(url->s)+1) == '/') {
		smart_str_appends(dest, "//");
	}
	if (url_parts->user) {
		smart_str_appends(dest, ZSTR_VAL(url_parts->user));
		if (url_parts->pass) {
			smart_str_appends(dest, ZSTR_VAL(url_parts->pass));
			smart_str_appendc(dest, ':');
		}
		smart_str_appendc(dest, '@');
	}
	if (url_parts->host) {
		smart_str_appends(dest, ZSTR_VAL(url_parts->host));
	}
	if (url_parts->port) {
		smart_str_appendc(dest, ':');
		smart_str_append_unsigned(dest, (long)url_parts->port);
	}
	if (url_parts->path) {
		smart_str_appends(dest, ZSTR_VAL(url_parts->path));
	}
	smart_str_appendc(dest, '?');
	if (url_parts->query) {
		smart_str_appends(dest, ZSTR_VAL(url_parts->query));
		smart_str_appends(dest, separator);
		smart_str_append_smart_str(dest, url_app);
	} else {
		smart_str_append_smart_str(dest, url_app);
	}
	if (url_parts->fragment) {
		smart_str_appendc(dest, '#');
		smart_str_appends(dest, ZSTR_VAL(url_parts->fragment));
	}
	php_url_free(url_parts);
}

enum {
	TAG_NORMAL = 0,
	TAG_FORM
};

enum {
	ATTR_NORMAL = 0,
	ATTR_ACTION
};

#undef YYFILL
#undef YYCTYPE
#undef YYCURSOR
#undef YYLIMIT
#undef YYMARKER

static inline void tag_arg(url_adapt_state_ex_t *ctx, char quotes, char type)
{
	char f = 0;

	/* arg.s is string WITHOUT NUL.
	   To avoid partial match, NUL is added here */
	ZSTR_VAL(ctx->arg.s)[ZSTR_LEN(ctx->arg.s)] = '\0';
	if (!strcasecmp(ZSTR_VAL(ctx->arg.s), ctx->lookup_data)) {
		f = 1;
	}

	if (quotes) {
		smart_str_appendc(&ctx->result, type);
	}
	if (f) {
		append_modified_url(&ctx->val, &ctx->result, &ctx->url_app, PG(arg_separator).output);
	} else {
		smart_str_append_smart_str(&ctx->result, &ctx->val);
	}
	if (quotes) {
		smart_str_appendc(&ctx->result, type);
	}
}

enum {
	STATE_PLAIN = 0,
	STATE_TAG,
	STATE_NEXT_ARG,
	STATE_ARG,
	STATE_BEFORE_VAL,
	STATE_VAL
};

#define YYFILL(n) goto stop
#define YYCTYPE unsigned char
#define YYCURSOR xp
#define YYLIMIT end
#define YYMARKER q
#define STATE ctx->state

#define STD_PARA url_adapt_state_ex_t *ctx, char *start, char *YYCURSOR
#define STD_ARGS ctx, start, xp

#ifdef SCANNER_DEBUG
#define scdebug(x) printf x
#else
#define scdebug(x)
#endif

static inline void passthru(STD_PARA)
{
	scdebug(("appending %d chars, starting with %c\n", YYCURSOR-start, *start));
	smart_str_appendl(&ctx->result, start, YYCURSOR - start);
}


static int check_http_host(char *target)
{
	zval *host, *tmp;
	zend_string *host_tmp;
	char *colon;

	if ((tmp = zend_hash_find(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER))) &&
		Z_TYPE_P(tmp) == IS_ARRAY &&
		(host = zend_hash_str_find(Z_ARRVAL_P(tmp), ZEND_STRL("HTTP_HOST"))) &&
		Z_TYPE_P(host) == IS_STRING) {
		host_tmp = zend_string_init(Z_STRVAL_P(host), Z_STRLEN_P(host), 0);
		/* HTTP_HOST could be 'localhost:8888' etc. */
		colon = strchr(ZSTR_VAL(host_tmp), ':');
		if (colon) {
			ZSTR_LEN(host_tmp) = colon - ZSTR_VAL(host_tmp);
			ZSTR_VAL(host_tmp)[ZSTR_LEN(host_tmp)] = '\0';
		}
		if (!strcasecmp(ZSTR_VAL(host_tmp), target)) {
			zend_string_release_ex(host_tmp, 0);
			return SUCCESS;
		}
		zend_string_release_ex(host_tmp, 0);
	}
	return FAILURE;
}

static int check_host_whitelist(url_adapt_state_ex_t *ctx)
{
	php_url *url_parts = NULL;
	HashTable *allowed_hosts = ctx->type ? &BG(url_adapt_session_hosts_ht) : &BG(url_adapt_output_hosts_ht);

	ZEND_ASSERT(ctx->tag_type == TAG_FORM);

	if (ctx->attr_val.s && ZSTR_LEN(ctx->attr_val.s)) {
		url_parts = php_url_parse_ex(ZSTR_VAL(ctx->attr_val.s), ZSTR_LEN(ctx->attr_val.s));
	} else {
		return SUCCESS; /* empty URL is valid */
	}

	if (!url_parts) {
		return FAILURE;
	}
	if (url_parts->scheme) {
		/* Only http/https should be handled.
		   A bit hacky check this here, but saves a URL parse. */
		if (!zend_string_equals_literal_ci(url_parts->scheme, "http") &&
			!zend_string_equals_literal_ci(url_parts->scheme, "https")) {
		php_url_free(url_parts);
		return FAILURE;
		}
	}
	if (!url_parts->host) {
		php_url_free(url_parts);
		return SUCCESS;
	}
	if (!zend_hash_num_elements(allowed_hosts) &&
		check_http_host(ZSTR_VAL(url_parts->host)) == SUCCESS) {
		php_url_free(url_parts);
		return SUCCESS;
	}
	if (!zend_hash_find(allowed_hosts, url_parts->host)) {
		php_url_free(url_parts);
		return FAILURE;
	}
	php_url_free(url_parts);
	return SUCCESS;
}

/*
 * This function appends a hidden input field after a <form>.
 */
static void handle_form(STD_PARA)
{
	int doit = 0;

	if (ZSTR_LEN(ctx->form_app.s) > 0) {
		switch (ZSTR_LEN(ctx->tag.s)) {
			case sizeof("form") - 1:
				if (!strncasecmp(ZSTR_VAL(ctx->tag.s), "form", ZSTR_LEN(ctx->tag.s))
					&& check_host_whitelist(ctx) == SUCCESS) {
					doit = 1;
				}
				break;
		}
	}

	if (doit) {
		smart_str_append_smart_str(&ctx->result, &ctx->form_app);
	}
}

/*
 *  HANDLE_TAG copies the HTML Tag and checks whether we
 *  have that tag in our table. If we might modify it,
 *  we continue to scan the tag, otherwise we simply copy the complete
 *  HTML stuff to the result buffer.
 */

static inline void handle_tag(STD_PARA)
{
	int ok = 0;
	unsigned int i;

	if (ctx->tag.s) {
		ZSTR_LEN(ctx->tag.s) = 0;
	}
	smart_str_appendl(&ctx->tag, start, YYCURSOR - start);
	for (i = 0; i < ZSTR_LEN(ctx->tag.s); i++)
		ZSTR_VAL(ctx->tag.s)[i] = tolower((int)(unsigned char)ZSTR_VAL(ctx->tag.s)[i]);
    /* intentionally using str_find here, in case the hash value is set, but the string val is changed later */
	if ((ctx->lookup_data = zend_hash_str_find_ptr(ctx->tags, ZSTR_VAL(ctx->tag.s), ZSTR_LEN(ctx->tag.s))) != NULL) {
		ok = 1;
		if (ZSTR_LEN(ctx->tag.s) == sizeof("form")-1
			&& !strncasecmp(ZSTR_VAL(ctx->tag.s), "form", ZSTR_LEN(ctx->tag.s))) {
			ctx->tag_type = TAG_FORM;
		} else {
			ctx->tag_type = TAG_NORMAL;
		}
	}
	STATE = ok ? STATE_NEXT_ARG : STATE_PLAIN;
}

static inline void handle_arg(STD_PARA)
{
	if (ctx->arg.s) {
		ZSTR_LEN(ctx->arg.s) = 0;
	}
	smart_str_appendl(&ctx->arg, start, YYCURSOR - start);
	if (ctx->tag_type == TAG_FORM &&
		strncasecmp(ZSTR_VAL(ctx->arg.s), "action", ZSTR_LEN(ctx->arg.s)) == 0) {
		ctx->attr_type = ATTR_ACTION;
	} else {
		ctx->attr_type = ATTR_NORMAL;
	}
}

static inline void handle_val(STD_PARA, char quotes, char type)
{
	smart_str_setl(&ctx->val, start + quotes, YYCURSOR - start - quotes * 2);
	if (ctx->tag_type == TAG_FORM && ctx->attr_type == ATTR_ACTION) {
		smart_str_setl(&ctx->attr_val, start + quotes, YYCURSOR - start - quotes * 2);
	}
	tag_arg(ctx, quotes, type);
}

static inline void xx_mainloop(url_adapt_state_ex_t *ctx, const char *newdata, size_t newlen)
{
	char *end, *q;
	char *xp;
	char *start;
	size_t rest;

	smart_str_appendl(&ctx->buf, newdata, newlen);

	YYCURSOR = ZSTR_VAL(ctx->buf.s);
	YYLIMIT = ZSTR_VAL(ctx->buf.s) + ZSTR_LEN(ctx->buf.s);

	switch (STATE) {
		case STATE_PLAIN: goto state_plain;
		case STATE_TAG: goto state_tag;
		case STATE_NEXT_ARG: goto state_next_arg;
		case STATE_ARG: goto state_arg;
		case STATE_BEFORE_VAL: goto state_before_val;
		case STATE_VAL: goto state_val;
	}


state_plain_begin:
	STATE = STATE_PLAIN;

state_plain:
	start = YYCURSOR;
/*!re2c
  "<"				{ passthru(STD_ARGS); STATE = STATE_TAG; goto state_tag; }
  N+ 				{ passthru(STD_ARGS); goto state_plain; }
*/

state_tag:
	start = YYCURSOR;
/*!re2c
  alphanamespace+	{ handle_tag(STD_ARGS); /* Sets STATE */; passthru(STD_ARGS); if (STATE == STATE_PLAIN) goto state_plain; else goto state_next_arg; }
  any		{ passthru(STD_ARGS); goto state_plain_begin; }
*/

state_next_arg_begin:
	STATE = STATE_NEXT_ARG;

state_next_arg:
	start = YYCURSOR;
/*!re2c
  [/]? [>]		{ passthru(STD_ARGS); handle_form(STD_ARGS); goto state_plain_begin; }
  [ \v\r\t\n]+	{ passthru(STD_ARGS); goto state_next_arg; }
  alpha		{ --YYCURSOR; STATE = STATE_ARG; goto state_arg; }
  any		{ passthru(STD_ARGS); goto state_plain_begin; }
*/

state_arg:
	start = YYCURSOR;
/*!re2c
  alpha alphadash*	{ passthru(STD_ARGS); handle_arg(STD_ARGS); STATE = STATE_BEFORE_VAL; goto state_before_val; }
  any		{ passthru(STD_ARGS); STATE = STATE_NEXT_ARG; goto state_next_arg; }
*/

state_before_val:
	start = YYCURSOR;
/*!re2c
  [ ]* "=" [ ]*		{ passthru(STD_ARGS); STATE = STATE_VAL; goto state_val; }
  any				{ --YYCURSOR; goto state_next_arg_begin; }
*/


state_val:
	start = YYCURSOR;
/*!re2c
  ["] (any\[">])* ["]	{ handle_val(STD_ARGS, 1, '"'); goto state_next_arg_begin; }
  ['] (any\['>])* [']	{ handle_val(STD_ARGS, 1, '\''); goto state_next_arg_begin; }
  (any\[ \r\t\n>'"])+	{ handle_val(STD_ARGS, 0, ' '); goto state_next_arg_begin; }
  any					{ passthru(STD_ARGS); goto state_next_arg_begin; }
*/

stop:
	if (YYLIMIT < start) {
		/* XXX: Crash avoidance. Need to work with reporter to figure out what goes wrong */
		rest = 0;
	} else {
		rest = YYLIMIT - start;
		scdebug(("stopped in state %d at pos %d (%d:%c) %d\n", STATE, YYCURSOR - ctx->buf.c, *YYCURSOR, *YYCURSOR, rest));
	}

	if (rest) memmove(ZSTR_VAL(ctx->buf.s), start, rest);
	ZSTR_LEN(ctx->buf.s) = rest;
}


PHPAPI char *php_url_scanner_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, size_t *newlen, int encode)
{
	char *result;
	smart_str surl = {0};
	smart_str buf = {0};
	smart_str url_app = {0};
	zend_string *encoded;

	smart_str_appendl(&surl, url, urllen);

	if (encode) {
		encoded = php_raw_url_encode(name, strlen(name));
		smart_str_appendl(&url_app, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
		zend_string_free(encoded);
	} else {
		smart_str_appends(&url_app, name);
	}
	smart_str_appendc(&url_app, '=');
	if (encode) {
		encoded = php_raw_url_encode(value, strlen(value));
		smart_str_appendl(&url_app, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
		zend_string_free(encoded);
	} else {
		smart_str_appends(&url_app, value);
	}

	append_modified_url(&surl, &buf, &url_app, PG(arg_separator).output);

	smart_str_0(&buf);
	if (newlen) *newlen = ZSTR_LEN(buf.s);
	result = estrndup(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));

	smart_str_free(&url_app);
	smart_str_free(&buf);

	return result;
}


static char *url_adapt_ext(const char *src, size_t srclen, size_t *newlen, bool do_flush, url_adapt_state_ex_t *ctx)
{
	char *retval;

	xx_mainloop(ctx, src, srclen);

	if (!ctx->result.s) {
		smart_str_appendl(&ctx->result, "", 0);
		*newlen = 0;
	} else {
		*newlen = ZSTR_LEN(ctx->result.s);
	}
	smart_str_0(&ctx->result);
	if (do_flush) {
		smart_str_append(&ctx->result, ctx->buf.s);
		*newlen += ZSTR_LEN(ctx->buf.s);
		smart_str_free(&ctx->buf);
		smart_str_free(&ctx->val);
		smart_str_free(&ctx->attr_val);
	}
	retval = estrndup(ZSTR_VAL(ctx->result.s), ZSTR_LEN(ctx->result.s));
	smart_str_free(&ctx->result);
	return retval;
}

static int php_url_scanner_ex_activate(int type)
{
	url_adapt_state_ex_t *ctx;

	if (type) {
		ctx = &BG(url_adapt_session_ex);
	} else {
		ctx = &BG(url_adapt_output_ex);
	}

	memset(ctx, 0, XtOffsetOf(url_adapt_state_ex_t, tags));

	return SUCCESS;
}

static int php_url_scanner_ex_deactivate(int type)
{
	url_adapt_state_ex_t *ctx;

	if (type) {
		ctx = &BG(url_adapt_session_ex);
	} else {
		ctx = &BG(url_adapt_output_ex);
	}

	smart_str_free(&ctx->result);
	smart_str_free(&ctx->buf);
	smart_str_free(&ctx->tag);
	smart_str_free(&ctx->arg);
	smart_str_free(&ctx->attr_val);

	return SUCCESS;
}

static inline void php_url_scanner_session_handler_impl(char *output, size_t output_len, char **handled_output, size_t *handled_output_len, int mode, int type)
{
	size_t len;
	url_adapt_state_ex_t *url_state;

	if (type) {
		url_state = &BG(url_adapt_session_ex);
	} else {
		url_state = &BG(url_adapt_output_ex);
	}

	if (ZSTR_LEN(url_state->url_app.s) != 0) {
		*handled_output = url_adapt_ext(output, output_len, &len, (bool) (mode & (PHP_OUTPUT_HANDLER_END | PHP_OUTPUT_HANDLER_CONT | PHP_OUTPUT_HANDLER_FLUSH | PHP_OUTPUT_HANDLER_FINAL) ? 1 : 0), url_state);
		if (sizeof(unsigned int) < sizeof(size_t)) {
			if (len > UINT_MAX)
				len = UINT_MAX;
		}
		*handled_output_len = len;
	} else if (ZSTR_LEN(url_state->url_app.s) == 0) {
		url_adapt_state_ex_t *ctx = url_state;
		if (ctx->buf.s && ZSTR_LEN(ctx->buf.s)) {
			smart_str_append(&ctx->result, ctx->buf.s);
			smart_str_appendl(&ctx->result, output, output_len);

			*handled_output = estrndup(ZSTR_VAL(ctx->result.s), ZSTR_LEN(ctx->result.s));
			*handled_output_len = ZSTR_LEN(ctx->buf.s) + output_len;

			smart_str_free(&ctx->buf);
			smart_str_free(&ctx->result);
		} else {
			*handled_output = estrndup(output, *handled_output_len = output_len);
		}
	} else {
		*handled_output = NULL;
	}
}

static void php_url_scanner_session_handler(char *output, size_t output_len, char **handled_output, size_t *handled_output_len, int mode)
{
	php_url_scanner_session_handler_impl(output, output_len, handled_output, handled_output_len, mode, 1);
}

static void php_url_scanner_output_handler(char *output, size_t output_len, char **handled_output, size_t *handled_output_len, int mode)
{
	php_url_scanner_session_handler_impl(output, output_len, handled_output, handled_output_len, mode, 0);
}

static inline int php_url_scanner_add_var_impl(const char *name, size_t name_len, const char *value, size_t value_len, int encode, int type)
{
	smart_str sname = {0};
	smart_str svalue = {0};
	smart_str hname = {0};
	smart_str hvalue = {0};
	zend_string *encoded;
	url_adapt_state_ex_t *url_state;
	php_output_handler_func_t handler;

	if (type) {
		url_state = &BG(url_adapt_session_ex);
		handler = php_url_scanner_session_handler;
	} else {
		url_state = &BG(url_adapt_output_ex);
		handler = php_url_scanner_output_handler;
	}

	if (!url_state->active) {
		php_url_scanner_ex_activate(type);
		php_output_start_internal(ZEND_STRL("URL-Rewriter"), handler, 0, PHP_OUTPUT_HANDLER_STDFLAGS);
		url_state->active = 1;
	}

	if (url_state->url_app.s && ZSTR_LEN(url_state->url_app.s) != 0) {
		smart_str_appends(&url_state->url_app, PG(arg_separator).output);
	}

	if (encode) {
		encoded = php_raw_url_encode(name, name_len);
		smart_str_appendl(&sname, ZSTR_VAL(encoded), ZSTR_LEN(encoded)); zend_string_free(encoded);
		encoded = php_raw_url_encode(value, value_len);
		smart_str_appendl(&svalue, ZSTR_VAL(encoded), ZSTR_LEN(encoded)); zend_string_free(encoded);
		encoded = php_escape_html_entities_ex((const unsigned char *) name, name_len, 0, ENT_QUOTES|ENT_SUBSTITUTE, NULL, /* double_encode */ 0, /* quiet */ 1);
		smart_str_appendl(&hname, ZSTR_VAL(encoded), ZSTR_LEN(encoded)); zend_string_free(encoded);
		encoded = php_escape_html_entities_ex((const unsigned char *) value, value_len, 0, ENT_QUOTES|ENT_SUBSTITUTE, NULL, /* double_encode */ 0, /* quiet */ 1);
		smart_str_appendl(&hvalue, ZSTR_VAL(encoded), ZSTR_LEN(encoded)); zend_string_free(encoded);
	} else {
		smart_str_appendl(&sname, name, name_len);
		smart_str_appendl(&svalue, value, value_len);
		smart_str_appendl(&hname, name, name_len);
		smart_str_appendl(&hvalue, value, value_len);
	}

	smart_str_append_smart_str(&url_state->url_app, &sname);
	smart_str_appendc(&url_state->url_app, '=');
	smart_str_append_smart_str(&url_state->url_app, &svalue);

	smart_str_appends(&url_state->form_app, "<input type=\"hidden\" name=\"");
	smart_str_append_smart_str(&url_state->form_app, &hname);
	smart_str_appends(&url_state->form_app, "\" value=\"");
	smart_str_append_smart_str(&url_state->form_app, &hvalue);
	smart_str_appends(&url_state->form_app, "\" />");

	smart_str_free(&sname);
	smart_str_free(&svalue);
	smart_str_free(&hname);
	smart_str_free(&hvalue);

	return SUCCESS;
}


PHPAPI int php_url_scanner_add_session_var(const char *name, size_t name_len, const char *value, size_t value_len, int encode)
{
	return php_url_scanner_add_var_impl(name, name_len, value, value_len, encode, 1);
}


PHPAPI int php_url_scanner_add_var(const char *name, size_t name_len, const char *value, size_t value_len, int encode)
{
	return php_url_scanner_add_var_impl(name, name_len, value, value_len, encode, 0);
}


static inline void php_url_scanner_reset_vars_impl(int type) {
	url_adapt_state_ex_t *url_state;

	if (type) {
		url_state = &BG(url_adapt_session_ex);
	} else {
		url_state = &BG(url_adapt_output_ex);
	}

	if (url_state->form_app.s) {
		ZSTR_LEN(url_state->form_app.s) = 0;
	}
	if (url_state->url_app.s) {
		ZSTR_LEN(url_state->url_app.s) = 0;
	}
}


PHPAPI int php_url_scanner_reset_session_vars(void)
{
	php_url_scanner_reset_vars_impl(1);
	return SUCCESS;
}


PHPAPI int php_url_scanner_reset_vars(void)
{
	php_url_scanner_reset_vars_impl(0);
	return SUCCESS;
}


static inline int php_url_scanner_reset_var_impl(zend_string *name, int encode, int type)
{
	char *start, *end, *limit;
	size_t separator_len;
	smart_str sname = {0};
	smart_str hname = {0};
	smart_str url_app = {0};
	smart_str form_app = {0};
	zend_string *encoded;
	int ret = SUCCESS;
	bool sep_removed = 0;
	url_adapt_state_ex_t *url_state;

	if (type) {
		url_state = &BG(url_adapt_session_ex);
	} else {
		url_state = &BG(url_adapt_output_ex);
	}

	/* Short circuit check. Only check url_app. */
	if (!url_state->url_app.s || !ZSTR_LEN(url_state->url_app.s)) {
		return SUCCESS;
	}

	if (encode) {
		encoded = php_raw_url_encode(ZSTR_VAL(name), ZSTR_LEN(name));
		smart_str_appendl(&sname, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
		zend_string_free(encoded);
		encoded = php_escape_html_entities_ex((const unsigned char *) ZSTR_VAL(name), ZSTR_LEN(name), 0, ENT_QUOTES|ENT_SUBSTITUTE, SG(default_charset), /* double_encode */ 0, /* quiet */ 1);
		smart_str_appendl(&hname, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
		zend_string_free(encoded);
	} else {
		smart_str_appendl(&sname, ZSTR_VAL(name), ZSTR_LEN(name));
		smart_str_appendl(&hname, ZSTR_VAL(name), ZSTR_LEN(name));
	}
	smart_str_0(&sname);
	smart_str_0(&hname);

	smart_str_append_smart_str(&url_app, &sname);
	smart_str_appendc(&url_app, '=');
	smart_str_0(&url_app);

	smart_str_appends(&form_app, "<input type=\"hidden\" name=\"");
	smart_str_append_smart_str(&form_app, &hname);
	smart_str_appends(&form_app, "\" value=\"");
	smart_str_0(&form_app);

	/* Short circuit check. Only check url_app. */
	start = (char *) php_memnstr(ZSTR_VAL(url_state->url_app.s),
								 ZSTR_VAL(url_app.s), ZSTR_LEN(url_app.s),
								 ZSTR_VAL(url_state->url_app.s) + ZSTR_LEN(url_state->url_app.s));
	if (!start) {
		ret = FAILURE;
		goto finish;
	}

	/* Get end of url var */
	limit = ZSTR_VAL(url_state->url_app.s) + ZSTR_LEN(url_state->url_app.s);
	end = start + ZSTR_LEN(url_app.s);
	separator_len = strlen(PG(arg_separator).output);
	while (end < limit) {
		if (!memcmp(end, PG(arg_separator).output, separator_len)) {
			end += separator_len;
			sep_removed = 1;
			break;
		}
		end++;
	}
	/* Remove all when this is the only rewrite var */
	if (ZSTR_LEN(url_state->url_app.s) == end - start) {
		php_url_scanner_reset_vars_impl(type);
		goto finish;
	}
	/* Check preceding separator */
	if (!sep_removed
		&& (size_t)(start - PG(arg_separator).output) >= separator_len
		&& !memcmp(start - separator_len, PG(arg_separator).output, separator_len)) {
		start -= separator_len;
	}
	/* Remove partially */
	memmove(start, end,
			ZSTR_LEN(url_state->url_app.s) - (end - ZSTR_VAL(url_state->url_app.s)));
	ZSTR_LEN(url_state->url_app.s) -= end - start;
	ZSTR_VAL(url_state->url_app.s)[ZSTR_LEN(url_state->url_app.s)] = '\0';

	/* Remove form var */
	start = (char *) php_memnstr(ZSTR_VAL(url_state->form_app.s),
						ZSTR_VAL(form_app.s), ZSTR_LEN(form_app.s),
						ZSTR_VAL(url_state->form_app.s) + ZSTR_LEN(url_state->form_app.s));
	if (!start) {
		/* Should not happen */
		ret = FAILURE;
		php_url_scanner_reset_vars_impl(type);
		goto finish;
	}
	/* Get end of form var */
	limit = ZSTR_VAL(url_state->form_app.s) + ZSTR_LEN(url_state->form_app.s);
	end = start + ZSTR_LEN(form_app.s);
	while (end < limit) {
		if (*end == '>') {
			end += 1;
			break;
		}
		end++;
	}
	/* Remove partially */
	memmove(start, end,
			ZSTR_LEN(url_state->form_app.s) - (end - ZSTR_VAL(url_state->form_app.s)));
	ZSTR_LEN(url_state->form_app.s) -= end - start;
	ZSTR_VAL(url_state->form_app.s)[ZSTR_LEN(url_state->form_app.s)] = '\0';

finish:
	smart_str_free(&url_app);
	smart_str_free(&form_app);
	smart_str_free(&sname);
	smart_str_free(&hname);
	return ret;
}


PHPAPI int php_url_scanner_reset_session_var(zend_string *name, int encode)
{
	return php_url_scanner_reset_var_impl(name, encode, 1);
}


PHPAPI int php_url_scanner_reset_var(zend_string *name, int encode)
{
	return php_url_scanner_reset_var_impl(name, encode, 0);
}


PHP_MINIT_FUNCTION(url_scanner)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(url_scanner)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

PHP_RINIT_FUNCTION(url_scanner)
{
	BG(url_adapt_session_ex).active    = 0;
	BG(url_adapt_session_ex).tag_type  = 0;
	BG(url_adapt_session_ex).attr_type = 0;
	BG(url_adapt_output_ex).active    = 0;
	BG(url_adapt_output_ex).tag_type  = 0;
	BG(url_adapt_output_ex).attr_type = 0;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(url_scanner)
{
	if (BG(url_adapt_session_ex).active) {
		php_url_scanner_ex_deactivate(1);
		BG(url_adapt_session_ex).active    = 0;
		BG(url_adapt_session_ex).tag_type  = 0;
		BG(url_adapt_session_ex).attr_type = 0;
	}
	smart_str_free(&BG(url_adapt_session_ex).form_app);
	smart_str_free(&BG(url_adapt_session_ex).url_app);

	if (BG(url_adapt_output_ex).active) {
		php_url_scanner_ex_deactivate(0);
		BG(url_adapt_output_ex).active    = 0;
		BG(url_adapt_output_ex).tag_type  = 0;
		BG(url_adapt_output_ex).attr_type = 0;
	}
	smart_str_free(&BG(url_adapt_output_ex).form_app);
	smart_str_free(&BG(url_adapt_output_ex).url_app);

	return SUCCESS;
}
