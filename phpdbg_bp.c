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
#include "phpdbg_utils.h"
#include "zend_globals.h"

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
    struct stat sb;

    if (VCWD_STAT(path, &sb) != FAILURE) {
        if (sb.st_mode & (S_IFREG|S_IFLNK)) {
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

            phpdbg_notice("Breakpoint #%d added at %s:%ld",
                new_break.id, new_break.filename, new_break.line);
        } else {
            phpdbg_error("Cannot set breakpoint in %s, it is not a regular file", path);
        }
    } else {
        phpdbg_error("Cannot stat %s, it does not exist", path);
    }
} /* }}} */

void phpdbg_set_breakpoint_symbol(const char *name TSRMLS_DC) /* {{{ */
{
	size_t name_len = strlen(name);

	if (!zend_hash_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], name, name_len)) {
		phpdbg_breaksymbol_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_SYM_BP;

		new_break.symbol = estrndup(name, name_len);
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_update(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], new_break.symbol,
			name_len, &new_break, sizeof(phpdbg_breaksymbol_t), NULL);

	    phpdbg_notice("Breakpoint #%d added at %s",
			new_break.id, new_break.symbol);
	} else {
	    phpdbg_notice("Breakpoint exists at %s", name);
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

        phpdbg_notice("Breakpoint #%d added at %s::%s",
            new_break.id, class_name, func_name);
    } else {
		phpdbg_notice("Breakpoint exists at %s::%s", class_name, func_name);
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

		zend_hash_index_update(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline,
			&new_break, sizeof(phpdbg_breakline_t), NULL);

	    phpdbg_notice("Breakpoint #%d added at %#lx",
			new_break.id, new_break.opline);
	} else {
	    phpdbg_notice("Breakpoint exists at %#lx", opline);
	}
} /* }}} */

void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

		new_break.opline = (zend_ulong) opline;
		new_break.id = PHPDBG_G(bp_count)++;

		zend_hash_index_update(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE],
			(zend_ulong) opline, &new_break, sizeof(phpdbg_breakline_t), NULL);

	    phpdbg_notice("Breakpoint #%d added at %#lx",
			new_break.id, new_break.opline);
	}
} /* }}} */

void phpdbg_set_breakpoint_expression(const char *expr, size_t expr_len TSRMLS_DC) /* {{{ */
{
    zend_ulong hash = zend_inline_hash_func(expr, expr_len);

    if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], hash)) {
        phpdbg_breakcond_t new_break;
        zend_uint cops = CG(compiler_options);
        zval pv;

        ZVAL_STRINGL(&new_break.code, expr, expr_len, 1);

	    new_break.id = PHPDBG_G(bp_count)++;

        cops = CG(compiler_options);

        CG(compiler_options) = ZEND_COMPILE_DEFAULT_FOR_EVAL;

		Z_STRLEN(pv) = expr_len + sizeof("return ;") - 1;
		Z_STRVAL(pv) = emalloc(Z_STRLEN(pv) + 1);
		memcpy(Z_STRVAL(pv), "return ", sizeof("return ") - 1);
		memcpy(Z_STRVAL(pv) + sizeof("return ") - 1, expr, expr_len);
		Z_STRVAL(pv)[Z_STRLEN(pv) - 1] = ';';
		Z_STRVAL(pv)[Z_STRLEN(pv)] = '\0';
		Z_TYPE(pv) = IS_STRING;

		new_break.ops = zend_compile_string(
			&pv, "Conditional Breakpoint Code" TSRMLS_CC);

		if (new_break.ops) {
			phpdbg_breakcond_t *broken;

			zend_hash_index_update(
				&PHPDBG_G(bp)[PHPDBG_BREAK_COND], hash, &new_break,
				sizeof(phpdbg_breakcond_t), (void**)&broken);

			phpdbg_notice("Conditional breakpoint #%d added %s/%p",
				broken->id, Z_STRVAL(broken->code), broken->ops);

			PHPDBG_G(flags) |= PHPDBG_HAS_COND_BP;
		} else {
			 phpdbg_error(
				"Failed to compile code for expression %s", expr);
			 zval_dtor(&new_break.code);
			 PHPDBG_G(bp_count)--;
		}
		CG(compiler_options) = cops;
    } else {
        phpdbg_notice("Conditional break %s exists", expr);
    }
} /* }}} */

