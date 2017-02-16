/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "php_ini.h"
#include "php_globals.h"
#include "php_string.h"
#define STATE_TAG SOME_OTHER_STATE_TAG
#include "basic_functions.h"
#include "url.h"
#undef STATE_TAG

#define url_scanner url_scanner_ex

#include "zend_smart_str.h"

static void tag_dtor(zval *zv)
{
	free(Z_PTR_P(zv));
}

static PHP_INI_MH(OnUpdateTags)
{
	url_adapt_state_ex_t *ctx;
	char *key;
	char *tmp;
	char *lasts = NULL;

	ctx = &BG(url_adapt_state_ex);

	tmp = estrndup(ZSTR_VAL(new_value), ZSTR_LEN(new_value));

	if (ctx->tags)
		zend_hash_destroy(ctx->tags);
	else {
		ctx->tags = malloc(sizeof(HashTable));
		if (!ctx->tags) {
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

			*val++ = '\0';
			for (q = key; *q; q++)
				*q = tolower(*q);
			keylen = q - key;
			/* key is stored withOUT NUL
			   val is stored WITH    NUL */
			zend_hash_str_add_mem(ctx->tags, key, keylen, val, strlen(val)+1);
		}
	}

	efree(tmp);

	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("url_rewriter.tags", "a=href,area=href,frame=src,form=,fieldset=", PHP_INI_ALL, OnUpdateTags, url_adapt_state_ex, php_basic_globals, basic_globals)
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
	register const char *p, *q;
	const char *bash = NULL;
	const char *sep = "?";

	/*
	 * Don't modify "//example.com" full path, unless
	 * HTTP_HOST matches.
	 */
	if (ZSTR_LEN(url->s) > 2 && ZSTR_VAL(url->s)[0] == '/' && ZSTR_VAL(url->s)[1] == '/') {
		const char *end_chars = "/\"'?>\r\n";
		zval *tmp = NULL, *http_host = NULL;
		size_t target_len, host_len;
		if ((!(tmp = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SERVER"))))
			|| Z_TYPE_P(tmp) != IS_ARRAY
			|| !(http_host = zend_hash_str_find(HASH_OF(tmp), ZEND_STRL("HTTP_HOST")))
			|| Z_TYPE_P(http_host) != IS_STRING) {
			smart_str_append_smart_str(dest, url);
			return;
		}

		/* HTTP_HOST could be "example.com:8888", etc. */
		/* Need to find end of URL in buffer */
		host_len   = strcspn(Z_STRVAL_P(http_host), ":");
		target_len = php_strcspn(
			ZSTR_VAL(url->s) + 2, (char *) end_chars,
			ZSTR_VAL(url->s) + ZSTR_LEN(url->s), (char *) end_chars + strlen(end_chars));
		if (host_len
			&& host_len == target_len
			&& strncasecmp(Z_STRVAL_P(http_host), ZSTR_VAL(url->s)+2, host_len)) {
			smart_str_append_smart_str(dest, url);
			return;
		}
	}

	q = (p = ZSTR_VAL(url->s)) + ZSTR_LEN(url->s);

scan:
/*!re2c
  ":"		{ smart_str_append_smart_str(dest, url); return; }
  "?"		{ sep = separator; goto scan; }
  "#"		{ bash = p - 1; goto done; }
  (any\[:?#])+		{ goto scan; }
*/
done:

	/* Don't modify URLs of the format "#mark" */
	if (bash && bash - ZSTR_VAL(url->s) == 0) {
		smart_str_append_smart_str(dest, url);
		return;
	}

	if (bash)
		smart_str_appendl(dest, ZSTR_VAL(url->s), bash - ZSTR_VAL(url->s));
	else
		smart_str_append_smart_str(dest, url);

	smart_str_appends(dest, sep);
	smart_str_append_smart_str(dest, url_app);

	if (bash)
		smart_str_appendl(dest, bash, q - bash);
}


#undef YYFILL
#undef YYCTYPE
#undef YYCURSOR
#undef YYLIMIT
#undef YYMARKER

static inline void tag_arg(url_adapt_state_ex_t *ctx, char quotes, char type)
{
	char f = 0;

	if (strncasecmp(ZSTR_VAL(ctx->arg.s), ctx->lookup_data, ZSTR_LEN(ctx->arg.s)) == 0)
		f = 1;

	if (quotes)
		smart_str_appendc(&ctx->result, type);
	if (f) {
		append_modified_url(&ctx->val, &ctx->result, &ctx->url_app, PG(arg_separator).output);
	} else {
		smart_str_append_smart_str(&ctx->result, &ctx->val);
	}
	if (quotes)
		smart_str_appendc(&ctx->result, type);
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

#if SCANNER_DEBUG
#define scdebug(x) printf x
#else
#define scdebug(x)
#endif

static inline void passthru(STD_PARA)
{
	scdebug(("appending %d chars, starting with %c\n", YYCURSOR-start, *start));
	smart_str_appendl(&ctx->result, start, YYCURSOR - start);
}

/*
 * This function appends a hidden input field after a <form> or
 * <fieldset>.  The latter is important for XHTML.
 */

static void handle_form(STD_PARA)
{
	int doit = 0;

	if (ZSTR_LEN(ctx->form_app.s) > 0) {
		switch (ZSTR_LEN(ctx->tag.s)) {
			case sizeof("form") - 1:
				if (!strncasecmp(ZSTR_VAL(ctx->tag.s), "form", sizeof("form") - 1)) {
					doit = 1;
				}
				if (doit && ctx->val.s && ctx->lookup_data && *ctx->lookup_data) {
					char *e, *p = (char *)zend_memnstr(ZSTR_VAL(ctx->val.s), "://", sizeof("://") - 1, ZSTR_VAL(ctx->val.s) + ZSTR_LEN(ctx->val.s));
					if (p) {
						e = memchr(p, '/', (ZSTR_VAL(ctx->val.s) + ZSTR_LEN(ctx->val.s)) - p);
						if (!e) {
							e = ZSTR_VAL(ctx->val.s) + ZSTR_LEN(ctx->val.s);
						}
						if ((e - p) && strncasecmp(p, ctx->lookup_data, (e - p))) {
							doit = 0;
						}
					}
				}
				break;

			case sizeof("fieldset") - 1:
				if (!strncasecmp(ZSTR_VAL(ctx->tag.s), "fieldset", sizeof("fieldset") - 1)) {
					doit = 1;
				}
				break;
		}

		if (doit)
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
	if ((ctx->lookup_data = zend_hash_str_find_ptr(ctx->tags, ZSTR_VAL(ctx->tag.s), ZSTR_LEN(ctx->tag.s))) != NULL)
		ok = 1;
	STATE = ok ? STATE_NEXT_ARG : STATE_PLAIN;
}

static inline void handle_arg(STD_PARA)
{
	if (ctx->arg.s) {
		ZSTR_LEN(ctx->arg.s) = 0;
	}
	smart_str_appendl(&ctx->arg, start, YYCURSOR - start);
}

static inline void handle_val(STD_PARA, char quotes, char type)
{
	smart_str_setl(&ctx->val, start + quotes, YYCURSOR - start - quotes * 2);
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


PHPAPI char *php_url_scanner_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, size_t *newlen, int urlencode)
{
	char *result;
	smart_str surl = {0};
	smart_str buf = {0};
	smart_str url_app = {0};
	zend_string *encoded;

	smart_str_appendl(&surl, url, urllen);

	if (urlencode) {
		encoded = php_raw_url_encode(name, strlen(name));
		smart_str_appendl(&url_app, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
		zend_string_free(encoded);
	} else {
		smart_str_appends(&url_app, name);
	}
	smart_str_appendc(&url_app, '=');
	if (urlencode) {
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


static char *url_adapt_ext(const char *src, size_t srclen, size_t *newlen, zend_bool do_flush)
{
	url_adapt_state_ex_t *ctx;
	char *retval;

	ctx = &BG(url_adapt_state_ex);

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
	}
	retval = estrndup(ZSTR_VAL(ctx->result.s), ZSTR_LEN(ctx->result.s));
	smart_str_free(&ctx->result);
	return retval;
}

static int php_url_scanner_ex_activate(void)
{
	url_adapt_state_ex_t *ctx;

	ctx = &BG(url_adapt_state_ex);

	memset(ctx, 0, ((size_t) &((url_adapt_state_ex_t *)0)->tags));

	return SUCCESS;
}

static int php_url_scanner_ex_deactivate(void)
{
	url_adapt_state_ex_t *ctx;

	ctx = &BG(url_adapt_state_ex);

	smart_str_free(&ctx->result);
	smart_str_free(&ctx->buf);
	smart_str_free(&ctx->tag);
	smart_str_free(&ctx->arg);

	return SUCCESS;
}

static void php_url_scanner_output_handler(char *output, size_t output_len, char **handled_output, size_t *handled_output_len, int mode)
{
	size_t len;

	if (ZSTR_LEN(BG(url_adapt_state_ex).url_app.s) != 0) {
		*handled_output = url_adapt_ext(output, output_len, &len, (zend_bool) (mode & (PHP_OUTPUT_HANDLER_END | PHP_OUTPUT_HANDLER_CONT | PHP_OUTPUT_HANDLER_FLUSH | PHP_OUTPUT_HANDLER_FINAL) ? 1 : 0));
		if (sizeof(uint) < sizeof(size_t)) {
			if (len > UINT_MAX)
				len = UINT_MAX;
		}
		*handled_output_len = len;
	} else if (ZSTR_LEN(BG(url_adapt_state_ex).url_app.s) == 0) {
		url_adapt_state_ex_t *ctx = &BG(url_adapt_state_ex);
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

PHPAPI int php_url_scanner_add_var(char *name, size_t name_len, char *value, size_t value_len, int urlencode)
{
	smart_str sname = {0};
	smart_str svalue = {0};
	zend_string *encoded;

	if (!BG(url_adapt_state_ex).active) {
		php_url_scanner_ex_activate();
		php_output_start_internal(ZEND_STRL("URL-Rewriter"), php_url_scanner_output_handler, 0, PHP_OUTPUT_HANDLER_STDFLAGS);
		BG(url_adapt_state_ex).active = 1;
	}

	if (BG(url_adapt_state_ex).url_app.s && ZSTR_LEN(BG(url_adapt_state_ex).url_app.s) != 0) {
		smart_str_appends(&BG(url_adapt_state_ex).url_app, PG(arg_separator).output);
	}

	if (urlencode) {
		encoded = php_raw_url_encode(name, name_len);
		smart_str_appendl(&sname, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
		zend_string_free(encoded);
		encoded = php_raw_url_encode(value, value_len);
		smart_str_appendl(&svalue, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
		zend_string_free(encoded);
	} else {
		smart_str_appendl(&sname, name, name_len);
		smart_str_appendl(&svalue, value, value_len);
	}

	smart_str_append_smart_str(&BG(url_adapt_state_ex).url_app, &sname);
	smart_str_appendc(&BG(url_adapt_state_ex).url_app, '=');
	smart_str_append_smart_str(&BG(url_adapt_state_ex).url_app, &svalue);

	smart_str_appends(&BG(url_adapt_state_ex).form_app, "<input type=\"hidden\" name=\"");
	smart_str_append_smart_str(&BG(url_adapt_state_ex).form_app, &sname);
	smart_str_appends(&BG(url_adapt_state_ex).form_app, "\" value=\"");
	smart_str_append_smart_str(&BG(url_adapt_state_ex).form_app, &svalue);
	smart_str_appends(&BG(url_adapt_state_ex).form_app, "\" />");

	smart_str_free(&sname);
	smart_str_free(&svalue);

	return SUCCESS;
}

PHPAPI int php_url_scanner_reset_vars(void)
{
	if (BG(url_adapt_state_ex).form_app.s) {
		ZSTR_LEN(BG(url_adapt_state_ex).form_app.s) = 0;
	}
	if (BG(url_adapt_state_ex).url_app.s) {
		ZSTR_LEN(BG(url_adapt_state_ex).url_app.s) = 0;
	}

	return SUCCESS;
}

PHP_MINIT_FUNCTION(url_scanner)
{
	BG(url_adapt_state_ex).tags = NULL;

	BG(url_adapt_state_ex).form_app.s = BG(url_adapt_state_ex).url_app.s = NULL;

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
	BG(url_adapt_state_ex).active = 0;

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(url_scanner)
{
	if (BG(url_adapt_state_ex).active) {
		php_url_scanner_ex_deactivate();
		BG(url_adapt_state_ex).active = 0;
	}

	smart_str_free(&BG(url_adapt_state_ex).form_app);
	smart_str_free(&BG(url_adapt_state_ex).url_app);

	return SUCCESS;
}
