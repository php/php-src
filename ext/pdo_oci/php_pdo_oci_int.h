/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <oci.h>

/* stuff we use in an OCI database handle */
typedef struct {
	OCIServer	*server;
	OCISession	*session;
	OCIEnv 		*env;
	OCIError	*err;
	OCISvcCtx 	*svc;
	/* OCI9; 0 == use NLS_LANG */
	ub2			charset;
	sword		last_err;

	unsigned attached:1;
	unsigned _reserved:31;
} pdo_oci_db_handle;

typedef struct {
	OCIDefine	*def;
	ub2			fetched_len;
	ub2			retcode;
	sb2			indicator;

	char *data;
	unsigned long datalen;

} pdo_oci_column;

typedef struct {
	pdo_oci_db_handle *H;
	OCIStmt		*stmt;
	OCIError	*err;
	sword		last_err;
	ub2			stmt_type;

	pdo_oci_column *cols;
} pdo_oci_stmt;

typedef struct {
	OCIBind 	*bind;	/* allocated by OCI */
	sb2			oci_type;
	sb2			indicator;
	ub2			retcode;

	ub4			actual_len;

	dvoid		*thing;	/* for LOBS, REFCURSORS etc. */
} pdo_oci_bound_param;

extern const ub4 PDO_OCI_INIT_MODE;
extern pdo_driver_t pdo_oci_driver;
extern OCIEnv *pdo_oci_Env;

ub4 _oci_error(OCIError *err, char *what, sword status, const char *file, int line TSRMLS_DC);
#define oci_error(e, w, s)	_oci_error(e, w, s, __FILE__, __LINE__ TSRMLS_CC)

ub4 oci_handle_error(pdo_dbh_t *dbh, pdo_oci_db_handle *H, ub4 errcode);

extern struct pdo_stmt_methods oci_stmt_methods;

