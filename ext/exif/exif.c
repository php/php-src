/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*  ToDos
 *
 * 	See if example images from http://www.exif.org have illegal
 * 		thumbnail sizes or if code is corrupt.
 * 	Create/Update exif headers.
 * 	Create/Remove/Update image thumbnails.
 */

/*  Security
 *
 *  At current time i do not see any security problems but a potential
 *  attacker could generate an image with recursive ifd pointers...(Marcus)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/file.h"

#if HAVE_EXIF

/* When EXIF_DEBUG is defined the module generates a lot of debug messages
 * that help understanding what is going on. This can and should be used
 * while extending the module as it shows if you are at the right position.
 * You are always considered to have a copy of TIFF6.0 and EXIF2.10 standard.
 */
#undef EXIF_DEBUG

#undef EXIF_JPEG2000

#include "php_exif.h"
#include <math.h>
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_image.h"
#include "ext/standard/info.h" 

#if HAVE_MBSTRING && !defined(COMPILE_DL_MBSTRING) 
#define EXIF_USE_MBSTRING 1
#else
#define EXIF_USE_MBSTRING 0
#endif

#if EXIF_USE_MBSTRING
#include "ext/mbstring/mbstring.h"
#endif

typedef unsigned char uchar;

#ifndef TRUE
#	define TRUE 1
#	define FALSE 0
#endif

#ifndef max
#	define max(a,b) ((a)>(b) ? (a) : (b))
#endif

#define EFREE_IF(ptr)	if (ptr) efree(ptr)

static unsigned char exif_thumbnail_force_ref[] = {2, BYREF_NONE, BYREF_FORCE_REST};

/* {{{ exif_functions[]
 */
function_entry exif_functions[] = {
	PHP_FE(exif_read_data, NULL)
	PHP_FALIAS(read_exif_data, exif_read_data, NULL)
	PHP_FE(exif_tagname, NULL)
	PHP_FE(exif_thumbnail, exif_thumbnail_force_ref)
	PHP_FE(exif_imagetype, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

#define EXIF_VERSION "1.4 $Id$"

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(exif)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "EXIF Support", "enabled");
	php_info_print_table_row(2, "EXIF Version", EXIF_VERSION);
	php_info_print_table_row(2, "Supported EXIF Version", "0220");
	php_info_print_table_row(2, "Supported filetypes", "JPEG,TIFF");
	php_info_print_table_end();
}
/* }}} */

ZEND_BEGIN_MODULE_GLOBALS(exif)
	char * encode_unicode;
	char * decode_unicode_be;
	char * decode_unicode_le;
	char * encode_jis;
	char * decode_jis_be;
	char * decode_jis_le;
ZEND_END_MODULE_GLOBALS(exif) 

ZEND_DECLARE_MODULE_GLOBALS(exif)

#ifdef ZTS
#define EXIF_G(v) TSRMG(exif_globals_id, zend_exif_globals *, v)
#else
#define EXIF_G(v) (exif_globals.v)
#endif
 
/* {{{ PHP_INI
 */

