/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

/* $Id */

#ifndef PHP_DAV_H
#define PHP_DAV_H

#if HAVE_MOD_DAV

typedef struct {
    int foo;
    char *mkcol_test_handler;
    char *mkcol_create_handler;
} phpdav_module;

extern zend_module_entry phpdav_module_entry;
#define phpdav_module_ptr &phpdav_module_entry

int phpdav_mkcol_test_handler(request_rec *);

PHP_FUNCTION(dav_set_mkcol_handlers);

#else /* !HAVE_MOD_DAV */

#define phpdav_module_ptr NULL

#endif /* HAVE_MOD_DAV */

#define phpext_dav_ptr phpdav_module_ptr

#endif /* PHP_DAV_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
