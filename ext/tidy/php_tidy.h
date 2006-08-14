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

static PHP_MINIT_FUNCTION(tidy);
static PHP_MSHUTDOWN_FUNCTION(tidy);
static PHP_RINIT_FUNCTION(tidy);
static PHP_MINFO_FUNCTION(tidy);

static PHP_FUNCTION(tidy_getopt);
static PHP_FUNCTION(tidy_parse_string);
static PHP_FUNCTION(tidy_parse_file);
static PHP_FUNCTION(tidy_clean_repair);
static PHP_FUNCTION(tidy_repair_string);
static PHP_FUNCTION(tidy_repair_file);
static PHP_FUNCTION(tidy_diagnose);
static PHP_FUNCTION(tidy_get_output);
static PHP_FUNCTION(tidy_get_error_buffer);
static PHP_FUNCTION(tidy_get_release);
static PHP_FUNCTION(tidy_reset_config);
static PHP_FUNCTION(tidy_get_config);
static PHP_FUNCTION(tidy_get_status);
static PHP_FUNCTION(tidy_get_html_ver);
#if HAVE_TIDYOPTGETDOC
static PHP_FUNCTION(tidy_get_opt_doc);
#endif
static PHP_FUNCTION(tidy_is_xhtml);
static PHP_FUNCTION(tidy_is_xml);
static PHP_FUNCTION(tidy_error_count);
static PHP_FUNCTION(tidy_warning_count);
static PHP_FUNCTION(tidy_access_count);
static PHP_FUNCTION(tidy_config_count);

static PHP_FUNCTION(ob_tidyhandler);

static PHP_FUNCTION(tidy_get_root);
static PHP_FUNCTION(tidy_get_html);
static PHP_FUNCTION(tidy_get_head);
static PHP_FUNCTION(tidy_get_body);

static TIDY_DOC_METHOD(__construct);
static TIDY_DOC_METHOD(parseFile);
static TIDY_DOC_METHOD(parseString);

static TIDY_NODE_METHOD(__construct);
static TIDY_NODE_METHOD(hasChildren);
static TIDY_NODE_METHOD(hasSiblings);
static TIDY_NODE_METHOD(isComment);
static TIDY_NODE_METHOD(isHtml);
static TIDY_NODE_METHOD(isXhtml);
static TIDY_NODE_METHOD(isXml);
static TIDY_NODE_METHOD(isText);
static TIDY_NODE_METHOD(isJste);
static TIDY_NODE_METHOD(isAsp);
static TIDY_NODE_METHOD(isPhp);

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
