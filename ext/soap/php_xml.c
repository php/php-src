#include "php_soap.h"

/* Channel libxml file io layer through the PHP streams subsystem.
 * This allows use of ftps:// and https:// urls */

int php_stream_xmlIO_match_wrapper(const char *filename)
{
	TSRMLS_FETCH();
	return php_stream_locate_url_wrapper(filename, NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) ? 1 : 0;
}

void *php_stream_xmlIO_open_wrapper(const char *filename)
{
	TSRMLS_FETCH();
	return php_stream_open_wrapper((char*)filename, "rb", ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);
}

int php_stream_xmlIO_read(void *context, char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_read((php_stream*)context, buffer, len);
}

int php_stream_xmlIO_close(void *context)
{
	TSRMLS_FETCH();
	return php_stream_close((php_stream*)context);
}

xmlNsPtr attr_find_ns(xmlAttrPtr node)
{
	if (node->ns) {
		return node->ns;
	} else if (node->parent->ns) {
		return node->parent->ns;
	} else {
		return xmlSearchNs(node->doc, node->parent, NULL);
	}
}

xmlNsPtr node_find_ns(xmlNodePtr node)
{
	if (node->ns) {
		return node->ns;
	} else {
		return xmlSearchNs(node->doc, node, NULL);
	}
}

int attr_is_equal_ex(xmlAttrPtr node, char *name, char *ns)
{
	if (name == NULL || strcmp(node->name, name) == 0) {
		if (ns) {
			xmlNsPtr nsPtr = attr_find_ns(node);
			return (strcmp(nsPtr->href, ns) == 0);
		}
		return TRUE;
	}
	return FALSE;
}

int node_is_equal_ex(xmlNodePtr node, char *name, char *ns)
{
	if (name == NULL || strcmp(node->name, name) == 0) {
		if (ns) {
			xmlNsPtr nsPtr = node_find_ns(node);
			return (strcmp(nsPtr->href, ns) == 0);
		}
		return TRUE;
	}
	return FALSE;
}


xmlAttrPtr get_attribute_ex(xmlAttrPtr node, char *name, char *ns)
{
	while (node!=NULL) {
		if (attr_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_ex(xmlNodePtr node, char *name, char *ns)
{
	while (node!=NULL) {
		if (node_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_recurisve_ex(xmlNodePtr node, char *name, char *ns)
{
	while (node != NULL) {
		if (node_is_equal_ex(node, name, ns)) {
			return node;
		} else if (node->children != NULL) {
			xmlNodePtr tmp = get_node_recurisve_ex(node->children, name, ns);
			if (tmp) {
				return tmp;
			}
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns)
{
	xmlAttrPtr attr;

	while (node != NULL) {
		if (name != NULL) {
			node = get_node_ex(node, name, name_ns);
			if (node==NULL) {
				return NULL;
			}
		}

		attr = get_attribute_ex(node->properties, attribute, attr_ns);
		if (attr != NULL && strcmp(attr->children->content, value) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns)
{
	while (node != NULL) {
		if (node_is_equal_ex(node, name, name_ns)) {
			xmlAttrPtr attr = get_attribute_ex(node->properties, attribute, attr_ns);
			if (attr != NULL && strcmp(attr->children->content, value) == 0) {
				return node;
			}
		}
		if (node->children != NULL) {
			xmlNodePtr tmp = get_node_with_attribute_recursive_ex(node->children, name, name_ns, attribute, value, attr_ns);
			if (tmp) {
				return tmp;
			}
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr check_and_resolve_href(xmlNodePtr data)
{
	if (data && data->properties) {
		xmlAttrPtr href = get_attribute(data->properties, "href");
		if (href) {
			/*  Internal href try and find node */
			if (href->children->content[0] == '#') {
				return get_node_with_attribute_recursive(data->doc->children, NULL, "id", &href->children->content[1]);
			}
			/*  TODO: External href....? */
		}
	}
	return data;
}

int parse_namespace(const char *inval, char **value, char **namespace)
{
	char *found = strchr(inval, ':');

	if (found != NULL && found != inval) {
		(*namespace) = estrndup(inval, found - inval);
		(*value) = estrdup(++found);
	} else {
		(*value) = estrdup(inval);
		(*namespace) = NULL;
	}

	return FALSE;
}
