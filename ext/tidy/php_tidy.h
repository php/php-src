/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: John Coggeshall <john@php.net>                               |
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

#define TIDY_METHOD_MAP(name, func_name, arg_types) \
	ZEND_NAMED_FE(name, ZEND_FN(func_name), arg_types)
#define TIDY_NODE_METHOD(name)    PHP_FUNCTION(tnm_ ##name)
#define TIDY_NODE_ME(name, param) TIDY_METHOD_MAP(name, tnm_ ##name, param)
#define TIDY_DOC_METHOD(name)     PHP_FUNCTION(tdm_ ##name)
#define TIDY_DOC_ME(name, param)  TIDY_METHOD_MAP(name, tdm_ ##name, param)
#define TIDY_ATTR_METHOD(name)    PHP_FUNCTION(tam_ ##name)
#define TIDY_ATTR_ME(name, param) TIDY_METHOD_MAP(name, tam_ ##name, param)

PHP_MINIT_FUNCTION(tidy);
PHP_MSHUTDOWN_FUNCTION(tidy);
PHP_RINIT_FUNCTION(tidy);
PHP_MINFO_FUNCTION(tidy);

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
#if HAVE_TIDYOPTGETDOC
PHP_FUNCTION(tidy_get_opt_doc);
#endif
PHP_FUNCTION(tidy_is_xhtml);
PHP_FUNCTION(tidy_is_xml);
PHP_FUNCTION(tidy_error_count);
PHP_FUNCTION(tidy_warning_count);
PHP_FUNCTION(tidy_access_count);
PHP_FUNCTION(tidy_config_count);

PHP_FUNCTION(ob_tidyhandler);

PHP_FUNCTION(tidy_get_root);
PHP_FUNCTION(tidy_get_html);
PHP_FUNCTION(tidy_get_head);
PHP_FUNCTION(tidy_get_body);

TIDY_DOC_METHOD(__construct);
TIDY_DOC_METHOD(parseFile);
TIDY_DOC_METHOD(parseString);

TIDY_NODE_METHOD(__construct);
TIDY_NODE_METHOD(hasChildren);
TIDY_NODE_METHOD(hasSiblings);
TIDY_NODE_METHOD(isComment);
TIDY_NODE_METHOD(isHtml);
TIDY_NODE_METHOD(isXhtml);
TIDY_NODE_METHOD(isXml);
TIDY_NODE_METHOD(isText);
TIDY_NODE_METHOD(isJste);
TIDY_NODE_METHOD(isAsp);
TIDY_NODE_METHOD(isPhp);

ZEND_BEGIN_MODULE_GLOBALS(tidy)
	char *default_config;
    zval *inst;
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
