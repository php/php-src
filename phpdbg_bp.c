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

	PHPDBG_G(flags) |= PHPDBG_HAS_FILE_BP;

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE],
		new_break.filename, path_len, (void**)&break_files_ptr) == FAILURE) {
		zend_llist break_files;

		zend_llist_init(&break_files, sizeof(phpdbg_breakfile_t),
			phpdbg_llist_breakfile_dtor, 0);

		zend_hash_update(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE],
			new_break.filename, path_len, &break_files, sizeof(zend_llist),
			(void**)&break_files_ptr);
	}

	new_break.id = PHPDBG_G(bp_count)++;
	zend_llist_add_element(break_files_ptr, &new_break);

	printf(
	    "%sBreakpoint #%d added at %s:%ld%s\n",
	    PHPDBG_BOLD_LINE(TSRMLS_C),
	    new_break.id, new_break.filename, new_break.line,
	    PHPDBG_END_LINE(TSRMLS_C));
} /* }}} */

void phpdbg_set_breakpoint_symbol(const char *name TSRMLS_DC) /* {{{ */
{
	size_t name_len = strlen(name);

	if (!zend_hash_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], name, name_len)) {
		phpdbg_breaksymbol_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_SYM_BP;

		new_break.symbol = estrndup(name, name_len + 1);
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_update(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], new_break.symbol,
			name_len, &new_break, sizeof(phpdbg_breaksymbol_t), NULL);

	    printf(
	        "%sBreakpoint #%d added at %s%s\n",
	        PHPDBG_BOLD_LINE(TSRMLS_C),
	        new_break.id, new_break.symbol,
	        PHPDBG_END_LINE(TSRMLS_C));
	} else {
	    printf(
	        "%sBreakpoint exists at %s%s\n",
	        PHPDBG_BOLD_LINE(TSRMLS_C), name, PHPDBG_END_LINE(TSRMLS_C));
	}
} /* }}} */

void phpdbg_set_breakpoint_method(const char* class_name, const char* func_name TSRMLS_DC) /* {{{ */
{
    HashTable class_breaks, *class_table;
    size_t class_len = strlen(class_name);
    size_t func_len = strlen(func_name);

    if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], class_name,
		class_len, (void**)&class_table) != SUCCESS) {
        zend_hash_init(&class_breaks, 8, NULL, phpdbg_class_breaks_dtor, 0);
        zend_hash_update(
            &PHPDBG_G(bp)[PHPDBG_BREAK_METHOD],
            class_name, class_len,
            (void**)&class_breaks, sizeof(HashTable), (void**)&class_table);
    }

    if (!zend_hash_exists(class_table, func_name, func_len)) {
        phpdbg_breakmethod_t new_break;

        PHPDBG_G(flags) |= PHPDBG_HAS_METHOD_BP;

        new_break.class_name = class_name;
        new_break.class_len = class_len;
        new_break.func_name = func_name;
        new_break.func_len = func_len;
        new_break.id = PHPDBG_G(bp_count)++;

        zend_hash_update(class_table, func_name, func_len,
			&new_break, sizeof(phpdbg_breakmethod_t), NULL);

        printf(
            "%sBreakpoint #%d added at %s::%s%s\n",
            PHPDBG_BOLD_LINE(TSRMLS_C),
            new_break.id, class_name, func_name,
            PHPDBG_END_LINE(TSRMLS_C));
    } else {
        printf(
            "%sBreakpoint exists at %s::%s%s\n",
             PHPDBG_BOLD_LINE(TSRMLS_C),
             class_name, func_name,
             PHPDBG_END_LINE(TSRMLS_C));
    }
} /* }}} */

void phpdbg_set_breakpoint_opline(zend_ulong opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

        new_break.name = NULL;
		new_break.opline = opline;
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_index_update(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline, &new_break, sizeof(phpdbg_breakline_t), NULL);

	    printf("%sBreakpoint #%d added at %#lx%s\n",
	        PHPDBG_BOLD_LINE(TSRMLS_C),
	        new_break.id, new_break.opline,
	        PHPDBG_END_LINE(TSRMLS_C));
	} else {
	    printf(
	        "%sBreakpoint exists at %#lx%s\n",
	        PHPDBG_BOLD_LINE(TSRMLS_C), opline, PHPDBG_END_LINE(TSRMLS_C));
	}
} /* }}} */

void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

		new_break.opline = (zend_ulong) opline;
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_index_update(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline, &new_break, sizeof(phpdbg_breakline_t), NULL);

	    printf(
	        "%sBreakpoint #%d added at %#lx%s\n",
	        PHPDBG_BOLD_LINE(TSRMLS_C),
	        new_break.id, new_break.opline,
	        PHPDBG_END_LINE(TSRMLS_C));
	}
} /* }}} */

