#include "php_soap.h"

static int schema_simpleType(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr simpleType, sdlTypePtr cur_type);
static int schema_complexType(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr compType, sdlTypePtr cur_type);
static int schema_sequence(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr seqType, sdlTypePtr cur_type);
static int schema_list(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr listType, sdlTypePtr cur_type);
static int schema_union(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr unionType, sdlTypePtr cur_type);
static int schema_simpleContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr simpCompType, sdlTypePtr cur_type);
static int schema_restriction_simpleContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type, int simpleType);
static int schema_restriction_complexContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type);
static int schema_extension_simpleContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type);
static int schema_extension_complexContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type);
static int schema_all(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type);
static int schema_group(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr groupType, sdlTypePtr cur_type);
static int schema_choice(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr choiceType, sdlTypePtr cur_type);
static int schema_element(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr element, sdlTypePtr cur_type);
static int schema_attribute(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr attrType, sdlTypePtr cur_type);
static int schema_any(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type);
static int schema_attributeGroup(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr attrType, sdlTypePtr cur_type);
static int schema_attributeGroupRef(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr attrType, sdlTypePtr cur_type);

static int schema_restriction_var_int(xmlNodePtr val, sdlRestrictionIntPtr *valptr);

static int schema_restriction_var_char(xmlNodePtr val, sdlRestrictionCharPtr *valptr);
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
int load_schema(sdlPtr sdl,xmlNodePtr schema)
{
	xmlNodePtr trav;
	xmlAttrPtr tns;

	if (!sdl->types) {
		sdl->types = malloc(sizeof(HashTable));
		zend_hash_init(sdl->types, 0, NULL, delete_type, 1);
	}
	if (!sdl->elements) {
		sdl->elements = malloc(sizeof(HashTable));
		zend_hash_init(sdl->elements, 0, NULL, delete_type, 1);
	}

	tns = get_attribute(schema->properties, "targetNamespace");

	trav = schema->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"include")) {
				/* TODO: <include> support */
			} else if (node_is_equal(trav,"import")) {
				/* TODO: <import> support */
			} else if (node_is_equal(trav,"redefine")) {
				/* TODO: <redefine> support */
			} else if (node_is_equal(trav,"annotation")) {
				/* TODO: <annotation> support */
			} else {
			  break;
			}
		}
		trav = trav->next;
	}

	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"simpleType")) {
				schema_simpleType(sdl, tns, trav, NULL);
			} else if (node_is_equal(trav,"complexType")) {
				schema_complexType(sdl, tns, trav, NULL);
			} else if (node_is_equal(trav,"group")) {
				schema_group(sdl, tns, trav, NULL);
			} else if (node_is_equal(trav,"attributeGroup")) {
				schema_attributeGroup(sdl, tns, trav, NULL);
			} else if (node_is_equal(trav,"element")) {
				schema_element(sdl, tns, trav, NULL);
			} else if (node_is_equal(trav,"attribute")) {
				schema_attribute(sdl, tns, trav, NULL);
			} else if (node_is_equal(trav,"notation")) {
				/* TODO: <notation> support */
			} else if (node_is_equal(trav,"annotation")) {
				/* TODO: <annotation> support */
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in schema)",trav->name);
			}
		}
		trav = trav->next;
	}		
	return TRUE;
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
static int schema_simpleType(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr simpleType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr name, ns;

	ns = get_attribute(simpleType->properties, "targetNamespace");
	if (ns == NULL) {
		ns = tsn;
	}

	name = get_attribute(simpleType->properties, "name");
	if (cur_type != NULL) {
		/* Anonymous type inside <element> or <restriction> */
		sdlTypePtr newType, *ptr;

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));
		newType->kind = XSD_TYPEKIND_SIMPLE;
		if (name != NULL) {
			newType->name = strdup(name->children->content);
			newType->namens = strdup(ns->children->content);
		} else {
			newType->name = strdup(cur_type->name);
			newType->namens = strdup(cur_type->namens);
		}

		zend_hash_next_index_insert(sdl->types,  &newType, sizeof(sdlTypePtr), (void **)&ptr);

		if (sdl->encoders == NULL) {
			sdl->encoders = malloc(sizeof(HashTable));
			zend_hash_init(sdl->encoders, 0, NULL, delete_encoder, 1);
		}
		cur_type->encode = malloc(sizeof(encode));
		memset(cur_type->encode, 0, sizeof(encode));
		cur_type->encode->details.ns = strdup(newType->namens);
		cur_type->encode->details.type_str = strdup(newType->name);
		cur_type->encode->details.sdl_type = *ptr;
		cur_type->encode->to_xml = sdl_guess_convert_xml;
		cur_type->encode->to_zval = sdl_guess_convert_zval;
		zend_hash_next_index_insert(sdl->encoders,  &cur_type->encode, sizeof(encodePtr), NULL);

		cur_type =*ptr;

	} else if (name != NULL) {
		sdlTypePtr newType, *ptr;

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));
		newType->kind = XSD_TYPEKIND_SIMPLE;
		newType->name = strdup(name->children->content);
		newType->namens = strdup(ns->children->content);

		if (cur_type == NULL) {
			zend_hash_next_index_insert(sdl->types,  &newType, sizeof(sdlTypePtr), (void **)&ptr);
		} else {
			if (cur_type->elements == NULL) {
				cur_type->elements = malloc(sizeof(HashTable));
				zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
			}
			zend_hash_update(cur_type->elements, newType->name, strlen(newType->name)+1, &newType, sizeof(sdlTypePtr), (void **)&ptr);
		}
		cur_type = (*ptr);

		create_encoder(sdl, cur_type, ns->children->content, name->children->content);
	} else {
		php_error(E_ERROR, "Error parsing schema (simpleType has no 'name' attribute)");
	}

	trav = simpleType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotation> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav,"restriction")) {
			schema_restriction_simpleContent(sdl, tsn, trav, cur_type, 1);
			trav = trav->next;
		} else if (node_is_equal(trav,"list")) {
			cur_type->kind = XSD_TYPEKIND_LIST;
			schema_list(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"union")) {
			cur_type->kind = XSD_TYPEKIND_UNION;
			schema_union(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else {
			php_error(E_ERROR, "Error parsing schema (unexpected <%s> in simpleType)",trav->name);
		}
	} else {
		php_error(E_ERROR, "Error parsing schema (expected <restriction>, <list> or <union> in simpleType)");
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in simpleType)",trav->name);
	}

	return TRUE;
}

