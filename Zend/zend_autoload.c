/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Anthony Ferrara <ircmaxell@php.net>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_execute.h"
#include "zend_globals.h"
#include "zend_globals_macros.h"
#include "zend_autoload.h"
#include "zend_hash.h"
#include "zend_execute.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

static char* zend_autoload_get_name_key(zend_fcall_info *fci, int *length, zend_bool *do_free TSRMLS_DC);

int zend_autoload_call(zval* name, long type TSRMLS_DC)
{
	zval *ztype, *retval = NULL;
	char *lc_name;
	int lc_length;
	HashTable *symbol_table;
	HashPosition function_pos;
	zend_autoload_func *func_info;
	char dummy = 1;

	if (Z_TYPE_P(name) != IS_STRING) {
		return FAILURE;
	}

	switch (type) {
		case ZEND_AUTOLOAD_CLASS:
			symbol_table = EG(class_table);
			break;
		case ZEND_AUTOLOAD_FUNCTION:
			symbol_table = EG(function_table);
			break;
		default:
			return FAILURE;
	}

	lc_length = Z_STRLEN_P(name);
	lc_name = zend_str_tolower_dup(Z_STRVAL_P(name), lc_length);

	if (EG(autoload_funcs) == NULL || EG(autoload_funcs)->nNumOfElements == 0) {
		if (type == ZEND_AUTOLOAD_CLASS
			&& (	
				EG(autoload_legacy) != NULL
				|| zend_lookup_function_ex(ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME) - 1, NULL, 0, &EG(autoload_legacy) TSRMLS_CC) == SUCCESS
			)
		) {
			zend_call_method_with_1_params(NULL, NULL, &EG(autoload_legacy), ZEND_AUTOLOAD_FUNC_NAME, &retval, name);
			if (zend_hash_exists(symbol_table, lc_name, lc_length + 1)) {
				efree(lc_name);
				return SUCCESS;
			}
		}
		efree(lc_name);
		return FAILURE;
	}

	if (EG(autoload_stack) == NULL) {
		ALLOC_HASHTABLE(EG(autoload_stack));
		zend_hash_init(EG(autoload_stack), 0, NULL, NULL, 0);
	}

	if (zend_hash_add(EG(autoload_stack), lc_name, lc_length, (void**)&dummy, sizeof(char), NULL) == FAILURE) {
		efree(lc_name);
		return FAILURE;
	}

	MAKE_STD_ZVAL(ztype);
	ZVAL_LONG(ztype, type);

	zend_hash_internal_pointer_reset_ex(EG(autoload_funcs), &function_pos);
	while(zend_hash_has_more_elements_ex(EG(autoload_funcs), &function_pos) == SUCCESS) {
		zend_hash_get_current_data_ex(EG(autoload_funcs), (void **) &func_info, &function_pos);
		if (func_info->type & type) {
			func_info->fci.retval_ptr_ptr = &retval;
			zend_fcall_info_argn(&func_info->fci, 2, &name, &ztype);
			zend_call_function(&func_info->fci, &func_info->fcc TSRMLS_CC);
			zend_exception_save(TSRMLS_C);
			if (retval) {
				zval_ptr_dtor(&retval);
				retval = NULL;
			}
			if (zend_hash_exists(symbol_table, lc_name, lc_length + 1)) {
				break;
			}
		}
		zend_hash_move_forward_ex(EG(autoload_funcs), &function_pos);
	}
	zend_exception_restore(TSRMLS_C);

	Z_DELREF_P(ztype);
	zend_hash_del(EG(autoload_stack), lc_name, lc_length);
	efree(lc_name);
	return SUCCESS;
}

#define HT_MOVE_TAIL_TO_HEAD(ht)                            \
    (ht)->pListTail->pListNext = (ht)->pListHead;           \
    (ht)->pListHead = (ht)->pListTail;                      \
    (ht)->pListTail = (ht)->pListHead->pListLast;           \
    (ht)->pListHead->pListNext->pListLast = (ht)->pListHead;\
    (ht)->pListTail->pListNext = NULL;                      \
    (ht)->pListHead->pListLast = NULL;