int phpdbg_find_breakpoint_file(zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	size_t name_len = strlen(op_array->filename);
	zend_llist *break_list;
	zend_llist_element *le;

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], op_array->filename, name_len,
		(void**)&break_list) == FAILURE) {
		return FAILURE;
	}

	for (le = break_list->head; le; le = le->next) {
		const phpdbg_breakfile_t *bp = (phpdbg_breakfile_t*)le->data;

		if (bp->line == (*EG(opline_ptr))->lineno) {
			printf("%sBreakpoint #%d at %s:%ld%s\n",
			PHPDBG_BOLD_LINE(TSRMLS_C),
			bp->id, bp->filename, bp->line,
			PHPDBG_END_LINE(TSRMLS_C));
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

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], fname, strlen(fname),
		(void**)&bp) == SUCCESS) {
		printf("%sBreakpoint #%d in %s() at %s:%u%s\n",
		    PHPDBG_BOLD_LINE(TSRMLS_C),
		    bp->id, bp->symbol,
			zend_get_executed_filename(TSRMLS_C),
			zend_get_executed_lineno(TSRMLS_C),
			PHPDBG_END_LINE(TSRMLS_C));
		return SUCCESS;
	}

	return FAILURE;
} /* }}} */

int phpdbg_find_breakpoint_method(zend_op_array *ops TSRMLS_DC) /* {{{ */
{
	HashTable *class_table;
	phpdbg_breakmethod_t *bp;

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], ops->scope->name, ops->scope->name_length,
	        (void**)&class_table) == SUCCESS) {
		if (zend_hash_find(
		        class_table,
		        ops->function_name,
		        strlen(ops->function_name), (void**)&bp) == SUCCESS) {

		    printf(
		        "%sBreakpoint #%d in %s::%s() at %s:%u%s\n",
		        PHPDBG_BOLD_LINE(TSRMLS_C),
		        bp->id, bp->class_name, bp->func_name,
			    zend_get_executed_filename(TSRMLS_C),
			    zend_get_executed_lineno(TSRMLS_C),
			    PHPDBG_END_LINE(TSRMLS_C));
			return SUCCESS;
		}
	}

	return FAILURE;
} /* }}} */

int phpdbg_find_breakpoint_opline(phpdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	phpdbg_breakline_t *bp;

	if (zend_hash_index_find(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline,
	        (void**)&bp) == SUCCESS) {
		printf("%sBreakpoint #%d in %#lx at %s:%u%s\n",
		    PHPDBG_BOLD_LINE(TSRMLS_C),
		    bp->id, bp->opline,
			zend_get_executed_filename(TSRMLS_C),
			zend_get_executed_lineno(TSRMLS_C),
			PHPDBG_END_LINE(TSRMLS_C));

		return SUCCESS;
	}

	return FAILURE;
} /* }}} */

void phpdbg_clear_breakpoints(TSRMLS_D) /* {{{ */
{
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);

    PHPDBG_G(flags) &= ~PHPDBG_BP_MASK;

    PHPDBG_G(bp_count) = 0;
} /* }}} */

void phpdbg_print_breakpoints(zend_ulong type TSRMLS_DC) /* {{{ */
{
    switch (type) {
        case PHPDBG_BREAK_SYM: if ((PHPDBG_G(flags) & PHPDBG_HAS_SYM_BP)) {
            HashPosition position;
            phpdbg_breaksymbol_t *brake;

            PHPDBG_SEP_LINE(TSRMLS_C);
            printf("Function Breakpoints:\n");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], &position);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], (void**) &brake, &position) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], &position)) {
                 printf(
                    "#%d\t\t%s\n", brake->id, brake->symbol);
            }
        } break;
        
        case PHPDBG_BREAK_METHOD: if ((PHPDBG_G(flags) & PHPDBG_HAS_METHOD_BP)) {
            HashPosition position[2];
            HashTable *class_table;
            char *class_name = NULL;
            zend_uint class_len = 0;
            zend_ulong class_idx = 0L;
            
            PHPDBG_SEP_LINE(TSRMLS_C);
            printf("Method Breakpoints:\n");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], &position[0]);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], (void**) &class_table, &position[0]) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], &position[0])) {
                 
                 if (zend_hash_get_current_key_ex(
                    &PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], 
                    &class_name, &class_len, &class_idx, 0, &position[0]) == HASH_KEY_IS_STRING) {
                    
                     phpdbg_breakmethod_t *brake;
                    
                     for (zend_hash_internal_pointer_reset_ex(class_table, &position[1]);
                          zend_hash_get_current_data_ex(class_table, (void**)&brake, &position[1]) == SUCCESS;
                          zend_hash_move_forward_ex(class_table, &position[1])) {
                          printf(
                            "#%d\t\t%s::%s\n", brake->id, brake->class_name, brake->func_name);
                     }
                 }
                 
            }
        } break;
        
        case PHPDBG_BREAK_FILE: if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP)) {
            HashPosition position;
            zend_llist *points;

            PHPDBG_SEP_LINE(TSRMLS_C);
            printf("File Breakpoints:\n");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], (void**) &points, &position) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position)) {
                 zend_llist_position lposition;
                 phpdbg_breakfile_t *brake;

                 if ((brake = zend_llist_get_first_ex(points, &lposition))) {
                    do {
                        printf("#%d\t\t%s:%lu\n", brake->id, brake->filename, brake->line);
                    } while ((brake = zend_llist_get_next_ex(points, &lposition)));
                 }
            }
        } break;
        
        case PHPDBG_BREAK_OPLINE: if ((PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP)) {
            HashPosition position;
            phpdbg_breakline_t *brake;

            PHPDBG_SEP_LINE(TSRMLS_C);
            printf("Opline Breakpoints:\n");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], &position);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (void**) &brake, &position) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], &position)) {
                 printf("#%d\t\t%p\n", brake->id, brake->opline);
            }
        } break;
    }
} /* }}} */