/*
<list
  id = ID
  itemType = QName
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleType?))
</list>
*/
static int schema_list(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr listType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr itemType;

	itemType = get_attribute(listType->properties, "itemType");
	if (itemType != NULL) {
		char *type, *ns;
		xmlNsPtr nsptr;

		parse_namespace(itemType->children->content, &type, &ns);
		nsptr = xmlSearchNs(listType->doc, listType, ns);
		if (nsptr != NULL) {
			sdlTypePtr newType, *tmp;

			newType = malloc(sizeof(sdlType));
			memset(newType, 0, sizeof(sdlType));

			newType->name = strdup(type);
			newType->namens = strdup(nsptr->href);

			newType->encode = get_create_encoder(sdl, newType, (char *)nsptr->href, type);

			if (cur_type->elements == NULL) {
				cur_type->elements = malloc(sizeof(HashTable));
				zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
			}
			zend_hash_next_index_insert(cur_type->elements, &newType, sizeof(sdlTypePtr), (void **)&tmp);			
		}
		if (type) {efree(type);}
		if (ns) {efree(ns);}
	}

	trav = listType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotation> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"simpleType")) {
		sdlTypePtr newType, *tmp;

		if (itemType != NULL) {
			php_error(E_ERROR, "Error parsing schema (element have both 'itemType' attribute and subtype)");
		}

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));

		newType->name = strdup("anonymous");
		newType->namens = strdup(tsn->children->content);

		if (cur_type->elements == NULL) {
			cur_type->elements = malloc(sizeof(HashTable));
			zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
		}
		zend_hash_next_index_insert(cur_type->elements, &newType, sizeof(sdlTypePtr), (void **)&tmp);			

		schema_simpleType(sdl, tsn, trav, newType);
	  trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in list)",trav->name);
	}
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
static int schema_union(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr unionType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr memberTypes;

	memberTypes = get_attribute(unionType->properties, "memberTypes");
	if (memberTypes != NULL) {
		char *str, *start, *end, *next;
		char *type, *ns;
		xmlNsPtr nsptr;

		str = estrdup(memberTypes->children->content);
		whiteSpace_collapse(str);
		start = str;
		while (start != NULL && *start != '\0') {
			end = strchr(start,' ');
			if (end == NULL) {
			  next = NULL;
			} else {
			  *end = '\0';
			  next = end+1;
			}

			parse_namespace(start, &type, &ns);
			nsptr = xmlSearchNs(unionType->doc, unionType, ns);
			if (nsptr != NULL) {
				sdlTypePtr newType, *tmp;

				newType = malloc(sizeof(sdlType));
				memset(newType, 0, sizeof(sdlType));

				newType->name = strdup(type);
				newType->namens = strdup(nsptr->href);

				newType->encode = get_create_encoder(sdl, newType, (char *)nsptr->href, type);

				if (cur_type->elements == NULL) {
					cur_type->elements = malloc(sizeof(HashTable));
					zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
				}
				zend_hash_next_index_insert(cur_type->elements, &newType, sizeof(sdlTypePtr), (void **)&tmp);			
			}
			if (type) {efree(type);}
			if (ns) {efree(ns);}

			start = next;
		}
		efree(str);
	}

	trav = unionType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotation> support */
		trav = trav->next;
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"simpleType")) {
				sdlTypePtr newType, *tmp;

				if (memberTypes != NULL) {
					php_error(E_ERROR, "Error parsing schema (union have both 'memberTypes' attribute and subtypes)");
				}
				
				newType = malloc(sizeof(sdlType));
				memset(newType, 0, sizeof(sdlType));

				newType->name = strdup("anonymous");
				newType->namens = strdup(tsn->children->content);

				if (cur_type->elements == NULL) {
					cur_type->elements = malloc(sizeof(HashTable));
					zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
				}
				zend_hash_next_index_insert(cur_type->elements, &newType, sizeof(sdlTypePtr), (void **)&tmp);			

				schema_simpleType(sdl, tsn, trav, newType);

			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in union)",trav->name);
			}
		}
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in union)",trav->name);
	}
	return TRUE;
}

