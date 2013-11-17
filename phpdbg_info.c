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

#include "php.h"
#include "phpdbg_utils.h"
#include "phpdbg_info.h"

PHPDBG_INFO(files) /* {{{ */
{
    HashPosition pos;
    char *fname;

	phpdbg_notice("Included files: %d",
		zend_hash_num_elements(&EG(included_files)));

	zend_hash_internal_pointer_reset_ex(&EG(included_files), &pos);
	while (zend_hash_get_current_key_ex(&EG(included_files), &fname,
		NULL, NULL, 0, &pos) == HASH_KEY_IS_STRING) {
		phpdbg_writeln("File: %s", fname);
		zend_hash_move_forward_ex(&EG(included_files), &pos);
	}

	return SUCCESS;
} /* }}} */

static inline void phpdbg_print_class_name(zend_class_entry **ce TSRMLS_DC) { /* {{{ */
	phpdbg_write(
		"%s %s %s (%d)",
		((*ce)->type == ZEND_USER_CLASS) ? 
			"User" : "Internal",
        ((*ce)->ce_flags & ZEND_ACC_INTERFACE) ? 
			"Interface" :
				((*ce)->ce_flags & ZEND_ACC_ABSTRACT) ?
				"Abstract Class" :
					"Class", 
		(*ce)->name, zend_hash_num_elements(&(*ce)->function_table));
} /* }}} */

PHPDBG_INFO(classes) /* {{{ */
{
    HashPosition position;
	zend_class_entry **ce;
	HashTable classes;
	
	zend_hash_init(&classes, 8, NULL, NULL, 0);

	for (zend_hash_internal_pointer_reset_ex(EG(class_table), &position);	
		zend_hash_get_current_data_ex(EG(class_table), (void**)&ce, &position) == SUCCESS;
		zend_hash_move_forward_ex(EG(class_table), &position)) {

        if ((*ce)->type == ZEND_USER_CLASS) {
        	zend_hash_next_index_insert(
        		&classes, ce, sizeof(ce), NULL);
        }
	}
	
	phpdbg_notice("User Classes (%d)",
		zend_hash_num_elements(&classes));

	for (zend_hash_internal_pointer_reset_ex(&classes, &position);	
		zend_hash_get_current_data_ex(&classes, (void**)&ce, &position) == SUCCESS;
		zend_hash_move_forward_ex(&classes, &position)) {

		phpdbg_print_class_name(ce TSRMLS_CC);
		phpdbg_writeln(EMPTY);
		
		if ((*ce)->parent) {
			zend_class_entry *pce = (*ce)->parent;
			do {
				phpdbg_write("|-------- ");
				phpdbg_print_class_name(&pce TSRMLS_CC);
				phpdbg_writeln(EMPTY);
			} while(pce = pce->parent);
		}
		
        if ((*ce)->info.user.filename) {
    		phpdbg_writeln(
    			"|---- in %s on line %lu", 
    			(*ce)->info.user.filename,
    			(*ce)->info.user.line_start);
    	} else phpdbg_writeln("|---- no source code");
	}

	zend_hash_destroy(&classes);
	
	return SUCCESS;
} /* }}} */