ZEND_API ZEND_INI_MH(OnUpdateEncode)
{
#if EXIF_USE_MBSTRING
	if (new_value && strlen(new_value) && !php_mb_check_encoding_list(new_value TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal encoding ignored: '%s'", new_value);
		return FAILURE;
	}
#endif
	return OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
}

ZEND_API ZEND_INI_MH(OnUpdateDecode)
{
#if EXIF_USE_MBSTRING
	if (!php_mb_check_encoding_list(new_value TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal encoding ignored: '%s'", new_value);
		return FAILURE;
	}
#endif
	return OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
}

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("exif.encode_unicode",          "ISO-8859-15", PHP_INI_ALL, OnUpdateEncode, encode_unicode,    zend_exif_globals, exif_globals)
    STD_PHP_INI_ENTRY("exif.decode_unicode_motorola", "UCS-2BE",     PHP_INI_ALL, OnUpdateDecode, decode_unicode_be, zend_exif_globals, exif_globals)
    STD_PHP_INI_ENTRY("exif.decode_unicode_intel",    "UCS-2LE",     PHP_INI_ALL, OnUpdateDecode, decode_unicode_le, zend_exif_globals, exif_globals)
    STD_PHP_INI_ENTRY("exif.encode_jis",              "",            PHP_INI_ALL, OnUpdateEncode, encode_jis,        zend_exif_globals, exif_globals)
    STD_PHP_INI_ENTRY("exif.decode_jis_motorola",     "JIS",         PHP_INI_ALL, OnUpdateDecode, decode_jis_be,     zend_exif_globals, exif_globals)
    STD_PHP_INI_ENTRY("exif.decode_jis_intel",        "JIS",         PHP_INI_ALL, OnUpdateDecode, decode_jis_le,     zend_exif_globals, exif_globals)
PHP_INI_END()
/* }}} */
 
/* {{{ php_extname_init_globals
 */
static void php_exif_init_globals(zend_exif_globals *exif_globals)
{
	exif_globals->encode_unicode    = NULL;
	exif_globals->decode_unicode_be = NULL;
	exif_globals->decode_unicode_le = NULL;
	exif_globals->encode_jis        = NULL;
	exif_globals->decode_jis_be     = NULL;
	exif_globals->decode_jis_le     = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(exif)
   Get the size of an image as 4-element array */
PHP_MINIT_FUNCTION(exif)
{
	ZEND_INIT_MODULE_GLOBALS(exif, php_exif_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	REGISTER_LONG_CONSTANT("EXIF_USE_MBSTRING", EXIF_USE_MBSTRING, CONST_CS | CONST_PERSISTENT); 
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(exif)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ exif_module_entry
 */
zend_module_entry exif_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"exif",
	exif_functions,
	PHP_MINIT(exif), 
	PHP_MSHUTDOWN(exif),
	NULL, NULL,
	PHP_MINFO(exif),
#if ZEND_MODULE_API_NO >= 20010901
	EXIF_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EXIF
ZEND_GET_MODULE(exif)
#endif

/* {{{ php_strnlen
 * get length of string if buffer if less than buffer size or buffer size */
static size_t php_strnlen(char* str, size_t maxlen) {
	size_t len = 0;

	if (str && maxlen && *str) {
		do {
			len++;
		} while (--maxlen && *(++str));
	}
	return len;
}
/* }}} */

/* {{{ error messages
*/
static const char * EXIF_ERROR_EALLOC    = "Cannot allocate memory for all data";
static const char * EXIF_ERROR_FILEEOF   = "Unexpected end of file reached";
static const char * EXIF_ERROR_CORRUPT   = "File structure corrupted";
static const char * EXIF_ERROR_THUMBEOF  = "Thumbnail goes IFD boundary or end of file reached";
static const char * EXIF_ERROR_FSREALLOC = "Illegal reallocating of undefined file section";

#define EXIF_ERRLOG_EALLOC     php_error_docref(NULL TSRMLS_CC, E_ERROR,   EXIF_ERROR_EALLOC);
#define EXIF_ERRLOG_FILEEOF    php_error_docref(NULL TSRMLS_CC, E_WARNING, EXIF_ERROR_FILEEOF);
#define EXIF_ERRLOG_CORRUPT    php_error_docref(NULL TSRMLS_CC, E_WARNING, EXIF_ERROR_CORRUPT);
#define EXIF_ERRLOG_THUMBEOF   php_error_docref(NULL TSRMLS_CC, E_WARNING, EXIF_ERROR_THUMBEOF);
#define EXIF_ERRLOG_FSREALLOC  php_error_docref(NULL TSRMLS_CC, E_WARNING, EXIF_ERROR_FSREALLOC);
/* }}} */

/* {{{ format description defines
   Describes format descriptor
*/
static int php_tiff_bytes_per_format[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8, 1};
#define NUM_FORMATS 13

#define TAG_FMT_BYTE       1
#define TAG_FMT_STRING     2
#define TAG_FMT_USHORT     3
#define TAG_FMT_ULONG      4
#define TAG_FMT_URATIONAL  5
#define TAG_FMT_SBYTE      6
#define TAG_FMT_UNDEFINED  7
#define TAG_FMT_SSHORT     8
#define TAG_FMT_SLONG      9
#define TAG_FMT_SRATIONAL 10
#define TAG_FMT_SINGLE    11
#define TAG_FMT_DOUBLE    12
#define TAG_FMT_IFD       13

#ifdef EXIF_DEBUG
static char *exif_get_tagformat(int format)
{
	switch(format) {
		case TAG_FMT_BYTE:      return "BYTE";
		case TAG_FMT_STRING:    return "STRING";
		case TAG_FMT_USHORT:    return "USHORT";
		case TAG_FMT_ULONG:     return "ULONG";
		case TAG_FMT_URATIONAL: return "URATIONAL";
		case TAG_FMT_SBYTE:     return "SBYTE";
		case TAG_FMT_UNDEFINED: return "UNDEFINED";
		case TAG_FMT_SSHORT:    return "SSHORT";
		case TAG_FMT_SLONG:     return "SLONG";
		case TAG_FMT_SRATIONAL: return "SRATIONAL";
		case TAG_FMT_SINGLE:    return "SINGLE";
		case TAG_FMT_DOUBLE:    return "DOUBLE";
		case TAG_FMT_IFD:       return "IFD";
	}
	return "*Illegal";
}
#endif

/* Describes tag values */
#define TAG_GPS_VERSION_ID              0x0000
#define TAG_GPS_LATITUDE_REF            0x0001
#define TAG_GPS_LATITUDE                0x0002
#define TAG_GPS_LONGITUDE_REF           0x0003
#define TAG_GPS_LONGITUDE               0x0004
#define TAG_GPS_ALTITUDE_REF            0x0005
#define TAG_GPS_ALTITUDE                0x0006
#define TAG_GPS_TIME_STAMP              0x0007
#define TAG_GPS_SATELLITES              0x0008
#define TAG_GPS_STATUS                  0x0009
#define TAG_GPS_MEASURE_MODE            0x000A
#define TAG_GPS_DOP                     0x000B
#define TAG_GPS_SPEED_REF               0x000C
#define TAG_GPS_SPEED                   0x000D
#define TAG_GPS_TRACK_REF               0x000E
#define TAG_GPS_TRACK                   0x000F
#define TAG_GPS_IMG_DIRECTION_REF       0x0010
#define TAG_GPS_IMG_DIRECTION           0x0011
#define TAG_GPS_MAP_DATUM               0x0012
#define TAG_GPS_DEST_LATITUDE_REF       0x0013
#define TAG_GPS_DEST_LATITUDE           0x0014
#define TAG_GPS_DEST_LONGITUDE_REF      0x0015
#define TAG_GPS_DEST_LONGITUDE          0x0016
#define TAG_GPS_DEST_BEARING_REF        0x0017
#define TAG_GPS_DEST_BEARING            0x0018
#define TAG_GPS_DEST_DISTANCE_REF       0x0019
#define TAG_GPS_DEST_DISTANCE           0x001A
#define TAG_GPS_PROCESSING_METHOD       0x001B
#define TAG_GPS_AREA_INFORMATION        0x001C
#define TAG_GPS_DATE_STAMP              0x001D
#define TAG_GPS_DIFFERENTIAL            0x001E
#define TAG_TIFF_COMMENT                0x00FE /* SHOUDLNT HAPPEN */
#define TAG_NEW_SUBFILE                 0x00FE /* New version of subfile tag */
#define TAG_SUBFILE_TYPE                0x00FF /* Old version of subfile tag */
#define TAG_IMAGEWIDTH                  0x0100
#define TAG_IMAGEHEIGHT                 0x0101
#define TAG_BITS_PER_SAMPLE             0x0102
#define TAG_COMPRESSION                 0x0103
#define TAG_PHOTOMETRIC_INTERPRETATION  0x0106
#define TAG_TRESHHOLDING                0x0107
#define TAG_CELL_WIDTH                  0x0108
#define TAG_CELL_HEIGHT                 0x0109
#define TAG_FILL_ORDER                  0x010A
#define TAG_DOCUMENT_NAME               0x010D
#define TAG_IMAGE_DESCRIPTION           0x010E
#define TAG_MAKE                        0x010F
#define TAG_MODEL                       0x0110
#define TAG_STRIP_OFFSETS               0x0111
#define TAG_ORIENTATION                 0x0112
#define TAG_SAMPLES_PER_PIXEL           0x0115
#define TAG_ROWS_PER_STRIP              0x0116
#define TAG_STRIP_BYTE_COUNTS           0x0117
#define TAG_MIN_SAMPPLE_VALUE           0x0118
#define TAG_MAX_SAMPLE_VALUE            0x0119
#define TAG_X_RESOLUTION                0x011A
#define TAG_Y_RESOLUTION                0x011B
#define TAG_PLANAR_CONFIGURATION        0x011C
#define TAG_PAGE_NAME                   0x011D
#define TAG_X_POSITION                  0x011E
#define TAG_Y_POSITION                  0x011F
#define TAG_FREE_OFFSETS                0x0120
#define TAG_FREE_BYTE_COUNTS            0x0121
#define TAG_GRAY_RESPONSE_UNIT          0x0122
#define TAG_GRAY_RESPONSE_CURVE         0x0123
#define TAG_RESOLUTION_UNIT             0x0128
#define TAG_PAGE_NUMBER                 0x0129
#define TAG_TRANSFER_FUNCTION           0x012D
#define TAG_SOFTWARE                    0x0131
#define TAG_DATETIME                    0x0132
#define TAG_ARTIST                      0x013B
#define TAG_HOST_COMPUTER               0x013C
#define TAG_PREDICTOR                   0x013D
#define TAG_WHITE_POINT                 0x013E
#define TAG_PRIMARY_CHROMATICITIES      0x013F
#define TAG_COLOR_MAP                   0x0140
#define TAG_HALFTONE_HINTS              0x0141
#define TAG_TILE_WIDTH                  0x0142
#define TAG_TILE_LENGTH                 0x0143
#define TAG_TILE_OFFSETS                0x0144
#define TAG_TILE_BYTE_COUNTS            0x0145
#define TAG_SUB_IFD                     0x014A
#define TAG_INK_SETMPUTER               0x014C
#define TAG_INK_NAMES                   0x014D
#define TAG_NUMBER_OF_INKS              0x014E
#define TAG_DOT_RANGE                   0x0150
#define TAG_TARGET_PRINTER              0x0151
#define TAG_EXTRA_SAMPLE                0x0152
#define TAG_SAMPLE_FORMAT               0x0153
#define TAG_S_MIN_SAMPLE_VALUE          0x0154
#define TAG_S_MAX_SAMPLE_VALUE          0x0155
#define TAG_TRANSFER_RANGE              0x0156
#define TAG_JPEG_TABLES                 0x015B
#define TAG_JPEG_PROC                   0x0200
#define TAG_JPEG_INTERCHANGE_FORMAT     0x0201
#define TAG_JPEG_INTERCHANGE_FORMAT_LEN 0x0202
#define TAG_JPEG_RESTART_INTERVAL       0x0203
#define TAG_JPEG_LOSSLESS_PREDICTOR     0x0205
#define TAG_JPEG_POINT_TRANSFORMS       0x0206
#define TAG_JPEG_Q_TABLES               0x0207
#define TAG_JPEG_DC_TABLES              0x0208
#define TAG_JPEG_AC_TABLES              0x0209
#define TAG_YCC_COEFFICIENTS            0x0211
#define TAG_YCC_SUB_SAMPLING            0x0212
#define TAG_YCC_POSITIONING             0x0213
#define TAG_REFERENCE_BLACK_WHITE       0x0214
/* 0x0301 - 0x0302 */
/* 0x0320 */
/* 0x0343 */
/* 0x5001 - 0x501B */
/* 0x5021 - 0x503B */
/* 0x5090 - 0x5091 */
/* 0x5100 - 0x5101 */
/* 0x5110 - 0x5113 */
/* 0x80E3 - 0x80E6 */
/* 0x828d - 0x828F */
#define TAG_COPYRIGHT                   0x8298
#define TAG_EXPOSURETIME                0x829A
#define TAG_FNUMBER                     0x829D
#define TAG_EXIF_IFD_POINTER            0x8769
#define TAG_ICC_PROFILE                 0x8773
#define TAG_EXPOSURE_PROGRAM            0x8822
#define TAG_SPECTRAL_SENSITY            0x8824
#define TAG_GPS_IFD_POINTER             0x8825
#define TAG_ISOSPEED                    0x8827
#define TAG_OPTOELECTRIC_CONVERSION_F   0x8828
/* 0x8829 - 0x882b */
#define TAG_EXIFVERSION                 0x9000
#define TAG_DATE_TIME_ORIGINAL          0x9003
#define TAG_DATE_TIME_DIGITIZED         0x9004
#define TAG_COMPONENT_CONFIG            0x9101
#define TAG_COMPRESSED_BITS_PER_PIXEL   0x9102
#define TAG_SHUTTERSPEED                0x9201
#define TAG_APERTURE                    0x9202
#define TAG_BRIGHTNESS_VALUE            0x9203
#define TAG_EXPOSURE_BIAS_VALUE         0x9204
#define TAG_MAX_APERTURE                0x9205
#define TAG_SUBJECT_DISTANCE            0x9206
#define TAG_METRIC_MODULE               0x9207
#define TAG_LIGHT_SOURCE                0x9208
#define TAG_FLASH                       0x9209
#define TAG_FOCAL_LENGTH                0x920A
/* 0x920B - 0x920D */
/* 0x9211 - 0x9216 */
#define TAG_SUBJECT_AREA                0x9214
#define TAG_MAKER_NOTE                  0x927C
#define TAG_USERCOMMENT                 0x9286
#define TAG_SUB_SEC_TIME                0x9290
#define TAG_SUB_SEC_TIME_ORIGINAL       0x9291
#define TAG_SUB_SEC_TIME_DIGITIZED      0x9292
/* 0x923F */
/* 0x935C */
#define TAG_XP_TITLE                    0x9C9B
#define TAG_XP_COMMENTS                 0x9C9C
#define TAG_XP_AUTHOR                   0x9C9D
#define TAG_XP_KEYWORDS                 0x9C9E
#define TAG_XP_SUBJECT                  0x9C9F
#define TAG_FLASH_PIX_VERSION           0xA000
#define TAG_COLOR_SPACE                 0xA001
#define TAG_COMP_IMAGE_WIDTH            0xA002 /* compressed images only */
#define TAG_COMP_IMAGE_HEIGHT           0xA003
#define TAG_RELATED_SOUND_FILE          0xA004
#define TAG_INTEROP_IFD_POINTER         0xA005 /* IFD pointer */
#define TAG_FLASH_ENERGY                0xA20B
#define TAG_SPATIAL_FREQUENCY_RESPONSE  0xA20C
#define TAG_FOCALPLANE_X_RES            0xA20E
#define TAG_FOCALPLANE_Y_RES            0xA20F
#define TAG_FOCALPLANE_RESOLUTION_UNIT  0xA210
#define TAG_SUBJECT_LOCATION            0xA214
#define TAG_EXPOSURE_INDEX              0xA215
#define TAG_SENSING_METHOD              0xA217
#define TAG_FILE_SOURCE                 0xA300
#define TAG_SCENE_TYPE                  0xA301
#define TAG_CFA_PATTERN                 0xA302
#define TAG_CUSTOM_RENDERED             0xA401
#define TAG_EXPOSURE_MODE               0xA402
#define TAG_WHITE_BALANCE               0xA403
#define TAG_DIGITAL_ZOOM_RATIO          0xA404
#define TAG_FOCAL_LENGTH_IN_35_MM_FILM  0xA405
#define TAG_SCENE_CAPTURE_TYPE          0xA406
#define TAG_GAIN_CONTROL                0xA407
#define TAG_CONTRAST                    0xA408
#define TAG_SATURATION                  0xA409
#define TAG_SHARPNESS                   0xA40A
#define TAG_DEVICE_SETTING_DESCRIPTION  0xA40B
#define TAG_SUBJECT_DISTANCE_RANGE      0xA40C
#define TAG_IMAGE_UNIQUE_ID             0xA420

/* Olympus specific tags */
#define TAG_OLYMPUS_SPECIALMODE         0x0200
#define TAG_OLYMPUS_JPEGQUAL            0x0201
#define TAG_OLYMPUS_MACRO               0x0202
#define TAG_OLYMPUS_DIGIZOOM            0x0204
#define TAG_OLYMPUS_SOFTWARERELEASE     0x0207
#define TAG_OLYMPUS_PICTINFO            0x0208
#define TAG_OLYMPUS_CAMERAID            0x0209
/* end Olympus specific tags */

/* Internal */
#define TAG_NONE               			-1 /* note that -1 <> 0xFFFF */
#define TAG_COMPUTED_VALUE     			-2
#define TAG_END_OF_LIST                 0xFFFD

/* Values for TAG_PHOTOMETRIC_INTERPRETATION */
#define PMI_BLACK_IS_ZERO       0
#define PMI_WHITE_IS_ZERO       1
#define PMI_RGB          	    2
#define PMI_PALETTE_COLOR       3
#define PMI_TRANSPARENCY_MASK   4
#define PMI_SEPARATED           5
#define PMI_YCBCR               6
#define PMI_CIELAB              8

/* }}} */

/* {{{ TabTable[]
 */
typedef const struct {
	unsigned short Tag;
	char *Desc;
} tag_info_type;

typedef tag_info_type  tag_info_array[];
typedef tag_info_type  *tag_table_type;

#define TAG_TABLE_END \
  {TAG_NONE,           "No tag value"},\
  {TAG_COMPUTED_VALUE, "Computed value"},\
  {TAG_END_OF_LIST,    ""}  /* Important for exif_get_tagname() IF value != "" functionresult is != false */

static tag_info_array tag_table_IFD = {
  { 0x000B, "ACDComment"},
  { 0x00FE, "NewSubFile"}, /* better name it 'ImageType' ? */
  { 0x00FF, "SubFile"},
  { 0x0100, "ImageWidth"},
  { 0x0101, "ImageLength"},
  { 0x0102, "BitsPerSample"},
  { 0x0103, "Compression"},
  { 0x0106, "PhotometricInterpretation"},
  { 0x010A, "FillOrder"},
  { 0x010D, "DocumentName"},
  { 0x010E, "ImageDescription"},
  { 0x010F, "Make"},
  { 0x0110, "Model"},
  { 0x0111, "StripOffsets"},
  { 0x0112, "Orientation"},
  { 0x0115, "SamplesPerPixel"},
  { 0x0116, "RowsPerStrip"},
  { 0x0117, "StripByteCounts"},
  { 0x0118, "MinSampleValue"},
  { 0x0119, "MaxSampleValue"},
  { 0x011A, "XResolution"},
  { 0x011B, "YResolution"},
  { 0x011C, "PlanarConfiguration"},
  { 0x011D, "PageName"},
  { 0x011E, "XPosition"},
  { 0x011F, "YPosition"},
  { 0x0120, "FreeOffsets"},
  { 0x0121, "FreeByteCounts"},
  { 0x0122, "GrayResponseUnit"},
  { 0x0123, "GrayResponseCurve"},
  { 0x0124, "T4Options"},
  { 0x0125, "T6Options"},
  { 0x0128, "ResolutionUnit"},
  { 0x0129, "PageNumber"},
  { 0x012D, "TransferFunction"},
  { 0x0131, "Software"},
  { 0x0132, "DateTime"},
  { 0x013B, "Artist"},
  { 0x013C, "HostComputer"},
  { 0x013D, "Predictor"},
  { 0x013E, "WhitePoint"},
  { 0x013F, "PrimaryChromaticities"},
  { 0x0140, "ColorMap"},
  { 0x0141, "HalfToneHints"},
  { 0x0142, "TileWidth"},
  { 0x0143, "TileLength"},
  { 0x0144, "TileOffsets"},
  { 0x0145, "TileByteCounts"},
  { 0x014A, "SubIFD"},
  { 0x014C, "InkSet"},
  { 0x014D, "InkNames"},
  { 0x014E, "NumberOfInks"},
  { 0x0150, "DotRange"},
  { 0x0151, "TargetPrinter"},
  { 0x0152, "ExtraSample"},
  { 0x0153, "SampleFormat"},
  { 0x0154, "SMinSampleValue"},
  { 0x0155, "SMaxSampleValue"},
  { 0x0156, "TransferRange"},
  { 0x0157, "ClipPath"},
  { 0x0158, "XClipPathUnits"},
  { 0x0159, "YClipPathUnits"},
  { 0x015A, "Indexed"},
  { 0x015B, "JPEGTables"},
  { 0x015F, "OPIProxy"},
  { 0x0200, "JPEGProc"},
  { 0x0201, "JPEGInterchangeFormat"},
  { 0x0202, "JPEGInterchangeFormatLength"},
  { 0x0203, "JPEGRestartInterval"},
  { 0x0205, "JPEGLosslessPredictors"},
  { 0x0206, "JPEGPointTransforms"},
  { 0x0207, "JPEGQTables"},
  { 0x0208, "JPEGDCTables"},
  { 0x0209, "JPEGACTables"},
  { 0x0211, "YCbCrCoefficients"},
  { 0x0212, "YCbCrSubSampling"},
  { 0x0213, "YCbCrPositioning"},
  { 0x0214, "ReferenceBlackWhite"},
  { 0x02BC, "ExtensibleMetadataPlatform"}, /* XAP: Extensible Authoring Publishing, obsoleted by XMP: Extensible Metadata Platform */
  { 0x0301, "Gamma"}, 
  { 0x0302, "ICCProfileDescriptor"}, 
  { 0x0303, "SRGBRenderingIntent"}, 
  { 0x0320, "ImageTitle"}, 
  { 0x5001, "ResolutionXUnit"}, 
  { 0x5002, "ResolutionYUnit"}, 
  { 0x5003, "ResolutionXLengthUnit"}, 
  { 0x5004, "ResolutionYLengthUnit"}, 
  { 0x5005, "PrintFlags"}, 
  { 0x5006, "PrintFlagsVersion"}, 
  { 0x5007, "PrintFlagsCrop"}, 
  { 0x5008, "PrintFlagsBleedWidth"}, 
  { 0x5009, "PrintFlagsBleedWidthScale"}, 
  { 0x500A, "HalftoneLPI"}, 
  { 0x500B, "HalftoneLPIUnit"}, 
  { 0x500C, "HalftoneDegree"}, 
  { 0x500D, "HalftoneShape"}, 
  { 0x500E, "HalftoneMisc"}, 
  { 0x500F, "HalftoneScreen"}, 
  { 0x5010, "JPEGQuality"}, 
  { 0x5011, "GridSize"}, 
  { 0x5012, "ThumbnailFormat"}, 
  { 0x5013, "ThumbnailWidth"}, 
  { 0x5014, "ThumbnailHeight"}, 
  { 0x5015, "ThumbnailColorDepth"}, 
  { 0x5016, "ThumbnailPlanes"}, 
  { 0x5017, "ThumbnailRawBytes"}, 
  { 0x5018, "ThumbnailSize"}, 
  { 0x5019, "ThumbnailCompressedSize"}, 
  { 0x501A, "ColorTransferFunction"}, 
  { 0x501B, "ThumbnailData"}, 
  { 0x5020, "ThumbnailImageWidth"}, 
  { 0x5021, "ThumbnailImageHeight"}, 
  { 0x5022, "ThumbnailBitsPerSample"}, 
  { 0x5023, "ThumbnailCompression"}, 
  { 0x5024, "ThumbnailPhotometricInterp"}, 
  { 0x5025, "ThumbnailImageDescription"}, 
  { 0x5026, "ThumbnailEquipMake"}, 
  { 0x5027, "ThumbnailEquipModel"}, 
  { 0x5028, "ThumbnailStripOffsets"}, 
  { 0x5029, "ThumbnailOrientation"}, 
  { 0x502A, "ThumbnailSamplesPerPixel"}, 
  { 0x502B, "ThumbnailRowsPerStrip"}, 
  { 0x502C, "ThumbnailStripBytesCount"}, 
  { 0x502D, "ThumbnailResolutionX"}, 
  { 0x502E, "ThumbnailResolutionY"}, 
  { 0x502F, "ThumbnailPlanarConfig"}, 
  { 0x5030, "ThumbnailResolutionUnit"}, 
  { 0x5031, "ThumbnailTransferFunction"}, 
  { 0x5032, "ThumbnailSoftwareUsed"}, 
  { 0x5033, "ThumbnailDateTime"}, 
  { 0x5034, "ThumbnailArtist"}, 
  { 0x5035, "ThumbnailWhitePoint"}, 
  { 0x5036, "ThumbnailPrimaryChromaticities"}, 
  { 0x5037, "ThumbnailYCbCrCoefficients"}, 
  { 0x5038, "ThumbnailYCbCrSubsampling"}, 
  { 0x5039, "ThumbnailYCbCrPositioning"}, 
  { 0x503A, "ThumbnailRefBlackWhite"}, 
  { 0x503B, "ThumbnailCopyRight"}, 
  { 0x5090, "LuminanceTable"}, 
  { 0x5091, "ChrominanceTable"}, 
  { 0x5100, "FrameDelay"}, 
  { 0x5101, "LoopCount"}, 
  { 0x5110, "PixelUnit"}, 
  { 0x5111, "PixelPerUnitX"}, 
  { 0x5112, "PixelPerUnitY"}, 
  { 0x5113, "PaletteHistogram"}, 
  { 0x1000, "RelatedImageFileFormat"},
  { 0x800D, "ImageID"},
  { 0x80E3, "Matteing"},   /* obsoleted by ExtraSamples */
  { 0x80E4, "DataType"},   /* obsoleted by SampleFormat */
  { 0x80E5, "ImageDepth"},
  { 0x80E6, "TileDepth"},
  { 0x828D, "CFARepeatPatternDim"},
  { 0x828E, "CFAPattern"},
  { 0x828F, "BatteryLevel"},
  { 0x8298, "Copyright"},
  { 0x829A, "ExposureTime"},
  { 0x829D, "FNumber"},
  { 0x83BB, "IPTC/NAA"},
  { 0x84E3, "IT8RasterPadding"},
  { 0x84E5, "IT8ColorTable"},
  { 0x8649, "ImageResourceInformation"}, /* PhotoShop */
  { 0x8769, "Exif_IFD_Pointer"},
  { 0x8773, "ICC_Profile"},
  { 0x8822, "ExposureProgram"},
  { 0x8824, "SpectralSensity"},
  { 0x8828, "OECF"},
  { 0x8825, "GPS_IFD_Pointer"},
  { 0x8827, "ISOSpeedRatings"},
  { 0x8828, "OECF"},
  { 0x9000, "ExifVersion"},
  { 0x9003, "DateTimeOriginal"},
  { 0x9004, "DateTimeDigitized"},
  { 0x9101, "ComponentsConfiguration"},
  { 0x9102, "CompressedBitsPerPixel"},
  { 0x9201, "ShutterSpeedValue"},
  { 0x9202, "ApertureValue"},
  { 0x9203, "BrightnessValue"},
  { 0x9204, "ExposureBiasValue"},
  { 0x9205, "MaxApertureValue"},
  { 0x9206, "SubjectDistance"},
  { 0x9207, "MeteringMode"},
  { 0x9208, "LightSource"},
  { 0x9209, "Flash"},
  { 0x920A, "FocalLength"},
  { 0x920B, "FlashEnergy"},                 /* 0xA20B  in JPEG   */
  { 0x920C, "SpatialFrequencyResponse"},    /* 0xA20C    -  -    */
  { 0x920D, "Noise"},
  { 0x920E, "FocalPlaneXResolution"},       /* 0xA20E    -  -    */
  { 0x920F, "FocalPlaneYResolution"},       /* 0xA20F    -  -    */
  { 0x9210, "FocalPlaneResolutionUnit"},    /* 0xA210    -  -    */
  { 0x9211, "ImageNumber"},
  { 0x9212, "SecurityClassification"},
  { 0x9213, "ImageHistory"},
  { 0x9214, "SubjectLocation"},             /* 0xA214    -  -    */
  { 0x9215, "ExposureIndex"},               /* 0xA215    -  -    */
  { 0x9216, "TIFF/EPStandardID"},
  { 0x9217, "SensingMethod"},               /* 0xA217    -  -    */
  { 0x923F, "StoNits"},
  { 0x927C, "MakerNote"},
  { 0x9286, "UserComment"},
  { 0x9290, "SubSecTime"},
  { 0x9291, "SubSecTimeOriginal"},
  { 0x9292, "SubSecTimeDigitized"},
  { 0x935C, "ImageSourceData"},             /* "Adobe Photoshop Document Data Block": 8BIM... */
  { 0x9c9b, "Title" },                      /* Win XP specific, Unicode  */
  { 0x9c9c, "Comments" },                   /* Win XP specific, Unicode  */
  { 0x9c9d, "Author" },                     /* Win XP specific, Unicode  */
  { 0x9c9e, "Keywords" },                   /* Win XP specific, Unicode  */
  { 0x9c9f, "Subject" },                    /* Win XP specific, Unicode, not to be confused with SubjectDistance and SubjectLocation */
  { 0xA000, "FlashPixVersion"},
  { 0xA001, "ColorSpace"},
  { 0xA002, "ExifImageWidth"},
  { 0xA003, "ExifImageLength"},
  { 0xA004, "RelatedSoundFile"},
  { 0xA005, "InteroperabilityOffset"},
  { 0xA20B, "FlashEnergy"},                 /* 0x920B in TIFF/EP */
  { 0xA20C, "SpatialFrequencyResponse"},    /* 0x920C    -  -    */
  { 0xA20D, "Noise"},
  { 0xA20E, "FocalPlaneXResolution"},    	/* 0x920E    -  -    */
  { 0xA20F, "FocalPlaneYResolution"},       /* 0x920F    -  -    */
  { 0xA210, "FocalPlaneResolutionUnit"},    /* 0x9210    -  -    */
  { 0xA211, "ImageNumber"},
  { 0xA212, "SecurityClassification"},
  { 0xA213, "ImageHistory"},
  { 0xA214, "SubjectLocation"},             /* 0x9214    -  -    */
  { 0xA215, "ExposureIndex"},               /* 0x9215    -  -    */
  { 0xA216, "TIFF/EPStandardID"},
  { 0xA217, "SensingMethod"},               /* 0x9217    -  -    */
  { 0xA300, "FileSource"},
  { 0xA301, "SceneType"},
  { 0xA302, "CFAPattern"},
  { 0xA401, "CustomRendered"},
  { 0xA402, "ExposureMode"},
  { 0xA403, "WhiteBalance"},
  { 0xA404, "DigitalZoomRatio"},
  { 0xA405, "FocalLengthIn35mmFilm"},
  { 0xA406, "SceneCaptureType"},
  { 0xA407, "GainControl"},
  { 0xA408, "Contrast"},
  { 0xA409, "Saturation"},
  { 0xA40A, "Sharpness"},
  { 0xA40B, "DeviceSettingDescription"},
  { 0xA40C, "SubjectDistanceRange"},
  { 0xA420, "ImageUniqueID"},
  TAG_TABLE_END
} ;

static tag_info_array tag_table_GPS = {
  { 0x0000, "GPSVersion"},
  { 0x0001, "GPSLatitudeRef"},
  { 0x0002, "GPSLatitude"},
  { 0x0003, "GPSLongitudeRef"},
  { 0x0004, "GPSLongitude"},
  { 0x0005, "GPSAltitudeRef"},
  { 0x0006, "GPSAltitude"},
  { 0x0007, "GPSTimeStamp"},
  { 0x0008, "GPSSatellites"},
  { 0x0009, "GPSStatus"},
  { 0x000A, "GPSMeasureMode"},
  { 0x000B, "GPSDOP"},
  { 0x000C, "GPSSpeedRef"},
  { 0x000D, "GPSSpeed"},
  { 0x000E, "GPSTrackRef"},
  { 0x000F, "GPSTrack"},
  { 0x0010, "GPSImgDirectionRef"},
  { 0x0011, "GPSImgDirection"},
  { 0x0012, "GPSMapDatum"},
  { 0x0013, "GPSDestLatitudeRef"},
  { 0x0014, "GPSDestLatitude"},
  { 0x0015, "GPSDestLongitudeRef"},
  { 0x0016, "GPSDestLongitude"},
  { 0x0017, "GPSDestBearingRef"},
  { 0x0018, "GPSDestBearing"},
  { 0x0019, "GPSDestDistanceRef"},
  { 0x001A, "GPSDestDistance"},
  { 0x001B, "GPSProcessingMode"},
  { 0x001C, "GPSAreaInformation"},
  { 0x001D, "GPSDateStamp"},
  { 0x001E, "GPSDifferential"},
  TAG_TABLE_END
};

static tag_info_array tag_table_IOP = {
  { 0x0001, "InterOperabilityIndex"}, /* should be 'R98' or 'THM' */
  { 0x0002, "InterOperabilityVersion"},
  { 0x1000, "RelatedFileFormat"},
  { 0x1001, "RelatedImageWidth"},
  { 0x1002, "RelatedImageHeight"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_CANON = {
  { 0x0001, "ModeArray"}, /* guess */
  { 0x0004, "ImageInfo"}, /* guess */
  { 0x0006, "ImageType"},
  { 0x0007, "FirmwareVersion"},
  { 0x0008, "ImageNumber"},
  { 0x0009, "OwnerName"},
  { 0x000C, "Camera"},
  { 0x000F, "CustomFunctions"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_CASIO = {
  { 0x0001, "RecordingMode"},
  { 0x0002, "Quality"},
  { 0x0003, "FocusingMode"},
  { 0x0004, "FlashMode"},
  { 0x0005, "FlashIntensity"},
  { 0x0006, "ObjectDistance"},
  { 0x0007, "WhiteBalance"},
  { 0x000A, "DigitalZoom"},
  { 0x000B, "Sharpness"},
  { 0x000C, "Contrast"},
  { 0x000D, "Saturation"},
  { 0x0014, "CCDSensitivity"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_FUJI = {
  { 0x0000, "Version"},
  { 0x1000, "Quality"},
  { 0x1001, "Sharpness"},
  { 0x1002, "WhiteBalance"},
  { 0x1003, "Color"},
  { 0x1004, "Tone"},
  { 0x1010, "FlashMode"},
  { 0x1011, "FlashStrength"},
  { 0x1020, "Macro"},
  { 0x1021, "FocusMode"},
  { 0x1030, "SlowSync"},
  { 0x1031, "PictureMode"},
  { 0x1100, "ContTake"},
  { 0x1300, "BlurWarning"},
  { 0x1301, "FocusWarning"},
  { 0x1302, "AEWarning "},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_NIKON = {
  { 0x0003, "Quality"},
  { 0x0004, "ColorMode"},
  { 0x0005, "ImageAdjustment"},
  { 0x0006, "CCDSensitivity"},
  { 0x0007, "WhiteBalance"},
  { 0x0008, "Focus"},
  { 0x000a, "DigitalZoom"},
  { 0x000b, "Converter"},
  TAG_TABLE_END
};
  
static tag_info_array tag_table_VND_NIKON_990 = {
  { 0x0001, "Version"},
  { 0x0002, "ISOSetting"},
  { 0x0003, "ColorMode"},
  { 0x0004, "Quality"},
  { 0x0005, "WhiteBalance"},
  { 0x0006, "ImageSharpening"},
  { 0x0007, "FocusMode"},
  { 0x0008, "FlashSetting"},
  { 0x000F, "ISOSelection"},
  { 0x0080, "ImageAdjustment"},
  { 0x0082, "AuxiliaryLens"},
  { 0x0085, "ManualFocusDistance"},
  { 0x0086, "DigitalZoom"},
  { 0x0088, "AFFocusPosition"},
  { 0x0010, "DataDump"},
  TAG_TABLE_END
};
  
static tag_info_array tag_table_VND_OLYMPUS = {
  { 0x0200, "SpecialMode"},
  { 0x0201, "JPEGQuality"},
  { 0x0202, "Macro"},
  { 0x0204, "DigitalZoom"},
  { 0x0207, "SoftwareRelease"},
  { 0x0208, "PictureInfo"},
  { 0x0209, "CameraId"},
  { 0x0F00, "DataDump"},
  TAG_TABLE_END
};

typedef enum mn_byte_order_t {
	MN_ORDER_INTEL    = 0,
	MN_ORDER_MOTOROLA = 1,
	MN_ORDER_NORMAL
} mn_byte_order_t;

typedef enum mn_offset_mode_t {
	MN_OFFSET_NORMAL,
	MN_OFFSET_MAKER,
	MN_OFFSET_GUESS
} mn_offset_mode_t;

typedef struct {
	tag_table_type   tag_table;
	char *           make;
	char *           model;
	char *           id_string;
	int              id_string_len;
	int              offset;
	mn_byte_order_t  byte_order;
	mn_offset_mode_t offset_mode;
} maker_note_type;

static const maker_note_type maker_note_array[] = {
  { tag_table_VND_CANON,     "Canon",                   NULL,  NULL,                       0,  0,  MN_ORDER_INTEL,    MN_OFFSET_GUESS},
/*  { tag_table_VND_CANON,     "Canon",                   NULL,  NULL,                       0,  0,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},*/
  { tag_table_VND_CASIO,     "CASIO",                   NULL,  NULL,                       0,  0,  MN_ORDER_MOTOROLA, MN_OFFSET_NORMAL},
  { tag_table_VND_FUJI,      "FUJIFILM",                NULL,  "FUJIFILM\x0C\x00\x00\x00", 12, 12, MN_ORDER_INTEL,    MN_OFFSET_MAKER},
  { tag_table_VND_NIKON,     "NIKON",                   NULL,  "Nikon\x00\x01\x00",        8,  8,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_NIKON_990, "NIKON",                   NULL,  NULL,                       0,  0,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_OLYMPUS,   "OLYMPUS OPTICAL CO.,LTD", NULL,  "OLYMP\x00\x01\x00",        8,  8,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
};
/* }}} */

/* {{{ exif_get_tagname
	Get headername for tag_num or NULL if not defined */
static char * exif_get_tagname(int tag_num, char *ret, int len, tag_table_type tag_table TSRMLS_DC)
{
	int i, t;
	char tmp[32];

	for (i=0;;i++) {
		if ((t=tag_table[i].Tag) == tag_num || t==TAG_END_OF_LIST) {
			if (t==TAG_END_OF_LIST) {
				break;
			}
			if (ret && len)  {
				strncpy(ret, tag_table[i].Desc, abs(len));
				if (len<0) {
					len = -len;
					ret[len-1]='\0';
					for(i=strlen(ret);i<len;i++)ret[i]=' ';
				}
				ret[len-1]='\0';
				return ret;
			}
			return tag_table[i].Desc;
		}
	}
	if (ret && len) {
		snprintf(tmp, sizeof(tmp), "UndefinedTag:0x%04X", tag_num);
		strncpy(ret, tmp, abs(len));
		if (len<0) {
			len = -len;
			ret[len-1]='\0';
			for(i=strlen(ret);i<len;i++)ret[i]=' ';
		}
		ret[len-1]='\0';
		return ret;
	}
	return "";
}
/* }}} */

/* {{{ exif_char_dump
 * Do not use! This is a debug function... */
#ifdef EXIF_DEBUG
static unsigned char* exif_char_dump(unsigned char * addr, int len, int offset)
{
	static unsigned char buf[4096+1];
	static unsigned char tmp[20];
	int c, i, p=0, n = 5+31;

	p += sprintf(buf+p, "\nDump Len: %08X (%d)", len, len);
	if (len) {
		for(i=0; i<len+15 && p+n<=sizeof(buf); i++) {
			if (i%16==0) {
				p += sprintf(buf+p, "\n%08X: ", i+offset);
			}
			if (i<len) {
				c = *addr++;
				p += sprintf(buf+p, "%02X ", c);
				tmp[i%16] = c>=32 ? c : '.';
				tmp[(i%16)+1] = '\0';
			} else {
				p += sprintf(buf+p, "   ");
			}
			if (i%16==15) {
				p += sprintf(buf+p, "    %s", tmp);
				if (i>=len) {
					break;
				}
			}
		}
	}
	buf[sizeof(buf)-1] = '\0';
	return buf;
}
#endif
/* }}} */

/* {{{ php_jpg_get16
   Get 16 bits motorola order (always) for jpeg header stuff.
*/
static int php_jpg_get16(void *value)
{
	return (((uchar *)value)[0] << 8) | ((uchar *)value)[1];
}
/* }}} */

/* {{{ php_ifd_get16u
 * Convert a 16 bit unsigned value from file's native byte order */
static int php_ifd_get16u(void *value, int motorola_intel)
{
	if (motorola_intel) {
		return (((uchar *)value)[0] << 8) | ((uchar *)value)[1];
	} else {
		return (((uchar *)value)[1] << 8) | ((uchar *)value)[0];
	}
}
/* }}} */

/* {{{ php_ifd_get16s
 * Convert a 16 bit signed value from file's native byte order */
static signed short php_ifd_get16s(void *value, int motorola_intel)
{
	return (signed short)php_ifd_get16u(value, motorola_intel);
}
/* }}} */

/* {{{ php_ifd_get32s
 * Convert a 32 bit signed value from file's native byte order */
static int php_ifd_get32s(void *value, int motorola_intel)
{
	if (motorola_intel) {
		return  (((char  *)value)[0] << 24)
			  | (((uchar *)value)[1] << 16)
			  | (((uchar *)value)[2] << 8 )
			  | (((uchar *)value)[3]      );
	} else {
		return  (((char  *)value)[3] << 24)
			  | (((uchar *)value)[2] << 16)
			  | (((uchar *)value)[1] << 8 )
			  | (((uchar *)value)[0]      );
	}
}
/* }}} */

/* {{{ php_ifd_get32u
 * Write 32 bit unsigned value to data */
static unsigned php_ifd_get32u(void *value, int motorola_intel)
{
	return (unsigned)php_ifd_get32s(value, motorola_intel) & 0xffffffff;
}
/* }}} */

/* {{{ php_ifd_set16u
 * Write 16 bit unsigned value to data */
static void php_ifd_set16u(char *data, unsigned int value, int motorola_intel)
{
	if (motorola_intel) {
		data[0] = (value & 0xFF00) >> 8;
		data[1] = (value & 0x00FF);
	} else {
		data[1] = (value & 0xFF00) >> 8;
		data[0] = (value & 0x00FF);
	}
}
/* }}} */

/* {{{ php_ifd_set32u
 * Convert a 32 bit unsigned value from file's native byte order */
static void php_ifd_set32u(char *data, size_t value, int motorola_intel)
{
	if (motorola_intel) {
		data[0] = (value & 0xFF000000) >> 24;
		data[1] = (value & 0x00FF0000) >> 16;
		data[2] = (value & 0x0000FF00) >>  8;
		data[3] = (value & 0x000000FF);
	} else {
		data[3] = (value & 0xFF000000) >> 24;
		data[2] = (value & 0x00FF0000) >> 16;
		data[1] = (value & 0x0000FF00) >>  8;
		data[0] = (value & 0x000000FF);
	}
}
/* }}} */

/* {{{ exif_convert_any_format
 * Evaluate number, be it int, rational, or float from directory. */
static double exif_convert_any_format(void *value, int format, int motorola_intel TSRMLS_DC)
{
	int 		s_den;
	unsigned 	u_den;

	switch(format) {
		case TAG_FMT_SBYTE:     return *(signed char *)value;
		case TAG_FMT_BYTE:      return *(uchar *)value;

		case TAG_FMT_USHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_ULONG:     return php_ifd_get32u(value, motorola_intel);

		case TAG_FMT_URATIONAL:
			u_den = php_ifd_get32u(4+(char *)value, motorola_intel);
			if (u_den == 0) {
				return 0;
			} else {
				return (double)php_ifd_get32u(value, motorola_intel) / u_den;
			}

		case TAG_FMT_SRATIONAL:
			s_den = php_ifd_get32s(4+(char *)value, motorola_intel);
			if (s_den == 0) {
				return 0;
			} else {
				return (double)php_ifd_get32s(value, motorola_intel) / s_den;
			}

		case TAG_FMT_SSHORT:    return (signed short)php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_SLONG:     return php_ifd_get32s(value, motorola_intel);

		/* Not sure if this is correct (never seen float used in Exif format) */
		case TAG_FMT_SINGLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type single");
#endif
			return (double)*(float *)value;
		case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type double");
#endif
			return *(double *)value;
	}
	return 0;
}
/* }}} */

/* {{{ exif_convert_any_to_int
 * Evaluate number, be it int, rational, or float from directory. */
static size_t exif_convert_any_to_int(void *value, int format, int motorola_intel TSRMLS_DC)
{
	int 		s_den;
	unsigned 	u_den;

	switch(format) {
		case TAG_FMT_SBYTE:     return *(signed char *)value;
		case TAG_FMT_BYTE:      return *(uchar *)value;

		case TAG_FMT_USHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_ULONG:     return php_ifd_get32u(value, motorola_intel);

		case TAG_FMT_URATIONAL:
			u_den = php_ifd_get32u(4+(char *)value, motorola_intel);
			if (u_den == 0) {
				return 0;
			} else {
				return php_ifd_get32u(value, motorola_intel) / u_den;
			}

		case TAG_FMT_SRATIONAL:
			s_den = php_ifd_get32s(4+(char *)value, motorola_intel);
			if (s_den == 0) {
				return 0;
			} else {
				return php_ifd_get32s(value, motorola_intel) / s_den;
			}

		case TAG_FMT_SSHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_SLONG:     return php_ifd_get32s(value, motorola_intel);

		/* Not sure if this is correct (never seen float used in Exif format) */
		case TAG_FMT_SINGLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type single");
#endif
			return (size_t)*(float *)value;
		case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found value of type double");
#endif
			return (size_t)*(double *)value;
	}
	return 0;
}
/* }}} */

/* {{{ struct image_info_value, image_info_list
*/
#ifndef WORD
#define WORD unsigned short
#endif
#ifndef DWORD
#define DWORD unsigned int
#endif

typedef struct {
	int             num;
	int             den;
} signed_rational;

typedef struct {
	unsigned int    num;
	unsigned int    den;
} unsigned_rational;

typedef union _image_info_value {
	char 				*s;
	unsigned            u;
	int 				i;
	float               f;
	double              d;
	signed_rational 	sr;
	unsigned_rational 	ur;
	union _image_info_value   *list;
} image_info_value;

typedef struct {
	WORD                tag;
	WORD                format;
	DWORD               length;
	DWORD               dummy;  /* value ptr of tiff directory entry */
	char 				*name;
	image_info_value    value;
} image_info_data;

typedef struct {
	int                 count;
	image_info_data 	*list;
} image_info_list;
/* }}} */

/* {{{ exif_get_sectionname
 Returns the name of a section
*/
#define SECTION_FILE        0
#define SECTION_COMPUTED    1
#define SECTION_ANY_TAG     2
#define SECTION_IFD0        3
#define SECTION_THUMBNAIL   4
#define SECTION_COMMENT     5
#define SECTION_APP0        6
#define SECTION_EXIF        7
#define SECTION_FPIX        8
#define SECTION_GPS         9
#define SECTION_INTEROP     10
#define SECTION_APP12       11
#define SECTION_WINXP       12
#define SECTION_MAKERNOTE   13
#define SECTION_COUNT       14

#define FOUND_FILE          (1<<SECTION_FILE)
#define FOUND_COMPUTED      (1<<SECTION_COMPUTED)
#define FOUND_ANY_TAG       (1<<SECTION_ANY_TAG)
#define FOUND_IFD0          (1<<SECTION_IFD0)
#define FOUND_THUMBNAIL     (1<<SECTION_THUMBNAIL)
#define FOUND_COMMENT       (1<<SECTION_COMMENT)
#define FOUND_APP0          (1<<SECTION_APP0)
#define FOUND_EXIF          (1<<SECTION_EXIF)
#define FOUND_FPIX          (1<<SECTION_FPIX)
#define FOUND_GPS           (1<<SECTION_GPS)
#define FOUND_INTEROP       (1<<SECTION_INTEROP)
#define FOUND_APP12         (1<<SECTION_APP12)
#define FOUND_WINXP         (1<<SECTION_WINXP)
#define FOUND_MAKERNOTE     (1<<SECTION_MAKERNOTE)

static char *exif_get_sectionname(int section)
{
	switch(section) {
		case SECTION_FILE:      return "FILE";
		case SECTION_COMPUTED:  return "COMPUTED";
		case SECTION_ANY_TAG:   return "ANY_TAG";
		case SECTION_IFD0:      return "IFD0";
		case SECTION_THUMBNAIL: return "THUMBNAIL";
		case SECTION_COMMENT:   return "COMMENT";
		case SECTION_APP0:      return "APP0";
		case SECTION_EXIF:      return "EXIF";
		case SECTION_FPIX:      return "FPIX";
		case SECTION_GPS:       return "GPS";
		case SECTION_INTEROP:   return "INTEROP";
		case SECTION_APP12:     return "APP12";
		case SECTION_WINXP:     return "WINXP";
		case SECTION_MAKERNOTE: return "MAKERNOTE";
	}
	return "";
}

static const tag_table_type exif_get_tag_table(int section)
{
	switch(section) {
		case SECTION_FILE:      return &tag_table_IFD[0];
		case SECTION_COMPUTED:  return &tag_table_IFD[0];
		case SECTION_ANY_TAG:   return &tag_table_IFD[0];
		case SECTION_IFD0:      return &tag_table_IFD[0];
		case SECTION_THUMBNAIL: return &tag_table_IFD[0];
		case SECTION_COMMENT:   return &tag_table_IFD[0];
		case SECTION_APP0:      return &tag_table_IFD[0];
		case SECTION_EXIF:      return &tag_table_IFD[0];
		case SECTION_FPIX:      return &tag_table_IFD[0];
		case SECTION_GPS:       return &tag_table_GPS[0];
		case SECTION_INTEROP:   return &tag_table_IOP[0];
		case SECTION_APP12:     return &tag_table_IFD[0];
		case SECTION_WINXP:     return &tag_table_IFD[0];
	}
	return &tag_table_IFD[0];
}
/* }}} */

/* {{{ exif_get_sectionlist
   Return list of sectionnames specified by sectionlist. Return value must be freed
*/
static char *exif_get_sectionlist(int sectionlist TSRMLS_DC)
{
	int i, len=0;
	char *sections;

	for(i=0; i<SECTION_COUNT; i++) {
		len += strlen(exif_get_sectionname(i))+2;
	}
	sections = emalloc(len+1);
	if (!sections) {
		EXIF_ERRLOG_EALLOC
		return NULL;
	}
	sections[0] = '\0';
	len = 0;
	for(i=0; i<SECTION_COUNT; i++) {
		if (sectionlist&(1<<i)) {
			sprintf(sections+len, "%s, ", exif_get_sectionname(i));
			len = strlen(sections);
		}
	}
	if (len>2)
		sections[len-2] = '\0';
	return sections;
}
/* }}} */

/* {{{ struct image_info_type
   This structure stores Exif header image elements in a simple manner
   Used to store camera data as extracted from the various ways that it can be
   stored in a nexif header
*/

typedef struct {
	int     type;
	size_t  size;
	uchar   *data;
} file_section;

typedef struct {
	int             count;
	file_section    *list;
} file_section_list;

typedef struct {
	image_filetype  filetype;
	size_t          width, height;
	size_t          size;
	size_t          offset;
	char 	        *data;
} thumbnail_data;

typedef struct {
	char			*value;
	size_t			size;
	int				tag;
} xp_field_type;

typedef struct {
	int             count;
	xp_field_type   *list;
} xp_field_list;

/* This structure is used to store a section of a Jpeg file. */
typedef struct {
	php_stream      *infile;
	char            *FileName;
	time_t          FileDateTime;
	size_t          FileSize;
	image_filetype  FileType;
	int             Height, Width;
	int             IsColor;

	char            *make;
	char            *model;

	float           ApertureFNumber;
	float           ExposureTime;
	double          FocalplaneUnits;
	float           CCDWidth;
	double          FocalplaneXRes;
	size_t          ExifImageWidth;
	float           FocalLength;
	float           Distance;

	int             motorola_intel; /* 1 Motorola; 0 Intel */

	char            *UserComment;
	int             UserCommentLength;
	char            *UserCommentEncoding;
	char            *encode_unicode;
	char            *decode_unicode_be;
	char            *decode_unicode_le;
	char            *encode_jis;
	char            *decode_jis_be;
	char            *decode_jis_le;
	char            *Copyright;/* EXIF standard defines Copyright as "<Photographer> [ '\0' <Editor> ] ['\0']" */
	char            *CopyrightPhotographer;
	char            *CopyrightEditor;

	xp_field_list   xp_fields;

	thumbnail_data  Thumbnail;
	/* other */
	int             sections_found; /* FOUND_<marker> */
	image_info_list info_list[SECTION_COUNT];
	/* for parsing */
	int             read_thumbnail;
	int             read_all;
	/* internal */
	file_section_list 	file;
} image_info_type;
/* }}} */

/* {{{ exif_error_docref */
static void exif_error_docref(const char *docref TSRMLS_DC, const image_info_type *ImageInfo, int type, const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	php_verror(docref, ImageInfo->FileName?ImageInfo->FileName:"", type, format, args TSRMLS_CC);
	va_end(args);
}
/* }}} */

/* {{{ jpeg_sof_info
 */
typedef struct {
	int     bits_per_sample;
	size_t  width;
	size_t  height;
	int     num_components;
} jpeg_sof_info;
/* }}} */

/* {{{ exif_file_sections_add
 Add a file_section to image_info
 returns the used block or -1. if size>0 and data == NULL buffer of size is allocated
*/
static int exif_file_sections_add(image_info_type *ImageInfo, int type, size_t size, uchar *data)
{
	file_section    *tmp;
	int             count = ImageInfo->file.count;

	tmp = erealloc(ImageInfo->file.list, (count+1)*sizeof(file_section));
	if (tmp == NULL) {
		return 0;
	}
	ImageInfo->file.list = tmp;
	ImageInfo->file.list[count].type = 0xFFFF;
	ImageInfo->file.list[count].data = NULL;
	ImageInfo->file.list[count].size = 0;
	ImageInfo->file.count = count+1;
	if (!size) {
		data = NULL;
	} else if (data == NULL) {
		if ((data = emalloc(size)) == NULL) {
			return -1;
		}
	}
	ImageInfo->file.list[count].type = type;
	ImageInfo->file.list[count].data = data;
	ImageInfo->file.list[count].size = size;
	return count;
}
/* }}} */

/* {{{ exif_file_sections_realloc
 Reallocate a file section returns 0 on success and -1 on failure
*/
static int exif_file_sections_realloc(image_info_type *ImageInfo, int section_index, size_t size TSRMLS_DC)
{
	void *tmp;

	if (section_index >= ImageInfo->file.count) {
		EXIF_ERRLOG_FSREALLOC
		return -1;
	}
	if (!(tmp = erealloc(ImageInfo->file.list[section_index].data, size)) && size) {
		EXIF_ERRLOG_EALLOC
		return -1;
	}
	ImageInfo->file.list[section_index].data = tmp;
	ImageInfo->file.list[section_index].size = size;
	return 0;
}
/* }}} */

/* {{{ exif_file_section_free
   Discard all file_sections in ImageInfo
*/
static int exif_file_sections_free(image_info_type *ImageInfo)
{
	int i;

	if (ImageInfo->file.count) {
		for (i=0; i<ImageInfo->file.count; i++) {
			EFREE_IF(ImageInfo->file.list[i].data);
		}
	}
	EFREE_IF(ImageInfo->file.list);
	ImageInfo->file.count = 0;
	return TRUE;
}
/* }}} */

/* {{{ exif_iif_add_value
 Add a value to image_info
*/
static void exif_iif_add_value(image_info_type *image_info, int section_index, char *name, int tag, int format, size_t length, void* value, int motorola_intel TSRMLS_DC)
{
	size_t idex;
	void *vptr;
	image_info_value *info_value;
	image_info_data  *info_data;
	image_info_data  *list;

	list = erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1)*sizeof(image_info_data));
	if (!list) {
		EXIF_ERRLOG_EALLOC
		return;
	}
	image_info->info_list[section_index].list = list;

	info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
	info_data->tag    = tag;
	info_data->format = format;
	info_data->length = length;
	info_data->name   = estrdup(name);
	if (!info_data->name) {
		EXIF_ERRLOG_EALLOC
		return;
	}
	info_value        = &info_data->value;

	switch (format) {
		case TAG_FMT_STRING:
			if (value) {
				length = php_strnlen(value, length);
				if (PG(magic_quotes_runtime)) {
					info_data->value.s = php_addslashes(value, length, &length, 0 TSRMLS_CC);
				} else {
					info_value->s = estrndup(value, length);
				}
				info_data->length = length;
			} else {
				info_data->length = 0;
				info_value->s = estrdup("");
			}
			if (!info_value->s) {
				EXIF_ERRLOG_EALLOC
				info_data->length = 0;
				break; /* better return with "" instead of possible causing problems */
			}
			break;

		default:
			/* Standard says more types possible but skip them...
			 * but allow users to handle data if they know how to
			 * So not return but use type UNDEFINED
			 * return;
			 */
			info_data->tag = TAG_FMT_UNDEFINED;/* otherwise not freed from memory */
		case TAG_FMT_SBYTE:
		case TAG_FMT_BYTE:
		/* in contrast to strings bytes do not need to allocate buffer for NULL if length==0 */
			if (!length)
				break;
		case TAG_FMT_UNDEFINED:
			if (value) {
				/* do not recompute length here */
				if (PG(magic_quotes_runtime)) {
					info_data->value.s = php_addslashes(value, length, &length, 0 TSRMLS_CC);
				} else {
					info_value->s = estrndup(value, length);
				}
				info_data->length = length;
			} else {
				info_data->length = 0;
				info_value->s = estrdup("");
			}
			if (!info_value->s) {
				EXIF_ERRLOG_EALLOC
				return;
			}
			break;

		case TAG_FMT_USHORT:
		case TAG_FMT_ULONG:
		case TAG_FMT_URATIONAL:
		case TAG_FMT_SSHORT:
		case TAG_FMT_SLONG:
		case TAG_FMT_SRATIONAL:
		case TAG_FMT_SINGLE:
		case TAG_FMT_DOUBLE:
			if (length==0) {
				break;
			} else
			if (length>1) {
				info_data->value.list = emalloc(length*sizeof(image_info_value));
				if (!info_data->value.list) {
					EXIF_ERRLOG_EALLOC
					return;
				}
			} else {
				info_value = &info_data->value;
			}
			for (idex=0,vptr=value; idex<length; idex++,(char *)vptr+=php_tiff_bytes_per_format[format]) {
				if (length>1) {
					info_value = &info_data->value.list[idex];
				}
				switch (format) {
					case TAG_FMT_USHORT:
						info_value->u = php_ifd_get16u(vptr, motorola_intel);
						break;

					case TAG_FMT_ULONG:
						info_value->u = php_ifd_get32u(vptr, motorola_intel);
						break;

					case TAG_FMT_URATIONAL:
						info_value->ur.num = php_ifd_get32u(vptr, motorola_intel);
						info_value->ur.den = php_ifd_get32u(4+(char *)vptr, motorola_intel);
						break;

					case TAG_FMT_SSHORT:
						info_value->i = php_ifd_get16s(vptr, motorola_intel);
						break;

					case TAG_FMT_SLONG:
						info_value->i = php_ifd_get32s(vptr, motorola_intel);
						break;

					case TAG_FMT_SRATIONAL:
						info_value->sr.num = php_ifd_get32u(vptr, motorola_intel);
						info_value->sr.den = php_ifd_get32u(4+(char *)vptr, motorola_intel);
						break;

					case TAG_FMT_SINGLE:
#ifdef EXIF_DEBUG
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Found value of type single");
#endif
						info_value->f = *(float *)value;

					case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Found value of type double");
#endif
						info_value->d = *(double *)value;
						break;
				}
			}
	}
	image_info->sections_found |= 1<<section_index;
	image_info->info_list[section_index].count++;
}
/* }}} */

/* {{{ exif_iif_add_tag
 Add a tag from IFD to image_info
*/
static void exif_iif_add_tag(image_info_type *image_info, int section_index, char *name, int tag, int format, size_t length, void* value TSRMLS_DC)
{
	exif_iif_add_value(image_info, section_index, name, tag, format, length, value, image_info->motorola_intel TSRMLS_CC);
}
/* }}} */

/* {{{ exif_iif_add_int
 Add an int value to image_info
*/
static void exif_iif_add_int(image_info_type *image_info, int section_index, char *name, int value TSRMLS_DC)
{
	image_info_data  *info_data;
	image_info_data  *list;

	list = erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1)*sizeof(image_info_data));
	if (!list) {
		EXIF_ERRLOG_EALLOC
		return;
	}
	image_info->info_list[section_index].list = list;

	info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
	info_data->tag    = TAG_NONE;
	info_data->format = TAG_FMT_SLONG;
	info_data->length = 1;
	info_data->name   = estrdup(name);
	if (!info_data->name) {
		EXIF_ERRLOG_EALLOC
		return;
	}
	info_data->value.i = value;
	image_info->sections_found |= 1<<section_index;
	image_info->info_list[section_index].count++;
}
/* }}} */

/* {{{ exif_iif_add_str
 Add a string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_str(image_info_type *image_info, int section_index, char *name, char *value TSRMLS_DC)
{
	image_info_data  *info_data;
	image_info_data  *list;

	if (value) {
		list = erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1)*sizeof(image_info_data));
		if (!list) {
			EXIF_ERRLOG_EALLOC
			return;
		}
		image_info->info_list[section_index].list = list;
		info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
		info_data->tag    = TAG_NONE;
		info_data->format = TAG_FMT_STRING;
		info_data->length = 1;
		info_data->name   = estrdup(name);
		if (!info_data->name) {
			EXIF_ERRLOG_EALLOC
			return;
		}
		if (PG(magic_quotes_runtime)) {
			info_data->value.s = php_addslashes(value, strlen(value), NULL, 0 TSRMLS_CC);
		} else {
			info_data->value.s = estrdup(value);
			if (!info_data->value.s) {
				EXIF_ERRLOG_EALLOC
				return;
			}
		}
		image_info->sections_found |= 1<<section_index;
		image_info->info_list[section_index].count++;
	}
}
/* }}} */

/* {{{ exif_iif_add_fmt
 Add a format string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_fmt(image_info_type *image_info, int section_index, char *name TSRMLS_DC, char *value, ...)
{
	char             *tmp;
	va_list 		 arglist;

	va_start(arglist, value);
	if (value) {
		vspprintf(&tmp, 0, value, arglist);
		exif_iif_add_str(image_info, section_index, name, tmp TSRMLS_CC);
		efree(tmp);
	}
	va_end(arglist);
}
/* }}} */

/* {{{ exif_iif_add_str
 Add a string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_buffer(image_info_type *image_info, int section_index, char *name, int length, char *value TSRMLS_DC)
{
	image_info_data  *info_data;
	image_info_data  *list;

	if (value) {
		list = erealloc(image_info->info_list[section_index].list, (image_info->info_list[section_index].count+1)*sizeof(image_info_data));
		if (!list) {
			EXIF_ERRLOG_EALLOC
			return;
		}
		image_info->info_list[section_index].list = list;
		info_data  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
		info_data->tag    = TAG_NONE;
		info_data->format = TAG_FMT_UNDEFINED;
		info_data->length = length;
		info_data->name   = estrdup(name);
		if (!info_data->name) {
			EXIF_ERRLOG_EALLOC
			return;
		}
		if (PG(magic_quotes_runtime)) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL TSRMLS_CC, image_info, E_NOTICE, "Adding %s as buffer%s", name, exif_char_dump(value, length, 0));
#endif
			info_data->value.s = php_addslashes(value, length, &length, 0 TSRMLS_CC);
			info_data->length = length;
		} else {
			info_data->value.s = emalloc(length+1);
			if (!info_data->value.s) {
				EXIF_ERRLOG_EALLOC
				return;
			}
			memcpy(info_data->value.s, value, length);
			info_data->value.s[length] = 0;
		}
		image_info->sections_found |= 1<<section_index;
		image_info->info_list[section_index].count++;
	}
}
/* }}} */

/* {{{ exif_iif_free
 Free memory allocated for image_info
*/
static void exif_iif_free(image_info_type *image_info, int section_index) {
	int  i;
	void *f; /* faster */

	if (image_info->info_list[section_index].count) {
		for (i=0; i < image_info->info_list[section_index].count; i++) {
			if ((f=image_info->info_list[section_index].list[i].name) != NULL) {
				efree(f);
			}
			switch(image_info->info_list[section_index].list[i].format) {
				case TAG_FMT_SBYTE:
				case TAG_FMT_BYTE:
					/* in contrast to strings bytes do not need to allocate buffer for NULL if length==0 */
					if (image_info->info_list[section_index].list[i].length<1)
						break;
				default:
				case TAG_FMT_UNDEFINED:
				case TAG_FMT_STRING:
					if ((f=image_info->info_list[section_index].list[i].value.s) != NULL) {
						efree(f);
					}
					break;

				case TAG_FMT_USHORT:
				case TAG_FMT_ULONG:
				case TAG_FMT_URATIONAL:
				case TAG_FMT_SSHORT:
				case TAG_FMT_SLONG:
				case TAG_FMT_SRATIONAL:
				case TAG_FMT_SINGLE:
				case TAG_FMT_DOUBLE:
					/* nothing to do here */
					if (image_info->info_list[section_index].list[i].length > 1) {
						if ((f=image_info->info_list[section_index].list[i].value.list) != NULL) {
							efree(f);
						}
					}
					break;
			}
		}
	}
	EFREE_IF(image_info->info_list[section_index].list);
}
/* }}} */

/* {{{ add_assoc_image_info
 * Add image_info to associative array value. */
static void add_assoc_image_info(pval *value, int sub_array, image_info_type *image_info, int section_index TSRMLS_DC)
{
	char    buffer[64], *val, *name, uname[64];
	int     i, ap, l, b, idx=0, done, unknown=0;
#ifdef EXIF_DEBUG
	int     info_tag;
#endif
	image_info_value *info_value;
	image_info_data  *info_data;
	pval 			 *tmpi, *array = NULL;

#ifdef EXIF_DEBUG
/*		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Adding %d infos from section %s", image_info->info_list[section_index].count, exif_get_sectionname(section_index));*/
#endif
	if (image_info->info_list[section_index].count) {
		if (sub_array) {
			MAKE_STD_ZVAL(tmpi);
			array_init(tmpi);
		} else {
			tmpi = value;
		}

		for(i=0; i<image_info->info_list[section_index].count; i++) {
			done       = 0;
			info_data  = &image_info->info_list[section_index].list[i];
#ifdef EXIF_DEBUG
			info_tag   = info_data->tag; /* conversion */
#endif
			info_value = &info_data->value;
			if (!(name = info_data->name)) {
				snprintf(uname, sizeof(uname), "%d", unknown++);
				name = uname;
			}
#ifdef EXIF_DEBUG
/*		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Adding infos: tag(0x%04X,%12s,L=0x%04X): %s", info_tag, exif_get_tagname(info_tag, buffer, -12, exif_get_tag_table(section_index) TSRMLS_CC), info_data->length, info_data->format==TAG_FMT_STRING?(info_value&&info_value->s?info_value->s:"<no data>"):exif_get_tagformat(info_data->format));*/
#endif
			if (info_data->length==0) {
				add_assoc_null(tmpi, name);
			} else {
				switch (info_data->format) {
					default:
						/* Standard says more types possible but skip them...
						 * but allow users to handle data if they know how to
						 * So not return but use type UNDEFINED
						 * return;
						 */
					case TAG_FMT_BYTE:
					case TAG_FMT_SBYTE:
					case TAG_FMT_UNDEFINED:
						if (!info_value->s) {
							add_assoc_stringl(tmpi, name, "", 0, 1);
						} else {
							add_assoc_stringl(tmpi, name, info_value->s, info_data->length, 1);
						}
						break;

					case TAG_FMT_STRING:
						if (!(val = info_value->s)) {
							val = "";
						}
						if (section_index==SECTION_COMMENT) {
							add_index_string(tmpi, idx++, val, 1);
						} else {
							add_assoc_string(tmpi, name, val, 1);
						}
						break;

					case TAG_FMT_URATIONAL:
					case TAG_FMT_SRATIONAL:
					/*case TAG_FMT_BYTE:
					case TAG_FMT_SBYTE:*/
					case TAG_FMT_USHORT:
					case TAG_FMT_SSHORT:
					case TAG_FMT_SINGLE:
					case TAG_FMT_DOUBLE:
					case TAG_FMT_ULONG:
					case TAG_FMT_SLONG:
						/* now the rest, first see if it becomes an array */
						if ((l = info_data->length) > 1) {
							array = NULL;
							MAKE_STD_ZVAL(array);
							array_init(array);
						}
						for(ap=0; ap<l; ap++) {
							if (l>1) {
								info_value = &info_data->value.list[ap];
							}
							switch (info_data->format) {
								case TAG_FMT_BYTE:
									if (l>1) {
										info_value = &info_data->value;
										for (b=0;b<l;b++) {
											add_index_long(array, b, (int)(info_value->s[b]));
										}
										break;
									}
								case TAG_FMT_USHORT:
								case TAG_FMT_ULONG:
									if (l==1) {
										add_assoc_long(tmpi, name, (int)info_value->u);
									} else {
										add_index_long(array, ap, (int)info_value->u);
									}
									break;

								case TAG_FMT_URATIONAL:
									snprintf(buffer, sizeof(buffer), "%i/%i", info_value->ur.num, info_value->ur.den);
									if (l==1) {
										add_assoc_string(tmpi, name, buffer, 1);
									} else {
										add_index_string(array, ap, buffer, 1);
									}
									break;

								case TAG_FMT_SBYTE:
									if (l>1) {
										info_value = &info_data->value;
										for (b=0;b<l;b++) {
											add_index_long(array, ap, (int)info_value->s[b]);
										}
										break;
									}
								case TAG_FMT_SSHORT:
								case TAG_FMT_SLONG:
									if (l==1) {
										add_assoc_long(tmpi, name, info_value->i);
									} else {
										add_index_long(array, ap, info_value->i);
									}
									break;

								case TAG_FMT_SRATIONAL:
									snprintf(buffer, sizeof(buffer), "%i/%i", info_value->sr.num, info_value->sr.den);
									if (l==1) {
										add_assoc_string(tmpi, name, buffer, 1);
									} else {
										add_index_string(array, ap, buffer, 1);
									}
									break;

								case TAG_FMT_SINGLE:
									if (l==1) {
										add_assoc_double(tmpi, name, info_value->f);
									} else {
										add_index_double(array, ap, info_value->f);
									}
									break;

								case TAG_FMT_DOUBLE:
									if (l==1) {
										add_assoc_double(tmpi, name, info_value->d);
									} else {
										add_index_double(array, ap, info_value->d);
									}
									break;
							}
							info_value = &info_data->value.list[ap];
						}
						if (l>1) {
							add_assoc_zval(tmpi, name, array);
						}
						break;
				}
			}
		}
		if (sub_array) {
			add_assoc_zval(value, exif_get_sectionname(section_index), tmpi);
		}
	}
}
/* }}} */

/* {{{ Markers
   JPEG markers consist of one or more 0xFF bytes, followed by a marker
   code byte (which is not an FF).  Here are the marker codes of interest
   in this program.  (See jdmarker.c for a more complete list.)
*/

#define M_TEM   0x01    /* temp for arithmetic coding              */
#define M_RES   0x02    /* reserved                                */
#define M_SOF0  0xC0    /* Start Of Frame N                        */
#define M_SOF1  0xC1    /* N indicates which compression process   */
#define M_SOF2  0xC2    /* Only SOF0-SOF2 are now in common use    */
#define M_SOF3  0xC3
#define M_DHT   0xC4
#define M_SOF5  0xC5    /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_JPEG  0x08    /* reserved for extensions                 */
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_DAC   0xCC    /* arithmetic table                         */
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_RST0  0xD0    /* restart segment                          */
#define M_RST1  0xD1
#define M_RST2  0xD2
#define M_RST3  0xD3
#define M_RST4  0xD4
#define M_RST5  0xD5
#define M_RST6  0xD6
#define M_RST7  0xD7
#define M_SOI   0xD8    /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9    /* End Of Image (end of datastream)         */
#define M_SOS   0xDA    /* Start Of Scan (begins compressed data)   */
#define M_DQT   0xDB
#define M_DNL   0xDC
#define M_DRI   0xDD
#define M_DHP   0xDE
#define M_EXP   0xDF
#define M_APP0  0xE0    /* JPEG: 'JFIFF' AND (additional 'JFXX')    */
#define M_EXIF  0xE1    /* Exif Attribute Information               */
#define M_APP2  0xE2    /* Flash Pix Extension Data?                */
#define M_APP3  0xE3
#define M_APP4  0xE4
#define M_APP5  0xE5
#define M_APP6  0xE6
#define M_APP7  0xE7
#define M_APP8  0xE8
#define M_APP9  0xE9
#define M_APP10 0xEA
#define M_APP11 0xEB
#define M_APP12 0xEC
#define M_APP13 0xED    /* IPTC International Press Telecommunications Council */
#define M_APP14 0xEE    /* Software, Copyright?                     */
#define M_APP15 0xEF
#define M_JPG0  0xF0
#define M_JPG1  0xF1
#define M_JPG2  0xF2
#define M_JPG3  0xF3
#define M_JPG4  0xF4
#define M_JPG5  0xF5
#define M_JPG6  0xF6
#define M_JPG7  0xF7
#define M_JPG8  0xF8
#define M_JPG9  0xF9
#define M_JPG10 0xFA
#define M_JPG11 0xFB
#define M_JPG12 0xFC
#define M_JPG13 0xFD
#define M_COM   0xFE    /* COMment                                  */

#define M_PSEUDO 0x123 	/* Extra value.                             */

/* }}} */

/* {{{ jpeg2000 markers
 */
/* Markers x30 - x3F do not have a segment */
/* Markers x00, x01, xFE, xC0 - xDF ISO/IEC 10918-1 -> M_<xx> */
/* Markers xF0 - xF7 ISO/IEC 10918-3 */
/* Markers xF7 - xF8 ISO/IEC 14495-1 */
/* XY=Main/Tile-header:(R:required, N:not_allowed, O:optional, L:last_marker) */
#define JC_SOC   0x4F   /* NN, Start of codestream                          */
#define JC_SIZ   0x51   /* RN, Image and tile size                          */
#define JC_COD   0x52   /* RO, Codeing style defaulte                       */
#define JC_COC   0x53   /* OO, Coding style component                       */
#define JC_TLM   0x55   /* ON, Tile part length main header                 */
#define JC_PLM   0x57   /* ON, Packet length main header                    */
#define JC_PLT   0x58   /* NO, Packet length tile part header               */
#define JC_QCD   0x5C   /* RO, Quantization default                         */
#define JC_QCC   0x5D   /* OO, Quantization component                       */
#define JC_RGN   0x5E   /* OO, Region of interest                           */
#define JC_POD   0x5F   /* OO, Progression order default                    */
#define JC_PPM   0x60   /* ON, Packed packet headers main header            */
#define JC_PPT   0x61   /* NO, Packet packet headers tile part header       */
#define JC_CME   0x64   /* OO, Comment: "LL E <text>" E=0:binary, E=1:ascii */
#define JC_SOT   0x90   /* NR, Start of tile                                */
#define JC_SOP   0x91   /* NO, Start of packeter default                    */
#define JC_EPH   0x92   /* NO, End of packet header                         */
#define JC_SOD   0x93   /* NL, Start of data                                */
#define JC_EOC   0xD9   /* NN, End of codestream                            */
/* }}} */

/* {{{ exif_process_COM
   Process a COM marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
static void exif_process_COM (image_info_type *image_info, char *value, size_t length TSRMLS_DC)
{
	exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_STRING, length-2, value+2 TSRMLS_CC);
}
/* }}} */

/* {{{ exif_process_CME
   Process a CME marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
#ifdef EXIF_JPEG2000
static void exif_process_CME (image_info_type *image_info, char *value, size_t length TSRMLS_DC)
{
	if (length>3) {
		switch(value[2]) {
			case 0:
				exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_UNDEFINED, length, value TSRMLS_CC);
				break;
			case 1:
				exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_STRING, length, value);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined JPEG2000 comment encoding");
				break;
		}
	} else {
		exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_UNDEFINED, 0, NULL);
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "JPEG2000 comment section to small");
	}
}
#endif
/* }}} */

/* {{{ exif_process_SOFn
 * Process a SOFn marker.  This is useful for the image dimensions */
static void exif_process_SOFn (uchar *Data, int marker, jpeg_sof_info *result)
{
/* 0xFF SOSn SectLen(2) Bits(1) Height(2) Width(2) Channels(1)  3*Channels (1)  */
	result->bits_per_sample = Data[2];
	result->height          = php_jpg_get16(Data+3);
	result->width           = php_jpg_get16(Data+5);
	result->num_components  = Data[7];

/*	switch (marker) {
		case M_SOF0:  process = "Baseline";  break;
		case M_SOF1:  process = "Extended sequential";  break;
		case M_SOF2:  process = "Progressive";  break;
		case M_SOF3:  process = "Lossless";  break;
		case M_SOF5:  process = "Differential sequential";  break;
		case M_SOF6:  process = "Differential progressive";  break;
		case M_SOF7:  process = "Differential lossless";  break;
		case M_SOF9:  process = "Extended sequential, arithmetic coding";  break;
		case M_SOF10: process = "Progressive, arithmetic coding";  break;
		case M_SOF11: process = "Lossless, arithmetic coding";  break;
		case M_SOF13: process = "Differential sequential, arithmetic coding";  break;
		case M_SOF14: process = "Differential progressive, arithmetic coding"; break;
		case M_SOF15: process = "Differential lossless, arithmetic coding";  break;
		default:      process = "Unknown";  break;
	}*/
}
/* }}} */

/* forward declarations */
static int exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, char *offset_base, size_t IFDlength, size_t displacement, int section_index TSRMLS_DC);
static int exif_process_IFD_TAG(    image_info_type *ImageInfo, char *dir_entry, char *offset_base, size_t IFDlength, size_t displacement, int section_index, int ReadNextIFD, tag_table_type tag_table TSRMLS_DC);

/* {{{ exif_get_markername
	Get name of marker */
#ifdef EXIF_DEBUG
static char * exif_get_markername(int marker)
{
	switch(marker) {
		case 0xC0: return "SOF0";
		case 0xC1: return "SOF1";
		case 0xC2: return "SOF2";
		case 0xC3: return "SOF3";
		case 0xC4: return "DHT";
		case 0xC5: return "SOF5";
		case 0xC6: return "SOF6";
		case 0xC7: return "SOF7";
		case 0xC9: return "SOF9";
		case 0xCA: return "SOF10";
		case 0xCB: return "SOF11";
		case 0xCD: return "SOF13";
		case 0xCE: return "SOF14";
		case 0xCF: return "SOF15";
		case 0xD8: return "SOI";
		case 0xD9: return "EOI";
		case 0xDA: return "SOS";
		case 0xDB: return "DQT";
		case 0xDC: return "DNL";
		case 0xDD: return "DRI";
		case 0xDE: return "DHP";
		case 0xDF: return "EXP";
		case 0xE0: return "APP0";
		case 0xE1: return "EXIF";
		case 0xE2: return "FPIX";
		case 0xE3: return "APP3";
		case 0xE4: return "APP4";
		case 0xE5: return "APP5";
		case 0xE6: return "APP6";
		case 0xE7: return "APP7";
		case 0xE8: return "APP8";
		case 0xE9: return "APP9";
		case 0xEA: return "APP10";
		case 0xEB: return "APP11";
		case 0xEC: return "APP12";
		case 0xED: return "APP13";
		case 0xEE: return "APP14";
		case 0xEF: return "APP15";
		case 0xF0: return "JPG0";
		case 0xFD: return "JPG13";
		case 0xFE: return "COM";
		case 0x01: return "TEM";
	}
	return "Unknown";
}
#endif
/* }}} */

/* {{{ proto string|false exif_tagname(index)
	Get headername for index or false if not defined */
PHP_FUNCTION(exif_tagname)
{
	pval **p_num;
	int tag, ac = ZEND_NUM_ARGS();
	char *szTemp;

	if ((ac < 1 || ac > 1) || zend_get_parameters_ex(ac, &p_num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(p_num);
	tag = Z_LVAL_PP(p_num);
	szTemp = exif_get_tagname(tag, NULL, 0, tag_table_IFD TSRMLS_CC);
	if (tag<0 || !szTemp || !szTemp[0]) {
		RETURN_BOOL(FALSE);
	} else {
		RETURN_STRING(szTemp, 1)
	}
}
/* }}} */

/* {{{ exif_ifd_make_value
 * Create a value for an ifd from an info_data pointer */
static void* exif_ifd_make_value(image_info_data *info_data, int motorola_intel TSRMLS_DC) {
	size_t  byte_count;
	char    *value_ptr, *data_ptr;
	size_t  i;

	image_info_value  *info_value;

	byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
	value_ptr = emalloc(max(byte_count, 4));
	if (!value_ptr) {
		EXIF_ERRLOG_EALLOC
		return NULL;
	}
	memset(value_ptr, 0, 4);
	if (!info_data->length) {
		return value_ptr;
	}
	if (info_data->format == TAG_FMT_UNDEFINED || info_data->format == TAG_FMT_STRING
	  || (byte_count>1 && (info_data->format == TAG_FMT_BYTE || info_data->format == TAG_FMT_SBYTE))
	) {
		memmove(value_ptr, info_data->value.s, byte_count);
		return value_ptr;
	} else if (info_data->format == TAG_FMT_BYTE) {
		*value_ptr = info_data->value.u;
		return value_ptr;
	} else if (info_data->format == TAG_FMT_SBYTE) {
		*value_ptr = info_data->value.i;
		return value_ptr;
	} else {
		data_ptr = value_ptr;
		for(i=0; i<info_data->length; i++) {
			if (info_data->length==1) {
				info_value = &info_data->value;
			} else {
				info_value = &info_data->value.list[i];
			}
			switch(info_data->format) {
				case TAG_FMT_USHORT:
					php_ifd_set16u(data_ptr, info_value->u, motorola_intel);
					data_ptr += 2;
					break;
				case TAG_FMT_ULONG:
					php_ifd_set32u(data_ptr, info_value->u, motorola_intel);
					data_ptr += 4;
					break;
				case TAG_FMT_SSHORT:
					php_ifd_set16u(data_ptr, info_value->i, motorola_intel);
					data_ptr += 2;
					break;
				case TAG_FMT_SLONG:
					php_ifd_set32u(data_ptr, info_value->i, motorola_intel);
					data_ptr += 4;
					break;
				case TAG_FMT_URATIONAL:
					php_ifd_set32u(data_ptr,   info_value->sr.num, motorola_intel);
					php_ifd_set32u(data_ptr+4, info_value->sr.den, motorola_intel);
					data_ptr += 8;
					break;
				case TAG_FMT_SRATIONAL:
					php_ifd_set32u(data_ptr,   info_value->ur.num, motorola_intel);
					php_ifd_set32u(data_ptr+4, info_value->ur.den, motorola_intel);
					data_ptr += 8;
					break;
				case TAG_FMT_SINGLE:
					memmove(data_ptr, &info_data->value.f, byte_count);
					data_ptr += 4;
					break;
				case TAG_FMT_DOUBLE:
					memmove(data_ptr, &info_data->value.d, byte_count);
					data_ptr += 8;
					break;
			}
		}
	}
	return value_ptr;
}
/* }}} */

/* {{{ exif_thumbnail_build
 * Check and build thumbnail */
static void exif_thumbnail_build(image_info_type *ImageInfo TSRMLS_DC) {
	size_t            new_size, new_move, new_value;
	char              *new_data;
	void              *value_ptr;
	int               i, byte_count;
	image_info_list   *info_list;
	image_info_data   *info_data;
#ifdef EXIF_DEBUG
	char              tagname[64];
#endif

	if (!ImageInfo->read_thumbnail || !ImageInfo->Thumbnail.offset || !ImageInfo->Thumbnail.size) {
		return; /* ignore this call */
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail: filetype = %d", ImageInfo->Thumbnail.filetype);
#endif
	switch(ImageInfo->Thumbnail.filetype) {
		default:
		case IMAGE_FILETYPE_JPEG:
			/* done */
			break;
		case IMAGE_FILETYPE_TIFF_II:
		case IMAGE_FILETYPE_TIFF_MM:
			info_list = &ImageInfo->info_list[SECTION_THUMBNAIL];
			new_size  = 8 + 2 + info_list->count * 12 + 4;
#ifdef EXIF_DEBUG
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail: size of signature + directory(%d): 0x%02X", info_list->count, new_size);
#endif
			new_value= new_size; /* offset for ifd values outside ifd directory */
			for (i=0; i<info_list->count; i++) {
				info_data  = &info_list->list[i];
				byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
				if (byte_count > 4) {
					new_size += byte_count;
				}
			}
			new_move = new_size;
			new_data = erealloc(ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size+new_size);
			if (!new_data) {
				EXIF_ERRLOG_EALLOC
				efree(ImageInfo->Thumbnail.data);
				ImageInfo->Thumbnail.data = NULL;
				ImageInfo->Thumbnail.size = 0;
				return;
			}
			ImageInfo->Thumbnail.data = new_data;
			memmove(ImageInfo->Thumbnail.data + new_move, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
			ImageInfo->Thumbnail.size += new_size;
			/* fill in data */
			if (ImageInfo->motorola_intel) {
				memmove(new_data, "MM\x00\x2a\x00\x00\x00\x08", 8);
			} else {
				memmove(new_data, "II\x2a\x00\x08\x00\x00\x00", 8);
			}
			new_data += 8;
			php_ifd_set16u(new_data, info_list->count, ImageInfo->motorola_intel);
			new_data += 2;
			for (i=0; i<info_list->count; i++) {
				info_data  = &info_list->list[i];
				byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail: process tag(x%04X=%s): %s%s (%d bytes)", info_data->tag, exif_get_tagname(info_data->tag, tagname, -12, tag_table_IFD TSRMLS_CC), (info_data->length>1)&&info_data->format!=TAG_FMT_UNDEFINED&&info_data->format!=TAG_FMT_STRING?"ARRAY OF ":"", exif_get_tagformat(info_data->format), byte_count);
#endif
				if (info_data->tag==TAG_STRIP_OFFSETS || info_data->tag==TAG_JPEG_INTERCHANGE_FORMAT) {
					php_ifd_set16u(new_data + 0, info_data->tag,    ImageInfo->motorola_intel);
					php_ifd_set16u(new_data + 2, TAG_FMT_ULONG,     ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 4, 1,                 ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 8, new_move,          ImageInfo->motorola_intel);
				} else {
					php_ifd_set16u(new_data + 0, info_data->tag,    ImageInfo->motorola_intel);
					php_ifd_set16u(new_data + 2, info_data->format, ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 4, info_data->length, ImageInfo->motorola_intel);
					value_ptr  = exif_ifd_make_value(info_data, ImageInfo->motorola_intel TSRMLS_CC);
					if (!value_ptr) {
						EXIF_ERRLOG_EALLOC
						efree(ImageInfo->Thumbnail.data);
						ImageInfo->Thumbnail.data = NULL;
						ImageInfo->Thumbnail.size = 0;
						return;
					}
					if (byte_count <= 4) {
						memmove(new_data+8, value_ptr, 4);
					} else {
						php_ifd_set32u(new_data+8, new_value, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
						exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail: writing with value offset: 0x%04X + 0x%02X", new_value, byte_count);
#endif
						memmove(ImageInfo->Thumbnail.data+new_value, value_ptr, byte_count);
						new_value += byte_count;
					}
					efree(value_ptr);
				}
				new_data += 12;
			}
			memset(new_data, 0, 4); /* next ifd pointer */
#ifdef EXIF_DEBUG
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail: created");
#endif
			break;
	}
}
/* }}} */

/* {{{ exif_thumbnail_extract
 * Grab the thumbnail, corrected */
static void exif_thumbnail_extract(image_info_type *ImageInfo, char *offset, size_t length TSRMLS_DC) {
	if (ImageInfo->Thumbnail.data) {
		exif_error_docref("exif_read_data#error_mult_thumb" TSRMLS_CC, ImageInfo, E_WARNING, "multiple possible thumbnails");
		return; /* Should not happen */
	}
	if (!ImageInfo->read_thumbnail)	{
		return; /* ignore this call */
	}
	/* according to exif2.1, the thumbnail is not supposed to be greater than 64K */
	if (ImageInfo->Thumbnail.size >= 65536
	 || ImageInfo->Thumbnail.size <= 0
	 || ImageInfo->Thumbnail.offset <= 0
	) {
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "illegal thumbnail size/offset");
		return;
	}
	/* Check to make sure we are not going to go past the ExifLength */
	if ((ImageInfo->Thumbnail.offset + ImageInfo->Thumbnail.size) > length) {
		EXIF_ERRLOG_THUMBEOF
		return;
	}
	ImageInfo->Thumbnail.data = estrndup(offset + ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
	if (!ImageInfo->Thumbnail.data) {
		EXIF_ERRLOG_EALLOC
	}
	exif_thumbnail_build(ImageInfo TSRMLS_CC);
}
/* }}} */

/* {{{ exif_process_undefined
 * Copy a string/buffer in Exif header to a character string and return length of allocated buffer if any. */
static int exif_process_undefined(char **result, char *value, size_t byte_count TSRMLS_DC) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 * estrndup does not return length
	 */
	if (byte_count) {
		(*result) = estrndup(value, byte_count); /* NULL @ byte_count!!! */
		if (!*result) {
			EXIF_ERRLOG_EALLOC
			return 0;
		}
		return byte_count+1;
	}
	return 0;
}
/* }}} */

/* {{{ exif_process_string_raw
 * Copy a string in Exif header to a character string returns length of allocated buffer if any. */
#if !EXIF_USE_MBSTRING
static int exif_process_string_raw(char **result, char *value, size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 */
	if (byte_count) {
		(*result) = emalloc(byte_count+1);
		memcpy(*result, value, byte_count);
		(*result)[byte_count] = '\0';
		return byte_count+1;
	}
	return 0;
}
#endif
/* }}} */

/* {{{ exif_process_string
 * Copy a string in Exif header to a character string and return length of allocated buffer if any.
 * In contrast to exif_process_string this function does allways return a string buffer */
static int exif_process_string(char **result, char *value, size_t byte_count TSRMLS_DC) {
	/* we cannot use strlcpy - here the problem is that we cannot use strlen to
	 * determin length of string and we cannot use strlcpy with len=byte_count+1
	 * because then we might get into an EXCEPTION if we exceed an allocated
	 * memory page...so we use php_strnlen in conjunction with memcpy and add the NUL
	 * char.
	 * estrdup would sometimes allocate more memory and does not return length
	 */
	if ((byte_count=php_strnlen(value, byte_count)) > 0) {
		return exif_process_undefined(result, value, byte_count TSRMLS_CC);
	}
	(*result) = estrndup("", 1); /* force empty string */
	if (!*result) {
		EXIF_ERRLOG_EALLOC
		return 0;
	}
	return byte_count+1;
}
/* }}} */

/* {{{ exif_process_user_comment
 * Process UserComment in IFD. */
static int exif_process_user_comment(image_info_type *ImageInfo, char **pszInfoPtr, char **pszEncoding, char *szValuePtr, int ByteCount TSRMLS_DC)
{
	int   a;

#if EXIF_USE_MBSTRING
	char  *decode;
	size_t len;;
#endif

	*pszEncoding = NULL;
	/* Copy the comment */
	if (ByteCount>=8) {
		if (!memcmp(szValuePtr, "UNICODE\0", 8)) {
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
#if EXIF_USE_MBSTRING
			/* First try to detect BOM: ZERO WIDTH NOBREAK SPACE (FEFF 16) 
			 * since we have no encoding support for the BOM yet we skip that.
			 */
			if (!memcmp(szValuePtr, "\xFE\xFF", 2)) {
				decode = "UCS-2BE";
				szValuePtr = szValuePtr+2;
				ByteCount -= 2;
			} else if (!memcmp(szValuePtr, "\xFF\xFE", 2)) {
				decode = "UCS-2LE";
				szValuePtr = szValuePtr+2;
				ByteCount -= 2;
			} else if (ImageInfo->motorola_intel) {
				decode = ImageInfo->decode_unicode_be;
			} else {
				decode = ImageInfo->decode_unicode_le;
			}
			*pszInfoPtr = php_mb_convert_encoding(szValuePtr, ByteCount, ImageInfo->encode_unicode, decode, &len TSRMLS_CC);
			return len;
#else
			return exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
#endif
		} else
		if (!memcmp(szValuePtr, "ASCII\0\0\0", 8)) {
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
		} else
		if (!memcmp(szValuePtr, "JIS\0\0\0\0\0", 8)) {
			/* JIS should be tanslated to MB or we leave it to the user - leave it to the user */
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
#if EXIF_USE_MBSTRING
			if (ImageInfo->motorola_intel) {
				*pszInfoPtr = php_mb_convert_encoding(szValuePtr, ByteCount, ImageInfo->encode_jis, ImageInfo->decode_jis_be, &len TSRMLS_CC);
			} else {
				*pszInfoPtr = php_mb_convert_encoding(szValuePtr, ByteCount, ImageInfo->encode_jis, ImageInfo->decode_jis_le, &len TSRMLS_CC);
			}
			return len;
#else
			return exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
#endif
		} else
		if (!memcmp(szValuePtr, "\0\0\0\0\0\0\0\0", 8)) {
			/* 8 NULL means undefined and should be ASCII... */
			*pszEncoding = estrdup("UNDEFINED");
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
		}
	}

	/* Olympus has this padded with trailing spaces.  Remove these first. */
	if (ByteCount>0) {
		for (a=ByteCount-1;a && szValuePtr[a]==' ';a--) {
			(szValuePtr)[a] = '\0';
		}
	}

	/* normal text without encoding */
	exif_process_string(pszInfoPtr, szValuePtr, ByteCount TSRMLS_CC);
	return strlen(*pszInfoPtr);
}
/* }}} */

/* {{{ exif_process_unicode
 * Process unicode field in IFD. */
static int exif_process_unicode(image_info_type *ImageInfo, xp_field_type *xp_field, int tag, char *szValuePtr, int ByteCount TSRMLS_DC)
{
	xp_field->tag = tag;	

	/* Copy the comment */
#if EXIF_USE_MBSTRING
/*  What if MS supports big-endian with XP? */
/*	if (ImageInfo->motorola_intel) {
		xp_field->value = php_mb_convert_encoding(szValuePtr, ByteCount, ImageInfo->encode_unicode, ImageInfo->decode_unicode_be, &xp_field->size TSRMLS_CC);
	} else {
		xp_field->value = php_mb_convert_encoding(szValuePtr, ByteCount, ImageInfo->encode_unicode, ImageInfo->decode_unicode_le, &xp_field->size TSRMLS_CC);
	}*/
	xp_field->value = php_mb_convert_encoding(szValuePtr, ByteCount, ImageInfo->encode_unicode, ImageInfo->decode_unicode_le, &xp_field->size TSRMLS_CC);
	return xp_field->size;
#else
	xp_field->size = exif_process_string_raw(&xp_field->value, szValuePtr, ByteCount);
	return xp_field->size;
#endif
}
/* }}} */

/* {{{ exif_process_IFD_in_MAKERNOTE
 * Process nested IFDs directories in Maker Note. */
static int exif_process_IFD_in_MAKERNOTE(image_info_type *ImageInfo, char * value_ptr, int value_len, char *offset_base, size_t IFDlength, size_t displacement TSRMLS_DC)
{
	int de, i=0, section_index = SECTION_MAKERNOTE;
	int NumDirEntries, old_motorola_intel, offset_diff;
	const maker_note_type *maker_note;
	char *dir_start;

	for (i=0; i<=sizeof(maker_note_array)/sizeof(maker_note_type); i++) {
		if (i==sizeof(maker_note_array)/sizeof(maker_note_type))
			return FALSE;
		maker_note = maker_note_array+i;
		
		/*exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "check (%s,%s)", maker_note->make?maker_note->make:"", maker_note->model?maker_note->model:"");*/
		if (maker_note->make && (!ImageInfo->make || strcmp(maker_note->make, ImageInfo->make)))
			continue;
		if (maker_note->model && (!ImageInfo->model || strcmp(maker_note->model, ImageInfo->model)))
			continue;
		if (maker_note->id_string && strncmp(maker_note->id_string, value_ptr, maker_note->id_string_len))
			continue;
		break;
	}

	dir_start = value_ptr + maker_note->offset;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "process %s @x%04X + 0x%04X=%d: %s", exif_get_sectionname(section_index), (int)dir_start-(int)offset_base+maker_note->offset+displacement, value_len, value_len, exif_char_dump(value_ptr, value_len, (int)dir_start-(int)offset_base+maker_note->offset+displacement));
#endif

	ImageInfo->sections_found |= FOUND_MAKERNOTE;

	old_motorola_intel = ImageInfo->motorola_intel;
	switch (maker_note->byte_order) {
		case MN_ORDER_INTEL:
			ImageInfo->motorola_intel = 0;
			break;
		case MN_ORDER_MOTOROLA:
			ImageInfo->motorola_intel = 1;
			break;
		default:
		case MN_ORDER_NORMAL:
			break;
	}

	NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);

	switch (maker_note->offset_mode) {
		case MN_OFFSET_MAKER:
			offset_base = value_ptr;
			break;
		case MN_OFFSET_GUESS:
			offset_diff = 2 + NumDirEntries*12 + 4 - php_ifd_get32u(dir_start+10, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Using automatic offset correction: 0x%04X", ((int)dir_start-(int)offset_base+maker_note->offset+displacement) + offset_diff);
#endif
			offset_base = value_ptr + offset_diff;
			break;
		default:
		case MN_OFFSET_NORMAL:
			break;
	}

	if ((2+NumDirEntries*12) > value_len) {
		exif_error_docref("exif_read_data#error_ifd" TSRMLS_CC, ImageInfo, E_WARNING, "illegal IFD size: 2 + x%04X*12 = x%04X > x%04X", NumDirEntries, 2+NumDirEntries*12, value_len);
		return FALSE;
	}

	for (de=0;de<NumDirEntries;de++) {
		if (!exif_process_IFD_TAG(ImageInfo, dir_start + 2 + 12 * de,
								  offset_base, IFDlength, displacement, section_index, 0, maker_note->tag_table TSRMLS_CC)) {
			return FALSE;
		}
	}
	ImageInfo->motorola_intel = old_motorola_intel;
/*	NextDirOffset (must be NULL) = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);*/
#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "subsection %s done", exif_get_sectionname(SECTION_MAKERNOTE));
#endif
	return TRUE;
}
/* }}} */

/* {{{ exif_process_IFD_TAG
 * Process one of the nested IFDs directories. */
static int exif_process_IFD_TAG(image_info_type *ImageInfo, char *dir_entry, char *offset_base, size_t IFDlength, size_t displacement, int section_index, int ReadNextIFD, tag_table_type tag_table TSRMLS_DC)
{
	size_t length;
	int tag, format, components;
	char *value_ptr, tagname[64], cbuf[32], *outside=NULL;
	size_t byte_count, offset_val, fpos, fgot;
	xp_field_type *tmp_xp;

	tag = php_ifd_get16u(dir_entry, ImageInfo->motorola_intel);
	format = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
	components = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);

	if (!format || format > NUM_FORMATS) {
		/* (-1) catches illegal zero case as unsigned underflows to positive large. */
		exif_error_docref("exif_read_data#error_ifd" TSRMLS_CC, ImageInfo, E_WARNING, "process tag(x%04X=%s): Illegal format code 0x%04X, suppose BYTE", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), format);
		format = TAG_FMT_BYTE;
		/*return TRUE;*/
	}

	byte_count = components * php_tiff_bytes_per_format[format];

	if (byte_count > 4) {
		offset_val = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
		/* If its bigger than 4 bytes, the dir entry contains an offset. */
		value_ptr = offset_base+offset_val;
		if (offset_val+byte_count > IFDlength || value_ptr < dir_entry) {
			/*
			// It is important to check for IMAGE_FILETYPE_TIFF
			// JPEG does not use absolute pointers instead its pointers are relative to the start
			// of the TIFF header in APP1 section.
			*/
			if (offset_val<0 || offset_val+byte_count>ImageInfo->FileSize || (ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_II && ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_MM)) {
				if (value_ptr < dir_entry) {
					/* we can read this if offset_val > 0 */
					/* some files have their values in other parts of the file */
					exif_error_docref("exif_read_data#error_ifd" TSRMLS_CC, ImageInfo, E_WARNING, "process tag(x%04X=%s): Illegal pointer offset(x%04X < x%04X)", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), offset_val, dir_entry);
				} else {
					/* this is for sure not allowed */
					/* exception are IFD pointers */
					exif_error_docref("exif_read_data#error_ifd" TSRMLS_CC, ImageInfo, E_WARNING, "process tag(x%04X=%s): Illegal pointer offset(x%04X + x%04X = x%04X > x%04X)", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), offset_val, byte_count, offset_val+byte_count, IFDlength);
				}
				return TRUE;
			}
			if (byte_count>sizeof(cbuf)) {
				/* mark as outside range and get buffer */
				value_ptr = emalloc(byte_count);
				if (!value_ptr) {
					EXIF_ERRLOG_EALLOC
					return FALSE;
				}
				outside = value_ptr;
			} else {
				/*
				// in most cases we only access a small range so
				// it is faster to use a static buffer there
				// BUT it offers also the possibility to have
				// pointers read without the need to free them
				// explicitley before returning.
				*/
				value_ptr = cbuf;
			}

			fpos = php_stream_tell(ImageInfo->infile);
			php_stream_seek(ImageInfo->infile, offset_val, SEEK_SET);
			fgot = php_stream_tell(ImageInfo->infile);
			if (fgot!=offset_val) {
				EFREE_IF(outside);
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "wrong file pointer: 0x%08X != 0x08X", fgot, offset_val);
				return FALSE;
			}
			fgot = php_stream_read(ImageInfo->infile, value_ptr, byte_count);
			php_stream_seek(ImageInfo->infile, fpos, SEEK_SET);
			if (fgot<byte_count) {
				EFREE_IF(outside);
				EXIF_ERRLOG_FILEEOF
				return FALSE;
			}
		}
	} else {
		/* 4 bytes or less and value is in the dir entry itself */
		value_ptr = dir_entry+8;
		offset_val= value_ptr-offset_base;
	}

	ImageInfo->sections_found |= FOUND_ANY_TAG;
#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "process tag(x%04X=%s,@x%04X + x%04X(=%d)): %s%s", tag, exif_get_tagname(tag, tagname, -12, tag_table TSRMLS_CC), offset_val+displacement, byte_count, byte_count, (components>1)&&format!=TAG_FMT_UNDEFINED&&format!=TAG_FMT_STRING?"ARRAY OF ":"", format==TAG_FMT_STRING?(value_ptr?value_ptr:"<no data>"):exif_get_tagformat(format));
#endif
	if (section_index==SECTION_THUMBNAIL) {
		if (!ImageInfo->Thumbnail.data) {
			switch(tag) {
				case TAG_IMAGEWIDTH:
				case TAG_COMP_IMAGE_WIDTH:
					ImageInfo->Thumbnail.width = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;
	
				case TAG_IMAGEHEIGHT:
				case TAG_COMP_IMAGE_HEIGHT:
					ImageInfo->Thumbnail.height = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;
	
				case TAG_STRIP_OFFSETS:
				case TAG_JPEG_INTERCHANGE_FORMAT:
					/* accept both formats */
					ImageInfo->Thumbnail.offset = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;
	
				case TAG_STRIP_BYTE_COUNTS:
					if (ImageInfo->FileType == IMAGE_FILETYPE_TIFF_II || ImageInfo->FileType == IMAGE_FILETYPE_TIFF_MM) {
						ImageInfo->Thumbnail.filetype = ImageInfo->FileType;
					} else {
						/* motorola is easier to read */
						ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_TIFF_MM;
					}
					ImageInfo->Thumbnail.size = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					break;
	
				case TAG_JPEG_INTERCHANGE_FORMAT_LEN:
					if (ImageInfo->Thumbnail.filetype == IMAGE_FILETYPE_UNKNOWN) {
						ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_JPEG;
						ImageInfo->Thumbnail.size = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
					}
					break;
			}
		}
	} else {
		if (section_index==SECTION_IFD0 || section_index==SECTION_EXIF)
		switch(tag) {
			case TAG_COPYRIGHT:
				/* check for "<photographer> NUL <editor> NUL" */
				if (byte_count>1 && (length=php_strnlen(value_ptr, byte_count)) > 0) {
					if (length<byte_count-1) {
						/* When there are any characters after the first NUL */
						ImageInfo->CopyrightPhotographer  = estrdup(value_ptr);
						ImageInfo->CopyrightEditor        = estrdup(value_ptr+length+1);
						ImageInfo->Copyright              = emalloc(strlen(value_ptr)+strlen(value_ptr+length+1)+3);
						if (!ImageInfo->Copyright) {
							EXIF_ERRLOG_EALLOC
						} else {
							sprintf(ImageInfo->Copyright, "%s, %s", value_ptr, value_ptr+length+1);
						}
						/* format = TAG_FMT_UNDEFINED; this musn't be ASCII         */
						/* but we are not supposed to change this                   */
						/* keep in mind that image_info does not store editor value */
					} else {
						ImageInfo->Copyright = estrdup(value_ptr);
					}
				}
				break;   

			case TAG_USERCOMMENT:
				ImageInfo->UserCommentLength = exif_process_user_comment(ImageInfo, &(ImageInfo->UserComment), &(ImageInfo->UserCommentEncoding), value_ptr, byte_count TSRMLS_CC);
				break;

			case TAG_XP_TITLE:
			case TAG_XP_COMMENTS:
			case TAG_XP_AUTHOR:
			case TAG_XP_KEYWORDS:
			case TAG_XP_SUBJECT:
				tmp_xp = (xp_field_type*)erealloc(ImageInfo->xp_fields.list, sizeof(xp_field_type)*(ImageInfo->xp_fields.count+1));
				if (!tmp_xp) {
					EXIF_ERRLOG_EALLOC
				} else {
					ImageInfo->sections_found |= FOUND_WINXP;
					ImageInfo->xp_fields.list = tmp_xp;
					ImageInfo->xp_fields.count++;
					exif_process_unicode(ImageInfo, &(ImageInfo->xp_fields.list[ImageInfo->xp_fields.count-1]), tag, value_ptr, byte_count TSRMLS_CC);
				}
				break;

			case TAG_FNUMBER:
				/* Simplest way of expressing aperture, so I trust it the most.
				   (overwrite previously computd value if there is one) */
				ImageInfo->ApertureFNumber = (float)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_APERTURE:
			case TAG_MAX_APERTURE:
				/* More relevant info always comes earlier, so only use this field if we don't
				   have appropriate aperture information yet. */
				if (ImageInfo->ApertureFNumber == 0) {
					ImageInfo->ApertureFNumber
						= (float)exp(exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC)*log(2)*0.5);
				}
				break;

			case TAG_SHUTTERSPEED:
				/* More complicated way of expressing exposure time, so only use
				   this value if we don't already have it from somewhere else.
				   SHUTTERSPEED comes after EXPOSURE TIME
				  */
				if (ImageInfo->ExposureTime == 0) {
					ImageInfo->ExposureTime
						= (float)(1/exp(exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC)*log(2)));
				}
				break;
			case TAG_EXPOSURETIME:
				ImageInfo->ExposureTime = -1;
				break;

			case TAG_COMP_IMAGE_WIDTH:
				ImageInfo->ExifImageWidth = exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_FOCALPLANE_X_RES:
				ImageInfo->FocalplaneXRes = exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_SUBJECT_DISTANCE:
				/* Inidcates the distacne the autofocus camera is focused to.
				   Tends to be less accurate as distance increases. */
				ImageInfo->Distance = (float)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC);
				break;

			case TAG_FOCALPLANE_RESOLUTION_UNIT:
				switch((int)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel TSRMLS_CC)) {
					case 1: ImageInfo->FocalplaneUnits = 25.4; break; /* inch */
					case 2:
						/* According to the information I was using, 2 measn meters.
						   But looking at the Cannon powershot's files, inches is the only
						   sensible value. */
						ImageInfo->FocalplaneUnits = 25.4;
						break;

					case 3: ImageInfo->FocalplaneUnits = 10;   break;  /* centimeter */
					case 4: ImageInfo->FocalplaneUnits = 1;    break;  /* milimeter  */
					case 5: ImageInfo->FocalplaneUnits = .001; break;  /* micrometer */
				}
				break;

			case TAG_SUB_IFD:
				if (format==TAG_FMT_IFD) {
					/* If this is called we are either in a TIFFs thumbnail or a JPEG where we cannot handle it */
					/* TIFF thumbnail: our data structure cannot store a thumbnail of a thumbnail */
					/* JPEG do we have the data area and what to do with it */
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "skip SUB IFD");
				}
				break;

			case TAG_MAKE:
				ImageInfo->make = estrdup(value_ptr);
				break;
			case TAG_MODEL:
				ImageInfo->model = estrdup(value_ptr);
				break;

			case TAG_MAKER_NOTE:
				exif_process_IFD_in_MAKERNOTE(ImageInfo, value_ptr, byte_count, offset_base, IFDlength, displacement TSRMLS_CC);
				break;

			case TAG_EXIF_IFD_POINTER:
			case TAG_GPS_IFD_POINTER:
			case TAG_INTEROP_IFD_POINTER:
				if (ReadNextIFD) {
					char *Subdir_start;
					int sub_section_index = 0;
					switch(tag) {
						case TAG_EXIF_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "found EXIF");
#endif
							ImageInfo->sections_found |= FOUND_EXIF;
							sub_section_index = SECTION_EXIF;
							break;
						case TAG_GPS_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "found GPS");
#endif
							ImageInfo->sections_found |= FOUND_GPS;
							sub_section_index = SECTION_GPS;
							break;
						case TAG_INTEROP_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "found INTEROPERABILITY");
#endif
							ImageInfo->sections_found |= FOUND_INTEROP;
							sub_section_index = SECTION_INTEROP;
							break;
					}
					Subdir_start = offset_base + php_ifd_get32u(value_ptr, ImageInfo->motorola_intel);
					if (Subdir_start < offset_base || Subdir_start > offset_base+IFDlength) {
						exif_error_docref("exif_read_data#error_ifd" TSRMLS_CC, ImageInfo, E_WARNING, "Illegal IFD Pointer");
						return FALSE;
					}
					exif_process_IFD_in_JPEG(ImageInfo, Subdir_start, offset_base, IFDlength, displacement, sub_section_index TSRMLS_CC);
#ifdef EXIF_DEBUG
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "subsection %s done", exif_get_sectionname(sub_section_index));
#endif
				}
		}
	}
	exif_iif_add_tag(ImageInfo, section_index, exif_get_tagname(tag, tagname, sizeof(tagname), tag_table TSRMLS_CC), tag, format, components, value_ptr TSRMLS_CC);
	EFREE_IF(outside);
	return TRUE;
}
/* }}} */

