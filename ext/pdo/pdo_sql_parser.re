/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: George Schlossnagle <george@omniti.com>                      |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"

#define PDO_PARSER_TEXT 1
#define PDO_PARSER_BIND 2
#define PDO_PARSER_BIND_POS 3
#define PDO_PARSER_ESCAPED_QUESTION 4
#define PDO_PARSER_EOI 5

#define PDO_PARSER_BINDNO_ESCAPED_CHAR -1

#define RET(i) {s->cur = cursor; return i; }
#define SKIP_ONE(i) {s->cur = s->tok + 1; return i; }

#define YYCTYPE         unsigned char
#define YYCURSOR        cursor
#define YYLIMIT         s->end
#define YYMARKER        s->ptr
#define YYFILL(n)		{ RET(PDO_PARSER_EOI); }

typedef struct Scanner {
	const char *ptr, *cur, *tok, *end;
} Scanner;

static int scan(Scanner *s)
{
	const char *cursor = s->cur;

	s->tok = cursor;
	/*!re2c
	BINDCHR		= [:][a-zA-Z0-9_]+;
	QUESTION	= [?];
	ESCQUESTION	= [?][?];
	COMMENTS	= ("/*"([^*]+|[*]+[^/*])*[*]*"*/"|"--"[^\r\n]*);
	SPECIALS	= [:?"'-/];
	MULTICHAR	= [:]{2,};
	ANYNOEOF	= [\001-\377];
	*/

	/*!re2c
		(["](([\\]ANYNOEOF)|ANYNOEOF\["\\])*["]) { RET(PDO_PARSER_TEXT); }
		(['](([\\]ANYNOEOF)|ANYNOEOF\['\\])*[']) { RET(PDO_PARSER_TEXT); }
		MULTICHAR								{ RET(PDO_PARSER_TEXT); }
		ESCQUESTION								{ RET(PDO_PARSER_ESCAPED_QUESTION); }
		BINDCHR									{ RET(PDO_PARSER_BIND); }
		QUESTION								{ RET(PDO_PARSER_BIND_POS); }
		SPECIALS								{ SKIP_ONE(PDO_PARSER_TEXT); }
		COMMENTS								{ RET(PDO_PARSER_TEXT); }
		(ANYNOEOF\SPECIALS)+ 					{ RET(PDO_PARSER_TEXT); }
	*/
}

struct placeholder {
	const char *pos;
	size_t len;
	size_t qlen;	/* quoted length of value */
	char *quoted;	/* quoted value */
	int freeq;
	int bindno;
	struct placeholder *next;
};

static void free_param_name(zval *el) {
	efree(Z_PTR_P(el));
}

PDO_API int pdo_parse_params(pdo_stmt_t *stmt, const char *inquery, size_t inquery_len,
	char **outquery, size_t *outquery_len)
{
	Scanner s;
	const char *ptr;
	char *newbuffer;
	ptrdiff_t t;
	uint32_t bindno = 0;
	int ret = 0, escapes = 0;
	size_t newbuffer_len;
	HashTable *params;
	struct pdo_bound_param_data *param;
	int query_type = PDO_PLACEHOLDER_NONE;
	struct placeholder *placeholders = NULL, *placetail = NULL, *plc = NULL;

	ptr = *outquery;
	s.cur = inquery;
	s.end = inquery + inquery_len + 1;

	/* phase 1: look for args */
	while((t = scan(&s)) != PDO_PARSER_EOI) {
		if (t == PDO_PARSER_BIND || t == PDO_PARSER_BIND_POS || t == PDO_PARSER_ESCAPED_QUESTION) {
			if (t == PDO_PARSER_ESCAPED_QUESTION && stmt->supports_placeholders == PDO_PLACEHOLDER_POSITIONAL) {
				/* escaped question marks unsupported, treat as text */
				continue;
			}

			if (t == PDO_PARSER_BIND) {
				ptrdiff_t len = s.cur - s.tok;
				if ((inquery < (s.cur - len)) && isalnum(*(s.cur - len - 1))) {
					continue;
				}
				query_type |= PDO_PLACEHOLDER_NAMED;
			} else if (t == PDO_PARSER_BIND_POS) {
				query_type |= PDO_PLACEHOLDER_POSITIONAL;
			}

			plc = emalloc(sizeof(*plc));
			memset(plc, 0, sizeof(*plc));
			plc->next = NULL;
			plc->pos = s.tok;
			plc->len = s.cur - s.tok;

			if (t == PDO_PARSER_ESCAPED_QUESTION) {
				plc->bindno = PDO_PARSER_BINDNO_ESCAPED_CHAR;
				plc->quoted = "?";
				plc->qlen = 1;
				plc->freeq = 0;
				escapes++;
			} else {
				plc->bindno = bindno++;
			}

			if (placetail) {
				placetail->next = plc;
			} else {
				placeholders = plc;
			}
			placetail = plc;
		}
	}

	/* did the query make sense to me? */
	if (query_type == (PDO_PLACEHOLDER_NAMED|PDO_PLACEHOLDER_POSITIONAL)) {
		/* they mixed both types; punt */
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "mixed named and positional parameters");
		ret = -1;
		goto clean_up;
	}

	params = stmt->bound_params;
	if (stmt->supports_placeholders == PDO_PLACEHOLDER_NONE && params && bindno != zend_hash_num_elements(params)) {
		/* extra bit of validation for instances when same params are bound more than once */
		if (query_type != PDO_PLACEHOLDER_POSITIONAL && bindno > zend_hash_num_elements(params)) {
			int ok = 1;
			for (plc = placeholders; plc; plc = plc->next) {
				if ((param = zend_hash_str_find_ptr(params, plc->pos, plc->len)) == NULL) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				goto safe;
			}
		}
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "number of bound variables does not match number of tokens");
		ret = -1;
		goto clean_up;
	}

	if (!placeholders) {
		/* nothing to do; good! */
		return 0;
	}

	if (stmt->supports_placeholders == query_type && !stmt->named_rewrite_template) {
		/* query matches native syntax */
		if (escapes) {
			newbuffer_len = inquery_len;
			goto rewrite;
		}

		ret = 0;
		goto clean_up;
	}

	if (query_type == PDO_PLACEHOLDER_NAMED && stmt->named_rewrite_template) {
		/* magic/hack.
		 * We we pretend that the query was positional even if
		 * it was named so that we fall into the
		 * named rewrite case below.  Not too pretty,
		 * but it works. */
		query_type = PDO_PLACEHOLDER_POSITIONAL;
	}

