/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id: */

#include <stdio.h>
#include "php.h"
#include "php3_standard.h"
#include "php_globals.h"
#include "SAPI.h"

#include "zend_globals.h"

/*
 * php3_getpost()
 *
 * This reads the post form data into a string.
 * Remember to free this pointer when done with it.
 */
#if 0
static char *php3_getpost(pval *http_post_vars PLS_DC)
{
	char *buf = NULL;
	const char *ctype;
#if MODULE_MAGIC_NUMBER > 19961007
	char argsbuffer[HUGE_STRING_LEN];
#else
	int bytes;
#endif
	int length, cnt;
	int file_upload = 0;
	char *mb;
	char boundary[100];
	SLS_FETCH();
	
	ctype = request_info.content_type;
	if (!ctype) {
		php_error(E_WARNING, "POST Error: content-type missing");
		return NULL;
	}
	if (strncasecmp(ctype, "application/x-www-form-urlencoded", 33) && strncasecmp(ctype, "multipart/form-data", 19)
#if HAVE_FDFLIB
 && strncasecmp(ctype, "application/vnd.fdf", 19)
#endif
      ) {
		php_error(E_WARNING, "Unsupported content-type: %s", ctype);
		return NULL;
	}
	if (!strncasecmp(ctype, "multipart/form-data", 19)) {
		file_upload = 1;
		mb = strchr(ctype, '=');
		if (mb) {
			strncpy(boundary, mb + 1, sizeof(boundary));
		} else {
			php_error(E_WARNING, "File Upload Error: No MIME boundary found");
			php_error(E_WARNING, "There should have been a \"boundary=something\" in the Content-Type string");
			php_error(E_WARNING, "The Content-Type string was: \"%s\"", ctype);
			return NULL;
		}
	}
	length = request_info.content_length;
	cnt = length;
	buf = (char *) emalloc((length + 1) * sizeof(char));
	if (!buf) {
		php_error(E_WARNING, "Unable to allocate memory in php3_getpost()");
		return NULL;
	}
#if FHTTPD
    memcpy(buf,req->databuffer,length);
    buf[length]=0;
#else
#if MODULE_MAGIC_NUMBER > 19961007
	if (should_client_block(SG(server_context))) {
		void (*handler) (int);
		int dbsize, len_read, dbpos = 0;

		hard_timeout("copy script args", ((request_rec *) SG(server_context)));	/* start timeout timer */
		handler = signal(SIGPIPE, SIG_IGN);		/* Ignore sigpipes for now */
		while ((len_read = get_client_block(((request_rec *) SG(server_context)), argsbuffer, HUGE_STRING_LEN)) > 0) {
			if ((dbpos + len_read) > length)
				dbsize = length - dbpos;
			else
				dbsize = len_read;
			reset_timeout(((request_rec *) SG(server_context)));	/* Make sure we don't timeout */
			memcpy(buf + dbpos, argsbuffer, dbsize);
			dbpos += dbsize;
		}
		signal(SIGPIPE, handler);	/* restore normal sigpipe handling */
		kill_timeout(((request_rec *) SG(server_context)));	/* stop timeout timer */
	}
#else
	cnt = 0;
	do {
#if APACHE
		bytes = read_client_block(((request_rec *) SG(server_context)), buf + cnt, length - cnt);
#endif
#if CGI_BINARY
		bytes = fread(buf + cnt, 1, length - cnt, stdin);
#endif
#if USE_SAPI
		bytes = sapi_rqst->readclient(sapi_rqst->scid,buf + cnt, 1, length - cnt);
#endif
		cnt += bytes;
	} while (bytes && cnt < length);
#endif
#endif
	if (file_upload) {
		php3_mime_split(buf, cnt, boundary, http_post_vars PLS_CC);
		efree(buf);
		return NULL;
	}
	buf[cnt] = '\0';

#if HAVE_FDFLIB
	if (!strncasecmp(ctype, "application/vnd.fdf", 19)) {
		pval *postdata_ptr = (pval *) emalloc(sizeof(pval));
		
		postdata_ptr->type = IS_STRING;
		postdata_ptr->value.str.val = (char *) estrdup(buf);
		postdata_ptr->value.str.len = cnt;
		INIT_PZVAL(postdata_ptr);
		zend_hash_add(&symbol_table, "HTTP_FDF_DATA", sizeof("HTTP_FDF_DATA"), postdata_ptr, sizeof(pval *),NULL);
	}
#endif
	return (buf);
}
#else
static char *php3_getpost(pval *http_post_vars PLS_DC)
{
	SLS_FETCH();

	return SG(request_info).post_data;
}
#endif


