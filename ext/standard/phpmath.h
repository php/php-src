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
   | Authors: Jim Winstead (jimw@php.net)                                 |
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _PHPMATH_H
#define _PHPMATH_H
PHP_FUNCTION(sin);
PHP_FUNCTION(cos);
PHP_FUNCTION(tan);
PHP_FUNCTION(asin);
PHP_FUNCTION(acos);
PHP_FUNCTION(atan);
PHP_FUNCTION(atan2);
PHP_FUNCTION(pi);
PHP_FUNCTION(exp);
PHP_FUNCTION(log);
PHP_FUNCTION(log10);
PHP_FUNCTION(pow);
PHP_FUNCTION(sqrt);
PHP_FUNCTION(srand);
PHP_FUNCTION(rand);
PHP_FUNCTION(getrandmax);
PHP_FUNCTION(mt_srand);
PHP_FUNCTION(mt_rand);
PHP_FUNCTION(mt_getrandmax);
PHP_FUNCTION(abs);
PHP_FUNCTION(ceil);
PHP_FUNCTION(floor);
PHP_FUNCTION(round);
PHP_FUNCTION(decbin);
PHP_FUNCTION(dechex);
PHP_FUNCTION(decoct);
PHP_FUNCTION(bindec);
PHP_FUNCTION(hexdec);
PHP_FUNCTION(octdec);
PHP_FUNCTION(base_convert);
PHP_FUNCTION(number_format);
PHP_FUNCTION(deg2rad);
PHP_FUNCTION(rad2deg);

#endif /* _PHPMATH_H */
