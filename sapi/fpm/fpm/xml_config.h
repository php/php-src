
	/* $Id: xml_config.h,v 1.3 2008/09/18 23:02:58 anight Exp $ */
	/* (c) 2004-2007 Andrei Nigmatulin */

#ifndef XML_CONFIG_H
#define XML_CONFIG_H 1

#include <stdint.h>

struct xml_value_parser;

struct xml_value_parser {
	int type;
	char *name;
	char *(*parser)(void **, char *, void *, intptr_t offset);
	intptr_t offset;
};

struct xml_conf_section {
	void *(*conf)();
	char *path;
	struct xml_value_parser *parsers;
};

char *xml_conf_set_slot_boolean(void **conf, char *name, void *value, intptr_t offset);
char *xml_conf_set_slot_string(void **conf, char *name, void *value, intptr_t offset);
char *xml_conf_set_slot_integer(void **conf, char *name, void *value, intptr_t offset);
char *xml_conf_set_slot_time(void **conf, char *name, void *value, intptr_t offset);

int xml_conf_init();
void xml_conf_clean();
char *xml_conf_load_file(char *file);
char *xml_conf_parse_section(void **conf, struct xml_conf_section *section, void *ve);
int xml_conf_section_register(struct xml_conf_section *section);
int xml_conf_sections_register(struct xml_conf_section *sections[]);

#define expand_hms(value) (value) / 3600, ((value) % 3600) / 60, (value) % 60

#define expand_dhms(value) (value) / 86400, ((value) % 86400) / 3600, ((value) % 3600) / 60, (value) % 60

enum { XML_CONF_SCALAR = 1, XML_CONF_SUBSECTION = 2 };

#endif