/*
 * parse Get/Post/Cookie string and create appropriate variable
 *
 * This is a tad ugly because it was yanked out of the middle of
 * the old TreatData function.  This is a temporary measure filling 
 * the gap until a more flexible parser can be built to do this.
 */
void _php3_parse_gpc_data(char *val, char *var, pval *track_vars_array)
{
	int var_type;
	char *ind, *tmp = NULL, *ret = NULL;
	int var_len, val_len;
	pval *entry;
	ELS_FETCH();
	PLS_FETCH();
	
	var_type = php3_check_ident_type(var);
	if (var_type == GPC_INDEXED_ARRAY) {
		ind = php3_get_ident_index(var);
		if (PG(magic_quotes_gpc)) {
			ret = php_addslashes(ind, 0, NULL, 1);
		} else {
			ret = ind;
		}
	}
	if (var_type & GPC_ARRAY) {		/* array (indexed or not) */
		tmp = strchr(var, '[');
		if (tmp) {
			*tmp = '\0';
		}
	}
	/* ignore leading spaces in the variable name */
	while (*var && *var==' ') {
		var++;
	}
	var_len = strlen(var);
	if (var_len==0) { /* empty variable name, or variable name with a space in it */
		return;
	}

	/* ensure that we don't have spaces or dots in the variable name (not binary safe) */
	for (tmp=var; *tmp; tmp++) {
		switch(*tmp) {
			case ' ':
			case '.':
				*tmp='_';
				break;
		}
	}

	val_len = strlen(val);
	if (PG(magic_quotes_gpc)) {
		val = php_addslashes(val, val_len, &val_len, 0);
	} else {
		val = estrndup(val, val_len);
	}

	if (var_type & GPC_ARRAY) {
		pval *arr1, *arr2;
		pval **arr_ptr;

		/* If the array doesn't exist, create it */
		if (zend_hash_find(EG(active_symbol_table), var, var_len+1, (void **) &arr_ptr) == FAILURE) {
			arr1 = (pval *) emalloc(sizeof(pval));
			INIT_PZVAL(arr1);
			if (array_init(arr1)==FAILURE) {
				return;
			}
			zend_hash_update(EG(active_symbol_table), var, var_len+1, &arr1, sizeof(pval *), NULL);
			if (track_vars_array) {
				arr2 = (pval *) emalloc(sizeof(pval));
				INIT_PZVAL(arr2);
				if (array_init(arr2)==FAILURE) {
					return;
				}
				zend_hash_update(track_vars_array->value.ht, var, var_len+1, (void *) &arr2, sizeof(pval *),NULL);
			}
		} else {
			if ((*arr_ptr)->type!=IS_ARRAY) {
				if (--(*arr_ptr) > 0) {
					*arr_ptr = (pval *) emalloc(sizeof(pval));
					INIT_PZVAL(*arr_ptr);
				} else {
					pval_destructor(*arr_ptr);
				}
				if (array_init(*arr_ptr)==FAILURE) {
					return;
				}
				if (track_vars_array) {
					arr2 = (pval *) emalloc(sizeof(pval));
					INIT_PZVAL(arr2);
					if (array_init(arr2)==FAILURE) {
						return;
					}
					zend_hash_update(track_vars_array->value.ht, var, var_len+1, (void *) &arr2, sizeof(pval *),NULL);
				}
			}
			arr1 = *arr_ptr;
			if (track_vars_array && zend_hash_find(track_vars_array->value.ht, var, var_len+1, (void **) &arr_ptr) == FAILURE) {
				return;
			}
			arr2 = *arr_ptr;
		}
		/* Now create the element */
		entry = (pval *) emalloc(sizeof(pval));
		INIT_PZVAL(entry);
		entry->value.str.val = val;
		entry->value.str.len = val_len;
		entry->type = IS_STRING;

		/* And then insert it */
		if (ret) {		/* array */
			if (php3_check_type(ret) == IS_LONG) { /* numeric index */
				zend_hash_index_update(arr1->value.ht, atol(ret), &entry, sizeof(pval *),NULL);	/* s[ret]=tmp */
				if (track_vars_array) {
					zend_hash_index_update(arr2->value.ht, atol(ret), &entry, sizeof(pval *),NULL);
					entry->refcount++;
				}
			} else { /* associative index */
				zend_hash_update(arr1->value.ht, ret, strlen(ret)+1, &entry, sizeof(pval *),NULL);	/* s["ret"]=tmp */
				if (track_vars_array) {
					zend_hash_update(arr2->value.ht, ret, strlen(ret)+1, &entry, sizeof(pval *),NULL);
					entry->refcount++;
				}
			}
			efree(ret);
			ret = NULL;
		} else {		/* non-indexed array */
			zend_hash_next_index_insert(arr1->value.ht, &entry, sizeof(pval *),NULL);
			if (track_vars_array) {
				zend_hash_next_index_insert(arr2->value.ht, &entry, sizeof(pval *),NULL);
				entry->refcount++;
			}
		}
	} else {			/* we have a normal variable */
		pval *entry = (pval *) emalloc(sizeof(pval));
		
		entry->type = IS_STRING;
		INIT_PZVAL(entry);
		entry->value.str.val = val;
		entry->value.str.len = val_len;
		zend_hash_update(EG(active_symbol_table), var, var_len+1, (void *) &entry, sizeof(pval *),NULL);
		if (track_vars_array) {
			entry->refcount++;
			zend_hash_update(track_vars_array->value.ht, var, var_len+1, (void *) &entry, sizeof(pval *), NULL);
		}
	}
}


