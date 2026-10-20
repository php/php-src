/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_XML_H
#define PHP_XML_H

#ifdef HAVE_XML

extern zend_module_entry xml_module_entry;
#define xml_module_ptr &xml_module_entry

#include "php_version.h"
#define PHP_XML_VERSION PHP_VERSION

#include "expat_compat.h"

#ifdef XML_UNICODE
#error "UTF-16 Unicode support not implemented!"
#endif

#else
#define xml_module_ptr NULL
#endif /* HAVE_XML */

#define phpext_xml_ptr xml_module_ptr

enum php_xml_option {
	PHP_XML_OPTION_CASE_FOLDING = 1,
	PHP_XML_OPTION_TARGET_ENCODING,
	PHP_XML_OPTION_SKIP_TAGSTART,
	PHP_XML_OPTION_SKIP_WHITE,
	PHP_XML_OPTION_PARSE_HUGE,
};

#ifdef LIBXML_EXPAT_COMPAT
#define PHP_XML_SAX_IMPL "libxml"
#else
#define PHP_XML_SAX_IMPL "expat"
#endif

#endif /* PHP_XML_H */
