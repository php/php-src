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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "ext/standard/file.h"

/* When EXIF_DEBUG is defined the module generates a lot of debug messages
 * that help understanding what is going on. This can and should be used
 * while extending the module as it shows if you are at the right position.
 * You are always considered to have a copy of TIFF6.0 and EXIF2.10 standard.
 */
#undef EXIF_DEBUG

#ifdef EXIF_DEBUG
#define EXIFERR_DC , const char *_file, size_t _line
#define EXIFERR_CC , __FILE__, __LINE__
#else
#define EXIFERR_DC
#define EXIFERR_CC
#endif

#define USE_MBSTRING zend_hash_str_exists(&module_registry, "mbstring", sizeof("mbstring")-1)

#include "php_exif.h"
#include "exif_arginfo.h"
#include <math.h>
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_image.h"
#include "ext/standard/info.h"

/* needed for ssize_t definition */
#include <sys/types.h>

#ifdef __SANITIZE_ADDRESS__
# include <sanitizer/asan_interface.h>
#endif

typedef unsigned char uchar;

#ifndef max
#	define max(a,b) ((a)>(b) ? (a) : (b))
#endif

#define EFREE_IF(ptr)	if (ptr) efree(ptr)

#define MAX_IFD_NESTING_LEVEL 10
#define MAX_IFD_TAGS 1000

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(exif)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "EXIF Support", "enabled");
	php_info_print_table_row(2, "Supported EXIF Version", "0220");
	php_info_print_table_row(2, "Supported filetypes", "JPEG, TIFF");

	if (zend_hash_str_exists(&module_registry, "mbstring", sizeof("mbstring")-1)) {
		php_info_print_table_row(2, "Multibyte decoding support using mbstring", "enabled");
	} else {
		php_info_print_table_row(2, "Multibyte decoding support using mbstring", "disabled");
	}

	php_info_print_table_row(2, "Extended EXIF tag formats", "Canon, Casio, Fujifilm, Nikon, Olympus, Samsung, Panasonic, DJI, Sony, Pentax, Minolta, Sigma, Foveon, Kyocera, Ricoh, AGFA, Epson");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

ZEND_BEGIN_MODULE_GLOBALS(exif)
	char * encode_unicode;
	char * decode_unicode_be;
	char * decode_unicode_le;
	char * encode_jis;
	char * decode_jis_be;
	char * decode_jis_le;
	HashTable *tag_table_cache;
ZEND_END_MODULE_GLOBALS(exif)

ZEND_DECLARE_MODULE_GLOBALS(exif)
#define EXIF_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(exif, v)

#if defined(ZTS) && defined(COMPILE_DL_EXIF)
ZEND_TSRMLS_CACHE_DEFINE()
#endif

/* {{{ PHP_INI */

