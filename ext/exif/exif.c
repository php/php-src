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

/*	ToDos
 *
 *  JIS encoding for comments
 * 	See if example images from http://www.exif.org have illegal
 * 		thumbnail sizes or if code is corrupt.
 * 	Create/Update exif headers.
 * 	Create/Remove/Update image thumbnails.
 */

/* Fragments of the code in this module were borrowed from the public domain
 * jhead.c package with the author's consent.
 *
 *	The original header from the jhead.c file was:
 *
 * --------------------------------------------------------------------------
 *  Program to pull the information out of various types of EFIF digital
 *  camera files and show it in a reasonably consistent way
 *
 *  Version 0.9
 *
 *  Compiles with MSVC on Windows, or with GCC on Linux
 *
 *  Compileing under linux: Must include math library.
 *  Use: cc -lm -O3 -o jhead jhead.c
 *
 *  Matthias Wandel,  Dec 1999 - April 2000
 * --------------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_EXIF

/* When EXIF_DEBUG is defined the module generates a lot of debug messages
 * that help understanding what is going on. This can and should be used
 * while extending the module as it shows if you are at the right position.
 * You are always considered to have a copy of TIFF6.0 and EXIF 2.10 standard.
 */
#undef EXIF_DEBUG

#include "php_exif.h"
#include <math.h>
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_image.h"
#include "ext/standard/info.h"

typedef unsigned char uchar;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define EXIT_FAILURE  1
#define EXIT_SUCCESS  0

/* {{{ exif_functions[]
 */
function_entry exif_functions[] = {
	PHP_FE(exif_read_data, NULL)
	PHP_FALIAS(read_exif_data, exif_read_data, NULL)
	PHP_FE(exif_tagname, NULL)
	PHP_FE(exif_thumbnail, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

#define EXIF_VERSION "1.2"

PHP_MINFO_FUNCTION(exif);

/* {{{ exif_module_entry
 */
zend_module_entry exif_module_entry = {
	STANDARD_MODULE_HEADER,
	"exif",
	exif_functions,
	NULL, NULL,
	NULL, NULL,
	PHP_MINFO(exif),
	EXIF_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EXIF
ZEND_GET_MODULE(exif)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(exif)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "EXIF Support", "enabled" );
	php_info_print_table_row(2, "EXIF Version", EXIF_VERSION );
	php_info_print_table_row(2, "Supported EXIF Version", "0210");
	php_info_print_table_row(2, "Supported filetypes", "JPEG,TIFF");
	php_info_print_table_end();
}
/* }}} */

/* {{{ php_strnlen
 * get length of string if buffer if less than buffer size or buffer size */
static size_t php_strnlen( char* str, size_t maxlen) {
	size_t len = 0;

	if ( str && maxlen && *str) {
		do {
			len++;
		} while ( --maxlen && *(++str));
	}
	return len;
}
/* }}} */

/* {{{ format description defines
   Describes format descriptor
*/
static int php_tiff_bytes_per_format[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};
#define NUM_FORMATS 12

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
	}
	return "*Illegal";
}
#endif

/* Describes tag values */
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
#define TAG_STRIP_OFFSETS				0x0111
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
#define TAG_PREDICATOR                  0x013D
#define TAG_WHITE_POINT                 0x013E
#define TAG_PRIMARY_CHROMATICITIES      0x013F
#define TAG_COLOR_MAP                   0x0140
#define TAG_HALFTONE_HINTS              0x0141
#define TAG_TILE_WIDTH                  0x0142
#define TAG_TILE_LENGTH                 0x0143
#define TAG_TILE_OFFSETS                0x0144
#define TAG_TILE_BYTE_COUNTS            0x0145
#define TAG_INK_SETMPUTER               0x014C
#define TAG_NUMBER_OF_INKS              0x014E
#define TAG_INK_NAMES                   0x014D
#define TAG_DOT_RANGE                   0x0150
#define TAG_TARGET_PRINTER              0x0151
#define TAG_EXTRA_SAMPLE                0x0152
#define TAG_SAMPLE_FORMAT               0x0153
#define TAG_S_MIN_SAMPLE_VALUE          0x0154
#define TAG_S_MAX_SAMPLE_VALUE          0x0155
#define TAG_TRANSFER_RANGE              0x0156
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
#define TAG_COPYRIGHT                   0x8298
#define TAG_EXPOSURETIME                0x829A
#define TAG_FNUMBER                     0x829D
#define TAG_EXIF_IFD_POINTER            0x8769
#define TAG_GPS_IFD_POINTER             0x8825
#define TAG_ISOSPEED                    0x8827
#define TAG_EXIFVERSION                 0x9000
#define TAG_SHUTTERSPEED                0x9201
#define TAG_APERTURE                    0x9202
#define TAG_DATETIME_ORIGINAL           0x9003
#define TAG_MAXAPERTURE                 0x9205
#define TAG_SUBJECT_DISTANCE            0x9206
#define TAG_LIGHT_SOURCE                0x9208
#define TAG_FLASH                       0x9209
#define TAG_FOCALLENGTH                 0x920A
#define TAG_MARKER_NOTE				    0x927C
#define TAG_USERCOMMENT                 0x9286
#define TAG_FLASH_PIX_VERSION           0xA000
#define TAG_COLOR_SPACE                 0xA001
#define TAG_COMP_IMAGEWIDTH             0xA002 /* compressed images only */
#define TAG_COMP_IMAGEHEIGHT            0xA003
#define TAG_INTEROP_IFD_POINTER         0xA005 /* IFD pointer */
#define TAG_FOCALPLANEXRES              0xA20E
#define TAG_FOCALPLANEUNITS             0xA210

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

/* Values for TAG_PHOTOMETRIC_INTERPRETATION */
#define PMI_BLACK_IS_ZERO	    0
#define PMI_WHITE_IS_ZERO	    1
#define PMI_RGB          	    2
#define PMI_PALETTE_COLOR	    3
#define PMI_TRANSPARENCY_MASK   4
#define PMI_SEPARATED           5
#define PMI_YCBCR               6
#define PMI_CIELAB              8

/* }}} */

/* {{{ TabTable[]
 */

