#ifndef PHP_SOAP_XML_H
#define PHP_SOAP_XML_H

#define get_attribute(node, name) get_attribute_ex(node, name, NULL)
#define get_node(node, name) get_node_ex(node, name, NULL)
#define get_node_recursive(node, name) get_node_recursive_ex(node, name, NULL)
#define get_node_with_attribute(node, name, attr, val) get_node_with_attribute_ex(node, name, NULL, attr, val, NULL)
#define get_node_with_attribute_recursive(node, name, attr, val) get_node_with_attribute_recursive_ex(node, name, NULL, attr, val, NULL)
#define attr_is_equal(node, name) attr_is_equal_ex(node, name, NULL)

xmlNsPtr attr_find_ns(xmlAttrPtr node);
xmlNsPtr node_find_ns(xmlNodePtr node);
int attr_is_equal_ex(xmlAttrPtr node, char *name, char *ns);
int node_is_equal_ex(xmlNodePtr node, char *name, char *ns);
xmlAttrPtr get_attribute_ex(xmlAttrPtr node,char *name, char *ns);
xmlNodePtr get_node_ex(xmlNodePtr node,char *name, char *ns);
xmlNodePtr get_node_recurisve_ex(xmlNodePtr node,char *name, char *ns);
xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns);
xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns);
int parse_namespace(char *inval,char **value,char **namespace);
xmlNodePtr check_and_resolve_href(xmlNodePtr data);

#endif
