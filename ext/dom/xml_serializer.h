/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
