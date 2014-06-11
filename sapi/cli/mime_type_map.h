/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
*/

/* This is a generated file. Rather than modifying it, please run
 * "php generate_mime_type_map.php > mime_type_map.h" to regenerate the file. */

#ifndef PHP_CLI_SERVER_MIME_TYPE_MAP_H
#define PHP_CLI_SERVER_MIME_TYPE_MAP_H

typedef struct php_cli_server_ext_mime_type_pair {
	const char *ext;
	const char *mime_type;
} php_cli_server_ext_mime_type_pair;

static php_cli_server_ext_mime_type_pair mime_type_map[] = {
	{ "html", "text/html" },
	{ "htm", "text/html" },
	{ "js", "text/javascript" },
	{ "css", "text/css" },
	{ "gif", "image/gif" },
	{ "jpg", "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "jpe", "image/jpeg" },
	{ "pdf", "application/pdf" },
	{ "png", "image/png" },
	{ "svg", "image/svg+xml" },
	{ "txt", "text/plain" },
	{ "webm", "video/webm" },
	{ "ogv", "video/ogg" },
	{ "ogg", "audio/ogg" },
	{ "3gp", "video/3gpp" },    /* This is standard video format used for MMS in phones */
	{ "apk", "application/vnd.android.package-archive" },
	{ "avi", "video/x-msvideo" },
	{ "bmp", "image/x-ms-bmp" },
	{ "csv", "text/comma-separated-values" },
	{ "doc", "application/msword" },
	{ "docx", "application/msword" },
	{ "flac", "audio/flac" },
	{ "gz",  "application/x-gzip" },
	{ "gzip", "application/x-gzip" },
	{ "ics", "text/calendar" },
	{ "kml", "application/vnd.google-earth.kml+xml" },
	{ "kmz", "application/vnd.google-earth.kmz" },
	{ "m4a", "audio/mp4" },
	{ "mp3", "audio/mpeg" },
	{ "mp4", "video/mp4" },
	{ "mpg", "video/mpeg" },
	{ "mpeg", "video/mpeg" },
	{ "mov", "video/quicktime" },
	{ "odp", "application/vnd.oasis.opendocument.presentation" },
	{ "ods", "application/vnd.oasis.opendocument.spreadsheet" },
	{ "odt", "application/vnd.oasis.opendocument.text" },
	{ "oga", "audio/ogg" },
	{ "pdf", "application/pdf" },
	{ "pptx", "application/vnd.ms-powerpoint" },
	{ "pps", "application/vnd.ms-powerpoint" },
	{ "qt", "video/quicktime" },
	{ "swf", "application/x-shockwave-flash" },
	{ "tar", "application/x-tar" },
	{ "text", "text/plain" },
	{ "tif", "image/tiff" },
	{ "wav", "audio/wav" },
	{ "wmv", "video/x-ms-wmv" },
	{ "xls", "application/vnd.ms-excel" },
	{ "xlsx", "application/vnd.ms-excel" },
	{ "zip", "application/x-zip-compressed" },
	{ "xml", "application/xml" },
	{ "xsl", "application/xml" },
	{ "xsd", "application/xml" },
	{ NULL, NULL }
};

#endif /* PHP_CLI_SERVER_MIME_TYPE_MAP_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