/* {{{ exif_process_IFD_in_JPEG
 * Process one of the nested IFDs directories. */
static int exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, char *offset_base, size_t IFDlength, size_t displacement, int section_index TSRMLS_DC)
{
	int de;
	int NumDirEntries;
	int NextDirOffset;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "process %s (x%04X(=%d))", exif_get_sectionname(section_index), IFDlength, IFDlength);
#endif

	ImageInfo->sections_found |= FOUND_IFD0;

	NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);

	if ((dir_start+2+NumDirEntries*12) > (offset_base+IFDlength)) {
		exif_error_docref("exif_read_data#error_ifd" TSRMLS_CC, ImageInfo, E_WARNING, "illegal IFD size: x%04X + 2 + x%04X*12 = x%04X > x%04X", (int)dir_start+2-(int)offset_base, NumDirEntries, (int)dir_start+2+NumDirEntries*12-(int)offset_base, IFDlength);
		return FALSE;
	}

	for (de=0;de<NumDirEntries;de++) {
		if (!exif_process_IFD_TAG(ImageInfo, dir_start + 2 + 12 * de,
								  offset_base, IFDlength, displacement, section_index, 1, exif_get_tag_table(section_index) TSRMLS_CC)) {
			return FALSE;
		}
	}
	/*
	 * Hack to make it process IDF1 I hope
	 * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
	 */
	NextDirOffset = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);
	if (NextDirOffset) {
		/* the next line seems false but here IFDlength means length of all IFDs */
		if (offset_base + NextDirOffset < offset_base || offset_base + NextDirOffset > offset_base+IFDlength) {
			exif_error_docref("exif_read_data#error_ifd" TSRMLS_CC, ImageInfo, E_WARNING, "illegal IFD offset");
			return FALSE;
		}
		/* That is the IFD for the first thumbnail */
#ifdef EXIF_DEBUG
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "expect next IFD to be thumbnail");
#endif
		if (exif_process_IFD_in_JPEG(ImageInfo, offset_base + NextDirOffset, offset_base, IFDlength, displacement, SECTION_THUMBNAIL TSRMLS_CC)) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail size: 0x%04X", ImageInfo->Thumbnail.size);
