/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Marcus Boerger <helly@php.net>                               |
  |         Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* Stuff private to the PDO extension and not for consumption by PDO drivers
 * */

#include "php_pdo_error.h"

extern HashTable pdo_driver_hash;
extern HashTable pdo_driver_specific_ce_hash;
extern zend_class_entry *pdo_exception_ce;
int php_pdo_list_entry(void);

void pdo_dbh_init(int module_number);
void pdo_stmt_init(void);

extern const zend_function_entry pdo_dbh_functions[];
extern ZEND_RSRC_DTOR_FUNC(php_pdo_pdbh_dtor);

extern zend_object *pdo_dbstmt_new(zend_class_entry *ce);
extern const zend_function_entry pdo_dbstmt_functions[];
extern zend_class_entry *pdo_dbstmt_ce;
void pdo_dbstmt_free_storage(zend_object *std);
zend_object_iterator *pdo_stmt_iter_get(zend_class_entry *ce, zval *object, int by_ref);
extern zend_object_handlers pdo_dbstmt_object_handlers;
bool pdo_stmt_describe_columns(pdo_stmt_t *stmt);
bool pdo_stmt_setup_fetch_mode(pdo_stmt_t *stmt, zend_long mode, uint32_t mode_arg_num,
	zval *args, uint32_t variadic_num_args);

extern const zend_function_entry pdo_row_functions[];
extern zend_class_entry *pdo_row_ce;
extern zend_object_handlers pdo_row_object_handlers;

zend_object_iterator *php_pdo_dbstmt_iter_get(zend_class_entry *ce, zval *object);

extern pdo_driver_t *pdo_find_driver(const char *name, int namelen);

void pdo_sqlstate_init_error_table(void);
void pdo_sqlstate_fini_error_table(void);
const char *pdo_sqlstate_state_to_description(char *state);
bool pdo_hash_methods(pdo_dbh_object_t *dbh, int kind);
