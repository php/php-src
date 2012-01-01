/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
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

/* $Id$ */

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

static zend_class_entry *spl_ce_RuntimeException;

ZEND_DECLARE_MODULE_GLOBALS(pdo)
static PHP_GINIT_FUNCTION(pdo);

/* True global resources - no need for thread safety here */

/* the registry of PDO drivers */
HashTable pdo_driver_hash;

/* we use persistent resources for the driver connection stuff */
static int le_ppdo;

int php_pdo_list_entry(void)
{
	return le_ppdo;
}

/* for exceptional circumstances */
zend_class_entry *pdo_exception_ce;

PDO_API zend_class_entry *php_pdo_get_dbh_ce(void)
{
	return pdo_dbh_ce;
}

PDO_API zend_class_entry *php_pdo_get_exception(void)
{
	return pdo_exception_ce;
}

PDO_API char *php_pdo_str_tolower_dup(const char *src, int len)
{
	char *dest = emalloc(len + 1);
	zend_str_tolower_copy(dest, src, len);
	return dest;
}

PDO_API zend_class_entry *php_pdo_get_exception_base(int root TSRMLS_DC)
{
#if can_handle_soft_dependency_on_SPL && defined(HAVE_SPL) && ((PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1))
	if (!root) {
		if (!spl_ce_RuntimeException) {
			zend_class_entry **pce;

			if (zend_hash_find(CG(class_table), "runtimeexception", sizeof("RuntimeException"), (void **) &pce) == SUCCESS) {
				spl_ce_RuntimeException = *pce;
				return *pce;
			}
		} else {
			return spl_ce_RuntimeException;
		}
	}
#endif
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
	return zend_exception_get_default();
#else
	return zend_exception_get_default(TSRMLS_C);
#endif
}

zend_class_entry *pdo_dbh_ce, *pdo_dbstmt_ce, *pdo_row_ce;

/* {{{ proto array pdo_drivers()
 Return array of available PDO drivers */
PHP_FUNCTION(pdo_drivers)
{
	HashPosition pos;
	pdo_driver_t **pdriver;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	array_init(return_value);

	zend_hash_internal_pointer_reset_ex(&pdo_driver_hash, &pos);
	while (SUCCESS == zend_hash_get_current_data_ex(&pdo_driver_hash, (void**)&pdriver, &pos)) {
		add_next_index_stringl(return_value, (char*)(*pdriver)->driver_name, (*pdriver)->driver_name_len, 1);
		zend_hash_move_forward_ex(&pdo_driver_hash, &pos);
	}
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_pdo_drivers, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ pdo_functions[] */
const zend_function_entry pdo_functions[] = {
	PHP_FE(pdo_drivers,             arginfo_pdo_drivers)
	PHP_FE_END
};
/* }}} */

/* {{{ pdo_functions[] */
#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep pdo_deps[] = {
#ifdef HAVE_SPL
	ZEND_MOD_REQUIRED("spl")
#endif
	ZEND_MOD_END
};
#endif
/* }}} */

/* {{{ pdo_module_entry */
zend_module_entry pdo_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"PDO",
	pdo_functions,
	PHP_MINIT(pdo),
	PHP_MSHUTDOWN(pdo),
	NULL,
	NULL,
	PHP_MINFO(pdo),
	"1.0.4dev",
	PHP_MODULE_GLOBALS(pdo),
	PHP_GINIT(pdo),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/* TODO: visit persistent handles: for each persistent statement handle,
 * remove bound parameter associations */

#ifdef COMPILE_DL_PDO
ZEND_GET_MODULE(pdo)
#endif

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(pdo)
{
	pdo_globals->global_value = 0;
}
/* }}} */