#endif
			if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN
			&&  ImageInfo->Thumbnail.size
			&&  ImageInfo->Thumbnail.offset
			&&  ImageInfo->read_thumbnail
			) {
				exif_thumbnail_extract(ImageInfo, offset_base, IFDlength TSRMLS_CC);
			}
			return TRUE;
		} else {
			return FALSE;
		}
	}
	return TRUE;
}
/* }}} */

/* {{{ exif_process_TIFF_in_JPEG
   Process a TIFF header in a JPEG file
*/
static void exif_process_TIFF_in_JPEG(image_info_type *ImageInfo, char *CharBuf, size_t length, size_t displacement TSRMLS_DC)
{
	unsigned exif_value_2a, offset_of_ifd;

	/* set the thumbnail stuff to nothing so we can test to see if they get set up */
	if (memcmp(CharBuf, "II", 2) == 0) {
		ImageInfo->motorola_intel = 0;
	} else if (memcmp(CharBuf, "MM", 2) == 0) {
		ImageInfo->motorola_intel = 1;
	} else {
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "invalid TIFF alignment marker");
		return;
	}

	/* Check the next two values for correctness. */
	exif_value_2a = php_ifd_get16u(CharBuf+2, ImageInfo->motorola_intel);
	offset_of_ifd = php_ifd_get32u(CharBuf+4, ImageInfo->motorola_intel);
	if ( exif_value_2a != 0x2a || offset_of_ifd < 0x08) {
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "invalid TIFF start (1)");
		return;
	}

	ImageInfo->sections_found |= FOUND_IFD0;
	/* First directory starts at offset 8. Offsets starts at 0. */
	exif_process_IFD_in_JPEG(ImageInfo, CharBuf+offset_of_ifd, CharBuf, length/*-14*/, displacement, SECTION_IFD0 TSRMLS_CC);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "process TIFF in JPEG done");