static const struct {
	unsigned short Tag;
	char *Desc;
} TagTable[] = {
  { 0x00FE, "NewSubFile"},
  { 0x00FF, "SubFile"},
  {	0x0100,	"ImageWidth"},
  {	0x0101,	"ImageLength"},
  {	0x0102,	"BitsPerSample"},
  {	0x0103,	"Compression"},
  {	0x0106,	"PhotometricInterpretation"},
  {	0x010A,	"FillOrder"},
  {	0x010D,	"DocumentName"},
  {	0x010E,	"ImageDescription"},
  {	0x010F,	"Make"},
  {	0x0110,	"Model"},
  {	0x0111,	"StripOffsets"},
  {	0x0112,	"Orientation"},
  {	0x0115,	"SamplesPerPixel"},
  {	0x0116,	"RowsPerStrip"},
  {	0x0117,	"StripByteCounts"},
  {	0x0118,	"MinSampleValue"},
  {	0x0119,	"MaxSampleValue"},
  {	0x011A,	"XResolution"},
  {	0x011B,	"YResolution"},
  {	0x011C,	"PlanarConfiguration"},
  {	0x011D,	"PageName"},
  {	0x011E,	"XPosition"},
  {	0x011F,	"YPosition"},
  {	0x0120,	"FreeOffsets"},
  {	0x0121,	"FreeByteCounts"},
  {	0x0122,	"GrayResponseUnit"},
  {	0x0123,	"GrayResponseCurve"},
  {	0x0124,	"T4Options"},
  {	0x0125,	"T6Options"},
  {	0x0128,	"ResolutionUnit"},
  {	0x0129,	"PageNumber"},
  {	0x012D,	"TransferFunction"},
  {	0x0131,	"Software"},
  {	0x0132,	"DateTime"},
  {	0x013B,	"Artist"},
  {	0x013C,	"HostComputer"},
  {	0x013D,	"Predictor"},
  {	0x013E,	"WhitePoint"},
  {	0x013F,	"PrimaryChromaticities"},
  {	0x0140,	"ColorMap"},
  {	0x0141,	"HalfToneHints"},
  {	0x0142,	"TileWidth"},
  {	0x0143,	"TileLength"},
  {	0x0144,	"TileOffsets"},
  {	0x0145,	"TileByteCounts"},
  {	0x014C,	"InkSet"},
  {	0x014D,	"InkNames"},
  {	0x014E,	"NumberOfInks"},
  {	0x0150,	"DotRange"},
  {	0x0151,	"TargetPrinter"},
  {	0x0152,	"ExtraSample"},
  {	0x0153,	"SampleFormat"},
  {	0x0154,	"SMinSampleValue"},
  {	0x0155,	"SMaxSampleValue"},
  {	0x0156,	"TransferRange"},
  {	0x0200,	"JPEGProc"},
  {	0x0201,	"JPEGInterchangeFormat"},
  {	0x0202,	"JPEGInterchangeFormatLength"},
  {	0x0203,	"JPEGRestartInterval"},
  {	0x0205,	"JPEGLosslessPredictors"},
  {	0x0206,	"JPEGPointTransforms"},
  {	0x0207,	"JPEGQTables"},
  {	0x0208,	"JPEGDCTables"},
  {	0x0209,	"JPEGACTables"},
  {	0x0211,	"YCbCrCoefficients"},
  {	0x0212,	"YCbCrSubSampling"},
  {	0x0213,	"YCbCrPositioning"},
  {	0x0214,	"ReferenceBlackWhite"},
  { 0x1000, "RelatedImageFileFormat"},
  {	0x828D,	"CFARepeatPatternDim"},
  {	0x828E,	"CFAPattern"},
  {	0x828F,	"BatteryLevel"},
  {	0x8298,	"Copyright"},
  {	0x829A,	"ExposureTime"},
  {	0x829D,	"FNumber"},
  {	0x83BB,	"IPTC/NAA"},
  {	0x8769,	"Exif_IFD_Pointer"},
  {	0x8773,	"InterColorProfile"},
  {	0x8822,	"ExposureProgram"},
  {	0x8824,	"SpectralSensitivity"},
  {	0x8825,	"GPS_IFD_Pointer"},
  {	0x8827,	"ISOSpeedRatings"},
  {	0x8828,	"OECF"},
  {	0x9000,	"ExifVersion"},
  {	0x9003,	"DateTimeOriginal"},
  {	0x9004,	"DateTimeDigitized"},
  {	0x9101,	"ComponentsConfiguration"},
  {	0x9102,	"CompressedBitsPerPixel"},
  {	0x9201,	"ShutterSpeedValue"},
  {	0x9202,	"ApertureValue"},
  {	0x9203,	"BrightnessValue"},
  {	0x9204,	"ExposureBiasValue"},
  {	0x9205,	"MaxApertureValue"},
  {	0x9206,	"SubjectDistance"},
  {	0x9207,	"MeteringMode"},
  {	0x9208,	"LightSource"},
  {	0x9209,	"Flash"},
  {	0x920A,	"FocalLength"},
  {	0x920B,	"FlashEnergy"},			        /* 0xA20B  in JPEG   */
  {	0x920C,	"SpatialFrequencyResponse"},	/* 0xA20C    -  -    */
  {	0x920E,	"FocalPlaneXResolution"},    	/* 0xA20E    -  -    */
  {	0x920F,	"FocalPlaneYResolution"},	    /* 0xA20F    -  -    */
  {	0x9210,	"FocalPlaneResolutionUnit"},	/* 0xA210    -  -    */
  {	0x9214,	"SubjectLocation"},		        /* 0xA214    -  -    */
  {	0x9215,	"ExposureIndex"},		        /* 0xA215    -  -    */
  {	0x9217,	"SensingMethod"},		        /* 0xA217    -  -    */
  {	0x927C,	"MakerNote"},
  {	0x9286,	"UserComment"},
  {	0x9290,	"SubSecTime"},
  {	0x9291,	"SubSecTimeOriginal"},
  {	0x9292,	"SubSecTimeDigitized"},
  {	0xA000,	"FlashPixVersion"},
  {	0xA001,	"ColorSpace"},
  {	0xA002,	"ExifImageWidth"},
  {	0xA003,	"ExifImageLength"},
  {	0xA005,	"InteroperabilityOffset"},
  {	0xA20B,	"FlashEnergy"},			        /* 0x920B in TIFF/EP */
  {	0xA20C,	"SpatialFrequencyResponse"},	/* 0x920C    -  -    */
  {	0xA20E,	"FocalPlaneXResolution"},    	/* 0x920E    -  -    */
  {	0xA20F,	"FocalPlaneYResolution"},	    /* 0x920F    -  -    */
  {	0xA210,	"FocalPlaneResolutionUnit"},	/* 0x9210    -  -    */
  {	0xA214,	"SubjectLocation"},		        /* 0x9214    -  -    */
  {	0xA215,	"ExposureIndex"},		        /* 0x9215    -  -    */
  {	0xA217,	"SensingMethod"},		        /* 0x9217    -  -    */
  {	0xA300,	"FileSource"},
  {	0xA301,	"SceneType"},
  {TAG_NONE,           "no tag value"},
  {TAG_COMPUTED_VALUE, "computed value"},
  {      0, ""}  /* Important for exif_get_tagname() IF value != "" functionresult is != false */
} ;

/* }}} */

/* {{{ exif_get_tagname
    Get headername for tag_num or NULL if not defined */
static char * exif_get_tagname(int tag_num, char *ret)
{
	int i,t;

	for (i=0;;i++) {
		if ( (t=TagTable[i].Tag) == tag_num || !t) {
			if (ret)  {
				strcpy(ret,TagTable[i].Desc);
				if ( !t) sprintf(ret,"UndefinedTag:0x%04X", tag_num);
				return ret;
			}
			return TagTable[i].Desc;
		}
	}
	if (ret) {
		sprintf(ret,"UndefinedTag:0x%04X", tag_num);
		return ret;
	}
	return "";
}
/* }}} */

/* {{{ php_ifd_get16m
   Get 16 bits motorola order (always) for jpeg header stuff.
*/
static int php_ifd_get16m(void *value)
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
		return  ((( char *)value)[0] << 24)
		      | (((uchar *)value)[1] << 16)
			  | (((uchar *)value)[2] << 8 )
			  | (((uchar *)value)[3]      );
	} else {
		return  ((( char *)value)[3] << 24)
		      | (((uchar *)value)[2] << 16)
			  | (((uchar *)value)[1] << 8 )
			  | (((uchar *)value)[0]      );
	}
}
/* }}} */

/* {{{ php_ifd_get32u
 * Convert a 32 bit unsigned value from file's native byte order */
static unsigned php_ifd_get32u(void *value, int motorola_intel)
{
	return (unsigned)php_ifd_get32s(value, motorola_intel) & 0xffffffff;
}
/* }}} */

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
			php_error(E_WARNING, "Found value of type single");
			return (double)*(float *)value;
		case TAG_FMT_DOUBLE:
			php_error(E_WARNING, "Found value of type double");
			return *(double *)value;
	}
	return 0;
}
/* }}} */

/* {{{ struct image_info_value, image_info_list
*/
#ifndef WORD
#define WORD short
#endif
#ifndef DWORD
#define DWORD int
#endif

typedef struct {
	int				num;
	int				den;
} signed_rational;

typedef struct {
	unsigned int	num;
	unsigned int	den;
} unsigned_rational;

typedef struct {
	WORD		tag;
	WORD		format;
	DWORD		length;
	union {
		char 				*s;
		unsigned			u;
		int 				i;
		float				f;
		double				d;
		signed_rational 	sr;
		unsigned_rational 	ur;
	} value;
	char 		*name;
} image_info_value;

typedef struct {
	int					count;
	image_info_value 	*list;
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
#define SECTION_EXIF		7
#define SECTION_FPIX		8
#define SECTION_GPS         9
#define SECTION_INTEROP     10
#define SECTION_APP12       11
#define SECTION_COUNT		12

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
	}
	return "";
}
/* }}} */

/* {{{ exif_get_sectionlist
   Return list of sectionnames specified by sectionlist. Return value must be freed
*/
static char *exif_get_sectionlist(int sectionlist)
{
	int i,len=0;
	char *sections;

	for(i=0; i<SECTION_COUNT; i++) len += strlen(exif_get_sectionname(i))+2;
	sections = emalloc(len+1);
	sections[0] = '\0';
	len = 0;
	for(i=0; i<SECTION_COUNT; i++) {
		if (sectionlist&(1<<i)) {
			sprintf(sections+len,"%s, ",exif_get_sectionname(i));
			len = strlen(sections);
		}
	}
	if (len>2) sections[len-2] = '\0';
	return sections;
}
/* }}} */

/* {{{ struct image_info_type
   This structure stores Exif header image elements in a simple manner
   Used to store camera data as extracted from the various ways that it can be
   stored in a nexif header
*/

typedef struct {
	uchar *Data;
	int    Type;
	size_t Size;
} section_t;

/* EXIF standard defines Copyright as "<Photographer> [ '\0' <Editor> ] ['\0']" */
/* This structure is used to store a section of a Jpeg file. */
typedef struct {
	char           *FileName;
	time_t         FileDateTime;
	size_t         FileSize;
    image_filetype FileType;
    FILE           *infile;
	int            Height, Width;
	int            IsColor;

	float   ApertureFNumber;
	float   ExposureTime;
	double  FocalplaneUnits;
	float   CCDWidth;
	double  FocalplaneXRes;
	int     ExifImageWidth;
	float   FocalLength;
	float   Distance;

	int  motorola_intel; /* 1 Motorola; 0 Intel */

	char *UserComment;
	int  UserCommentLen;
	char UserCommentEncoding[12];

	char *Thumbnail;
	int  ThumbnailSize;
	int  ThumbnailOffset;
	/* other */
	int              sections_found; /* FOUND_<marker> */
	image_info_list  info_list[SECTION_COUNT];
	/* for parsing */
	int read_thumbnail;
	int read_all;
	/* internal */
	section_t sections[20];
	int sections_count;
} image_info_type;
/* }}} */

