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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#ifndef _PHP3_SNMP_H
#define _PHP3_SNMP_H

#if COMPILE_DL
#undef HAVE_SNMP
#define HAVE_SNMP 1
#endif
#if HAVE_SNMP
#ifndef DLEXPORT
#define DLEXPORT
#endif

extern php3_module_entry snmp_module_entry;
#define snmp_module_ptr &snmp_module_entry

extern int php3i_snmp_init(INIT_FUNC_ARGS);
extern PHP_FUNCTION(snmpget);
extern PHP_FUNCTION(snmpwalk);
extern PHP_FUNCTION(snmprealwalk);
extern void php3_info_snmp(void);
#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#endif  /* _PHP3_SNMP_H */
