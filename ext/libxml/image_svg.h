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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifndef LIBXML_IMAGE_SVG
#define LIBXML_IMAGE_SVG

#include "zend.h"

void php_libxml_register_image_svg_handler(void);
zend_result php_libxml_unregister_image_svg_handler(void);

#endif
