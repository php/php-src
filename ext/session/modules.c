/*
 * To add a PHP session module, #include its header file and 
 * add a ps_xxx_ptr in the struct...
 */

#include "mod_files.h"
#include "mod_mm.h"
#include "mod_user.h"

static ps_module *ps_modules[] = {
	ps_files_ptr,
	ps_mm_ptr,
	ps_user_ptr,
};