#endif

	/* Compute the CCD width, in milimeters. */
	if (ImageInfo->FocalplaneXRes != 0) {
		ImageInfo->CCDWidth = (float)(ImageInfo->ExifImageWidth * ImageInfo->FocalplaneUnits / ImageInfo->FocalplaneXRes);
	}
}
/* }}} */

/* {{{ exif_process_APP1
   Process an JPEG APP1 block marker
   Describes all the drivel that most digital cameras include...
*/
static void exif_process_APP1(image_info_type *ImageInfo, char *CharBuf, size_t length, size_t displacement TSRMLS_DC)
{
	/* Check the APP1 for Exif Identifier Code */
	static const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
	if (memcmp(CharBuf+2, ExifHeader, 6)) {
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "incorrect APP1 Exif Identifier Code");
		return;
	}
	exif_process_TIFF_in_JPEG(ImageInfo, CharBuf + 8, length - 8, displacement+8 TSRMLS_CC);
#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "process APP1/EXIF done");
#endif
}
/* }}} */

/* {{{ exif_process_APP12
   Process an JPEG APP12 block marker used by OLYMPUS
*/
static void exif_process_APP12(image_info_type *ImageInfo, char *buffer, size_t length TSRMLS_DC)
{
	size_t l1, l2=0;

	if ((l1 = php_strnlen(buffer+2, length-2)) > 0) {
		exif_iif_add_tag(ImageInfo, SECTION_APP12, "Company", TAG_NONE, TAG_FMT_STRING, l1, buffer+2 TSRMLS_CC);
		if (length > 2+l1+1) {
			l2 = php_strnlen(buffer+2+l1+1, length-2-l1+1);
			exif_iif_add_tag(ImageInfo, SECTION_APP12, "Info", TAG_NONE, TAG_FMT_STRING, l2, buffer+2+l1+1 TSRMLS_CC);
		}
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "process section APP12 with l1=%d, l2=%d done", l1, l2);
#endif
}
/* }}} */

