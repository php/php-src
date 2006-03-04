/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: John Coggeshall  <john@php.net>                              |
  |         Ilia Alshanetsky <ilia@php.net>				 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_TIDY_H
#define PHP_TIDY_H

extern zend_module_entry tidy_module_entry;
#define phpext_tidy_ptr &tidy_module_entry

#ifdef PHP_WIN32
#define PHP_TIDY_API __declspec(dllexport)
#else
#define PHP_TIDY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "tidyenum.h"
#include "tidy.h"
#include "buffio.h"

#ifdef ZTS
#define TG(v) TSRMG(tidy_globals_id, zend_tidy_globals *, v)
#else
#define TG(v) (tidy_globals.v)
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define TIDY_RV_FALSE(__t) __t->type = IS_BOOL; __t->value.lval = FALSE
#define TIDY_RV_TRUE(__t) __t->type = IS_BOOL; __t->value.lval = TRUE

#define REMOVE_NEWLINE(_z) _z->value.str.val[_z->value.str.len-1] = '\0'; _z->value.str.len--;

struct _PHPTidyDoc {
    
    TidyDoc     doc;
    TidyBuffer  *errbuf;
    zend_bool   parsed;
};

typedef struct _PHPTidyDoc PHPTidyDoc;


PHP_MINIT_FUNCTION(tidy);
PHP_MSHUTDOWN_FUNCTION(tidy);
PHP_RINIT_FUNCTION(tidy);
PHP_RSHUTDOWN_FUNCTION(tidy);
PHP_MINFO_FUNCTION(tidy);

PHP_FUNCTION(tidy_setopt);
PHP_FUNCTION(tidy_getopt);
PHP_FUNCTION(tidy_parse_string);
PHP_FUNCTION(tidy_parse_file);
PHP_FUNCTION(tidy_clean_repair);
PHP_FUNCTION(tidy_repair_string);
PHP_FUNCTION(tidy_repair_file);
PHP_FUNCTION(tidy_diagnose);
PHP_FUNCTION(tidy_get_output);
PHP_FUNCTION(tidy_get_error_buffer);
PHP_FUNCTION(tidy_get_release);
PHP_FUNCTION(tidy_reset_config);
PHP_FUNCTION(tidy_get_config);
PHP_FUNCTION(tidy_get_status);
PHP_FUNCTION(tidy_get_html_ver);
PHP_FUNCTION(tidy_is_xhtml);
PHP_FUNCTION(tidy_is_xml);
PHP_FUNCTION(tidy_error_count);
PHP_FUNCTION(tidy_warning_count);
PHP_FUNCTION(tidy_access_count);
PHP_FUNCTION(tidy_config_count);
PHP_FUNCTION(tidy_load_config);
PHP_FUNCTION(tidy_load_config_enc);
PHP_FUNCTION(tidy_set_encoding);
PHP_FUNCTION(tidy_save_config);

/* resource dtor */
void dtor_TidyDoc(zend_rsrc_list_entry * TSRMLS_DC);

ZEND_BEGIN_MODULE_GLOBALS(tidy)
	PHPTidyDoc *tdoc;
	zend_bool used;
	char *default_config;
ZEND_END_MODULE_GLOBALS(tidy)

#ifdef ZTS
#define TG(v) TSRMG(tidy_globals_id, zend_tidy_globals *, v)
#else
#define TG(v) (tidy_globals.v)
#endif




#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
