/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#if HAVE_EXIF

#include "php_version.h"
#define PHP_EXIF_VERSION "1.4 $Id: 8bdc0c8f27c2c9dd1f7551f1f9fe3ab57a06a4b1 $"

extern zend_module_entry exif_module_entry;
#define phpext_exif_ptr &exif_module_entry

PHP_FUNCTION(exif_read_data);
PHP_FUNCTION(exif_tagname);
PHP_FUNCTION(exif_thumbnail);
PHP_FUNCTION(exif_imagetype);
#endif