/* {{{ exif_scan_JPEG_header
 * Parse the marker stream until SOS or EOI is seen; */
static int exif_scan_JPEG_header(image_info_type *ImageInfo TSRMLS_DC)
{
	int section, sn;
	int marker = 0, last_marker = M_PSEUDO, comment_correction=1;
	int ll, lh;
	uchar *Data;
	size_t fpos, size, got, itemlen;
	jpeg_sof_info  sof_info;

	for(section=0;;section++) {
#ifdef EXIF_DEBUG
		fpos = php_stream_tell(ImageInfo->infile);
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "needing section %d @ 0x%08X", ImageInfo->file.count, fpos);
#endif

		/* get marker byte, swallowing possible padding                           */
		/* some software does not count the length bytes of COM section           */
		/* one company doing so is very much envolved in JPEG... so we accept too */
		if (last_marker==M_COM && comment_correction) {
			comment_correction = 2;
		}
		do {
			if ((marker = php_stream_getc(ImageInfo->infile)) == EOF) {
				EXIF_ERRLOG_CORRUPT
				return FALSE;
			}
			if (last_marker==M_COM && comment_correction>0) {
				if (marker!=0xFF) {
					marker = 0xff;
					comment_correction--;
				} else  {
					last_marker = M_PSEUDO; /* stop skipping 0 for M_COM */
				}
			}
		} while (marker == 0xff);
		if (last_marker==M_COM && !comment_correction) {
			exif_error_docref("exif_read_data#error_mcom" TSRMLS_CC, ImageInfo, E_NOTICE, "Image has corrupt M_COM section: some software set wrong length information");
		}
		if (last_marker==M_COM && comment_correction)
			return M_EOI; /* ah illegal: char after COM section not 0xFF */

		fpos = php_stream_tell(ImageInfo->infile);

		if (marker == 0xff) {
			/* 0xff is legal padding, but if we get that many, something's wrong. */
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "too many padding bytes!");
			return FALSE;
		}

		/* Read the length of the section. */
		lh = php_stream_getc(ImageInfo->infile);
		ll = php_stream_getc(ImageInfo->infile);

		itemlen = (lh << 8) | ll;

		if (itemlen < 2) {
			EXIF_ERRLOG_CORRUPT
			return FALSE;
		}

		if ((sn=exif_file_sections_add(ImageInfo, marker, itemlen+1, NULL))==-1) {
			EXIF_ERRLOG_EALLOC
			return FALSE;
		}
		Data = ImageInfo->file.list[sn].data;

		/* Store first two pre-read bytes. */
		Data[0] = (uchar)lh;
		Data[1] = (uchar)ll;

		got = php_stream_read(ImageInfo->infile, Data+2, itemlen-2); /* Read the whole section. */
		if (got != itemlen-2) {
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "error reading from file: got=x%04X(=%d) != itemlen-2=x%04X(=%d)", got, got, itemlen-2, itemlen-2);
			return FALSE;
		}

