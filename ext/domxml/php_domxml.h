/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Uwe Steinmann (Uwe.Steinmann@fernuni-hagen.de               |
   |                                                                      |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef PHP_DOMXML_H
#define PHP_DOMXML_H

#if HAVE_DOMXML
#include <libxml/parser.h>

extern zend_module_entry php_domxml_module_entry;
#define php_domxml_module_ptr &php_domxml_module_entry

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
PHP_FUNCTION(domxml_getattr);
PHP_FUNCTION(domxml_setattr);
PHP_FUNCTION(domxml_children);
PHP_FUNCTION(domxml_lastchild);
PHP_FUNCTION(domxml_parent);
PHP_FUNCTION(domxml_node);
PHP_FUNCTION(domxml_new_child);

/* Class Attribute methods */
PHP_FUNCTION(domxml_attrname);

PHP_FUNCTION(domxml_test);
#else
#define php_domxml_module_ptr NULL
#endif /* HAVE_DOMXML */
#define phpext_domxml_ptr php_domxml_module_ptr

#endif /* _PHP_DIR_H */
