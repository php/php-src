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

#ifndef PHP_IMAGE_H
#define PHP_IMAGE_H

PHP_FUNCTION(getimagesize);
PHP_FUNCTION(getimagesizefromstring);

PHP_FUNCTION(image_type_to_mime_type);
PHP_FUNCTION(image_type_to_extension);

/* {{{ enum image_filetype
   This enum is used to have ext/standard/image.c and ext/exif/exif.c use
   the same constants for file types.
*/
typedef enum
{ IMAGE_FILETYPE_UNKNOWN=0,
  IMAGE_FILETYPE_GIF=1,
  IMAGE_FILETYPE_JPEG,
  IMAGE_FILETYPE_PNG,
  IMAGE_FILETYPE_SWF,
  IMAGE_FILETYPE_PSD,
  IMAGE_FILETYPE_BMP,
  IMAGE_FILETYPE_TIFF_II, /* intel */
  IMAGE_FILETYPE_TIFF_MM, /* motorola */
  IMAGE_FILETYPE_JPC,
  IMAGE_FILETYPE_JP2,
  IMAGE_FILETYPE_JPX,
  IMAGE_FILETYPE_JB2,
  IMAGE_FILETYPE_SWC,
  IMAGE_FILETYPE_IFF,
  IMAGE_FILETYPE_WBMP,
  /* IMAGE_FILETYPE_JPEG2000 is a userland alias for IMAGE_FILETYPE_JPC */
  IMAGE_FILETYPE_XBM,
  IMAGE_FILETYPE_ICO,
/* WHEN EXTENDING: PLEASE ALSO REGISTER IN image.c:PHP_MINIT_FUNCTION(imagetypes) */
  IMAGE_FILETYPE_COUNT
} image_filetype;
/* }}} */

PHP_MINIT_FUNCTION(imagetypes);

PHPAPI int php_getimagetype(php_stream *stream, char *filetype);

PHPAPI char * php_image_type_to_mime_type(int image_type);

#endif /* PHP_IMAGE_H */
