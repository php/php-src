/*
	This file needs to be compiled in with php on windows so that the
	oracle dll will work (and can be compiled).  preferably, I would
	like to see if there is another way accomplish what needs to be
	done with the symbol_table in ora_set_param_values() in oracle.c.
	This is just a quick hack to get this out.

	Shane
*/

#include "php.h"

PHPAPI HashTable *php3i_get_symbol_table(void) {
	TLS_VARS;
	return &GLOBAL(symbol_table);
}

/* This is becoming a general callback file, rather than an oracle hack
 * file.  Seems we need the following now for xml. */

PHPAPI HashTable *php3i_get_function_table(void) {
	TLS_VARS;
	return &GLOBAL(function_table);
}
