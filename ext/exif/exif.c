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

/* Some parts of the code in this module was borrowed from the public domain
 * jhead.c package with the author's consent.
 */

/*	ToDos
 *
 *  JIS encoding for comments
 * 	See if example images from http://www.exif.org have illegal
 * 		thumbnail sizes or if code is corrupt.
 * 	Create/Update exif headers.
 * 	Create/Remove/Update image thumbnails.
 */

/*	The original header from the jhead.c file was:
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
/* #undef EXIF_DEBUG /**/

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
	PHP_FE(read_exif_data, NULL)
	PHP_FALIAS(exif_read_data,read_exif_data, NULL)
	PHP_FE(exif_headername, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

#define EXIF_VERSION "1.1b $Revision$"

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
	php_info_print_table_row(2, "Supported EXIF Version", "02100");
	php_info_print_table_row(2, "Supported filetypes", "JPEG,TIFF");
	php_info_print_table_row(2, "Thumbnails", "none");
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
  {      0, "UndefinedTag"}                 /* Important for exif_get_tagname() */
} ;
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

/* {{{ structs
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
		unsigned int		u;
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

#define ADD_IMAGE_INFO_xSTR( _image_info_ptr_ , _name_ , _tag_ , _length_ , _value_ )                                \
	_image_info_ptr_.list = erealloc(_image_info_ptr_.list,(_image_info_ptr_.count+1)*sizeof(image_info_value));    \
	((_image_info_ptr_).list[_image_info_ptr_.count]).tag = _tag_;                                                  \
	((_image_info_ptr_).list[_image_info_ptr_.count]).format = TAG_FMT_STRING;                                      \
	((_image_info_ptr_).list[_image_info_ptr_.count]).length = _length_;                                            \
	((_image_info_ptr_).list[_image_info_ptr_.count]).value.s = emalloc(_length_+1);                                \
	memcpy(((_image_info_ptr_).list[_image_info_ptr_.count]).value.s, _value_, _length_);                           \
	(((_image_info_ptr_).list[_image_info_ptr_.count]).value.s)[_length_] = '\0';                                   \
	((_image_info_ptr_).list[_image_info_ptr_.count]).name = emalloc(strlen(_name_)+1);                             \
	strcpy(((_image_info_ptr_).list[_image_info_ptr_.count]).name, _name_);                                         \
	(_image_info_ptr_).count++;                                                                                     \

/* }}} */

/* {{{ exif_add_image_info
 Free memory allocated for image_info
*/
void exif_add_image_info( image_info_list *image_info, char *name, int tag, int format, int length, void* value, int motorola_intel)
{
	image_info_value *info_value;
	image_info_value *list;

	list = erealloc(image_info->list,(image_info->count+1)*sizeof(image_info_value));
	if ( !list) {
		php_error(E_WARNING,"Unable to allocate more memory for data");
		return;
	}

	image_info->list = list;
    info_value = &(image_info->list[image_info->count]);

	info_value->tag = tag;
	info_value->format = format;
	info_value->length = 1; /* we do not support arrays other than STRING/UNDEFINED */
	switch (format) {
		case TAG_FMT_BYTE:
			info_value->value.u = *(uchar*)value;
			break;

		case TAG_FMT_STRING:
			length = php_strnlen(value,length);
			info_value->value.s = emalloc(length+1);
			strcpy(info_value->value.s,value);
			info_value->length = length;
			break;

		case TAG_FMT_USHORT:
			info_value->value.u = php_ifd_get16u(value,motorola_intel);
			break;

		case TAG_FMT_ULONG:
			info_value->value.u = php_ifd_get32u(value,motorola_intel);
			break;

		case TAG_FMT_URATIONAL:
			info_value->value.ur.num = php_ifd_get32u(value, motorola_intel);
			info_value->value.ur.den = php_ifd_get32u(4+(char *)value, motorola_intel);
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
		case TAG_FMT_UNDEFINED:
			info_value->value.s = emalloc(length+1);
			memcpy(info_value->value.s,value,length);
			info_value->value.s[length] = '\0';
			info_value->length = length; /* not +1 !!! */
			break;

		case TAG_FMT_SSHORT:
			info_value->value.i = php_ifd_get16s(value,motorola_intel);
			break;

		case TAG_FMT_SLONG:
			info_value->value.i = php_ifd_get32s(value,motorola_intel);
			break;

		case TAG_FMT_SRATIONAL:
			info_value->value.sr.num = php_ifd_get32u(value, motorola_intel);
			info_value->value.sr.den = php_ifd_get32u(4+(char *)value, motorola_intel);
			break;

		case TAG_FMT_SINGLE:
			php_error(E_WARNING, "Found value of type single");
			info_value->value.f = (double)*(float *)value;

		case TAG_FMT_DOUBLE:
			php_error(E_WARNING, "Found value of type double");
			info_value->value.d = *(double *)value;
			break;
	}
	info_value->name = emalloc(strlen(name)+1);
	strcpy(info_value->name, name);
	image_info->count++;
}
/* }}} */

/* {{{ exif_free_image_info
 Free memory allocated for image_info
*/
void exif_free_image_info( image_info_list *image_info)
{
	if ( image_info->count) {
		int i;

		for (i=0; i<image_info->count; i++)
		{
			efree((image_info->list)[i].name);
			if ((image_info->list)[i].format == TAG_FMT_STRING)
			{
				efree((image_info->list)[i].value.s);
			}
		}
		efree(image_info->list);
	}
}
/* }}} */

#undef RETURN_SUB_ARRAYS