safe:
	/* what are we going to do ? */
	if (stmt->supports_placeholders == PDO_PLACEHOLDER_NONE) {
		/* query generation */

		newbuffer_len = inquery_len;

		/* let's quote all the values */
		for (plc = placeholders; plc && params; plc = plc->next) {
			if (plc->bindno == PDO_PARSER_BINDNO_ESCAPED_CHAR) {
				/* escaped character */
				continue;
			}

			if (query_type == PDO_PLACEHOLDER_NONE) {
				continue;
			}

			if (query_type == PDO_PLACEHOLDER_POSITIONAL) {
				param = zend_hash_index_find_ptr(params, plc->bindno);
			} else {
				param = zend_hash_str_find_ptr(params, plc->pos, plc->len);
			}
			if (param == NULL) {
				/* parameter was not defined */
				ret = -1;
				pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "parameter was not defined");
				goto clean_up;
			}
			if (stmt->dbh->methods->quoter) {
				zval *parameter;
				if (Z_ISREF(param->parameter)) {
					parameter = Z_REFVAL(param->parameter);
				} else {
					parameter = &param->parameter;
				}
				if (param->param_type == PDO_PARAM_LOB && Z_TYPE_P(parameter) == IS_RESOURCE) {
					php_stream *stm;

					php_stream_from_zval_no_verify(stm, parameter);
					if (stm) {
						zend_string *buf;

						buf = php_stream_copy_to_mem(stm, PHP_STREAM_COPY_ALL, 0);
						if (!buf) {
							buf = ZSTR_EMPTY_ALLOC();
						}
						if (!stmt->dbh->methods->quoter(stmt->dbh, ZSTR_VAL(buf), ZSTR_LEN(buf), &plc->quoted, &plc->qlen,
								param->param_type)) {
							/* bork */
							ret = -1;
							strncpy(stmt->error_code, stmt->dbh->error_code, 6);
							if (buf) {
								zend_string_release_ex(buf, 0);
							}
							goto clean_up;
						}
						if (buf) {
							zend_string_release_ex(buf, 0);
						}
					} else {
						pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource");
						ret = -1;
						goto clean_up;
					}
					plc->freeq = 1;
				} else {
					enum pdo_param_type param_type = param->param_type;
					zend_string *buf = NULL;

					/* assume all types are nullable */
					if (Z_TYPE_P(parameter) == IS_NULL) {
						param_type = PDO_PARAM_NULL;
					}

					switch (param_type) {
						case PDO_PARAM_BOOL:
							plc->quoted = zend_is_true(parameter) ? "1" : "0";
							plc->qlen = sizeof("1")-1;
							plc->freeq = 0;
							break;

						case PDO_PARAM_INT:
							buf = zend_long_to_str(zval_get_long(parameter));

							plc->qlen = ZSTR_LEN(buf);
							plc->quoted = estrdup(ZSTR_VAL(buf));
							plc->freeq = 1;
							break;

						case PDO_PARAM_NULL:
							plc->quoted = "NULL";
							plc->qlen = sizeof("NULL")-1;
							plc->freeq = 0;
							break;

						default:
							buf = zval_get_string(parameter);
							if (EG(exception) ||
								!stmt->dbh->methods->quoter(stmt->dbh, ZSTR_VAL(buf),
									ZSTR_LEN(buf), &plc->quoted, &plc->qlen,
									param_type)) {
								/* bork */
								ret = -1;
								strncpy(stmt->error_code, stmt->dbh->error_code, 6);
								if (buf) {
									zend_string_release_ex(buf, 0);
								}
								goto clean_up;
							}
							plc->freeq = 1;
					}

					if (buf) {
						zend_string_release_ex(buf, 0);
					}
				}
			} else {
				zval *parameter;
				if (Z_ISREF(param->parameter)) {
					parameter = Z_REFVAL(param->parameter);
				} else {
					parameter = &param->parameter;
				}
				plc->quoted = Z_STRVAL_P(parameter);
				plc->qlen = Z_STRLEN_P(parameter);
			}
			newbuffer_len += plc->qlen;
		}

