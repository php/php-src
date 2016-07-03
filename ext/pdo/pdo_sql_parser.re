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
  | Author: George Schlossnagle <george@omniti.com>                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"

#define PDO_PARSER_TEXT 1
#define PDO_PARSER_BIND 2
#define PDO_PARSER_BIND_POS 3
#define PDO_PARSER_EOI 4

#define RET(i) {s->cur = cursor; return i; }
#define SKIP_ONE(i) {s->cur = s->tok + 1; return i; }

#define YYCTYPE         unsigned char
#define YYCURSOR        cursor
#define YYLIMIT         s->end
#define YYMARKER        s->ptr
#define YYFILL(n)		{ RET(PDO_PARSER_EOI); }

typedef struct Scanner {
	char 	*ptr, *cur, *tok, *end;
} Scanner;

static int scan(Scanner *s) 
{
	char *cursor = s->cur;

	s->tok = cursor;
	/*!re2c
	BINDCHR		= [:][a-zA-Z0-9_]+;
	QUESTION	= [?];
	COMMENTS	= ("/*"([^*]+|[*]+[^/*])*[*]*"*/"|"--"[^\r\n]*);
	SPECIALS	= [:?"'-/];
	MULTICHAR	= ([:]{2,}|[?]{2,});
	ANYNOEOF	= [\001-\377];
	*/

	/*!re2c
		(["](([\\]ANYNOEOF)|ANYNOEOF\["\\])*["]) { RET(PDO_PARSER_TEXT); }
		(['](([\\]ANYNOEOF)|ANYNOEOF\['\\])*[']) { RET(PDO_PARSER_TEXT); }
		MULTICHAR								{ RET(PDO_PARSER_TEXT); }
		BINDCHR									{ RET(PDO_PARSER_BIND); }
		QUESTION								{ RET(PDO_PARSER_BIND_POS); }
		SPECIALS								{ SKIP_ONE(PDO_PARSER_TEXT); }
		COMMENTS								{ RET(PDO_PARSER_TEXT); }
		(ANYNOEOF\SPECIALS)+ 					{ RET(PDO_PARSER_TEXT); }
	*/	
}

struct placeholder {
	char *pos;
	int len;
	int bindno;
	int qlen;		/* quoted length of value */
	char *quoted;	/* quoted value */
	int freeq;
	struct placeholder *next;
};

PDO_API int pdo_parse_params(pdo_stmt_t *stmt, char *inquery, int inquery_len, 
	char **outquery, int *outquery_len TSRMLS_DC)
{
	Scanner s;
	char *ptr, *newbuffer;
	int t;
	int bindno = 0;
	int ret = 0;
	int newbuffer_len;
	HashTable *params;
	struct pdo_bound_param_data *param;
	int query_type = PDO_PLACEHOLDER_NONE;
	struct placeholder *placeholders = NULL, *placetail = NULL, *plc = NULL;

	ptr = *outquery;
	s.cur = inquery;
	s.end = inquery + inquery_len + 1;

	/* phase 1: look for args */
	while((t = scan(&s)) != PDO_PARSER_EOI) {
		if (t == PDO_PARSER_BIND || t == PDO_PARSER_BIND_POS) {
			if (t == PDO_PARSER_BIND) {
				int len = s.cur - s.tok;
				if ((inquery < (s.cur - len)) && isalnum(*(s.cur - len - 1))) {
					continue;
				}
				query_type |= PDO_PLACEHOLDER_NAMED;
			} else {
				query_type |= PDO_PLACEHOLDER_POSITIONAL;
			}

			plc = emalloc(sizeof(*plc));
			memset(plc, 0, sizeof(*plc));
			plc->next = NULL;
			plc->pos = s.tok;
			plc->len = s.cur - s.tok;
			plc->bindno = bindno++;

			if (placetail) {
				placetail->next = plc;
			} else {
				placeholders = plc;
			}
			placetail = plc;
		}
	}

	if (bindno == 0) {
		/* nothing to do; good! */
		return 0;
	}

	/* did the query make sense to me? */
	if (query_type == (PDO_PLACEHOLDER_NAMED|PDO_PLACEHOLDER_POSITIONAL)) {
		/* they mixed both types; punt */
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "mixed named and positional parameters" TSRMLS_CC);
		ret = -1;
		goto clean_up;
	}

	if (stmt->supports_placeholders == query_type && !stmt->named_rewrite_template) {
		/* query matches native syntax */
		ret = 0;
		goto clean_up;
	}

	if (stmt->named_rewrite_template) {
		/* magic/hack.
		 * We we pretend that the query was positional even if
		 * it was named so that we fall into the
		 * named rewrite case below.  Not too pretty,
		 * but it works. */
		query_type = PDO_PLACEHOLDER_POSITIONAL;
	}
	
	params = stmt->bound_params;
	
	/* Do we have placeholders but no bound params */
	if (bindno && !params && stmt->supports_placeholders == PDO_PLACEHOLDER_NONE) {
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "no parameters were bound" TSRMLS_CC);
		ret = -1;
		goto clean_up;
	}

	if (params && bindno != zend_hash_num_elements(params) && stmt->supports_placeholders == PDO_PLACEHOLDER_NONE) {
		/* extra bit of validation for instances when same params are bound more then once */
		if (query_type != PDO_PLACEHOLDER_POSITIONAL && bindno > zend_hash_num_elements(params)) {
			int ok = 1;
			for (plc = placeholders; plc; plc = plc->next) {
				if (zend_hash_find(params, plc->pos, plc->len, (void**) &param) == FAILURE) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				goto safe;
			}
		}
		pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "number of bound variables does not match number of tokens" TSRMLS_CC);
		ret = -1;
		goto clean_up;
	}
