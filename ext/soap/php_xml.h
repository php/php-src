#ifndef PHP_SOAP_XML_H
#define PHP_SOAP_XML_H

#define get_attribute(node, name) get_attribute_ex(node, name, NULL)
#define get_node(node, name) get_node_ex(node, name, NULL)
#define get_node_recursive(node, name) get_node_recursive_ex(node, name, NULL)
#define get_node_with_attribute(node, name, attr, val) get_node_with_attribute_ex(node, name, NULL, attr, val, NULL)
#define get_node_with_attribute_recursive(node, name, attr, val) get_node_with_attribute_recursive_ex(node, name, NULL, attr, val, NULL)
#define node_is_equal(node, name) node_is_equal_ex(node, name, NULL)
#define attr_is_equal(node, name) attr_is_equal_ex(node, name, NULL)

xmlNsPtr attr_find_ns(xmlAttrPtr node);
xmlNsPtr node_find_ns(xmlNodePtr node);
int attr_is_equal_ex(xmlAttrPtr node, char *name, char *ns);
int node_is_equal_ex(xmlNodePtr node, char *name, char *ns);
xmlAttrPtr get_attribute_ex(xmlAttrPtr node,char *name, char *ns);
xmlNodePtr get_node_ex(xmlNodePtr node,char *name, char *ns);
xmlNodePtr get_node_recursive_ex(xmlNodePtr node,char *name, char *ns);
xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns);
xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns);
int parse_namespace(const char *inval,char **value,char **namespace);

int php_stream_xmlIO_match_wrapper(const char *filename);
void *php_stream_xmlIO_open_wrapper(const char *filename);
int php_stream_xmlIO_read(void *context, char *buffer, int len);
int php_stream_xmlIO_close(void *context);

#define FOREACHATTRNODE(n,c,i) \
	do { \
		if (n == NULL) { \
			break; \
		} \
		if (c) { \
			i = get_attribute(n,c); \
		} else { \
			i = n; \
		} \
		if (i != NULL) { \
			n = i;

#define FOREACHNODE(n,c,i) \
	do { \
		if (n == NULL) { \
			break; \
		} \
		if (c) { \
			i = get_node(n,c); \
		} else { \
			i = n; \
		} \
		if(i != NULL) { \
			n = i;

#define ENDFOREACH(n) \
		} \
	} while ((n = n->next));

#endif
