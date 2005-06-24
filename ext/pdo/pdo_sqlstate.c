/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pdo.h"
#include "php_pdo_driver.h"

struct pdo_sqlstate_info {
	char state[6];
	const char *desc;
};

static HashTable err_hash;

static struct pdo_sqlstate_info err_initializer[] = {
	{ "00000",  "No error" },
	{ "01001",	"Cursor operation conflict" },
	{ "01002",	"Disconnect error" },
	{ "01003",	"NULL value eliminated in set function" },
	{ "01004",	"String data, right truncated" },
	{ "01006",	"Privilege not revoked" },
	{ "01007",	"Privilege not granted" },
	{ "01S00",	"Invalid connection string attribute" },
	{ "01S01",	"Error in row" },
	{ "01S02",	"Option value changed" },
	{ "01S06",	"Attempt to fetch before the result set returned the first rowset" },
	{ "01S07",	"Fractional truncation" },
	{ "01S08",	"Error saving File DSN" },
	{ "01S09",	"Invalid keyword" },
	{ "07002",	"COUNT field incorrect" },
	{ "07005",	"Prepared statement not a cursor-specification" },
	{ "07006",	"Restricted data type attribute violation" },
	{ "07009",	"Invalid descriptor index" },
	{ "07S01",	"Invalid use of default parameter" },
	{ "08001",	"Client unable to establish connection" },
	{ "08002",	"Connection name in use" },
	{ "08003",	"Connection does not exist" },
	{ "08004",	"Server rejected the connection" },
	{ "08007",	"Connection failure during transaction" },
	{ "08S01",	"Communication link failure" },
	{ "21S01",	"Insert value list does not match column list" },
	{ "21S02",	"Degree of derived table does not match column list" },
	{ "22001",	"String data, right truncated" },
	{ "22002",	"Indicator variable required but not supplied" },
	{ "22003",	"Numeric value out of range" },
	{ "22007",	"Invalid datetime format" },
	{ "22008",	"Datetime field overflow" },
	{ "22012",	"Division by zero" },
	{ "22015",	"Interval field overflow" },
	{ "22018",	"Invalid character value for cast specification" },
	{ "22019",	"Invalid escape character" },
	{ "22025",	"Invalid escape sequence" },
	{ "22026",	"String data, length mismatch" },
	{ "23000",	"Integrity constraint violation" },
	{ "24000",	"Invalid cursor state" },
	{ "25000",	"Invalid transaction state" },
	{ "25S01",	"Transaction state" },
	{ "25S02",	"Transaction is still active" },
	{ "25S03",	"Transaction is rolled back" },
	{ "28000",	"Invalid authorization specification" },
	{ "34000",	"Invalid cursor name" },
	{ "3C000",	"Duplicate cursor name" },
	{ "3D000",	"Invalid catalog name" },
	{ "3F000",	"Invalid schema name" },
	{ "40001",	"Serialization failure" },
	{ "40003",	"Statement completion unknown" },
	{ "42000",	"Syntax error or access violation" },
	{ "42S01",	"Base table or view already exists" },
	{ "42S02",	"Base table or view not found" },
	{ "42S11",	"Index already exists" },
	{ "42S12",	"Index not found" },
	{ "42S21",	"Column already exists" },
	{ "42S22",	"Column not found" },
	{ "44000",	"WITH CHECK OPTION violation" },
	{ "HY000",	"General error" },
	{ "HY001",	"Memory allocation error" },
	{ "HY003",	"Invalid application buffer type" },
	{ "HY004",	"Invalid SQL data type" },
	{ "HY007",	"Associated statement is not prepared" },
	{ "HY008",	"Operation canceled" },
	{ "HY009",	"Invalid use of null pointer" },
	{ "HY010",	"Function sequence error" },
	{ "HY011",	"Attribute cannot be set now" },
	{ "HY012",	"Invalid transaction operation code" },
	{ "HY013",	"Memory management error" },
	{ "HY014",	"Limit on the number of handles exceeded" },
	{ "HY015",	"No cursor name available" },
	{ "HY016",	"Cannot modify an implementation row descriptor" },
	{ "HY017",	"Invalid use of an automatically allocated descriptor handle" },
	{ "HY018",	"Server declined cancel request" },
	{ "HY019",	"Non-character and non-binary data sent in pieces" },
	{ "HY020",	"Attempt to concatenate a null value" },
	{ "HY021",	"Inconsistent descriptor information" },
	{ "HY024",	"Invalid attribute value" },
	{ "HY090",	"Invalid string or buffer length" },
	{ "HY091",	"Invalid descriptor field identifier" },
	{ "HY092",	"Invalid attribute/option identifier" },
	{ "HY093",	"Invalid parameter number" },
	{ "HY095",	"Function type out of range" },
	{ "HY096",	"Invalid information type" },
	{ "HY097",	"Column type out of range" },
	{ "HY098",	"Scope type out of range" },
	{ "HY099",	"Nullable type out of range" },
	{ "HY100",	"Uniqueness option type out of range" },
	{ "HY101",	"Accuracy option type out of range" },
	{ "HY103",	"Invalid retrieval code" },
	{ "HY104",	"Invalid precision or scale value" },
	{ "HY105",	"Invalid parameter type" },
	{ "HY106",	"Fetch type out of range" },
	{ "HY107",	"Row value out of range" },
	{ "HY109",	"Invalid cursor position" },
	{ "HY110",	"Invalid driver completion" },
	{ "HY111",	"Invalid bookmark value" },
	{ "HYC00",	"Optional feature not implemented" },
	{ "HYT00",	"Timeout expired" },
	{ "HYT01",	"Connection timeout expired" },
	{ "IM001",	"Driver does not support this function" },
	{ "IM002",	"Data source name not found and no default driver specified" },
	{ "IM003",	"Specified driver could not be loaded" },
	{ "IM004",	"Driver's SQLAllocHandle on SQL_HANDLE_ENV failed" },
	{ "IM005",	"Driver's SQLAllocHandle on SQL_HANDLE_DBC failed" },
	{ "IM006",	"Driver's SQLSetConnectAttr failed" },
	{ "IM007",	"No data source or driver specified; dialog prohibited" },
	{ "IM008",	"Dialog failed" },
	{ "IM009",	"Unable to load translation DLL" },
	{ "IM010",	"Data source name too long" },
	{ "IM011",	"Driver name too long" },
	{ "IM012",	"DRIVER keyword syntax error" },
	{ "IM013",	"Trace file error" },
	{ "IM014",	"Invalid name of File DSN" },
	{ "IM015",	"Corrupt file data source" },
	{ "PDDRV",	"Nested unbuffered query, refer to driver error for details" }
};

void pdo_sqlstate_fini_error_table(void)
{
	zend_hash_destroy(&err_hash);
}

int pdo_sqlstate_init_error_table(void)
{
	int i;
	struct pdo_sqlstate_info *info;

	if (FAILURE == zend_hash_init(&err_hash,
			sizeof(err_initializer)/sizeof(err_initializer[0]), NULL, NULL, 1)) {
		return FAILURE;
	}

	for (i = 0; i < sizeof(err_initializer)/sizeof(err_initializer[0]); i++) {
		info = &err_initializer[i];

		zend_hash_add(&err_hash, info->state, sizeof(info->state), &info, sizeof(info), NULL);
	}

	return SUCCESS;
}

const char *pdo_sqlstate_state_to_description(char *state)
{
	struct pdo_sqlstate_info **info;
	if (SUCCESS == zend_hash_find(&err_hash, state, sizeof(err_initializer[0].state),
			(void**)&info)) {
		return (*info)->desc;
	}
	return NULL;
}

