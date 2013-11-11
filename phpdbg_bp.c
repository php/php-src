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
	    "Breakpoint #%d added at %s:%d\n", new_break.id, new_break.filename, new_break.line);
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
	    
	    printf("Breakpoint #%d added at %s\n", new_break.id, new_break.symbol);
	} else {
	    printf("Breakpoint exists at %s\n", name);
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
			printf("Breakpoint #%d at %s:%ld\n", bp->id, bp->filename, bp->line);
			return SUCCESS;
		}
	}

	return FAILURE;
} /* }}} */

int phpdbg_find_breakpoint_symbol(zend_function *fbc TSRMLS_DC) /* {{{ */
{
	const char *fname;
	phpdbg_breaksymbol_t *bp;

	if (fbc->type != ZEND_USER_FUNCTION) {
		return FAILURE;
	}

	fname = ((zend_op_array*)fbc)->function_name;

	if (!fname) {
		fname = "main";
	}

	if (zend_hash_find(&PHPDBG_G(bp_symbols), fname, strlen(fname),
		(void**)&bp) == SUCCESS) {
		printf("Breakpoint #%d in %s() at %s\n", bp->id, bp->symbol,
			zend_get_executed_filename(TSRMLS_C));
		return SUCCESS;
	}

	return FAILURE;
} /* }}} */
