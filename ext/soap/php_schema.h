#ifndef PHP_SCHEMA_H
#define PHP_SCHEMA_H

int load_schema(sdlPtr *sdl,xmlNodePtr schema);
int schema_simpleType(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr simpleType, sdlTypePtr cur_type);
int schema_complexType(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr compType, sdlTypePtr cur_type);
int schema_sequence(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr seqType, sdlTypePtr cur_type);
int schema_list(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr listType, sdlTypePtr cur_type);
int schema_union(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr unionType, sdlTypePtr cur_type);
int schema_simpleContent(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr simpCompType, sdlTypePtr cur_type);
int schema_restriction_simpleType(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type);
int schema_restriction_simpleContent(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type);
int schema_restriction_complexContent(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr restType, sdlTypePtr cur_type);
int schema_extension(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type);
int schema_all(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type);
int schema_group(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr groupType, sdlTypePtr cur_type);
int schema_choice(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr choiceType, sdlTypePtr cur_type);
int schema_element(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr element, sdlTypePtr cur_type);
int schema_attribute(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr attrType, sdlTypePtr cur_type);
int schema_any(sdlPtr *sdl, xmlAttrPtr tsn, xmlNodePtr extType, sdlTypePtr cur_type);

int schema_restriction_var_int(xmlNodePtr val, sdlRestrictionIntPtr *valptr);
void delete_restriction_var_int(void *rvi);

int schema_restriction_var_char(xmlNodePtr val, sdlRestrictionCharPtr *valptr);
void delete_schema_restriction_var_char(void *srvc);

#endif
