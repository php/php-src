/*
  +----------------------------------------------------------------------+
  | PHP version 4.0                                                      |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Sascha Schumann <sascha@schumann.cx>                        |
  +----------------------------------------------------------------------+
*/

#include "php.h"

#ifdef TRANS_SID

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "php_globals.h"
#define STATE_TAG SOME_OTHER_STATE_TAG
#include "basic_functions.h"
#undef STATE_TAG

#define url_adapt_ext url_adapt_ext_ex
#define url_scanner url_scanner_ex
#define url_adapt_state url_adapt_state_ex
#define url_adapt_state_t url_adapt_state_ex_t

static inline void smart_str_append(smart_str *dest, smart_str *src)
{
	size_t newlen;

	if (!dest->c)
		dest->len = dest->a = 0;
	
	newlen = dest->len + src->len;
	if (newlen >= dest->a) {
		dest->c = realloc(dest->c, newlen + 101);
		dest->a = newlen + 100;
	}
	memcpy(dest->c + dest->len, src->c, src->len);
	dest->c[dest->len = newlen] = '\0';
}

static inline void smart_str_free(smart_str *s)
{
	if (s->c) {
		free(s->c);
		s->c = NULL;
	}
	s->a = s->len = 0;
}

static inline void smart_str_copyl(smart_str *dest, const char *src, size_t len)
{
	dest->c = realloc(dest->c, len + 1);
	memcpy(dest->c, src, len);
	dest->c[len] = '\0';
	dest->a = dest->len = len;
}

static inline void smart_str_appendl(smart_str *dest, const char *src, size_t len)
{
	smart_str s;

	s.c = (char *) src;
	s.len = len;

	smart_str_append(dest, &s);
}

static inline void smart_str_setl(smart_str *dest, const char *src, size_t len)
{
	dest->len = len;
	dest->a = len + 1;
	dest->c = src;
}

static inline void smart_str_appends(smart_str *dest, const char *src)
{
	smart_str_appendl(dest, src, strlen(src));
}

static inline void smart_str_copys(smart_str *dest, const char *src)
{
	smart_str_copyl(dest, src, strlen(src));
}

static inline void smart_str_sets(smart_str *dest, const char *src)
{
	smart_str_setl(dest, src, strlen(src));
}

static inline void attach_url(smart_str *url, smart_str *name, smart_str *val, const char *separator)
{
	if (strchr(url->c, ':')) return;

	if (strchr(url->c, '?'))
		smart_str_appendl(url, separator, 1);
	else
		smart_str_appendl(url, "?", 1);

	smart_str_append(url, name);
	smart_str_appendl(url, "=", 1);
	smart_str_append(url, val);
}

static char *check_tag_arg[] = {
	"a", "href",
	"area", "href",
	"frame", "source",
	"img", "src",
	NULL
};

static inline void tag_arg(url_adapt_state_t *ctx PLS_DC)
{
	char f = 0;
	int i;

	for (i = 0; check_tag_arg[i]; i += 2) {
		if (strcasecmp(ctx->tag.c, check_tag_arg[i]) == 0
				&& strcasecmp(ctx->arg.c, check_tag_arg[i + 1]) == 0) {
			f = 1;
			break;
		}
	}

	smart_str_appends(&ctx->result, "\"");
	if (f) {
		attach_url(&ctx->para, &ctx->name, &ctx->value, PG(arg_separator));
	}
	smart_str_append(&ctx->result, &ctx->para);
	smart_str_appends(&ctx->result, "\"");
}

/*!re2c
all = [\000-\377];
*/

#define NEXT continue

#define COPY_ALL \
	smart_str_appendl(&ctx->result, start, YYCURSOR - start); \
	start = NULL; \
	NEXT
	
#define YYFILL(n) goto finish
#define YYCTYPE unsigned char
#define YYLIMIT (ctx->work.c+ctx->work.len)
#define YYCURSOR cursor
#define YYMARKER marker

#define HANDLE_FORM \
	if (strcasecmp(ctx->tag.c, "form") == 0) { \
		smart_str_appends(&ctx->result, "><INPUT TYPE=HIDDEN NAME=\""); \
		smart_str_append(&ctx->result, &ctx->name); \
		smart_str_appends(&ctx->result, "\" VALUE=\""); \
		smart_str_append(&ctx->result, &ctx->value); \
		smart_str_appends(&ctx->result, "\""); \
	}

#define GO(n) ctx->state = n

enum {
	STATE_PLAIN,
	STATE_TAG,
	STATE_NEXT_ARG,
	STATE_ARG,
	STATE_PARA
};

