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
  | Author: Christian Stocker <chregu@php.net>                           |
  +----------------------------------------------------------------------+
*/

#ifndef XSL_FE_H
#define XSL_FE_H

extern const zend_function_entry php_xsl_xsltprocessor_class_functions[];
extern zend_class_entry *xsl_xsltprocessor_class_entry;

PHP_METHOD(XSLTProcessor, importStylesheet);
PHP_METHOD(XSLTProcessor, transformToDoc);
PHP_METHOD(XSLTProcessor, transformToUri);
PHP_METHOD(XSLTProcessor, transformToXml);
PHP_METHOD(XSLTProcessor, setParameter);
PHP_METHOD(XSLTProcessor, getParameter);
PHP_METHOD(XSLTProcessor, removeParameter);
PHP_METHOD(XSLTProcessor, hasExsltSupport);
PHP_METHOD(XSLTProcessor, registerPHPFunctions);
PHP_METHOD(XSLTProcessor, setProfiling);
PHP_METHOD(XSLTProcessor, setSecurityPrefs);
PHP_METHOD(XSLTProcessor, getSecurityPrefs);

#endif
