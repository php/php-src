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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef TYPE_H
#define TYPE_H

extern int php_check_type(char *str);
extern int php_check_ident_type(char *str);
extern char *php_get_ident_index(char *str);

#define GPC_REGULAR 0x1
#define GPC_INDEXED_ARRAY 0x2
#define GPC_NON_INDEXED_ARRAY 0x4
#define GPC_ARRAY (GPC_INDEXED_ARRAY | GPC_NON_INDEXED_ARRAY)

#endif
