#ifndef PHP_OVRIMOS_H
#define PHP_OVRIMOS_H

#if HAVE_LIBSQLCLI

#if PHP_API_VERSION < 19990421
#define  zend_module_entry zend_module_entry
#include "zend_modules.h"
#include "internal_functions.h"
#endif

extern zend_module_entry ovrimos_module_entry;
#define ovrimos_module_ptr &ovrimos_module_entry

PHP_FUNCTION(ovrimos_connect);
PHP_FUNCTION(ovrimos_close);
PHP_FUNCTION(ovrimos_longreadlen);
PHP_FUNCTION(ovrimos_prepare);
PHP_FUNCTION(ovrimos_execute);
PHP_FUNCTION(ovrimos_cursor);
PHP_FUNCTION(ovrimos_exec);
PHP_FUNCTION(ovrimos_fetch_into);
PHP_FUNCTION(ovrimos_fetch_row);
PHP_FUNCTION(ovrimos_result);
PHP_FUNCTION(ovrimos_result_all);
PHP_FUNCTION(ovrimos_free_result);
PHP_FUNCTION(ovrimos_num_rows);
PHP_FUNCTION(ovrimos_num_fields);
PHP_FUNCTION(ovrimos_field_name);
PHP_FUNCTION(ovrimos_field_type);
PHP_FUNCTION(ovrimos_field_len);
PHP_FUNCTION(ovrimos_field_num);
/*PHP_FUNCTION(ovrimos_autocommit);*/
PHP_FUNCTION(ovrimos_commit);
PHP_FUNCTION(ovrimos_rollback);
/*PHP_FUNCTION(ovrimos_setoption);*/

#else
#define ovrimos_module_ptr NULL
#endif

#define phpext_ovrimos_ptr ovrimos_module_ptr

#endif
