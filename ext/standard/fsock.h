/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Wez Furlong                                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php 3.0 revision 1.24 1999-06-18 [ssb] */

#ifndef FSOCK_H
#define FSOCK_H

#ifdef NETWARE
#ifdef NEW_LIBC
#include "sys/timeval.h"
#else
#include "netware/time_nw.h"    /* For 'timeval' */
#endif
#endif

#include "file.h"

#include "php_network.h"

PHP_FUNCTION(fsockopen);
PHP_FUNCTION(pfsockopen);

PHPAPI int php_lookup_hostname(const char *addr, struct in_addr *in);

PHP_RSHUTDOWN_FUNCTION(fsock);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: sw=4 ts=4
 */
#endif /* FSOCK_H */