/* {{{ exif_add_image_info
 Free memory allocated for image_info
*/
void exif_add_image_info( image_info_type *image_info, int section_index, char *name, int tag, int format, int length, void* value)
{
	image_info_value *info_value, *list;

	list = erealloc(image_info->info_list[section_index].list,(image_info->info_list[section_index].count+1)*sizeof(image_info_value));
	if ( !list) {
		php_error(E_WARNING,"Cannot allocate memory for all data");
		return;
	}
	image_info->info_list[section_index].list = list;
	image_info->sections_found |= 1<<section_index;

    info_value = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];

	info_value->name = emalloc(strlen(name)+1);
	strcpy(info_value->name, name);

	info_value->tag    = tag;
	info_value->format = format;
	info_value->length = 1; /* we do not support arrays other than STRING/UNDEFINED */
	switch (format) {
		case TAG_FMT_BYTE:
			info_value->value.u = *(uchar*)value;
			break;

		case TAG_FMT_STRING:
			if ( value) {
				length = php_strnlen(value,length);
				info_value->length = length;
				info_value->value.s = estrndup(value,length);
			} else {
				info_value->length = 0;
				info_value->value.s = estrdup("");
			}
			if ( !info_value->value.s) {
				info_value->length = 0;
				php_error(E_WARNING, "Could not allocate memory for thumbnail");
				break; /* better return with "" instead of possible casing problems */
			}
			break;

		case TAG_FMT_USHORT:
			info_value->value.u = php_ifd_get16u(value,image_info->motorola_intel);
			break;

		case TAG_FMT_ULONG:
			info_value->value.u = php_ifd_get32u(value,image_info->motorola_intel);
			break;

		case TAG_FMT_URATIONAL:
			info_value->value.ur.num = php_ifd_get32u(value, image_info->motorola_intel);
			info_value->value.ur.den = php_ifd_get32u(4+(char *)value, image_info->motorola_intel);
			break;

		case TAG_FMT_SBYTE:
			info_value->value.i = *(char*)value;
			break;

		default:
			/* Standard says more types possible but skip them...
			 * but allow users to handle data if they know how to
			 * So not return but use type UNDEFINED
			 * return;
			 */
			info_value->tag = TAG_FMT_UNDEFINED;/* otherwise not freed from memory */
		case TAG_FMT_UNDEFINED:
			if ( value) {
				info_value->value.s = estrndup(value,length);
				info_value->length = length;
			} else {
				info_value->length = 0;
				info_value->value.s = estrdup("");
			}
			if ( !info_value->value.s) {
				info_value->length = 0;
				php_error(E_WARNING, "Could not allocate memory for thumbnail");
				return;
			}
			break;

		case TAG_FMT_SSHORT:
			info_value->value.i = php_ifd_get16s(value,image_info->motorola_intel);
			break;

		case TAG_FMT_SLONG:
			info_value->value.i = php_ifd_get32s(value,image_info->motorola_intel);
			break;

		case TAG_FMT_SRATIONAL:
			info_value->value.sr.num = php_ifd_get32u(value, image_info->motorola_intel);
			info_value->value.sr.den = php_ifd_get32u(4+(char *)value, image_info->motorola_intel);
			break;

		case TAG_FMT_SINGLE:
			php_error(E_WARNING, "Found value of type single");
			info_value->value.f = *(float *)value;

		case TAG_FMT_DOUBLE:
			php_error(E_WARNING, "Found value of type double");
			info_value->value.d = *(double *)value;
			break;
	}

	image_info->info_list[section_index].count++;
}
/* }}} */

/* {{{ exif_iif_add_int
 Add a tag from IFD to image_info
*/
void exif_iif_add_int( image_info_type *image_info, int section_index, char *name, int value)
{
	image_info_value *info_value, *list;

	list = erealloc(image_info->info_list[section_index].list,(image_info->info_list[section_index].count+1)*sizeof(image_info_value));
	if ( !list) {
		php_error(E_WARNING, "Could not allocate memory for thumbnail");
		return;
	}
	image_info->info_list[section_index].list = list;

    info_value  = &image_info->info_list[section_index].list[image_info->info_list[section_index].count];
	info_value->tag    = TAG_NONE;
	info_value->format = TAG_FMT_SLONG;
	info_value->length = 1;
	info_value->name   = estrdup(name);
	if ( !info_value->name) {
		php_error(E_WARNING, "Could not allocate memory for thumbnail");
		return;
	}
	info_value->value.i = value;
	image_info->sections_found |= 1<<section_index;
	image_info->info_list[section_index].count++;
}
/* }}} */

/* {{{ exif_free_image_info
 Free memory allocated for image_info
*/
void exif_free_image_info( image_info_type *image_info, int section_index)
{
	int i;
	void *f;

	if (image_info->info_list[section_index].count)
	{
		for (i=0; i < image_info->info_list[section_index].count; i++)
		{
			if ( (f=image_info->info_list[section_index].list[i].name) != NULL)
			{
				efree(f);
			}
			if ( image_info->info_list[section_index].list[i].format == TAG_FMT_STRING
			   ||image_info->info_list[section_index].list[i].format == TAG_FMT_UNDEFINED
			)
			{
				if ( (f=image_info->info_list[section_index].list[i].value.s) != NULL)
				{
					efree(f);
				}
			}
		}
		if ( (f=image_info->info_list[section_index].list) != NULL)
		{
			efree(f);
		}
	}
}
/* }}} */

#undef RETURN_SUB_ARRAYS

