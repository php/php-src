#ifndef PHP_SCHEMA_H
#define PHP_SCHEMA_H

int load_schema(sdlPtr sdl, xmlNodePtr schema);
int schema_pass2(sdlPtr sdl);

#endif
