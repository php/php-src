/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                             |
   +----------------------------------------------------------------------+
 */

#ifndef LIBXML_IMAGE_SVG
#define LIBXML_IMAGE_SVG

#include "zend.h"

void php_libxml_register_image_svg_handler(void);
zend_result php_libxml_unregister_image_svg_handler(void);

#endif
