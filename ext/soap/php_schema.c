#include "php_soap.h"
/*
2.6.1 xsi:type
2.6.2 xsi:nil
2.6.3 xsi:schemaLocation, xsi:noNamespaceSchemaLocation
*/

/*
<schema
  attributeFormDefault = (qualified | unqualified) : unqualified
  blockDefault = (#all | List of (extension | restriction | substitution))  : ''
  elementFormDefault = (qualified | unqualified) : unqualified
  finalDefault = (#all | List of (extension | restriction))  : ''
  id = ID
  targetNamespace = anyURI
  version = token
  xml:lang = language
  {any attributes with non-schema namespace . . .}>
  Content: ((include | import | redefine | annotation)*, (((simpleType | complexType | group | attributeGroup) | element | attribute | notation), annotation*)*)
</schema>
*/
int load_schema(sdlPtr *sdl,xmlNodePtr schema)
{
	xmlNodePtr trav, element, compType, simpleType, attribute;
	xmlAttrPtr tns;

	if(!(*sdl)->types)
	{
		(*sdl)->types = malloc(sizeof(HashTable));
		zend_hash_init((*sdl)->types, 0, NULL, delete_type, 1);
	}

	tns = get_attribute(schema->properties, "targetNamespace");

	trav = schema->children;
	FOREACHNODE(trav,"complexType",compType)
	{
		schema_complexType(sdl, tns, compType, NULL);
	}
	ENDFOREACH(trav);

	trav = schema->children;
	FOREACHNODE(trav,"simpleType",simpleType)
	{
		schema_simpleType(sdl, tns, simpleType, NULL);
	}
	ENDFOREACH(trav);

	trav = schema->children;
	FOREACHNODE(trav,"element",element)
	{
		schema_element(sdl, tns, element, NULL);
	}
	ENDFOREACH(trav);

	trav = schema->children;
	FOREACHNODE(trav, "attribute", attribute)
	{
		schema_attribute(sdl, tns, attribute, NULL);
	}
	ENDFOREACH(trav);
	return FALSE;
}
/*
<simpleType
  final = (#all | (list | union | restriction))
  id = ID
  name = NCName
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (restriction | list | union))
</simpleType>
*/
int schema_simpleType(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr simpleType, sdlTypePtr cur_type)
{
	xmlNodePtr content;
	xmlAttrPtr name, ns;

	ns = get_attribute(simpleType->properties, "targetNamespace");
	if(ns == NULL)
		ns = tsn;

	name = get_attribute(simpleType->properties, "name");
	if(name != NULL)
	{
		HashTable *ht;
		smart_str key = {0};
		sdlTypePtr newType, *ptr;

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));
		newType->name = strdup(name->children->content);
		newType->namens = strdup(ns->children->content);

		if(cur_type == NULL)
		{
			ht = (*sdl)->types;
			smart_str_appends(&key, newType->namens);
			smart_str_appendc(&key, ':');
			smart_str_appends(&key, newType->name);
			smart_str_0(&key);
		}
		else
		{
			if(cur_type->elements == NULL)
			{
				cur_type->elements = malloc(sizeof(HashTable));
				zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
			}
			ht = cur_type->elements;
			smart_str_appends(&key, cur_type->name);
		}

		zend_hash_add(ht, key.c, key.len + 1, &newType, sizeof(sdlTypePtr), (void **)&ptr);
		cur_type = (*ptr);
		smart_str_free(&key);
	}

	content = get_node(simpleType->children, "restriction");
	if(content != NULL)
	{
		schema_restriction_simpleType(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(simpleType->children, "list");
	if(content != NULL)
	{
		schema_list(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(simpleType->children, "union");
	if(content != NULL)
	{
		schema_union(sdl, tsn, content, cur_type);
		return TRUE;
	}

	return FALSE;
}

/*
<list
  id = ID
  itemType = QName
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleType?))
</list>
*/
int schema_list(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr listType, sdlTypePtr cur_type)
{
	return TRUE;
}

/*
<union
  id = ID
  memberTypes = List of QName
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleType*))
</union>
*/
int schema_union(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr unionType, sdlTypePtr cur_type)
{
	return TRUE;
}

/*
<simpleContent
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (restriction | extension))
</simpleContent>
*/
int schema_simpleContent(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr simpCompType, sdlTypePtr cur_type)
{
	xmlNodePtr content;

	content = get_node(simpCompType->children, "restriction");
	if(content == NULL)
	{
		schema_restriction_simpleContent(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(simpCompType->children, "extension");
	if(content == NULL)
	{
		//schema_extension(sdl, tsn, content, cur_type);
		php_error(E_ERROR, "Error parsing schema (doesn't support extensions on simpleContent)");
		return TRUE;
	}

	php_error(E_ERROR, "Error parsing schema (simpleContent)");
	return FALSE;
}

/*
<restriction
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleType?, (minExclusive | minInclusive | maxExclusive | maxInclusive | totalDigits | fractionDigits | length | minLength | maxLength | enumeration | whiteSpace | pattern)*)?, ((attribute | attributeGroup)*, anyAttribute?))
</restriction>
*/
int schema_restriction_simpleType(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type)
{
	xmlNodePtr content, trav;
	xmlAttrPtr base;

	content = get_node(restType->children, "simpleType");
	if(content != NULL)
	{
		schema_simpleType(sdl, tsn, content, cur_type);
		return TRUE;
	}

	base = get_attribute(restType->properties, "base");
	if(base != NULL)
	{
		//cur_type->base = estrdup(base->children->content);
	}

	if(cur_type->restrictions == NULL)
	{
		cur_type->restrictions = malloc(sizeof(sdlRestrictions));
		memset(cur_type->restrictions, 0, sizeof(sdlRestrictions));
	}

	trav = restType->children;
	do
	{
		if(trav->type == XML_ELEMENT_NODE)
		{
			if(!strcmp(trav->name, "minExclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->minExclusive);
			else if(!strcmp(trav->name, "minInclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->minInclusive);
			else if(!strcmp(trav->name, "maxExclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->maxExclusive);
			else if(!strcmp(trav->name, "maxInclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->maxInclusive);
			else if(!strcmp(trav->name, "totalDigits"))
				schema_restriction_var_int(trav, &cur_type->restrictions->totalDigits);
			else if(!strcmp(trav->name, "fractionDigits"))
				schema_restriction_var_int(trav, &cur_type->restrictions->fractionDigits);
			else if(!strcmp(trav->name, "length"))
				schema_restriction_var_int(trav, &cur_type->restrictions->length);
			else if(!strcmp(trav->name, "minLength"))
				schema_restriction_var_int(trav, &cur_type->restrictions->minLength);
			else if(!strcmp(trav->name, "maxLength"))
				schema_restriction_var_int(trav, &cur_type->restrictions->maxLength);
			else if(!strcmp(trav->name, "whiteSpace"))
				schema_restriction_var_char(trav, &cur_type->restrictions->whiteSpace);
			else if(!strcmp(trav->name, "pattern"))
				schema_restriction_var_char(trav, &cur_type->restrictions->pattern);
			else if(!strcmp(trav->name, "enumeration"))
			{
				sdlRestrictionCharPtr enumval = NULL;

				schema_restriction_var_char(trav, &enumval);
				if(cur_type->restrictions->enumeration == NULL)
				{
					cur_type->restrictions->enumeration = malloc(sizeof(HashTable));
					zend_hash_init(cur_type->restrictions->enumeration, 0, NULL, delete_schema_restriction_var_char, 1);
				}
				zend_hash_next_index_insert(cur_type->restrictions->enumeration, &enumval, sizeof(sdlRestrictionCharPtr), NULL);
			}
		}
	}while(trav = trav->next);

	return TRUE;
}

/*
<restriction
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (group | all | choice | sequence)?, ((attribute | attributeGroup)*, anyAttribute?))
</restriction>
*/
int schema_restriction_complexContent(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type)
{
	xmlAttrPtr base;
	xmlNodePtr trav;

	base = get_attribute(restType->properties, "base");
	if(base != NULL)
	{
		char *type, *ns;
		xmlNsPtr nsptr;

		parse_namespace(base->children->content, &type, &ns);
		nsptr = xmlSearchNs(restType->doc, restType, ns);
		if(nsptr != NULL)
		{
			cur_type->encode = get_encoder((*sdl), (char *)nsptr->href, type);
		}
		if(type) efree(type);
		if(ns) efree(ns);
	}

	trav = restType->children;
	do
	{
		if(trav->type == XML_ELEMENT_NODE)
		{
			if(!strcmp(trav->name, "group"))
			{
				schema_group(sdl, tsn, trav, cur_type);
				return TRUE;
			}
			else if(!strcmp(trav->name, "all"))
			{
				schema_all(sdl, tsn, trav, cur_type);
				return TRUE;
			}
			else if(!strcmp(trav->name, "choice"))
			{
				schema_choice(sdl, tsn, trav, cur_type);
				return TRUE;
			}
			else if(!strcmp(trav->name, "sequence"))
			{
				schema_sequence(sdl, tsn, trav, cur_type);
				return TRUE;
			}
			else if(!strcmp(trav->name, "attribute"))
			{
				schema_attribute(sdl, tsn, trav, cur_type);
			}
		}
	}while(trav = trav->next);

	return TRUE;
}

/*
<restriction
  base = QName
  id = ID
  {any attributes with non-schema Namespace . . .}>
  Content: (annotation?, (simpleType?, (minExclusive | minInclusive | maxExclusive | maxInclusive | totalDigits | fractionDigits | length | minLength | maxLength | enumeration | whiteSpace | pattern)*))
</restriction>
*/
int schema_restriction_simpleContent(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type)
{
	xmlNodePtr content, trav;
	xmlAttrPtr base;

	base = get_attribute(restType->properties, "base");
	if(base != NULL)
		cur_type->encode = get_encoder_from_prefix((*sdl), restType, base->children->content);

	content = get_node(restType->children, "simpleType");
	if(content != NULL)
	{
		schema_simpleType(sdl, tsn, content, cur_type);
		return TRUE;
	}

	if(cur_type->restrictions == NULL)
		cur_type->restrictions = malloc(sizeof(sdlRestrictions));

	trav = restType->children;
	do
	{
		if(trav->type == XML_ELEMENT_NODE)
		{
			if(!strcmp(trav->name, "minExclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->minExclusive);
			else if(!strcmp(trav->name, "minInclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->minInclusive);
			else if(!strcmp(trav->name, "maxExclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->maxExclusive);
			else if(!strcmp(trav->name, "maxInclusive"))
				schema_restriction_var_int(trav, &cur_type->restrictions->maxInclusive);
			else if(!strcmp(trav->name, "totalDigits"))
				schema_restriction_var_int(trav, &cur_type->restrictions->totalDigits);
			else if(!strcmp(trav->name, "fractionDigits"))
				schema_restriction_var_int(trav, &cur_type->restrictions->fractionDigits);
			else if(!strcmp(trav->name, "length"))
				schema_restriction_var_int(trav, &cur_type->restrictions->length);
			else if(!strcmp(trav->name, "minLength"))
				schema_restriction_var_int(trav, &cur_type->restrictions->minLength);
			else if(!strcmp(trav->name, "maxLength"))
				schema_restriction_var_int(trav, &cur_type->restrictions->maxLength);
			else if(!strcmp(trav->name, "whiteSpace"))
				schema_restriction_var_char(trav, &cur_type->restrictions->whiteSpace);
			else if(!strcmp(trav->name, "pattern"))
				schema_restriction_var_char(trav, &cur_type->restrictions->pattern);
			else if(!strcmp(trav->name, "enumeration"))
			{
				sdlRestrictionCharPtr enumval = NULL;

				schema_restriction_var_char(trav, &enumval);
				if(cur_type->restrictions->enumeration == NULL)
				{
					cur_type->restrictions->enumeration = malloc(sizeof(HashTable));
					zend_hash_init(cur_type->restrictions->enumeration, 0, NULL, delete_schema_restriction_var_char, 1);
				}
				zend_hash_next_index_insert(cur_type->restrictions->enumeration, &enumval, sizeof(sdlRestrictionCharPtr), NULL);
			}
		}
	}while(trav = trav->next);

	return TRUE;
}

int schema_restriction_var_int(xmlNodePtr val, sdlRestrictionIntPtr *valptr)
{
	xmlAttrPtr fixed, value, id;

	if((*valptr) == NULL)
		(*valptr) = malloc(sizeof(sdlRestrictionInt));

	fixed = get_attribute(val->properties, "fixed");
	(*valptr)->fixed = FALSE;
	if(fixed != NULL)
	{
		if(!strcmp(fixed->children->content, "true") ||
			!strcmp(fixed->children->content, "1"))
			(*valptr)->fixed = TRUE;
	}

	id = get_attribute(val->properties, "id");
	if(id != NULL)
		(*valptr)->id = strdup(id->children->content);

	value = get_attribute(val->properties, "value");
	if(value == NULL)
		php_error(E_ERROR, "Error parsing wsdl schema \"missing value for minExclusive\"");

	(*valptr)->value = atoi(value->children->content);

	return TRUE;
}

void delete_restriction_var_int(void *rvi)
{
	sdlRestrictionIntPtr ptr = *((sdlRestrictionIntPtr*)rvi);
	if(ptr->id);
		free(ptr->id);
	free(ptr);
}

int schema_restriction_var_char(xmlNodePtr val, sdlRestrictionCharPtr *valptr)
{
	xmlAttrPtr fixed, value, id;

	if((*valptr) == NULL)
		(*valptr) = malloc(sizeof(sdlRestrictionChar));

	fixed = get_attribute(val->properties, "fixed");
	(*valptr)->fixed = FALSE;
	if(fixed != NULL)
	{
		if(!strcmp(fixed->children->content, "true") ||
			!strcmp(fixed->children->content, "1"))
			(*valptr)->fixed = TRUE;
	}

	id = get_attribute(val->properties, "id");
	if(id != NULL)
		(*valptr)->id = strdup(id->children->content);

	value = get_attribute(val->properties, "value");
	if(value == NULL)
		php_error(E_ERROR, "Error parsing wsdl schema \"missing value restriction\"");

	(*valptr)->value = strdup(value->children->content);
	return TRUE;
}

void delete_schema_restriction_var_char(void *srvc)
{
	sdlRestrictionCharPtr ptr = *((sdlRestrictionCharPtr*)srvc);
	if(ptr->id)
		free(ptr->id);
	if(ptr->value)
		free(ptr->value);
	free(ptr);
}

/*
From simpleContent (not supported):
<extension
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, ((attribute | attributeGroup)*, anyAttribute?))
</extension>

From complexContent:
<extension
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, ((group | all | choice | sequence)?, ((attribute | attributeGroup)*, anyAttribute?)))
</extension>
*/
int schema_extension(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type)
{
	xmlNodePtr content;
	xmlAttrPtr base;

	base = get_attribute(extType->properties, "base");

	content = get_node(extType->children, "group");
	if(content != NULL)
	{
		schema_group(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(extType->children, "all");
	if(content != NULL)
	{
		schema_all(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(extType->children, "choice");
	if(content != NULL)
	{
		schema_choice(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(extType->children, "sequence");
	if(content != NULL)
	{
		schema_sequence(sdl, tsn, content, cur_type);
		return TRUE;
	}
	return FALSE;
}

/*
<all
  id = ID
  maxOccurs = 1 : 1
  minOccurs = (0 | 1) : 1
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, element*)
</all>
*/
int schema_all(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr all, sdlTypePtr cur_type)
{
	xmlNodePtr element, trav;

	trav = all->children;
	FOREACHNODE(trav, "element", element)
	{
		schema_element(sdl, tsn, element, cur_type);
	}
	ENDFOREACH(trav);
	return TRUE;
}

/*
<group
  name = NCName>
  Content: (annotation?, (all | choice | sequence))
</group>
*/
int schema_group(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr groupType, sdlTypePtr cur_type)
{
	xmlNodePtr content;
	xmlAttrPtr name;

	name = get_attribute(groupType->properties, "name");
	if(name != NULL)
	{

	}

	content = get_node(groupType->children, "all");
	if(content != NULL)
	{
		schema_all(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(groupType->children, "choice");
	if(content != NULL)
	{
		schema_choice(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(groupType->children, "sequence");
	if(content != NULL)
	{
		schema_sequence(sdl, tsn, content, cur_type);
		return TRUE;
	}
	return FALSE;
}
/*
<choice
  id = ID
  maxOccurs = (nonNegativeInteger | unbounded)  : 1
  minOccurs = nonNegativeInteger : 1
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (element | group | choice | sequence | any)*)
</choice>
*/
int schema_choice(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr choiceType, sdlTypePtr cur_type)
{
	xmlNodePtr trav, data;

	//cur_type->property_type = CHOICE;

	trav = choiceType->children;
	FOREACHNODE(trav, "element", data)
	{
		schema_element(sdl, tsn, data, cur_type);
	}
	ENDFOREACH(trav);

	trav = choiceType->children;
	FOREACHNODE(trav, "group", data)
	{
		schema_group(sdl, tsn, data, cur_type);
	}
	ENDFOREACH(trav);

	trav = choiceType->children;
	FOREACHNODE(trav, "choice", data)
	{
		schema_choice(sdl, tsn, data, cur_type);
	}
	ENDFOREACH(trav);

	trav = choiceType->children;
	FOREACHNODE(trav, "sequence", data)
	{
		schema_sequence(sdl, tsn, data, cur_type);
	}
	ENDFOREACH(trav);

	trav = choiceType->children;
	FOREACHNODE(trav, "any", data)
	{
		schema_any(sdl, tsn, data, cur_type);
	}
	ENDFOREACH(trav);

	return TRUE;
}

/*
<sequence
  id = ID
  maxOccurs = (nonNegativeInteger | unbounded)  : 1
  minOccurs = nonNegativeInteger : 1
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (element | group | choice | sequence | any)*)
</sequence>
*/
int schema_sequence(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr seqType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;

	trav = seqType->children;
	do
	{
		if(trav->type == XML_ELEMENT_NODE)
		{
			if(!strcmp(trav->name, "element"))
			{
				schema_element(sdl, tsn, trav, cur_type);
			}
			else if(!strcmp(trav->name, "group"))
			{
				schema_group(sdl, tsn, trav, cur_type);
			}
			else if(!strcmp(trav->name, "choice"))
			{
				schema_choice(sdl, tsn, trav, cur_type);
			}
			else if(!strcmp(trav->name, "sequence"))
			{
				schema_sequence(sdl, tsn, trav, cur_type);
			}
			else if(!strcmp(trav->name, "any"))
			{
				schema_any(sdl, tsn, trav, cur_type);
			}
		}
	}
	while(trav = trav->next);

	return TRUE;
}

int schema_any(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type)
{
	return TRUE;
}

/*
<complexContent
  id = ID
  mixed = boolean
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (restriction | extension))
</complexContent>
*/
int schema_complexContent(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr compCont, sdlTypePtr cur_type)
{
	xmlNodePtr content;

	content = get_node(compCont->children, "restriction");
	if(content != NULL)
	{
		schema_restriction_complexContent(sdl, tsn, content, cur_type);
		return TRUE;
	}

	return TRUE;
}

/*
<complexType
  abstract = boolean : false
  block = (#all | List of (extension | restriction))
  final = (#all | List of (extension | restriction))
  id = ID
  mixed = boolean : false
  name = NCName
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleContent | complexContent | ((group | all | choice | sequence)?, ((attribute | attributeGroup)*, anyAttribute?))))
</complexType>
*/
int schema_complexType(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr compType, sdlTypePtr cur_type)
{
	xmlNodePtr content;
	xmlAttrPtr attrs, name, ns;

	attrs = compType->properties;
	ns = get_attribute(attrs, "targetNamespace");
	if(ns == NULL)
		ns = tsn;

	name = get_attribute(attrs, "name");
	if(name)
	{
		HashTable *ht;
		sdlTypePtr newType, *ptr;
		smart_str key = {0};

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));
		newType->name = strdup(name->children->content);
		newType->namens = strdup(ns->children->content);

		if(cur_type == NULL)
		{
			ht = (*sdl)->types;
			smart_str_appends(&key, newType->namens);
			smart_str_appendc(&key, ':');
			smart_str_appends(&key, newType->name);
			smart_str_0(&key);
		}
		else
		{
			if(cur_type->elements == NULL)
			{
				cur_type->elements = malloc(sizeof(HashTable));
				zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
			}
			ht = cur_type->elements;
			smart_str_appends(&key, newType->name);
		}

		zend_hash_add(ht, key.c, key.len + 1, &newType, sizeof(sdlTypePtr), (void **)&ptr);
		cur_type = (*ptr);
		create_encoder((*sdl), cur_type, ns->children->content, name->children->content);
		smart_str_free(&key);
	}

	content = get_node(compType->children, "simpleContent");
	if(content != NULL)
	{
		schema_simpleContent(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(compType->children, "complexContent");
	if(content != NULL)
	{
		schema_complexContent(sdl, tsn, content, cur_type);
		return TRUE;
	}

	//(group | all | choice | sequence)
	content = get_node(compType->children, "group");
	if(content != NULL)
	{
		schema_group(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(compType->children, "all");
	if(content != NULL)
	{
		schema_all(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(compType->children, "choice");
	if(content != NULL)
	{
		schema_choice(sdl, tsn, content, cur_type);
		return TRUE;
	}

	content = get_node(compType->children, "sequence");
	if(content != NULL)
		schema_sequence(sdl, tsn, content, cur_type);

	return TRUE;
}
/*
<element
  abstract = boolean : false
  block = (#all | List of (extension | restriction | substitution))
  default = string
  final = (#all | List of (extension | restriction))
  fixed = string
  form = (qualified | unqualified)
  id = ID
  maxOccurs = (nonNegativeInteger | unbounded)  : 1
  minOccurs = nonNegativeInteger : 1
  name = NCName
  nillable = boolean : false
  ref = QName
  substitutionGroup = QName
  type = QName
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, ((simpleType | complexType)?, (unique | key | keyref)*))
</element>
*/
int schema_element(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr element, sdlTypePtr cur_type)
{
	xmlNodePtr content;
	xmlAttrPtr attrs, curattr, name, ns;
	TSRMLS_FETCH();

	attrs = element->properties;
	ns = get_attribute(attrs, "targetNamespace");
	if(ns == NULL)
		ns = tsn;

	name = get_attribute(attrs, "name");
	if(!name)
		name = get_attribute(attrs, "ref");
	if(name)
	{
		HashTable *addHash;
		sdlTypePtr newType, *tmp;
		smart_str key = {0};

		newType = malloc(sizeof(sdlType));

		memset(newType, 0, sizeof(sdlType));
		newType->name = strdup(name->children->content);
		newType->namens = strdup(tsn->children->content);
		newType->nullable = FALSE;
		newType->min_occurs = 1;
		newType->max_occurs = 1;

		if(cur_type == NULL)
		{
			addHash = (*sdl)->types;
			smart_str_appends(&key, newType->namens);
			smart_str_appendc(&key, ':');
			smart_str_appends(&key, newType->name);
		}
		else
		{
			if(cur_type->elements == NULL)
			{
				cur_type->elements = malloc(sizeof(HashTable));
				zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
			}
			addHash = cur_type->elements;
			smart_str_appends(&key, newType->name);
		}

		smart_str_0(&key);
		zend_hash_add(addHash, key.c, key.len + 1, &newType, sizeof(sdlTypePtr), (void **)&tmp);
		cur_type = (*tmp);
		create_encoder((*sdl), cur_type, ns->children->content, name->children->content);
		smart_str_free(&key);
	}

	curattr = get_attribute(attrs, "maxOccurs");
	if(curattr)
	{
		if(!strcmp(curattr->children->content, "unbounded"))
			cur_type->max_occurs = -1;
		else
			cur_type->max_occurs = atoi(curattr->children->content);
	}

	curattr = get_attribute(attrs, "minOccurs");
	if(curattr)
		cur_type->min_occurs = atoi(curattr->children->content);

	//nillable = boolean : false
	attrs = element->properties;
	curattr = get_attribute(attrs, "nillable");
	if(curattr)
	{
		if(!stricmp(curattr->children->content, "true") ||
			!stricmp(curattr->children->content, "1"))
			cur_type->nullable = TRUE;
		else
			cur_type->nullable = FALSE;
	}
	else
		cur_type->nullable = FALSE;

	//type = QName
	curattr = get_attribute(attrs, "type");
	if(!curattr)
		curattr = name;
	if(curattr)
	{
		char *cptype, *str_ns;
		xmlNsPtr nsptr;

		parse_namespace(curattr->children->content, &cptype, &str_ns);
		if(str_ns)
			nsptr = xmlSearchNs(element->doc, element, str_ns);
		else
			nsptr = xmlSearchNsByHref(element->doc, element, ns->children->content);

		cur_type->encode = get_create_encoder((*sdl), cur_type, (char *)nsptr->href, (char *)cptype);
		if(str_ns) efree(str_ns);
		if(cptype) efree(cptype);
	}

	if(cur_type->max_occurs == -1 || cur_type->max_occurs > 1)
		cur_type->encode = get_conversion(SOAP_ENC_ARRAY);

	content = get_node(element->children, "simpleType");
	if(content)
		schema_simpleType(sdl, tsn, content, cur_type);

	content = get_node(element->children, "complexType");
	if(content)
		schema_complexType(sdl, tsn, content, cur_type);

	return FALSE;
}

/*
<attribute
  default = string
  fixed = string
  form = (qualified | unqualified)
  id = ID
  name = NCName
  ref = QName
  type = QName
  use = (optional | prohibited | required) : optional
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleType?))
</attribute>
*/
int schema_attribute(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr attrType, sdlTypePtr cur_type)
{
	xmlAttrPtr attr;
	sdlAttributePtr newAttr;
	xmlAttrPtr trav;
	smart_str key = {0};

	newAttr = malloc(sizeof(sdlAttribute));
	memset(newAttr, 0, sizeof(sdlAttribute));

	if(cur_type->attributes == NULL)
	{
		cur_type->attributes = malloc(sizeof(HashTable));
		zend_hash_init(cur_type->attributes, 0, NULL, delete_attribute, 1);
	}

	trav = attrType->properties;
	FOREACHATTRNODE(trav, NULL, attr)
	{
		if(attr_is_equal_ex(trav, "default", SCHEMA_NAMESPACE))
			newAttr->def = strdup(attr->children->content);
		else if(attr_is_equal_ex(trav, "fixed", SCHEMA_NAMESPACE))
			newAttr->fixed = strdup(attr->children->content);
		else if(attr_is_equal_ex(trav, "form", SCHEMA_NAMESPACE))
			newAttr->form = strdup(attr->children->content);
		else if(attr_is_equal_ex(trav, "id", SCHEMA_NAMESPACE))
			newAttr->id = strdup(attr->children->content);
		else if(attr_is_equal_ex(trav, "name", SCHEMA_NAMESPACE))
			newAttr->name = strdup(attr->children->content);
		else if(attr_is_equal_ex(trav, "ref", SCHEMA_NAMESPACE))
			newAttr->ref= strdup(attr->children->content);
		else if(attr_is_equal_ex(trav, "type", SCHEMA_NAMESPACE))
			newAttr->type = strdup(attr->children->content);
		else if(attr_is_equal_ex(trav, "use", SCHEMA_NAMESPACE))
			newAttr->use = strdup(attr->children->content);
		else
		{
			xmlNsPtr nsPtr = attr_find_ns(trav);

			if(strcmp(nsPtr->href, SCHEMA_NAMESPACE))
			{
				smart_str key2 = {0};

				if(!newAttr->extraAttributes)
				{
					newAttr->extraAttributes = malloc(sizeof(HashTable));
					zend_hash_init(newAttr->extraAttributes, 0, NULL, NULL, 1);
				}

				smart_str_appends(&key2, nsPtr->href);
				smart_str_appendc(&key2, ':');
				smart_str_appends(&key2, trav->name);
				smart_str_0(&key2);
				zend_hash_add(newAttr->extraAttributes, key2.c, key2.len + 1, &trav, sizeof(xmlAttrPtr), NULL);
				smart_str_free(&key2);
			}
		}
	}
	ENDFOREACH(trav);


	if(newAttr->ref || newAttr->name)
	{
		xmlNsPtr ns;

		if(newAttr->ref)
		{
			char *value, *prefix = NULL;

			parse_namespace(newAttr->ref, &value, &prefix);
			ns = xmlSearchNs(attrType->doc, attrType, prefix);
			smart_str_appends(&key, ns->href);
			smart_str_appendc(&key, ':');
			smart_str_appends(&key, value);

			if(value)
				efree(value);
			if(prefix)
				efree(prefix);
		}
		else
		{
			ns = node_find_ns(attrType);
			smart_str_appends(&key, ns->href);
			smart_str_appendc(&key, ':');
			smart_str_appends(&key, newAttr->name);
		}


		if(ns)
		{
			smart_str_0(&key);
			zend_hash_add(cur_type->attributes, key.c, key.len + 1, &newAttr, sizeof(sdlAttributePtr), NULL);
			smart_str_free(&key);
			return TRUE;
		}
	}

	zend_hash_next_index_insert(cur_type->attributes, &newAttr, sizeof(sdlAttributePtr), NULL);
	return TRUE;
}