#ifdef EXIF_DEBUG
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "process section(x%02X=%s) @ x%04X + x%04X(=%d)", marker, exif_get_markername(marker), fpos, itemlen, itemlen);
#endif
		switch(marker) {
			case M_SOS:   /* stop before hitting compressed data  */
				/* If reading entire image is requested, read the rest of the data. */
				if (ImageInfo->read_all) {
					/* Determine how much file is left. */
					fpos = php_stream_tell(ImageInfo->infile);
					size = ImageInfo->FileSize - fpos;
					if ((sn=exif_file_sections_add(ImageInfo, M_PSEUDO, size, NULL))==-1) {
						EXIF_ERRLOG_EALLOC
						return FALSE;
					}
					Data = ImageInfo->file.list[sn].data;
					got = php_stream_read(ImageInfo->infile, Data, size);
					if (got != size) {
						EXIF_ERRLOG_FILEEOF
						return FALSE;
					}
				}
				return TRUE;

			case M_EOI:   /* in case it's a tables-only JPEG stream */
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "no image in jpeg!");
				return (ImageInfo->sections_found&(~FOUND_COMPUTED)) ? TRUE : FALSE;

			case M_COM: /* Comment section */
				exif_process_COM(ImageInfo, (char *)Data, itemlen TSRMLS_CC);
				break;

			case M_EXIF:
				if (!(ImageInfo->sections_found&FOUND_IFD0)) {
					/*ImageInfo->sections_found |= FOUND_EXIF;*/
					/* Seen files from some 'U-lead' software with Vivitar scanner
					   that uses marker 31 later in the file (no clue what for!) */
					exif_process_APP1(ImageInfo, (char *)Data, itemlen, fpos TSRMLS_CC);
				}
				break;

			case M_APP12:
				exif_process_APP12(ImageInfo, (char *)Data, itemlen TSRMLS_CC);
				break;


			case M_SOF0:
			case M_SOF1:
			case M_SOF2:
			case M_SOF3:
			case M_SOF5:
			case M_SOF6:
			case M_SOF7:
			case M_SOF9:
			case M_SOF10:
			case M_SOF11:
			case M_SOF13:
			case M_SOF14:
			case M_SOF15:
				exif_process_SOFn(Data, marker, &sof_info);
				ImageInfo->Width  = sof_info.width;
				ImageInfo->Height = sof_info.height;
				if (sof_info.num_components == 3) {
					ImageInfo->IsColor = 1;
				} else {
					ImageInfo->IsColor = 0;
				}
				break;
			default:
				/* skip any other marker silently. */
				break;
		}

		/* keep track of last marker */
		last_marker = marker;
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "done");
#endif
	return TRUE;
}
/* }}} */

/* {{{ exif_scan_thumbnail
 * scan JPEG in thumbnail (memory) */
static int exif_scan_thumbnail(image_info_type *ImageInfo TSRMLS_DC)
{
	uchar           c, *data = ImageInfo->Thumbnail.data;
	int             n, marker;
	size_t          length=2, pos=0;
	jpeg_sof_info   sof_info;

	if (!data) {
		return FALSE; /* nothing to do here */
	}
	if (memcmp(data, "\xFF\xD8\xFF", 3)) {
		if (!ImageInfo->Thumbnail.width && !ImageInfo->Thumbnail.height) {
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "Thumbnail is not a JPEG image");
		}
		return FALSE;
	}
	for (;;) {
		pos += length;
		if (pos>=ImageInfo->Thumbnail.size) 
			return FALSE;
		c = data[pos++];
		if (pos>=ImageInfo->Thumbnail.size) 
			return FALSE;
		if (c != 0xFF) {
			return FALSE;
		}
		n = 8;
		while ((c = data[pos++]) == 0xFF && n--) {
			if (pos+3>=ImageInfo->Thumbnail.size) 
				return FALSE;
			/* +3 = pos++ of next check when reaching marker + 2 bytes for length */
		}
		if (c == 0xFF) 
			return FALSE;
		marker = c;
		length = php_jpg_get16(data+pos);
		if (pos+length>=ImageInfo->Thumbnail.size) {
			return FALSE;
		}
#ifdef EXIF_DEBUG
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail: process section(x%02X=%s) @ x%04X + x%04X", marker, exif_get_markername(marker), pos, length);
#endif
		switch (marker) {
			case M_SOF0:
			case M_SOF1:
			case M_SOF2:
			case M_SOF3:
			case M_SOF5:
			case M_SOF6:
			case M_SOF7:
			case M_SOF9:
			case M_SOF10:
			case M_SOF11:
			case M_SOF13:
			case M_SOF14:
			case M_SOF15:
				/* handle SOFn block */
				exif_process_SOFn(data+pos, marker, &sof_info);
				ImageInfo->Thumbnail.height   = sof_info.height;
				ImageInfo->Thumbnail.width    = sof_info.width;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Thumbnail: size: %d * %d", sof_info.width, sof_info.height);
#endif
				return TRUE;

			case M_SOS:
			case M_EOI:
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "could not compute size of thumbnail");
				return FALSE;
				break;

			default:
				/* just skip */
				break;
		}
	}

	exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "could not compute size of thumbnail");
	return FALSE;
}
/* }}} */

/* {{{ exif_process_IFD_in_TIFF
 * Parse the TIFF header; */
static int exif_process_IFD_in_TIFF(image_info_type *ImageInfo, size_t dir_offset, int section_index TSRMLS_DC)
{
	int i, sn, num_entries, sub_section_index = 0;
	unsigned char *dir_entry;
	char tagname[64];
	size_t ifd_size, dir_size, entry_offset, next_offset, entry_length, entry_value=0, fgot;
	int entry_tag , entry_type;
	tag_table_type tag_table = exif_get_tag_table(section_index);

	if (ImageInfo->FileSize >= dir_offset+2) {
		if ((sn=exif_file_sections_add(ImageInfo, M_PSEUDO, 2, NULL))==-1) {
			EXIF_ERRLOG_EALLOC
			return FALSE;
		}
#ifdef EXIF_DEBUG
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, 2);
#endif
		php_stream_seek(ImageInfo->infile, dir_offset, SEEK_SET); /* we do not know the order of sections */
		php_stream_read(ImageInfo->infile, ImageInfo->file.list[sn].data, 2);
		num_entries = php_ifd_get16u(ImageInfo->file.list[sn].data, ImageInfo->motorola_intel);
		dir_size = 2/*num dir entries*/ +12/*length of entry*/*num_entries +4/* offset to next ifd (points to thumbnail or NULL)*/;
		if (ImageInfo->FileSize >= dir_offset+dir_size) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X), IFD entries(%d)", ImageInfo->FileSize, dir_offset+2, dir_size-2, num_entries);
#endif
			if (exif_file_sections_realloc(ImageInfo, sn, dir_size TSRMLS_CC)) {
				return FALSE;
			}
			php_stream_read(ImageInfo->infile, ImageInfo->file.list[sn].data+2, dir_size-2);
			/*exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "Dump: %s", exif_char_dump(ImageInfo->file.list[sn].data, dir_size, 0));*/
			next_offset = php_ifd_get32u(ImageInfo->file.list[sn].data + dir_size - 4, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read from TIFF done, next offset x%04X", next_offset);
#endif
			/* now we have the directory we can look how long it should be */
			ifd_size = dir_size;
			for(i=0;i<num_entries;i++) {
				dir_entry 	 = ImageInfo->file.list[sn].data+2+i*12;
				entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
				entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
				if (entry_type > NUM_FORMATS) {
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read from TIFF: tag(0x%04X,%12s): Illegal format code 0x%04X, switching to BYTE", entry_tag, exif_get_tagname(entry_tag, tagname, -12, tag_table TSRMLS_CC), entry_type);
					/* Since this is repeated in exif_process_IFD_TAG make it a notice here */
					/* and make it a warning in the exif_process_IFD_TAG which is called    */
					/* elsewhere. */
					entry_type = TAG_FMT_BYTE;
					/*The next line would break the image on writeback: */
					/* php_ifd_set16u(dir_entry+2, entry_type, ImageInfo->motorola_intel);*/
				}
				entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel) * php_tiff_bytes_per_format[entry_type];
				if (entry_length <= 4) {
					switch(entry_type) {
						case TAG_FMT_USHORT:
							entry_value  = php_ifd_get16u(dir_entry+8, ImageInfo->motorola_intel);
							break;
						case TAG_FMT_SSHORT:
							entry_value  = php_ifd_get16s(dir_entry+8, ImageInfo->motorola_intel);
							break;
						case TAG_FMT_ULONG:
							entry_value  = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
							break;
						case TAG_FMT_SLONG:
							entry_value  = php_ifd_get32s(dir_entry+8, ImageInfo->motorola_intel);
							break;
					}
					switch(entry_tag) {
						case TAG_IMAGEWIDTH:
						case TAG_COMP_IMAGE_WIDTH:
							ImageInfo->Width  = entry_value;
							break;
						case TAG_IMAGEHEIGHT:
						case TAG_COMP_IMAGE_HEIGHT:
							ImageInfo->Height = entry_value;
							break;
						case TAG_PHOTOMETRIC_INTERPRETATION:
							switch (entry_value) {
								case PMI_BLACK_IS_ZERO:
								case PMI_WHITE_IS_ZERO:
								case PMI_TRANSPARENCY_MASK:
									ImageInfo->IsColor = 0;
									break;
								case PMI_RGB:
								case PMI_PALETTE_COLOR:
								case PMI_SEPARATED:
								case PMI_YCBCR:
								case PMI_CIELAB:
									ImageInfo->IsColor = 1;
									break;
							}
							break;
					}
				} else {
					entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
					/* if entry needs expading ifd cache and entry is at end of current ifd cache. */
					/* otherwise there may be huge holes between two entries */
					if (entry_offset + entry_length > dir_offset + ifd_size
					  && entry_offset == dir_offset + ifd_size) {
						ifd_size = entry_offset + entry_length - dir_offset;
#ifdef EXIF_DEBUG
						exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "resize struct: x%04X + x%04X - x%04X = x%04X", entry_offset, entry_length, dir_offset, ifd_size);
#endif
					}
				}
			}
			if (ImageInfo->FileSize >= dir_offset + ImageInfo->file.list[sn].size) {
				if (ifd_size > dir_size) {
					if (dir_offset + ifd_size > ImageInfo->FileSize) {
						exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "error in TIFF: filesize(x%04X) less than size of IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
						return FALSE;
					}
					if (exif_file_sections_realloc(ImageInfo, sn, ifd_size TSRMLS_CC)) {
						return FALSE;
					}
					/* read values not stored in directory itself */
#ifdef EXIF_DEBUG
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read from TIFF: filesize(x%04X), IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
#endif
					php_stream_read(ImageInfo->infile, ImageInfo->file.list[sn].data+dir_size, ifd_size-dir_size);
#ifdef EXIF_DEBUG
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read from TIFF, done");
#endif
				}
				/* now process the tags */
				for(i=0;i<num_entries;i++) {
					dir_entry 	 = ImageInfo->file.list[sn].data+2+i*12;
					entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
					entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
					/*entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);*/
					if (entry_tag == TAG_EXIF_IFD_POINTER ||
						entry_tag == TAG_INTEROP_IFD_POINTER ||
						entry_tag == TAG_GPS_IFD_POINTER ||
						entry_tag == TAG_SUB_IFD
					) {
						switch(entry_tag) {
							case TAG_EXIF_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_EXIF;
								sub_section_index = SECTION_EXIF;
								break;
							case TAG_GPS_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_GPS;
								sub_section_index = SECTION_GPS;
								break;
							case TAG_INTEROP_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_INTEROP;
								sub_section_index = SECTION_INTEROP;
								break;
							case TAG_SUB_IFD:
								ImageInfo->sections_found |= FOUND_THUMBNAIL;
								sub_section_index = SECTION_THUMBNAIL;
								break;
						}
						entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
						exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "next IFD: %s @x%04X", exif_get_sectionname(sub_section_index), entry_offset);
#endif
						exif_process_IFD_in_TIFF(ImageInfo, entry_offset, sub_section_index TSRMLS_CC);
						if (section_index!=SECTION_THUMBNAIL && entry_tag==TAG_SUB_IFD) {
							if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN
							&&  ImageInfo->Thumbnail.size
							&&  ImageInfo->Thumbnail.offset
							&&  ImageInfo->read_thumbnail
							) {
#ifdef EXIF_DEBUG
								exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "%s THUMBNAIL @0x%04X + 0x%04X", ImageInfo->Thumbnail.data ? "ignore" : "read", ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
#endif
								if (!ImageInfo->Thumbnail.data) {
									ImageInfo->Thumbnail.data = emalloc(ImageInfo->Thumbnail.size);
									if (!ImageInfo->Thumbnail.data) {
										EXIF_ERRLOG_EALLOC
									} else {
										php_stream_seek(ImageInfo->infile, ImageInfo->Thumbnail.offset, SEEK_SET);
										fgot = php_stream_read(ImageInfo->infile, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
										if (fgot < ImageInfo->Thumbnail.size) {
											EXIF_ERRLOG_THUMBEOF
										}
										exif_thumbnail_build(ImageInfo TSRMLS_CC);
									}
								}
							}
						}
#ifdef EXIF_DEBUG
						exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "next IFD: %s done", exif_get_sectionname(sub_section_index));
#endif
					} else {
						if (!exif_process_IFD_TAG(ImageInfo, dir_entry,
												  ImageInfo->file.list[sn].data-dir_offset,
												  ifd_size, 0, section_index, 0, tag_table TSRMLS_CC)) {
							return FALSE;
						}
					}
				}
				/* If we had a thumbnail in a SUB_IFD we have ANOTHER image in NEXT IFD */
				if (next_offset && section_index != SECTION_THUMBNAIL) {
					/* this should be a thumbnail IFD */
					/* the thumbnail itself is stored at Tag=StripOffsets */
#ifdef EXIF_DEBUG
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read next IFD (THUMBNAIL) at x%04X", next_offset);
#endif
					exif_process_IFD_in_TIFF(ImageInfo, next_offset, SECTION_THUMBNAIL TSRMLS_CC);
#ifdef EXIF_DEBUG
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "%s THUMBNAIL @0x%04X + 0x%04X", ImageInfo->Thumbnail.data ? "ignore" : "read", ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
#endif
					if (!ImageInfo->Thumbnail.data && ImageInfo->Thumbnail.offset && ImageInfo->Thumbnail.size && ImageInfo->read_thumbnail) {
						ImageInfo->Thumbnail.data = emalloc(ImageInfo->Thumbnail.size);
						if (!ImageInfo->Thumbnail.data) {
							EXIF_ERRLOG_EALLOC
						} else {
							php_stream_seek(ImageInfo->infile, ImageInfo->Thumbnail.offset, SEEK_SET);
							fgot = php_stream_read(ImageInfo->infile, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
							if (fgot < ImageInfo->Thumbnail.size) {
								EXIF_ERRLOG_THUMBEOF
							}
							exif_thumbnail_build(ImageInfo TSRMLS_CC);
						}
					}
#ifdef EXIF_DEBUG
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "read next IFD (THUMBNAIL) done");
#endif
				}
				return TRUE;
			} else {
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "error in TIFF: filesize(x%04X) less than size of IFD(x%04X)", ImageInfo->FileSize, dir_offset+ImageInfo->file.list[sn].size);
				return FALSE;
			}
		} else {
			exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "error in TIFF: filesize(x%04X) less than size of IFD dir(x%04X)", ImageInfo->FileSize, dir_offset+dir_size);
			return FALSE;
		}
	} else {
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "error in TIFF: filesize(x%04X) less than start of IFD dir(x%04X)", ImageInfo->FileSize, dir_offset+2);
		return FALSE;
	}
}
/* }}} */

