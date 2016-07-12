/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#ifndef PHP_SOAP_XML_H
#define PHP_SOAP_XML_H

#define get_attribute(node, name) get_attribute_ex(node, name, NULL)
#define get_node(node, name) get_node_ex(node, name, NULL)
#define get_node_recursive(node, name) get_node_recursive_ex(node, name, NULL)
#define get_node_with_attribute(node, name, attr, val) get_node_with_attribute_ex(node, name, NULL, attr, val, NULL)
#define get_node_with_attribute_recursive(node, name, attr, val) get_node_with_attribute_recursive_ex(node, name, NULL, attr, val, NULL)
#define node_is_equal(node, name) node_is_equal_ex(node, name, NULL)
#define attr_is_equal(node, name) attr_is_equal_ex(node, name, NULL)

xmlDocPtr soap_xmlParseFile(const char *filename);
xmlDocPtr soap_xmlParseMemory(const void *buf, size_t size);

xmlNsPtr attr_find_ns(xmlAttrPtr node);
xmlNsPtr node_find_ns(xmlNodePtr node);
int attr_is_equal_ex(xmlAttrPtr node, char *name, char *ns);
int node_is_equal_ex(xmlNodePtr node, char *name, char *ns);
xmlAttrPtr get_attribute_ex(xmlAttrPtr node,char *name, char *ns);
xmlNodePtr get_node_ex(xmlNodePtr node,char *name, char *ns);
xmlNodePtr get_node_recursive_ex(xmlNodePtr node,char *name, char *ns);
xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns);
xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns);
int parse_namespace(const xmlChar *inval,char **value,char **namespace);

#define FOREACHATTRNODE(n,c,i)      FOREACHATTRNODEEX(n,c,NULL,i)
#define FOREACHATTRNODEEX(n,c,ns,i) \
	do { \
		if (n == NULL) { \
			break; \
		} \
		if (c) { \
			i = get_attribute_ex(n,c,ns); \
		} else { \
			i = n; \
		} \
		if (i != NULL) { \
			n = i;

#define FOREACHNODE(n,c,i)      FOREACHNODEEX(n,c,NULL,i)
#define FOREACHNODEEX(n,c,ns,i) \
	do { \
		if (n == NULL) { \
			break; \
		} \
		if (c) { \
			i = get_node_ex(n,c,NULL); \
		} else { \
			i = n; \
		} \
		if(i != NULL) { \
			n = i;

#define ENDFOREACH(n) \
		} \
	} while ((n = n->next));

#endif