static void mainloop(url_adapt_state_t *ctx, smart_str *newstuff)
{
	char *para_start, *arg_start, *tag_start;
	char *start = NULL;
	char *cursor, *marker;
	PLS_FETCH();

	arg_start = para_start = tag_start = NULL;
	smart_str_append(&ctx->work, newstuff);
	smart_str_free(&ctx->result);

	cursor = ctx->work.c;
	
	while (YYCURSOR < YYLIMIT) {
		start = YYCURSOR;

#ifdef SCANNER_DEBUG
	printf("state %d:%s'\n", ctx->state, YYCURSOR);
#endif
		switch (ctx->state) {
			
		case STATE_PLAIN:
/*!re2c
  "<" 			{ tag_start = YYCURSOR; GO(STATE_TAG); COPY_ALL;}
  (all\[<])+ 	{ COPY_ALL; }
*/
  			break;
		
		case STATE_TAG:
/*!re2c
  [a-zA-Z]+ [ >]	{ 
  						YYCURSOR--; 
  						arg_start = YYCURSOR;
						smart_str_copyl(&ctx->tag, start, YYCURSOR - start); 
#ifdef SCANNER_DEBUG
						printf("TAG(%s)\n", ctx->tag.c);
#endif
						GO(STATE_NEXT_ARG);
						COPY_ALL; 
				}
  all			{ 
  						YYCURSOR--; 
						GO(STATE_PLAIN);
						tag_start = NULL; 
						NEXT;
				}
*/
			break;

		case STATE_NEXT_ARG:
/*!re2c
  [ ]+			{
  						GO(STATE_ARG);
						NEXT; 
				}
  ">"			{
  						HANDLE_FORM;
						GO(STATE_PLAIN);
						tag_start = NULL;
						COPY_ALL;
				}
*/
			break;

		case STATE_ARG:
				smart_str_appendl(&ctx->result, " ", 1);
/*!re2c
  [a-zA-Z]+ [ ]* "=" [ ]*  		{
  						char *p;

						for (p = start; isalpha(*p); p++);
						smart_str_setl(&ctx->arg, start, p - start);
#ifdef SCANNER_DEBUG
						printf("ARG(%s)\n", ctx->arg.c);
#endif
						para_start = YYCURSOR;
						ctx->state++;
						COPY_ALL;
				}
  (all\[ =>])*		{
  						arg_start = YYCURSOR;
						ctx->state--;
						COPY_ALL;
				}
*/
			break;

		case STATE_PARA:
/*!re2c
  ["] (all\[^>"])* ["] [ >]		{
  						YYCURSOR--;
						para_start = NULL;
						smart_str_setl(&ctx->para, start + 1, YYCURSOR - start - 2);
#ifdef SCANNER_DEBUG
						printf("PARA(%s)\n", ctx->para.c);
#endif
						tag_arg(ctx PLS_CC);
						arg_start = YYCURSOR;
						GO(STATE_NEXT_ARG);
						NEXT;
				}
  (all\[^> ])+ [ >]		{
  						YYCURSOR--;
						para_start = NULL;
						smart_str_setl(&ctx->para, start, YYCURSOR - start);
#ifdef SCANNER_DEBUG
						printf("PARA(%s)\n", ctx->para.c);
#endif
						tag_arg(ctx PLS_CC);
						arg_start = YYCURSOR;
						GO(STATE_NEXT_ARG);
						NEXT;
				}
  all			{
  						YYCURSOR--;
						ctx->state = 2;
						NEXT;
				}
*/
			break;
		}
	}

#define PRESERVE(s) \
	size_t n = ctx->work.len - (s - ctx->work.c); \
	memmove(ctx->work.c, s, n + 1); \
	ctx->work.len = n
	
finish:
	if (ctx->state >= 2) {
		if (para_start) {
			PRESERVE(para_start);
			ctx->state = 4;
		} else {
			if (arg_start) { PRESERVE(arg_start); }
			ctx->state = 2;
		}
	} else if (tag_start) {
		PRESERVE(tag_start);
		ctx->state = 1;
	} else {
		ctx->state = 0;
		if (start) smart_str_appendl(&ctx->result, start, YYCURSOR - start);
		smart_str_free(&ctx->work);
	}

#ifdef SCANNER_DEBUG 
	if (ctx->work.c) {
		printf("PRESERVING %s'\n", ctx->work.c);
	}
#endif
}

char *url_adapt_ext(const char *src, size_t srclen, const char *name, const char *value, size_t *newlen)
{
	smart_str str = {0,0};
	char *ret;
	BLS_FETCH();

	smart_str_sets(&BG(url_adapt_state).name, name);
	smart_str_sets(&BG(url_adapt_state).value, value);
	str.c = (char *) src;
	str.len = srclen;
	mainloop(&BG(url_adapt_state), &str);

	*newlen = BG(url_adapt_state).result.len;

#ifdef SCANNER_DEBUG
	printf("(%d)NEW(%d): %s'\n", srclen, BG(url_adapt_state).result.len, BG(url_adapt_state).result.c);
#endif

#if 1
	ret = BG(url_adapt_state).result.c;
	BG(url_adapt_state).result.c = NULL;
	return ret;
#else
	return strdup(BG(url_adapt_state).result.c);
#endif
}

PHP_RINIT_FUNCTION(url_scanner)
{
	BLS_FETCH();

	memset(&BG(url_adapt_state), 0, sizeof(BG(url_adapt_state)));

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(url_scanner)
{
	BLS_FETCH();

	smart_str_free(&BG(url_adapt_state).result);
	smart_str_free(&BG(url_adapt_state).work);

	return SUCCESS;
}

#endif
