
	/* $Id: xml_config.c,v 1.9 2008/08/26 15:09:15 anight Exp $ */
	/* (c) 2004-2007 Andrei Nigmatulin */

#include "fpm_config.h"

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "xml_config.h"

static struct xml_conf_section **xml_conf_sections = 0;
static int xml_conf_sections_allocated = 0;
static int xml_conf_sections_used = 0;

char *xml_conf_set_slot_boolean(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;
	long value_y = !strcasecmp(value, "yes") || !strcmp(value,  "1") || !strcasecmp(value, "on");
	long value_n = !strcasecmp(value, "no")  || !strcmp(value,  "0") || !strcasecmp(value, "off");

	if (!value_y && !value_n) {
		return "xml_conf_set_slot(): invalid boolean value";
	}

#ifdef XML_CONF_DEBUG
	fprintf(stderr, "setting boolean '%s' => %s\n", name, value_y ? "TRUE" : "FALSE");
#endif

	* (int *) ((char *) *conf + offset) = value_y ? 1 : 0;

	return NULL;
}

char *xml_conf_set_slot_string(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;
	char *v = strdup(value);

	if (!v) return "xml_conf_set_slot_string(): strdup() failed";

#ifdef XML_CONF_DEBUG
	fprintf(stderr, "setting string '%s' => '%s'\n", name, v);
#endif

	* (char **) ((char *) *conf + offset) = v;

	return NULL;
}

char *xml_conf_set_slot_integer(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;
	int v = atoi(value);

	* (int *) ((char *) *conf + offset) = v;

#ifdef XML_CONF_DEBUG
	fprintf(stderr, "setting integer '%s' => %d\n", name, v);
#endif

	return NULL;
}

char *xml_conf_set_slot_time(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;
	int len = strlen(value);
	char suffix;
	int seconds;

	if (!len) return "xml_conf_set_slot_timeval(): invalid timeval value";

	suffix = value[len-1];

	value[len-1] = '\0';

	switch (suffix) {
		case 's' :
			seconds = atoi(value);
			break;
		case 'm' :
			seconds = 60 * atoi(value);
			break;
		case 'h' :
			seconds = 60 * 60 * atoi(value);
			break;
		case 'd' :
			seconds = 24 * 60 * 60 * atoi(value);
			break;
		default :
			return "xml_conf_set_slot_timeval(): unknown suffix used in timeval value";
	}

	* (int *) ((char *) *conf + offset) = seconds;

#ifdef XML_CONF_DEBUG
	fprintf(stderr, "setting time '%s' => %d:%02d:%02d:%02d\n", name, expand_dhms(seconds));
#endif

	return NULL;
}

char *xml_conf_parse_section(void **conf, struct xml_conf_section *section, void *xml_node)
{
	xmlNode *element = xml_node;
	char *ret = 0;

#ifdef XML_CONF_DEBUG
	fprintf(stderr, "processing a section %s\n", section->path);
#endif

	for ( ; element; element = element->next) {
		if (element->type == XML_ELEMENT_NODE && !strcmp((const char *) element->name, "value") && element->children) {
			xmlChar *name = xmlGetProp(element, (unsigned char *) "name");

			if (name) {
				int i;

#ifdef XML_CONF_DEBUG
				fprintf(stderr, "found a value: %s\n", name);
#endif
				for (i = 0; section->parsers[i].parser; i++) {
					if (!section->parsers[i].name || !strcmp(section->parsers[i].name, (char *) name)) {
						break;
					}
				}

				if (section->parsers[i].parser) {
					if (section->parsers[i].type == XML_CONF_SCALAR) {
						if (element->children->type == XML_TEXT_NODE) {
							ret = section->parsers[i].parser(conf, (char *) name, element->children->content, section->parsers[i].offset);
						}
						else {
							ret = "XML_TEXT_NODE is expected, something different is given";
						}
					}
					else {
						ret = section->parsers[i].parser(conf, (char *) name, element->children, section->parsers[i].offset);
					}

					xmlFree(name);
					if (ret) return ret;
					else continue;
				}

				fprintf(stderr, "Warning, unknown setting '%s' in section '%s'\n", (char *) name, section->path);

				xmlFree(name);
			}
		}
	}

	return NULL;
}

static char *xml_conf_parse_file(xmlNode *element)
{
	char *ret = 0;

	for ( ; element; element = element->next) {

		if (element->parent && element->type == XML_ELEMENT_NODE && !strcmp((const char *) element->name, "section")) {
			xmlChar *name = xmlGetProp(element, (unsigned char *) "name");

			if (name) {
				char *parent_name = (char *) xmlGetNodePath(element->parent);
				char *full_name;
				int i;
				struct xml_conf_section *section = NULL;

#ifdef XML_CONF_DEBUG
				fprintf(stderr, "got a section: %s/%s\n", parent_name, name);
#endif
				full_name = alloca(strlen(parent_name) + strlen((char *) name) + 1 + 1);

				sprintf(full_name, "%s/%s", parent_name, (char *) name);

				xmlFree(parent_name);
				xmlFree(name);

				for (i = 0; i < xml_conf_sections_used; i++) {
					if (!strcmp(xml_conf_sections[i]->path, full_name)) {
						section = xml_conf_sections[i];
					}
				}

				if (section) { /* found a registered section */
					void *conf = section->conf();
					ret = xml_conf_parse_section(&conf, section, element->children);
					if (ret) break;
				}

			}
		}

		if (element->children) {
			ret = xml_conf_parse_file(element->children);
			if (ret) break;
		}
	}

	return ret;
}

char *xml_conf_load_file(char *file)
{
	char *ret = 0;
	xmlDoc *doc;

	LIBXML_TEST_VERSION

	doc = xmlParseFile(file);

	if (doc) {
		ret = xml_conf_parse_file(doc->children);
		xmlFreeDoc(doc);
	}
	else {
		ret = "failed to parse conf file";
	}

	xmlCleanupParser();
	return ret;
}

int xml_conf_init()
{
	return 0;
}

void xml_conf_clean()
{
	if (xml_conf_sections) {
		free(xml_conf_sections);
	}
}

int xml_conf_section_register(struct xml_conf_section *section)
{
	if (xml_conf_sections_allocated == xml_conf_sections_used) {
		int new_size = xml_conf_sections_used + 10;
		void *new_ptr = realloc(xml_conf_sections, sizeof(struct xml_conf_section *) * new_size);

		if (new_ptr) {
			xml_conf_sections = new_ptr;
			xml_conf_sections_allocated = new_size;
		}
		else {
			fprintf(stderr, "xml_conf_section_register(): out of memory\n");
			return -1;
		}
	}

	xml_conf_sections[xml_conf_sections_used++] = section;

	return 0;
}

int xml_conf_sections_register(struct xml_conf_section *sections[])
{
	for ( ; sections && *sections; sections++) {
		if (0 > xml_conf_section_register(*sections)) {
			return -1;
		}
	}

	return 0;
}