/* {{{ add_assoc_image_info
 Add image_info to associative array value.
*/
void add_assoc_image_info( pval *value, int sub_array, char *name, image_info_list *image_info)
{
	char	buffer[64];

	if ( image_info->count)
	{
		int i;
		pval *tmpi;

		if ( sub_array) {
			MAKE_STD_ZVAL(tmpi);
			array_init(tmpi);
		} else {
			tmpi = value;
		}

		for(i=0; i<image_info->count; i++)
		{
			#ifdef EXIF_DEBUG
			php_error(E_NOTICE,"adding info #%d: '%s:%s'", i, name, image_info->list[i].name);
			#endif
			switch(image_info->list[i].format)
			{
				default:
					/* Standard says more types possible but skip them...
					 * but allow users to handle data if they know how to
					 * So not return but use type UNDEFINED
					 * return;
					 */
				case TAG_FMT_UNDEFINED:
					add_assoc_stringl(tmpi, image_info->list[i].name, image_info->list[i].value.s, image_info->list[i].length, 1);
					break;

				case TAG_FMT_STRING:
					add_assoc_string(tmpi, image_info->list[i].name, image_info->list[i].value.s, 1);
					break;

				case TAG_FMT_BYTE:
				case TAG_FMT_USHORT:
				case TAG_FMT_ULONG:
					add_assoc_long(tmpi, image_info->list[i].name, image_info->list[i].value.u);
					break;

				case TAG_FMT_URATIONAL:
					sprintf(buffer,"%i/%i", image_info->list[i].value.ur.num, image_info->list[i].value.ur.den);
					add_assoc_string(tmpi, image_info->list[i].name, buffer, 1);
					break;

				case TAG_FMT_SBYTE:
				case TAG_FMT_SSHORT:
				case TAG_FMT_SLONG:
					add_assoc_long(tmpi, image_info->list[i].name, image_info->list[i].value.i);
					break;

				case TAG_FMT_SRATIONAL:
					sprintf(buffer,"%i/%i", image_info->list[i].value.sr.num, image_info->list[i].value.sr.den);
					add_assoc_string(tmpi, image_info->list[i].name, buffer, 1);
					break;

				case TAG_FMT_SINGLE:
					add_assoc_double(tmpi, image_info->list[i].name, image_info->list[i].value.f);
					break;

				case TAG_FMT_DOUBLE:
					add_assoc_double(tmpi, image_info->list[i].name, image_info->list[i].value.d);
					break;
			}
		}
		if ( sub_array) {
			add_assoc_zval(value, name, tmpi);
		}
	}
}
/* }}} */

/* {{{ structs
   This structure stores Exif header image elements in a simple manner
   Used to store camera data as extracted from the various ways that it can be
   stored in a nexif header
*/

#define FOUND_ANY_TAG	0x00000001
#define FOUND_IFD0		0x00000002
#define FOUND_THUMBNAIL	0x00000002
#define FOUND_COMMENT	0x00000008
#define FOUND_APP0      0x00000010
#define FOUND_EXIF		0x00000020
#define FOUND_FPIX		0x00000040
#define FOUND_GPS		0x00000080
#define FOUND_INTEROP	0x00000100
#define FOUND_APP12     0x00000200

typedef struct {
	uchar *Data;
	int    Type;
	size_t Size;
} section_t;

#define EXIF_MAX_COMMENTS 12

/* EXIF standard defines Copyright as "<Photographer> [ '\0' <Editor> ] ['\0']" */
#define EXIF_MAX_COPYRIGHT 2

/* This structure is used to store a section of a Jpeg file. */
typedef struct {
	char  *FileName;
	time_t FileDateTime;
	size_t FileSize;
    image_filetype FileType;
	int   Height, Width;
	int   IsColor;
	int   FlashUsed;
	float FocalLength;
	float ExposureTime;
	float ApertureFNumber;
	float Distance;
	float CCDWidth;
	char  *Comments[EXIF_MAX_COMMENTS];
	int numComments;
	char UserCommentEncoding[12];
	char *UserComment;
	double FocalplaneXRes;
	double FocalplaneUnits;
	int ExifImageWidth;
	int MotorolaOrder;
	int Orientation;
	int ISOspeed;
	char *RawCopyright;
	int lenRawCopyright;
	char *Copyright[EXIF_MAX_COPYRIGHT];
	int numCopyrights;
	char *Thumbnail;
	int ThumbnailSize;
	int ThumbnailOffset;
	/* Olympus vars */
/*	int SpecialMode;
	int JpegQual;
	int Macro;
	int DigiZoom;
	char SoftwareRelease[16];
	char PictInfo[64];
	char CameraId[64];*/
	/* other */
	image_info_list  ANY;
	image_info_list  IFD0;
	image_info_list  APP1;
	image_info_list  EXIF;
	image_info_list  FPIX;
	image_info_list  GPS;
	image_info_list  INTEROP;
	image_info_list  APP12;
	/* for parsing */
	int read_thumbnail;
	int read_all;
	/* internal */
	int sections_found; /* FOUND_<marker> */
	section_t sections[20];
	int sections_count;
	/* End Olympus vars */
} image_info_type;
/* }}} */

/* {{{ Markers
   JPEG markers consist of one or more 0xFF bytes, followed by a marker
   code byte (which is not an FF).  Here are the marker codes of interest
   in this program.  (See jdmarker.c for a more complete list.)
*/

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
#define M_TEM   0x01

#define PSEUDO_IMAGE_MARKER 0x123; /* Extra value. */

/* }}} */

/* {{{ php_ifd_get16m
   Get 16 bits motorola order (always) for jpeg header stuff.
*/
static int php_ifd_get16m(void *Short)
{
	return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
}
/* }}} */

/* {{{ exif_process_COM
   Process a COM marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
static void exif_process_COM (image_info_type *ImageInfo, uchar *Data, int length)
{
	int ch;
	char Comment[250];
	int nch;
	int a;

	if(ImageInfo->numComments == EXIF_MAX_COMMENTS) return;

	nch = 0;

	if (length > 200) length = 200; /* Truncate if it won't fit in our structure. */

	for (a=2;a<length;a++) {
		ch = Data[a];

		if (ch == '\r' && Data[a+1] == '\n') continue; /* Remove cr followed by lf. */

		if (isprint(ch) || ch == '\n' || ch == '\t') {
			Comment[nch++] = (char)ch;
		} else {
			Comment[nch++] = '?';
		}
	}

	Comment[nch] = '\0'; /* Null terminate */

	a = ImageInfo->numComments;

	(ImageInfo->Comments)[a] = emalloc(nch+1);
	strcpy(ImageInfo->Comments[a], Comment);
	(ImageInfo->numComments)++;
	ImageInfo->sections_found |= FOUND_COMMENT;
	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"process comment section: '%s'",ImageInfo->Comments[a]);
	#endif
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

static void exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *DirStart, char *OffsetBase, unsigned IFDlength);

/* {{{ exif_get_tagname
    Get headername for tag_num or NULL if not defined */
char * exif_get_tagname(int tag_num)
{
	int i,t;

	for (i=0;;i++) {
		if ( (t=TagTable[i].Tag) == tag_num || !t) return TagTable[i].Desc;
	}
	return "";
}
/* }}} */

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