int phpdbg_find_breakpoint_file(zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	size_t name_len = strlen(op_array->filename);
	zend_llist *break_list;
	zend_llist_element *le;

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], op_array->filename,
		name_len, (void**)&break_list) == FAILURE) {
		return FAILURE;
	}

	for (le = break_list->head; le; le = le->next) {
		const phpdbg_breakfile_t *bp = (phpdbg_breakfile_t*)le->data;

		if (bp->line == (*EG(opline_ptr))->lineno) {
			phpdbg_notice("Breakpoint #%d at %s:%ld",
				bp->id, bp->filename, bp->line);
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
		phpdbg_notice("Breakpoint #%d in %s() at %s:%u",
		    bp->id, bp->symbol,
			zend_get_executed_filename(TSRMLS_C),
			zend_get_executed_lineno(TSRMLS_C));
		return SUCCESS;
	}

	return FAILURE;
} /* }}} */

/*
* @TODO(anyone) this is case sensitive
*/
int phpdbg_find_breakpoint_method(zend_op_array *ops TSRMLS_DC) /* {{{ */
{
	HashTable *class_table;
	phpdbg_breakmethod_t *bp;

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], ops->scope->name,
		ops->scope->name_length, (void**)&class_table) == SUCCESS) {
		if (zend_hash_find(
		        class_table,
		        ops->function_name,
		        strlen(ops->function_name), (void**)&bp) == SUCCESS) {

		    phpdbg_notice("Breakpoint #%d in %s::%s() at %s:%u",
		        bp->id, bp->class_name, bp->func_name,
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

	if (zend_hash_index_find(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE],
		(zend_ulong) opline, (void**)&bp) == SUCCESS) {
		phpdbg_notice("Breakpoint #%d in %#lx at %s:%u",
		    bp->id, bp->opline,
			zend_get_executed_filename(TSRMLS_C),
			zend_get_executed_lineno(TSRMLS_C));

		return SUCCESS;
	}

	return FAILURE;
} /* }}} */

int phpdbg_find_conditional_breakpoint(TSRMLS_D) /* {{{ */
{
	phpdbg_breakcond_t *bp;
    HashPosition position;
    int breakpoint = FAILURE;

    for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], &position);
         zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], (void*)&bp, &position) == SUCCESS;
         zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], &position)) {

         zval *retval = NULL;
         int orig_interactive = CG(interactive);
         zval **orig_retval = EG(return_value_ptr_ptr);
         zend_op_array *orig_ops = EG(active_op_array);
         zend_op **orig_opline = EG(opline_ptr);

         ALLOC_INIT_ZVAL(retval);

         EG(return_value_ptr_ptr) = &retval;
         EG(active_op_array) = bp->ops;
         EG(no_extensions) = 1;

         if (!EG(active_symbol_table)) {
            zend_rebuild_symbol_table(TSRMLS_C);
         }

         CG(interactive) = 0;

         zend_try {
            PHPDBG_G(flags) |= PHPDBG_IN_COND_BP;
            zend_execute(
                EG(active_op_array) TSRMLS_CC);
            if (i_zend_is_true(retval)) {
                breakpoint = SUCCESS;
            }
         } zend_catch {
            phpdbg_error(
                "Error detected while evaluating expression %s", Z_STRVAL(bp->code));
            CG(interactive) = orig_interactive;

            EG(no_extensions)=1;
            EG(return_value_ptr_ptr) = orig_retval;
            EG(active_op_array) = orig_ops;
            EG(opline_ptr) = orig_opline;
            PHPDBG_G(flags) &= ~PHPDBG_IN_COND_BP;
         } zend_end_try();

         CG(interactive) = orig_interactive;

         EG(no_extensions)=1;
         EG(return_value_ptr_ptr) = orig_retval;
         EG(active_op_array) = orig_ops;
         EG(opline_ptr) = orig_opline;
         PHPDBG_G(flags) &= ~PHPDBG_IN_COND_BP;

         if (breakpoint == SUCCESS) {
            break;
         }
    }

    if (breakpoint == SUCCESS) {
        phpdbg_notice("Conditional breakpoint #%d: (%s) %s:%u",
		    bp->id, Z_STRVAL(bp->code),
			zend_get_executed_filename(TSRMLS_C),
			zend_get_executed_lineno(TSRMLS_C));
    }

	return breakpoint;
} /* }}} */

