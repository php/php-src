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
 * parse Get/Post/Cookie string and create appropriate variable
 *
 * This is a tad ugly because it was yanked out of the middle of
 * the old TreatData function.  This is a temporary measure filling 
 * the gap until a more flexible parser can be built to do this.
 */
void php_parse_gpc_data(char *val, char *var, pval *track_vars_array ELS_DC PLS_DC)
{
	int var_type;
	char *ind, *tmp = NULL, *ret = NULL;
	int var_len, val_len;
	pval *entry;
	
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


void php_treat_data(int arg, char *str ELS_DC PLS_DC SLS_DC)
{
	char *res = NULL, *var, *val;
	pval *array_ptr;
	int free_buffer=0;
	
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
		res = SG(request_info).post_data;
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
			php_parse_gpc_data(val,var,array_ptr ELS_CC PLS_CC);
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


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
