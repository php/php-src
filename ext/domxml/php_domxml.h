/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Uwe Steinmann (Uwe.Steinmann@fernuni-hagen.de               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_DOMXML_H
#define PHP_DOMXML_H

#if HAVE_DOMXML
#include <libxml/parser.h>
#if defined(LIBXML_XPATH_ENABLED)
#include <libxml/xpath.h>
#endif
#if defined(LIBXML_XPTR_ENABLED)
#include <libxml/xpointer.h>
#endif

extern zend_module_entry domxml_module_entry;
#define domxml_module_ptr &domxml_module_entry

/* directory functions */
extern PHP_MINIT_FUNCTION(domxml);
extern PHP_MINFO_FUNCTION(domxml);
PHP_FUNCTION(xmldoc);
PHP_FUNCTION(xmldocfile);
PHP_FUNCTION(xmltree);
PHP_FUNCTION(domxml_new_xmldoc);

/* Class Document methods */
PHP_FUNCTION(domxml_root);
PHP_FUNCTION(domxml_add_root);
PHP_FUNCTION(domxml_intdtd);
PHP_FUNCTION(domxml_dumpmem);

/* Class Node methods */
PHP_FUNCTION(domxml_attributes);
PHP_FUNCTION(domxml_get_attribute);
PHP_FUNCTION(domxml_set_attribute);
PHP_FUNCTION(domxml_children);
PHP_FUNCTION(domxml_last_child);
PHP_FUNCTION(domxml_parent);
PHP_FUNCTION(domxml_node);
PHP_FUNCTION(domxml_unlink_node);
PHP_FUNCTION(domxml_new_child);
PHP_FUNCTION(domxml_set_content);

/* Class Attribute methods */
PHP_FUNCTION(domxml_attrname);

/* Class XPathContext methods */
#if defined(LIBXML_XPATH_ENABLED)
PHP_FUNCTION(xpath_init);
PHP_FUNCTION(xpath_new_context);
PHP_FUNCTION(xpath_eval);
PHP_FUNCTION(xpath_eval_expression);
#endif
#if defined(LIBXML_XPTR_ENABLED)
PHP_FUNCTION(xptr_new_context);
PHP_FUNCTION(xptr_eval);
#endif

PHP_FUNCTION(domxml_test);
#else
#define domxml_module_ptr NULL
#endif /* HAVE_DOMXML */
#define phpext_domxml_ptr domxml_module_ptr

#endif /* _PHP_DIR_H */