static char* zend_autoload_get_name_key(zend_fcall_info *fci, int *length, zend_bool *do_free TSRMLS_DC) {
	char *name;
	switch (Z_TYPE_P(fci->function_name)) {
		case IS_STRING:
			*length = Z_STRLEN_P(fci->function_name);
			return Z_STRVAL_P(fci->function_name);
			break;
		case IS_OBJECT:
			*length = sizeof(zend_object_handle);
			name = emalloc(*length + 1);
			*do_free = 1;
			memcpy(name, &Z_OBJ_HANDLE_P(fci->function_name), *length);
			name[*length] = '\0';
			return name;
			break;
		default:
			return 0;
	}
}

int zend_autoload_register(zend_autoload_func *func, zend_bool prepend TSRMLS_DC)
{
	char *lc_name;
	zend_bool do_free = 0;
	int lc_length, status = SUCCESS;

	lc_name = zend_autoload_get_name_key(&func->fci, &lc_length, &do_free);
	if (lc_name == 0) {
		zend_error_noreturn(E_ERROR, "Unknown Function Name Type Provided");
	} else if (do_free) {
		Z_ADDREF_P(func->callable);
	}

	if (!EG(autoload_funcs)) {
		ALLOC_HASHTABLE(EG(autoload_funcs));
		zend_hash_init(EG(autoload_funcs), 0, NULL, NULL, 0);
	} else if (zend_hash_exists(EG(autoload_funcs), lc_name, lc_length + 1)) {
		if (do_free) {
			efree(lc_name);
			Z_DELREF_P(func->callable);
		}
		return FAILURE;
	}

	if (zend_hash_add(EG(autoload_funcs), lc_name, lc_length + 1, (void**) func, sizeof(zend_autoload_func), NULL) == FAILURE) {
		status = FAILURE;
	} else if (prepend) {
		HT_MOVE_TAIL_TO_HEAD(EG(autoload_funcs));
	}
	if (do_free) {
		efree(lc_name);
		Z_DELREF_P(func->callable);
	}
	return status;
}

int zend_autoload_unregister(zval *callable TSRMLS_DC)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	char *lc_name;
	zend_bool do_free = 0;
	int lc_length;

	if (zend_fcall_info_init(callable, 0, &fci, &fcc, NULL, NULL) == FAILURE) {
		return FAILURE;
	}

	lc_name = zend_autoload_get_name_key(&fci, &lc_length, &do_free);
	if (lc_name == 0) {
		return FAILURE;
	}

	zend_hash_del(EG(autoload_funcs), lc_name, lc_length);
	if (do_free) {
		efree(lc_name);
	}

	return SUCCESS;
}

ZEND_FUNCTION(autoload_register)
{
	zval *callable;
	zend_autoload_func *func;
	zend_bool prepend = 0;
	long type = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_DC, "z|lb", &callable, &type, &prepend) == FAILURE) {
		return;
	}

	func = emalloc(sizeof(zend_autoload_func));

	if (zend_fcall_info_init(callable, 0, &func->fci, &func->fcc, NULL, NULL) == FAILURE) {
		efree(func);
		zend_error_noreturn(E_ERROR, "Expecting a valid callback");
	}

	func->callable = callable;
	Z_ADDREF_P(callable);

	if (!type) {
		func->type = ZEND_AUTOLOAD_FUNCTION | ZEND_AUTOLOAD_CLASS;
	} else {
		func->type = type;
	}

	if (zend_autoload_register(func, prepend TSRMLS_CC) == FAILURE) {
		Z_DELREF_P(callable);
		efree(func);
	}
}

ZEND_FUNCTION(autoload_unregister)
{
	zval *callable;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_DC, "z", &callable) == FAILURE) {
		return;
	}
	RETVAL_BOOL(zend_autoload_unregister(callable TSRMLS_CC) == SUCCESS);
}

