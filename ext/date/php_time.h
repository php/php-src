/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   |          Tim Düsterhus <timwolla@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_DATE_TIME_H
# define PHP_DATE_TIME_H

# include "php.h"

PHP_MINIT_FUNCTION(date_time);

PHPAPI extern zend_class_entry *php_date_ce_time_timeexception;

# include "time_duration.h"

#endif /* PHP_DATE_TIME_H */
