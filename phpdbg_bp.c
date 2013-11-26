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
#include "phpdbg.h"
#include "phpdbg_bp.h"
#include "phpdbg_utils.h"
#include "phpdbg_opcode.h"
#include "zend_globals.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

/*
* Note:
*	A break point must always set the corect id and type
*	A set breakpoint function must always map new points
*/
static inline _phpdbg_break_mapping(int id, HashTable* table TSRMLS_DC) {
	zend_hash_index_update(
		&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], (id), (void**) &table, sizeof(void*), NULL);
}

#define PHPDBG_BREAK_MAPPING(id, table) _phpdbg_break_mapping(id, table TSRMLS_CC)
#define PHPDBG_BREAK_UNMAPPING(id) \
	zend_hash_index_del(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], (id))

static void phpdbg_file_breaks_dtor(void *data) /* {{{ */
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

PHPDBG_API void phpdbg_export_breakpoints(FILE *handle TSRMLS_DC) /* {{{ */
{
	HashPosition position[2];
	HashTable **table = NULL;
	zend_ulong id = 0L;
	
	if (zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP])) {
		phpdbg_notice(
			"Exporting %ld breakpoints", 
			zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP]));
		
		for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], &position[0]);
			zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], (void**)&table, &position[0]) == SUCCESS;
			zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], &position[0])) {
			phpdbg_breakbase_t *brake;

			zend_hash_get_current_key_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], NULL, NULL, &id, 0, &position[0]);
			
			for (zend_hash_internal_pointer_reset_ex((*table), &position[1]);
				zend_hash_get_current_data_ex((*table), (void**)&brake, &position[1]) == SUCCESS;
				zend_hash_move_forward_ex((*table), &position[1])) {
				if (brake->id == id) {
					switch (brake->type) {
						case PHPDBG_BREAK_FILE: {
							fprintf(handle, 
								"break file %s:%lu\n", 
								((phpdbg_breakfile_t*)brake)->filename, 
								((phpdbg_breakfile_t*)brake)->line);
						} break;
						
						case PHPDBG_BREAK_SYM: {
							fprintf(handle, 
								"break func %s\n", 
								((phpdbg_breaksymbol_t*)brake)->symbol);
						} break;
						
						case PHPDBG_BREAK_METHOD: {
							fprintf(handle, 
								"break method %s::%s\n", 
								((phpdbg_breakmethod_t*)brake)->class_name, 
								((phpdbg_breakmethod_t*)brake)->func_name);
						} break;
						
						case PHPDBG_BREAK_OPCODE: {
							fprintf(handle, 
								"break op %s\n", 
								((phpdbg_breakop_t*)brake)->name);
						} break;
						
						case PHPDBG_BREAK_COND: {
							fprintf(handle, 
								"break on %s\n", 
								Z_STRVAL(((phpdbg_breakcond_t*)brake)->code));
						} break;
					}
				}
			}
		}
	}
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_file(const char *path, long line_num TSRMLS_DC) /* {{{ */
{
	struct stat sb;

	if (VCWD_STAT(path, &sb) != FAILURE) {
		if (sb.st_mode & (S_IFREG|S_IFLNK)) {
			HashTable *broken;
			phpdbg_breakfile_t new_break;
			size_t path_len = strlen(path);

			if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE],
				path, path_len, (void**)&broken) == FAILURE) {
				HashTable breaks;
				
				zend_hash_init(&breaks, 8, NULL, phpdbg_file_breaks_dtor, 0);

				zend_hash_update(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE],
					path, path_len, &breaks, sizeof(HashTable),
					(void**)&broken);
			}
			
			if (!zend_hash_index_exists(broken, line_num)) {
				PHPDBG_G(flags) |= PHPDBG_HAS_FILE_BP;
				
				new_break.filename = estrndup(path, path_len);
				new_break.line = line_num;
				new_break.id = PHPDBG_G(bp_count)++;
				new_break.type = PHPDBG_BREAK_FILE;
				
				zend_hash_index_update(
					broken, line_num, (void**)&new_break, sizeof(phpdbg_breakfile_t), NULL);
			
				phpdbg_notice("Breakpoint #%d added at %s:%ld",
					new_break.id, new_break.filename, new_break.line);
				
				PHPDBG_BREAK_MAPPING(new_break.id, broken);
			} else {
				phpdbg_error("Breakpoint at %s:%ld exists", path, line_num);
			}
			
		} else {
			phpdbg_error("Cannot set breakpoint in %s, it is not a regular file", path);
		}
	} else {
		phpdbg_error("Cannot stat %s, it does not exist", path);
	}
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_symbol(const char *name, size_t name_len TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], name, name_len)) {
		phpdbg_breaksymbol_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_SYM_BP;

		new_break.symbol = estrndup(name, name_len);
		new_break.id = PHPDBG_G(bp_count)++;
		new_break.type = PHPDBG_BREAK_SYM;
		
		zend_hash_update(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], new_break.symbol,
			name_len, &new_break, sizeof(phpdbg_breaksymbol_t), NULL);

		phpdbg_notice("Breakpoint #%d added at %s",
			new_break.id, new_break.symbol);
		
		PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	} else {
		phpdbg_notice("Breakpoint exists at %s", name);
	}
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_method(const char* class_name, const char* func_name TSRMLS_DC) /* {{{ */
{
	HashTable class_breaks, *class_table;
    size_t class_len = strlen(class_name);
    size_t func_len = strlen(func_name);
    char *lcname = zend_str_tolower_dup(func_name, func_len);	

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

		new_break.class_name = estrndup(class_name, class_len);
		new_break.class_len = class_len;
		new_break.func_name = estrndup(func_name, func_len);
		new_break.func_len = func_len;
		new_break.id = PHPDBG_G(bp_count)++;
		new_break.type = PHPDBG_BREAK_METHOD;
		
		zend_hash_update(class_table, lcname, func_len,
			&new_break, sizeof(phpdbg_breakmethod_t), NULL);

		phpdbg_notice("Breakpoint #%d added at %s::%s",
			new_break.id, class_name, func_name);
		
		PHPDBG_BREAK_MAPPING(new_break.id, class_table);
	} else {
		phpdbg_notice("Breakpoint exists at %s::%s", class_name, func_name);
    }
    
    efree(lcname);
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_opline(zend_ulong opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

		new_break.name = NULL;
		new_break.opline = opline;
		new_break.id = PHPDBG_G(bp_count)++;
		new_break.type = PHPDBG_BREAK_OPLINE;
		
		zend_hash_index_update(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline,
			&new_break, sizeof(phpdbg_breakline_t), NULL);

		phpdbg_notice("Breakpoint #%d added at %#lx",
			new_break.id, new_break.opline);
		PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	} else {
		phpdbg_notice("Breakpoint exists at %#lx", opline);
	}
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_opcode(const char *name, size_t name_len TSRMLS_DC) /* {{{ */
{
	phpdbg_breakop_t new_break;
	zend_ulong hash = zend_hash_func(name, name_len);

	if (zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], hash)) {
		phpdbg_notice(
			"Breakpoint exists for %s", name);
		return;
	}

	new_break.hash = hash;
	new_break.name = estrndup(name, name_len);
	new_break.id = PHPDBG_G(bp_count)++;
	new_break.type = PHPDBG_BREAK_OPCODE;

	zend_hash_index_update(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], hash,
		&new_break, sizeof(phpdbg_breakop_t), NULL);

	PHPDBG_G(flags) |= PHPDBG_HAS_OPCODE_BP;

	phpdbg_notice("Breakpoint #%d added at %s", new_break.id, name);
	PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE]);
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

		new_break.opline = (zend_ulong) opline;
		new_break.id = PHPDBG_G(bp_count)++;
		new_break.type = PHPDBG_BREAK_OPLINE;
		
		zend_hash_index_update(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE],
			(zend_ulong) opline, &new_break, sizeof(phpdbg_breakline_t), NULL);

		phpdbg_notice("Breakpoint #%d added at %#lx",
			new_break.id, new_break.opline);
		PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	}
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_expression(const char *expr, size_t expr_len TSRMLS_DC) /* {{{ */
{
	zend_ulong hash = zend_inline_hash_func(expr, expr_len);

	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], hash)) {
		phpdbg_breakcond_t new_break;
		zend_uint cops = CG(compiler_options);
		zval pv;

		ZVAL_STRINGL(&new_break.code, expr, expr_len, 1);

		new_break.hash = hash;
		new_break.id = PHPDBG_G(bp_count)++;
		new_break.type = PHPDBG_BREAK_COND;

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
			phpdbg_breakcond_t *brake;

			zend_hash_index_update(
				&PHPDBG_G(bp)[PHPDBG_BREAK_COND], hash, &new_break,
				sizeof(phpdbg_breakcond_t), (void**)&brake);

			phpdbg_notice("Conditional breakpoint #%d added %s/%p",
				brake->id, Z_STRVAL(brake->code), brake->ops);

			PHPDBG_G(flags) |= PHPDBG_HAS_COND_BP;
			PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
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
	HashTable *breaks;
	phpdbg_breakfile_t *brake;
	size_t name_len = strlen(op_array->filename);
		
	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], op_array->filename,
		name_len, (void**)&breaks) == FAILURE) {
		return FAILURE;
	}

	if (zend_hash_index_find(breaks, (*EG(opline_ptr))->lineno, (void**)&brake) == SUCCESS) {
		phpdbg_notice("Breakpoint #%d at %s:%ld",
			brake->id, brake->filename, brake->line);
		return SUCCESS;
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
		return phpdbg_find_breakpoint_method(ops TSRMLS_CC);
	}

	fname = ops->function_name;

	if (!fname) {
		fname = "main";
	}

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], fname, strlen(fname), (void**)&bp) == SUCCESS) {
		phpdbg_notice("Breakpoint #%d in %s() at %s:%u",
			bp->id, bp->symbol,
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

	if (zend_hash_find(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], ops->scope->name,
		ops->scope->name_length, (void**)&class_table) == SUCCESS) {
		char *lcname = zend_str_tolower_dup(ops->function_name, strlen(ops->function_name));
		size_t lcname_len = strlen(lcname);
		
		if (zend_hash_find(
		        class_table,
		        lcname,
		        lcname_len, (void**)&bp) == SUCCESS) {
			efree(lcname);
		    phpdbg_notice("Breakpoint #%d in %s::%s() at %s:%u",
		        bp->id, bp->class_name, bp->func_name,
			    zend_get_executed_filename(TSRMLS_C),
			    zend_get_executed_lineno(TSRMLS_C));
			return SUCCESS;
		}
		
		efree(lcname);
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

int phpdbg_find_breakpoint_opcode(zend_uchar opcode TSRMLS_DC) /* {{{ */
{
	phpdbg_breakop_t *bp;
	const char *opname = phpdbg_decode_opcode(opcode);

	if (memcmp(opname, PHPDBG_STRL("UNKNOWN")) == 0) {
		return FAILURE;
	}

	if (zend_hash_index_find(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE],
		zend_hash_func(opname, strlen(opname)), (void**)&bp) == SUCCESS) {
		phpdbg_notice("Breakpoint #%d in %s at %s:%u",
			bp->id,
			opname,
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
			zend_execute(EG(active_op_array) TSRMLS_CC);
			if (i_zend_is_true(retval)) {
				breakpoint = SUCCESS;
			}
		} zend_catch {
			phpdbg_error("Error detected while evaluating expression %s", Z_STRVAL(bp->code));
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

int phpdbg_find_breakpoint(zend_execute_data* execute_data TSRMLS_DC) /* {{{ */
{
	if (!(PHPDBG_G(flags) & PHPDBG_IS_BP_ENABLED)) {
		return FAILURE;
	}

	/* conditions cannot be executed by eval()'d code */
	if (!(PHPDBG_G(flags) & PHPDBG_IN_EVAL)
		&& (PHPDBG_G(flags) & PHPDBG_HAS_COND_BP)
		&& phpdbg_find_conditional_breakpoint(TSRMLS_C) == SUCCESS) {
		return SUCCESS;
	}

	if (PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP
		&& phpdbg_find_breakpoint_file(execute_data->op_array TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}

	if (PHPDBG_G(flags) & (PHPDBG_HAS_METHOD_BP|PHPDBG_HAS_SYM_BP)) {
		/* check we are at the beginning of the stack */
		if (execute_data->opline == EG(active_op_array)->opcodes) {
			if (phpdbg_find_breakpoint_symbol(
					execute_data->function_state.function TSRMLS_CC) == SUCCESS) {
				return SUCCESS;
			}
		}
	}

	if (PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP
		&& phpdbg_find_breakpoint_opline(execute_data->opline TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}

	if (PHPDBG_G(flags) & PHPDBG_HAS_OPCODE_BP
		&& phpdbg_find_breakpoint_opcode(execute_data->opline->opcode TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}

	return FAILURE;
} /* }}} */

PHPDBG_API void phpdbg_delete_breakpoint(zend_ulong num TSRMLS_DC) /* {{{ */
{
	HashTable **table;

	if (zend_hash_index_find(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], num, (void**)&table) == SUCCESS) {
		HashPosition position;
		phpdbg_breakbase_t *brake;
		
		for (zend_hash_internal_pointer_reset_ex((*table), &position);
			zend_hash_get_current_data_ex((*table), (void**)&brake, &position) == SUCCESS;
			zend_hash_move_forward_ex((*table), &position)) {
			char *key;
			zend_uint klen;
			zend_ulong idx;
			
			if (brake->id == num) {
				int type = brake->type;
				char *name = NULL;
				size_t name_len = 0L;
			
				switch (type) {
					case PHPDBG_BREAK_FILE:
					case PHPDBG_BREAK_METHOD:
						if (zend_hash_num_elements((*table)) == 1) {
							name = estrdup(brake->name);
							name_len = strlen(name);
							if (zend_hash_num_elements(&PHPDBG_G(bp)[type]) == 1) {
								PHPDBG_G(flags) &= ~(1<<(brake->type+1));
							}
						}
					break;
					
					default: {
						if (zend_hash_num_elements((*table)) == 1) {
							PHPDBG_G(flags) &= ~(1<<(brake->type+1));
						}
					}
				}
				
				switch (zend_hash_get_current_key_ex(
					(*table), &key, &klen, &idx, 0, &position)) {
					
					case HASH_KEY_IS_STRING:
						zend_hash_del((*table), key, klen);
					break;	
					
					default:
						zend_hash_index_del((*table), idx);
				}
				
				switch (type) {
					case PHPDBG_BREAK_FILE:
					case PHPDBG_BREAK_METHOD:
						if (name) {
							zend_hash_del(&PHPDBG_G(bp)[type], name, name_len);
							efree(name);
						}
					break;
				}
deleted:
				phpdbg_notice("Deleted breakpoint #%ld", num);
				PHPDBG_BREAK_UNMAPPING(num);
				return;
			}	
		}
		
		phpdbg_error("Failed to delete breakpoint #%ld", num);
	} else {
		phpdbg_error("Failed to find breakpoint #%ld", num);
	}
} /* }}} */

PHPDBG_API void phpdbg_clear_breakpoints(TSRMLS_D) /* {{{ */
{
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP]);
	
	PHPDBG_G(flags) &= ~PHPDBG_BP_MASK;

	PHPDBG_G(bp_count) = 0;
} /* }}} */

PHPDBG_API void phpdbg_print_breakpoints(zend_ulong type TSRMLS_DC) /* {{{ */
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
				phpdbg_writeln("#%d\t\t%s", brake->id, brake->symbol);
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

				if (zend_hash_get_current_key_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD],
					&class_name, &class_len, &class_idx, 0, &position[0]) == HASH_KEY_IS_STRING) {
					phpdbg_breakmethod_t *brake;

					for (zend_hash_internal_pointer_reset_ex(class_table, &position[1]);
					     zend_hash_get_current_data_ex(class_table, (void**)&brake, &position[1]) == SUCCESS;
					     zend_hash_move_forward_ex(class_table, &position[1])) {
						phpdbg_writeln("#%d\t\t%s::%s", brake->id, brake->class_name, brake->func_name);
					}
				}

			}
		} break;

		case PHPDBG_BREAK_FILE: if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP)) {
			HashPosition position[2];
			HashTable *points;
			
			phpdbg_writeln(SEPARATE);
			phpdbg_writeln("File Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position[0]);
			     zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], (void**) &points, &position[0]) == SUCCESS;
			     zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], &position[0])) {
				phpdbg_breakfile_t *brake;

				for (zend_hash_internal_pointer_reset_ex(points, &position[1]);
				     zend_hash_get_current_data_ex(points, (void**)&brake, &position[1]) == SUCCESS;
				     zend_hash_move_forward_ex(points, &position[1])) {
					phpdbg_writeln("#%d\t\t%s:%lu", brake->id, brake->filename, brake->line);
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

		case PHPDBG_BREAK_OPCODE: if (PHPDBG_G(flags) & PHPDBG_HAS_OPCODE_BP) {
			HashPosition position;
			phpdbg_breakop_t *brake;

			phpdbg_writeln(SEPARATE);
			phpdbg_writeln("Opcode Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], &position);
			     zend_hash_get_current_data_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], (void**) &brake, &position) == SUCCESS;
			     zend_hash_move_forward_ex(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], &position)) {
				phpdbg_writeln("#%d\t\t%s", brake->id, brake->name);
			}
		} break;
	}
} /* }}} */

