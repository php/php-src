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
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_ERROR_H
#define PHP_PDO_ERROR_H

#include "php_pdo_driver.h"

PDO_API void pdo_handle_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt);

#define PDO_DBH_CLEAR_ERR()             do { \
	ZEND_ASSERT(sizeof(dbh->error_code) == sizeof(PDO_ERR_NONE)); \
	memcpy(dbh->error_code, PDO_ERR_NONE, sizeof(PDO_ERR_NONE)); \
	if (dbh->query_stmt) { \
		dbh->query_stmt = NULL; \
		OBJ_RELEASE(dbh->query_stmt_obj); \
		dbh->query_stmt_obj = NULL; \
	} \
} while (0)
#define PDO_STMT_CLEAR_ERR() do { \
	ZEND_ASSERT(sizeof(stmt->error_code) == sizeof(PDO_ERR_NONE)); \
	memcpy(stmt->error_code, PDO_ERR_NONE, sizeof(PDO_ERR_NONE)); \
} while (0)
#define PDO_HANDLE_DBH_ERR()    if (strcmp(dbh->error_code, PDO_ERR_NONE)) { pdo_handle_error(dbh, NULL); }
#define PDO_HANDLE_STMT_ERR_EX(cleanup_instruction)   if (strcmp(stmt->error_code, PDO_ERR_NONE) != 0) {  cleanup_instruction pdo_handle_error(stmt->dbh, stmt); }
#define PDO_HANDLE_STMT_ERR() PDO_HANDLE_STMT_ERR_EX(;)

#endif /* PHP_PDO_ERROR_H */