ZEND_INI_MH(OnUpdateEncode)
{
	if (new_value && ZSTR_LEN(new_value)) {
		const zend_encoding **return_list;
		size_t return_size;
		if (FAILURE == zend_multibyte_parse_encoding_list(ZSTR_VAL(new_value), ZSTR_LEN(new_value),
	&return_list, &return_size, 0)) {
			php_error_docref(NULL, E_WARNING, "Illegal encoding ignored: '%s'", ZSTR_VAL(new_value));
			return FAILURE;
		}
		pefree((void *) return_list, 0);
	}
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

ZEND_INI_MH(OnUpdateDecode)
{
	if (new_value) {
		const zend_encoding **return_list;
		size_t return_size;
		if (FAILURE == zend_multibyte_parse_encoding_list(ZSTR_VAL(new_value), ZSTR_LEN(new_value),
	&return_list, &return_size, 0)) {
			php_error_docref(NULL, E_WARNING, "Illegal encoding ignored: '%s'", ZSTR_VAL(new_value));
			return FAILURE;
		}
		pefree((void *) return_list, 0);
	}
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
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

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(exif)
{
#if defined(COMPILE_DL_EXIF) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	exif_globals->encode_unicode    = NULL;
	exif_globals->decode_unicode_be = NULL;
	exif_globals->decode_unicode_le = NULL;
	exif_globals->encode_jis        = NULL;
	exif_globals->decode_jis_be     = NULL;
	exif_globals->decode_jis_le     = NULL;
	exif_globals->tag_table_cache   = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(exif) */
PHP_MINIT_FUNCTION(exif)
{
	REGISTER_INI_ENTRIES();

	register_exif_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(exif)
{
	UNREGISTER_INI_ENTRIES();
	if (EXIF_G(tag_table_cache)) {
		zend_hash_destroy(EXIF_G(tag_table_cache));
		free(EXIF_G(tag_table_cache));
	}
	return SUCCESS;
}
/* }}} */

/* {{{ exif dependencies */
static const zend_module_dep exif_module_deps[] = {
	ZEND_MOD_REQUIRED("standard")
	ZEND_MOD_OPTIONAL("mbstring")
	ZEND_MOD_END
};
/* }}} */

/* {{{ exif_module_entry */
zend_module_entry exif_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	exif_module_deps,
	"exif",
	ext_functions,
	PHP_MINIT(exif),
	PHP_MSHUTDOWN(exif),
	NULL, NULL,
	PHP_MINFO(exif),
	PHP_EXIF_VERSION,
	PHP_MODULE_GLOBALS(exif),
	PHP_GINIT(exif),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_EXIF
ZEND_GET_MODULE(exif)
#endif

/* php_stream_read() may return early without reading all data, depending on the chunk size
 * and whether it's a URL stream or not. This helper keeps reading until the requested amount
 * is read or until there is no more data available to read. */
static ssize_t exif_read_from_stream_file_looped(php_stream *stream, char *buf, size_t count)
{
	ssize_t total_read = 0;
	while (total_read < count) {
		ssize_t ret = php_stream_read(stream, buf + total_read, count - total_read);
		if (ret == -1) {
			return -1;
		}
		if (ret == 0) {
			break;
		}
		total_read += ret;
	}
	return total_read;
}

/* }}} */

/* {{{ error messages */
static const char *const EXIF_ERROR_FILEEOF   = "Unexpected end of file reached";
static const char *const EXIF_ERROR_CORRUPT   = "File structure corrupted";
static const char *const EXIF_ERROR_THUMBEOF  = "Thumbnail goes IFD boundary or end of file reached";
static const char *const EXIF_ERROR_FSREALLOC = "Illegal reallocating of undefined file section";

#define EXIF_ERRLOG_FILEEOF(ImageInfo)    exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "%s", EXIF_ERROR_FILEEOF);
#define EXIF_ERRLOG_CORRUPT(ImageInfo)    exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "%s", EXIF_ERROR_CORRUPT);
#define EXIF_ERRLOG_THUMBEOF(ImageInfo)   exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "%s", EXIF_ERROR_THUMBEOF);
#define EXIF_ERRLOG_FSREALLOC(ImageInfo)  exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "%s", EXIF_ERROR_FSREALLOC);
/* }}} */

/* {{{ format description defines
   Describes format descriptor
*/
static const int php_tiff_bytes_per_format[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8, 1};
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
#define TAG_TIFF_COMMENT                0x00FE /* SHOULDN'T HAPPEN */
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

/* {{{ TabTable[] */
typedef const struct {
	unsigned short Tag;
	char *Desc;
} tag_info_type;

typedef tag_info_type  tag_info_array[];
typedef tag_info_type  *tag_table_type;

#define TAG_TABLE_END \
  {TAG_NONE,           "No tag value"},\
  {TAG_COMPUTED_VALUE, "Computed value"},\
  {TAG_END_OF_LIST,    ""}  /* Important for exif_get_tagname() IF value != "" function result is != false */

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

static tag_info_array tag_table_VND_SAMSUNG = {
  { 0x0001, "Version"},
  { 0x0021, "PictureWizard"},
  { 0x0030, "LocalLocationName"},
  { 0x0031, "LocationName"},
  { 0x0035, "Preview"},
  { 0x0043, "CameraTemperature"},
  { 0xa001, "FirmwareName"},
  { 0xa003, "LensType"},
  { 0xa004, "LensFirmware"},
  { 0xa010, "SensorAreas"},
  { 0xa011, "ColorSpace"},
  { 0xa012, "SmartRange"},
  { 0xa013, "ExposureBiasValue"},
  { 0xa014, "ISO"},
  { 0xa018, "ExposureTime"},
  { 0xa019, "FNumber"},
  { 0xa01a, "FocalLengthIn35mmFormat"},
  { 0xa020, "EncryptionKey"},
  { 0xa021, "WB_RGGBLevelsUncorrected"},
  { 0xa022, "WB_RGGBLevelsAuto"},
  { 0xa023, "WB_RGGBLevelsIlluminator1"},
  { 0xa024, "WB_RGGBLevelsIlluminator2"},
  { 0xa028, "WB_RGGBLevelsBlack"},
  { 0xa030, "ColorMatrix"},
  { 0xa031, "ColorMatrixSRGB"},
  { 0xa032, "ColorMatrixAdobeRGB"},
  { 0xa040, "ToneCurve1"},
  { 0xa041, "ToneCurve2"},
  { 0xa042, "ToneCurve3"},
  { 0xa043, "ToneCurve4"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_PANASONIC = {
  { 0x0001, "Quality"},
  { 0x0002, "FirmwareVersion"},
  { 0x0003, "WhiteBalance"},
  { 0x0007, "FocusMode"},
  { 0x000f, "AFMode"},
  { 0x001a, "ImageStabilization"},
  { 0x001c, "Macro"},
  { 0x001f, "ShootingMode"},
  { 0x0020, "Audio"},
  { 0x0021, "DataDump"},
  { 0x0023, "WhiteBalanceBias"},
  { 0x0024, "FlashBias"},
  { 0x0025, "InternalSerialNumber"},
  { 0x0026, "ExifVersion"},
  { 0x0028, "ColorEffect"},
  { 0x0029, "TimeSincePowerOn"},
  { 0x002a, "BurstMode"},
  { 0x002b, "SequenceNumber"},
  { 0x002c, "Contrast"},
  { 0x002d, "NoiseReduction"},
  { 0x002e, "SelfTimer"},
  { 0x0030, "Rotation"},
  { 0x0031, "AFAssistLamp"},
  { 0x0032, "ColorMode"},
  { 0x0033, "BabyAge1"},
  { 0x0034, "OpticalZoomMode"},
  { 0x0035, "ConversionLens"},
  { 0x0036, "TravelDay"},
  { 0x0039, "Contrast"},
  { 0x003a, "WorldTimeLocation"},
  { 0x003b, "TextStamp1"},
  { 0x003c, "ProgramISO"},
  { 0x003d, "AdvancedSceneType"},
  { 0x003e, "TextStamp2"},
  { 0x003f, "FacesDetected"},
  { 0x0040, "Saturation"},
  { 0x0041, "Sharpness"},
  { 0x0042, "FilmMode"},
  { 0x0044, "ColorTempKelvin"},
  { 0x0045, "BracketSettings"},
  { 0x0046, "WBAdjustAB"},
  { 0x0047, "WBAdjustGM"},
  { 0x0048, "FlashCurtain"},
  { 0x0049, "LongShutterNoiseReduction"},
  { 0x004b, "ImageWidth"},
  { 0x004c, "ImageHeight"},
  { 0x004d, "AFPointPosition"},
  { 0x004e, "FaceDetInfo"},
  { 0x0051, "LensType"},
  { 0x0052, "LensSerialNumber"},
  { 0x0053, "AccessoryType"},
  { 0x0054, "AccessorySerialNumber"},
  { 0x0059, "Transform1"},
  { 0x005d, "IntelligentExposure"},
  { 0x0060, "LensFirmwareVersion"},
  { 0x0061, "FaceRecInfo"},
  { 0x0062, "FlashWarning"},
  { 0x0065, "Title"},
  { 0x0066, "BabyName"},
  { 0x0067, "Location"},
  { 0x0069, "Country"},
  { 0x006b, "State"},
  { 0x006d, "City"},
  { 0x006f, "Landmark"},
  { 0x0070, "IntelligentResolution"},
  { 0x0077, "BurstSheed"},
  { 0x0079, "IntelligentDRange"},
  { 0x007c, "ClearRetouch"},
  { 0x0080, "City2"},
  { 0x0086, "ManometerPressure"},
  { 0x0089, "PhotoStyle"},
  { 0x008a, "ShadingCompensation"},
  { 0x008c, "AccelerometerZ"},
  { 0x008d, "AccelerometerX"},
  { 0x008e, "AccelerometerY"},
  { 0x008f, "CameraOrientation"},
  { 0x0090, "RollAngle"},
  { 0x0091, "PitchAngle"},
  { 0x0093, "SweepPanoramaDirection"},
  { 0x0094, "PanoramaFieldOfView"},
  { 0x0096, "TimerRecording"},
  { 0x009d, "InternalNDFilter"},
  { 0x009e, "HDR"},
  { 0x009f, "ShutterType"},
  { 0x00a3, "ClearRetouchValue"},
  { 0x00ab, "TouchAE"},
  { 0x0e00, "PrintIM"},
  { 0x8000, "MakerNoteVersion"},
  { 0x8001, "SceneMode"},
  { 0x8004, "WBRedLevel"},
  { 0x8005, "WBGreenLevel"},
  { 0x8006, "WBBlueLevel"},
  { 0x8007, "FlashFired"},
  { 0x8008, "TextStamp3"},
  { 0x8009, "TextStamp4"},
  { 0x8010, "BabyAge2"},
  { 0x8012, "Transform2"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_DJI = {
  { 0x0001, "Make"},
  { 0x0003, "SpeedX"},
  { 0x0004, "SpeedY"},
  { 0x0005, "SpeedZ"},
  { 0x0006, "Pitch"},
  { 0x0007, "Yaw"},
  { 0x0008, "Roll"},
  { 0x0009, "CameraPitch"},
  { 0x000a, "CameraYaw"},
  { 0x000b, "CameraRoll"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_SONY = {
  { 0x0102, "Quality"},
  { 0x0104, "FlashExposureComp"},
  { 0x0105, "Teleconverter"},
  { 0x0112, "WhiteBalanceFineTune"},
  { 0x0114, "CameraSettings"},
  { 0x0115, "WhiteBalance"},
  { 0x0116, "ExtraInfo"},
  { 0x0e00, "PrintIM"},
  { 0x1000, "MultiBurstMode"},
  { 0x1001, "MultiBurstImageWidth"},
  { 0x1002, "MultiBurstImageHeight"},
  { 0x1003, "Panorama"},
  { 0x2001, "PreviewImage"},
  { 0x2002, "Rating"},
  { 0x2004, "Contrast"},
  { 0x2005, "Saturation"},
  { 0x2006, "Sharpness"},
  { 0x2007, "Brightness"},
  { 0x2008, "LongExposureNoiseReduction"},
  { 0x2009, "HighISONoiseReduction"},
  { 0x200a, "AutoHDR"},
  { 0x3000, "ShotInfo"},
  { 0xb000, "FileFormat"},
  { 0xb001, "SonyModelID"},
  { 0xb020, "ColorReproduction"},
  { 0xb021, "ColorTemperature"},
  { 0xb022, "ColorCompensationFilter"},
  { 0xb023, "SceneMode"},
  { 0xb024, "ZoneMatching"},
  { 0xb025, "DynamicRangeOptimizer"},
  { 0xb026, "ImageStabilization"},
  { 0xb027, "LensID"},
  { 0xb028, "MinoltaMakerNote"},
  { 0xb029, "ColorMode"},
  { 0xb02b, "FullImageSize"},
  { 0xb02c, "PreviewImageSize"},
  { 0xb040, "Macro"},
  { 0xb041, "ExposureMode"},
  { 0xb042, "FocusMode"},
  { 0xb043, "AFMode"},
  { 0xb044, "AFIlluminator"},
  { 0xb047, "JPEGQuality"},
  { 0xb048, "FlashLevel"},
  { 0xb049, "ReleaseMode"},
  { 0xb04a, "SequenceNumber"},
  { 0xb04b, "AntiBlur"},
  { 0xb04e, "FocusMode"},
  { 0xb04f, "DynamicRangeOptimizer"},
  { 0xb050, "HighISONoiseReduction2"},
  { 0xb052, "IntelligentAuto"},
  { 0xb054, "WhiteBalance2"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_PENTAX = {
  { 0x0000, "Version"},
  { 0x0001, "Mode"},
  { 0x0002, "PreviewResolution"},
  { 0x0003, "PreviewLength"},
  { 0x0004, "PreviewOffset"},
  { 0x0005, "ModelID"},
  { 0x0006, "Date"},
  { 0x0007, "Time"},
  { 0x0008, "Quality"},
  { 0x0009, "Size"},
  { 0x000c, "Flash"},
  { 0x000d, "Focus"},
  { 0x000e, "AFPoint"},
  { 0x000f, "AFPointInFocus"},
  { 0x0012, "ExposureTime"},
  { 0x0013, "FNumber"},
  { 0x0014, "ISO"},
  { 0x0016, "ExposureCompensation"},
  { 0x0017, "MeteringMode"},
  { 0x0018, "AutoBracketing"},
  { 0x0019, "WhiteBalance"},
  { 0x001a, "WhiteBalanceMode"},
  { 0x001b, "BlueBalance"},
  { 0x001c, "RedBalance"},
  { 0x001d, "FocalLength"},
  { 0x001e, "DigitalZoom"},
  { 0x001f, "Saturation"},
  { 0x0020, "Contrast"},
  { 0x0021, "Sharpness"},
  { 0x0022, "Location"},
  { 0x0023, "Hometown"},
  { 0x0024, "Destination"},
  { 0x0025, "HometownDST"},
  { 0x0026, "DestinationDST"},
  { 0x0027, "DSPFirmwareVersion"},
  { 0x0028, "CPUFirmwareVersion"},
  { 0x0029, "FrameNumber"},
  { 0x002d, "EffectiveLV"},
  { 0x0032, "ImageProcessing"},
  { 0x0033, "PictureMode"},
  { 0x0034, "DriveMode"},
  { 0x0037, "ColorSpace"},
  { 0x0038, "ImageAreaOffset"},
  { 0x0039, "RawImageSize"},
  { 0x003e, "PreviewImageBorders"},
  { 0x003f, "LensType"},
  { 0x0040, "SensitivityAdjust"},
  { 0x0041, "DigitalFilter"},
  { 0x0047, "Temperature"},
  { 0x0048, "AELock"},
  { 0x0049, "NoiseReduction"},
  { 0x004d, "FlashExposureCompensation"},
  { 0x004f, "ImageTone"},
  { 0x0050, "ColorTemperature"},
  { 0x005c, "ShakeReduction"},
  { 0x005d, "ShutterCount"},
  { 0x0069, "DynamicRangeExpansion"},
  { 0x0071, "HighISONoiseReduction"},
  { 0x0072, "AFAdjustment"},
  { 0x0200, "BlackPoint"},
  { 0x0201, "WhitePoint"},
  { 0x0205, "ShotInfo"},
  { 0x0206, "AEInfo"},
  { 0x0207, "LensInfo"},
  { 0x0208, "FlashInfo"},
  { 0x0209, "AEMeteringSegments"},
  { 0x020a, "FlashADump"},
  { 0x020b, "FlashBDump"},
  { 0x020d, "WB_RGGBLevelsDaylight"},
  { 0x020e, "WB_RGGBLevelsShade"},
  { 0x020f, "WB_RGGBLevelsCloudy"},
  { 0x0210, "WB_RGGBLevelsTungsten"},
  { 0x0211, "WB_RGGBLevelsFluorescentD"},
  { 0x0212, "WB_RGGBLevelsFluorescentN"},
  { 0x0213, "WB_RGGBLevelsFluorescentW"},
  { 0x0214, "WB_RGGBLevelsFlash"},
  { 0x0215, "CameraInfo"},
  { 0x0216, "BatteryInfo"},
  { 0x021f, "AFInfo"},
  { 0x0222, "ColorInfo"},
  { 0x0229, "SerialNumber"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_MINOLTA = {
  { 0x0000, "Version"},
  { 0x0001, "CameraSettingsStdOld"},
  { 0x0003, "CameraSettingsStdNew"},
  { 0x0004, "CameraSettings7D"},
  { 0x0018, "ImageStabilizationData"},
  { 0x0020, "WBInfoA100"},
  { 0x0040, "CompressedImageSize"},
  { 0x0081, "Thumbnail"},
  { 0x0088, "ThumbnailOffset"},
  { 0x0089, "ThumbnailLength"},
  { 0x0100, "SceneMode"},
  { 0x0101, "ColorMode"},
  { 0x0102, "Quality"},
  { 0x0104, "FlashExposureComp"},
  { 0x0105, "Teleconverter"},
  { 0x0107, "ImageStabilization"},
  { 0x0109, "RawAndJpgRecording"},
  { 0x010a, "ZoneMatching"},
  { 0x010b, "ColorTemperature"},
  { 0x010c, "LensID"},
  { 0x0111, "ColorCompensationFilter"},
  { 0x0112, "WhiteBalanceFineTune"},
  { 0x0113, "ImageStabilizationA100"},
  { 0x0114, "CameraSettings5D"},
  { 0x0115, "WhiteBalance"},
  { 0x0e00, "PrintIM"},
  { 0x0f00, "CameraSettingsZ1"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_SIGMA = {
  { 0x0002, "SerialNumber"},
  { 0x0003, "DriveMode"},
  { 0x0004, "ResolutionMode"},
  { 0x0005, "AutofocusMode"},
  { 0x0006, "FocusSetting"},
  { 0x0007, "WhiteBalance"},
  { 0x0008, "ExposureMode"},
  { 0x0009, "MeteringMode"},
  { 0x000a, "LensRange"},
  { 0x000b, "ColorSpace"},
  { 0x000c, "Exposure"},
  { 0x000d, "Contrast"},
  { 0x000e, "Shadow"},
  { 0x000f, "Highlight"},
  { 0x0010, "Saturation"},
  { 0x0011, "Sharpness"},
  { 0x0012, "FillLight"},
  { 0x0014, "ColorAdjustment"},
  { 0x0015, "AdjustmentMode"},
  { 0x0016, "Quality"},
  { 0x0017, "Firmware"},
  { 0x0018, "Software"},
  { 0x0019, "AutoBracket"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_KYOCERA = {
  { 0x0001, "FormatThumbnail"},
  { 0x0E00, "PrintImageMatchingInfo"},
  TAG_TABLE_END
};

static tag_info_array tag_table_VND_RICOH = {
  { 0x0001, "MakerNoteDataType"},
  { 0x0002, "Version"},
  { 0x0E00, "PrintImageMatchingInfo"},
  { 0x2001, "RicohCameraInfoMakerNoteSubIFD"},
  TAG_TABLE_END
};

typedef enum mn_byte_order_t {
	MN_ORDER_INTEL    = 0,
	MN_ORDER_MOTOROLA = 1,
	MN_ORDER_NORMAL
} mn_byte_order_t;

typedef enum mn_offset_mode_t {
	MN_OFFSET_NORMAL,
	MN_OFFSET_MAKER
} mn_offset_mode_t;

typedef struct {
	tag_table_type   tag_table;
	char *           make;
	char *           id_string;
	int              id_string_len;
	int              offset;
	mn_byte_order_t  byte_order;
	mn_offset_mode_t offset_mode;
} maker_note_type;

/* Some maker notes (e.g. DJI info tag) require custom parsing */
#define REQUIRES_CUSTOM_PARSING NULL

/* Remember to update PHP_MINFO if updated */
static const maker_note_type maker_note_array[] = {
  { tag_table_VND_CANON,     "Canon",                   NULL,								0,  0,  MN_ORDER_INTEL,    MN_OFFSET_NORMAL},
  { tag_table_VND_CASIO,     "CASIO",                   NULL,							 	0,  0,  MN_ORDER_MOTOROLA, MN_OFFSET_NORMAL},
  { tag_table_VND_FUJI,      "FUJIFILM",                "FUJIFILM\x0C\x00\x00\x00",		 	12, 12, MN_ORDER_INTEL,    MN_OFFSET_MAKER},
  { tag_table_VND_NIKON,     "NIKON",                   "Nikon\x00\x01\x00",				8,  8,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_NIKON_990, "NIKON",                   NULL,								0,  0,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_OLYMPUS,   "OLYMPUS OPTICAL CO.,LTD", "OLYMP\x00\x01\x00",				8,  8,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_SAMSUNG,   "SAMSUNG",                 NULL,								0,  0,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_PANASONIC, "Panasonic",               "Panasonic\x00\x00\x00",			12, 12, MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { REQUIRES_CUSTOM_PARSING, "DJI",                     "[ae_dbg_info:",					13, 13, MN_ORDER_MOTOROLA, MN_OFFSET_NORMAL},
  { tag_table_VND_DJI,       "DJI",                     NULL,								0,  0,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_SONY,      "SONY",                    "SONY DSC \x00\x00\x00",			12, 12, MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_SONY,      "SONY",                    NULL,								0,  0,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_PENTAX,    "PENTAX",                  "AOC\x00",						 	6,  6,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_MINOLTA,   "Minolta, KONICA MINOLTA", NULL,								0,  0,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_SIGMA,     "SIGMA, FOVEON",           "SIGMA\x00\x00\x00",				10, 10, MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_SIGMA,     "SIGMA, FOVEON",           "FOVEON\x00\x00\x00",				10, 10, MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_KYOCERA,   "KYOCERA, CONTAX",			"KYOCERA            \x00\x00\x00",	22, 22, MN_ORDER_NORMAL,   MN_OFFSET_MAKER},
  { tag_table_VND_RICOH,	 "RICOH",					"Ricoh",							5,  5,  MN_ORDER_MOTOROLA, MN_OFFSET_NORMAL},
  { tag_table_VND_RICOH,     "RICOH",					"RICOH",							5,  5,  MN_ORDER_MOTOROLA, MN_OFFSET_NORMAL},

  /* These re-uses existing formats */
  { tag_table_VND_OLYMPUS,   "AGFA",					"AGFA \x00\x01",					8,  8,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL},
  { tag_table_VND_OLYMPUS,   "EPSON",					"EPSON\x00\x01\x00",				8,  8,  MN_ORDER_NORMAL,   MN_OFFSET_NORMAL}
};
/* }}} */

static HashTable *exif_make_tag_ht(tag_info_type *tag_table)
{
	HashTable *ht = malloc(sizeof(HashTable));
	zend_hash_init(ht, 0, NULL, NULL, 1);
	while (tag_table->Tag != TAG_END_OF_LIST) {
		if (!zend_hash_index_add_ptr(ht, tag_table->Tag, tag_table->Desc)) {
			zend_error(E_CORE_ERROR, "Duplicate tag %x", tag_table->Tag);
		}
		tag_table++;
	}
	return ht;
}

static void exif_tag_ht_dtor(zval *zv)
{
	HashTable *ht = Z_PTR_P(zv);
	zend_hash_destroy(ht);
	free(ht);
}

static HashTable *exif_get_tag_ht(tag_info_type *tag_table)
{
	HashTable *ht;

	if (!EXIF_G(tag_table_cache)) {
		EXIF_G(tag_table_cache) = malloc(sizeof(HashTable));
		zend_hash_init(EXIF_G(tag_table_cache), 0, NULL, exif_tag_ht_dtor, 1);
	}

	ht = zend_hash_index_find_ptr(EXIF_G(tag_table_cache), (uintptr_t) tag_table);
	if (ht) {
		return ht;
	}

	ht = exif_make_tag_ht(tag_table);
	zend_hash_index_add_new_ptr(EXIF_G(tag_table_cache), (uintptr_t) tag_table, ht);
	return ht;
}

/* {{{ exif_get_tagname
	Get headername for tag_num or NULL if not defined */
static char *exif_get_tagname(int tag_num, tag_table_type tag_table)
{
	return zend_hash_index_find_ptr(exif_get_tag_ht(tag_table), tag_num);
}
/* }}} */

static char *exif_get_tagname_debug(int tag_num, tag_table_type tag_table)
{
	char *desc = zend_hash_index_find_ptr(exif_get_tag_ht(tag_table), tag_num);
	if (desc) {
		return desc;
	}
	return "UndefinedTag";
}

static char *exif_get_tagname_key(int tag_num, char *buf, size_t buf_size, tag_table_type tag_table)
{
	char *desc = zend_hash_index_find_ptr(exif_get_tag_ht(tag_table), tag_num);
	if (desc) {
		return desc;
	}
	snprintf(buf, buf_size, "UndefinedTag:0x%04X", tag_num);
	return buf;
}

/* {{{ exif_char_dump
 * Do not use! This is a debug function... */
#ifdef EXIF_DEBUG
static char* exif_char_dump(char * addr, int len, int offset)
{
	static char buf[4096+1];
	static char tmp[20];
	int c, i, p=0, n = 5+31;

	p += slprintf(buf+p, sizeof(buf)-p, "\nDump Len: %08X (%d)", len, len);
	if (len) {
		for(i=0; i<len+15 && p+n<=sizeof(buf); i++) {
			if (i%16==0) {
				p += slprintf(buf+p, sizeof(buf)-p, "\n%08X: ", i+offset);
			}
			if (i<len) {
				c = *((unsigned char *)addr++);
				p += slprintf(buf+p, sizeof(buf)-p, "%02X ", c);
				tmp[i%16] = c>=32 ? c : '.';
				tmp[(i%16)+1] = '\0';
			} else {
				p += slprintf(buf+p, sizeof(buf)-p, "   ");
			}
			if (i%16==15) {
				p += slprintf(buf+p, sizeof(buf)-p, "    %s", tmp);
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

/* {{{ php_ifd_get32u
 * Convert a 32 bit unsigned value from file's native byte order */
static unsigned php_ifd_get32u(void *void_value, int motorola_intel)
{
	uchar *value = (uchar *) void_value;
	if (motorola_intel) {
		return  ((unsigned)value[0] << 24)
			  | ((unsigned)value[1] << 16)
			  | ((unsigned)value[2] << 8 )
			  | ((unsigned)value[3]      );
	} else {
		return  ((unsigned)value[3] << 24)
			  | ((unsigned)value[2] << 16)
			  | ((unsigned)value[1] << 8 )
			  | ((unsigned)value[0]      );
	}
}
/* }}} */

/* {{{ php_ifd_get64u
 * Convert a 64 bit unsigned value from file's native byte order */
static uint64_t php_ifd_get64u(void *void_value, int motorola_intel)
{
	uchar *value = (uchar *) void_value;
	if (motorola_intel) {
		return ((uint64_t)value[0] << 56)
			| ((uint64_t)value[1] << 48)
			| ((uint64_t)value[2] << 40)
			| ((uint64_t)value[3] << 32)
			| ((uint64_t)value[4] << 24)
			| ((uint64_t)value[5] << 16)
			| ((uint64_t)value[6] << 8 )
			| ((uint64_t)value[7]      );
	} else {
		return ((uint64_t)value[7] << 56)
			| ((uint64_t)value[6] << 48)
			| ((uint64_t)value[5] << 40)
			| ((uint64_t)value[4] << 32)
			| ((uint64_t)value[3] << 24)
			| ((uint64_t)value[2] << 16)
			| ((uint64_t)value[1] << 8 )
			| ((uint64_t)value[0]      );
	}
}
/* }}} */

/* {{{ php_ifd_get32u
 * Convert a 32 bit signed value from file's native byte order */
static unsigned php_ifd_get32s(void *value, int motorola_intel)
{
	return (int) php_ifd_get32u(value, motorola_intel);
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
		data[1] = (char) ((value & 0x00FF0000) >> 16);
		data[2] = (value & 0x0000FF00) >>  8;
		data[3] = (value & 0x000000FF);
	} else {
		data[3] = (value & 0xFF000000) >> 24;
		data[2] = (char) ((value & 0x00FF0000) >> 16);
		data[1] = (value & 0x0000FF00) >>  8;
		data[0] = (value & 0x000000FF);
	}
}
/* }}} */

static float php_ifd_get_float(char *data) {
	union { uint32_t i; float f; } u;
	u.i = php_ifd_get32u(data, 0);
	return u.f;
}

static double php_ifd_get_double(char *data) {
	union { uint64_t i; double f; } u;
	u.i = php_ifd_get64u(data, 0);
	return u.f;
}

#ifdef EXIF_DEBUG
char * exif_dump_data(int *dump_free, int format, int components, int motorola_intel, char *value_ptr) /* {{{ */
{
	char *dump;
	int len;

	*dump_free = 0;
	if (format == TAG_FMT_STRING) {
		return value_ptr ? value_ptr : "<no data>";
	}
	if (format == TAG_FMT_UNDEFINED) {
		return "<undefined>";
	}
	if (format == TAG_FMT_IFD) {
		return "";
	}
	if (format == TAG_FMT_SINGLE || format == TAG_FMT_DOUBLE) {
		return "<not implemented>";
	}
	*dump_free = 1;
	if (components > 1) {
		len = spprintf(&dump, 0, "(%d) {", components);
	} else {
		len = spprintf(&dump, 0, "{");
	}
	while(components > 0) {
		switch(format) {
			case TAG_FMT_BYTE:
			case TAG_FMT_UNDEFINED:
			case TAG_FMT_STRING:
			case TAG_FMT_SBYTE:
				dump = erealloc(dump, len + 4 + 1);
				snprintf(dump + len, 4 + 1, "0x%02X", *value_ptr);
				len += 4;
				value_ptr++;
				break;
			case TAG_FMT_USHORT:
			case TAG_FMT_SSHORT:
				dump = erealloc(dump, len + 6 + 1);
				snprintf(dump + len, 6 + 1, "0x%04X", php_ifd_get16s(value_ptr, motorola_intel));
				len += 6;
				value_ptr += 2;
				break;
			case TAG_FMT_ULONG:
			case TAG_FMT_SLONG:
				dump = erealloc(dump, len + 6 + 1);
				snprintf(dump + len, 6 + 1, "0x%04X", php_ifd_get32s(value_ptr, motorola_intel));
				len += 6;
				value_ptr += 4;
				break;
			case TAG_FMT_URATIONAL:
			case TAG_FMT_SRATIONAL:
				dump = erealloc(dump, len + 13 + 1);
				snprintf(dump + len, 13 + 1, "0x%04X/0x%04X", php_ifd_get32s(value_ptr, motorola_intel), php_ifd_get32s(value_ptr+4, motorola_intel));
				len += 13;
				value_ptr += 8;
				break;
		}
		if (components > 0) {
			dump = erealloc(dump, len + 2 + 1);
			snprintf(dump + len, 2 + 1, ", ");
			len += 2;
			components--;
		} else{
			break;
		}
	}
	dump = erealloc(dump, len + 1 + 1);
	snprintf(dump + len, 1 + 1, "}");
	return dump;
}
/* }}} */
#endif

/* {{{ exif_convert_any_format
 * Evaluate number, be it int, rational, or float from directory. */
static double exif_convert_any_format(void *value, int format, int motorola_intel)
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
			php_error_docref(NULL, E_NOTICE, "Found value of type single");
#endif
			return (double) php_ifd_get_float(value);
		case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL, E_NOTICE, "Found value of type double");
#endif
			return php_ifd_get_double(value);
	}
	return 0;
}
/* }}} */

/* {{{ exif_rewrite_tag_format_to_unsigned
 * Rewrite format tag so that it specifies an unsigned type for a tag */
static int exif_rewrite_tag_format_to_unsigned(int format)
{
	switch(format) {
		case TAG_FMT_SBYTE: return TAG_FMT_BYTE;
		case TAG_FMT_SRATIONAL: return TAG_FMT_URATIONAL;
		case TAG_FMT_SSHORT: return TAG_FMT_USHORT;
		case TAG_FMT_SLONG: return TAG_FMT_ULONG;
	}
	return format;
}
/* }}} */

/* Use saturation for out of bounds values to avoid UB */
static size_t float_to_size_t(float x) {
	if (x < 0.0f || zend_isnan(x)) {
		return 0;
	} else if (x > (float) SIZE_MAX) {
		return SIZE_MAX;
	} else {
		return (size_t) x;
	}
}

static size_t double_to_size_t(double x) {
	if (x < 0.0 || zend_isnan(x)) {
		return 0;
	} else if (x > (double) SIZE_MAX) {
		return SIZE_MAX;
	} else {
		return (size_t) x;
	}
}

/* {{{ exif_convert_any_to_int
 * Evaluate number, be it int, rational, or float from directory. */
static size_t exif_convert_any_to_int(void *value, int format, int motorola_intel)
{
	switch (format) {
		case TAG_FMT_SBYTE:     return *(signed char *)value;
		case TAG_FMT_BYTE:      return *(uchar *)value;

		case TAG_FMT_USHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_ULONG:     return php_ifd_get32u(value, motorola_intel);

		case TAG_FMT_URATIONAL: {
			unsigned u_den = php_ifd_get32u(4+(char *)value, motorola_intel);
			if (u_den == 0) {
				return 0;
			} else {
				return php_ifd_get32u(value, motorola_intel) / u_den;
			}
		}

		case TAG_FMT_SRATIONAL: {
			int s_num = php_ifd_get32s(value, motorola_intel);
			int s_den = php_ifd_get32s(4+(char *)value, motorola_intel);
			if (s_den == 0) {
				return 0;
			} else if (s_num == INT_MIN && s_den == -1) {
				return INT_MAX;
			} else {
				return s_num / s_den;
			}
		}

		case TAG_FMT_SSHORT:    return php_ifd_get16u(value, motorola_intel);
		case TAG_FMT_SLONG:     return php_ifd_get32s(value, motorola_intel);

		/* Not sure if this is correct (never seen float used in Exif format) */
		case TAG_FMT_SINGLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL, E_NOTICE, "Found value of type single");
#endif
			return float_to_size_t(php_ifd_get_float(value));
		case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
			php_error_docref(NULL, E_NOTICE, "Found value of type double");
#endif
			return double_to_size_t(php_ifd_get_double(value));
	}
	return 0;
}
/* }}} */

/* {{{ struct image_info_value, image_info_list */
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
	int                 alloc_count;
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

static tag_table_type exif_get_tag_table(int section)
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
static char *exif_get_sectionlist(int sectionlist)
{
	int i, len, ml = 0;
	char *sections;

	for(i=0; i<SECTION_COUNT; i++) {
		ml += strlen(exif_get_sectionname(i))+2;
	}
	sections = safe_emalloc(ml, 1, 1);
	sections[0] = '\0';
	len = 0;
	for(i=0; i<SECTION_COUNT; i++) {
		if (sectionlist&(1<<i)) {
			snprintf(sections+len, ml-len, "%s, ", exif_get_sectionname(i));
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
	int             alloc_count;
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
	int             ifd_nesting_level;
	int             ifd_count;
	int             num_errors;
	/* internal */
	file_section_list 	file;
} image_info_type;
/* }}} */

// EXIF_DEBUG can produce lots of messages
#ifndef EXIF_DEBUG
#define EXIF_MAX_ERRORS 10
#else
#define EXIF_MAX_ERRORS 100000
#endif

/* {{{ exif_error_docref */
static void exif_error_docref(const char *docref EXIFERR_DC, image_info_type *ImageInfo, int type, const char *format, ...)
{
	va_list args;

	if (ImageInfo) {
		if (++ImageInfo->num_errors > EXIF_MAX_ERRORS) {
			if (ImageInfo->num_errors == EXIF_MAX_ERRORS+1) {
				php_error_docref(docref, type,
					"Further exif parsing errors have been suppressed");
			}
			return;
		}
	}

	va_start(args, format);
#ifdef EXIF_DEBUG
	{
		char *buf;

		spprintf(&buf, 0, "%s(%ld): %s", _file, _line, format);
		php_verror(docref, ImageInfo && ImageInfo->FileName ? ImageInfo->FileName:"", type, buf, args);
		efree(buf);
	}
#else
	php_verror(docref, ImageInfo && ImageInfo->FileName ? ImageInfo->FileName:"", type, format, args);
#endif
	va_end(args);
}
/* }}} */

/* {{{ jpeg_sof_info */
typedef struct {
	int     bits_per_sample;
	size_t  width;
	size_t  height;
	int     num_components;
} jpeg_sof_info;
/* }}} */

/* Base address for offset references, together with valid memory range.
 * The valid range does not necessarily include the offset base. */
typedef struct {
	char *offset_base;
	char *valid_start; /* inclusive */
	char *valid_end;   /* exclusive */
} exif_offset_info;

static zend_always_inline bool ptr_offset_overflows(char *ptr, size_t offset) {
	return UINTPTR_MAX - (uintptr_t) ptr < offset;
}

static inline void exif_offset_info_init(
		exif_offset_info *info, char *offset_base, char *valid_start, size_t valid_length) {
	ZEND_ASSERT(!ptr_offset_overflows(valid_start, valid_length));
#ifdef __SANITIZE_ADDRESS__
	ZEND_ASSERT(!__asan_region_is_poisoned(valid_start, valid_length));
#endif
	info->offset_base = offset_base;
	info->valid_start = valid_start;
	info->valid_end = valid_start + valid_length;
}

/* Try to get a pointer at offset_base+offset with length dereferenceable bytes. */
static inline char *exif_offset_info_try_get(
		const exif_offset_info *info, size_t offset, size_t length) {
	char *start, *end;
	if (ptr_offset_overflows(info->offset_base, offset)) {
		return NULL;
	}

	start = info->offset_base + offset;
	if (ptr_offset_overflows(start, length)) {
		return NULL;
	}

	end = start + length;
	if (start < info->valid_start || end > info->valid_end) {
		return NULL;
	}

	return start;
}

static inline bool exif_offset_info_contains(
		const exif_offset_info *info, char *start, size_t length) {
	char *end;
	if (ptr_offset_overflows(start, length)) {
		return 0;
	}

	/* start and valid_start are both inclusive, end and valid_end are both exclusive,
	 * so we use >= and <= to do the checks, respectively. */
	end = start + length;
	return start >= info->valid_start && end <= info->valid_end;
}

#ifdef EXIF_DEBUG
static inline int exif_offset_info_length(const exif_offset_info *info)
{
	return info->valid_end - info->valid_start;
}
#endif

/* {{{ exif_file_sections_add
 Add a file_section to image_info
 returns the used block or -1. if size>0 and data == NULL buffer of size is allocated
*/
static int exif_file_sections_add(image_info_type *ImageInfo, int type, size_t size, uchar *data)
{
	int count = ImageInfo->file.count;
	if (count == ImageInfo->file.alloc_count) {
		int new_alloc_count = ImageInfo->file.alloc_count ? ImageInfo->file.alloc_count * 2 : 1;
		ImageInfo->file.list = safe_erealloc(
			ImageInfo->file.list, new_alloc_count, sizeof(file_section), 0);
		ImageInfo->file.alloc_count = new_alloc_count;
	}

	ImageInfo->file.list[count].type = 0xFFFF;
	ImageInfo->file.list[count].data = NULL;
	ImageInfo->file.list[count].size = 0;
	ImageInfo->file.count = count+1;
	if (!size) {
		data = NULL;
	} else if (data == NULL) {
		data = safe_emalloc(size, 1, 0);
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
static int exif_file_sections_realloc(image_info_type *ImageInfo, int section_index, size_t size)
{
	void *tmp;

	/* This is not a malloc/realloc check. It is a plausibility check for the
	 * function parameters (requirements engineering).
	 */
	if (section_index >= ImageInfo->file.count) {
		EXIF_ERRLOG_FSREALLOC(ImageInfo)
		return -1;
	}
	tmp = safe_erealloc(ImageInfo->file.list[section_index].data, 1, size, 0);
	ImageInfo->file.list[section_index].data = tmp;
	ImageInfo->file.list[section_index].size = size;
	return 0;
}
/* }}} */

/* {{{ exif_file_section_free
   Discard all file_sections in ImageInfo
*/
static bool exif_file_sections_free(image_info_type *ImageInfo)
{
	int i;

	if (ImageInfo->file.count) {
		for (i=0; i<ImageInfo->file.count; i++) {
			EFREE_IF(ImageInfo->file.list[i].data);
		}
	}
	EFREE_IF(ImageInfo->file.list);
	ImageInfo->file.count = 0;
	return true;
}
/* }}} */

static image_info_data *exif_alloc_image_info_data(image_info_list *info_list) {
	if (info_list->count == info_list->alloc_count) {
		int new_alloc_count = info_list->alloc_count ? info_list->alloc_count * 2 : 1;
		info_list->list = safe_erealloc(
			info_list->list, new_alloc_count, sizeof(image_info_data), 0);
		info_list->alloc_count = new_alloc_count;
	}
	return &info_list->list[info_list->count++];
}

/* {{{ exif_iif_add_value
 Add a value to image_info
*/
static void exif_iif_add_value(image_info_type *image_info, int section_index, char *name, int tag, int format, int length, void* value, size_t value_len, int motorola_intel)
{
	size_t idex;
	void *vptr, *vptr_end;
	image_info_value *info_value;
	image_info_data  *info_data;

	if (length < 0) {
		return;
	}

	info_data = exif_alloc_image_info_data(&image_info->info_list[section_index]);
	memset(info_data, 0, sizeof(image_info_data));
	info_data->tag    = tag;
	info_data->format = format;
	info_data->length = length;
	info_data->name   = estrdup(name);
	info_value        = &info_data->value;

	switch (format) {
		case TAG_FMT_STRING:
			if (length > value_len) {
				exif_error_docref("exif_iif_add_value" EXIFERR_CC, image_info, E_WARNING, "length > value_len: %d > %zu", length, value_len);
				value = NULL;
			}
			if (value) {
				length = (int)zend_strnlen(value, length);
				info_value->s = estrndup(value, length);
				info_data->length = length;
			} else {
				info_data->length = 0;
				info_value->s = estrdup("");
			}
			break;

		default:
			/* Standard says more types possible but skip them...
			 * but allow users to handle data if they know how to
			 * So not return but use type UNDEFINED
			 * return;
			 */
			info_data->tag = TAG_FMT_UNDEFINED;/* otherwise not freed from memory */
			ZEND_FALLTHROUGH;
		case TAG_FMT_SBYTE:
		case TAG_FMT_BYTE:
		/* in contrast to strings bytes do not need to allocate buffer for NULL if length==0 */
			if (!length) {
				break;
			}
			ZEND_FALLTHROUGH;
		case TAG_FMT_UNDEFINED:
			if (length > value_len) {
				exif_error_docref("exif_iif_add_value" EXIFERR_CC, image_info, E_WARNING, "length > value_len: %d > %zu", length, value_len);
				value = NULL;
			}
			if (value) {
				if (tag == TAG_MAKER_NOTE) {
					length = (int) zend_strnlen(value, length);
				}

				/* do not recompute length here */
				info_value->s = estrndup(value, length);
				info_data->length = length;
			} else {
				info_data->length = 0;
				info_value->s = estrdup("");
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
				info_value->list = safe_emalloc(length, sizeof(image_info_value), 0);
			} else {
				info_value = &info_data->value;
			}
			vptr_end = (char *) value + value_len;
			for (idex=0,vptr=value; idex<(size_t)length; idex++,vptr=(char *) vptr + php_tiff_bytes_per_format[format]) {
				if ((char *) vptr_end - (char *) vptr < php_tiff_bytes_per_format[format]) {
					exif_error_docref("exif_iif_add_value" EXIFERR_CC, image_info, E_WARNING, "Value too short");
					break;
				}
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
						php_error_docref(NULL, E_WARNING, "Found value of type single");
#endif
						info_value->f = php_ifd_get_float(value);
						break;
					case TAG_FMT_DOUBLE:
#ifdef EXIF_DEBUG
						php_error_docref(NULL, E_WARNING, "Found value of type double");
#endif
						info_value->d = php_ifd_get_double(value);
						break;
				}
			}
	}
	image_info->sections_found |= 1<<section_index;
}
/* }}} */

/* {{{ exif_iif_add_tag
 Add a tag from IFD to image_info
*/
static void exif_iif_add_tag(image_info_type *image_info, int section_index, char *name, int tag, int format, size_t length, void* value, size_t value_len)
{
	exif_iif_add_value(image_info, section_index, name, tag, format, (int)length, value, value_len, image_info->motorola_intel);
}
/* }}} */

/* {{{ exif_iif_add_int
 Add an int value to image_info
*/
static void exif_iif_add_int(image_info_type *image_info, int section_index, char *name, int value)
{
	image_info_data *info_data = exif_alloc_image_info_data(&image_info->info_list[section_index]);
	info_data->tag    = TAG_NONE;
	info_data->format = TAG_FMT_SLONG;
	info_data->length = 1;
	info_data->name   = estrdup(name);
	info_data->value.i = value;
	image_info->sections_found |= 1<<section_index;
}
/* }}} */

/* {{{ exif_iif_add_str
 Add a string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_str(image_info_type *image_info, int section_index, char *name, char *value)
{
	if (value) {
		image_info_data *info_data =
			exif_alloc_image_info_data(&image_info->info_list[section_index]);
		info_data->tag    = TAG_NONE;
		info_data->format = TAG_FMT_STRING;
		info_data->length = 1;
		info_data->name   = estrdup(name);
		info_data->value.s = estrdup(value);
		image_info->sections_found |= 1<<section_index;
	}
}
/* }}} */

/* {{{ exif_iif_add_fmt
 Add a format string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_fmt(image_info_type *image_info, int section_index, char *name, char *value, ...)
{
	char             *tmp;
	va_list 		 arglist;

	va_start(arglist, value);
	if (value) {
		vspprintf(&tmp, 0, value, arglist);
		exif_iif_add_str(image_info, section_index, name, tmp);
		efree(tmp);
	}
	va_end(arglist);
}
/* }}} */

/* {{{ exif_iif_add_str
 Add a string value to image_info MUST BE NUL TERMINATED
*/
static void exif_iif_add_buffer(image_info_type *image_info, int section_index, char *name, int length, char *value)
{
	if (value) {
		image_info_data *info_data =
			exif_alloc_image_info_data(&image_info->info_list[section_index]);
		info_data->tag    = TAG_NONE;
		info_data->format = TAG_FMT_UNDEFINED;
		info_data->length = length;
		info_data->name   = estrdup(name);
		info_data->value.s = safe_emalloc(length, 1, 1);
		memcpy(info_data->value.s, value, length);
		info_data->value.s[length] = 0;
		image_info->sections_found |= 1<<section_index;
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
				case TAG_FMT_UNDEFINED:
				case TAG_FMT_STRING:
				case TAG_FMT_SBYTE:
				case TAG_FMT_BYTE:
				default:
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
static void add_assoc_image_info(zval *value, int sub_array, image_info_type *image_info, int section_index)
{
	char buffer[64], uname[64];
	int idx = 0, unknown = 0;

	if (!image_info->info_list[section_index].count) {
		return;
	}

	zval tmpi;
	if (sub_array) {
		array_init(&tmpi);
	} else {
		ZVAL_COPY_VALUE(&tmpi, value);
	}

	for (int i = 0; i<image_info->info_list[section_index].count; i++) {
		image_info_data *info_data = &image_info->info_list[section_index].list[i];
		image_info_value *info_value = &info_data->value;
		const char *name = info_data->name;
		if (!name) {
			snprintf(uname, sizeof(uname), "%d", unknown++);
			name = uname;
		}

		if (info_data->length == 0) {
			add_assoc_null(&tmpi, name);
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
						add_assoc_stringl(&tmpi, name, "", 0);
					} else {
						add_assoc_stringl(&tmpi, name, info_value->s, info_data->length);
					}
					break;

				case TAG_FMT_STRING: {
					const char *val = info_value->s ? info_value->s : "";
					if (section_index==SECTION_COMMENT) {
						add_index_string(&tmpi, idx++, val);
					} else {
						add_assoc_string(&tmpi, name, val);
					}
					break;
				}

				case TAG_FMT_URATIONAL:
				case TAG_FMT_SRATIONAL:
				case TAG_FMT_USHORT:
				case TAG_FMT_SSHORT:
				case TAG_FMT_SINGLE:
				case TAG_FMT_DOUBLE:
				case TAG_FMT_ULONG:
				case TAG_FMT_SLONG: {
					/* now the rest, first see if it becomes an array */
					zval array;
					int l = info_data->length;
					if (l > 1) {
						array_init(&array);
					}
					for (int ap = 0; ap < l; ap++) {
						if (l>1) {
							info_value = &info_data->value.list[ap];
						}
						switch (info_data->format) {
							case TAG_FMT_BYTE:
								if (l>1) {
									info_value = &info_data->value;
									for (int b = 0; b < l; b++) {
										add_index_long(&array, b, (int)(info_value->s[b]));
									}
									break;
								}
								ZEND_FALLTHROUGH;
							case TAG_FMT_USHORT:
							case TAG_FMT_ULONG:
								if (l==1) {
									add_assoc_long(&tmpi, name, (int)info_value->u);
								} else {
									add_index_long(&array, ap, (int)info_value->u);
								}
								break;

							case TAG_FMT_URATIONAL:
								snprintf(buffer, sizeof(buffer), "%u/%u", info_value->ur.num, info_value->ur.den);
								if (l==1) {
									add_assoc_string(&tmpi, name, buffer);
								} else {
									add_index_string(&array, ap, buffer);
								}
								break;

							case TAG_FMT_SBYTE:
								if (l>1) {
									info_value = &info_data->value;
									for (int b = 0; b < l; b++) {
										add_index_long(&array, ap, (int)info_value->s[b]);
									}
									break;
								}
								ZEND_FALLTHROUGH;
							case TAG_FMT_SSHORT:
							case TAG_FMT_SLONG:
								if (l==1) {
									add_assoc_long(&tmpi, name, info_value->i);
								} else {
									add_index_long(&array, ap, info_value->i);
								}
								break;

							case TAG_FMT_SRATIONAL:
								snprintf(buffer, sizeof(buffer), "%i/%i", info_value->sr.num, info_value->sr.den);
								if (l==1) {
									add_assoc_string(&tmpi, name, buffer);
								} else {
									add_index_string(&array, ap, buffer);
								}
								break;

							case TAG_FMT_SINGLE:
								if (l==1) {
									add_assoc_double(&tmpi, name, info_value->f);
								} else {
									add_index_double(&array, ap, info_value->f);
								}
								break;

							case TAG_FMT_DOUBLE:
								if (l==1) {
									add_assoc_double(&tmpi, name, info_value->d);
								} else {
									add_index_double(&array, ap, info_value->d);
								}
								break;
						}
					}
					if (l > 1) {
						add_assoc_zval(&tmpi, name, &array);
					}
					break;
				}
			}
		}
	}
	if (sub_array) {
		add_assoc_zval(value, exif_get_sectionname(section_index), &tmpi);
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

/* {{{ exif_process_COM
   Process a COM marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
static void exif_process_COM (image_info_type *image_info, char *value, size_t length)
{
	exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_STRING, length-2, value+2, length-2);
}
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
}
/* }}} */

/* forward declarations */
static bool exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, const exif_offset_info *info, size_t displacement, int section_index, int tag);
static bool exif_process_IFD_TAG(image_info_type *ImageInfo, char *dir_entry, const exif_offset_info *info, size_t displacement, int section_index, int ReadNextIFD, tag_table_type tag_table);
static bool exif_process_IFD_in_TIFF(image_info_type *ImageInfo, size_t dir_offset, int section_index);

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

/* {{{ Get headername for index or false if not defined */
PHP_FUNCTION(exif_tagname)
{
	zend_long tag;
	char *szTemp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &tag) == FAILURE) {
		RETURN_THROWS();
	}

	szTemp = exif_get_tagname(tag, tag_table_IFD);
	if (tag < 0 || !szTemp) {
		RETURN_FALSE;
	}

	RETURN_STRING(szTemp);
}
/* }}} */

/* {{{ exif_ifd_make_value
 * Create a value for an ifd from an info_data pointer */
static void* exif_ifd_make_value(image_info_data *info_data, int motorola_intel) {
	size_t  byte_count;
	char    *value_ptr, *data_ptr;
	size_t  i;

	image_info_value  *info_value;

	byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
	value_ptr = safe_emalloc(max(byte_count, 4), 1, 0);
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
					memmove(data_ptr, &info_value->f, 4);
					data_ptr += 4;
					break;
				case TAG_FMT_DOUBLE:
					memmove(data_ptr, &info_value->d, 8);
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
static void exif_thumbnail_build(image_info_type *ImageInfo) {
	size_t            new_size, new_move, new_value;
	char              *new_data;
	void              *value_ptr;
	int               i, byte_count;
	image_info_list   *info_list;
	image_info_data   *info_data;

	if (!ImageInfo->read_thumbnail || !ImageInfo->Thumbnail.offset || !ImageInfo->Thumbnail.size) {
		return; /* ignore this call */
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: filetype = %d", ImageInfo->Thumbnail.filetype);
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
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: size of signature + directory(%d): 0x%02X", info_list->count, new_size);
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
			new_data = safe_erealloc(ImageInfo->Thumbnail.data, 1, ImageInfo->Thumbnail.size, new_size);
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
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: process tag(x%04X=%s): %s%s (%d bytes)", info_data->tag, exif_get_tagname_debug(info_data->tag, tag_table_IFD), (info_data->length>1)&&info_data->format!=TAG_FMT_UNDEFINED&&info_data->format!=TAG_FMT_STRING?"ARRAY OF ":"", exif_get_tagformat(info_data->format), byte_count);
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
					value_ptr  = exif_ifd_make_value(info_data, ImageInfo->motorola_intel);
					if (byte_count <= 4) {
						memmove(new_data+8, value_ptr, 4);
					} else {
						php_ifd_set32u(new_data+8, new_value, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: writing with value offset: 0x%04X + 0x%02X", new_value, byte_count);
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
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: created");
#endif
			break;
	}
}
/* }}} */

/* {{{ exif_thumbnail_extract
 * Grab the thumbnail, corrected */
static void exif_thumbnail_extract(image_info_type *ImageInfo, const exif_offset_info *info) {
	if (ImageInfo->Thumbnail.data) {
		exif_error_docref("exif_read_data#error_mult_thumb" EXIFERR_CC, ImageInfo, E_WARNING, "Multiple possible thumbnails");
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
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Illegal thumbnail size/offset");
		return;
	}
	/* Check to make sure we are not going to go past the ExifLength */
	char *thumbnail = exif_offset_info_try_get(
		info, ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
	if (!thumbnail) {
		EXIF_ERRLOG_THUMBEOF(ImageInfo)
		return;
	}
	ImageInfo->Thumbnail.data = estrndup(thumbnail, ImageInfo->Thumbnail.size);
	exif_thumbnail_build(ImageInfo);
}
/* }}} */

/* {{{ exif_process_undefined
 * Copy a string/buffer in Exif header to a character string and return length of allocated buffer if any. */
static int exif_process_undefined(char **result, char *value, size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 * estrndup does not return length
	 */
	if (byte_count) {
		(*result) = estrndup(value, byte_count); /* NULL @ byte_count!!! */
		return byte_count+1;
	}
	return 0;
}
/* }}} */

/* {{{ exif_process_string_raw
 * Copy a string in Exif header to a character string returns length of allocated buffer if any. */
static int exif_process_string_raw(char **result, char *value, size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 */
	if (byte_count) {
		(*result) = safe_emalloc(byte_count, 1, 1);
		memcpy(*result, value, byte_count);
		(*result)[byte_count] = '\0';
		return byte_count+1;
	}
	return 0;
}
/* }}} */

/* {{{ exif_process_string
 * Copy a string in Exif header to a character string and return length of allocated buffer if any.
 * In contrast to exif_process_string this function does always return a string buffer */
static int exif_process_string(char **result, char *value, size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we cannot use strlen to
	 * determine length of string and we cannot use strlcpy with len=byte_count+1
	 * because then we might get into an EXCEPTION if we exceed an allocated
	 * memory page...so we use zend_strnlen in conjunction with memcpy and add the NUL
	 * char.
	 * estrdup would sometimes allocate more memory and does not return length
	 */
	if ((byte_count=zend_strnlen(value, byte_count)) > 0) {
		return exif_process_undefined(result, value, byte_count);
	}
	(*result) = estrndup("", 1); /* force empty string */
	return byte_count+1;
}
/* }}} */

/* {{{ exif_process_user_comment
 * Process UserComment in IFD. */
static int exif_process_user_comment(image_info_type *ImageInfo, char **pszInfoPtr, char **pszEncoding, char *szValuePtr, int ByteCount)
{
	int   a;
	char  *decode;
	size_t len;

	*pszEncoding = NULL;
	/* Copy the comment */
	if (ByteCount>=8) {
		const zend_encoding *from, *to;
		if (!memcmp(szValuePtr, "UNICODE\0", 8)) {
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
			/* First try to detect BOM: ZERO WIDTH NOBREAK SPACE (FEFF 16)
			 * since we have no encoding support for the BOM yet we skip that.
			 */
			if (ByteCount >=2 && !memcmp(szValuePtr, "\xFE\xFF", 2)) {
				decode = "UCS-2BE";
				szValuePtr = szValuePtr+2;
				ByteCount -= 2;
			} else if (ByteCount >=2 && !memcmp(szValuePtr, "\xFF\xFE", 2)) {
				decode = "UCS-2LE";
				szValuePtr = szValuePtr+2;
				ByteCount -= 2;
			} else if (ImageInfo->motorola_intel) {
				decode = ImageInfo->decode_unicode_be;
			} else {
				decode = ImageInfo->decode_unicode_le;
			}
			to = zend_multibyte_fetch_encoding(ImageInfo->encode_unicode);
			from = zend_multibyte_fetch_encoding(decode);
			/* XXX this will fail again if encoding_converter returns on error something different than SIZE_MAX   */
			if (!to || !from || zend_multibyte_encoding_converter(
					(unsigned char**)pszInfoPtr,
					&len,
					(unsigned char*)szValuePtr,
					ByteCount,
					to,
					from) == (size_t)-1) {
				len = exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
			}
			return len;
		} else if (!memcmp(szValuePtr, "ASCII\0\0\0", 8)) {
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
		} else if (!memcmp(szValuePtr, "JIS\0\0\0\0\0", 8)) {
			/* JIS should be translated to MB or we leave it to the user - leave it to the user */
			*pszEncoding = estrdup((const char*)szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
			/* XXX this will fail again if encoding_converter returns on error something different than SIZE_MAX   */
			to = zend_multibyte_fetch_encoding(ImageInfo->encode_jis);
			from = zend_multibyte_fetch_encoding(ImageInfo->motorola_intel ? ImageInfo->decode_jis_be : ImageInfo->decode_jis_le);
			if (!to || !from || zend_multibyte_encoding_converter(
					(unsigned char**)pszInfoPtr,
					&len,
					(unsigned char*)szValuePtr,
					ByteCount,
					to,
					from) == (size_t)-1) {
				len = exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
			}
			return len;
		} else if (!memcmp(szValuePtr, "\0\0\0\0\0\0\0\0", 8)) {
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
	exif_process_string(pszInfoPtr, szValuePtr, ByteCount);
	return strlen(*pszInfoPtr);
}
/* }}} */

/* {{{ exif_process_unicode
 * Process unicode field in IFD. */
static int exif_process_unicode(image_info_type *ImageInfo, xp_field_type *xp_field, int tag, char *szValuePtr, int ByteCount)
{
	xp_field->tag = tag;
	xp_field->value = NULL;
	/* XXX this will fail again if encoding_converter returns on error something different than SIZE_MAX   */
	if (zend_multibyte_encoding_converter(
			(unsigned char**)&xp_field->value,
			&xp_field->size,
			(unsigned char*)szValuePtr,
			ByteCount,
			zend_multibyte_fetch_encoding(ImageInfo->encode_unicode),
			zend_multibyte_fetch_encoding(ImageInfo->motorola_intel ? ImageInfo->decode_unicode_be : ImageInfo->decode_unicode_le)
			) == (size_t)-1) {
		xp_field->size = exif_process_string_raw(&xp_field->value, szValuePtr, ByteCount);
	}
	return xp_field->size;
}
/* }}} */

/* {{{ exif_process_IFD_in_MAKERNOTE
 * Process nested IFDs directories in Maker Note. */
static bool exif_process_IFD_in_MAKERNOTE(image_info_type *ImageInfo, char * value_ptr, int value_len, const exif_offset_info *info, size_t displacement)
{
	size_t i;
	int de, section_index = SECTION_MAKERNOTE;
	int NumDirEntries, old_motorola_intel;
	const maker_note_type *maker_note;
	char *dir_start;
	exif_offset_info new_info;

	for (i=0; i<=sizeof(maker_note_array)/sizeof(maker_note_type); i++) {
		if (i==sizeof(maker_note_array)/sizeof(maker_note_type)) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "No maker note data found. Detected maker: %s (length = %d)", ImageInfo->make, ImageInfo->make ? strlen(ImageInfo->make) : 0);
#endif
			/* unknown manufacturer, not an error, use it as a string */
			return true;
		}

		maker_note = maker_note_array+i;

		if (maker_note->make && (!ImageInfo->make || strcmp(maker_note->make, ImageInfo->make)))
			continue;
		if (maker_note->id_string && value_len >= maker_note->id_string_len
				&& strncmp(maker_note->id_string, value_ptr, maker_note->id_string_len))
			continue;
		break;
	}

	if (value_len < 2 || maker_note->offset >= value_len - 1) {
		/* Do not go past the value end */
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "IFD data too short: 0x%04X offset 0x%04X", value_len, maker_note->offset);
		return true;
	}

	if (UNEXPECTED(maker_note->tag_table == REQUIRES_CUSTOM_PARSING)) {
		/* Custom parsing required, which is not implemented at this point
		 * Return true so that other metadata can still be parsed. */
		return true;
	}

	dir_start = value_ptr + maker_note->offset;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process %s @0x%04X + 0x%04X=%d: %s", exif_get_sectionname(section_index), (intptr_t)dir_start-(intptr_t)info->offset_base+maker_note->offset+displacement, value_len, value_len, exif_char_dump(value_ptr, value_len, (intptr_t)dir_start-(intptr_t)info->offset_base+maker_note->offset+displacement));
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

	/* It can be that motorola_intel is wrongly mapped, let's try inverting it */
	if ((2+NumDirEntries*12) > value_len) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Potentially invalid endianess, trying again with different endianness before imminent failure.");

		ImageInfo->motorola_intel = ImageInfo->motorola_intel == 0 ? 1 : 0;
		NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);
	}

	if ((2+NumDirEntries*12) > value_len) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size: 2 + 0x%04X*12 = 0x%04X > 0x%04X", NumDirEntries, 2+NumDirEntries*12, value_len);
		return false;
	}
	if ((dir_start - value_ptr) > value_len - (2+NumDirEntries*12)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size: 0x%04X > 0x%04X", (dir_start - value_ptr) + (2+NumDirEntries*12), value_len);
		return false;
	}

	switch (maker_note->offset_mode) {
		case MN_OFFSET_MAKER:
			exif_offset_info_init(&new_info, value_ptr, value_ptr, value_len);
			info = &new_info;
			break;
		default:
		case MN_OFFSET_NORMAL:
			break;
	}

	for (de=0;de<NumDirEntries;de++) {
		size_t offset = 2 + 12 * de;
		if (!exif_process_IFD_TAG(ImageInfo, dir_start + offset,
								  info, displacement, section_index, 0, maker_note->tag_table)) {
			return false;
		}
	}
	ImageInfo->motorola_intel = old_motorola_intel;
/*	NextDirOffset (must be NULL) = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);*/
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Subsection %s done", exif_get_sectionname(SECTION_MAKERNOTE));
#endif
	return true;
}
/* }}} */

#define REQUIRE_NON_EMPTY() do { \
	if (byte_count == 0) { \
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Cannot be empty", tag, exif_get_tagname_debug(tag, tag_table)); \
		return false; \
	} \
} while (0)


/* {{{ exif_process_IFD_TAG
 * Process one of the nested IFDs directories. */
static bool exif_process_IFD_TAG_impl(image_info_type *ImageInfo, char *dir_entry, const exif_offset_info *info, size_t displacement, int section_index, int ReadNextIFD, tag_table_type tag_table)
{
	size_t length;
	unsigned int tag, format, components;
	char *value_ptr, tagname[64], cbuf[32], *outside=NULL;
	size_t byte_count, offset_val, fpos, fgot;
	int64_t byte_count_signed;
	xp_field_type *tmp_xp;
#ifdef EXIF_DEBUG
	char *dump_data;
	int dump_free;
#endif /* EXIF_DEBUG */

	tag = php_ifd_get16u(dir_entry, ImageInfo->motorola_intel);
	format = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
	components = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);

	if (!format || format > NUM_FORMATS) {
		/* (-1) catches illegal zero case as unsigned underflows to positive large. */
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal format code 0x%04X, suppose BYTE", tag, exif_get_tagname_debug(tag, tag_table), format);
		format = TAG_FMT_BYTE;
	}

	byte_count_signed = (int64_t)components * php_tiff_bytes_per_format[format];

	if (byte_count_signed < 0 || (byte_count_signed > INT32_MAX)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal byte_count", tag, exif_get_tagname_debug(tag, tag_table));
		return false;
	}

	byte_count = (size_t)byte_count_signed;

	if (byte_count > 4) {
		/* If its bigger than 4 bytes, the dir entry contains an offset. */
		offset_val = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
		value_ptr = exif_offset_info_try_get(info, offset_val, byte_count);
		if (!value_ptr) {
			/* It is important to check for IMAGE_FILETYPE_TIFF
			 * JPEG does not use absolute pointers instead its pointers are
			 * relative to the start of the TIFF header in APP1 section. */
			// TODO: Shouldn't we also be taking "displacement" into account here?
			if (byte_count > ImageInfo->FileSize || offset_val>ImageInfo->FileSize-byte_count || (ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_II && ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_MM && ImageInfo->FileType!=IMAGE_FILETYPE_JPEG)) {
				exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Process tag(x%04X=%s): Illegal pointer offset(x%04X + x%04X = x%04X > x%04X)", tag, exif_get_tagname_debug(tag, tag_table), offset_val, byte_count, offset_val+byte_count, ImageInfo->FileSize);
				return false;
			}
			if (byte_count>sizeof(cbuf)) {
				/* mark as outside range and get buffer */
				value_ptr = safe_emalloc(byte_count, 1, 0);
				outside = value_ptr;
			} else {
				/* In most cases we only access a small range so
				 * it is faster to use a static buffer there
				 * BUT it offers also the possibility to have
				 * pointers read without the need to free them
				 * explicitly before returning. */
				memset(&cbuf, 0, sizeof(cbuf));
				value_ptr = cbuf;
			}

			fpos = php_stream_tell(ImageInfo->infile);
			php_stream_seek(ImageInfo->infile, displacement+offset_val, SEEK_SET);
			fgot = php_stream_tell(ImageInfo->infile);
			if (fgot!=displacement+offset_val) {
				EFREE_IF(outside);
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Wrong file pointer: 0x%08X != 0x%08X", fgot, displacement+offset_val);
				return false;
			}
			fgot = exif_read_from_stream_file_looped(ImageInfo->infile, value_ptr, byte_count);
			php_stream_seek(ImageInfo->infile, fpos, SEEK_SET);
			if (fgot != byte_count) {
				EFREE_IF(outside);
				EXIF_ERRLOG_FILEEOF(ImageInfo)
				return false;
			}
		}
	} else {
		/* 4 bytes or less and value is in the dir entry itself */
		value_ptr = dir_entry+8;
		// TODO: This is dubious, but the value is only used for debugging.
		offset_val = value_ptr-info->offset_base;
	}

	ImageInfo->sections_found |= FOUND_ANY_TAG;
#ifdef EXIF_DEBUG
	dump_data = exif_dump_data(&dump_free, format, components, ImageInfo->motorola_intel, value_ptr);
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE,
		"Process tag(x%04X=%s,@0x%04X + x%04X(=%d)): %s%s %s",
		tag, exif_get_tagname_debug(tag, tag_table), offset_val+displacement, byte_count, byte_count, (components>1)&&format!=TAG_FMT_UNDEFINED&&format!=TAG_FMT_STRING?"ARRAY OF ":"", exif_get_tagformat(format), dump_data);
	if (dump_free) {
		efree(dump_data);
	}
#endif

	/* NB: The following code may not assume that there is at least one component!
	 * byte_count may be zero! */

	if (section_index==SECTION_THUMBNAIL) {
		if (!ImageInfo->Thumbnail.data) {
			REQUIRE_NON_EMPTY();
			switch(tag) {
				case TAG_IMAGEWIDTH:
				case TAG_COMP_IMAGE_WIDTH:
					ImageInfo->Thumbnail.width = exif_convert_any_to_int(value_ptr, exif_rewrite_tag_format_to_unsigned(format), ImageInfo->motorola_intel);
					break;

				case TAG_IMAGEHEIGHT:
				case TAG_COMP_IMAGE_HEIGHT:
					ImageInfo->Thumbnail.height = exif_convert_any_to_int(value_ptr, exif_rewrite_tag_format_to_unsigned(format), ImageInfo->motorola_intel);
					break;

				case TAG_STRIP_OFFSETS:
				case TAG_JPEG_INTERCHANGE_FORMAT:
					/* accept both formats */
					ImageInfo->Thumbnail.offset = exif_convert_any_to_int(value_ptr, exif_rewrite_tag_format_to_unsigned(format), ImageInfo->motorola_intel);
					break;

				case TAG_STRIP_BYTE_COUNTS:
					if (ImageInfo->FileType == IMAGE_FILETYPE_TIFF_II || ImageInfo->FileType == IMAGE_FILETYPE_TIFF_MM) {
						ImageInfo->Thumbnail.filetype = ImageInfo->FileType;
					} else {
						/* motorola is easier to read */
						ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_TIFF_MM;
					}
					ImageInfo->Thumbnail.size = exif_convert_any_to_int(value_ptr, exif_rewrite_tag_format_to_unsigned(format), ImageInfo->motorola_intel);
					break;

				case TAG_JPEG_INTERCHANGE_FORMAT_LEN:
					if (ImageInfo->Thumbnail.filetype == IMAGE_FILETYPE_UNKNOWN) {
						ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_JPEG;
						ImageInfo->Thumbnail.size = exif_convert_any_to_int(value_ptr, exif_rewrite_tag_format_to_unsigned(format), ImageInfo->motorola_intel);
					}
					break;
			}
		}
	} else {
		if (section_index==SECTION_IFD0 || section_index==SECTION_EXIF)
		switch(tag) {
			case TAG_COPYRIGHT:
				/* check for "<photographer> NUL <editor> NUL" */
				if (byte_count>1 && (length=zend_strnlen(value_ptr, byte_count)) > 0) {
					if (length<byte_count-1) {
						/* When there are any characters after the first NUL */
						EFREE_IF(ImageInfo->CopyrightPhotographer);
						EFREE_IF(ImageInfo->CopyrightEditor);
						EFREE_IF(ImageInfo->Copyright);
						ImageInfo->CopyrightPhotographer  = estrdup(value_ptr);
						ImageInfo->CopyrightEditor        = estrndup(value_ptr+length+1, byte_count-length-1);
						spprintf(&ImageInfo->Copyright, 0, "%s, %s", ImageInfo->CopyrightPhotographer, ImageInfo->CopyrightEditor);
						/* format = TAG_FMT_UNDEFINED; this mustn't be ASCII         */
						/* but we are not supposed to change this                   */
						/* keep in mind that image_info does not store editor value */
					} else {
						EFREE_IF(ImageInfo->Copyright);
						ImageInfo->Copyright = estrndup(value_ptr, byte_count);
					}
				}
				break;

			case TAG_USERCOMMENT:
				EFREE_IF(ImageInfo->UserComment);
				ImageInfo->UserComment = NULL;
				EFREE_IF(ImageInfo->UserCommentEncoding);
				ImageInfo->UserCommentEncoding = NULL;
				ImageInfo->UserCommentLength = exif_process_user_comment(ImageInfo, &(ImageInfo->UserComment), &(ImageInfo->UserCommentEncoding), value_ptr, byte_count);
				break;

			case TAG_XP_TITLE:
			case TAG_XP_COMMENTS:
			case TAG_XP_AUTHOR:
			case TAG_XP_KEYWORDS:
			case TAG_XP_SUBJECT:
				tmp_xp = (xp_field_type*)safe_erealloc(ImageInfo->xp_fields.list, (ImageInfo->xp_fields.count+1), sizeof(xp_field_type), 0);
				ImageInfo->sections_found |= FOUND_WINXP;
				ImageInfo->xp_fields.list = tmp_xp;
				ImageInfo->xp_fields.count++;
				exif_process_unicode(ImageInfo, &(ImageInfo->xp_fields.list[ImageInfo->xp_fields.count-1]), tag, value_ptr, byte_count);
				break;

			case TAG_FNUMBER:
				/* Simplest way of expressing aperture, so I trust it the most.
				   (overwrite previously computed value if there is one) */
				REQUIRE_NON_EMPTY();
				ImageInfo->ApertureFNumber = (float)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				break;

			case TAG_APERTURE:
			case TAG_MAX_APERTURE:
				/* More relevant info always comes earlier, so only use this field if we don't
				   have appropriate aperture information yet. */
				if (ImageInfo->ApertureFNumber == 0) {
					REQUIRE_NON_EMPTY();
					ImageInfo->ApertureFNumber
						= expf(exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel)*logf(2.0)*0.5);
				}
				break;

			case TAG_SHUTTERSPEED:
				/* More complicated way of expressing exposure time, so only use
				   this value if we don't already have it from somewhere else.
				   SHUTTERSPEED comes after EXPOSURE TIME
				  */
				if (ImageInfo->ExposureTime == 0) {
					REQUIRE_NON_EMPTY();
					ImageInfo->ExposureTime
						= expf(-exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel)*logf(2.0));
				}
				break;
			case TAG_EXPOSURETIME:
				ImageInfo->ExposureTime = -1;
				break;

			case TAG_COMP_IMAGE_WIDTH:
				REQUIRE_NON_EMPTY();
				ImageInfo->ExifImageWidth = exif_convert_any_to_int(value_ptr, exif_rewrite_tag_format_to_unsigned(format), ImageInfo->motorola_intel);
				break;

			case TAG_FOCALPLANE_X_RES:
				REQUIRE_NON_EMPTY();
				ImageInfo->FocalplaneXRes = exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				break;

			case TAG_SUBJECT_DISTANCE:
				/* Inidcates the distacne the autofocus camera is focused to.
				   Tends to be less accurate as distance increases. */
				REQUIRE_NON_EMPTY();
				ImageInfo->Distance = (float)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				break;

			case TAG_FOCALPLANE_RESOLUTION_UNIT:
				REQUIRE_NON_EMPTY();
				switch (exif_convert_any_to_int(value_ptr, format, ImageInfo->motorola_intel)) {
					case 1: ImageInfo->FocalplaneUnits = 25.4; break; /* inch */
					case 2:
						/* According to the information I was using, 2 means meters.
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
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Skip SUB IFD");
				}
				break;

			case TAG_MAKE:
				EFREE_IF(ImageInfo->make);
				ImageInfo->make = estrndup(value_ptr, byte_count);
				break;
			case TAG_MODEL:
				EFREE_IF(ImageInfo->model);
				ImageInfo->model = estrndup(value_ptr, byte_count);
				break;

			case TAG_MAKER_NOTE:
				if (!exif_process_IFD_in_MAKERNOTE(ImageInfo, value_ptr, byte_count, info, displacement)) {
					EFREE_IF(outside);
					return false;
				}
				break;

			case TAG_EXIF_IFD_POINTER:
			case TAG_GPS_IFD_POINTER:
			case TAG_INTEROP_IFD_POINTER:
				if (ReadNextIFD) {
					REQUIRE_NON_EMPTY();
					char *Subdir_start;
					int sub_section_index = 0;
					switch(tag) {
						case TAG_EXIF_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Found EXIF");
#endif
							ImageInfo->sections_found |= FOUND_EXIF;
							sub_section_index = SECTION_EXIF;
							break;
						case TAG_GPS_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Found GPS");
#endif
							ImageInfo->sections_found |= FOUND_GPS;
							sub_section_index = SECTION_GPS;
							break;
						case TAG_INTEROP_IFD_POINTER:
#ifdef EXIF_DEBUG
							exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Found INTEROPERABILITY");
#endif
							ImageInfo->sections_found |= FOUND_INTEROP;
							sub_section_index = SECTION_INTEROP;
							break;
					}
					offset_val = php_ifd_get32u(value_ptr, ImageInfo->motorola_intel);
					Subdir_start = exif_offset_info_try_get(info, offset_val, 0);
					if (!Subdir_start) {
						exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD Pointer");
						EFREE_IF(outside);
						return false;
					}
					if (!exif_process_IFD_in_JPEG(ImageInfo, Subdir_start, info, displacement, sub_section_index, tag)) {
						EFREE_IF(outside);
						return false;
					}
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Subsection %s done", exif_get_sectionname(sub_section_index));
#endif
				}
		}
	}
	exif_iif_add_tag(ImageInfo, section_index, exif_get_tagname_key(tag, tagname, sizeof(tagname), tag_table), tag, format, components, value_ptr, byte_count);
	EFREE_IF(outside);
	return true;
}
/* }}} */

static bool exif_process_IFD_TAG(image_info_type *ImageInfo, char *dir_entry, const exif_offset_info *info, size_t displacement, int section_index, int ReadNextIFD, tag_table_type tag_table)
{
	bool result;
	/* Protect against corrupt headers */
	if (ImageInfo->ifd_count++ > MAX_IFD_TAGS) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "corrupt EXIF header: maximum IFD tag count reached");
		return false;
	}
	if (ImageInfo->ifd_nesting_level > MAX_IFD_NESTING_LEVEL) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "corrupt EXIF header: maximum directory nesting level reached");
		return false;
	}
	ImageInfo->ifd_nesting_level++;
	result = exif_process_IFD_TAG_impl(ImageInfo, dir_entry, info, displacement, section_index, ReadNextIFD, tag_table);
	ImageInfo->ifd_nesting_level--;
	return result;
}

/* {{{ exif_process_IFD_in_JPEG
 * Process one of the nested IFDs directories. */
static bool exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, const exif_offset_info *info, size_t displacement, int section_index, int tag)
{
	int de;
	int NumDirEntries;
	int NextDirOffset = 0;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process %s (x%04X(=%d))", exif_get_sectionname(section_index), exif_offset_info_length(info), exif_offset_info_length(info));
#endif

	ImageInfo->sections_found |= FOUND_IFD0;

	if (!exif_offset_info_contains(info, dir_start, 2)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size");
		return false;
	}

	NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);

	if (!exif_offset_info_contains(info, dir_start+2, NumDirEntries*12)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size: x%04X + 2 + x%04X*12 = x%04X > x%04X", (int)((size_t)dir_start+2-(size_t)info->valid_start), NumDirEntries, (int)((size_t)dir_start+2+NumDirEntries*12-(size_t)info->valid_start), info->valid_end - info->valid_start);
		return false;
	}

	for (de=0;de<NumDirEntries;de++) {
		if (!exif_process_IFD_TAG(ImageInfo, dir_start + 2 + 12 * de,
								  info, displacement, section_index, 1, exif_get_tag_table(section_index))) {
			return false;
		}
	}
	/*
	 * Ignore IFD2 if it purportedly exists
	 */
	if (section_index == SECTION_THUMBNAIL) {
		return true;
	}
	/*
	 * Hack to make it process IDF1 I hope
	 * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
	 */
	if (!exif_offset_info_contains(info, dir_start+2+NumDirEntries*12, 4)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size");
		return false;
	}

	if (tag != TAG_EXIF_IFD_POINTER && tag != TAG_GPS_IFD_POINTER) {
		NextDirOffset = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);
	}

	if (NextDirOffset) {
		char *next_dir_start = exif_offset_info_try_get(info, NextDirOffset, 0);
		if (!next_dir_start) {
			exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD offset");
			return false;
		}
		/* That is the IFD for the first thumbnail */
#ifdef EXIF_DEBUG
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Expect next IFD to be thumbnail");
#endif
		if (exif_process_IFD_in_JPEG(ImageInfo, next_dir_start, info, displacement, SECTION_THUMBNAIL, 0)) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail size: 0x%04X", ImageInfo->Thumbnail.size);
#endif
			if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN
			&&  ImageInfo->Thumbnail.size
			&&  ImageInfo->Thumbnail.offset
			&&  ImageInfo->read_thumbnail
			) {
				exif_thumbnail_extract(ImageInfo, info);
			}
			return true;
		} else {
			return false;
		}
	}
	return true;
}
/* }}} */

/* {{{ exif_process_TIFF_in_JPEG
   Process a TIFF header in a JPEG file
*/
static void exif_process_TIFF_in_JPEG(image_info_type *ImageInfo, char *CharBuf, size_t length, size_t displacement)
{
	unsigned exif_value_2a, offset_of_ifd;
	exif_offset_info info;

	if (length < 2) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Missing TIFF alignment marker");
		return;
	}

	/* set the thumbnail stuff to nothing so we can test to see if they get set up */
	if (memcmp(CharBuf, "II", 2) == 0) {
		ImageInfo->motorola_intel = 0;
	} else if (memcmp(CharBuf, "MM", 2) == 0) {
		ImageInfo->motorola_intel = 1;
	} else {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF alignment marker");
		return;
	}

	/* Check the next two values for correctness. */
	if (length < 8) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF start (1)");
		return;
	}
	exif_value_2a = php_ifd_get16u(CharBuf+2, ImageInfo->motorola_intel);
	offset_of_ifd = php_ifd_get32u(CharBuf+4, ImageInfo->motorola_intel);
	if (exif_value_2a != 0x2a || offset_of_ifd < 0x08) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF start (1)");
		return;
	}
	if (offset_of_ifd > length) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid IFD start");
		return;
	}

	ImageInfo->sections_found |= FOUND_IFD0;
	/* First directory starts at offset 8. Offsets starts at 0. */
	exif_offset_info_init(&info, CharBuf, CharBuf, length/*-14*/);
	exif_process_IFD_in_JPEG(ImageInfo, CharBuf+offset_of_ifd, &info, displacement, SECTION_IFD0, 0);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process TIFF in JPEG done");
#endif

	/* Compute the CCD width, in millimeters. */
	if (ImageInfo->FocalplaneXRes != 0) {
		ImageInfo->CCDWidth = (float)(ImageInfo->ExifImageWidth * ImageInfo->FocalplaneUnits / ImageInfo->FocalplaneXRes);
	}
}
/* }}} */

/* {{{ exif_process_APP1
   Process an JPEG APP1 block marker
   Describes all the drivel that most digital cameras include...
*/
static void exif_process_APP1(image_info_type *ImageInfo, char *CharBuf, size_t length, size_t displacement)
{
	/* Check the APP1 for Exif Identifier Code */
	static const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
	if (length <= 8 || memcmp(CharBuf+2, ExifHeader, 6)) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Incorrect APP1 Exif Identifier Code");
		return;
	}
	exif_process_TIFF_in_JPEG(ImageInfo, CharBuf + 8, length - 8, displacement+8);
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process APP1/EXIF done");
#endif
}
/* }}} */

/* {{{ exif_process_APP12
   Process an JPEG APP12 block marker used by OLYMPUS
*/
static void exif_process_APP12(image_info_type *ImageInfo, char *buffer, size_t length)
{
	size_t l1, l2=0;

	if ((l1 = zend_strnlen(buffer+2, length-2)) > 0) {
		exif_iif_add_tag(ImageInfo, SECTION_APP12, "Company", TAG_NONE, TAG_FMT_STRING, l1, buffer+2, l1);
		if (length > 2+l1+1) {
			l2 = zend_strnlen(buffer+2+l1+1, length-2-l1-1);
			exif_iif_add_tag(ImageInfo, SECTION_APP12, "Info", TAG_NONE, TAG_FMT_STRING, l2, buffer+2+l1+1, l2);
		}
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process section APP12 with l1=%d, l2=%d done", l1, l2);
#endif
}
/* }}} */

/* {{{ exif_scan_JPEG_header
 * Parse the marker stream until SOS or EOI is seen; */
static bool exif_scan_JPEG_header(image_info_type *ImageInfo)
{
	int sn;
	int marker = 0, last_marker = M_PSEUDO, comment_correction=1;
	unsigned int ll, lh;
	uchar *Data;
	size_t fpos, size, got, itemlen;
	jpeg_sof_info sof_info;

	while (true) {
#ifdef EXIF_DEBUG
		fpos = php_stream_tell(ImageInfo->infile);
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Needing section %d @ 0x%08X", ImageInfo->file.count, fpos);
#endif

		/* get marker byte, swallowing possible padding                           */
		/* some software does not count the length bytes of COM section           */
		/* one company doing so is very much involved in JPEG... so we accept too */
		if (last_marker==M_COM && comment_correction) {
			comment_correction = 2;
		}
		do {
			if ((marker = php_stream_getc(ImageInfo->infile)) == EOF) {
				EXIF_ERRLOG_CORRUPT(ImageInfo)
				return false;
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
			exif_error_docref("exif_read_data#error_mcom" EXIFERR_CC, ImageInfo, E_NOTICE, "Image has corrupt COM section: some software set wrong length information");
		}
		if (last_marker==M_COM && comment_correction)
			return M_EOI; /* ah illegal: char after COM section not 0xFF */

		fpos = php_stream_tell(ImageInfo->infile);

		/* safety net in case the above algorithm change dramatically, should not trigger */
		ZEND_ASSERT(marker != 0xff);

		/* Read the length of the section. */
		if ((lh = php_stream_getc(ImageInfo->infile)) == (unsigned int)EOF) {
			EXIF_ERRLOG_CORRUPT(ImageInfo)
			return false;
		}
		if ((ll = php_stream_getc(ImageInfo->infile)) == (unsigned int)EOF) {
			EXIF_ERRLOG_CORRUPT(ImageInfo)
			return false;
		}

		itemlen = (lh << 8) | ll;

		if (itemlen < 2) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "%s, Section length: 0x%02X%02X", EXIF_ERROR_CORRUPT, lh, ll);
#else
			EXIF_ERRLOG_CORRUPT(ImageInfo)
#endif
			return false;
		}

		sn = exif_file_sections_add(ImageInfo, marker, itemlen, NULL);
		Data = ImageInfo->file.list[sn].data;

		/* Store first two pre-read bytes. */
		Data[0] = (uchar)lh;
		Data[1] = (uchar)ll;

		got = exif_read_from_stream_file_looped(ImageInfo->infile, (char*)(Data+2), itemlen-2); /* Read the whole section. */
		if (got != itemlen-2) {
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error reading from file: got=x%04X(=%d) != itemlen-2=x%04X(=%d)", got, got, itemlen-2, itemlen-2);
			return false;
		}

#ifdef EXIF_DEBUG
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process section(x%02X=%s) @ x%04X + x%04X(=%d)", marker, exif_get_markername(marker), fpos, itemlen, itemlen);
#endif
		switch(marker) {
			case M_SOS:   /* stop before hitting compressed data  */
				/* If reading entire image is requested, read the rest of the data. */
				if (ImageInfo->read_all) {
					/* Determine how much file is left. */
					fpos = php_stream_tell(ImageInfo->infile);
					size = ImageInfo->FileSize - fpos;
					sn = exif_file_sections_add(ImageInfo, M_PSEUDO, size, NULL);
					Data = ImageInfo->file.list[sn].data;
					got = exif_read_from_stream_file_looped(ImageInfo->infile, (char*)Data, size);
					if (got != size) {
						EXIF_ERRLOG_FILEEOF(ImageInfo)
						return false;
					}
				}
				return true;

			case M_EOI:   /* in case it's a tables-only JPEG stream */
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "No image in jpeg!");
				return (ImageInfo->sections_found&(~FOUND_COMPUTED)) ? true : false;

			case M_COM: /* Comment section */
				exif_process_COM(ImageInfo, (char *)Data, itemlen);
				break;

			case M_EXIF:
				if (!(ImageInfo->sections_found&FOUND_IFD0)) {
					/*ImageInfo->sections_found |= FOUND_EXIF;*/
					/* Seen files from some 'U-lead' software with Vivitar scanner
					   that uses marker 31 later in the file (no clue what for!) */
					exif_process_APP1(ImageInfo, (char *)Data, itemlen, fpos);
				}
				break;

			case M_APP12:
				exif_process_APP12(ImageInfo, (char *)Data, itemlen);
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
				if ((itemlen - 2) < 6) {
					return false;
				}

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
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Done");
#endif
	return true;
}
/* }}} */

/* {{{ exif_scan_thumbnail
 * scan JPEG in thumbnail (memory) */
static bool exif_scan_thumbnail(image_info_type *ImageInfo)
{
	uchar           c, *data = (uchar*)ImageInfo->Thumbnail.data;
	int             n, marker;
	size_t          length=2, pos=0;
	jpeg_sof_info   sof_info;

	if (!data || ImageInfo->Thumbnail.size < 4) {
		return false; /* nothing to do here */
	}
	if (memcmp(data, "\xFF\xD8\xFF", 3)) {
		if (!ImageInfo->Thumbnail.width && !ImageInfo->Thumbnail.height) {
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Thumbnail is not a JPEG image");
		}
		return false;
	}
	for (;;) {
		pos += length;
		if (pos>=ImageInfo->Thumbnail.size)
			return false;
		c = data[pos++];
		if (pos>=ImageInfo->Thumbnail.size)
			return false;
		if (c != 0xFF) {
			return false;
		}
		n = 8;
		while ((c = data[pos++]) == 0xFF && n--) {
			if (pos+3>=ImageInfo->Thumbnail.size)
				return false;
			/* +3 = pos++ of next check when reaching marker + 2 bytes for length */
		}
		if (c == 0xFF)
			return false;
		marker = c;
		if (pos>=ImageInfo->Thumbnail.size)
			return false;
		length = php_jpg_get16(data+pos);
		if (length > ImageInfo->Thumbnail.size || pos >= ImageInfo->Thumbnail.size - length) {
			return false;
		}
#ifdef EXIF_DEBUG
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: process section(x%02X=%s) @ x%04X + x%04X", marker, exif_get_markername(marker), pos, length);
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
				if (length < 8 || ImageInfo->Thumbnail.size - 8 < pos) {
					/* exif_process_SOFn needs 8 bytes */
					return false;
				}
				exif_process_SOFn(data+pos, marker, &sof_info);
				ImageInfo->Thumbnail.height   = sof_info.height;
				ImageInfo->Thumbnail.width    = sof_info.width;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: size: %d * %d", sof_info.width, sof_info.height);
#endif
				return true;

			case M_SOS:
			case M_EOI:
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Could not compute size of thumbnail");
				return false;
				break;

			default:
				/* just skip */
				break;
		}
	}

	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Could not compute size of thumbnail");
	return false;
}
/* }}} */

/* {{{ exif_process_IFD_in_TIFF
 * Parse the TIFF header; */
static bool exif_process_IFD_in_TIFF_impl(image_info_type *ImageInfo, size_t dir_offset, int section_index)
{
	int i, sn, num_entries, sub_section_index = 0;
	unsigned char *dir_entry;
	size_t ifd_size, dir_size, entry_offset, next_offset, entry_length, entry_value=0, fgot;
	int entry_tag , entry_type;
	tag_table_type tag_table = exif_get_tag_table(section_index);

	if (ImageInfo->FileSize >= 2 && ImageInfo->FileSize - 2 >= dir_offset) {
		sn = exif_file_sections_add(ImageInfo, M_PSEUDO, 2, NULL);
#ifdef EXIF_DEBUG
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, 2);
#endif
		php_stream_seek(ImageInfo->infile, dir_offset, SEEK_SET); /* we do not know the order of sections */
		if (UNEXPECTED(exif_read_from_stream_file_looped(ImageInfo->infile, (char*)ImageInfo->file.list[sn].data, 2) != 2)) {
			return false;
		}
		num_entries = php_ifd_get16u(ImageInfo->file.list[sn].data, ImageInfo->motorola_intel);
		dir_size = 2/*num dir entries*/ +12/*length of entry*/*(size_t)num_entries +4/* offset to next ifd (points to thumbnail or NULL)*/;
		if (ImageInfo->FileSize >= dir_size && ImageInfo->FileSize - dir_size >= dir_offset) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X), IFD entries(%d)", ImageInfo->FileSize, dir_offset+2, dir_size-2, num_entries);
#endif
			if (exif_file_sections_realloc(ImageInfo, sn, dir_size)) {
				return false;
			}
			if (UNEXPECTED(exif_read_from_stream_file_looped(ImageInfo->infile, (char*)(ImageInfo->file.list[sn].data+2), dir_size-2) != dir_size - 2)) {
				return false;
			}
			next_offset = php_ifd_get32u(ImageInfo->file.list[sn].data + dir_size - 4, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF done, next offset x%04X", next_offset);
#endif
			/* now we have the directory we can look how long it should be */
			ifd_size = dir_size;
			for(i=0;i<num_entries;i++) {
				dir_entry 	 = ImageInfo->file.list[sn].data+2+i*12;
				entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
				entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
				if (entry_type > NUM_FORMATS) {
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: tag(0x%04X,%12s): Illegal format code 0x%04X, switching to BYTE", entry_tag, exif_get_tagname_debug(entry_tag, tag_table), entry_type);
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
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Resize struct: x%04X + x%04X - x%04X = x%04X", entry_offset, entry_length, dir_offset, ifd_size);
#endif
					}
				}
			}
			if (ImageInfo->FileSize >= ImageInfo->file.list[sn].size && ImageInfo->FileSize - ImageInfo->file.list[sn].size >= dir_offset) {
				if (ifd_size > dir_size) {
					if (ImageInfo->FileSize < ifd_size || dir_offset > ImageInfo->FileSize - ifd_size) {
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than size of IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
						return false;
					}
					if (exif_file_sections_realloc(ImageInfo, sn, ifd_size)) {
						return false;
					}
					/* read values not stored in directory itself */
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: filesize(x%04X), IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
#endif
					exif_read_from_stream_file_looped(ImageInfo->infile, (char*)(ImageInfo->file.list[sn].data+dir_size), ifd_size-dir_size);
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF, done");
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
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Next IFD: %s @0x%04X", exif_get_sectionname(sub_section_index), entry_offset);
#endif
						exif_process_IFD_in_TIFF(ImageInfo, entry_offset, sub_section_index);
						if (section_index!=SECTION_THUMBNAIL && entry_tag==TAG_SUB_IFD) {
							if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN
							&&  ImageInfo->Thumbnail.size
							&&  ImageInfo->Thumbnail.offset
							&&  ImageInfo->read_thumbnail
							) {
#ifdef EXIF_DEBUG
								exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "%s THUMBNAIL @0x%04X + 0x%04X", ImageInfo->Thumbnail.data ? "Ignore" : "Read", ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
#endif
								if (!ImageInfo->Thumbnail.data) {
									ImageInfo->Thumbnail.data = safe_emalloc(ImageInfo->Thumbnail.size, 1, 0);
									php_stream_seek(ImageInfo->infile, ImageInfo->Thumbnail.offset, SEEK_SET);
									fgot = exif_read_from_stream_file_looped(ImageInfo->infile, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
									if (fgot != ImageInfo->Thumbnail.size) {
										EXIF_ERRLOG_THUMBEOF(ImageInfo)
										efree(ImageInfo->Thumbnail.data);

										ImageInfo->Thumbnail.data = NULL;
									} else {
										exif_thumbnail_build(ImageInfo);
									}
								}
							}
						}
#ifdef EXIF_DEBUG
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Next IFD: %s done", exif_get_sectionname(sub_section_index));
#endif
					} else {
						exif_offset_info info;
						exif_offset_info_init(&info,
							(char *) (ImageInfo->file.list[sn].data - dir_offset),
							(char *) ImageInfo->file.list[sn].data, ifd_size);
						if (!exif_process_IFD_TAG(ImageInfo, (char*)dir_entry, &info,
												  0, section_index, 0, tag_table)) {
							return false;
						}
					}
				}
				/* If we had a thumbnail in a SUB_IFD we have ANOTHER image in NEXT IFD */
				if (next_offset && section_index != SECTION_THUMBNAIL) {
					/* this should be a thumbnail IFD */
					/* the thumbnail itself is stored at Tag=StripOffsets */
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read next IFD (THUMBNAIL) at x%04X", next_offset);
#endif
					exif_process_IFD_in_TIFF(ImageInfo, next_offset, SECTION_THUMBNAIL);
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "%s THUMBNAIL @0x%04X + 0x%04X", ImageInfo->Thumbnail.data ? "Ignore" : "Read", ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
#endif
					if (!ImageInfo->Thumbnail.data && ImageInfo->Thumbnail.offset && ImageInfo->Thumbnail.size && ImageInfo->read_thumbnail) {
						ImageInfo->Thumbnail.data = safe_emalloc(ImageInfo->Thumbnail.size, 1, 0);
						php_stream_seek(ImageInfo->infile, ImageInfo->Thumbnail.offset, SEEK_SET);
						fgot = exif_read_from_stream_file_looped(ImageInfo->infile, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
						if (fgot != ImageInfo->Thumbnail.size) {
							EXIF_ERRLOG_THUMBEOF(ImageInfo)
							efree(ImageInfo->Thumbnail.data);
							ImageInfo->Thumbnail.data = NULL;
						} else {
							exif_thumbnail_build(ImageInfo);
						}
					}
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read next IFD (THUMBNAIL) done");
#endif
				}
				return true;
			} else {
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than size of IFD(x%04X)", ImageInfo->FileSize, dir_offset+ImageInfo->file.list[sn].size);
				return false;
			}
		} else {
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than size of IFD dir(x%04X)", ImageInfo->FileSize, dir_offset+dir_size);
			return false;
		}
	} else {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than start of IFD dir(x%04X)", ImageInfo->FileSize, dir_offset+2);
		return false;
	}
}
/* }}} */

static bool exif_process_IFD_in_TIFF(image_info_type *ImageInfo, size_t dir_offset, int section_index)
{
	bool result;
	if (ImageInfo->ifd_count++ > MAX_IFD_TAGS) {
		return false;
	}
	if (ImageInfo->ifd_nesting_level > MAX_IFD_NESTING_LEVEL) {
		return false;
	}
	ImageInfo->ifd_nesting_level++;
	result = exif_process_IFD_in_TIFF_impl(ImageInfo, dir_offset, section_index);
	ImageInfo->ifd_nesting_level--;
	return result;
}

/* {{{ exif_scan_FILE_header
 * Parse the marker stream until SOS or EOI is seen; */
static bool exif_scan_FILE_header(image_info_type *ImageInfo)
{
	unsigned char file_header[8];
	bool ret = false;

	ImageInfo->FileType = IMAGE_FILETYPE_UNKNOWN;

	if (ImageInfo->FileSize >= 2) {
		php_stream_seek(ImageInfo->infile, 0, SEEK_SET);
		if (exif_read_from_stream_file_looped(ImageInfo->infile, (char*)file_header, 2) != 2) {
			return false;
		}
		if ((file_header[0]==0xff) && (file_header[1]==M_SOI)) {
			ImageInfo->FileType = IMAGE_FILETYPE_JPEG;
			if (exif_scan_JPEG_header(ImageInfo)) {
				ret = true;
			} else {
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid JPEG file");
			}
		} else if (ImageInfo->FileSize >= 8) {
			if (exif_read_from_stream_file_looped(ImageInfo->infile, (char*)(file_header+2), 6) != 6) {
				return false;
			}
			if (!memcmp(file_header, "II\x2A\x00", 4)) {
				ImageInfo->FileType = IMAGE_FILETYPE_TIFF_II;
				ImageInfo->motorola_intel = 0;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "File has TIFF/II format");
#endif
				ImageInfo->sections_found |= FOUND_IFD0;
				if (exif_process_IFD_in_TIFF(ImageInfo,
											 php_ifd_get32u(file_header + 4, ImageInfo->motorola_intel),
											 SECTION_IFD0)) {
					ret = true;
				} else {
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF file");
				}
			} else if (!memcmp(file_header, "MM\x00\x2a", 4)) {
				ImageInfo->FileType = IMAGE_FILETYPE_TIFF_MM;
				ImageInfo->motorola_intel = 1;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "File has TIFF/MM format");
#endif
				ImageInfo->sections_found |= FOUND_IFD0;
				if (exif_process_IFD_in_TIFF(ImageInfo,
											 php_ifd_get32u(file_header + 4, ImageInfo->motorola_intel),
											 SECTION_IFD0)) {
					ret = true;
				} else {
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF file");
				}
			} else {
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "File not supported");
				return false;
			}
		}
	} else {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "File too small (%d)", ImageInfo->FileSize);
	}
	return ret;
}
/* }}} */

/* {{{ exif_discard_imageinfo
   Discard data scanned by exif_read_file.
*/
static bool exif_discard_imageinfo(image_info_type *ImageInfo)
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
	return true;
}
/* }}} */