/*
<simpleContent
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (restriction | extension))
</simpleContent>
*/
static int schema_simpleContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr simpCompType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;

	trav = simpCompType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotation> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav, "restriction")) {
			schema_restriction_simpleContent(sdl, tsn, trav, cur_type, 0);
			trav = trav->next;
		} else if (node_is_equal(trav, "extension")) {
			schema_extension_simpleContent(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else {
			php_error(E_ERROR, "Error parsing schema (unexpected <%s> in simpleContent)",trav->name);
		}
	} else {
		php_error(E_ERROR, "Error parsing schema (expected <restriction> or <extension> in simpleContent)");
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in simpleContent)",trav->name);
	}

	return TRUE;
}

/*
simpleType:<restriction
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleType?, (minExclusive | minInclusive | maxExclusive | maxInclusive | totalDigits | fractionDigits | length | minLength | maxLength | enumeration | whiteSpace | pattern)*)?)
</restriction>
simpleContent:<restriction
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, (simpleType?, (minExclusive | minInclusive | maxExclusive | maxInclusive | totalDigits | fractionDigits | length | minLength | maxLength | enumeration | whiteSpace | pattern)*)?, ((attribute | attributeGroup)*, anyAttribute?))
</restriction>
*/
static int schema_restriction_simpleContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type, int simpleType)
{
	xmlNodePtr trav;
	xmlAttrPtr base;

	base = get_attribute(restType->properties, "base");
	if (base != NULL) {
		char *type, *ns;
		xmlNsPtr nsptr;

		parse_namespace(base->children->content, &type, &ns);
		nsptr = xmlSearchNs(restType->doc, restType, ns);
		if (nsptr != NULL) {
			cur_type->encode = get_create_encoder(sdl, cur_type, (char *)nsptr->href, type);
		}
		if (type) {efree(type);}
		if (ns) {efree(ns);}
	} else if (!simpleType) {
		php_error(E_ERROR, "Error parsing schema (restriction has no 'base' attribute)");
	}

	if (cur_type->restrictions == NULL) {
		cur_type->restrictions = malloc(sizeof(sdlRestrictions));
		memset(cur_type->restrictions, 0, sizeof(sdlRestrictions));
	}

	trav = restType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
	  trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav, "annotaion")) {
		/* TODO: <annotation> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
	  trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav, "simpleType")) {
		schema_simpleType(sdl, tsn, trav, cur_type);
		trav = trav->next;
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (!strcmp(trav->name, "minExclusive")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->minExclusive);
			} else if (!strcmp(trav->name, "minInclusive")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->minInclusive);
			} else if (!strcmp(trav->name, "maxExclusive")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->maxExclusive);
			} else if (!strcmp(trav->name, "maxInclusive")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->maxInclusive);
			} else if (!strcmp(trav->name, "totalDigits")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->totalDigits);
			} else if (!strcmp(trav->name, "fractionDigits")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->fractionDigits);
			} else if (!strcmp(trav->name, "length")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->length);
			} else if (!strcmp(trav->name, "minLength")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->minLength);
			} else if (!strcmp(trav->name, "maxLength")) {
				schema_restriction_var_int(trav, &cur_type->restrictions->maxLength);
			} else if (!strcmp(trav->name, "whiteSpace")) {
				schema_restriction_var_char(trav, &cur_type->restrictions->whiteSpace);
			} else if (!strcmp(trav->name, "pattern")) {
				schema_restriction_var_char(trav, &cur_type->restrictions->pattern);
			} else if (!strcmp(trav->name, "enumeration")) {
				sdlRestrictionCharPtr enumval = NULL;

				schema_restriction_var_char(trav, &enumval);
				if (cur_type->restrictions->enumeration == NULL) {
					cur_type->restrictions->enumeration = malloc(sizeof(HashTable));
					zend_hash_init(cur_type->restrictions->enumeration, 0, NULL, delete_schema_restriction_var_char, 1);
				}
				zend_hash_add(cur_type->restrictions->enumeration, enumval->value, strlen(enumval->value)+1, &enumval, sizeof(sdlRestrictionCharPtr), NULL);
			} else {
				break;
			}
		}
	  trav = trav->next;
	}
	if (!simpleType) {
		while (trav != NULL) {
			if (trav->type == XML_ELEMENT_NODE) {
				if (node_is_equal(trav,"attribute")) {
					schema_attribute(sdl, tsn, trav, cur_type);
				} else if (node_is_equal(trav,"attributeGroup")) {
					schema_attributeGroupRef(sdl, tsn, trav, cur_type);
				} else if (node_is_equal(trav,"anyAttribute")) {
					/* TODO: <anyAttribute> support */
					trav = trav->next;
					break;
				} else {
					php_error(E_ERROR, "Error parsing schema (unexpected <%s> in restriction)",trav->name);
				}
			}
			trav = trav->next;
		}
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in restriction)",trav->name);
	}

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
static int schema_restriction_complexContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type)
{
	xmlAttrPtr base;
	xmlNodePtr trav;

	base = get_attribute(restType->properties, "base");
	if (base != NULL) {
		char *type, *ns;
		xmlNsPtr nsptr;

		parse_namespace(base->children->content, &type, &ns);
		nsptr = xmlSearchNs(restType->doc, restType, ns);
		if (nsptr != NULL) {
			cur_type->encode = get_create_encoder(sdl, cur_type, (char *)nsptr->href, type);
		}
		if (type) {efree(type);}
		if (ns) {efree(ns);}
	} else {
		php_error(E_ERROR, "Error parsing schema (restriction has no 'base' attribute)");
	}

	trav = restType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotaion> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav,"group")) {
			schema_group(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"all")) {
			cur_type->kind = XSD_TYPEKIND_ALL;
			schema_all(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"choice")) {
			cur_type->kind = XSD_TYPEKIND_CHOICE;
			schema_choice(sdl, tsn, trav, cur_type);
			trav = trav->next;
			cur_type->kind = XSD_TYPEKIND_SEQUENCE;
		} else if (node_is_equal(trav,"sequence")) {
			schema_sequence(sdl, tsn, trav, cur_type);
			trav = trav->next;
		}
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"attribute")) {
				schema_attribute(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"attributeGroup")) {
				schema_attributeGroupRef(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"anyAttribute")) {
				/* TODO: <anyAttribute> support */
				trav = trav->next;
				break;
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in restriction)",trav->name);
			}
		}
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in restriction)",trav->name);
	}

	return TRUE;
}

