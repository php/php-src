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
	zend_string *quoted;	/* quoted value */
	int bindno;
	struct placeholder *next;
};

static void free_param_name(zval *el) {
	zend_string_release(Z_PTR_P(el));
}

PDO_API int pdo_parse_params(pdo_stmt_t *stmt, zend_string *inquery, zend_string **outquery)
{
	Scanner s;
	char *newbuffer;
	ptrdiff_t t;
	uint32_t bindno = 0;
	int ret = 0, escapes = 0;
	size_t newbuffer_len;
	HashTable *params;
	struct pdo_bound_param_data *param;
	int query_type = PDO_PLACEHOLDER_NONE;
	struct placeholder *placeholders = NULL, *placetail = NULL, *plc = NULL;

	s.cur = ZSTR_VAL(inquery);
	s.end = s.cur + ZSTR_LEN(inquery) + 1;

	/* phase 1: look for args */
	while((t = scan(&s)) != PDO_PARSER_EOI) {
		if (t == PDO_PARSER_BIND || t == PDO_PARSER_BIND_POS || t == PDO_PARSER_ESCAPED_QUESTION) {
			if (t == PDO_PARSER_ESCAPED_QUESTION && stmt->supports_placeholders == PDO_PLACEHOLDER_POSITIONAL) {
				/* escaped question marks unsupported, treat as text */
				continue;
			}

			if (t == PDO_PARSER_BIND) {
				ptrdiff_t len = s.cur - s.tok;
				if ((ZSTR_VAL(inquery) < (s.cur - len)) && isalnum(*(s.cur - len - 1))) {
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
				plc->quoted = ZSTR_CHAR('?');
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
			newbuffer_len = ZSTR_LEN(inquery);
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

		newbuffer_len = ZSTR_LEN(inquery);

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

						plc->quoted = stmt->dbh->methods->quoter(stmt->dbh, buf, param->param_type);

						if (buf) {
							zend_string_release_ex(buf, 0);
						}
						if (plc->quoted == NULL) {
							/* bork */
							ret = -1;
							strncpy(stmt->error_code, stmt->dbh->error_code, 6);
							goto clean_up;
						}

					} else {
						pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource");
						ret = -1;
						goto clean_up;
					}
				} else {
					enum pdo_param_type param_type = param->param_type;
					zend_string *buf = NULL;

					/* assume all types are nullable */
					if (Z_TYPE_P(parameter) == IS_NULL) {
						param_type = PDO_PARAM_NULL;
					}

					switch (param_type) {
						case PDO_PARAM_BOOL:
							plc->quoted = zend_is_true(parameter) ? ZSTR_CHAR('1') : ZSTR_CHAR('0');
							break;

						case PDO_PARAM_INT:
							plc->quoted = zend_long_to_str(zval_get_long(parameter));
							break;

						case PDO_PARAM_NULL:
							plc->quoted = ZSTR_KNOWN(ZEND_STR_NULL);
							break;

						default: {
							buf = zval_try_get_string(parameter);
							/* parameter does not have a string representation, buf == NULL */
							if (EG(exception)) {
								/* bork */
								ret = -1;
								strncpy(stmt->error_code, stmt->dbh->error_code, 6);
								goto clean_up;
							}

							plc->quoted = stmt->dbh->methods->quoter(stmt->dbh, buf, param_type);
						}
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
				plc->quoted = zend_string_copy(Z_STR_P(parameter));
			}
			newbuffer_len += ZSTR_LEN(plc->quoted);
		}

rewrite:
		/* allocate output buffer */
		*outquery = zend_string_alloc(newbuffer_len, 0);
		newbuffer = ZSTR_VAL(*outquery);

		/* and build the query */
		const char *ptr = ZSTR_VAL(inquery);
		plc = placeholders;

		do {
			t = plc->pos - ptr;
			if (t) {
				memcpy(newbuffer, ptr, t);
				newbuffer += t;
			}
			if (plc->quoted) {
				memcpy(newbuffer, ZSTR_VAL(plc->quoted), ZSTR_LEN(plc->quoted));
				newbuffer += ZSTR_LEN(plc->quoted);
			} else {
				memcpy(newbuffer, plc->pos, plc->len);
				newbuffer += plc->len;
			}
			ptr = plc->pos + plc->len;

			plc = plc->next;
		} while (plc);

		t = ZSTR_VAL(inquery) + ZSTR_LEN(inquery) - ptr;
		if (t) {
			memcpy(newbuffer, ptr, t);
			newbuffer += t;
		}
		*newbuffer = '\0';
		ZSTR_LEN(*outquery) = newbuffer - ZSTR_VAL(*outquery);

		ret = 1;
		goto clean_up;

	} else if (query_type == PDO_PLACEHOLDER_POSITIONAL) {
		/* rewrite ? to :pdoX */
		const char *tmpl = stmt->named_rewrite_template ? stmt->named_rewrite_template : ":pdo%d";
		int bind_no = 1;

		newbuffer_len = ZSTR_LEN(inquery);

		if (stmt->bound_param_map == NULL) {
			ALLOC_HASHTABLE(stmt->bound_param_map);
			zend_hash_init(stmt->bound_param_map, 13, NULL, free_param_name, 0);
		}

		for (plc = placeholders; plc; plc = plc->next) {
			int skip_map = 0;
			zend_string *p;
			zend_string *idxbuf;

			if (plc->bindno == PDO_PARSER_BINDNO_ESCAPED_CHAR) {
				continue;
			}

			zend_string *name = zend_string_init(plc->pos, plc->len, 0);

			/* check if bound parameter is already available */
			if (zend_string_equals_literal(name, "?") || (p = zend_hash_find_ptr(stmt->bound_param_map, name)) == NULL) {
				idxbuf = zend_strpprintf(0, tmpl, bind_no++);
			} else {
				idxbuf = zend_string_copy(p);
				skip_map = 1;
			}

			plc->quoted = idxbuf;
			newbuffer_len += ZSTR_LEN(plc->quoted);

			if (!skip_map && stmt->named_rewrite_template) {
				/* create a mapping */
				zend_hash_update_ptr(stmt->bound_param_map, name, zend_string_copy(plc->quoted));
			}

			/* map number to name */
			zend_hash_index_update_ptr(stmt->bound_param_map, plc->bindno, zend_string_copy(plc->quoted));

			zend_string_release(name);
		}

		goto rewrite;

	} else {
		/* rewrite :name to ? */

		newbuffer_len = ZSTR_LEN(inquery);

		if (stmt->bound_param_map == NULL) {
			ALLOC_HASHTABLE(stmt->bound_param_map);
			zend_hash_init(stmt->bound_param_map, 13, NULL, free_param_name, 0);
		}

		for (plc = placeholders; plc; plc = plc->next) {
			zend_string *name = zend_string_init(plc->pos, plc->len, 0);
			zend_hash_index_update_ptr(stmt->bound_param_map, plc->bindno, name);
			plc->quoted = ZSTR_CHAR('?');
			newbuffer_len -= plc->len - 1;
		}

		goto rewrite;
	}

clean_up:

	while (placeholders) {
		plc = placeholders;
		placeholders = plc->next;
		if (plc->quoted) {
			zend_string_release_ex(plc->quoted, 0);
		}
		efree(plc);
	}

	return ret;
}
