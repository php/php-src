/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

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
