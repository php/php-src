/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moshe Doron <mosdoron@netvision.net.il>                      |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SUNFUNCS_H
#define PHP_SUNFUNCS_H

/* default ini entries: */
/* Jerusalem one. */
#define DATE_DEFAULT_LATITUDE "31.7667"
#define DATE_DEFAULT_LONGITUDE "35.2333"

/* on 90'50; common jewish sunset declaration (start of sun body appear) */
#define DATE_SUNSET_ZENITH "90.83"

/* on 90'50; common jewish  sunrise declaration (sun body disappeared) */
#define DATE_SUNRISE_ZENITH "90.83"

#define SUNFUNCS_RET_TIMESTAMP 0
#define SUNFUNCS_RET_STRING 1
#define SUNFUNCS_RET_DOUBLE 2

PHP_FUNCTION(date_sunrise);
PHP_FUNCTION(date_sunset);

#endif /* PHP_SUNFUNCS_H */