rewrite:
		/* allocate output buffer */
		newbuffer = emalloc(newbuffer_len + 1);
		*outquery = newbuffer;

		/* and build the query */
		plc = placeholders;
		ptr = inquery;

		do {
			t = plc->pos - ptr;
			if (t) {
				memcpy(newbuffer, ptr, t);
				newbuffer += t;
			}
			if (plc->quoted) {
				memcpy(newbuffer, plc->quoted, plc->qlen);
				newbuffer += plc->qlen;
			} else {
				memcpy(newbuffer, plc->pos, plc->len);
				newbuffer += plc->len;
			}
			ptr = plc->pos + plc->len;

			plc = plc->next;
		} while (plc);

		t = (inquery + inquery_len) - ptr;
		if (t) {
			memcpy(newbuffer, ptr, t);
			newbuffer += t;
		}
		*newbuffer = '\0';
		*outquery_len = newbuffer - *outquery;

		ret = 1;
		goto clean_up;

	} else if (query_type == PDO_PLACEHOLDER_POSITIONAL) {
		/* rewrite ? to :pdoX */
		char *name, *idxbuf;
		const char *tmpl = stmt->named_rewrite_template ? stmt->named_rewrite_template : ":pdo%d";
		int bind_no = 1;

		newbuffer_len = inquery_len;

		if (stmt->bound_param_map == NULL) {
			ALLOC_HASHTABLE(stmt->bound_param_map);
			zend_hash_init(stmt->bound_param_map, 13, NULL, free_param_name, 0);
		}

		for (plc = placeholders; plc; plc = plc->next) {
			int skip_map = 0;
			char *p;

			if (plc->bindno == PDO_PARSER_BINDNO_ESCAPED_CHAR) {
				continue;
			}

			name = estrndup(plc->pos, plc->len);

			/* check if bound parameter is already available */
			if (!strcmp(name, "?") || (p = zend_hash_str_find_ptr(stmt->bound_param_map, name, plc->len)) == NULL) {
				spprintf(&idxbuf, 0, tmpl, bind_no++);
			} else {
				idxbuf = estrdup(p);
				skip_map = 1;
			}

			plc->quoted = idxbuf;
			plc->qlen = strlen(plc->quoted);
			plc->freeq = 1;
			newbuffer_len += plc->qlen;

			if (!skip_map && stmt->named_rewrite_template) {
				/* create a mapping */
				zend_hash_str_update_mem(stmt->bound_param_map, name, plc->len, idxbuf, plc->qlen + 1);
			}

			/* map number to name */
			zend_hash_index_update_mem(stmt->bound_param_map, plc->bindno, idxbuf, plc->qlen + 1);

			efree(name);
		}

		goto rewrite;

	} else {
		/* rewrite :name to ? */

		newbuffer_len = inquery_len;

		if (stmt->bound_param_map == NULL) {
			ALLOC_HASHTABLE(stmt->bound_param_map);
			zend_hash_init(stmt->bound_param_map, 13, NULL, free_param_name, 0);
		}

		for (plc = placeholders; plc; plc = plc->next) {
			char *name;
			name = estrndup(plc->pos, plc->len);
			zend_hash_index_update_mem(stmt->bound_param_map, plc->bindno, name, plc->len + 1);
			efree(name);
			plc->quoted = "?";
			plc->qlen = 1;
			newbuffer_len -= plc->len - 1;
		}

		goto rewrite;
	}

clean_up:

	while (placeholders) {
		plc = placeholders;
		placeholders = plc->next;

		if (plc->freeq) {
			efree(plc->quoted);
		}

		efree(plc);
	}

	return ret;
}
