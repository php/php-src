/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_hash.h"
#include "zend_llist.h"
#include "phpdbg.h"
#include "phpdbg_bp.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static void phpdbg_llist_breakfile_dtor(void *data) /* {{{ */
{
	phpdbg_breakfile_t *bp = (phpdbg_breakfile_t*) data;

	efree((char*)bp->filename);
} /* }}} */


static void phpdbg_class_breaks_dtor(void *data) /* {{{ */
{
    phpdbg_breakmethod_t *bp = (phpdbg_breakmethod_t*) data;
    
    efree((char*)bp->class_name);
    efree((char*)bp->func_name);
} /* }}} */

void phpdbg_set_breakpoint_file(const char *path, long line_num TSRMLS_DC) /* {{{ */
{
	phpdbg_breakfile_t new_break;
	zend_llist *break_files_ptr;
	size_t path_len = strlen(path);

	new_break.filename = estrndup(path, path_len);
	new_break.line = line_num;

	PHPDBG_G(has_file_bp) = 1;

	if (zend_hash_find(&PHPDBG_G(bp_files),
		new_break.filename, path_len, (void**)&break_files_ptr) == FAILURE) {
		zend_llist break_files;

		zend_llist_init(&break_files, sizeof(phpdbg_breakfile_t),
			phpdbg_llist_breakfile_dtor, 0);

		zend_hash_update(&PHPDBG_G(bp_files),
			new_break.filename, path_len, &break_files, sizeof(zend_llist),
			(void**)&break_files_ptr);
	}

	new_break.id = PHPDBG_G(bp_count)++;
	zend_llist_add_element(break_files_ptr, &new_break);

	printf(
	    "[Breakpoint #%d added at %s:%ld]\n", new_break.id, new_break.filename, new_break.line);
} /* }}} */

void phpdbg_set_breakpoint_symbol(const char *name TSRMLS_DC) /* {{{ */
{
	size_t name_len = strlen(name);

	if (!zend_hash_exists(&PHPDBG_G(bp_symbols), name, name_len)) {
		phpdbg_breaksymbol_t new_break;

		PHPDBG_G(has_sym_bp) = 1;

		new_break.symbol = estrndup(name, name_len + 1);
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_update(&PHPDBG_G(bp_symbols), new_break.symbol,
			name_len, &new_break, sizeof(phpdbg_breaksymbol_t), NULL);

	    printf("[Breakpoint #%d added at %s]\n", new_break.id, new_break.symbol);
	} else {
	    printf("[Breakpoint exists at %s]\n", name);
	}
} /* }}} */

void phpdbg_set_breakpoint_method(const char* class_name, 
                                  size_t class_len,
                                  const char* func_name, 
                                  size_t func_len TSRMLS_DC) /* {{{ */
{
    HashTable class_breaks, *class_table;
    
    if (zend_hash_find(&PHPDBG_G(bp_methods), class_name, class_len, (void**)&class_table) != SUCCESS) {
        zend_hash_init(
            &class_breaks, 8, NULL, phpdbg_class_breaks_dtor, 0);
        zend_hash_update(
            &PHPDBG_G(bp_methods), 
            class_name, class_len, 
            (void**)&class_breaks, sizeof(HashTable), (void**)&class_table);
    }

    if (!zend_hash_exists(class_table, func_name, func_len)) {
        phpdbg_breakmethod_t new_break;
        
        PHPDBG_G(has_method_bp) = 1;
        
        new_break.class_name = class_name;
        new_break.class_len = class_len;
        new_break.func_name = func_name;
        new_break.func_len = func_len;
        new_break.id = PHPDBG_G(bp_count)++;
                
        zend_hash_update(class_table, func_name, func_len, &new_break, sizeof(phpdbg_breakmethod_t), NULL);
        printf(
            "[Breakpoint #%d added at %s::%s]\n", new_break.id, class_name, func_name);
    } else {
        printf(
            "[Breakpoint exists at %s::%s]\n", class_name, func_name);
    }
} /* }}} */