/* {{{ add_assoc_image_info
 Add image_info to associative array value.
*/
void add_assoc_image_info( pval *value, int sub_array, image_info_type *image_info, int section_index)
{
	char	buffer[64], *val, *name, uname[64];
	int		idx=0, unknown=0;
	image_info_value	*info_value;

	if ( image_info->info_list[section_index].count)
	{
		int i;
		pval *tmpi;

		if ( sub_array) {
			MAKE_STD_ZVAL(tmpi);
			array_init(tmpi);
		} else {
			tmpi = value;
		}

		for(i=0; i<image_info->info_list[section_index].count; i++)
		{
			info_value = &image_info->info_list[section_index].list[i];
#ifdef EXIF_DEBUG
			/* php_error(E_NOTICE,"adding info #%d: '%s:%s'", i, exif_get_sectionname(section_index), info_value->name);*/
#endif
			if ( !(name = info_value->name)) {
				sprintf(uname,"%d", unknown++);
				name = uname;
			}
			switch(info_value->format)
			{
				default:
					/* Standard says more types possible but skip them...
					 * but allow users to handle data if they know how to
					 * So not return but use type UNDEFINED
					 * return;
					 */
				case TAG_FMT_UNDEFINED:
					if ( !info_value->value.s) {
						add_assoc_stringl(tmpi, name, "", 0, 1);
					} else {
						add_assoc_stringl(tmpi, name, info_value->value.s, info_value->length, 1);
					}
					break;

				case TAG_FMT_STRING:
					if ( !(val = info_value->value.s)) val = "";
					if (section_index==SECTION_COMMENT) {
						add_index_string(tmpi, idx++, val, 1);
					} else {
						add_assoc_string(tmpi, name, val, 1);
					}
					break;

				case TAG_FMT_BYTE:
				case TAG_FMT_USHORT:
				case TAG_FMT_ULONG:
					add_assoc_long(tmpi, name, (int)info_value->value.u);
					break;

				case TAG_FMT_URATIONAL:
					sprintf(buffer,"%i/%i", info_value->value.ur.num, info_value->value.ur.den);
					add_assoc_string(tmpi, name, buffer, 1);
					break;

				case TAG_FMT_SBYTE:
				case TAG_FMT_SSHORT:
				case TAG_FMT_SLONG:
					add_assoc_long(tmpi, name, info_value->value.i);
					break;

				case TAG_FMT_SRATIONAL:
					sprintf(buffer,"%i/%i", info_value->value.sr.num, info_value->value.sr.den);
					add_assoc_string(tmpi, name, buffer, 1);
					break;

				case TAG_FMT_SINGLE:
					add_assoc_double(tmpi, name, info_value->value.f);
					break;

				case TAG_FMT_DOUBLE:
					add_assoc_double(tmpi, name, info_value->value.d);
					break;
			}
		}
		if ( sub_array) {
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

#define M_TEM   0x01
#define M_SOF0  0xC0            /* Start Of Frame N                        */
#define M_SOF1  0xC1            /* N indicates which compression process   */
#define M_SOF2  0xC2            /* Only SOF0-SOF2 are now in common use    */
#define M_SOF3  0xC3
#define M_DHT   0xC4
#define M_SOF5  0xC5            /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9            /* End Of Image (end of datastream)         */
#define M_SOS   0xDA            /* Start Of Scan (begins compressed data)   */
#define M_DQT   0xDB
#define M_DNL   0xDC
#define M_DRI   0xDD
#define M_DHP   0xDE
#define M_EXP   0xDF
#define M_APP0  0xE0
#define M_EXIF  0xE1            /* Exif Attribute Information               */
#define M_FPIX  0xE2            /* Flash Pix Extension Data                 */
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
#define M_APP13 0xED
#define M_APP14 0xEE
#define M_APP15 0xEF
#define M_JPG0  0xF0
#define M_JPG13 0xFD
#define M_COM   0xFE            /* COMment                                  */

#define PSEUDO_IMAGE_MARKER 0x123; /* Extra value. */

/* }}} */

/* {{{ exif_process_COM
   Process a COM marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
static void exif_process_COM (image_info_type *image_info, uchar *value, int length)
{
    exif_add_image_info( image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_STRING, length-2, value+2);
}
/* }}} */

/* {{{ exif_process_SOFn
 * Process a SOFn marker.  This is useful for the image dimensions */
static void exif_process_SOFn (image_info_type *ImageInfo, uchar *Data, int marker)
{
	int data_precision, num_components;
	const char *process;

	data_precision = Data[2];
	ImageInfo->Height = php_ifd_get16m(Data+3);
	ImageInfo->Width = php_ifd_get16m(Data+5);
	num_components = Data[7];

	if (num_components == 3) {
		ImageInfo->IsColor = 1;
	} else {
		ImageInfo->IsColor = 0;
	}

	switch (marker) {
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
	}
}
/* }}} */

static void exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *DirStart, char *OffsetBase, unsigned IFDlength, int sub_section_index);

/* {{{ exif_get_markername
    Get name of marker */
#ifdef EXIF_DEBUG
char * exif_get_markername(int marker)
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
	szTemp = exif_get_tagname(tag,NULL);
	if ( tag<0 || !szTemp || !szTemp[0]) {
		RETURN_BOOL(FALSE);
	} else {
		RETURN_STRING(szTemp, 1)
	}
}
/* }}} */

/* {{{ exif_extract_thumbnail
 * Grab the thumbnail - by Matt Bonneau */
static void exif_extract_thumbnail(image_info_type *ImageInfo, char *offset, unsigned length) {
	/* according to exif2.1, the thumbnail is not supposed to be greater than 64K */
	if ( ImageInfo->read_thumbnail) {
		if (  ImageInfo->ThumbnailSize >= 65536
		   || ImageInfo->ThumbnailSize <= 0
		   || ImageInfo->ThumbnailOffset <= 0)
		{
			php_error(E_WARNING, "Illegal thumbnail size/offset");
			return;
		}
		ImageInfo->Thumbnail = emalloc(ImageInfo->ThumbnailSize);
		if (!ImageInfo->Thumbnail) {
			php_error(E_WARNING, "Could not allocate memory for thumbnail");
			return;
		} else {
			/* Check to make sure we are not going to go past the ExifLength */
			if ((unsigned)(ImageInfo->ThumbnailOffset + ImageInfo->ThumbnailSize) > length) {
				php_error(E_WARNING, "Thumbnail goes beyond exif header boundary");
				return;
			} else {
				memcpy(ImageInfo->Thumbnail, offset + ImageInfo->ThumbnailOffset, ImageInfo->ThumbnailSize);
			}
		}
	}
}
/* }}} */

/* {{{ exif_process_string_raw
 * Copy a string in Exif header to a character string returns length of allocated buffer if any. */
static int exif_process_string_raw(char **result,char *value,size_t byte_count) {
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
/* }}} */

/* {{{ exif_process_string
 * Copy a string in Exif header to a character string returns length of allocated buffer if any. */
static int exif_process_string(char **result,char *value,size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we cannot use strlen to
	 * determin length of string and we cannot use strlcpy with len=byte_count+1
	 * because then we might get into an EXCEPTION if we exceed an allocated
	 * memory page...so we use php_strnlen in conjunction with memcpy and add the NUL
	 * char.
	 */
	if ((byte_count=php_strnlen(value,byte_count)) > 0) {
		(*result) = emalloc(byte_count+1);
		memcpy(*result, value, byte_count);
		(*result)[byte_count] = '\0';
		return byte_count+1;
	}
	return 0;
}
/* }}} */

/* {{{ exif_process_user_comment
 * Process UserComment in IFD. */
static int exif_process_user_comment(char **pszInfoPtr,char *szEncoding,char *szValuePtr,int ByteCount)
{
	int   a;

	*szEncoding = '\0';
	/* Copy the comment */
	if ( ByteCount>=8) {
		if (!memcmp(szValuePtr, "UNICODE\0", 8) || !memcmp(szValuePtr, "JIS\0\0\0\0\0", 8)) {
			/* treat JIS encoding as if it where UNICODE */
			strcpy( szEncoding, szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
			return exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
		}
		if ( !memcmp(szValuePtr, "ASCII\0\0\0", 8)) {
			strcpy( szEncoding, szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
		}
		if ( !memcmp(szValuePtr,"\0\0\0\0\0\0\0\0",8)) {
			/* 8 NULL means undefined and should be ASCII... */
			strcpy( szEncoding, "UNDEFINED");
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
		}
	}

	/* Olympus has this padded with trailing spaces.  Remove these first. */
	if (ByteCount) for (a=ByteCount-1;a && szValuePtr[a]==' ';a--) (szValuePtr)[a] = '\0';

	/* normal text without encoding */
	return exif_process_string(pszInfoPtr, szValuePtr, ByteCount);
}
/* }}} */

/* {{{ exif_char_dump
 * Do not use! This is a debug function... */
#ifdef EXIF_DEBUG
static unsigned char* exif_char_dump( unsigned char * addr, int len)
{
  static unsigned char buf[1024+1];
  int i, p=0;

  for(i=0; i<len && p<sizeof(buf); i++)
  {
    if ( i%64==0) buf[p++] = '\n';
    if (*addr>=32)
    {
      buf[p++] = *addr;
    }
    else
    {
      buf[p++] = '?';
    }
    addr++;
  }
  buf[sizeof(buf)-1]=0;
  return buf;
}
#endif
/* }}} */

/* {{{ exif_process_IFD_TAG
 * Process one of the nested IFDs directories. */
static void exif_process_IFD_TAG(image_info_type *ImageInfo, char *dir_entry, char *offset_base, size_t IFDlength, int section_index, int ReadNextIFD)
{
	int l;
	int tag, format, components;
	char *value_ptr, tagname[64], cbuf[32], *outside=NULL;
	size_t byte_count, offset_val, fpos, fgot;

	tag = php_ifd_get16u(dir_entry, ImageInfo->motorola_intel);
	format = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
	components = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);

	if ((format-1) >= NUM_FORMATS) {
		/* (-1) catches illegal zero case as unsigned underflows to positive large. */
		php_error(E_WARNING, "Illegal format code in IFD");
		return;
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
					php_error(E_WARNING, "process tag(x%04X=%s): Illegal pointer offset(x%04X < x%04X)", tag, exif_get_tagname(tag,tagname), offset_val, dir_entry);
				} else {
					/* this is for sure not allowed */
					/* exception are IFD pointers */
					php_error(E_WARNING, "process tag(x%04X=%s): Illegal pointer offset(x%04X + x%04X = x%04X > x%04X)", tag, exif_get_tagname(tag,tagname), offset_val, byte_count, offset_val+byte_count, IFDlength);
				}
				return;
			}
			if ( byte_count>sizeof(cbuf)) {
				/* mark as outside range and get buffer */
				value_ptr = emalloc(byte_count);
				if ( !value_ptr) {
					php_error(E_WARNING, "could not allocate data for entire image");
					return;
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
			fpos = ftell(ImageInfo->infile);
			fseek(ImageInfo->infile, offset_val, SEEK_SET);
			fgot = ftell(ImageInfo->infile);
			if ( fgot!=offset_val) {
				if ( outside) efree( outside);
				php_error(E_WARNING,"Wrong file pointer: 0x%08X != 0x08X", fgot, offset_val);
				return;
			}
			fgot = fread(value_ptr, 1, byte_count, ImageInfo->infile);
			fseek(ImageInfo->infile, fpos, SEEK_SET);
			if ( fgot<byte_count) {
				if ( outside) efree( outside);
				php_error(E_WARNING, "File corrupt: End of file reached.");
				return;
			}
		}
	} else {
		/* 4 bytes or less and value is in the dir entry itself */
		value_ptr = dir_entry+8;
	}

	ImageInfo->sections_found |= FOUND_ANY_TAG;
#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"process tag(x%04x=%s,@x%04X+x%04X(=%d)): %s", tag, exif_get_tagname(tag,tagname), value_ptr-offset_base, byte_count, byte_count, format==TAG_FMT_STRING?(value_ptr?value_ptr:"<no data>"):exif_get_tagformat(format));
#endif
	if (section_index==SECTION_THUMBNAIL) {
		switch(tag) {
			case TAG_STRIP_OFFSETS:
			case TAG_JPEG_INTERCHANGE_FORMAT:
			    /* accept both formats */
				ImageInfo->ThumbnailOffset = (int)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				break;

			case TAG_JPEG_INTERCHANGE_FORMAT_LEN:
				ImageInfo->ThumbnailSize = (int)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				/* no need to check for offset here: Exif says values have to be stored in tag order */
				if ( ImageInfo->FileType == IMAGE_FILETYPE_JPEG) {
					exif_extract_thumbnail(ImageInfo, offset_base, IFDlength);
				}
				break;

		}
	} else{
		switch(tag) {
			case TAG_COPYRIGHT:
				if (byte_count>1 && (l=php_strnlen(value_ptr,byte_count)) > 0) {
					if ( (size_t)l<byte_count-1) {
						/* When there are any characters after the first NUL */
						exif_add_image_info( ImageInfo, SECTION_COMPUTED, "Copyright.Photographer", TAG_COPYRIGHT, TAG_FMT_STRING, l, value_ptr);
						exif_add_image_info( ImageInfo, SECTION_COMPUTED, "Copyright.Editor",       TAG_COPYRIGHT, TAG_FMT_STRING, byte_count-l-1, value_ptr+l+1);
						#ifdef EXIF_DEBUG
						php_error(E_NOTICE,"added copyrights: %s, %s", value_ptr, value_ptr+l+1);
						#endif
				    }
				}
				break;

			case TAG_USERCOMMENT:
				ImageInfo->UserCommentLen = exif_process_user_comment(&(ImageInfo->UserComment),ImageInfo->UserCommentEncoding,value_ptr,byte_count);
				if (ImageInfo->UserCommentLen)
					ImageInfo->UserCommentLen--; /* We want number of characters not allocation size */
				break;

			/* this is only a comment if type is string! */
/*			case TAG_TIFF_COMMENT:
				if (format == TAG_FMT_STRING) {
					php_error(E_WARNING,"Found TIFF Comment with wrong TAG 0x00FE");
					if (ImageInfo->numComments < EXIF_MAX_COMMENTS) {
						if (exif_process_string(&((ImageInfo->Comments)[ImageInfo->numComments]),value_ptr,byte_count)) {
							ImageInfo->numComments++;
						}
					}
				}
				break;*/

			/* More relevant info always comes earlier, so only use this field if we don't
			   have appropriate aperture information yet. */
			case TAG_APERTURE:
			case TAG_MAXAPERTURE:
				if (ImageInfo->ApertureFNumber == 0) {
					ImageInfo->ApertureFNumber
						= (float)exp(exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel)*log(2)*0.5);
				}
				break;

			case TAG_SHUTTERSPEED:
				/* More complicated way of expressing exposure time, so only use
				   this value if we don't already have it from somewhere else.
				   SHUTTERSPEED comes after EXPOSURE TIME
				  */
				if (ImageInfo->ExposureTime == 0) {
					ImageInfo->ExposureTime
						= (float)(1/exp(exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel)*log(2)));
				}
				break;
			case TAG_EXPOSURETIME:
				ImageInfo->ExposureTime = -1;
				break;

			case TAG_COMP_IMAGEWIDTH:
				ImageInfo->ExifImageWidth = (int)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				break;

			case TAG_FOCALPLANEXRES:
				ImageInfo->FocalplaneXRes = exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				break;

			case TAG_SUBJECT_DISTANCE:
				/* Inidcates the distacne the autofocus camera is focused to.
				   Tends to be less accurate as distance increases. */
				ImageInfo->Distance = (float)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel);
				break;

			case TAG_FOCALPLANEUNITS:
				switch((int)exif_convert_any_format(value_ptr, format, ImageInfo->motorola_intel)) {
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

			case TAG_EXIF_IFD_POINTER:
			case TAG_GPS_IFD_POINTER:
			case TAG_INTEROP_IFD_POINTER:
				if ( ReadNextIFD) {
					char *SubdirStart;
					int sub_section_index;
					switch(tag) {
						case TAG_EXIF_IFD_POINTER:
							#ifdef EXIF_DEBUG
							php_error(E_NOTICE,"found EXIF");
							#endif
							ImageInfo->sections_found |= FOUND_EXIF;
							sub_section_index = SECTION_EXIF;
							break;
						case TAG_GPS_IFD_POINTER:
							#ifdef EXIF_DEBUG
							php_error(E_NOTICE,"found GPS");
							#endif
							ImageInfo->sections_found |= FOUND_GPS;
							sub_section_index = SECTION_GPS;
							break;
						case TAG_INTEROP_IFD_POINTER:
							#ifdef EXIF_DEBUG
							php_error(E_NOTICE,"found INTEROPERABILITY");
							#endif
							ImageInfo->sections_found |= FOUND_INTEROP;
							sub_section_index = SECTION_INTEROP;
							break;
					}
					SubdirStart = offset_base + php_ifd_get32u(value_ptr, ImageInfo->motorola_intel);
					if (SubdirStart < offset_base || SubdirStart > offset_base+IFDlength) {
						php_error(E_WARNING, "Illegal IFD Pointer");
						break;
					}
					exif_process_IFD_in_JPEG(ImageInfo, SubdirStart, offset_base, IFDlength, sub_section_index);
					#ifdef EXIF_DEBUG
					php_error(E_NOTICE,"subsection %s done", exif_get_sectionname(sub_section_index));
					#endif
					break;
				}
				break;
		}
	}
	/* correctly would be to set components as length
	 * but we are ignoring length for those types where it is not the same as byte_count
	 */
	exif_add_image_info( ImageInfo, section_index, exif_get_tagname(tag,tagname), tag, format, byte_count, value_ptr);
	if ( outside) efree( outside);
}
/* }}} */