/* {{{ exif_read_from_impl */
static bool exif_read_from_impl(image_info_type *ImageInfo, php_stream *stream, int read_thumbnail, int read_all)
{
	bool ret;
	zend_stat_t st = {0};

	/* Start with an empty image information structure. */
	memset(ImageInfo, 0, sizeof(*ImageInfo));

	ImageInfo->motorola_intel	= -1; /* flag as unknown */
	ImageInfo->infile			= stream;
	ImageInfo->FileName			= NULL;

	if (php_stream_is(ImageInfo->infile, PHP_STREAM_IS_STDIO)) {
		if (VCWD_STAT(stream->orig_path, &st) >= 0) {
			zend_string *base;
			if ((st.st_mode & S_IFMT) != S_IFREG) {
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Not a file");
				ImageInfo->infile = NULL;
				return false;
			}

			/* Store file name */
			base = php_basename(stream->orig_path, strlen(stream->orig_path), NULL, 0);
			ImageInfo->FileName = estrndup(ZSTR_VAL(base), ZSTR_LEN(base));

			zend_string_release_ex(base, 0);

			/* Store file date/time. */
			ImageInfo->FileDateTime = st.st_mtime;
			ImageInfo->FileSize = st.st_size;
		}
	} else {
		if (!ImageInfo->FileSize) {
			php_stream_seek(ImageInfo->infile, 0, SEEK_END);
			ImageInfo->FileSize = php_stream_tell(ImageInfo->infile);
			php_stream_seek(ImageInfo->infile, 0, SEEK_SET);
		}
	}

	ImageInfo->read_thumbnail		= read_thumbnail;
	ImageInfo->read_all				= read_all;
	ImageInfo->Thumbnail.filetype	= IMAGE_FILETYPE_UNKNOWN;

	ImageInfo->encode_unicode		= estrdup(EXIF_G(encode_unicode));
	ImageInfo->decode_unicode_be	= estrdup(EXIF_G(decode_unicode_be));
	ImageInfo->decode_unicode_le	= estrdup(EXIF_G(decode_unicode_le));
	ImageInfo->encode_jis			= estrdup(EXIF_G(encode_jis));
	ImageInfo->decode_jis_be	 	= estrdup(EXIF_G(decode_jis_be));
	ImageInfo->decode_jis_le		= estrdup(EXIF_G(decode_jis_le));


	ImageInfo->ifd_nesting_level = 0;
	ImageInfo->ifd_count = 0;
	ImageInfo->num_errors = 0;

	/* Scan the headers */
	ret = exif_scan_FILE_header(ImageInfo);

	return ret;
}
/* }}} */