PDO_API int php_pdo_register_driver(pdo_driver_t *driver)
{
	if (driver->api_version != PDO_DRIVER_API) {
		zend_error(E_ERROR, "PDO: driver %s requires PDO API version %ld; this is PDO version %d",
			driver->driver_name, driver->api_version, PDO_DRIVER_API);
		return FAILURE;
	}
	if (!zend_hash_exists(&module_registry, "pdo", sizeof("pdo"))) {
		zend_error(E_ERROR, "You MUST load PDO before loading any PDO drivers");
		return FAILURE;	/* NOTREACHED */
	}

	return zend_hash_add(&pdo_driver_hash, (char*)driver->driver_name, driver->driver_name_len,
			(void**)&driver, sizeof(pdo_driver_t *), NULL);
}

PDO_API void php_pdo_unregister_driver(pdo_driver_t *driver)
{
	if (!zend_hash_exists(&module_registry, "pdo", sizeof("pdo"))) {
		return;
	}

	zend_hash_del(&pdo_driver_hash, (char*)driver->driver_name, driver->driver_name_len);
}

pdo_driver_t *pdo_find_driver(const char *name, int namelen)
{
	pdo_driver_t **driver = NULL;
	
	zend_hash_find(&pdo_driver_hash, (char*)name, namelen, (void**)&driver);

	return driver ? *driver : NULL;
}

PDO_API int php_pdo_parse_data_source(const char *data_source,
		unsigned long data_source_len, struct pdo_data_src_parser *parsed,
		int nparams)
{
	int i, j;
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

static const char digit_vec[] = "0123456789";
PDO_API char *php_pdo_int64_to_str(pdo_int64_t i64 TSRMLS_DC)
{
	char buffer[65];
	char outbuf[65] = "";
	register char *p;
	long long_val;
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

	while ((pdo_uint64_t)i64 > (pdo_uint64_t)LONG_MAX) {
		pdo_uint64_t quo = (pdo_uint64_t)i64 / (unsigned int)10;
		unsigned int rem = (unsigned int)(i64 - quo*10U);
		*--p = digit_vec[rem];
		i64 = (pdo_int64_t)quo;
	}
	long_val = (long)i64;
	while (long_val != 0) {
		long quo = long_val / 10;
		*--p = digit_vec[(unsigned int)(long_val - quo * 10)];
		long_val = quo;
	}
	while ((*dst++ = *p++) != 0)
		;
	*dst = '\0';
	return estrdup(outbuf);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo)
{
	zend_class_entry ce;

	spl_ce_RuntimeException = NULL;

	if (FAILURE == pdo_sqlstate_init_error_table()) {
		return FAILURE;
	}

	zend_hash_init(&pdo_driver_hash, 0, NULL, NULL, 1);

	le_ppdo = zend_register_list_destructors_ex(NULL, php_pdo_pdbh_dtor,
		"PDO persistent database", module_number);

	INIT_CLASS_ENTRY(ce, "PDOException", NULL);

 	pdo_exception_ce = zend_register_internal_class_ex(&ce, php_pdo_get_exception_base(0 TSRMLS_CC), NULL TSRMLS_CC);

	zend_declare_property_null(pdo_exception_ce, "errorInfo", sizeof("errorInfo")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	pdo_dbh_init(TSRMLS_C);
	pdo_stmt_init(TSRMLS_C);

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
	HashPosition pos;
	char *drivers = NULL, *ldrivers = estrdup("");
	pdo_driver_t **pdriver;
	
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO support", "enabled");

	zend_hash_internal_pointer_reset_ex(&pdo_driver_hash, &pos);
	while (SUCCESS == zend_hash_get_current_data_ex(&pdo_driver_hash, (void**)&pdriver, &pos)) {
		spprintf(&drivers, 0, "%s, %s", ldrivers, (*pdriver)->driver_name);
		zend_hash_move_forward_ex(&pdo_driver_hash, &pos);
		efree(ldrivers);
		ldrivers = drivers;
	}
	
	php_info_print_table_row(2, "PDO drivers", drivers ? drivers+2 : "");

	if (drivers) {
		efree(drivers);
	} else {
		efree(ldrivers);
	}

	php_info_print_table_end();

}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