void phpdbg_set_breakpoint_opline(const char *name TSRMLS_DC) /* {{{ */
{
	zend_ulong opline = strtoul(name, 0, 16);

	if (!zend_hash_index_exists(&PHPDBG_G(bp_oplines), opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(has_opline_bp) = 1;

        new_break.name = strdup(name);
		new_break.opline = opline;
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_index_update(&PHPDBG_G(bp_oplines), opline, &new_break, sizeof(phpdbg_breakline_t), NULL);

	    printf("[Breakpoint #%d added at %s]\n", new_break.id, new_break.name);
	} else {
	    printf("[Breakpoint exists at %s]\n", name);
	}
} /* }}} */

void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp_oplines), (zend_ulong) opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(has_opline_bp) = 1;

        asprintf(
            (char**)&new_break.name, "%p", (zend_op*) opline);

		new_break.opline = (zend_ulong) opline;
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_index_update(&PHPDBG_G(bp_oplines), (zend_ulong) opline, &new_break, sizeof(phpdbg_breakline_t), NULL);

	    printf("[Breakpoint #%d added at %p]\n", new_break.id, (zend_op*) new_break.opline);
	}
} /* }}} */

int phpdbg_find_breakpoint_file(zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	size_t name_len = strlen(op_array->filename);
	zend_llist *break_list;
	zend_llist_element *le;

	if (zend_hash_find(&PHPDBG_G(bp_files), op_array->filename, name_len,
		(void**)&break_list) == FAILURE) {
		return FAILURE;
	}

	for (le = break_list->head; le; le = le->next) {
		const phpdbg_breakfile_t *bp = (phpdbg_breakfile_t*)le->data;

		if (bp->line == (*EG(opline_ptr))->lineno) {
			printf("[Breakpoint #%d at %s:%ld]\n", bp->id, bp->filename, bp->line);
			return SUCCESS;
		}
	}

	return FAILURE;
} /* }}} */

int phpdbg_find_breakpoint_symbol(zend_function *fbc TSRMLS_DC) /* {{{ */
{
	const char *fname;
	zend_op_array *ops;
	phpdbg_breaksymbol_t *bp;

	if (fbc->type != ZEND_USER_FUNCTION) {
		return FAILURE;
	}
	
	ops = (zend_op_array*)fbc;
	
	if (ops->scope) {
	    /* find method breaks here */
	    return phpdbg_find_breakpoint_method(
	        ops TSRMLS_CC);
	}

	fname = ops->function_name;

	if (!fname) {
		fname = "main";
	}

	if (zend_hash_find(&PHPDBG_G(bp_symbols), fname, strlen(fname),
		(void**)&bp) == SUCCESS) {
		printf("[Breakpoint #%d in %s() at %s:%u]\n", bp->id, bp->symbol,
			zend_get_executed_filename(TSRMLS_C),
			zend_get_executed_lineno(TSRMLS_C));
		return SUCCESS;
	}

	return FAILURE;
} /* }}} */

int phpdbg_find_breakpoint_method(zend_op_array *ops TSRMLS_DC) /* {{{ */
{
	HashTable *class_table;
	phpdbg_breakmethod_t *bp;

	if (zend_hash_find(&PHPDBG_G(bp_methods), ops->scope->name, ops->scope->name_length, 
	        (void**)&class_table) == SUCCESS) {
		if (zend_hash_find(
		        class_table,
		        ops->function_name, 
		        strlen(ops->function_name), (void**)&bp) == SUCCESS) {
		        
		    printf(
		        "[Breakpoint #%d in %s::%s() at %s:%u]\n", bp->id, bp->class_name, bp->func_name,
			    zend_get_executed_filename(TSRMLS_C),
			    zend_get_executed_lineno(TSRMLS_C));
			return SUCCESS;
		}
	}

	return FAILURE;
} /* }}} */

int phpdbg_find_breakpoint_opline(phpdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	phpdbg_breakline_t *bp;

	if (zend_hash_index_find(&PHPDBG_G(bp_oplines), (zend_ulong) opline,
	        (void**)&bp) == SUCCESS) {
		printf("[Breakpoint #%d in %s at %s:%u]\n", bp->id, bp->name,
			zend_get_executed_filename(TSRMLS_C),
			zend_get_executed_lineno(TSRMLS_C));

		return SUCCESS;
	}

	return FAILURE;
} /* }}} */

void phpdbg_clear_breakpoints(TSRMLS_D) /* {{{ */
{
    zend_hash_clean(&PHPDBG_G(bp_files));
    zend_hash_clean(&PHPDBG_G(bp_symbols));
    zend_hash_clean(&PHPDBG_G(bp_oplines));
    zend_hash_clean(&PHPDBG_G(bp_methods));
    
    PHPDBG_G(has_file_bp) = 0;
    PHPDBG_G(has_sym_bp) = 0;
    PHPDBG_G(has_opline_bp) = 0;
    PHPDBG_G(has_method_bp) = 0;
    PHPDBG_G(bp_count) = 0;
} /* }}} */