safe:
	/* what are we going to do ? */
	if (stmt->supports_placeholders == PDO_PLACEHOLDER_NONE) {
		/* query generation */

		newbuffer_len = inquery_len;

		/* let's quote all the values */	
		for (plc = placeholders; plc; plc = plc->next) {
			if (query_type == PDO_PLACEHOLDER_POSITIONAL) {
				ret = zend_hash_index_find(params, plc->bindno, (void**) &param);
			} else {
				ret = zend_hash_find(params, plc->pos, plc->len, (void**) &param);
			}
			if (ret == FAILURE) {
				/* parameter was not defined */
				ret = -1;
				pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "parameter was not defined" TSRMLS_CC);
				goto clean_up;
			}
			if (stmt->dbh->methods->quoter) {
				if (param->param_type == PDO_PARAM_LOB && Z_TYPE_P(param->parameter) == IS_RESOURCE) {
					php_stream *stm;

					php_stream_from_zval_no_verify(stm, &param->parameter);
					if (stm) {
						size_t len;
						char *buf = NULL;
					
						len = php_stream_copy_to_mem(stm, &buf, PHP_STREAM_COPY_ALL, 0);
						if (!stmt->dbh->methods->quoter(stmt->dbh, buf, len, &plc->quoted, &plc->qlen,
								param->param_type TSRMLS_CC)) {
							/* bork */
							ret = -1;
							strncpy(stmt->error_code, stmt->dbh->error_code, 6);
							if (buf) {
								efree(buf);
							}
							goto clean_up;
						}
						if (buf) {
							efree(buf);
						}
					} else {
						pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource" TSRMLS_CC);
						ret = -1;
						goto clean_up;
					}
					plc->freeq = 1;
				} else {
					zval tmp_param = *param->parameter;
					zval_copy_ctor(&tmp_param);
					switch (Z_TYPE(tmp_param)) {
						case IS_NULL:
							plc->quoted = "NULL";
							plc->qlen = sizeof("NULL")-1;
							plc->freeq = 0;
							break;

						case IS_BOOL:
							convert_to_long(&tmp_param);
							/* fall through */
						case IS_LONG:
						case IS_DOUBLE:
							convert_to_string(&tmp_param);
							plc->qlen = Z_STRLEN(tmp_param);
							plc->quoted = estrdup(Z_STRVAL(tmp_param));
							plc->freeq = 1;
							break;

						default:
							convert_to_string(&tmp_param);
							if (!stmt->dbh->methods->quoter(stmt->dbh, Z_STRVAL(tmp_param),
									Z_STRLEN(tmp_param), &plc->quoted, &plc->qlen,
									param->param_type TSRMLS_CC)) {
								/* bork */
								ret = -1;
								strncpy(stmt->error_code, stmt->dbh->error_code, 6);
								goto clean_up;
							}
							plc->freeq = 1;
					}
					zval_dtor(&tmp_param);
				}
			} else {
				plc->quoted = Z_STRVAL_P(param->parameter);
				plc->qlen = Z_STRLEN_P(param->parameter);
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
			memcpy(newbuffer, plc->quoted, plc->qlen);
			newbuffer += plc->qlen;
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
			zend_hash_init(stmt->bound_param_map, 13, NULL, NULL, 0);
		}

		for (plc = placeholders; plc; plc = plc->next) {
			int skip_map = 0;
			char *p;
			name = estrndup(plc->pos, plc->len);

			/* check if bound parameter is already available */
			if (!strcmp(name, "?") || zend_hash_find(stmt->bound_param_map, name, plc->len + 1, (void**) &p) == FAILURE) {
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
				zend_hash_update(stmt->bound_param_map, name, plc->len + 1, idxbuf, plc->qlen + 1, NULL);
			}

			/* map number to name */
			zend_hash_index_update(stmt->bound_param_map, plc->bindno, idxbuf, plc->qlen + 1, NULL);
			
			efree(name);
		}
				
		goto rewrite;

	} else {
		/* rewrite :name to ? */
		
		newbuffer_len = inquery_len;
	
		if (stmt->bound_param_map == NULL) {
			ALLOC_HASHTABLE(stmt->bound_param_map);
			zend_hash_init(stmt->bound_param_map, 13, NULL, NULL, 0);
		}
		
		for (plc = placeholders; plc; plc = plc->next) {
			char *name;
			
			name = estrndup(plc->pos, plc->len);
			zend_hash_index_update(stmt->bound_param_map, plc->bindno, name, plc->len + 1, NULL);
			efree(name);
			plc->quoted = "?";
			plc->qlen = 1;
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

#if 0
int old_pdo_parse_params(pdo_stmt_t *stmt, char *inquery, int inquery_len, char **outquery, 
		int *outquery_len TSRMLS_DC)
{
	Scanner s;
	char *ptr;
	int t;
	int bindno = 0;
	int newbuffer_len;
	int padding;
	HashTable *params = stmt->bound_params;
	struct pdo_bound_param_data *param;
	/* allocate buffer for query with expanded binds, ptr is our writing pointer */
	newbuffer_len = inquery_len;

	/* calculate the possible padding factor due to quoting */
	if(stmt->dbh->max_escaped_char_length) {
		padding = stmt->dbh->max_escaped_char_length;
	} else {
		padding = 3;
	}
	if(params) {
		zend_hash_internal_pointer_reset(params);
		while (SUCCESS == zend_hash_get_current_data(params, (void**)&param)) {
			if(param->parameter) {
				convert_to_string(param->parameter);
				/* accommodate a string that needs to be fully quoted
                   bind placeholders are at least 2 characters, so
                   the accommodate their own "'s
                */
				newbuffer_len += padding * Z_STRLEN_P(param->parameter);
			}
			zend_hash_move_forward(params);
		}
	}
	*outquery = (char *) emalloc(newbuffer_len + 1);
	*outquery_len = 0;

	ptr = *outquery;
	s.cur = inquery;
	while((t = scan(&s)) != PDO_PARSER_EOI) {
		if(t == PDO_PARSER_TEXT) {
			memcpy(ptr, s.tok, s.cur - s.tok);
			ptr += (s.cur - s.tok);
			*outquery_len += (s.cur - s.tok);
		}
		else if(t == PDO_PARSER_BIND) {
			if(!params) { 
				/* error */
				efree(*outquery);
				*outquery = NULL;
				return (int) (s.cur - inquery);
			}
			/* lookup bind first via hash and then index */
			/* stupid keys need to be null-terminated, even though we know their length */
			if((SUCCESS == zend_hash_find(params, s.tok, s.cur-s.tok,(void **)&param))  
			    ||
			   (SUCCESS == zend_hash_index_find(params, bindno, (void **)&param))) 
			{
				char *quotedstr;
				int quotedstrlen;
				/* restore the in-string key, doesn't need null-termination here */
				/* currently everything is a string here */
				
				/* quote the bind value if necessary */
				if(stmt->dbh->methods->quoter(stmt->dbh, Z_STRVAL_P(param->parameter), 
					Z_STRLEN_P(param->parameter), &quotedstr, &quotedstrlen TSRMLS_CC))
				{
					memcpy(ptr, quotedstr, quotedstrlen);
					ptr += quotedstrlen;
					*outquery_len += quotedstrlen;
					efree(quotedstr);
				} else {
					memcpy(ptr, Z_STRVAL_P(param->parameter), Z_STRLEN_P(param->parameter));
					ptr += Z_STRLEN_P(param->parameter);
					*outquery_len += (Z_STRLEN_P(param->parameter));
				}
			}
			else {
				/* error and cleanup */
				efree(*outquery);
				*outquery = NULL;
				return (int) (s.cur - inquery);
			}
			bindno++;
		}
		else if(t == PDO_PARSER_BIND_POS) {
			if(!params) { 
				/* error */
				efree(*outquery);
				*outquery = NULL;
				return (int) (s.cur - inquery);
			}
			/* lookup bind by index */
			if(SUCCESS == zend_hash_index_find(params, bindno, (void **)&param)) 
			{
				char *quotedstr;
				int quotedstrlen;
				/* currently everything is a string here */
				
				/* quote the bind value if necessary */
				if(stmt->dbh->methods->quoter(stmt->dbh, Z_STRVAL_P(param->parameter), 
					Z_STRLEN_P(param->parameter), &quotedstr, &quotedstrlen TSRMLS_CC))
				{
					memcpy(ptr, quotedstr, quotedstrlen);
					ptr += quotedstrlen;
					*outquery_len += quotedstrlen;
					efree(quotedstr);
				} else {
					memcpy(ptr, Z_STRVAL_P(param->parameter), Z_STRLEN_P(param->parameter));
					ptr += Z_STRLEN_P(param->parameter);
					*outquery_len += (Z_STRLEN_P(param->parameter));
				}
			}
			else {
				/* error and cleanup */
				efree(*outquery);
				*outquery = NULL;
				return (int) (s.cur - inquery);
			}
			bindno++;
		}
	}	
	*ptr = '\0';
	return 0;
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker ft=c
 * vim<600: noet sw=4 ts=4
 */