/* {{{ exif_scan_FILE_header
 * Parse the marker stream until SOS or EOI is seen; */
static int exif_scan_FILE_header(image_info_type *ImageInfo TSRMLS_DC)
{
	unsigned char file_header[8];
	int ret = FALSE;

	ImageInfo->FileType = IMAGE_FILETYPE_UNKNOWN;

	if (ImageInfo->FileSize >= 2) {
		php_stream_seek(ImageInfo->infile, 0, SEEK_SET);
		php_stream_read(ImageInfo->infile, file_header, 2);
		if ((file_header[0]==0xff) && (file_header[1]==M_SOI)) {
			ImageInfo->FileType = IMAGE_FILETYPE_JPEG;
			if (exif_scan_JPEG_header(ImageInfo TSRMLS_CC)) {
				ret = TRUE;
			} else {
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "invalid JPEG file");
			}
		} else if (ImageInfo->FileSize >= 8) {
			php_stream_read(ImageInfo->infile, file_header+2, 6);
			if (!memcmp(file_header, "II\x2A\x00", 4)) {
				ImageInfo->FileType = IMAGE_FILETYPE_TIFF_II;
				ImageInfo->motorola_intel = 0;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "file has TIFF/II format");
#endif
				ImageInfo->sections_found |= FOUND_IFD0;
				if (exif_process_IFD_in_TIFF(ImageInfo, 
											 php_ifd_get32u(file_header + 4, ImageInfo->motorola_intel),
											 SECTION_IFD0 TSRMLS_CC)) {
					ret = TRUE;
				} else {
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "invalid TIFF file");
				}
			}
			else
			if (!memcmp(file_header, "MM\x00\x2a", 4)) {
				ImageInfo->FileType = IMAGE_FILETYPE_TIFF_MM;
				ImageInfo->motorola_intel = 1;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "file has TIFF/MM format");
#endif
				ImageInfo->sections_found |= FOUND_IFD0;
				if (exif_process_IFD_in_TIFF(ImageInfo,
											 php_ifd_get32u(file_header + 4, ImageInfo->motorola_intel),
											 SECTION_IFD0 TSRMLS_CC)) {
					ret = TRUE;
				} else {
					exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "invalid TIFF file");
				}
			} else {
				exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "file not supported");
				return FALSE;
			}
		}
	} else {
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "file to small (%d)", ImageInfo->FileSize);
	}
	return ret;
}
/* }}} */

/* {{{ exif_discard_imageinfo
   Discard data scanned by exif_read_file.
*/
static int exif_discard_imageinfo(image_info_type *ImageInfo)
{
	int i;

	EFREE_IF(ImageInfo->FileName);
	EFREE_IF(ImageInfo->UserComment);
	EFREE_IF(ImageInfo->UserCommentEncoding);
	EFREE_IF(ImageInfo->Copyright);
	EFREE_IF(ImageInfo->CopyrightPhotographer);
	EFREE_IF(ImageInfo->CopyrightEditor);
	EFREE_IF(ImageInfo->Thumbnail.data);
	EFREE_IF(ImageInfo->encode_unicode);
	EFREE_IF(ImageInfo->decode_unicode_be);
	EFREE_IF(ImageInfo->decode_unicode_le);
	EFREE_IF(ImageInfo->encode_jis);
	EFREE_IF(ImageInfo->decode_jis_be);
	EFREE_IF(ImageInfo->decode_jis_le);
	EFREE_IF(ImageInfo->make);
	EFREE_IF(ImageInfo->model);
	for (i=0; i<ImageInfo->xp_fields.count; i++) {
		EFREE_IF(ImageInfo->xp_fields.list[i].value);
	}
	EFREE_IF(ImageInfo->xp_fields.list);
	for (i=0; i<SECTION_COUNT; i++) {
		exif_iif_free(ImageInfo, i);
	}
	exif_file_sections_free(ImageInfo);
	memset(ImageInfo, 0, sizeof(*ImageInfo));
	return TRUE;
}
/* }}} */

/* {{{ exif_read_file
 */
static int exif_read_file(image_info_type *ImageInfo, char *FileName, int read_thumbnail, int read_all TSRMLS_DC)
{
	int ret;
	struct stat st;

	/* Start with an empty image information structure. */
	memset(ImageInfo, 0, sizeof(*ImageInfo));

	ImageInfo->motorola_intel = -1; /* flag as unknown */

	ImageInfo->infile = php_stream_open_wrapper(FileName, "rb", STREAM_MUST_SEEK|IGNORE_PATH|ENFORCE_SAFE_MODE, NULL);
	if (!ImageInfo->infile) {
		exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_WARNING, "unable to open file");
		return FALSE;
	}

	ImageInfo->FileName = php_basename(FileName, strlen(FileName), NULL, 0);
	ImageInfo->read_thumbnail = read_thumbnail;
	ImageInfo->read_all = read_all;
	ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_UNKNOWN;

	ImageInfo->encode_unicode    = estrdup(EXIF_G(encode_unicode));
	ImageInfo->decode_unicode_be = estrdup(EXIF_G(decode_unicode_be));
	ImageInfo->decode_unicode_le = estrdup(EXIF_G(decode_unicode_le));
	ImageInfo->encode_jis        = estrdup(EXIF_G(encode_jis));
	ImageInfo->decode_jis_be     = estrdup(EXIF_G(decode_jis_be));
	ImageInfo->decode_jis_le     = estrdup(EXIF_G(decode_jis_le));

	if (php_stream_is(ImageInfo->infile, PHP_STREAM_IS_STDIO)) {
		if (VCWD_STAT(FileName, &st) >= 0) {
			/* Store file date/time. */
			ImageInfo->FileDateTime = st.st_mtime;
			ImageInfo->FileSize = st.st_size;
			/*exif_error_docref(NULL TSRMLS_CC, ImageInfo, E_NOTICE, "open stream is file: %d", ImageInfo->FileSize);*/
		}
	} else {
		if (!ImageInfo->FileSize) {
			php_stream_seek(ImageInfo->infile, 0, SEEK_END);
			ImageInfo->FileSize = php_stream_tell(ImageInfo->infile);
			php_stream_seek(ImageInfo->infile, 0, SEEK_SET);
		}
	}

	/* Scan the JPEG headers. */
	ret = exif_scan_FILE_header(ImageInfo TSRMLS_CC);

	php_stream_close(ImageInfo->infile);
	return ret;
}
/* }}} */

/* {{{ proto array|false exif_read_data(string filename [, sections_needed [, sub_arrays[, read_thumbnail]]])
   Reads header data from the JPEG/TIFF image filename and optionally reads the internal thumbnails */
PHP_FUNCTION(exif_read_data)
{
	pval **p_name, **p_sections_needed, **p_sub_arrays, **p_read_thumbnail, **p_read_all;
	int i, ac = ZEND_NUM_ARGS(), ret, sections_needed=0, sub_arrays=0, read_thumbnail=0, read_all=0;
	image_info_type ImageInfo;
	char tmp[64], *sections_str, *s;

	memset(&ImageInfo, 0, sizeof(ImageInfo));

	if ((ac < 1 || ac > 4) || zend_get_parameters_ex(ac, &p_name, &p_sections_needed, &p_sub_arrays, &p_read_thumbnail, &p_read_all) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(p_name);

	if(ac >= 2) {
		convert_to_string_ex(p_sections_needed);
		sections_str = emalloc(strlen(Z_STRVAL_PP(p_sections_needed))+3);
		if (!sections_str) {
			EXIF_ERRLOG_EALLOC
			RETURN_FALSE;
		}
		sprintf(sections_str, ",%s,", Z_STRVAL_PP(p_sections_needed));
		/* sections_str DOES start with , and SPACES are NOT allowed in names */
		s = sections_str;
		while(*++s) {
			if(*s==' ') {
				*s = ',';
			}
		}
		for (i=0; i<SECTION_COUNT; i++) {
			sprintf(tmp, ",%s,", exif_get_sectionname(i));
			if (strstr(sections_str, tmp)) {
				sections_needed |= 1<<i;
			}
		}
		EFREE_IF(sections_str);
		/* now see what we need */
#ifdef EXIF_DEBUG
		sections_str = exif_get_sectionlist(sections_needed TSRMLS_CC);
		if (!sections_str) {
			RETURN_FALSE;
		}
		exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "sections needed: %s", sections_str[0] ? sections_str : "None");
		EFREE_IF(sections_str);
#endif
	}
	if(ac >= 3) {
		convert_to_long_ex(p_sub_arrays);
		sub_arrays = Z_LVAL_PP(p_sub_arrays);
	}
	if(ac >= 4) {
		convert_to_long_ex(p_read_thumbnail);
		read_thumbnail = Z_LVAL_PP(p_read_thumbnail);
	}
	if(ac >= 5) {
		convert_to_long_ex(p_read_all);
		read_all = Z_LVAL_PP(p_read_all);
	}
	/* parameters 3,4 will be working in later versions.... */
	read_all = 0;       /* just to make function work for 4.2 tree */

	ret = exif_read_file(&ImageInfo, Z_STRVAL_PP(p_name), read_thumbnail, read_all TSRMLS_CC);

   	sections_str = exif_get_sectionlist(ImageInfo.sections_found TSRMLS_CC);

#ifdef EXIF_DEBUG
	if (sections_str) 
		exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "sections found: %s", sections_str[0] ? sections_str : "None");
#endif

	ImageInfo.sections_found |= FOUND_COMPUTED;/* do not inform about in debug*/

	if (ret==FALSE || (sections_needed && !(sections_needed&ImageInfo.sections_found)) || array_init(return_value) == FAILURE) {
		/* array_init must be checked at last! otherwise the array must be freed if a later test fails. */
		exif_discard_imageinfo(&ImageInfo);
	   	EFREE_IF(sections_str);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "generate section FILE");
#endif

	/* now we can add our information */
	exif_iif_add_str(&ImageInfo, SECTION_FILE, "FileName",      ImageInfo.FileName TSRMLS_CC);
	exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileDateTime",  ImageInfo.FileDateTime TSRMLS_CC);
	exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileSize",      ImageInfo.FileSize TSRMLS_CC);
	exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileType",      ImageInfo.FileType TSRMLS_CC);
	exif_iif_add_str(&ImageInfo, SECTION_FILE, "MimeType",      (char*)php_image_type_to_mime_type(ImageInfo.FileType) TSRMLS_CC);
	exif_iif_add_str(&ImageInfo, SECTION_FILE, "SectionsFound", sections_str ? sections_str : "NONE" TSRMLS_CC);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "generate section COMPUTED");
#endif

	if (ImageInfo.Width>0 &&  ImageInfo.Height>0) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "html"    TSRMLS_CC, "width=\"%d\" height=\"%d\"", ImageInfo.Width, ImageInfo.Height);
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Height", ImageInfo.Height TSRMLS_CC);
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Width",  ImageInfo.Width TSRMLS_CC);
	}
	exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "IsColor", ImageInfo.IsColor TSRMLS_CC);
	if (ImageInfo.motorola_intel != -1) {
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "ByteOrderMotorola", ImageInfo.motorola_intel TSRMLS_CC);
	}
	if (ImageInfo.FocalLength) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "FocalLength" TSRMLS_CC, "%4.1fmm", ImageInfo.FocalLength);
		if(ImageInfo.CCDWidth) {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "35mmFocalLength" TSRMLS_CC, "%dmm", (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*35+0.5));
		}
	}
	if(ImageInfo.CCDWidth) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "CCDWidth" TSRMLS_CC, "%dmm", (int)ImageInfo.CCDWidth);
	}
	if(ImageInfo.ExposureTime>0) {
		if(ImageInfo.ExposureTime <= 0.5) {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ExposureTime" TSRMLS_CC, "%0.3f s (1/%d)", ImageInfo.ExposureTime, (int)(0.5 + 1/ImageInfo.ExposureTime));
		} else {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ExposureTime" TSRMLS_CC, "%0.3f s", ImageInfo.ExposureTime);
		}
	}
	if(ImageInfo.ApertureFNumber) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ApertureFNumber" TSRMLS_CC, "f/%.1f", ImageInfo.ApertureFNumber);
	}
	if(ImageInfo.Distance) {
		if(ImageInfo.Distance<0) {
			exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "FocusDistance", "Infinite" TSRMLS_CC);
		} else {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "FocusDistance" TSRMLS_CC, "%0.2fm", ImageInfo.Distance);
		}
	}
	if (ImageInfo.UserComment) {
		exif_iif_add_buffer(&ImageInfo, SECTION_COMPUTED, "UserComment", ImageInfo.UserCommentLength, ImageInfo.UserComment TSRMLS_CC);
		if (ImageInfo.UserCommentEncoding && strlen(ImageInfo.UserCommentEncoding)) {
			exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "UserCommentEncoding", ImageInfo.UserCommentEncoding TSRMLS_CC);
		}
	}

	exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright",              ImageInfo.Copyright TSRMLS_CC);
	exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright.Photographer", ImageInfo.CopyrightPhotographer TSRMLS_CC);
	exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright.Editor",       ImageInfo.CopyrightEditor TSRMLS_CC);

	for (i=0; i<ImageInfo.xp_fields.count; i++) {
		exif_iif_add_str(&ImageInfo, SECTION_WINXP, exif_get_tagname(ImageInfo.xp_fields.list[i].tag, NULL, 0, exif_get_tag_table(SECTION_WINXP) TSRMLS_CC), ImageInfo.xp_fields.list[i].value TSRMLS_CC);
	}
	if (ImageInfo.Thumbnail.size) {
		if (read_thumbnail) {
			/* not exif_iif_add_str : this is a buffer */
			exif_iif_add_tag(&ImageInfo, SECTION_THUMBNAIL, "THUMBNAIL", TAG_NONE, TAG_FMT_UNDEFINED, ImageInfo.Thumbnail.size, ImageInfo.Thumbnail.data TSRMLS_CC);
		}
		if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
			/* try to evaluate if thumbnail data is present */
			exif_scan_thumbnail(&ImageInfo TSRMLS_CC);
		}
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.FileType", ImageInfo.Thumbnail.filetype TSRMLS_CC);
		exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Thumbnail.MimeType", (char*)php_image_type_to_mime_type(ImageInfo.Thumbnail.filetype) TSRMLS_CC);
	}
	if (ImageInfo.Thumbnail.width && ImageInfo.Thumbnail.height) {
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.Height", ImageInfo.Thumbnail.height TSRMLS_CC);
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.Width",  ImageInfo.Thumbnail.width TSRMLS_CC);
	}
   	EFREE_IF(sections_str);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "adding image infos");
#endif

	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_FILE       TSRMLS_CC);
	add_assoc_image_info(return_value, 1,          &ImageInfo, SECTION_COMPUTED   TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_ANY_TAG    TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_IFD0       TSRMLS_CC);
	add_assoc_image_info(return_value, 1,          &ImageInfo, SECTION_THUMBNAIL  TSRMLS_CC);
	add_assoc_image_info(return_value, 1,          &ImageInfo, SECTION_COMMENT    TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_EXIF       TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_GPS        TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_INTEROP    TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_FPIX       TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_APP12      TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_WINXP      TSRMLS_CC);
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_MAKERNOTE  TSRMLS_CC);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "discarding info");
#endif

	exif_discard_imageinfo(&ImageInfo);

#ifdef EXIF_DEBUG
	php_error_docref1(NULL TSRMLS_CC, Z_STRVAL_PP(p_name), E_NOTICE, "done");
#endif
}
/* }}} */

/* {{{ proto string|false exif_thumbnail(string filename [, &width, &height [, &imagetype]])
   Reads the embedded thumbnail */
PHP_FUNCTION(exif_thumbnail)
{
	zval **p_name, **p_width, **p_height, **p_imagetype;
	int ret, arg_c = ZEND_NUM_ARGS();
	image_info_type ImageInfo;

	memset(&ImageInfo, 0, sizeof(ImageInfo));

	if ((arg_c!=1 && arg_c!=3 && arg_c!=4) || zend_get_parameters_ex(arg_c, &p_name, &p_width, &p_height, &p_imagetype) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(p_name);
	if (arg_c >= 3) {
		zval_dtor(*p_width);
		zval_dtor(*p_height);
	}
	if (arg_c >= 4) {
		zval_dtor(*p_imagetype);
	}

	ret = exif_read_file(&ImageInfo, Z_STRVAL_PP(p_name), 1, 0 TSRMLS_CC);
	if (ret==FALSE) {
		exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "Thumbnail data %d %d %d, %d x %d", ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size, ImageInfo.Thumbnail.filetype, ImageInfo.Thumbnail.width, ImageInfo.Thumbnail.height);
#endif
	if (!ImageInfo.Thumbnail.data || !ImageInfo.Thumbnail.size) {
		exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "returning thumbnail(%d)", ImageInfo.Thumbnail.size);
#endif

	ZVAL_STRINGL(return_value, ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size, 1);
	if (arg_c >= 3) {
		if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
			exif_scan_thumbnail(&ImageInfo TSRMLS_CC);
		}
		ZVAL_LONG(*p_width,  ImageInfo.Thumbnail.width);
		ZVAL_LONG(*p_height, ImageInfo.Thumbnail.height);
	}
	if (arg_c >= 4)	{
		ZVAL_LONG(*p_imagetype, ImageInfo.Thumbnail.filetype);
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL TSRMLS_CC, &ImageInfo, E_NOTICE, "discarding info");
#endif

	exif_discard_imageinfo(&ImageInfo);

#ifdef EXIF_DEBUG
	php_error_docref1(NULL TSRMLS_CC, Z_STRVAL_PP(p_name), E_NOTICE, "done");
#endif
}
/* }}} */

/* {{{ proto int exif_imagetype(string imagefile)
   Get the type of an image */
PHP_FUNCTION(exif_imagetype)
{
	zval **arg1;
	php_stream * stream;
 	int itype = 0;

	if (ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;

	if (zend_get_parameters_ex(1, &arg1) == FAILURE)
		WRONG_PARAM_COUNT;

	stream = php_stream_open_wrapper(Z_STRVAL_PP(arg1), "rb", IGNORE_PATH|ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);

	if (stream == NULL) {
		RETURN_FALSE;
	}

	itype = php_getimagetype(stream, NULL TSRMLS_CC);

	php_stream_close(stream);

	if (itype == IMAGE_FILETYPE_UNKNOWN) {
		RETURN_FALSE;
	} else {
		ZVAL_LONG(return_value, itype);
	}
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
