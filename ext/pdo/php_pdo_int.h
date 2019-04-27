/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
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
extern zend_class_entry *pdo_exception_ce;
PDO_API zend_class_entry *php_pdo_get_exception_base(int root);
int php_pdo_list_entry(void);

void pdo_dbh_init(void);
void pdo_stmt_init(void);

extern zend_object *pdo_dbh_new(zend_class_entry *ce);
extern const zend_function_entry pdo_dbh_functions[];
extern zend_class_entry *pdo_dbh_ce;
extern ZEND_RSRC_DTOR_FUNC(php_pdo_pdbh_dtor);

extern zend_object *pdo_dbstmt_new(zend_class_entry *ce);
extern const zend_function_entry pdo_dbstmt_functions[];
extern zend_class_entry *pdo_dbstmt_ce;
void pdo_dbstmt_free_storage(zend_object *std);
zend_object_iterator *pdo_stmt_iter_get(zend_class_entry *ce, zval *object, int by_ref);
extern zend_object_handlers pdo_dbstmt_object_handlers;
int pdo_stmt_describe_columns(pdo_stmt_t *stmt);
int pdo_stmt_setup_fetch_mode(INTERNAL_FUNCTION_PARAMETERS, pdo_stmt_t *stmt, int skip_first_arg);

extern zend_object *pdo_row_new(zend_class_entry *ce);
extern const zend_function_entry pdo_row_functions[];
extern zend_class_entry *pdo_row_ce;
void pdo_row_free_storage(zend_object *std);
extern zend_object_handlers pdo_row_object_handlers;

zend_object_iterator *php_pdo_dbstmt_iter_get(zend_class_entry *ce, zval *object);

extern pdo_driver_t *pdo_find_driver(const char *name, int namelen);

int pdo_sqlstate_init_error_table(void);
void pdo_sqlstate_fini_error_table(void);
const char *pdo_sqlstate_state_to_description(char *state);
int pdo_hash_methods(pdo_dbh_object_t *dbh, int kind);