void phpdbg_clear_breakpoints(TSRMLS_D) /* {{{ */
{
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
    zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);

    PHPDBG_G(flags) &= ~PHPDBG_BP_MASK;

    PHPDBG_G(bp_count) = 0;
} /* }}} */

void phpdbg_print_breakpoints(zend_ulong type TSRMLS_DC) /* {{{ */
{
    switch (type) {
        case PHPDBG_BREAK_SYM: if ((PHPDBG_G(flags) & PHPDBG_HAS_SYM_BP)) {
            HashPosition position;
            phpdbg_breaksymbol_t *brake;

            phpdbg_writeln(SEPARATE);
            phpdbg_writeln("Function Breakpoints:");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], &position);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], (void**) &brake, &position) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], &position)) {
                 phpdbg_writeln(
                    "#%d\t\t%s", brake->id, brake->symbol);
            }
        } break;

        case PHPDBG_BREAK_METHOD: if ((PHPDBG_G(flags) & PHPDBG_HAS_METHOD_BP)) {
            HashPosition position[2];
            HashTable *class_table;
            char *class_name = NULL;
            zend_uint class_len = 0;
            zend_ulong class_idx = 0L;

            phpdbg_writeln(SEPARATE);
            phpdbg_writeln("Method Breakpoints:");
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
                          phpdbg_writeln(
                            "#%d\t\t%s::%s", brake->id, brake->class_name, brake->func_name);
                     }
                 }

            }
        } break;

        case PHPDBG_BREAK_FILE: if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP)) {
            HashPosition position;
            zend_llist *points;

            phpdbg_writeln(SEPARATE);
            phpdbg_writeln("File Breakpoints:");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], (void**) &points, &position) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position)) {
                 zend_llist_position lposition;
                 phpdbg_breakfile_t *brake;

                 if ((brake = zend_llist_get_first_ex(points, &lposition))) {
                    do {
                        phpdbg_writeln("#%d\t\t%s:%lu", brake->id, brake->filename, brake->line);
                    } while ((brake = zend_llist_get_next_ex(points, &lposition)));
                 }
            }
        } break;

        case PHPDBG_BREAK_OPLINE: if ((PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP)) {
            HashPosition position;
            phpdbg_breakline_t *brake;

            phpdbg_writeln(SEPARATE);
            phpdbg_writeln("Opline Breakpoints:");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], &position);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (void**) &brake, &position) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], &position)) {
                 phpdbg_writeln("#%d\t\t%#lx", brake->id, brake->opline);
            }
        } break;

        case PHPDBG_BREAK_COND: if ((PHPDBG_G(flags) & PHPDBG_HAS_COND_BP)) {
            HashPosition position;
            phpdbg_breakcond_t *brake;

            phpdbg_writeln(SEPARATE);
            phpdbg_writeln("Conditional Breakpoints:");
            for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], &position);
                 zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], (void**) &brake, &position) == SUCCESS;
                 zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], &position)) {
                 phpdbg_writeln("#%d\t\t%s", brake->id, Z_STRVAL(brake->code));
            }
        } break;
    }
} /* }}} */