/* {{{ exif_process_IFD_in_JPEG
 * Process one of the nested IFDs directories. */
static void exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *DirStart, char *OffsetBase, unsigned IFDlength, int section_index)
{
	int de;
	int NumDirEntries;
	int NextDirOffset;

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"exif_process_IFD_in_JPEG(x%04X(=%d))", IFDlength, IFDlength);
#endif

	ImageInfo->sections_found |= FOUND_IFD0;

	NumDirEntries = php_ifd_get16u(DirStart, ImageInfo->motorola_intel);

	if ((DirStart+2+NumDirEntries*12) > (OffsetBase+IFDlength)) {
		php_error(E_WARNING, "Illegally sized directory x%04X + 2 + x%04X*12 = x%04X > x%04X", (int)DirStart+2-(int)OffsetBase, NumDirEntries, (int)DirStart+2+NumDirEntries*12-(int)OffsetBase, IFDlength);
		return;
	}

	for (de=0;de<NumDirEntries;de++) {
		exif_process_IFD_TAG(ImageInfo,DirStart+2+12*de,OffsetBase,IFDlength,section_index,1);
	}
	/*
	 * Hack to make it process IDF1 I hope
	 * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
	 */
	NextDirOffset = php_ifd_get32u(DirStart+2+12*de, ImageInfo->motorola_intel);
	if (NextDirOffset) {
		/* the next line seems false but here IFDlength means length of all IFDs */
		if (OffsetBase + NextDirOffset < OffsetBase || OffsetBase + NextDirOffset > OffsetBase+IFDlength) {
			php_error(E_WARNING, "Illegal directory offset");
			return;
		}
		/* That is the IFD for the first thumbnail */
#ifdef EXIF_DEBUG
		php_error(E_NOTICE,"expect next IFD to be thumbnail");
#endif
		exif_process_IFD_in_JPEG(ImageInfo, OffsetBase + NextDirOffset, OffsetBase, IFDlength, SECTION_THUMBNAIL);
	}
}
/* }}} */

/* {{{ exif_process_TIFF_in_JPEG
   Process a TIFF header in a JPEG file
*/
static void exif_process_TIFF_in_JPEG(image_info_type *ImageInfo, char *CharBuf, unsigned int length)
{
	/* set the thumbnail stuff to nothing so we can test to see if they get set up */
	if (memcmp(CharBuf, "II", 2) == 0) {
		ImageInfo->motorola_intel = 0;
	} else if (memcmp(CharBuf, "MM", 2) == 0) {
		ImageInfo->motorola_intel = 1;
	} else {
		php_error(E_WARNING, "Invalid TIFF alignment marker.");
		return;
	}

	/* Check the next two values for correctness. */
	if (php_ifd_get16u(CharBuf+2, ImageInfo->motorola_intel) != 0x2a
	  || php_ifd_get32u(CharBuf+4, ImageInfo->motorola_intel) != 0x08) {
		php_error(E_WARNING, "Invalid TIFF start (1)");
		return;
	}

	/* First directory starts at offset 8. Offsets starts at 0. */
	exif_process_IFD_in_JPEG(ImageInfo, CharBuf+8, CharBuf, length/*-14*/, SECTION_IFD0);

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"exif_process_TIFF_in_JPEG, done");
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
static void exif_process_APP1(image_info_type *ImageInfo, char *CharBuf, unsigned int length)
{
	/* Check the APP1 for Exif Identifier Code */
	static const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
	if (memcmp(CharBuf+2, ExifHeader, 6)) {
		php_error(E_WARNING, "Incorrect APP1 Exif Identifier Code");
		return;
	}
	exif_process_TIFF_in_JPEG(ImageInfo,CharBuf+8,length-8);
#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"process Exif done");
#endif
}
/* }}} */