static int schema_restriction_var_int(xmlNodePtr val, sdlRestrictionIntPtr *valptr)
{
	xmlAttrPtr fixed, value, id;

	if ((*valptr) == NULL) {
		(*valptr) = malloc(sizeof(sdlRestrictionInt));
	}
	memset((*valptr), 0, sizeof(sdlRestrictionInt));

	fixed = get_attribute(val->properties, "fixed");
	(*valptr)->fixed = FALSE;
	if (fixed != NULL) {
		if (!strcmp(fixed->children->content, "true") ||
			!strcmp(fixed->children->content, "1"))
			(*valptr)->fixed = TRUE;
	}

	id = get_attribute(val->properties, "id");
	if (id != NULL) {
		(*valptr)->id = strdup(id->children->content);
	}

	value = get_attribute(val->properties, "value");
	if (value == NULL) {
		php_error(E_ERROR, "Error parsing wsdl (missing restriction value)");
	}

	(*valptr)->value = atoi(value->children->content);

	return TRUE;
}

static int schema_restriction_var_char(xmlNodePtr val, sdlRestrictionCharPtr *valptr)
{
	xmlAttrPtr fixed, value, id;

	if ((*valptr) == NULL) {
		(*valptr) = malloc(sizeof(sdlRestrictionChar));
	}
	memset((*valptr), 0, sizeof(sdlRestrictionChar));

	fixed = get_attribute(val->properties, "fixed");
	(*valptr)->fixed = FALSE;
	if (fixed != NULL) {
		if (!strcmp(fixed->children->content, "true") ||
		    !strcmp(fixed->children->content, "1")) {
			(*valptr)->fixed = TRUE;
		}
	}

	id = get_attribute(val->properties, "id");
	if (id != NULL) {
		(*valptr)->id = strdup(id->children->content);
	}

	value = get_attribute(val->properties, "value");
	if (value == NULL) {
		php_error(E_ERROR, "Error parsing wsdl (missing restriction value)");
	}

	(*valptr)->value = strdup(value->children->content);
	return TRUE;
}

/*
From simpleContent (not supported):
<extension
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, ((attribute | attributeGroup)*, anyAttribute?))
</extension>
*/
static int schema_extension_simpleContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr base;

	base = get_attribute(extType->properties, "base");
	if (base != NULL) {
		/*FIXME*/
	} else {
		php_error(E_ERROR, "Error parsing schema (extension has no 'base' attribute)");
	}

	trav = extType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotaion> support */
		trav = trav->next;
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"attribute")) {
				schema_attribute(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"attributeGroup")) {
				schema_attributeGroupRef(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"anyAttribute")) {
				/* TODO: <anyAttribute> support */
				trav = trav->next;
				break;
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in extension)",trav->name);
			}
		}
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in extension)",trav->name);
	}
	return TRUE;
}

