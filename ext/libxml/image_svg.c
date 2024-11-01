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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "image_svg.h"
#include "php_libxml.h"

#include "ext/standard/php_image.h"

#include <libxml/xmlreader.h>

#ifdef HAVE_LIBXML

static int svg_image_type_id;

static int php_libxml_svg_stream_read(void *context, char *buffer, int len)
{
	return php_stream_read(context, buffer, len);
}

/* Sanity check that the input only contains characters valid for a dimension (numbers with units, e.g. 5cm).
 * This also protects the user against injecting XSS.
 * Only accept [0-9a-zA-Z] */
static bool php_libxml_valid_dimension(const xmlChar *input)
{
	if (*input == '\0') {
		return false;
	}
	while (*input) {
		if (!((*input >= '0' && *input <= '9') || (*input >= 'a' && *input <= 'z') || (*input >= 'A' && *input <= 'Z'))) {
			return false;
		}
		input++;
	}
	return true;
}

zend_result php_libxml_svg_image_handle(php_stream *stream, struct php_gfxinfo **result)
{
	if (php_stream_rewind(stream)) {
		return FAILURE;
	}

	/* Early check before doing more expensive work */
	if (php_stream_getc(stream) != '<') {
		return FAILURE;
	}

	if (php_stream_rewind(stream)) {
		return FAILURE;
	}

	PHP_LIBXML_SANITIZE_GLOBALS(reader_for_stream);
	xmlTextReaderPtr reader = xmlReaderForIO(
		php_libxml_svg_stream_read,
		NULL,
		stream,
		NULL,
		NULL,
		XML_PARSE_NOWARNING | XML_PARSE_NOERROR | XML_PARSE_NONET
	);
	PHP_LIBXML_RESTORE_GLOBALS(reader_for_stream);

	if (!reader) {
		return FAILURE;
	}

	bool is_svg = false;
	while (xmlTextReaderRead(reader) == 1) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			/* Root must be an svg element */
			const xmlChar *name = xmlTextReaderConstLocalName(reader);
			if (!name || strcasecmp((const char *) name, "svg") != 0) {
				break;
			}

			xmlChar *width = xmlTextReaderGetAttribute(reader, BAD_CAST "width");
			xmlChar *height = xmlTextReaderGetAttribute(reader, BAD_CAST "height");
			if (!width || !height || !php_libxml_valid_dimension(width) || !php_libxml_valid_dimension(height)) {
				xmlFree(width);
				xmlFree(height);
				break;
			}

			is_svg = true;
			if (result) {
				*result = ecalloc(1, sizeof(**result));
				(*result)->width_str = zend_string_init((const char *) width, xmlStrlen(width), false);
				(*result)->height_str = zend_string_init((const char *) height, xmlStrlen(height), false);
			}

			xmlFree(width);
			xmlFree(height);
			break;
		}
	}

	xmlFreeTextReader(reader);

	return is_svg ? SUCCESS : FAILURE;
}

zend_result php_libxml_svg_image_identify(php_stream *stream)
{
	return php_libxml_svg_image_handle(stream, NULL);
}

struct php_gfxinfo *php_libxml_svg_image_get_info(php_stream *stream)
{
	struct php_gfxinfo *result = NULL;
	zend_result status = php_libxml_svg_image_handle(stream, &result);
	ZEND_ASSERT((status == SUCCESS) == (result != NULL));
	return result;
}

static const struct php_image_handler svg_image_handler = {
	"image/svg+xml",
	".svg",
	PHP_IMAGE_CONST_NAME("SVG"),
	php_libxml_svg_image_identify,
	php_libxml_svg_image_get_info,
};

void php_libxml_register_image_svg_handler(void)
{
	svg_image_type_id = php_image_register_handler(&svg_image_handler);
}

zend_result php_libxml_unregister_image_svg_handler(void)
{
	return php_image_unregister_handler(svg_image_type_id);
}

#endif