/* {{{ exif_process_APP12
   Process an JPEG APP12 block marker used by OLYMPUS
*/
static void exif_process_APP12(image_info_type *ImageInfo, char *buffer, unsigned int length)
{
	int l1, l2=0;

	if ( (l1 = php_strnlen(buffer+2,length-2)) > 0) {
		exif_add_image_info( ImageInfo, SECTION_APP12, "Company", TAG_NONE, TAG_FMT_STRING, l1, buffer+2);
		if ( length > 2+(unsigned int)l1+1) {
			l2 = php_strnlen(buffer+2+l1+1,length-2-l1+1);
			exif_add_image_info( ImageInfo, SECTION_APP12, "Info", TAG_NONE, TAG_FMT_STRING, l2, buffer+2+l1+1);
		}
	}
#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"process section APP12 with l1=%d, l2=%d done", l1, l2);
#endif
}
/* }}} */

/* {{{ exif_scan_JPEG_header
 * Parse the marker stream until SOS or EOI is seen; */
static int exif_scan_JPEG_header(image_info_type *ImageInfo, FILE *infile)
{
	int a;

	for(ImageInfo->sections_count=0;ImageInfo->sections_count < 19;) {
		int itemlen;
		int marker = 0;
		int ll, lh, got;
		uchar *Data;
#ifdef EXIF_DEBUG
		size_t fpos;
#endif

#ifdef EXIF_DEBUG
		fpos = ftell(infile);
		php_error(E_NOTICE,"search section %d at 0x%04X", ImageInfo->sections_count, fpos);
#endif

		for (a=0;a<7;a++) {
			marker = fgetc(infile);
			if (marker != 0xff) break;
		}
#ifdef EXIF_DEBUG
		fpos = ftell(infile);
#endif
		if (marker == 0xff) {
			/* 0xff is legal padding, but if we get that many, something's wrong. */
			php_error(E_WARNING, "too many padding bytes!");
			return FALSE;
		}

		ImageInfo->sections[ImageInfo->sections_count].Type = marker;

		/* Read the length of the section. */
		lh = fgetc(infile);
		ll = fgetc(infile);

		itemlen = (lh << 8) | ll;

		if (itemlen < 2) {
			php_error(E_WARNING, "invalid marker");
			return FALSE;
		}

		ImageInfo->sections[ImageInfo->sections_count].Size = itemlen;

		Data = (uchar *)emalloc(itemlen+1); /* Add 1 to allow sticking a 0 at the end. */
		ImageInfo->sections[ImageInfo->sections_count++].Data = Data;

		/* Store first two pre-read bytes. */
		Data[0] = (uchar)lh;
		Data[1] = (uchar)ll;

		got = fread(Data+2, 1, itemlen-2, infile); /* Read the whole section. */
		if (got != itemlen-2) {
			php_error(E_WARNING, "error reading from file: got=x%04X(=%d) != itemlen-2=x%04X(=%d)",got, got, itemlen-2, itemlen-2);
			return FALSE;
		}

#ifdef EXIF_DEBUG
		php_error(E_NOTICE,"process section(x%02X=%s) @ x%04X + x%04X(=%d)", marker, exif_get_markername(marker), fpos, itemlen, itemlen);
#endif
		switch(marker) {
			case M_SOS:   /* stop before hitting compressed data  */
				/* If reading entire image is requested, read the rest of the data. */
				if (ImageInfo->read_all) {
					int cp, ep, size;
					/* Determine how much file is left. */
					cp = ftell(infile);
					fseek(infile, 0, SEEK_END);
					ep = ftell(infile);
					fseek(infile, cp, SEEK_SET);

					size = ep-cp;
					Data = (uchar *)emalloc(size);
					if (Data == NULL) {
						php_error(E_WARNING, "could not allocate data for entire image");
						return FALSE;
					}

					got = fread(Data, 1, size, infile);
					if (got != size) {
						php_error(E_WARNING, "could not read the rest of the image");
						return FALSE;
					}

					ImageInfo->sections[ImageInfo->sections_count].Data = Data;
					ImageInfo->sections[ImageInfo->sections_count].Size = size;
					ImageInfo->sections[ImageInfo->sections_count].Type = PSEUDO_IMAGE_MARKER;
					(ImageInfo->sections_count)++;
					/*
					*HaveAll = 1;
					*/
					/* efree(Data); Done in php_exif_discard_sections */
				}
				return TRUE;

			case M_EOI:   /* in case it's a tables-only JPEG stream */
				php_error(E_WARNING, "No image in jpeg!");
				return (ImageInfo->sections_found&(~FOUND_COMPUTED)) ? TRUE : FALSE;

			case M_COM: /* Comment section */
				exif_process_COM(ImageInfo, (char *)Data, itemlen);
				break;

			case M_EXIF:
				if ( !(ImageInfo->sections_found&FOUND_EXIF)) {
					ImageInfo->sections_found |= FOUND_EXIF;
					/* Seen files from some 'U-lead' software with Vivitar scanner
					   that uses marker 31 later in the file (no clue what for!) */
					exif_process_APP1(ImageInfo, (char *)Data, itemlen);
				}
				break;
			case M_FPIX:
				/*ImageInfo->sections_found |= FOUND_FPIX;*/
#ifdef EXIF_DEBUG
				php_error(E_NOTICE,"Found Flash Pix Extension Data");
#endif
				/*exif_process_TIFF_in_JPEG(ImageInfo,(char *)Data,itemlen);*/
				break;
			case M_APP12:
				exif_process_APP12(ImageInfo,(char *)Data,itemlen);
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
				exif_process_SOFn(ImageInfo, Data, marker);
				break;
			default:
				/* skip any other marker silently. */
				break;
		}
	}
#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"exif_scan_JPEG_header, done");
#endif
	return TRUE;
}
/* }}} */

/* {{{ exif_process_IFD_in_TIFF
 * Parse the TIFF header; */