/*
From complexContent:
<extension
  base = QName
  id = ID
  {any attributes with non-schema namespace . . .}>
  Content: (annotation?, ((group | all | choice | sequence)?, ((attribute | attributeGroup)*, anyAttribute?)))
</extension>
*/
static int schema_extension_complexContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr base;

	base = get_attribute(extType->properties, "base");
	if (base != NULL) {
		/*FIXME*/
	} else {
		php_error(E_ERROR, "Error parsing schema (extension has no 'base' attribute)");
	}

	trav = extType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotaion> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav,"group")) {
			schema_group(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"all")) {
			cur_type->kind = XSD_TYPEKIND_ALL;
			schema_all(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"choice")) {
			cur_type->kind = XSD_TYPEKIND_CHOICE;
			schema_choice(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"sequence")) {
			cur_type->kind = XSD_TYPEKIND_SEQUENCE;
			schema_sequence(sdl, tsn, trav, cur_type);
			trav = trav->next;
		}
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"attribute")) {
				schema_attribute(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"attributeGroup")) {
				schema_attributeGroupRef(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"anyAttribute")) {
				/* TODO: <anyAttribute> support */
				trav = trav->next;
				break;
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in extension)",trav->name);
			}
		}
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in extension)",trav->name);
	}
	return TRUE;
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
static int schema_all(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr all, sdlTypePtr cur_type)
{
	xmlNodePtr trav;

	trav = all->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotaion> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"element")) {
				schema_element(sdl, tsn, trav, cur_type);
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in all)",trav->name);
			}
		}
		trav = trav->next;
	}
	return TRUE;
}

/*
<group
  name = NCName>
  Content: (annotation?, (all | choice | sequence))
</group>
*/
static int schema_group(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr groupType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr name;

	name = get_attribute(groupType->properties, "name");
	if (name != NULL) {
		/*FIXME*/
	}

	trav = groupType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotaion> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav,"choice")) {
			cur_type->kind = XSD_TYPEKIND_CHOICE;
			schema_choice(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"sequence")) {
			cur_type->kind = XSD_TYPEKIND_SEQUENCE;
			schema_sequence(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"all")) {
			cur_type->kind = XSD_TYPEKIND_ALL;
			schema_all(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else {
			php_error(E_ERROR, "Error parsing schema (unexpected <%s> in group)",trav->name);
		}
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in group)",trav->name);
	}
	return TRUE;
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
static int schema_choice(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr choiceType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;

	/*
	cur_type->property_type = CHOICE;
	*/

	trav = choiceType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotaion> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"element")) {
				schema_element(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"grouop")) {
				/*FIXME*/
				schema_group(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"choice")) {
				/*FIXME*/
				schema_choice(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"sequence")) {
				/*FIXME*/
				schema_sequence(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"any")) {
				/*FIXME*/
				schema_any(sdl, tsn, trav, cur_type);
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in choice)",trav->name);
			}
		}
		trav = trav->next;
	}
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
static int schema_sequence(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr seqType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;

	trav = seqType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotaion> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"element")) {
				schema_element(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"grouop")) {
				/*FIXME*/
				schema_group(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"choice")) {
				/*FIXME*/
				schema_choice(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"sequence")) {
				/*FIXME*/
				schema_sequence(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"any")) {
				/*FIXME*/
				schema_any(sdl, tsn, trav, cur_type);
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in sequence)",trav->name);
			}
		}
		trav = trav->next;
	}
	return TRUE;
}

static int schema_any(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type)
{
	/* TODO: <any> support */
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
static int schema_complexContent(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr compCont, sdlTypePtr cur_type)
{
	xmlNodePtr trav;

	trav = compCont->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav,"annotation")) {
		/* TODO: <annotation> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav, "restriction")) {
			schema_restriction_complexContent(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav, "extension")) {
			schema_extension_complexContent(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else {
			php_error(E_ERROR, "Error parsing schema (unexpected <%s> in complexContent)",trav->name);
		}
	} else {
		php_error(E_ERROR, "Error parsing schema (<restriction> or <extension> expected in complexContent)");
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in complexContent)", trav->name);
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
static int schema_complexType(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr compType, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr attrs, name, ns;
	TSRMLS_FETCH();

	attrs = compType->properties;
	ns = get_attribute(attrs, "targetNamespace");
	if (ns == NULL) {
		ns = tsn;
	}

	name = get_attribute(attrs, "name");
	if (cur_type != NULL) {
		/* Anonymous type inside <element> */
		sdlTypePtr newType, *ptr;

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));
		newType->kind = XSD_TYPEKIND_COMPLEX;
		if (name != NULL) {
			newType->name = strdup(name->children->content);
			newType->namens = strdup(ns->children->content);
		} else {
			newType->name = strdup(cur_type->name);
			newType->namens = strdup(cur_type->namens);
		}

		zend_hash_next_index_insert(sdl->types,  &newType, sizeof(sdlTypePtr), (void **)&ptr);

		if (sdl->encoders == NULL) {
			sdl->encoders = malloc(sizeof(HashTable));
			zend_hash_init(sdl->encoders, 0, NULL, delete_encoder, 1);
		}
		cur_type->encode = malloc(sizeof(encode));
		memset(cur_type->encode, 0, sizeof(encode));
		cur_type->encode->details.ns = strdup(newType->namens);
		cur_type->encode->details.type_str = strdup(newType->name);
		cur_type->encode->details.sdl_type = *ptr;
		cur_type->encode->to_xml = sdl_guess_convert_xml;
		cur_type->encode->to_zval = sdl_guess_convert_zval;
		zend_hash_next_index_insert(sdl->encoders,  &cur_type->encode, sizeof(encodePtr), NULL);

		cur_type =*ptr;

	} else if (name) {
		sdlTypePtr newType, *ptr;

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));
		newType->kind = XSD_TYPEKIND_COMPLEX;
		newType->name = strdup(name->children->content);
		newType->namens = strdup(ns->children->content);

		zend_hash_next_index_insert(sdl->types,  &newType, sizeof(sdlTypePtr), (void **)&ptr);

		cur_type = (*ptr);
		create_encoder(sdl, cur_type, ns->children->content, name->children->content);
