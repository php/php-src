/*
 * To add a PHP session module, #include its header file and 
 * add a ps_xxx_ptr in the struct...
 */

#include "mod_files.h"

static ps_module *ps_modules[] = {
	ps_files_ptr,
	0,
};

