#ifndef PHP_SCHEMA_H
#define PHP_SCHEMA_H

int load_schema(sdlPtr *sdl,xmlNodePtr schema);

void delete_restriction_var_int(void *rvi);
void delete_schema_restriction_var_char(void *srvc);

#endif