/* {{{ exif_read_from_stream */
static bool exif_read_from_stream(image_info_type *ImageInfo, php_stream *stream, int read_thumbnail, int read_all)
{
	bool ret;
	off_t old_pos = php_stream_tell(stream);

	if (old_pos) {
		php_stream_seek(stream, 0, SEEK_SET);
	}

	ret = exif_read_from_impl(ImageInfo, stream, read_thumbnail, read_all);

	if (old_pos) {
		php_stream_seek(stream, old_pos, SEEK_SET);
	}

	return ret;
}
/* }}} */

/* {{{ exif_read_from_file */
static bool exif_read_from_file(image_info_type *ImageInfo, char *FileName, int read_thumbnail, int read_all)
{
	bool ret;
	php_stream *stream;

	stream = php_stream_open_wrapper(FileName, "rb", STREAM_MUST_SEEK | IGNORE_PATH, NULL);

	if (!stream) {
		memset(&ImageInfo, 0, sizeof(ImageInfo));

		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Unable to open file");

		return false;
	}

	ret = exif_read_from_stream(ImageInfo, stream, read_thumbnail, read_all);

	php_stream_close(stream);

	return ret;
}
/* }}} */

/* {{{ Reads header data from an image and optionally reads the internal thumbnails */
PHP_FUNCTION(exif_read_data)
{
	zend_string *z_sections_needed = NULL;
	bool sub_arrays = 0, read_thumbnail = 0, read_all = 0;
	zval *stream;
	bool ret;
	int i, sections_needed = 0;
	image_info_type ImageInfo;
	char tmp[64], *sections_str, *s;

	/* Parse arguments */
	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_ZVAL(stream)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(z_sections_needed)
		Z_PARAM_BOOL(sub_arrays)
		Z_PARAM_BOOL(read_thumbnail)
	ZEND_PARSE_PARAMETERS_END();

	memset(&ImageInfo, 0, sizeof(ImageInfo));

	if (z_sections_needed) {
		spprintf(&sections_str, 0, ",%s,", ZSTR_VAL(z_sections_needed));
		/* sections_str DOES start with , and SPACES are NOT allowed in names */
		s = sections_str;
		while (*++s) {
			if (*s == ' ') {
				*s = ',';
			}
		}

		for (i = 0; i < SECTION_COUNT; i++) {
			snprintf(tmp, sizeof(tmp), ",%s,", exif_get_sectionname(i));
			if (strstr(sections_str, tmp)) {
				sections_needed |= 1<<i;
			}
		}
		EFREE_IF(sections_str);
		/* now see what we need */
#ifdef EXIF_DEBUG
		sections_str = exif_get_sectionlist(sections_needed);
		if (!sections_str) {
			RETURN_FALSE;
		}
		exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Sections needed: %s", sections_str[0] ? sections_str : "None");
		EFREE_IF(sections_str);
#endif
	}

	if (Z_TYPE_P(stream) == IS_RESOURCE) {
		php_stream *p_stream = NULL;

		php_stream_from_res(p_stream, Z_RES_P(stream));

		ret = exif_read_from_stream(&ImageInfo, p_stream, read_thumbnail, read_all);
	} else {
		if (!try_convert_to_string(stream)) {
			RETURN_THROWS();
		}

		if (!Z_STRLEN_P(stream)) {
			zend_argument_value_error(1, "cannot be empty");
			RETURN_THROWS();
		}

		if (CHECK_NULL_PATH(Z_STRVAL_P(stream), Z_STRLEN_P(stream))) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}

		ret = exif_read_from_file(&ImageInfo, Z_STRVAL_P(stream), read_thumbnail, read_all);
	}

	sections_str = exif_get_sectionlist(ImageInfo.sections_found);

