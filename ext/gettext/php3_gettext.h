/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _GETTEXT_H
#define _GETTEXT_H

#if HAVE_LIBINTL
#ifndef INIT_FUNC_ARGS
#include "modules.h"
#endif

extern php3_module_entry php3_gettext_module_entry;
#define php3_gettext_module_ptr &php3_gettext_module_entry

extern void php3_info_gettext(void);
extern void php3_textdomain(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gettext(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dgettext(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dcgettext(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bindtextdomain(INTERNAL_FUNCTION_PARAMETERS);

#else
#define php3_gettext_module_ptr NULL
#endif /* HAVE_LIBINTL */

#endif /* _GETTEXT_H */
