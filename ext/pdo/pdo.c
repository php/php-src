/*
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pdo.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "pdo_arginfo.h"

zend_class_entry *pdo_dbh_ce, *pdo_dbstmt_ce, *pdo_row_ce;

/* for exceptional circumstances */
zend_class_entry *pdo_exception_ce;

/* True global resources - no need for thread safety here */

/* the registry of PDO drivers */
HashTable pdo_driver_hash;

/* we use persistent resources for the driver connection stuff */
static int le_ppdo;

int php_pdo_list_entry(void) /* {{{ */
{
	return le_ppdo;
}
/* }}} */

PDO_API zend_class_entry *php_pdo_get_dbh_ce(void) /* {{{ */
{
	return pdo_dbh_ce;
}
/* }}} */

PDO_API zend_class_entry *php_pdo_get_exception(void) /* {{{ */
{
	return pdo_exception_ce;
}
/* }}} */

PDO_API char *php_pdo_str_tolower_dup(const char *src, int len) /* {{{ */
{
	char *dest = emalloc(len + 1);
	zend_str_tolower_copy(dest, src, len);
	return dest;
}
/* }}} */

/* {{{ Return array of available PDO drivers */
PHP_FUNCTION(pdo_drivers)
{
	pdo_driver_t *pdriver;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	ZEND_HASH_FOREACH_PTR(&pdo_driver_hash, pdriver) {
		add_next_index_stringl(return_value, (char*)pdriver->driver_name, pdriver->driver_name_len);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ pdo_functions[] */
static const zend_module_dep pdo_deps[] = {
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_END
};
/* }}} */

/* {{{ pdo_module_entry */
zend_module_entry pdo_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_deps,
	"PDO",
	ext_functions,
	PHP_MINIT(pdo),
	PHP_MSHUTDOWN(pdo),
	NULL,
	NULL,
	PHP_MINFO(pdo),
	PHP_PDO_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* TODO: visit persistent handles: for each persistent statement handle,
 * remove bound parameter associations */

#ifdef COMPILE_DL_PDO
ZEND_GET_MODULE(pdo)
#endif

PDO_API int php_pdo_register_driver(const pdo_driver_t *driver) /* {{{ */
{
	if (driver->api_version != PDO_DRIVER_API) {
		zend_error(E_ERROR, "PDO: driver %s requires PDO API version " ZEND_ULONG_FMT "; this is PDO version %d",
			driver->driver_name, driver->api_version, PDO_DRIVER_API);
		return FAILURE;
	}
	if (!zend_hash_str_exists(&module_registry, "pdo", sizeof("pdo") - 1)) {
		zend_error(E_ERROR, "You MUST load PDO before loading any PDO drivers");
		return FAILURE;	/* NOTREACHED */
	}

	return zend_hash_str_add_ptr(&pdo_driver_hash, (char*)driver->driver_name, driver->driver_name_len, (void*)driver) != NULL ? SUCCESS : FAILURE;
}
/* }}} */

PDO_API void php_pdo_unregister_driver(const pdo_driver_t *driver) /* {{{ */
{
	if (!zend_hash_str_exists(&module_registry, "pdo", sizeof("pdo") - 1)) {
		return;
	}

	zend_hash_str_del(&pdo_driver_hash, (char*)driver->driver_name, driver->driver_name_len);
}
/* }}} */

pdo_driver_t *pdo_find_driver(const char *name, int namelen) /* {{{ */
{
	return zend_hash_str_find_ptr(&pdo_driver_hash, (char*)name, namelen);
}
/* }}} */

PDO_API int php_pdo_parse_data_source(const char *data_source, zend_ulong data_source_len, struct pdo_data_src_parser *parsed, int nparams) /* {{{ */
{
	zend_ulong i;
	int j;
	int valstart = -1;
	int semi = -1;
	int optstart = 0;
	int nlen;
	int n_matches = 0;
	int n_semicolumns = 0;

	i = 0;
	while (i < data_source_len) {
		/* looking for NAME= */

		if (data_source[i] == '\0') {
			break;
		}

		if (data_source[i] != '=') {
			++i;
			continue;
		}

		valstart = ++i;

		/* now we're looking for VALUE; or just VALUE<NUL> */
		semi = -1;
		n_semicolumns = 0;
		while (i < data_source_len) {
			if (data_source[i] == '\0') {
				semi = i++;
				break;
			}
			if (data_source[i] == ';') {
				if ((i + 1 >= data_source_len) || data_source[i+1] != ';') {
					semi = i++;
					break;
				} else {
					n_semicolumns++;
					i += 2;
					continue;
				}
			}
			++i;
		}

		if (semi == -1) {
			semi = i;
		}

		/* find the entry in the array */
		nlen = valstart - optstart - 1;
		for (j = 0; j < nparams; j++) {
			if (0 == strncmp(data_source + optstart, parsed[j].optname, nlen) && parsed[j].optname[nlen] == '\0') {
				/* got a match */
				if (parsed[j].freeme) {
					efree(parsed[j].optval);
				}

				if (n_semicolumns == 0) {
					parsed[j].optval = estrndup(data_source + valstart, semi - valstart - n_semicolumns);
				} else {
					int vlen = semi - valstart;
					const char *orig_val = data_source + valstart;
					char *new_val  = (char *) emalloc(vlen - n_semicolumns + 1);

					parsed[j].optval = new_val;

					while (vlen && *orig_val) {
						*new_val = *orig_val;
						new_val++;

						if (*orig_val == ';') {
							orig_val+=2;
							vlen-=2;
						} else {
							orig_val++;
							vlen--;
						}
					}
					*new_val = '\0';
				}

				parsed[j].freeme = 1;
				++n_matches;
				break;
			}
		}

		while (i < data_source_len && isspace(data_source[i])) {
			i++;
		}

		optstart = i;
	}

	return n_matches;
}
/* }}} */

static const char digit_vec[] = "0123456789";
PDO_API char *php_pdo_int64_to_str(pdo_int64_t i64) /* {{{ */
{
	char buffer[65];
	char outbuf[65] = "";
	register char *p;
	zend_long long_val;
	char *dst = outbuf;

	if (i64 < 0) {
		i64 = -i64;
		*dst++ = '-';
	}

	if (i64 == 0) {
		*dst++ = '0';
		*dst++ = '\0';
		return estrdup(outbuf);
	}

	p = &buffer[sizeof(buffer)-1];
	*p = '\0';

	while ((pdo_uint64_t)i64 > (pdo_uint64_t)ZEND_LONG_MAX) {
		pdo_uint64_t quo = (pdo_uint64_t)i64 / (unsigned int)10;
		unsigned int rem = (unsigned int)(i64 - quo*10U);
		*--p = digit_vec[rem];
		i64 = (pdo_int64_t)quo;
	}
	long_val = (zend_long)i64;
	while (long_val != 0) {
		zend_long quo = long_val / 10;
		*--p = digit_vec[(unsigned int)(long_val - quo * 10)];
		long_val = quo;
	}
	while ((*dst++ = *p++) != 0)
		;
	*dst = '\0';
	return estrdup(outbuf);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo)
{
	zend_class_entry ce;

	if (FAILURE == pdo_sqlstate_init_error_table()) {
		return FAILURE;
	}

	zend_hash_init(&pdo_driver_hash, 0, NULL, NULL, 1);

	le_ppdo = zend_register_list_destructors_ex(NULL, php_pdo_pdbh_dtor,
		"PDO persistent database", module_number);

	INIT_CLASS_ENTRY(ce, "PDOException", NULL);

	pdo_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);

	zend_declare_property_null(pdo_exception_ce, "errorInfo", sizeof("errorInfo")-1, ZEND_ACC_PUBLIC);

	pdo_dbh_init();
	pdo_stmt_init();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pdo)
{
	zend_hash_destroy(&pdo_driver_hash);
	pdo_sqlstate_fini_error_table();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdo)
{
	char *drivers = NULL, *ldrivers = estrdup("");
	pdo_driver_t *pdriver;

	php_info_print_table_start();
	php_info_print_table_header(2, "PDO support", "enabled");

	ZEND_HASH_FOREACH_PTR(&pdo_driver_hash, pdriver) {
		spprintf(&drivers, 0, "%s, %s", ldrivers, pdriver->driver_name);
		efree(ldrivers);
		ldrivers = drivers;
	} ZEND_HASH_FOREACH_END();

	php_info_print_table_row(2, "PDO drivers", drivers ? drivers + 2 : "");

	if (drivers) {
		efree(drivers);
	} else {
		efree(ldrivers);
	}

	php_info_print_table_end();

}
/* }}} */
