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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef XML_SERIALIZER_H
#define XML_SERIALIZER_H

#include <Zend/zend_types.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>
#include <libxml/xmlIO.h>

struct php_dom_private_data;
typedef struct php_dom_private_data php_dom_private_data;

int dom_xml_serialize(xmlSaveCtxtPtr ctx, xmlOutputBufferPtr out, xmlNodePtr node, bool format, bool require_well_formed, php_dom_private_data *private_data);

#endif
