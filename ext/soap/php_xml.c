#include "php_soap.h"

xmlNsPtr attr_find_ns(xmlAttrPtr node)
{
	if(node->ns)
		return node->ns;
	else if(node->parent->ns)
		return node->parent->ns;
	else
		return xmlSearchNs(node->doc, node->parent, NULL);
}

xmlNsPtr node_find_ns(xmlNodePtr node)
{
	if(node->ns)
		return node->ns;
	else
		return xmlSearchNs(node->doc, node, NULL);
}

int attr_is_equal_ex(xmlAttrPtr node, char *name, char *ns)
{
	if(!strcmp(node->name, name))
	{
		if(ns)
		{
			xmlNsPtr nsPtr;
			if(node->ns)
				nsPtr = node->ns;
			else if(node->parent->ns)
				nsPtr = node->parent->ns;
			else
				nsPtr = xmlSearchNs(node->doc, node->parent, NULL);
			if(!strcmp(nsPtr->href, ns))
				return TRUE;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int node_is_equal_ex(xmlNodePtr node, char *name, char *ns)
{
	if(!strcmp(node->name, name))
	{
		if(ns)
		{
			xmlNsPtr nsPtr;
			if(node->ns)
				nsPtr = node->ns;
			else
				nsPtr = xmlSearchNs(node->doc, node, NULL);
			if(!strcmp(nsPtr->href, ns))
				return TRUE;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

xmlAttrPtr get_attribute_ex(xmlAttrPtr node, char *name, char *ns)
{
	xmlAttrPtr trav = node;
	if(node == NULL) return NULL;
	do {
		if(attr_is_equal_ex(trav, name, ns))
			return trav;
	} while(trav = trav->next);
	return NULL;
}

xmlNodePtr get_node_ex(xmlNodePtr node, char *name, char *ns)
{
	xmlNodePtr trav = node;
	if(node == NULL) return NULL;
	do {
		if(node_is_equal_ex(trav, name, ns))
			return trav;
	} while(trav = trav->next);
	return NULL;
}

xmlNodePtr get_node_recurisve_ex(xmlNodePtr node, char *name, char *ns)
{
	xmlNodePtr trav = node;
	if(node == NULL) return NULL;
	do
	{
		if(node_is_equal_ex(trav, name, ns))
			return trav;
		else
		{
			if(node->children != NULL)
			{
				xmlNodePtr tmp;
				tmp = get_node_recurisve_ex(node->children, name, ns);
				if(tmp)
					return tmp;
			}
		}
	} while(trav = trav->next);
	return NULL;
}

xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns)
{
	xmlNodePtr trav = node, cur;
	xmlAttrPtr attr;

	if(node == NULL) return NULL;
	do
	{
		if(name != NULL)
		{
			cur = get_node_ex(trav, name, name_ns);
			if(!cur)
				return cur;
		}
		else
			cur = trav;

		attr = get_attribute_ex(cur->properties, attribute, attr_ns);
		if(attr != NULL && strcmp(attr->children->content, value) == 0)
			return cur;
		else
		{
			if(cur->children != NULL)
			{
				xmlNodePtr tmp;
				tmp = get_node_with_attribute_ex(cur->children, name, name_ns, attribute, value, attr_ns);
				if(tmp)
					return tmp;
			}
		}
	}while(trav = trav->next);
	return NULL;
}

xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns)
{
	xmlNodePtr trav = node, cur;
	xmlAttrPtr attr;

	if(node == NULL) return NULL;
	do
	{
		if(name != NULL)
		{
			cur = get_node_recurisve_ex(trav, name, name_ns);
			if(!cur)
				return cur;
		}
		else
			cur = trav;

		attr = get_attribute_ex(cur->properties, attribute, attr_ns);
		if(attr != NULL && strcmp(attr->children->content, value) == 0)
			return cur;
		else
		{
			if(cur->children != NULL)
			{
				xmlNodePtr tmp;
				tmp = get_node_with_attribute_recursive_ex(cur->children, name, name_ns, attribute, value, attr_ns);
				if(tmp)
					return tmp;
			}
		}
	}while(trav = trav->next);
	return NULL;
}

xmlNodePtr check_and_resolve_href(xmlNodePtr data)
{
	xmlAttrPtr href;
	xmlNodePtr ret = data;

	if(!data || !data->properties)
		return ret;

	href = get_attribute(data->properties, "href");
	if(href)
	{
		// Internal href try and find node
		if(href->children->content[0] == '#')
		{
			ret = get_node_with_attribute_recursive(data->doc->children, NULL, "id", &href->children->content[1]);
		}
		// External href....?
	}

	return ret;
}

int parse_namespace(char *inval, char **value, char **namespace)
{
	char *found = strchr(inval, ':');

	if(found != NULL)
	{
		(*namespace) = estrndup(inval, found - inval);
		(*value) = estrdup(++found);
	}
	else
	{
		(*value) = estrdup(inval);
		(*namespace) = NULL;
	}

	return FALSE;
}