/* {{{ proto string|false exif_headername(index)
    Get headername for index or false if not defined */
PHP_FUNCTION(exif_headername)
{
	pval **p_num;
	int ac = ZEND_NUM_ARGS();
	char *szTemp;

	if ((ac < 1 || ac > 1) || zend_get_parameters_ex(ac, &p_num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(p_num);
	if ( (szTemp = exif_get_tagname(Z_LVAL_PP(p_num))) == NULL) {
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
	int   a,l;
	char  mbBuffer[MB_CUR_MAX];

	*szEncoding = '\0';
	/* Copy the comment */
	if ( ByteCount>=8) {
		if (!memcmp(szValuePtr, "UNICODE\0", 8) || !memcmp(szValuePtr, "JIS\0\0\0\0\0", 8)) {
			/* treat JIS encoding as if it where UNICODE */
			strcpy( szEncoding, szValuePtr);
			szValuePtr = szValuePtr+8;
			ByteCount -= 8;
			l = 0;
			a = 0;
			while(((wchar_t*)szValuePtr)[a]) {
				l += (int)wctomb( mbBuffer, *((wchar_t*)szValuePtr));
				if (sizeof(wchar_t)*a++ >= ByteCount) break; /* avoiding problems with corrupt headers */
			}
			if (l>1) {
				*pszInfoPtr = emalloc(l+1);
				wcstombs(*pszInfoPtr, (wchar_t*)(szValuePtr), l+1);
				(*pszInfoPtr)[l] = '\0';
				return l+1;
			}
			return 0;
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
	for (a=ByteCount-1;a && szValuePtr[a]==' ';a--) (szValuePtr)[a] = '\0';

	/* normal text without encoding */
	return exif_process_string(pszInfoPtr,szValuePtr,ByteCount);
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
static void exif_process_IFD_TAG(image_info_type *ImageInfo, char *dir_entry, char *offset_base, size_t IFDlength, int ReadNextIFD)
{
	int l;
	int tag, format, components;
	char *value_ptr;
	size_t byte_count;
	unsigned offset_val;

	tag = php_ifd_get16u(dir_entry, ImageInfo->MotorolaOrder);
	format = php_ifd_get16u(dir_entry+2, ImageInfo->MotorolaOrder);
	components = php_ifd_get32u(dir_entry+4, ImageInfo->MotorolaOrder);

	if ((format-1) >= NUM_FORMATS) {
		/* (-1) catches illegal zero case as unsigned underflows to positive large. */
		php_error(E_WARNING, "Illegal format code in IFD");
		return;
	}

	byte_count = components * php_tiff_bytes_per_format[format];

	if (byte_count > 4) {
		offset_val = php_ifd_get32u(dir_entry+8, ImageInfo->MotorolaOrder);
		/* If its bigger than 4 bytes, the dir entry contains an offset. */
		if (offset_val+byte_count > IFDlength) {
			/* Bogus pointer offset and / or byte_count value */
			php_error(E_WARNING, "Illegal pointer offset(x%04X + x%04X > x%04X) in IFD for Tag(x%04X=%s): ", offset_val, byte_count, IFDlength, tag, exif_get_tagname(tag));
			return;
		}
		value_ptr = offset_base+offset_val;
	} else {
		/* 4 bytes or less and value is in the dir entry itself */
		value_ptr = dir_entry+8;
	}

	/* Extract useful components of IFD */
	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"process tag(x%04x=%s,@x%04X+%i): %s", tag, exif_get_tagname(tag), value_ptr-offset_base, byte_count, format==TAG_FMT_STRING?(value_ptr?value_ptr:"<no data>"):"<no string>");
	#endif
	switch(tag) {
		case TAG_COPYRIGHT:
			if (exif_process_string_raw(&ImageInfo->RawCopyright,value_ptr,byte_count)) {
				l = strlen(ImageInfo->RawCopyright);
			} else {
				l = 0;
			}
			ImageInfo->lenRawCopyright = byte_count;
			if ( ImageInfo->numCopyrights==0 && byte_count>1 && l<byte_count-1) {
				exif_process_string(&((ImageInfo->Copyright)[ImageInfo->numCopyrights++]), value_ptr, l);
				exif_process_string(&((ImageInfo->Copyright)[ImageInfo->numCopyrights++]), value_ptr+l+1, byte_count-l-1);
		    }
			break;

/*		case TAG_ORIENTATION:
			ImageInfo->Orientation = (int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*/

/*		case TAG_ISOSPEED:
			ImageInfo->ISOspeed = (int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*/

		case TAG_USERCOMMENT:
			exif_process_user_comment(&(ImageInfo->UserComment),ImageInfo->UserCommentEncoding,value_ptr,byte_count);
			break;

		/* this is only a comment if type is string! */
/*		case TAG_TIFF_COMMENT:
			if (format == TAG_FMT_STRING) {
				php_error(E_WARNING,"Found TIFF Comment with wrong TAG 0x00FE");
				if (ImageInfo->numComments < EXIF_MAX_COMMENTS) {
					if (exif_process_string(&((ImageInfo->Comments)[ImageInfo->numComments]),value_ptr,byte_count)) {
						ImageInfo->numComments++;
					}
				}
			}
			break;*/

		#ifdef EXIF_DEBUG
		/*case TAG_MARKER_NOTE:
		*	php_error(E_NOTICE,exif_char_dump(value_ptr,byte_count));
		*	break;
		*/
		#endif

		/* Simplest way of expressing aperture, so I trust it the most.
		   (overwrite previously computd value if there is one) */
/*		case TAG_FNUMBER:
			ImageInfo->ApertureFNumber = (float)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*/

		/* More relevant info always comes earlier, so only use this field if we don't
		   have appropriate aperture information yet. */
		case TAG_APERTURE:
		case TAG_MAXAPERTURE:
			if (ImageInfo->ApertureFNumber == 0) {
				ImageInfo->ApertureFNumber
					= (float)exp(exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder)*log(2)*0.5);
			}
			break;

		/* Nice digital cameras actually save the focal length as a function
		   of how farthey are zoomed in. */
/*		case TAG_FOCALLENGTH:
			ImageInfo->FocalLength = (float)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*/

		/* Inidcates the distacne the autofocus camera is focused to.
		   Tends to be less accurate as distance increases. */
/*		case TAG_SUBJECT_DISTANCE:
			ImageInfo->Distance = (float)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*/

		/* Simplest way of expressing exposure time, so I trust it most.
		   (overwrite previously computd value if there is one) */
/*		case TAG_EXPOSURETIME:
			ImageInfo->ExposureTime = (float)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*/

		case TAG_SHUTTERSPEED:
			/* More complicated way of expressing exposure time, so only use
			   this value if we don't already have it from somewhere else. */
			if (ImageInfo->ExposureTime == 0) {
				ImageInfo->ExposureTime
					= (float)(1/exp(exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder)*log(2)));
			}
			break;

/*		case TAG_FLASH:
			if (exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder)) {
				ImageInfo->FlashUsed = 1;
			}
			break;*/

		case TAG_COMP_IMAGEWIDTH:
			ImageInfo->ExifImageWidth = (int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;

/*		case TAG_FOCALPLANEXRES:
			ImageInfo->FocalplaneXRes = exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*/

		case TAG_FOCALPLANEUNITS:
			switch((int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder)) {
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

/*
		case TAG_OLYMPUS_SPECIALMODE:
			ImageInfo->SpecialMode = (int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;*
		case TAG_OLYMPUS_DIGIZOOM:
			ImageInfo->DigiZoom = (int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;
		case TAG_OLYMPUS_SOFTWARERELEASE:
			strlcpy(ImageInfo->SoftwareRelease, value_ptr, sizeof(ImageInfo->SoftwareRelease));
			break;
		case TAG_OLYMPUS_PICTINFO:
			strlcpy(ImageInfo->PictInfo, value_ptr, sizeof(ImageInfo->PictInfo));
			break;
		case TAG_OLYMPUS_CAMERAID:
			strlcpy(ImageInfo->CameraId, value_ptr, sizeof(ImageInfo->CameraId));
			break;
*/
		case TAG_JPEG_INTERCHANGE_FORMAT:
			ImageInfo->ThumbnailOffset = (int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			break;

		case TAG_JPEG_INTERCHANGE_FORMAT_LEN:
			ImageInfo->ThumbnailSize = (int)exif_convert_any_format(value_ptr, format, ImageInfo->MotorolaOrder);
			/* no need to check for offset here: Exif says values have to be stored in tag order */
			exif_extract_thumbnail(ImageInfo, offset_base, IFDlength);
			break;

		case TAG_EXIF_IFD_POINTER:
		case TAG_GPS_IFD_POINTER:
		case TAG_INTEROP_IFD_POINTER:
			if ( ReadNextIFD) {
				char *SubdirStart;
				switch(tag) {
					case TAG_EXIF_IFD_POINTER:
						#ifdef EXIF_DEBUG
						php_error(E_NOTICE,"found EXIF");
						#endif
						ImageInfo->sections_found |= FOUND_EXIF;
						break;
					case TAG_GPS_IFD_POINTER:
						#ifdef EXIF_DEBUG
						php_error(E_NOTICE,"found GPS");
						#endif
						ImageInfo->sections_found |= FOUND_GPS;
						break;
					case TAG_INTEROP_IFD_POINTER:
						#ifdef EXIF_DEBUG
						php_error(E_NOTICE,"found INTEROPERABILITY");
						#endif
						ImageInfo->sections_found |= FOUND_INTEROP;
						return;
/*						break;*/
				}
				SubdirStart = offset_base + php_ifd_get32u(value_ptr, ImageInfo->MotorolaOrder);
				if (SubdirStart < offset_base || SubdirStart > offset_base+IFDlength) {
					php_error(E_WARNING, "Illegal IFD Pointer");
					return;
				}
				exif_process_IFD_in_JPEG(ImageInfo, SubdirStart, offset_base, IFDlength);
				/* continue? */
			}
			break;

		default:
			exif_add_image_info( &ImageInfo->ANY, exif_get_tagname(tag), tag, format, byte_count, value_ptr, ImageInfo->MotorolaOrder);
			break;
	}
}
/* }}} */

/* {{{ exif_process_IFD_in_JPEG
 * Process one of the nested IFDs directories. */
static void exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *DirStart, char *OffsetBase, unsigned IFDlength)
{
	int de;
	int NumDirEntries;
	int NextDirOffset;

	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"exif_process_IFD_in_JPEG(%d=x%04X)", IFDlength, IFDlength);
	#endif

	ImageInfo->sections_found |= FOUND_IFD0;

	NumDirEntries = php_ifd_get16u(DirStart, ImageInfo->MotorolaOrder);

	if ((DirStart+2+NumDirEntries*12) > (OffsetBase+IFDlength)) {
		php_error(E_WARNING, "Illegally sized directory");
		return;
	}

	ImageInfo->sections_found |= FOUND_ANY_TAG;
	for (de=0;de<NumDirEntries;de++) {
		exif_process_IFD_TAG(ImageInfo,DirStart+2+12*de,OffsetBase,IFDlength,1);
	}
	/*
	 * Hack to make it process IDF1 I hope
	 * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
	 */
	NextDirOffset = php_ifd_get32u(DirStart+2+12*de, ImageInfo->MotorolaOrder);
	if (NextDirOffset) {
		if (OffsetBase + NextDirOffset < OffsetBase || OffsetBase + NextDirOffset > OffsetBase+IFDlength) {
			php_error(E_WARNING, "Illegal directory offset");
			return;
		}
		/* That is the IFD for the first thumbnail
		#ifdef EXIF_DEBUG
		php_error(E_NOTICE,"expect next IFD to be thumbnail");
		#endif
		exif_process_IFD_in_JPEG(ImageInfo, OffsetBase + NextDirOffset, OffsetBase, IFDlength);
		*/
	}
}
/* }}} */

/* {{{ exif_process_TIFF_in_JPEG
   Process a TIFF header in a JPEG file
*/
static void exif_process_TIFF_in_JPEG(image_info_type *ImageInfo, char *CharBuf, unsigned int length)
{
	ImageInfo->FlashUsed = 0; /* If it s from a digicam, and it used flash, it says so. */

	ImageInfo->FocalplaneXRes = 0;
	ImageInfo->FocalplaneUnits = 0;
	ImageInfo->ExifImageWidth = 0;

	/* set the thumbnail stuff to nothing so we can test to see if they get set up */
	ImageInfo->Thumbnail = NULL;
	ImageInfo->ThumbnailSize = 0;

	if (memcmp(CharBuf, "II", 2) == 0) {
		ImageInfo->MotorolaOrder = 0;
	} else if (memcmp(CharBuf, "MM", 2) == 0) {
		ImageInfo->MotorolaOrder = 1;
	} else {
		php_error(E_WARNING, "Invalid TIFF alignment marker.");
		return;
	}

	/* Check the next two values for correctness. */
	if (php_ifd_get16u(CharBuf+2, ImageInfo->MotorolaOrder) != 0x2a
	  || php_ifd_get32u(CharBuf+4, ImageInfo->MotorolaOrder) != 0x08) {
		php_error(E_WARNING, "Invalid TIFF start (1)");
		return;
	}

	/* First directory starts at offset 8. Offsets starts at 0. */
	exif_process_IFD_in_JPEG(ImageInfo, CharBuf+8, CharBuf, length-14);

	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"exif_process_TIFF_in_JPEG, done");
	#endif

	/* MB: This is where I will make my attempt to get the tumbnail */


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
}
/* }}} */

/* {{{ exif_process_APP12
   Process an JPEG APP12 block marker used by OLYMPUS
*/
static void exif_process_APP12(image_info_type *ImageInfo, char *buffer, unsigned int length)
{
	int l1, l2=0;

	if ( (l1 = php_strnlen(buffer+2,length-2)) > 0) {
		exif_add_image_info( &ImageInfo->APP12, "Company", 1, TAG_FMT_STRING, l1, buffer+2, ImageInfo->MotorolaOrder);
		if ( length > 2+l1+1) {
			l2 = php_strnlen(buffer+2+l1+1,length-2-l1+1);
			exif_add_image_info( &ImageInfo->APP12, "Info", 1, TAG_FMT_STRING, l2, buffer+2+l1+1, ImageInfo->MotorolaOrder);
		}
		ImageInfo->sections_found |= FOUND_APP12;
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
	int HaveCom = FALSE;

	for(ImageInfo->sections_count=0;ImageInfo->sections_count < 19;) {
		int itemlen;
		int marker = 0;
		int ll, lh, got;
		uchar *Data;
		#ifdef EXIF_DEBUG
		size_t fpos;
		#endif

		#ifdef EXIF_DEBUG
		fpos = ftell(infile)-1;
		php_error(E_NOTICE,"process section at 0x%04X, len=%d", fpos, ImageInfo->sections_count);
		#endif

		for (a=0;a<7;a++) {
			marker = fgetc(infile);
			if (marker != 0xff) break;
		}
		#ifdef EXIF_DEBUG
		fpos = ftell(infile)-1;
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
		ImageInfo->sections[ImageInfo->sections_count].Data = Data;

		/* Store first two pre-read bytes. */
		Data[0] = (uchar)lh;
		Data[1] = (uchar)ll;

		got = fread(Data+2, 1, itemlen-2, infile); /* Read the whole section. */
		if (got != itemlen-2) {
			php_error(E_WARNING, "error reading from file: got=%d=x%04X != itemlen-2=%d=x%04X",got, got, itemlen-2, itemlen-2);
			return FALSE;
		}
		ImageInfo->sections_count += 1;

		#ifdef EXIF_DEBUG
		php_error(E_NOTICE,"process marker(x%02X=%s) at x%04X size %d", marker, exif_get_markername(marker), fpos, itemlen);
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
				return ImageInfo->sections_found ? TRUE : FALSE;

			case M_COM: /* Comment section */
				exif_process_COM(ImageInfo, Data, itemlen);
				break;

			case M_EXIF:
				ImageInfo->sections_found |= FOUND_EXIF;
				if (ImageInfo->sections_count <= 2) {
					/* Seen files from some 'U-lead' software with Vivitar scanner
					   that uses marker 31 later in the file (no clue what for!) */
					exif_process_APP1(ImageInfo, (char *)Data, itemlen);
				}
				break;
			case M_FPIX:
				ImageInfo->sections_found |= FOUND_FPIX;
				#ifdef EXIF_DEBUG
				php_error(E_NOTICE,"Found Flash Pix Extension Data");
				#endif
				//exif_process_TIFF_in_JPEG(ImageInfo,(char *)Data,itemlen);
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
static int exif_process_IFD_in_TIFF(image_info_type *ImageInfo, FILE *infile, size_t dir_offset)
{
	int i, sn, num_entries;
	unsigned char *dir_entry;
	size_t ifd_size, dir_size, entry_offset, next_offset, entry_length, entry_value;
	int entry_tag , entry_type;

	sn = ImageInfo->sections_count++;
	if ( ImageInfo->FileSize >= dir_offset+2) {
		ImageInfo->sections[sn].Size = 2;
		ImageInfo->sections[sn].Data = emalloc(ImageInfo->sections[sn].Size);
		fseek(infile,dir_offset,SEEK_SET); /* we do not know the order of sections */
		fread(ImageInfo->sections[sn].Data, 1, 2, infile);
	    num_entries = php_ifd_get16u(ImageInfo->sections[sn].Data, ImageInfo->MotorolaOrder);
		dir_size = 2/*num dir entries*/ +12/*length of entry*/*num_entries +4/* offset to next ifd (points to thumbnail or NULL)*/;
		#ifdef EXIF_DEBUG
		php_error(E_NOTICE,"Read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X), IFD entries(%d)", ImageInfo->FileSize, dir_offset, dir_size, num_entries);
		#endif
		if ( ImageInfo->FileSize >= dir_offset+dir_size) {
			ImageInfo->sections[sn].Size = dir_size;
			ImageInfo->sections[sn].Data = erealloc(ImageInfo->sections[sn].Data,ImageInfo->sections[sn].Size);
			fread(ImageInfo->sections[sn].Data+2, 1, dir_size-2, infile);
			next_offset = php_ifd_get32u(ImageInfo->sections[sn].Data + dir_size - 4, ImageInfo->MotorolaOrder);
			#ifdef EXIF_DEBUG
			php_error(E_NOTICE,"Read in TIFF done, next offset x%04X", next_offset);
			#endif
			/* now we have the directory we can look how long it should be */
			ifd_size = dir_size;
			for(i=0;i<num_entries;i++) {
				dir_entry 	 = ImageInfo->sections[sn].Data+2+i*12;
				entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->MotorolaOrder);
				entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->MotorolaOrder);
				if ( entry_type > NUM_FORMATS) {
					php_error(E_WARNING,"Error in TIFF: Illegal format, suppose bytes");
					entry_type = TAG_FMT_BYTE;
				}
				entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->MotorolaOrder) * php_tiff_bytes_per_format[entry_type];
				entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->MotorolaOrder) * php_tiff_bytes_per_format[entry_type];
				if ( entry_length > 4) {
					entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->MotorolaOrder);
					if ( entry_offset + entry_length > ifd_size) ifd_size = entry_offset + entry_length;
				} else {
					switch(entry_type) {
						case TAG_FMT_USHORT:
							entry_value  = php_ifd_get16u(dir_entry+8, ImageInfo->MotorolaOrder);
							break;
						case TAG_FMT_SSHORT:
							entry_value  = php_ifd_get16s(dir_entry+8, ImageInfo->MotorolaOrder);
							break;
						case TAG_FMT_ULONG:
							entry_value  = php_ifd_get32u(dir_entry+8, ImageInfo->MotorolaOrder);
							break;
						case TAG_FMT_SLONG:
							entry_value  = php_ifd_get32s(dir_entry+8, ImageInfo->MotorolaOrder);
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
				ImageInfo->sections_found |= FOUND_ANY_TAG;
				for(i=0;i<num_entries;i++) {
					dir_entry 	 = ImageInfo->sections[sn].Data+2+i*12;
					entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->MotorolaOrder);
					/*entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->MotorolaOrder);*/
					/*entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->MotorolaOrder);*/
					if (entry_tag == TAG_EXIF_IFD_POINTER ||
					    entry_tag == TAG_INTEROP_IFD_POINTER ||
					    entry_tag == TAG_GPS_IFD_POINTER
					   )
					{
						switch(entry_tag) {
							case TAG_EXIF_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_EXIF;
								break;
							case TAG_GPS_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_GPS;
								break;
							case TAG_INTEROP_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_INTEROP;
								break;
						}
						entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->MotorolaOrder);
						#ifdef EXIF_DEBUG
						php_error(E_NOTICE,"Found other IFD: %s at x%04X", exif_get_tagname(entry_tag), entry_offset);
						#endif
						exif_process_IFD_in_TIFF(ImageInfo,infile,entry_offset);
					} else {
						exif_process_IFD_TAG(ImageInfo,dir_entry,ImageInfo->sections[sn].Data-dir_offset,ifd_size,0);
					}
				}
				if (next_offset) {
					/* this should be a thumbnail IFD */
					/* the thumbnail itself is stored at Tag=StripOffsets */
					#ifdef EXIF_DEBUG
					php_error(E_NOTICE,"Read next IFD at x%04X", next_offset);
					#endif
					/*exif_process_IFD_in_TIFF(ImageInfo,infile,entry_offset);*/
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
		    if ( !memcmp(file_header,"II\x2A\x00\x08\x00\x00\x00", 8))
		    {
		    	ImageInfo->FileType = IMAGE_FILETYPE_TIFF;
		    	ImageInfo->MotorolaOrder = 0;
		    	#ifdef EXIF_DEBUG
		    	php_error(E_NOTICE,"File(%s) has TIFF/II format", ImageInfo->FileName);
		    	#endif
				ImageInfo->sections_found |= FOUND_IFD0;
		    	return exif_process_IFD_in_TIFF(ImageInfo,infile,8);
		    }
		    else
		    if ( !memcmp(file_header,"MM\x00\x2a\x00\x00\x00\x08", 8))
		    {
		    	ImageInfo->FileType = IMAGE_FILETYPE_TIFF;
		    	ImageInfo->MotorolaOrder = 1;
		    	#ifdef EXIF_DEBUG
		    	php_error(E_NOTICE,"File(%s) has TIFF/MM format", ImageInfo->FileName);
		    	#endif
				ImageInfo->sections_found |= FOUND_IFD0;
		    	return exif_process_IFD_in_TIFF(ImageInfo,infile,8);
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
		for (a=0;a<ImageInfo->sections_count-1;a++) {
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
	if (ImageInfo->UserComment)		efree(ImageInfo->UserComment);
	if (ImageInfo->Thumbnail) 		efree(ImageInfo->Thumbnail);
	if (ImageInfo->RawCopyright) 	efree(ImageInfo->RawCopyright);
	if (ImageInfo->numCopyrights) {
		for(i=0; i<ImageInfo->numCopyrights; i++) {
			efree((ImageInfo->Copyright)[i]);
		}
		ImageInfo->numCopyrights = 0;
	}
	if (ImageInfo->numComments) {
		for(i=0; i<ImageInfo->numComments; i++) {
			efree((ImageInfo->Comments)[i]);
		}
		ImageInfo->numComments = 0;
	}
	exif_free_image_info( &(ImageInfo->ANY));
	exif_free_image_info( &(ImageInfo->IFD0));
	exif_free_image_info( &(ImageInfo->APP1));
	exif_free_image_info( &(ImageInfo->EXIF));
	exif_free_image_info( &(ImageInfo->FPIX));
	exif_free_image_info( &(ImageInfo->GPS));
	exif_free_image_info( &(ImageInfo->INTEROP));
	exif_free_image_info( &(ImageInfo->APP12));
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

	ImageInfo->MotorolaOrder = 0;

	infile = VCWD_FOPEN(FileName, "rb"); /* Unix ignores 'b', windows needs it. */

	if (infile == NULL) {
		php_error(E_WARNING, "Unable to open '%s'", FileName);
		return FALSE;
	}
/*    CurrentFile = FileName; */

    /* php_error(E_WARNING,"EXIF: Process %s%s: %s", read_thumbnail?"thumbs ":"", read_all?"All ":"", FileName); /**/
	/* Start with an empty image information structure. */
	memset(ImageInfo, 0, sizeof(*ImageInfo));

	ImageInfo->FileName = php_basename(FileName, strlen(FileName), NULL, 0);
	ImageInfo->FocalLength = 0;
	ImageInfo->ExposureTime = 0;
	ImageInfo->ApertureFNumber = 0;
	ImageInfo->Distance = 0;
	ImageInfo->CCDWidth = 0;
	ImageInfo->FlashUsed = -1;
/*
	ImageInfo->SpecialMode = -1;
	ImageInfo->JpegQual = -1;
	ImageInfo->Macro = -1;
	ImageInfo->DigiZoom = -1;
*/

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
	ret = exif_scan_FILE_header(ImageInfo, infile);
	if (!ret) {
		php_error(E_WARNING, "Invalid JPEG/TIFF file: '%s'", FileName);
	} else {
		/*
		 * Thought this might pick out the embedded thumbnail, but it doesn't work.   -RL
		for (i=0;i<ImageInfo->sections_count-1;i++) {
			if (ImageInfo->sections[i].Type == M_EXIF) {
				thumbsize = ImageInfo->sections[i].Size;
				if(thumbsize>0) {
					ImageInfo->Thumbnail = emalloc(thumbsize+7);
					ImageInfo->ThumbnailSize = thumbsize;
					ImageInfo->Thumbnail[0] = 0xff;
					ImageInfo->Thumbnail[1] = 0xd8;
					ImageInfo->Thumbnail[2] = 0xff;
					memcpy(ImageInfo->Thumbnail+4, ImageInfo->sections[i].Data, thumbsize+4);
				}
			}
		}
		*/
	}
	/* discard sections not yet image info */
	php_exif_discard_sections(ImageInfo);

	fclose(infile);

	return ret;
}
/* }}} */

/* {{{ proto array read_exif_data(string filename [, read_thumbnails [, int read_all]])
   Reads the EXIF header data from the JPEG identified by filename and optionally reads the internal thumbnails */
PHP_FUNCTION(read_exif_data)
{
	pval **p_name, **p_read_thumbnail, **p_sections_needed, **p_read_all;
	int i, ac = ZEND_NUM_ARGS(), ret, sections_needed=0, read_thumbnail=0, read_all=0;
	image_info_type ImageInfo;
	char tmp[64], sections_str[64];

	if ((ac < 1 || ac > 2) || zend_get_parameters_ex(ac, &p_name, &p_sections_needed, &p_read_thumbnail, &p_read_all) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(p_name);

	if(ac >= 2) {
		convert_to_string_ex(p_sections_needed);
		strlcpy(sections_str,Z_STRVAL_PP(p_sections_needed),sizeof(sections_str)-1/*!!*/);
		sections_str[strlen(sections_str)+1] = '\0';
		sections_str[strlen(sections_str)+0] = ',';
		sections_needed |= strstr(sections_str,"ANY_TAG,")   ? FOUND_ANY_TAG   : 0;
		sections_needed |= strstr(sections_str,"IFD0,")      ? FOUND_IFD0      : 0;
		sections_needed |= strstr(sections_str,"THUMBNAIL,") ? FOUND_THUMBNAIL : 0;
		sections_needed |= strstr(sections_str,"COMMENT,")   ? FOUND_COMMENT   : 0;
		sections_needed |= strstr(sections_str,"EXIF,")      ? FOUND_EXIF      : 0;
		sections_needed |= strstr(sections_str,"GPS,")       ? FOUND_GPS       : 0;
		sections_needed |= strstr(sections_str,"INTEROP,")   ? FOUND_INTEROP   : 0;
		sections_needed |= strstr(sections_str,"FPIX,")      ? FOUND_FPIX      : 0;
		sections_needed |= strstr(sections_str,"APP12,")     ? FOUND_APP12     : 0;
    	#ifdef EXIF_DEBUG
		sprintf(sections_str,"%s%s%s%s%s%s%s%s%s",
			(sections_needed&FOUND_ANY_TAG) ? "ANY_TAG,"   : "",
			(sections_needed&FOUND_IFD0)    ? "IFD0,"      : "",
			(sections_needed&FOUND_IFD0)    ? "THUMBNAIL," : "",
			(sections_needed&FOUND_COMMENT) ? "COMMENT,"   : "",
			(sections_needed&FOUND_EXIF)    ? "EXIF,"      : "",
			(sections_needed&FOUND_GPS)     ? "GPS,"       : "",
			(sections_needed&FOUND_INTEROP) ? "INTEROP,"   : "",
			(sections_needed&FOUND_FPIX)    ? "FPIX,"      : "",
			(sections_needed&FOUND_APP12)   ? "APP12,"     : ""
		);
		if ( (i=strlen(sections_str)) > 0) sections_str[i-1] = '\0'; /* skip last ',' */
    	php_error(E_NOTICE,"Sections needed: %s", sections_str[0] ? sections_str : "None");
		#endif
	}
	if(ac >= 3) {
		convert_to_long_ex(p_read_thumbnail);
		read_thumbnail = Z_LVAL_PP(p_read_thumbnail);
	}
	if(ac >= 4) {
		convert_to_long_ex(p_read_all);
		read_all = Z_LVAL_PP(p_read_all);
	}
	/* parameters 3,4 will be working in later versions.... */
	read_thumbnail = 0; /* just to make function work for 4.2 tree */
	read_all = 0;       /* just to make function work for 4.2 tree */

	ret = php_exif_read_file(&ImageInfo, Z_STRVAL_PP(p_name), read_thumbnail, read_all TSRMLS_CC);

	sprintf(sections_str,"%s%s%s%s%s%s%s%s%s",
		(ImageInfo.sections_found&FOUND_ANY_TAG) ? "ANY_TAG,"   : "",
		(ImageInfo.sections_found&FOUND_IFD0)    ? "IFD0,"      : "",
		(ImageInfo.sections_found&FOUND_COMMENT) ? "THUMBNAIL," : "",
		(ImageInfo.sections_found&FOUND_COMMENT) ? "COMMENT,"   : "",
		(ImageInfo.sections_found&FOUND_EXIF)    ? "EXIF,"      : "",
		(ImageInfo.sections_found&FOUND_GPS)     ? "GPS,"       : "",
		(ImageInfo.sections_found&FOUND_INTEROP) ? "INTEROP,"   : "",
		(ImageInfo.sections_found&FOUND_FPIX)    ? "FPIX,"      : "",
		(ImageInfo.sections_found&FOUND_APP12)   ? "APP12,"     : ""
	);
	if ( (i=strlen(sections_str)) > 0) sections_str[i-1] = '\0'; /* skip last ',' */
	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Sections found: %s", sections_str[0] ? sections_str : "None");
	#endif

	if (ret==FALSE || array_init(return_value) == FAILURE || (sections_needed && !(sections_needed&ImageInfo.sections_found))) {
		php_exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Returning information");
	#endif

	/* generic created information */
	add_assoc_string(return_value, "FileName", ImageInfo.FileName, 1);
	add_assoc_long(return_value, "FileDateTime", ImageInfo.FileDateTime);
	add_assoc_long(return_value, "FileSize", ImageInfo.FileSize);
	add_assoc_string(return_value, "SectionsFound", sections_str, 1);
	/* ifd0/comment/exif/gps/interop information */
	if (ImageInfo.Width>0 &&  ImageInfo.Height>0) {
		sprintf(tmp, "width=\"%d\" height=\"%d\"", ImageInfo.Width, ImageInfo.Height);
		add_assoc_string(return_value, "html", tmp, 1);
		add_assoc_long(return_value, "Height", ImageInfo.Height);
		add_assoc_long(return_value, "Width", ImageInfo.Width);
	}
	add_assoc_long(return_value, "IsColor", ImageInfo.IsColor);
	if(ImageInfo.FlashUsed >= 0) {
		add_assoc_long(return_value, "FlashUsed", ImageInfo.FlashUsed);
	}
	if (ImageInfo.FocalLength) {
		sprintf(tmp, "%4.1fmm", ImageInfo.FocalLength);
		add_assoc_string(return_value, "FocalLength", tmp, 1);
		if(ImageInfo.CCDWidth) {
			sprintf(tmp, "%dmm", (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*35+0.5));
			add_assoc_string(return_value, "35mmFocalLength", tmp, 1);
		}
		add_assoc_double(return_value, "RawFocalLength", ImageInfo.FocalLength);
	}
	if(ImageInfo.ExposureTime) {
		if(ImageInfo.ExposureTime <= 0.5) {
			sprintf(tmp, "%6.3f s (1/%d)", ImageInfo.ExposureTime, (int)(0.5 + 1/ImageInfo.ExposureTime));
		} else {
			sprintf(tmp, "%6.3f s", ImageInfo.ExposureTime);
		}
		add_assoc_string(return_value, "ExposureTime", tmp, 1);
		add_assoc_double(return_value, "RawExposureTime", ImageInfo.ExposureTime);
	}
	if(ImageInfo.ApertureFNumber) {
		sprintf(tmp, "f/%4.1f", ImageInfo.ApertureFNumber);
		add_assoc_string(return_value, "ApertureFNumber", tmp, 1);
		add_assoc_double(return_value, "RawApertureFNumber", ImageInfo.ApertureFNumber);
	}
	if(ImageInfo.Distance) {
		if(ImageInfo.Distance<0) {
			add_assoc_string(return_value, "FocusDistance", "Infinite", 1);
		} else {
			sprintf(tmp, "%5.2fm", ImageInfo.Distance);
			add_assoc_string(return_value, "FocusDistance", tmp, 1);
		}
		add_assoc_double(return_value, "RawFocusDistance", ImageInfo.Distance);
	}
	if(ImageInfo.CCDWidth) {
		add_assoc_double(return_value, "CCDWidth", ImageInfo.CCDWidth);
	}
	if(ImageInfo.Orientation) {
		add_assoc_long(return_value, "Orientation", ImageInfo.Orientation);
	}
	if(ImageInfo.ISOspeed) {
		add_assoc_long(return_value, "ISOspeed", ImageInfo.ISOspeed);
	}
	if (ImageInfo.RawCopyright || ImageInfo.numCopyrights) {
		if (ImageInfo.RawCopyright && !ImageInfo.numCopyrights && ImageInfo.lenRawCopyright) {
			add_assoc_stringl(return_value, "Copyright", ImageInfo.RawCopyright, ImageInfo.lenRawCopyright, 1);
		}else{
			pval *tmpi;

			MAKE_STD_ZVAL(tmpi);
			array_init(tmpi);
			add_assoc_string(tmpi, "Photographer", ImageInfo.Copyright[0], 1);
			add_assoc_string(tmpi, "Editor",       ImageInfo.Copyright[1], 1);
			add_assoc_zval(return_value, "Copyright", tmpi);
		}
	}
	if(ImageInfo.numComments) {
		if(ImageInfo.numComments==1) {
			add_assoc_string(return_value, "Comments", (ImageInfo.Comments)[0], 1);
		} else {
			pval *tmpi;

			MAKE_STD_ZVAL(tmpi);
			array_init(tmpi);
			for(i=0; i<ImageInfo.numComments; i++) {
				add_index_string(tmpi, i, (ImageInfo.Comments)[i], 0);
			}
			add_assoc_zval(return_value, "Comments", tmpi);
		}
	}
	if (ImageInfo.UserComment) {
		add_assoc_string(return_value, "UserComment", ImageInfo.UserComment, 1);
		if ( ImageInfo.UserCommentEncoding[0]) {
			add_assoc_string(return_value, "UserCommentEncoding", ImageInfo.UserCommentEncoding, 1);
		}
	}
	if (ImageInfo.ThumbnailSize) {
		add_assoc_long(return_value, "ThumbnailSize", ImageInfo.ThumbnailSize);
	}
	if(ImageInfo.Thumbnail && ImageInfo.ThumbnailSize) {
		add_assoc_stringl(return_value, "Thumbnail", ImageInfo.Thumbnail, ImageInfo.ThumbnailSize, 1);
	}

	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"adding image infos");
	#endif

	add_assoc_image_info( return_value, 0, "ANY",     &(ImageInfo.ANY));
	add_assoc_image_info( return_value, 0, "IFD0",    &(ImageInfo.IFD0));
	add_assoc_image_info( return_value, 0, "APP1",    &(ImageInfo.APP1));
	add_assoc_image_info( return_value, 0, "EXIF",    &(ImageInfo.EXIF));
	add_assoc_image_info( return_value, 0, "FPIX",    &(ImageInfo.FPIX));
	add_assoc_image_info( return_value, 0, "GPS",     &(ImageInfo.GPS));
	add_assoc_image_info( return_value, 0, "INTEROP", &(ImageInfo.INTEROP));
	add_assoc_image_info( return_value, 0, "APP12",   &(ImageInfo.APP12));

	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"Discarding info");
	#endif

	php_exif_discard_imageinfo(&ImageInfo);

	#ifdef EXIF_DEBUG
	php_error(E_NOTICE,"read_exif_data done");
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
