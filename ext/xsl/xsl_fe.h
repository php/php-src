/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Christian Stocker <chregu@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef XSL_FE_H
#define XSL_FE_H

extern const zend_function_entry php_xsl_xsltprocessor_class_functions[];
extern zend_class_entry *xsl_xsltprocessor_class_entry;

PHP_FUNCTION(xsl_xsltprocessor_import_stylesheet);
PHP_FUNCTION(xsl_xsltprocessor_transform_to_doc);
PHP_FUNCTION(xsl_xsltprocessor_transform_to_uri);
PHP_FUNCTION(xsl_xsltprocessor_transform_to_xml);
PHP_FUNCTION(xsl_xsltprocessor_set_parameter);
PHP_FUNCTION(xsl_xsltprocessor_get_parameter);
PHP_FUNCTION(xsl_xsltprocessor_remove_parameter);
PHP_FUNCTION(xsl_xsltprocessor_has_exslt_support);
PHP_FUNCTION(xsl_xsltprocessor_register_php_functions);
PHP_FUNCTION(xsl_xsltprocessor_set_profiling);
PHP_FUNCTION(xsl_xsltprocessor_set_security_prefs);
PHP_FUNCTION(xsl_xsltprocessor_get_security_prefs);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
