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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_IMAGE_H
#define PHP_IMAGE_H

PHP_MINIT_FUNCTION(image);
PHP_MSHUTDOWN_FUNCTION(image);

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
  IMAGE_FILETYPE_WEBP,
  IMAGE_FILETYPE_AVIF,
  IMAGE_FILETYPE_HEIF,
/* WHEN EXTENDING: PLEASE ALSO REGISTER IN basic_function.stub.php */
  IMAGE_FILETYPE_FIXED_COUNT
} image_filetype;
/* }}} */

PHPAPI int php_getimagetype(php_stream *stream, const char *input, char *filetype);

PHPAPI const char * php_image_type_to_mime_type(int image_type);

PHPAPI bool php_is_image_avif(php_stream *stream);

/* return info as a struct, to make expansion easier */
struct php_gfxinfo {
	unsigned int width;
	unsigned int height;
	zend_string *width_unit;
	zend_string *height_unit;
	unsigned int bits;
	unsigned int channels;
};

typedef zend_result (*php_image_identify)(php_stream *stream);
typedef struct php_gfxinfo *(*php_image_get_info)(php_stream *stream);

struct php_image_handler {
	const char *mime_type;
	const char *extension;
	const char *const_name;
	php_image_identify identify;
	php_image_get_info get_info;
};

#define PHP_IMAGE_CONST_NAME(suffix) ("IMAGETYPE_" suffix)

/* This should only be called on module init */
PHPAPI int php_image_register_handler(const struct php_image_handler *handler);
/* This should only be called on module shutdown */
PHPAPI zend_result php_image_unregister_handler(int image_type);

#endif /* PHP_IMAGE_H */
