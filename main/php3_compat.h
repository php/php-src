/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP3_COMPAT_H
#define PHP3_COMPAT_H

typedef zval pval;

#define pval_copy_constructor zval_copy_ctor
#define pval_destructor	zval_dtor

#define _php3_hash_init zend_hash_init
#define _php3_hash_destroy zend_hash_destroy

#define _php3_hash_clean zend_hash_clean

#define _php3_hash_add_or_update zend_hash_add_or_update
#define _php3_hash_add zend_hash_add
#define _php3_hash_update zend_hash_update

#define _php3_hash_quick_add_or_update zend_hash_quick_add_or_update
#define _php3_hash_quick_add zend_hash_quick_add
#define _php3_hash_quick_update zend_hash_quick_update

#define _php3_hash_index_update_or_next_insert zend_hash_index_update_or_next_insert
#define _php3_hash_index_update zend_hash_index_update
#define _php3_hash_next_index_insert zend_hash_next_index_insert

#define _php3_hash_pointer_update zend_hash_pointer_update

#define _php3_hash_pointer_index_update_or_next_insert zend_hash_pointer_index_update_or_next_insert
#define _php3_hash_pointer_index_update zend_hash_pointer_index_update
#define _php3_hash_next_index_pointer_update zend_hash_next_index_pointer_update
#define _php3_hash_next_index_pointer_insert zend_hash_next_index_pointer_insert

#define _php3_hash_del_key_or_index zend_hash_del_key_or_index
#define _php3_hash_del zend_hash_del
#define _php3_hash_index_del zend_hash_index_del

#define _php3_hash_find zend_hash_find
#define _php3_hash_quick_find zend_hash_quick_find
#define _php3_hash_index_find zend_hash_index_find

#define _php3_hash_exists zend_hash_exists
#define _php3_hash_index_exists zend_hash_index_exists
#define _php3_hash_is_pointer zend_hash_is_pointer
#define _php3_hash_index_is_pointer zend_hash_index_is_pointer
#define _php3_hash_next_free_element zend_hash_next_free_element

#define _php3_hash_move_forward zend_hash_move_forward
#define _php3_hash_move_backwards zend_hash_move_backwards
#define _php3_hash_get_current_key zend_hash_get_current_key
#define _php3_hash_get_current_data zend_hash_get_current_data
#define _php3_hash_internal_pointer_reset zend_hash_internal_pointer_reset
#define _php3_hash_internal_pointer_end zend_hash_internal_pointer_end

#define _php3_hash_copy zend_hash_copy
#define _php3_hash_merge zend_hash_merge
#define _php3_hash_sort zend_hash_sort
#define _php3_hash_minmax zend_hash_minmax

#define _php3_hash_num_elements zend_hash_num_elements

#define _php3_hash_apply zend_hash_apply
#define _php3_hash_apply_with_argument zend_hash_apply_with_argument


#define php3_error php_error

#define php3_printf php_printf
#define _php3_sprintf php_sprintf



#define php3_module_entry zend_module_entry

#define php3_strndup		zend_strndup
#define php3_str_tolower	zend_str_tolower
#define php3_binary_strcmp	zend_binary_strcmp


#define php3_list_insert	zend_list_insert
#define php3_list_find		zend_list_find
#define php3_list_delete	zend_list_delete

#define php3_plist_insert	zend_plist_insert
#define php3_plist_find		zend_plist_find
#define php3_plist_delete	zend_plist_delete

#define zend_print_pval		zend_print_zval
#define zend_print_pval_r	zend_print_zval_r


#define function_entry		zend_function_entry

#define _php3_addslashes	php_addslashes
#define _php3_stripslashes	php_stripslashes
#define php3_dl             php_dl

#define getParameters		zend_get_parameters
#define getParametersArray	zend_get_parameters_array

#define list_entry			zend_rsrc_list_entry

#endif							/* PHP3_COMPAT_H */
