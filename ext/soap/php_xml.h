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
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

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

xmlNsPtr node_find_ns(xmlNodePtr node);
int attr_is_equal_ex(xmlAttrPtr node, const char *name, const char *ns);
int node_is_equal_ex(xmlNodePtr node, const char *name, const char *ns);
int node_is_equal_ex_one_of(xmlNodePtr node, const char *name, const char *const *namespaces);
xmlAttrPtr get_attribute_any_ns(xmlAttrPtr node, const char *name);
xmlAttrPtr get_attribute_ex(xmlAttrPtr node, const char *name, const char *ns);
xmlNodePtr get_node_ex(xmlNodePtr node, const char *name, const char *ns);
xmlNodePtr get_node_recursive_ex(xmlNodePtr node, const char *name, const char *ns);
xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, const char *name, const char *name_ns, const char *attribute, const char *value, const char *attr_ns);
xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, const char *name, const char *name_ns, const char *attribute, const char *value, const char *attr_ns);
void parse_namespace(const xmlChar *inval, const char **value, char **namespace);

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