/*
		if (cur_type->encode == NULL) {
			cur_type->encode = get_conversion(SOAP_ENC_OBJECT);
		}
*/
	} else {
		php_error(E_ERROR, "Error parsing schema (complexType has no 'name' attribute)");
		return FALSE;
	}

	trav = compType->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL && node_is_equal(trav, "annotation")) {
		/* TODO: <annotation> support */
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav,"simpleContent")) {
			schema_simpleContent(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"complexContent")) {
			schema_complexContent(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else {
			if (node_is_equal(trav,"group")) {
				schema_group(sdl, tsn, trav, cur_type);
				trav = trav->next;
			} else if (node_is_equal(trav,"all")) {
				cur_type->kind = XSD_TYPEKIND_ALL;
				schema_all(sdl, tsn, trav, cur_type);
				trav = trav->next;
			} else if (node_is_equal(trav,"choice")) {
				cur_type->kind = XSD_TYPEKIND_CHOICE;
				schema_choice(sdl, tsn, trav, cur_type);
				trav = trav->next;
			} else if (node_is_equal(trav,"sequence")) {
				cur_type->kind = XSD_TYPEKIND_SEQUENCE;
				schema_sequence(sdl, tsn, trav, cur_type);
				trav = trav->next;
			}
			while (trav != NULL) {
				if (trav->type == XML_ELEMENT_NODE) {
					if (node_is_equal(trav,"attribute")) {
						schema_attribute(sdl, tsn, trav, cur_type);
					} else if (node_is_equal(trav,"attributeGroup")) {
						schema_attributeGroupRef(sdl, tsn, trav, cur_type);
					} else if (node_is_equal(trav,"anyAttribute")) {
						/* TODO: <anyAttribute> support */
						trav = trav->next;
						break;
					} else {
						php_error(E_ERROR, "Error parsing schema ---(unexpected <%s> in complexType)",trav->name);
					}
				}
				trav = trav->next;
			}
		}
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema +++(unexpected <%s> in complexType)",trav->name);
	}
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
static int schema_element(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr element, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr attrs, curattr, ns, name, type, ref = NULL;

	attrs = element->properties;
	ns = get_attribute(attrs, "targetNamespace");
	if (ns == NULL) {
		ns = tsn;
	}

	name = get_attribute(attrs, "name");
	if (name == NULL) {
		name = ref = get_attribute(attrs, "ref");
	}

	if (name) {
		HashTable *addHash;
		sdlTypePtr newType, *tmp;
		smart_str key = {0};

		newType = malloc(sizeof(sdlType));
		memset(newType, 0, sizeof(sdlType));

		if (ref) {
			smart_str nscat = {0};
			char *type, *ns;
			xmlNsPtr nsptr;

			parse_namespace(ref->children->content, &type, &ns);
			nsptr = xmlSearchNs(element->doc, element, ns);
			if (nsptr != NULL) {
				smart_str_appends(&nscat, nsptr->href);
				smart_str_appendc(&nscat, ':');
				newType->namens = strdup(nsptr->href);
			}
			smart_str_appends(&nscat, type);
			newType->name = strdup(type);
			smart_str_0(&nscat);
			if (type) {efree(type);}
			if (ns) {efree(ns);}
			newType->ref = estrdup(nscat.c);
			smart_str_free(&nscat);
		} else {
			newType->name = strdup(name->children->content);
			newType->namens = strdup(ns->children->content);
		}

		newType->nillable = FALSE;
		newType->min_occurs = 1;
		newType->max_occurs = 1;

		if (cur_type == NULL) {
			addHash = sdl->elements;
			smart_str_appends(&key, newType->namens);
			smart_str_appendc(&key, ':');
			smart_str_appends(&key, newType->name);
		} else {
			if (cur_type->elements == NULL) {
				cur_type->elements = malloc(sizeof(HashTable));
				zend_hash_init(cur_type->elements, 0, NULL, delete_type, 1);
			}
			addHash = cur_type->elements;
			smart_str_appends(&key, newType->name);
		}

		smart_str_0(&key);
		zend_hash_update(addHash, key.c, key.len + 1, &newType, sizeof(sdlTypePtr), (void **)&tmp);
		cur_type = (*tmp);
		smart_str_free(&key);
	} else {
		php_error(E_ERROR, "Error parsing schema (element has no 'name' nor 'ref' attributes)");
	}

	curattr = get_attribute(attrs, "maxOccurs");
	if (curattr) {
		if (!strcmp(curattr->children->content, "unbounded")) {
			cur_type->max_occurs = -1;
		} else {
			cur_type->max_occurs = atoi(curattr->children->content);
		}
	}

	curattr = get_attribute(attrs, "minOccurs");
	if (curattr) {
		cur_type->min_occurs = atoi(curattr->children->content);
	}

	/* nillable = boolean : false */
	attrs = element->properties;
	curattr = get_attribute(attrs, "nillable");
	if (curattr) {
		if (!stricmp(curattr->children->content, "true") ||
			!stricmp(curattr->children->content, "1")) {
			cur_type->nillable = TRUE;
		} else {
			cur_type->nillable = FALSE;
		}
	} else {
		cur_type->nillable = FALSE;
	}

	/* type = QName */
	type = get_attribute(attrs, "type");
	if (type) {
		char *cptype, *str_ns;
		xmlNsPtr nsptr;

		if (ref != NULL) {
			php_error(E_ERROR, "Error parsing schema (element have both 'ref' and 'type' attributes)");
		}
		parse_namespace(type->children->content, &cptype, &str_ns);
		nsptr = xmlSearchNs(element->doc, element, str_ns);
		if (nsptr != NULL) {
			cur_type->encode = get_create_encoder(sdl, cur_type, (char *)nsptr->href, (char *)cptype);
		}
		if (str_ns) {efree(str_ns);}
		if (cptype) {efree(cptype);}
	}

/*
	if (cur_type->max_occurs == -1 || cur_type->max_occurs > 1) {
		cur_type->encode = get_conversion(SOAP_ENC_ARRAY);
	}
*/

	trav = element->children;
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		if (node_is_equal(trav,"simpleType")) {
			if (ref != NULL) {
				php_error(E_ERROR, "Error parsing schema (element have both 'ref' attribute and subtype)");
			} else if (type != NULL) {
				php_error(E_ERROR, "Error parsing schema (element have both 'type' attribute and subtype)");
			}
			schema_simpleType(sdl, tsn, trav, cur_type);
			trav = trav->next;
		} else if (node_is_equal(trav,"complexType")) {
			if (ref != NULL) {
				php_error(E_ERROR, "Error parsing schema (element have both 'ref' attribute and subtype)");
			} else if (type != NULL) {
				php_error(E_ERROR, "Error parsing schema (element have both 'type' attribute and subtype)");
			}
			schema_complexType(sdl, tsn, trav, cur_type);
			trav = trav->next;
		}
	}
	while (trav != NULL) {
		if (trav->type ==	XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"unique")) {
				/* TODO: <unique> support */
			} else if (node_is_equal(trav,"key")) {
				/* TODO: <key> support */
			} else if (node_is_equal(trav,"key")) {
				/* TODO: <keyref> support */
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in element)",trav->name);
			}			
		}
		trav = trav->next;
	}

	return TRUE;
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
static int schema_attribute(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr attrType, sdlTypePtr cur_type)
{
	xmlAttrPtr attr;
	sdlAttributePtr newAttr;
	xmlAttrPtr trav;
	smart_str key = {0};

	newAttr = malloc(sizeof(sdlAttribute));
	memset(newAttr, 0, sizeof(sdlAttribute));

	if (cur_type->attributes == NULL) {
		cur_type->attributes = malloc(sizeof(HashTable));
		zend_hash_init(cur_type->attributes, 0, NULL, delete_attribute, 1);
	}

	trav = attrType->properties;
	FOREACHATTRNODE(trav, NULL, attr) {
		if (attr_is_equal_ex(trav, "default", SCHEMA_NAMESPACE)) {
			newAttr->def = strdup(attr->children->content);
		} else if (attr_is_equal_ex(trav, "fixed", SCHEMA_NAMESPACE)) {
			newAttr->fixed = strdup(attr->children->content);
		} else if (attr_is_equal_ex(trav, "form", SCHEMA_NAMESPACE)) {
			newAttr->form = strdup(attr->children->content);
		} else if (attr_is_equal_ex(trav, "id", SCHEMA_NAMESPACE)) {
			newAttr->id = strdup(attr->children->content);
		} else if (attr_is_equal_ex(trav, "name", SCHEMA_NAMESPACE)) {
			newAttr->name = strdup(attr->children->content);
		} else if (attr_is_equal_ex(trav, "ref", SCHEMA_NAMESPACE)) {
			newAttr->ref= strdup(attr->children->content);
		} else if (attr_is_equal_ex(trav, "type", SCHEMA_NAMESPACE)) {
			newAttr->type = strdup(attr->children->content);
		} else if (attr_is_equal_ex(trav, "use", SCHEMA_NAMESPACE)) {
			newAttr->use = strdup(attr->children->content);
		} else {
			xmlNsPtr nsPtr = attr_find_ns(trav);

			if (strcmp(nsPtr->href, SCHEMA_NAMESPACE)) {
				smart_str key2 = {0};

				if (!newAttr->extraAttributes) {
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


	if (newAttr->ref || newAttr->name) {
		xmlNsPtr ns;

		if (newAttr->ref) {
			char *value, *prefix = NULL;

			parse_namespace(newAttr->ref, &value, &prefix);
			ns = xmlSearchNs(attrType->doc, attrType, prefix);
			if (ns != NULL) {
				smart_str_appends(&key, ns->href);
				smart_str_appendc(&key, ':');
			}
			smart_str_appends(&key, value);

			if (value) {efree(value);}
			if (prefix) {efree(prefix);}
		} else {
			ns = node_find_ns(attrType);
			if (ns != NULL) {
				smart_str_appends(&key, ns->href);
				smart_str_appendc(&key, ':');
			}
			smart_str_appends(&key, newAttr->name);
		}

		if (ns) {
			smart_str_0(&key);
			zend_hash_add(cur_type->attributes, key.c, key.len + 1, &newAttr, sizeof(sdlAttributePtr), NULL);
			smart_str_free(&key);
			return TRUE;
		}
		smart_str_free(&key);
	} else {
		php_error(E_ERROR, "Error parsing schema (attribute has no 'name' nor 'ref' attributes)");
	}

	zend_hash_next_index_insert(cur_type->attributes, &newAttr, sizeof(sdlAttributePtr), NULL);
	return TRUE;
}

static int schema_attributeGroup(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr attrGroup, sdlTypePtr cur_type)
{
	xmlNodePtr trav;
	xmlAttrPtr name;

	if ((name = get_attribute(attrGroup->properties, "name")) != NULL) {
		/*FIXME*/
	} else if ((name = get_attribute(attrGroup->properties, "ref")) != NULL) {
		/*FIXME*/
	} else {
		php_error(E_ERROR, "Error parsing schema (attributeGroup has no 'name nor 'ref'' attribute)");
	}
	trav = attrGroup->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (node_is_equal(trav,"attribute")) {
				schema_attribute(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"attributeGroup")) {
				schema_attributeGroupRef(sdl, tsn, trav, cur_type);
			} else if (node_is_equal(trav,"anyAttribute")) {
				/* TODO: <anyAttribute> support */
				trav = trav->next;
				break;
			} else {
				php_error(E_ERROR, "Error parsing schema (unexpected <%s> in attributeGroup)",trav->name);
			}
		}
		trav = trav->next;
	}
	while (trav != NULL && trav->type != XML_ELEMENT_NODE) {
		trav = trav->next;
	}
	if (trav != NULL) {
		php_error(E_ERROR, "Error parsing schema (unexpected <%s> in attributeGroup)",trav->name);
	}
	return TRUE;
}

static int schema_attributeGroupRef(sdlPtr sdl, xmlAttrPtr tsn, xmlNodePtr attrGroup, sdlTypePtr cur_type)
{
	xmlAttrPtr ref = get_attribute(attrGroup->properties, "ref");
	if (ref != NULL) {
		/*FIXME*/
	} else {
		php_error(E_ERROR, "Error parsing schema (attributeGroup has no 'ref' attribute)");
	}
	return TRUE;
}

static void schema_type_fixup(sdlPtr sdl, sdlTypePtr type)
{
	sdlTypePtr *tmp;

	if (type->ref != NULL) {
		if (zend_hash_find(sdl->elements, type->ref, strlen(type->ref)+1, (void**)&tmp) == SUCCESS) {			
			type->encode = (*tmp)->encode;
			/* TODO: nillable minOccurs, maxOccurs */
		} else {
			php_error(E_ERROR, "Error parsing schema (unresolved element 'ref' attribute)");
		}
		efree(type->ref);
		type->ref = NULL;
	}
	if (type->elements) {
		zend_hash_internal_pointer_reset(type->elements);
		while (zend_hash_get_current_data(type->elements,(void**)&tmp) == SUCCESS) {
			schema_type_fixup(sdl,*tmp);
			zend_hash_move_forward(type->elements);
		}
	}
}

int schema_pass2(sdlPtr sdl)
{
	sdlTypePtr *type;

	if (sdl->elements) {
		zend_hash_internal_pointer_reset(sdl->elements);
		while (zend_hash_get_current_data(sdl->elements,(void**)&type) == SUCCESS) {
			schema_type_fixup(sdl,*type);
			zend_hash_move_forward(sdl->elements);
		}
	}
	if (sdl->types) {
		zend_hash_internal_pointer_reset(sdl->types);
		while (zend_hash_get_current_data(sdl->types,(void**)&type) == SUCCESS) {
			schema_type_fixup(sdl,*type);
			zend_hash_move_forward(sdl->types);
		}
	}
	return TRUE;
}