static int exif_process_IFD_in_TIFF(image_info_type *ImageInfo, FILE *infile, size_t dir_offset, int section_index)
{
	int i, sn, num_entries, sub_section_index;
	unsigned char *dir_entry;
	size_t ifd_size, dir_size, entry_offset, next_offset, entry_length, entry_value;
	int entry_tag , entry_type;
#ifdef EXIF_DEBUG
	char tagname[64];
#endif

	sn = ImageInfo->sections_count++;
	if ( ImageInfo->FileSize >= dir_offset+2) {
		ImageInfo->sections[sn].Size = 2;
		ImageInfo->sections[sn].Data = emalloc(ImageInfo->sections[sn].Size);
		fseek(infile,dir_offset,SEEK_SET); /* we do not know the order of sections */
		fread(ImageInfo->sections[sn].Data, 1, 2, infile);
	    num_entries = php_ifd_get16u(ImageInfo->sections[sn].Data, ImageInfo->motorola_intel);
		dir_size = 2/*num dir entries*/ +12/*length of entry*/*num_entries +4/* offset to next ifd (points to thumbnail or NULL)*/;
#ifdef EXIF_DEBUG
		php_error(E_NOTICE,"Read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X), IFD entries(%d)", ImageInfo->FileSize, dir_offset, dir_size, num_entries);
#endif
		if ( ImageInfo->FileSize >= dir_offset+dir_size) {
			ImageInfo->sections[sn].Size = dir_size;
			ImageInfo->sections[sn].Data = erealloc(ImageInfo->sections[sn].Data,ImageInfo->sections[sn].Size);
			fread(ImageInfo->sections[sn].Data+2, 1, dir_size-2, infile);
			next_offset = php_ifd_get32u(ImageInfo->sections[sn].Data + dir_size - 4, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
			php_error(E_NOTICE,"Read from TIFF done, next offset x%04X", next_offset);
#endif
			/* now we have the directory we can look how long it should be */
			ifd_size = dir_size;
			for(i=0;i<num_entries;i++) {
				dir_entry 	 = ImageInfo->sections[sn].Data+2+i*12;
				entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
				entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
				if ( entry_type > NUM_FORMATS) {
					php_error(E_WARNING,"Error in TIFF: Illegal format, suppose bytes");
					entry_type = TAG_FMT_BYTE;
				}
				entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel) * php_tiff_bytes_per_format[entry_type];
				entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel) * php_tiff_bytes_per_format[entry_type];
				if ( entry_length > 4) {
					entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
					if ( entry_offset + entry_length > ifd_size) ifd_size = entry_offset + entry_length;
				} else {
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
						default:
						    continue;
					}
					switch(entry_tag) {
						case TAG_IMAGEWIDTH:
						case TAG_COMP_IMAGEWIDTH:
							ImageInfo->Width  = entry_value;
							break;
						case TAG_IMAGEHEIGHT:
						case TAG_COMP_IMAGEHEIGHT:
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
				}
			}
			ImageInfo->sections[sn].Size = ifd_size;
			ImageInfo->sections[sn].Data = erealloc(ImageInfo->sections[sn].Data,ImageInfo->sections[sn].Size);
			if ( ImageInfo->FileSize >= ImageInfo->sections[sn].Size) {
				if ( ifd_size > dir_size) {
					/* read values not stored in directory itself */
					if ( ImageInfo->sections[sn].Size > ImageInfo->FileSize) {
						php_error(E_ERROR,"Error in TIFF: filesize(x%04X) less than size of IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
						return FALSE;
					}
#ifdef EXIF_DEBUG
					php_error(E_NOTICE,"Read from TIFF: filesize(x%04X), IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
#endif
					fread(ImageInfo->sections[sn].Data+dir_size, 1, ifd_size-dir_size, infile);
#ifdef EXIF_DEBUG
					php_error(E_NOTICE,"Read from TIFF, done");
#endif
				}
				/* now process the tags */
				for(i=0;i<num_entries;i++) {
					dir_entry 	 = ImageInfo->sections[sn].Data+2+i*12;
					entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
					/*entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);*/
					/*entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);*/
					if (entry_tag == TAG_EXIF_IFD_POINTER ||
					    entry_tag == TAG_INTEROP_IFD_POINTER ||
					    entry_tag == TAG_GPS_IFD_POINTER
					   )
					{
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
						}
						entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
						php_error(E_NOTICE,"Found other IFD: %s at x%04X", exif_get_tagname(entry_tag,tagname), entry_offset);
#endif
						exif_process_IFD_in_TIFF(ImageInfo,infile,entry_offset,sub_section_index);
#ifdef EXIF_DEBUG
						php_error(E_NOTICE,"TIFF subsection %s done", exif_get_sectionname(sub_section_index));
#endif
					} else {
						exif_process_IFD_TAG(ImageInfo,dir_entry,ImageInfo->sections[sn].Data-dir_offset,ifd_size,section_index,0);
					}
				}
				if (next_offset && section_index != SECTION_THUMBNAIL) {
					/* this should be a thumbnail IFD */
					/* the thumbnail itself is stored at Tag=StripOffsets */
#ifdef EXIF_DEBUG
					php_error(E_NOTICE,"Read next IFD (THUMBNAIL) at x%04X", next_offset);
#endif
					exif_process_IFD_in_TIFF(ImageInfo,infile,next_offset,SECTION_THUMBNAIL);
#ifdef EXIF_DEBUG
					php_error(E_NOTICE,"Read THUMBNAIL @0x%04X + 0x%04X", ImageInfo->ThumbnailOffset, ImageInfo->ThumbnailSize);
#endif
					if (ImageInfo->ThumbnailOffset && ImageInfo->ThumbnailSize) {
						ImageInfo->Thumbnail = emalloc(ImageInfo->ThumbnailSize);
						if (!ImageInfo->Thumbnail) {
							php_error(E_WARNING, "Could not allocate memory for thumbnail");
						} else {
							fseek(infile,ImageInfo->ThumbnailOffset,SEEK_SET);
							fread(ImageInfo->Thumbnail, 1, ImageInfo->ThumbnailSize, infile);
						}
					}
#ifdef EXIF_DEBUG
					php_error(E_NOTICE,"Read next IFD (THUMBNAIL) done");
#endif
				}
				return TRUE;
			} else {
				php_error(E_ERROR,"Error in TIFF: filesize(x%04X) less than size of IFD(x%04X)", ImageInfo->sections[sn].Size);
				return FALSE;
			}
		} else {
			php_error(E_ERROR,"Error in TIFF: filesize(x%04X) less than size of IFD dir(x%04X)", ImageInfo->FileSize, dir_size);
			return FALSE;
		}
	} else {
		php_error(E_ERROR,"Error in TIFF: filesize(x%04X) less than size of IFD dir(x%04X)", ImageInfo->FileSize, dir_offset+2);
		return FALSE;
	}
}
/* }}} */

/* {{{ exif_scan_FILE_header
 * Parse the marker stream until SOS or EOI is seen; */
static int exif_scan_FILE_header (image_info_type *ImageInfo, FILE *infile)
{
    unsigned char file_header[8];

    ImageInfo->FileType = IMAGE_FILETYPE_UNKNOWN;

    if ( ImageInfo->FileSize >= 2) {
		fseek(infile, 0, SEEK_SET);
		fread(file_header, 1, 2, infile);
		if ( (file_header[0]==0xff) && (file_header[1]==M_SOI)) {
	    	ImageInfo->FileType = IMAGE_FILETYPE_JPEG;
	    	return exif_scan_JPEG_header(ImageInfo,infile);
		} else if ( ImageInfo->FileSize >= 8) {
			fread(file_header+2, 1, 6, infile);
		    if ( !memcmp(file_header,"II\x2A\x00", 4))
		    {
		    	ImageInfo->FileType = IMAGE_FILETYPE_TIFF_II;
		    	ImageInfo->motorola_intel = 0;
#ifdef EXIF_DEBUG
		    	php_error(E_NOTICE,"File(%s) has TIFF/II format", ImageInfo->FileName);
#endif
				ImageInfo->sections_found |= FOUND_IFD0;
		    	return exif_process_IFD_in_TIFF(ImageInfo,infile,php_ifd_get32u(file_header+4,ImageInfo->motorola_intel),SECTION_IFD0);
		    }
		    else
		    if ( !memcmp(file_header,"MM\x00\x2a", 4))
		    {
		    	ImageInfo->FileType = IMAGE_FILETYPE_TIFF_MM;
		    	ImageInfo->motorola_intel = 1;
#ifdef EXIF_DEBUG
		    	php_error(E_NOTICE,"File(%s) has TIFF/MM format", ImageInfo->FileName);
#endif
				ImageInfo->sections_found |= FOUND_IFD0;
		    	return exif_process_IFD_in_TIFF(ImageInfo,infile,php_ifd_get32u(file_header+4,ImageInfo->motorola_intel),SECTION_IFD0);
		    }
		}
	}
	php_error(E_WARNING,"File(%s) to small or filetype not supported", ImageInfo->FileName);
	return FALSE;
}
/* }}} */

/* {{{ php_exif_discard_sections
   Discard data read by php_exif_read_file.
*/
int php_exif_discard_sections(image_info_type *ImageInfo)
{
	int a;

	if ( ImageInfo->sections_count) {
		for (a=0;a<ImageInfo->sections_count;a++) {
			efree(ImageInfo->sections[a].Data);
		}
	}
	ImageInfo->sections_count = 0;
	return TRUE;
}
/* }}} */

/* {{{ php_exif_discard_imageinfo
   Discard data scanned by php_exif_read_file.
*/
int php_exif_discard_imageinfo(image_info_type *ImageInfo)
{
	int i;

	if (ImageInfo->FileName) 		efree(ImageInfo->FileName);
	if (ImageInfo->Thumbnail) 		efree(ImageInfo->Thumbnail);
	if (ImageInfo->UserComment)		efree(ImageInfo->UserComment);
	for (i=0; i<SECTION_COUNT; i++) {
		exif_free_image_info( ImageInfo, i);
	}
	php_exif_discard_sections(ImageInfo);
	memset(ImageInfo, 0, sizeof(*ImageInfo));
	return TRUE;
}
/* }}} */

/* {{{ php_exif_read_file
 */
int php_exif_read_file(image_info_type *ImageInfo, char *FileName, int read_thumbnail, int read_all TSRMLS_DC)
{
	int ret;
	FILE *infile;

	ImageInfo->motorola_intel = 0;

	infile = VCWD_FOPEN(FileName, "rb"); /* Unix ignores 'b', windows needs it. */

	if (infile == NULL) {
		php_error(E_WARNING, "Unable to open '%s'", FileName);
		return FALSE;
	}
    /* php_error(E_WARNING,"EXIF: Process %s%s: %s", read_thumbnail?"thumbs ":"", read_all?"All ":"", FileName); */
	/* Start with an empty image information structure. */
	memset(ImageInfo, 0, sizeof(*ImageInfo));

	ImageInfo->FileName = php_basename(FileName, strlen(FileName), NULL, 0);
	ImageInfo->read_thumbnail = read_thumbnail;
	ImageInfo->read_all = read_all;

	{
		/* Store file date/time. */
		struct stat st;
		if (VCWD_STAT(FileName, &st) >= 0) {
			ImageInfo->FileDateTime = st.st_mtime;
			ImageInfo->FileSize = st.st_size;
		} else {
			php_error(E_WARNING, "Can't get file statitics");
			return FALSE;
		}
	}

	/* Scan the JPEG headers. */
	ImageInfo->infile = infile;
	ret = exif_scan_FILE_header(ImageInfo, infile);
	if (!ret) {
		php_error(E_WARNING, "Invalid JPEG/TIFF file: '%s'", FileName);
	}

	fclose(infile);

	return ret;
}
/* }}} */