#ifdef EXIF_DEBUG
	if (sections_str) {
		exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Sections found: %s", sections_str[0] ? sections_str : "None");
	}
#endif

	ImageInfo.sections_found |= FOUND_COMPUTED|FOUND_FILE;/* do not inform about in debug*/

	if (ret == false || (sections_needed && !(sections_needed&ImageInfo.sections_found))) {
		/* array_init must be checked at last! otherwise the array must be freed if a later test fails. */
		exif_discard_imageinfo(&ImageInfo);
	   	EFREE_IF(sections_str);
		RETURN_FALSE;
	}

	array_init(return_value);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Generate section FILE");
#endif

	/* now we can add our information */
	exif_iif_add_str(&ImageInfo, SECTION_FILE, "FileName",      ImageInfo.FileName);
	exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileDateTime",  ImageInfo.FileDateTime);
	exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileSize",      ImageInfo.FileSize);
	exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileType",      ImageInfo.FileType);
	exif_iif_add_str(&ImageInfo, SECTION_FILE, "MimeType",      (char*)php_image_type_to_mime_type(ImageInfo.FileType));
	exif_iif_add_str(&ImageInfo, SECTION_FILE, "SectionsFound", sections_str ? sections_str : "NONE");

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Generate section COMPUTED");
#endif

	if (ImageInfo.Width>0 &&  ImageInfo.Height>0) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "html"   , "width=\"%d\" height=\"%d\"", ImageInfo.Width, ImageInfo.Height);
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Height", ImageInfo.Height);
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Width",  ImageInfo.Width);
	}
	exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "IsColor", ImageInfo.IsColor);
	if (ImageInfo.motorola_intel != -1) {
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "ByteOrderMotorola", ImageInfo.motorola_intel);
	}
	if (ImageInfo.FocalLength) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "FocalLength", "%4.1Fmm", ImageInfo.FocalLength);
		if(ImageInfo.CCDWidth) {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "35mmFocalLength", "%dmm", (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*35+0.5));
		}
	}
	if(ImageInfo.CCDWidth) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "CCDWidth", "%dmm", (int)ImageInfo.CCDWidth);
	}
	if(ImageInfo.ExposureTime>0) {
		float recip_exposure_time = 0.5f + 1.0f/ImageInfo.ExposureTime;
		if (ImageInfo.ExposureTime <= 0.5 && recip_exposure_time < (float)INT_MAX) {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ExposureTime", "%0.3F s (1/%d)", ImageInfo.ExposureTime, (int) recip_exposure_time);
		} else {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ExposureTime", "%0.3F s", ImageInfo.ExposureTime);
		}
	}
	if(ImageInfo.ApertureFNumber) {
		exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ApertureFNumber", "f/%.1F", ImageInfo.ApertureFNumber);
	}
	if(ImageInfo.Distance) {
		if(ImageInfo.Distance<0) {
			exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "FocusDistance", "Infinite");
		} else {
			exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "FocusDistance", "%0.2Fm", ImageInfo.Distance);
		}
	}
	if (ImageInfo.UserComment) {
		exif_iif_add_buffer(&ImageInfo, SECTION_COMPUTED, "UserComment", ImageInfo.UserCommentLength, ImageInfo.UserComment);
		if (ImageInfo.UserCommentEncoding && strlen(ImageInfo.UserCommentEncoding)) {
			exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "UserCommentEncoding", ImageInfo.UserCommentEncoding);
		}
	}

	exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright",              ImageInfo.Copyright);
	exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright.Photographer", ImageInfo.CopyrightPhotographer);
	exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright.Editor",       ImageInfo.CopyrightEditor);

	for (i=0; i<ImageInfo.xp_fields.count; i++) {
		exif_iif_add_str(&ImageInfo, SECTION_WINXP, exif_get_tagname_debug(ImageInfo.xp_fields.list[i].tag, exif_get_tag_table(SECTION_WINXP)), ImageInfo.xp_fields.list[i].value);
	}
	if (ImageInfo.Thumbnail.size) {
		if (read_thumbnail) {
			/* not exif_iif_add_str : this is a buffer */
			exif_iif_add_tag(&ImageInfo, SECTION_THUMBNAIL, "THUMBNAIL", TAG_NONE, TAG_FMT_UNDEFINED, ImageInfo.Thumbnail.size, ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size);
		}
		if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
			/* try to evaluate if thumbnail data is present */
			exif_scan_thumbnail(&ImageInfo);
		}
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.FileType", ImageInfo.Thumbnail.filetype);
		exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Thumbnail.MimeType", (char*)php_image_type_to_mime_type(ImageInfo.Thumbnail.filetype));
	}
	if (ImageInfo.Thumbnail.width && ImageInfo.Thumbnail.height) {
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.Height", ImageInfo.Thumbnail.height);
		exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.Width",  ImageInfo.Thumbnail.width);
	}
	EFREE_IF(sections_str);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Adding image infos");