void php3_treat_data(int arg, char *str)
{
	char *res = NULL, *var, *val;
	pval *array_ptr;
	int free_buffer=0;
	ELS_FETCH();
	PLS_FETCH();
	SLS_FETCH();
	
	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			if (PG(track_vars)) {
				array_ptr = (pval *) emalloc(sizeof(pval));
				array_init(array_ptr);
				INIT_PZVAL(array_ptr);
				switch (arg) {
					case PARSE_POST:
						zend_hash_add(&EG(symbol_table), "HTTP_POST_VARS", sizeof("HTTP_POST_VARS"), &array_ptr, sizeof(pval *),NULL);
						break;
					case PARSE_GET:
						zend_hash_add(&EG(symbol_table), "HTTP_GET_VARS", sizeof("HTTP_GET_VARS"), &array_ptr, sizeof(pval *),NULL);
						break;
					case PARSE_COOKIE:
						zend_hash_add(&EG(symbol_table), "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS"), &array_ptr, sizeof(pval *),NULL);
						break;
				}
			} else {
				array_ptr=NULL;
			}
			break;
		default:
			array_ptr=NULL;
			break;
	}

	if (arg == PARSE_POST) {			/* POST data */
		res = php3_getpost(array_ptr PLS_CC);
		free_buffer = 0;
	} else if (arg == PARSE_GET) {		/* GET data */
		var = SG(request_info).query_string;
		if (var && *var) {
			res = (char *) estrdup(var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_COOKIE) {		/* Cookie data */
		var = SG(request_info).cookie_data;
		if (var && *var) {
			res = (char *) estrdup(var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_STRING) {		/* String data */
		res = str;
		free_buffer = 1;
	}
	if (!res) {
		return;
	}
	
	if (arg == PARSE_COOKIE) {
		var = strtok(res, ";");
	} else if (arg == PARSE_POST) {
		var = strtok(res, "&");
	} else {
		var = strtok(res, PG(arg_separator));
	}

	while (var) {
		val = strchr(var, '=');
		if (val) { /* have a value */
			*val++ = '\0';
			/* FIXME: XXX: not binary safe, discards returned length */
			_php3_urldecode(var, strlen(var));
			_php3_urldecode(val, strlen(val));
			_php3_parse_gpc_data(val,var,array_ptr);
		}
		if (arg == PARSE_COOKIE) {
			var = strtok(NULL, ";");
		} else if (arg == PARSE_POST) {
			var = strtok(NULL, "&");
		} else {
			var = strtok(NULL, PG(arg_separator));
		}
	}
	if (free_buffer) {
		efree(res);
	}
}


PHPAPI void php3_TreatHeaders(void)
{
#if 0
#if APACHE
#if MODULE_MAGIC_NUMBER > 19961007
	const char *s = NULL;
#else
	char *s = NULL;
#endif
	char *t;
	char *user, *type;
	int len;
	char *escaped_str;
	request_rec *r;
	PLS_FETCH();
	SLS_FETCH();

	r = ((request_rec *) SG(server_context));
	
	if (r->headers_in)
		s = table_get(r->headers_in, "Authorization");
	if (!s)
		return;

	/* Check to make sure that this URL isn't authenticated
	   using a traditional auth module mechanism */
	if (auth_type(r)) {
		/*php_error(E_WARNING, "Authentication done by server module\n");*/
		return;
	}
	if (strcmp(t=getword(r->pool, &s, ' '), "Basic")) {
		/* Client tried to authenticate using wrong auth scheme */
		php_error(E_WARNING, "client used wrong authentication scheme (%s)", t);
		return;
	}
	t = uudecode(r->pool, s);
#if MODULE_MAGIC_NUMBER > 19961007
	user = getword_nulls_nc(r->pool, &t, ':');
#else
	user = getword(r->pool, &t, ':');
#endif
	type = "Basic";

	if (user) {
		if (PG(magic_quotes_gpc)) {
			escaped_str = php_addslashes(user, 0, &len, 0);
			SET_VAR_STRINGL("PHP_AUTH_USER", escaped_str, len);
		} else {
			SET_VAR_STRING("PHP_AUTH_USER", estrdup(user));
		}
	}
	if (t) {
		if (PG(magic_quotes_gpc)) {
			escaped_str = php_addslashes(t, 0, &len, 0);
			SET_VAR_STRINGL("PHP_AUTH_PW", escaped_str, len);
		} else {
			SET_VAR_STRING("PHP_AUTH_PW", estrdup(t));
		}
	}
	if (type) {
		if (PG(magic_quotes_gpc)) {
			escaped_str = php_addslashes(type, 0, &len, 0);
			SET_VAR_STRINGL("PHP_AUTH_TYPE", escaped_str, len);
		} else {
			SET_VAR_STRING("PHP_AUTH_TYPE", estrdup(type));
		}
	}
#endif
#if FHTTPD
	int i,len;
	struct rline *l;
	char *type;
	char *escaped_str;

	if(req && req->remote_user){
		for(i=0; i < req->nlines; i++){
			l=req->lines+i;
			if((l->paramc > 1)&&!strcasecmp(l->params[0], "REMOTE_PW")){
				type = "Basic";
				if (PG(magic_quotes_gpc)) {
					escaped_str = php_addslashes(type, 0, &len, 0);
					SET_VAR_STRINGL("PHP_AUTH_TYPE", escaped_str, len);
					escaped_str = php_addslashes(l->params[1], 0, &len, 0);
					SET_VAR_STRINGL("PHP_AUTH_PW", escaped_str, len);
					escaped_str = php_addslashes(req->remote_user, 0, &len, 0);
					SET_VAR_STRINGL("PHP_AUTH_USER", escaped_str, len);

				} else {
					SET_VAR_STRING("PHP_AUTH_TYPE", estrdup(type));
					SET_VAR_STRING("PHP_AUTH_PW", estrdup(l->params[1]));
					SET_VAR_STRING("PHP_AUTH_USER", estrdup(req->remote_user));
				}
				i=req->nlines;
			}
		}
	}
#endif
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