/* {{{ proto array|false read_exif_data(string filename [, sections_needed [, sub_arrays[, read_thumbnail]]])
   Reads header data from the JPEG/TIFF image filename and optionally reads the internal thumbnails */
PHP_FUNCTION(exif_read_data)
{
	pval **p_name, **p_sections_needed, **p_sub_arrays, **p_read_thumbnail, **p_read_all;
	int i, len, ac = ZEND_NUM_ARGS(), ret, sections_needed=0, sub_arrays=0, read_thumbnail=0, read_all=0;
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
		sprintf(sections_str,",%s,",Z_STRVAL_PP(p_sections_needed));
		/* sections_str DOES start with , and SPACES are NOT allowed in names */
		s = sections_str;
		while(*++s)
		{
			if(*s==' ') *s = ',';
		}
		for (i=0; i<SECTION_COUNT; i++)
		{
			sprintf(tmp,",%s,",exif_get_sectionname(i));
			if ( strstr(sections_str,tmp))
			{
				sections_needed |= 1<<i;
			}
		}
    	efree(sections_str);
    	/* now see what we need */
#ifdef EXIF_DEBUG
    	sections_str = exif_get_sectionlist(sections_needed);
    	php_error(E_NOTICE,"Sections needed: %s", sections_str[0] ? sections_str : "None");
    	efree(sections_str);
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

	ret = php_exif_read_file(&ImageInfo, Z_STRVAL_PP(p_name), read_thumbnail, read_all TSRMLS_CC);

   	sections_str = exif_get_sectionlist(ImageInfo.sections_found);

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Sections found: %s", sections_str[0] ? sections_str : "None");
#endif

	ImageInfo.sections_found |= FOUND_COMPUTED;/* do not inform about in debug*/

	if (ret==FALSE || (sections_needed && !(sections_needed&ImageInfo.sections_found)) || array_init(return_value) == FAILURE) {
		/* array_init must be checked at last! otherwise the array must be freed if a later test fails. */
		php_exif_discard_imageinfo(&ImageInfo);
		if ( sections_str) efree( sections_str);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Returning information");
#endif

	/*************************************************************************************************/
	/* generic created information must use motorola/intel organisation from the executing processor */
	i = 1;
	ImageInfo.motorola_intel = (*(char*)&i) ? 0 : 1;
	/* no more external processing from here                                                         */
	/*************************************************************************************************/

	/* now we can add our information */
	exif_add_image_info( &ImageInfo, SECTION_FILE, "FileName",      TAG_NONE, TAG_FMT_STRING, strlen(ImageInfo.FileName), ImageInfo.FileName);
	exif_iif_add_int( &ImageInfo, SECTION_FILE, "FileDateTime",  ImageInfo.FileDateTime);
	exif_iif_add_int( &ImageInfo, SECTION_FILE, "FileSize",      ImageInfo.FileSize);
	exif_add_image_info( &ImageInfo, SECTION_FILE, "SectionsFound", TAG_NONE, TAG_FMT_STRING, strlen(sections_str), sections_str);

	if (ImageInfo.Width>0 &&  ImageInfo.Height>0) {
		sprintf(tmp, "width=\"%d\" height=\"%d\"", ImageInfo.Width, ImageInfo.Height);
		exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "html",   TAG_NONE, TAG_FMT_STRING, strlen(tmp), tmp);
		exif_iif_add_int( &ImageInfo, SECTION_COMPUTED, "Height", ImageInfo.Height);
		exif_iif_add_int( &ImageInfo, SECTION_COMPUTED, "Width",  ImageInfo.Width);
	}
	exif_iif_add_int( &ImageInfo, SECTION_COMPUTED, "IsColor", ImageInfo.IsColor);
	if (ImageInfo.FocalLength) {
		sprintf(tmp, "%4.1fmm", ImageInfo.FocalLength);
		exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "FocalLength", TAG_NONE, TAG_FMT_STRING, strlen(tmp), tmp);
		if(ImageInfo.CCDWidth) {
			sprintf(tmp, "%dmm", (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*35+0.5));
			exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "35mmFocalLength", TAG_NONE, TAG_FMT_STRING, strlen(tmp), tmp);
		}
	}
	if(ImageInfo.CCDWidth) {
		sprintf(tmp, "%dmm", (int)ImageInfo.CCDWidth);
		exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "CCDWidth", TAG_NONE, TAG_FMT_STRING, strlen(tmp), tmp);
	}
	if(ImageInfo.ExposureTime>0) {
		if(ImageInfo.ExposureTime <= 0.5) {
			sprintf(tmp, "%0.3f s (1/%d)", ImageInfo.ExposureTime, (int)(0.5 + 1/ImageInfo.ExposureTime));
		} else {
			sprintf(tmp, "%0.3f s", ImageInfo.ExposureTime);
		}
		exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "ExposureTime", TAG_NONE, TAG_FMT_STRING, strlen(tmp), tmp);
	}
	if(ImageInfo.ApertureFNumber) {
		sprintf(tmp, "f/%.1f", ImageInfo.ApertureFNumber);
		exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "ApertureFNumber", TAG_NONE, TAG_FMT_STRING, strlen(tmp), tmp);
	}
	if(ImageInfo.Distance) {
		if(ImageInfo.Distance<0) {
			sprintf(tmp,"%s","Infinite");
		} else {
			sprintf(tmp, "%0.2fm", ImageInfo.Distance);
		}
		exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "FocusDistance", TAG_NONE, TAG_FMT_STRING, strlen(tmp), tmp);
	}
	if (ImageInfo.UserComment) {
		/*exif_iif_add_int( &ImageInfo, SECTION_COMPUTED, "UserCommentLen",  ImageInfo.UserCommentLen);*/
		exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "UserComment", TAG_NONE, TAG_FMT_UNDEFINED, ImageInfo.UserCommentLen, ImageInfo.UserComment);
		if ( (len=strlen(ImageInfo.UserCommentEncoding))) {
			exif_add_image_info( &ImageInfo, SECTION_COMPUTED, "UserCommentEncoding", TAG_NONE, TAG_FMT_STRING, len, ImageInfo.UserCommentEncoding);
		}
	}

	if ( read_thumbnail && ImageInfo.ThumbnailSize) {
		exif_add_image_info( &ImageInfo, SECTION_THUMBNAIL, "THUMBNAIL", TAG_NONE, TAG_FMT_UNDEFINED, ImageInfo.ThumbnailSize, ImageInfo.Thumbnail);
	}
   	efree(sections_str);

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"adding image infos");
#endif

	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_FILE);
	add_assoc_image_info( return_value, 1, 			&ImageInfo, SECTION_COMPUTED);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_ANY_TAG);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_IFD0);
	add_assoc_image_info( return_value, 1,          &ImageInfo, SECTION_THUMBNAIL);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_COMMENT);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_EXIF);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_GPS);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_INTEROP);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_FPIX);
	add_assoc_image_info( return_value, sub_arrays, &ImageInfo, SECTION_APP12);

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Discarding info");
#endif

	php_exif_discard_imageinfo(&ImageInfo);

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"read_exif_data done");
#endif
}
/* }}} */

/* {{{ proto string|false read_exif_data(string filename)
   Reads the embedded thumbnail */
PHP_FUNCTION(exif_thumbnail)
{
	pval **p_name;
	int ret, ac = ZEND_NUM_ARGS();
	image_info_type ImageInfo;

	memset(&ImageInfo, 0, sizeof(ImageInfo));

	if ( (ac != 1) || zend_get_parameters_ex(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(p_name);

	ret = php_exif_read_file(&ImageInfo, Z_STRVAL_PP(p_name), 1, 0 TSRMLS_CC);
	if (ret==FALSE) {
		php_exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

	if ( !ImageInfo.Thumbnail || !ImageInfo.ThumbnailSize) {
		php_exif_discard_imageinfo(&ImageInfo);
		php_error(E_NOTICE,"No thumbnail data found");
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Returning thumbnail(%d)", ImageInfo.ThumbnailSize);
#endif

	RETVAL_STRINGL(ImageInfo.Thumbnail, ImageInfo.ThumbnailSize, 1);

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Discarding info");
#endif

	php_exif_discard_imageinfo(&ImageInfo);

#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"exif_thumbnail done");
#endif
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