#endif

	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_FILE      );
	add_assoc_image_info(return_value, 1,          &ImageInfo, SECTION_COMPUTED  );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_ANY_TAG   );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_IFD0      );
	add_assoc_image_info(return_value, 1,          &ImageInfo, SECTION_THUMBNAIL );
	add_assoc_image_info(return_value, 1,          &ImageInfo, SECTION_COMMENT   );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_EXIF      );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_GPS       );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_INTEROP   );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_FPIX      );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_APP12     );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_WINXP     );
	add_assoc_image_info(return_value, sub_arrays, &ImageInfo, SECTION_MAKERNOTE );

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Discarding info");
#endif

	exif_discard_imageinfo(&ImageInfo);

#ifdef EXIF_DEBUG
	php_error_docref1(NULL, (Z_TYPE_P(stream) == IS_RESOURCE ? "<stream>" : Z_STRVAL_P(stream)), E_NOTICE, "Done");
#endif
}
/* }}} */

/* {{{ Reads the embedded thumbnail */
PHP_FUNCTION(exif_thumbnail)
{
	bool ret;
	int arg_c = ZEND_NUM_ARGS();
	image_info_type ImageInfo;
	zval *stream;
	zval *z_width = NULL, *z_height = NULL, *z_imagetype = NULL;

	/* Parse arguments */
	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_ZVAL(stream)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(z_width)
		Z_PARAM_ZVAL(z_height)
		Z_PARAM_ZVAL(z_imagetype)
	ZEND_PARSE_PARAMETERS_END();

	memset(&ImageInfo, 0, sizeof(ImageInfo));

	if (Z_TYPE_P(stream) == IS_RESOURCE) {
		php_stream *p_stream = NULL;

		php_stream_from_res(p_stream, Z_RES_P(stream));

		ret = exif_read_from_stream(&ImageInfo, p_stream, 1, 0);
	} else {
		if (!try_convert_to_string(stream)) {
			RETURN_THROWS();
		}

		if (!Z_STRLEN_P(stream)) {
			zend_argument_value_error(1, "cannot be empty");
			RETURN_THROWS();
		}

		if (CHECK_NULL_PATH(Z_STRVAL_P(stream), Z_STRLEN_P(stream))) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}

		ret = exif_read_from_file(&ImageInfo, Z_STRVAL_P(stream), 1, 0);
	}

	if (ret == false) {
		exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Thumbnail data %d %d %d, %d x %d", ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size, ImageInfo.Thumbnail.filetype, ImageInfo.Thumbnail.width, ImageInfo.Thumbnail.height);
#endif
	if (!ImageInfo.Thumbnail.data || !ImageInfo.Thumbnail.size) {
		exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Returning thumbnail(%d)", ImageInfo.Thumbnail.size);
#endif

	ZVAL_STRINGL(return_value, ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size);
	if (arg_c >= 3) {
		if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
			if (!exif_scan_thumbnail(&ImageInfo)) {
				ImageInfo.Thumbnail.width = ImageInfo.Thumbnail.height = 0;
			}
		}
		ZEND_TRY_ASSIGN_REF_LONG(z_width,  ImageInfo.Thumbnail.width);
		ZEND_TRY_ASSIGN_REF_LONG(z_height, ImageInfo.Thumbnail.height);
	}
	if (arg_c >= 4)	{
		ZEND_TRY_ASSIGN_REF_LONG(z_imagetype, ImageInfo.Thumbnail.filetype);
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Discarding info");
#endif

	exif_discard_imageinfo(&ImageInfo);

#ifdef EXIF_DEBUG
	php_error_docref1(NULL, (Z_TYPE_P(stream) == IS_RESOURCE ? "<stream>" : Z_STRVAL_P(stream)), E_NOTICE, "Done");
#endif
}
/* }}} */

/* {{{ Get the type of an image */
PHP_FUNCTION(exif_imagetype)
{
	char *imagefile;
	size_t imagefile_len;
	php_stream * stream;
	int itype = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &imagefile, &imagefile_len) == FAILURE) {
		RETURN_THROWS();
	}

	stream = php_stream_open_wrapper(imagefile, "rb", IGNORE_PATH|REPORT_ERRORS, NULL);

	if (stream == NULL) {
		RETURN_FALSE;
	}

	itype = php_getimagetype(stream, imagefile, NULL);

	php_stream_close(stream);

	if (itype == IMAGE_FILETYPE_UNKNOWN) {
		RETURN_FALSE;
	} else {
		ZVAL_LONG(return_value, itype);
	}
}
/* }}} */
