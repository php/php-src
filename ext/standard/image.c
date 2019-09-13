/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
#include <stdio.h>
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include "fopen_wrappers.h"
#include "ext/standard/fsock.h"
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "php_image.h"
#ifdef PHP_WIN32
#include "win32/php_stdint.h"
#endif

#if HAVE_ZLIB && !defined(COMPILE_DL_ZLIB)
#include "zlib.h"
#endif

/* file type markers */
PHPAPI const char php_sig_gif[3] = {'G', 'I', 'F'};
PHPAPI const char php_sig_psd[4] = {'8', 'B', 'P', 'S'};
PHPAPI const char php_sig_bmp[2] = {'B', 'M'};
PHPAPI const char php_sig_swf[3] = {'F', 'W', 'S'};
PHPAPI const char php_sig_swc[3] = {'C', 'W', 'S'};
PHPAPI const char php_sig_jpg[3] = {(char) 0xff, (char) 0xd8, (char) 0xff};
PHPAPI const char php_sig_png[8] = {(char) 0x89, (char) 0x50, (char) 0x4e, (char) 0x47,
                                    (char) 0x0d, (char) 0x0a, (char) 0x1a, (char) 0x0a};
PHPAPI const char php_sig_tif_ii[4] = {'I','I', (char)0x2A, (char)0x00};
PHPAPI const char php_sig_tif_mm[4] = {'M','M', (char)0x00, (char)0x2A};
PHPAPI const char php_sig_jpc[3]  = {(char)0xff, (char)0x4f, (char)0xff};
PHPAPI const char php_sig_jp2[12] = {(char)0x00, (char)0x00, (char)0x00, (char)0x0c,
                                     (char)0x6a, (char)0x50, (char)0x20, (char)0x20,
                                     (char)0x0d, (char)0x0a, (char)0x87, (char)0x0a};
PHPAPI const char php_sig_iff[4] = {'F','O','R','M'};
PHPAPI const char php_sig_ico[4] = {(char)0x00, (char)0x00, (char)0x01, (char)0x00};
PHPAPI const char php_sig_riff[4] = {'R', 'I', 'F', 'F'};
PHPAPI const char php_sig_webp[4] = {'W', 'E', 'B', 'P'};

/* REMEMBER TO ADD MIME-TYPE TO FUNCTION php_image_type_to_mime_type */
/* PCX must check first 64bytes and byte 0=0x0a and byte2 < 0x06 */

/* return info as a struct, to make expansion easier */

struct gfxinfo {
	unsigned int width;
	unsigned int height;
	unsigned int bits;
	unsigned int channels;
};

/* {{{ PHP_MINIT_FUNCTION(imagetypes)
 * Register IMAGETYPE_<xxx> constants used by GetImageSize(), image_type_to_mime_type, ext/exif */
PHP_MINIT_FUNCTION(imagetypes)
{
	REGISTER_LONG_CONSTANT("IMAGETYPE_GIF",     IMAGE_FILETYPE_GIF,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPEG",    IMAGE_FILETYPE_JPEG,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_PNG",     IMAGE_FILETYPE_PNG,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_SWF",     IMAGE_FILETYPE_SWF,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_PSD",     IMAGE_FILETYPE_PSD,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_BMP",     IMAGE_FILETYPE_BMP,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_TIFF_II", IMAGE_FILETYPE_TIFF_II, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_TIFF_MM", IMAGE_FILETYPE_TIFF_MM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPC",     IMAGE_FILETYPE_JPC,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JP2",     IMAGE_FILETYPE_JP2,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPX",     IMAGE_FILETYPE_JPX,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JB2",     IMAGE_FILETYPE_JB2,     CONST_CS | CONST_PERSISTENT);
#if HAVE_ZLIB && !defined(COMPILE_DL_ZLIB)
	REGISTER_LONG_CONSTANT("IMAGETYPE_SWC",     IMAGE_FILETYPE_SWC,     CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("IMAGETYPE_IFF",     IMAGE_FILETYPE_IFF,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_WBMP",    IMAGE_FILETYPE_WBMP,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPEG2000",IMAGE_FILETYPE_JPC,     CONST_CS | CONST_PERSISTENT); /* keep alias */
	REGISTER_LONG_CONSTANT("IMAGETYPE_XBM",     IMAGE_FILETYPE_XBM,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_ICO",     IMAGE_FILETYPE_ICO,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_WEBP",	IMAGE_FILETYPE_WEBP,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_UNKNOWN", IMAGE_FILETYPE_UNKNOWN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_COUNT",   IMAGE_FILETYPE_COUNT,   CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ php_handle_gif
 * routine to handle GIF files. If only everything were that easy... ;} */
static struct gfxinfo *php_handle_gif (php_stream * stream)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[5];

	if (php_stream_seek(stream, 3, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, (char*)dim, sizeof(dim)) != sizeof(dim))
		return NULL;

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
	result->width    = (unsigned int)dim[0] | (((unsigned int)dim[1])<<8);
	result->height   = (unsigned int)dim[2] | (((unsigned int)dim[3])<<8);
	result->bits     = dim[4]&0x80 ? ((((unsigned int)dim[4])&0x07) + 1) : 0;
	result->channels = 3; /* always */

	return result;
}
/* }}} */

/* {{{ php_handle_psd
 */
static struct gfxinfo *php_handle_psd (php_stream * stream)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[8];

	if (php_stream_seek(stream, 11, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, (char*)dim, sizeof(dim)) != sizeof(dim))
		return NULL;

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
	result->height   =  (((unsigned int)dim[0]) << 24) + (((unsigned int)dim[1]) << 16) + (((unsigned int)dim[2]) << 8) + ((unsigned int)dim[3]);
	result->width    =  (((unsigned int)dim[4]) << 24) + (((unsigned int)dim[5]) << 16) + (((unsigned int)dim[6]) << 8) + ((unsigned int)dim[7]);

	return result;
}
/* }}} */

/* {{{ php_handle_bmp
 */
static struct gfxinfo *php_handle_bmp (php_stream * stream)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[16];
	int size;

	if (php_stream_seek(stream, 11, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, (char*)dim, sizeof(dim)) != sizeof(dim))
		return NULL;

	size   = (((unsigned int)dim[ 3]) << 24) + (((unsigned int)dim[ 2]) << 16) + (((unsigned int)dim[ 1]) << 8) + ((unsigned int) dim[ 0]);
	if (size == 12) {
		result = (struct gfxinfo *) ecalloc (1, sizeof(struct gfxinfo));
		result->width    =  (((unsigned int)dim[ 5]) << 8) + ((unsigned int) dim[ 4]);
		result->height   =  (((unsigned int)dim[ 7]) << 8) + ((unsigned int) dim[ 6]);
		result->bits     =  ((unsigned int)dim[11]);
	} else if (size > 12 && (size <= 64 || size == 108 || size == 124)) {
		result = (struct gfxinfo *) ecalloc (1, sizeof(struct gfxinfo));
		result->width    =  (((unsigned int)dim[ 7]) << 24) + (((unsigned int)dim[ 6]) << 16) + (((unsigned int)dim[ 5]) << 8) + ((unsigned int) dim[ 4]);
		result->height   =  (((unsigned int)dim[11]) << 24) + (((unsigned int)dim[10]) << 16) + (((unsigned int)dim[ 9]) << 8) + ((unsigned int) dim[ 8]);
		result->height   =  abs((int32_t)result->height);
		result->bits     =  (((unsigned int)dim[15]) <<  8) +  ((unsigned int)dim[14]);
	} else {
		return NULL;
	}

	return result;
}
/* }}} */

/* {{{ php_swf_get_bits
 * routines to handle SWF files. */
static unsigned long int php_swf_get_bits (unsigned char* buffer, unsigned int pos, unsigned int count)
{
	unsigned int loop;
	unsigned long int result = 0;

	for (loop = pos; loop < pos + count; loop++)
	{
		result = result +
			((((buffer[loop / 8]) >> (7 - (loop % 8))) & 0x01) << (count - (loop - pos) - 1));
	}
	return result;
}
/* }}} */

#if HAVE_ZLIB && !defined(COMPILE_DL_ZLIB)
/* {{{ php_handle_swc
 */
static struct gfxinfo *php_handle_swc(php_stream * stream)
{
	struct gfxinfo *result = NULL;

	long bits;
	unsigned char a[64];
	unsigned long len=64, szlength;
	int factor = 1,maxfactor = 16;
	int status = 0;
	unsigned char *b, *buf = NULL;
	zend_string *bufz;

	if (php_stream_seek(stream, 5, SEEK_CUR)) {
		return NULL;
	}

	if (php_stream_read(stream, (char *) a, sizeof(a)) != sizeof(a)) {
		return NULL;
	}

	b = ecalloc(1, len + 1);

	if (uncompress(b, &len, a, sizeof(a)) != Z_OK) {
		/* failed to decompress the file, will try reading the rest of the file */
		if (php_stream_seek(stream, 8, SEEK_SET)) {
			efree(b);
			return NULL;
		}

		bufz = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);

		if (!bufz) {
			efree(b);
			return NULL;
		}

		/*
		 * zlib::uncompress() wants to know the output data length
		 * if none was given as a parameter
		 * we try from input length * 2 up to input length * 2^8
		 * doubling it whenever it wasn't big enough
		 * that should be eneugh for all real life cases
		*/

		do {
			szlength = ZSTR_LEN(bufz) * (1<<factor++);
			buf = erealloc(buf, szlength);
			status = uncompress(buf, &szlength, (unsigned char *) ZSTR_VAL(bufz), ZSTR_LEN(bufz));
		} while ((status==Z_BUF_ERROR)&&(factor<maxfactor));

		if (bufz) {
			zend_string_release_ex(bufz, 0);
		}

		if (status == Z_OK) {
			 memcpy(b, buf, len);
		}

		if (buf) {
			efree(buf);
		}
	}

	if (!status) {
		result = (struct gfxinfo *) ecalloc (1, sizeof (struct gfxinfo));
		bits = php_swf_get_bits (b, 0, 5);
		result->width = (php_swf_get_bits (b, 5 + bits, bits) -
			php_swf_get_bits (b, 5, bits)) / 20;
		result->height = (php_swf_get_bits (b, 5 + (3 * bits), bits) -
			php_swf_get_bits (b, 5 + (2 * bits), bits)) / 20;
	} else {
		result = NULL;
	}

	efree (b);
	return result;
}
/* }}} */
#endif

/* {{{ php_handle_swf
 */
static struct gfxinfo *php_handle_swf (php_stream * stream)
{
	struct gfxinfo *result = NULL;
	long bits;
	unsigned char a[32];

	if (php_stream_seek(stream, 5, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, (char*)a, sizeof(a)) != sizeof(a))
		return NULL;

	result = (struct gfxinfo *) ecalloc (1, sizeof (struct gfxinfo));
	bits = php_swf_get_bits (a, 0, 5);
	result->width = (php_swf_get_bits (a, 5 + bits, bits) -
		php_swf_get_bits (a, 5, bits)) / 20;
	result->height = (php_swf_get_bits (a, 5 + (3 * bits), bits) -
		php_swf_get_bits (a, 5 + (2 * bits), bits)) / 20;
	result->bits     = 0;
	result->channels = 0;
	return result;
}
/* }}} */

/* {{{ php_handle_png
 * routine to handle PNG files */
static struct gfxinfo *php_handle_png (php_stream * stream)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[9];
/* Width:              4 bytes
 * Height:             4 bytes
 * Bit depth:          1 byte
 * Color type:         1 byte
 * Compression method: 1 byte
 * Filter method:      1 byte
 * Interlace method:   1 byte
 */

	if (php_stream_seek(stream, 8, SEEK_CUR))
		return NULL;

	if((php_stream_read(stream, (char*)dim, sizeof(dim))) < sizeof(dim))
		return NULL;

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
	result->width  = (((unsigned int)dim[0]) << 24) + (((unsigned int)dim[1]) << 16) + (((unsigned int)dim[2]) << 8) + ((unsigned int)dim[3]);
	result->height = (((unsigned int)dim[4]) << 24) + (((unsigned int)dim[5]) << 16) + (((unsigned int)dim[6]) << 8) + ((unsigned int)dim[7]);
	result->bits   = (unsigned int)dim[8];
	return result;
}
/* }}} */

/* routines to handle JPEG data */

/* some defines for the different JPEG block types */
#define M_SOF0  0xC0			/* Start Of Frame N */
#define M_SOF1  0xC1			/* N indicates which compression process */
#define M_SOF2  0xC2			/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5			/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8
#define M_EOI   0xD9			/* End Of Image (end of datastream) */
#define M_SOS   0xDA			/* Start Of Scan (begins compressed data) */
#define M_APP0  0xe0
#define M_APP1  0xe1
#define M_APP2  0xe2
#define M_APP3  0xe3
#define M_APP4  0xe4
#define M_APP5  0xe5
#define M_APP6  0xe6
#define M_APP7  0xe7
#define M_APP8  0xe8
#define M_APP9  0xe9
#define M_APP10 0xea
#define M_APP11 0xeb
#define M_APP12 0xec
#define M_APP13 0xed
#define M_APP14 0xee
#define M_APP15 0xef
#define M_COM   0xFE            /* COMment                                  */

#define M_PSEUDO 0xFFD8			/* pseudo marker for start of image(byte 0) */

/* {{{ php_read2
 */
static unsigned short php_read2(php_stream * stream)
{
	unsigned char a[2];

	/* return 0 if we couldn't read enough data */
	if((php_stream_read(stream, (char *) a, sizeof(a))) < sizeof(a)) return 0;

	return (((unsigned short)a[0]) << 8) + ((unsigned short)a[1]);
}
/* }}} */

/* {{{ php_next_marker
 * get next marker byte from file */
static unsigned int php_next_marker(php_stream * stream, int last_marker, int ff_read)
{
	int a=0, marker;

	/* get marker byte, swallowing possible padding                           */
	if (!ff_read) {
		size_t extraneous = 0;

		while ((marker = php_stream_getc(stream)) != 0xff) {
			if (marker == EOF) {
				return M_EOI;/* we hit EOF */
	}
			extraneous++;
	}
		if (extraneous) {
			php_error_docref(NULL, E_WARNING, "corrupt JPEG data: %zu extraneous bytes before marker", extraneous);
		}
	}
	a = 1;
	do {
		if ((marker = php_stream_getc(stream)) == EOF)
		{
			return M_EOI;/* we hit EOF */
		}
		a++;
	} while (marker == 0xff);
	if (a < 2)
	{
		return M_EOI; /* at least one 0xff is needed before marker code */
	}
	return (unsigned int)marker;
}
/* }}} */

/* {{{ php_skip_variable
 * skip over a variable-length block; assumes proper length marker */
static int php_skip_variable(php_stream * stream)
{
	zend_off_t length = ((unsigned int)php_read2(stream));

	if (length < 2)	{
		return 0;
	}
	length = length - 2;
	php_stream_seek(stream, (zend_long)length, SEEK_CUR);
	return 1;
}
/* }}} */

/* {{{ php_read_APP
 */
static int php_read_APP(php_stream * stream, unsigned int marker, zval *info)
{
	unsigned short length;
	char *buffer;
	char markername[16];
	zval *tmp;

	length = php_read2(stream);
	if (length < 2)	{
		return 0;
	}
	length -= 2;				/* length includes itself */

	buffer = emalloc((size_t)length);

	if (php_stream_read(stream, buffer, (size_t) length) != length) {
		efree(buffer);
		return 0;
	}

	snprintf(markername, sizeof(markername), "APP%d", marker - M_APP0);

	if ((tmp = zend_hash_str_find(Z_ARRVAL_P(info), markername, strlen(markername))) == NULL) {
		/* XXX we only catch the 1st tag of it's kind! */
		add_assoc_stringl(info, markername, buffer, length);
	}

	efree(buffer);
	return 1;
}
/* }}} */

/* {{{ php_handle_jpeg
   main loop to parse JPEG structure */
static struct gfxinfo *php_handle_jpeg (php_stream * stream, zval *info)
{
	struct gfxinfo *result = NULL;
	unsigned int marker = M_PSEUDO;
	unsigned short length, ff_read=1;

	for (;;) {
		marker = php_next_marker(stream, marker, ff_read);
		ff_read = 0;
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
				if (result == NULL) {
					/* handle SOFn block */
					result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
					length = php_read2(stream);
					result->bits     = php_stream_getc(stream);
					result->height   = php_read2(stream);
					result->width    = php_read2(stream);
					result->channels = php_stream_getc(stream);
					if (!info || length < 8) { /* if we don't want an extanded info -> return */
						return result;
					}
					if (php_stream_seek(stream, length - 8, SEEK_CUR)) { /* file error after info */
						return result;
					}
				} else {
					if (!php_skip_variable(stream)) {
						return result;
					}
				}
				break;

			case M_APP0:
			case M_APP1:
			case M_APP2:
			case M_APP3:
			case M_APP4:
			case M_APP5:
			case M_APP6:
			case M_APP7:
			case M_APP8:
			case M_APP9:
			case M_APP10:
			case M_APP11:
			case M_APP12:
			case M_APP13:
			case M_APP14:
			case M_APP15:
				if (info) {
					if (!php_read_APP(stream, marker, info)) { /* read all the app marks... */
						return result;
					}
				} else {
					if (!php_skip_variable(stream)) {
						return result;
					}
				}
				break;

			case M_SOS:
			case M_EOI:
				return result;	/* we're about to hit image data, or are at EOF. stop processing. */

			default:
				if (!php_skip_variable(stream)) { /* anything else isn't interesting */
					return result;
				}
				break;
		}
	}

	return result; /* perhaps image broken -> no info but size */
}
/* }}} */

/* {{{ php_read4
 */
static unsigned int php_read4(php_stream * stream)
{
	unsigned char a[4];

	/* just return 0 if we hit the end-of-file */
	if ((php_stream_read(stream, (char*)a, sizeof(a))) != sizeof(a)) return 0;

	return (((unsigned int)a[0]) << 24)
	     + (((unsigned int)a[1]) << 16)
	     + (((unsigned int)a[2]) <<  8)
	     + (((unsigned int)a[3]));
}
/* }}} */

/* {{{ JPEG 2000 Marker Codes */
#define JPEG2000_MARKER_PREFIX 0xFF /* All marker codes start with this */
#define JPEG2000_MARKER_SOC 0x4F /* Start of Codestream */
#define JPEG2000_MARKER_SOT 0x90 /* Start of Tile part */
#define JPEG2000_MARKER_SOD 0x93 /* Start of Data */
#define JPEG2000_MARKER_EOC 0xD9 /* End of Codestream */
#define JPEG2000_MARKER_SIZ 0x51 /* Image and tile size */
#define JPEG2000_MARKER_COD 0x52 /* Coding style default */
#define JPEG2000_MARKER_COC 0x53 /* Coding style component */
#define JPEG2000_MARKER_RGN 0x5E /* Region of interest */
#define JPEG2000_MARKER_QCD 0x5C /* Quantization default */
#define JPEG2000_MARKER_QCC 0x5D /* Quantization component */
#define JPEG2000_MARKER_POC 0x5F /* Progression order change */
#define JPEG2000_MARKER_TLM 0x55 /* Tile-part lengths */
#define JPEG2000_MARKER_PLM 0x57 /* Packet length, main header */
#define JPEG2000_MARKER_PLT 0x58 /* Packet length, tile-part header */
#define JPEG2000_MARKER_PPM 0x60 /* Packed packet headers, main header */
#define JPEG2000_MARKER_PPT 0x61 /* Packed packet headers, tile part header */
#define JPEG2000_MARKER_SOP 0x91 /* Start of packet */
#define JPEG2000_MARKER_EPH 0x92 /* End of packet header */
#define JPEG2000_MARKER_CRG 0x63 /* Component registration */
#define JPEG2000_MARKER_COM 0x64 /* Comment */
/* }}} */

/* {{{ php_handle_jpc
   Main loop to parse JPEG2000 raw codestream structure */
static struct gfxinfo *php_handle_jpc(php_stream * stream)
{
	struct gfxinfo *result = NULL;
	int highest_bit_depth, bit_depth;
	unsigned char first_marker_id;
	unsigned int i;

	/* JPEG 2000 components can be vastly different from one another.
	   Each component can be sampled at a different resolution, use
	   a different colour space, have a separate colour depth, and
	   be compressed totally differently! This makes giving a single
	   "bit depth" answer somewhat problematic. For this implementation
	   we'll use the highest depth encountered. */

	/* Get the single byte that remains after the file type identification */
	first_marker_id = php_stream_getc(stream);

	/* Ensure that this marker is SIZ (as is mandated by the standard) */
	if (first_marker_id != JPEG2000_MARKER_SIZ) {
		php_error_docref(NULL, E_WARNING, "JPEG2000 codestream corrupt(Expected SIZ marker not found after SOC)");
		return NULL;
	}

	result = (struct gfxinfo *)ecalloc(1, sizeof(struct gfxinfo));

	php_read2(stream); /* Lsiz */
	php_read2(stream); /* Rsiz */
	result->width = php_read4(stream); /* Xsiz */
	result->height = php_read4(stream); /* Ysiz */

#if MBO_0
	php_read4(stream); /* XOsiz */
	php_read4(stream); /* YOsiz */
	php_read4(stream); /* XTsiz */
	php_read4(stream); /* YTsiz */
	php_read4(stream); /* XTOsiz */
	php_read4(stream); /* YTOsiz */
#else
	if (php_stream_seek(stream, 24, SEEK_CUR)) {
		efree(result);
		return NULL;
	}
#endif

	result->channels = php_read2(stream); /* Csiz */
	if ((result->channels == 0 && php_stream_eof(stream)) || result->channels > 256) {
		efree(result);
		return NULL;
	}

	/* Collect bit depth info */
	highest_bit_depth = 0;
	for (i = 0; i < result->channels; i++) {
		bit_depth = php_stream_getc(stream); /* Ssiz[i] */
		bit_depth++;
		if (bit_depth > highest_bit_depth) {
			highest_bit_depth = bit_depth;
		}

		php_stream_getc(stream); /* XRsiz[i] */
		php_stream_getc(stream); /* YRsiz[i] */
	}

	result->bits = highest_bit_depth;

	return result;
}
/* }}} */

/* {{{ php_handle_jp2
   main loop to parse JPEG 2000 JP2 wrapper format structure */
static struct gfxinfo *php_handle_jp2(php_stream *stream)
{
	struct gfxinfo *result = NULL;
	unsigned int box_length;
	unsigned int box_type;
	char jp2c_box_id[] = {(char)0x6a, (char)0x70, (char)0x32, (char)0x63};

	/* JP2 is a wrapper format for JPEG 2000. Data is contained within "boxes".
	   Boxes themselves can be contained within "super-boxes". Super-Boxes can
	   contain super-boxes which provides us with a hierarchical storage system.

	   It is valid for a JP2 file to contain multiple individual codestreams.
	   We'll just look for the first codestream at the root of the box structure
	   and handle that.
	*/

	for (;;)
	{
		box_length = php_read4(stream); /* LBox */
		/* TBox */
		if (php_stream_read(stream, (void *)&box_type, sizeof(box_type)) != sizeof(box_type)) {
			/* Use this as a general "out of stream" error */
			break;
		}

		if (box_length == 1) {
			/* We won't handle XLBoxes */
			return NULL;
		}

		if (!memcmp(&box_type, jp2c_box_id, 4))
		{
			/* Skip the first 3 bytes to emulate the file type examination */
			php_stream_seek(stream, 3, SEEK_CUR);

			result = php_handle_jpc(stream);
			break;
		}

		/* Stop if this was the last box */
		if ((int)box_length <= 0) {
			break;
		}

		/* Skip over LBox (Which includes both TBox and LBox itself */
		if (php_stream_seek(stream, box_length - 8, SEEK_CUR)) {
			break;
		}
	}

	if (result == NULL) {
		php_error_docref(NULL, E_WARNING, "JP2 file has no codestreams at root level");
	}

	return result;
}
/* }}} */

/* {{{ tiff constants
 */
PHPAPI const int php_tiff_bytes_per_format[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

/* uncompressed only */
#define TAG_IMAGEWIDTH              0x0100
#define TAG_IMAGEHEIGHT             0x0101
/* compressed images only */
#define TAG_COMP_IMAGEWIDTH         0xA002
#define TAG_COMP_IMAGEHEIGHT        0xA003

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
/* }}} */

/* {{{ php_ifd_get16u
 * Convert a 16 bit unsigned value from file's native byte order */
static int php_ifd_get16u(void *Short, int motorola_intel)
{
	if (motorola_intel) {
		return (((unsigned char *)Short)[0] << 8) | ((unsigned char *)Short)[1];
	} else {
		return (((unsigned char *)Short)[1] << 8) | ((unsigned char *)Short)[0];
	}
}
/* }}} */

/* {{{ php_ifd_get16s
 * Convert a 16 bit signed value from file's native byte order */
static signed short php_ifd_get16s(void *Short, int motorola_intel)
{
	return (signed short)php_ifd_get16u(Short, motorola_intel);
}
/* }}} */

/* {{{ php_ifd_get32s
 * Convert a 32 bit signed value from file's native byte order */
static int php_ifd_get32s(void *Long, int motorola_intel)
{
	if (motorola_intel) {
		return  ((( char *)Long)[0] << 24) | (((unsigned char *)Long)[1] << 16)
		      | (((unsigned char *)Long)[2] << 8 ) | (((unsigned char *)Long)[3] << 0 );
	} else {
		return  ((( char *)Long)[3] << 24) | (((unsigned char *)Long)[2] << 16)
		      | (((unsigned char *)Long)[1] << 8 ) | (((unsigned char *)Long)[0] << 0 );
	}
}
/* }}} */

/* {{{ php_ifd_get32u
 * Convert a 32 bit unsigned value from file's native byte order */
static unsigned php_ifd_get32u(void *Long, int motorola_intel)
{
	return (unsigned)php_ifd_get32s(Long, motorola_intel) & 0xffffffff;
}
/* }}} */

/* {{{ php_handle_tiff
   main loop to parse TIFF structure */
static struct gfxinfo *php_handle_tiff (php_stream * stream, zval *info, int motorola_intel)
{
	struct gfxinfo *result = NULL;
	int i, num_entries;
	unsigned char *dir_entry;
	size_t ifd_size, dir_size, entry_value, width=0, height=0, ifd_addr;
	int entry_tag , entry_type;
	char *ifd_data, ifd_ptr[4];

	if (php_stream_read(stream, ifd_ptr, 4) != 4)
		return NULL;
	ifd_addr = php_ifd_get32u(ifd_ptr, motorola_intel);
	if (php_stream_seek(stream, ifd_addr-8, SEEK_CUR))
		return NULL;
	ifd_size = 2;
	ifd_data = emalloc(ifd_size);
	if (php_stream_read(stream, ifd_data, 2) != 2) {
		efree(ifd_data);
		return NULL;
	}
	num_entries = php_ifd_get16u(ifd_data, motorola_intel);
	dir_size = 2/*num dir entries*/ +12/*length of entry*/*num_entries +4/* offset to next ifd (points to thumbnail or NULL)*/;
	ifd_size = dir_size;
	ifd_data = erealloc(ifd_data,ifd_size);
	if (php_stream_read(stream, ifd_data+2, dir_size-2) != dir_size-2) {
		efree(ifd_data);
		return NULL;
	}
	/* now we have the directory we can look how long it should be */
	ifd_size = dir_size;
	for(i=0;i<num_entries;i++) {
		dir_entry 	 = (unsigned char *) ifd_data+2+i*12;
		entry_tag    = php_ifd_get16u(dir_entry+0, motorola_intel);
		entry_type   = php_ifd_get16u(dir_entry+2, motorola_intel);
		switch(entry_type) {
			case TAG_FMT_BYTE:
			case TAG_FMT_SBYTE:
				entry_value  = (size_t)(dir_entry[8]);
				break;
			case TAG_FMT_USHORT:
				entry_value  = php_ifd_get16u(dir_entry+8, motorola_intel);
				break;
			case TAG_FMT_SSHORT:
				entry_value  = php_ifd_get16s(dir_entry+8, motorola_intel);
				break;
			case TAG_FMT_ULONG:
				entry_value  = php_ifd_get32u(dir_entry+8, motorola_intel);
				break;
			case TAG_FMT_SLONG:
				entry_value  = php_ifd_get32s(dir_entry+8, motorola_intel);
				break;
			default:
				continue;
		}
		switch(entry_tag) {
			case TAG_IMAGEWIDTH:
			case TAG_COMP_IMAGEWIDTH:
				width  = entry_value;
				break;
			case TAG_IMAGEHEIGHT:
			case TAG_COMP_IMAGEHEIGHT:
				height = entry_value;
				break;
		}
	}
	efree(ifd_data);
	if ( width && height) {
		/* not the same when in for-loop */
		result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
		result->height   = height;
		result->width    = width;
		result->bits     = 0;
		result->channels = 0;
		return result;
	}
	return NULL;
}
/* }}} */

/* {{{ php_handle_psd
 */
static struct gfxinfo *php_handle_iff(php_stream * stream)
{
	struct gfxinfo * result;
	unsigned char a[10];
	int chunkId;
	int size;
	short width, height, bits;

	if (php_stream_read(stream, (char *) a, 8) != 8) {
		return NULL;
	}
	if (strncmp((char *) a+4, "ILBM", 4) && strncmp((char *) a+4, "PBM ", 4)) {
		return NULL;
	}

	/* loop chunks to find BMHD chunk */
	do {
		if (php_stream_read(stream, (char*)a, 8) != 8) {
			return NULL;
		}
		chunkId = php_ifd_get32s(a+0, 1);
		size    = php_ifd_get32s(a+4, 1);
		if (size < 0) {
			return NULL;
		}
		if ((size & 1) == 1) {
			size++;
		}
		if (chunkId == 0x424d4844) { /* BMHD chunk */
			if (size < 9 || php_stream_read(stream, (char*)a, 9) != 9) {
				return NULL;
			}
			width  = php_ifd_get16s(a+0, 1);
			height = php_ifd_get16s(a+2, 1);
			bits   = a[8] & 0xff;
			if (width > 0 && height > 0 && bits > 0 && bits < 33) {
				result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
				result->width    = width;
				result->height   = height;
				result->bits     = bits;
				result->channels = 0;
				return result;
			}
		} else {
			if (php_stream_seek(stream, size, SEEK_CUR)) {
				return NULL;
			}
		}
	} while (1);
}
/* }}} */

/* {{{ php_get_wbmp
 * int WBMP file format type
 * byte Header Type
 *	byte Extended Header
 *		byte Header Data (type 00 = multibyte)
 *		byte Header Data (type 11 = name/pairs)
 * int Number of columns
 * int Number of rows
 */
static int php_get_wbmp(php_stream *stream, struct gfxinfo **result, int check)
{
	int i, width = 0, height = 0;

	if (php_stream_rewind(stream)) {
		return 0;
	}

	/* get type */
	if (php_stream_getc(stream) != 0) {
		return 0;
	}

	/* skip header */
	do {
		i = php_stream_getc(stream);
		if (i < 0) {
			return 0;
		}
	} while (i & 0x80);

	/* get width */
	do {
		i = php_stream_getc(stream);
		if (i < 0) {
			return 0;
		}
		width = (width << 7) | (i & 0x7f);
        /* maximum valid width for wbmp (although 127 may be a more accurate one) */
        if (width > 2048) {
            return 0;
        }
	} while (i & 0x80);

	/* get height */
	do {
		i = php_stream_getc(stream);
		if (i < 0) {
			return 0;
		}
		height = (height << 7) | (i & 0x7f);
        /* maximum valid height for wbmp (although 127 may be a more accurate one) */
        if (height > 2048) {
            return 0;
        }
	} while (i & 0x80);

	if (!height || !width) {
		return 0;
	}

	if (!check) {
		(*result)->width = width;
		(*result)->height = height;
	}

	return IMAGE_FILETYPE_WBMP;
}
/* }}} */

/* {{{ php_handle_wbmp
*/
static struct gfxinfo *php_handle_wbmp(php_stream * stream)
{
	struct gfxinfo *result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));

	if (!php_get_wbmp(stream, &result, 0)) {
		efree(result);
		return NULL;
	}

	return result;
}
/* }}} */

/* {{{ php_get_xbm
 */
static int php_get_xbm(php_stream *stream, struct gfxinfo **result)
{
    char *fline;
    char *iname;
    char *type;
    int value;
    unsigned int width = 0, height = 0;

	if (result) {
		*result = NULL;
	}
	if (php_stream_rewind(stream)) {
		return 0;
	}
	while ((fline=php_stream_gets(stream, NULL, 0)) != NULL) {
		iname = estrdup(fline); /* simple way to get necessary buffer of required size */
		if (sscanf(fline, "#define %s %d", iname, &value) == 2) {
			if (!(type = strrchr(iname, '_'))) {
				type = iname;
			} else {
				type++;
			}

			if (!strcmp("width", type)) {
				width = (unsigned int) value;
				if (height) {
					efree(iname);
					break;
				}
			}
			if (!strcmp("height", type)) {
				height = (unsigned int) value;
				if (width) {
					efree(iname);
					break;
				}
			}
		}
		efree(fline);
		efree(iname);
	}
	if (fline) {
		efree(fline);
	}

	if (width && height) {
		if (result) {
			*result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
			(*result)->width = width;
			(*result)->height = height;
		}
		return IMAGE_FILETYPE_XBM;
	}

	return 0;
}
/* }}} */

/* {{{ php_handle_xbm
 */
static struct gfxinfo *php_handle_xbm(php_stream * stream)
{
	struct gfxinfo *result;
	php_get_xbm(stream, &result);
	return result;
}
/* }}} */

/* {{{ php_handle_ico
 */
static struct gfxinfo *php_handle_ico(php_stream * stream)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[16];
	int num_icons = 0;

	if (php_stream_read(stream, (char *) dim, 2) != 2)
		return NULL;

	num_icons = (((unsigned int)dim[1]) << 8) + ((unsigned int) dim[0]);

	if (num_icons < 1 || num_icons > 255)
		return NULL;

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));

	while (num_icons > 0)
	{
		if (php_stream_read(stream, (char *) dim, sizeof(dim)) != sizeof(dim))
			break;

		if ((((unsigned int)dim[7]) <<  8) +  ((unsigned int)dim[6]) >= result->bits)
		{
			result->width    =  (unsigned int)dim[0];
			result->height   =  (unsigned int)dim[1];
			result->bits     =  (((unsigned int)dim[7]) <<  8) +  ((unsigned int)dim[6]);
		}
		num_icons--;
	}

	return result;
}
/* }}} */

/* {{{ php_handle_webp
 */
static struct gfxinfo *php_handle_webp(php_stream * stream)
{
	struct gfxinfo *result = NULL;
	const char sig[3] = {'V', 'P', '8'};
	unsigned char buf[18];
	char format;

	if (php_stream_read(stream, (char *) buf, 18) != 18)
		return NULL;

	if (memcmp(buf, sig, 3)) {
		return NULL;
	}
	switch (buf[3]) {
		case ' ':
		case 'L':
		case 'X':
			format = buf[3];
			break;
		default:
			return NULL;
	}

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));

	switch (format) {
		case ' ':
			result->width = buf[14] + ((buf[15] & 0x3F) << 8);
			result->height = buf[16] + ((buf[17] & 0x3F) << 8);
			break;
		case 'L':
			result->width = buf[9] + ((buf[10] & 0x3F) << 8) + 1;
			result->height = (buf[10] >> 6) + (buf[11] << 2) + ((buf[12] & 0xF) << 10) + 1;
			break;
		case 'X':
			result->width = buf[12] + (buf[13] << 8) + (buf[14] << 16) + 1;
			result->height = buf[15] + (buf[16] << 8) + (buf[17] << 16) + 1;
			break;
	}
	result->bits = 8; /* always 1 byte */

	return result;
}
/* }}} */

/* {{{ php_image_type_to_mime_type
 * Convert internal image_type to mime type */
PHPAPI char * php_image_type_to_mime_type(int image_type)
{
	switch( image_type) {
		case IMAGE_FILETYPE_GIF:
			return "image/gif";
		case IMAGE_FILETYPE_JPEG:
			return "image/jpeg";
		case IMAGE_FILETYPE_PNG:
			return "image/png";
		case IMAGE_FILETYPE_SWF:
		case IMAGE_FILETYPE_SWC:
			return "application/x-shockwave-flash";
		case IMAGE_FILETYPE_PSD:
			return "image/psd";
		case IMAGE_FILETYPE_BMP:
			return "image/bmp";
		case IMAGE_FILETYPE_TIFF_II:
		case IMAGE_FILETYPE_TIFF_MM:
			return "image/tiff";
		case IMAGE_FILETYPE_IFF:
			return "image/iff";
		case IMAGE_FILETYPE_WBMP:
			return "image/vnd.wap.wbmp";
		case IMAGE_FILETYPE_JPC:
			return "application/octet-stream";
		case IMAGE_FILETYPE_JP2:
			return "image/jp2";
		case IMAGE_FILETYPE_XBM:
			return "image/xbm";
		case IMAGE_FILETYPE_ICO:
			return "image/vnd.microsoft.icon";
		case IMAGE_FILETYPE_WEBP:
			return "image/webp";
		default:
		case IMAGE_FILETYPE_UNKNOWN:
			return "application/octet-stream"; /* suppose binary format */
	}
}
/* }}} */

/* {{{ proto string image_type_to_mime_type(int imagetype)
   Get Mime-Type for image-type returned by getimagesize, exif_read_data, exif_thumbnail, exif_imagetype */
PHP_FUNCTION(image_type_to_mime_type)
{
	zend_long p_image_type;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(p_image_type)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_STRING(return_value, (char*)php_image_type_to_mime_type(p_image_type));
}
/* }}} */

/* {{{ proto string image_type_to_extension(int imagetype [, bool include_dot])
   Get file extension for image-type returned by getimagesize, exif_read_data, exif_thumbnail, exif_imagetype */
PHP_FUNCTION(image_type_to_extension)
{
	zend_long image_type;
	zend_bool inc_dot=1;
	const char *imgext = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(image_type)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(inc_dot)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	switch (image_type) {
		case IMAGE_FILETYPE_GIF:
			imgext = ".gif";
			break;
		case IMAGE_FILETYPE_JPEG:
			imgext = ".jpeg";
			break;
		case IMAGE_FILETYPE_PNG:
			imgext = ".png";
			break;
		case IMAGE_FILETYPE_SWF:
		case IMAGE_FILETYPE_SWC:
			imgext = ".swf";
			break;
		case IMAGE_FILETYPE_PSD:
			imgext = ".psd";
			break;
		case IMAGE_FILETYPE_BMP:
		case IMAGE_FILETYPE_WBMP:
			imgext = ".bmp";
			break;
		case IMAGE_FILETYPE_TIFF_II:
		case IMAGE_FILETYPE_TIFF_MM:
			imgext = ".tiff";
			break;
		case IMAGE_FILETYPE_IFF:
			imgext = ".iff";
			break;
		case IMAGE_FILETYPE_JPC:
			imgext = ".jpc";
			break;
		case IMAGE_FILETYPE_JP2:
			imgext = ".jp2";
			break;
		case IMAGE_FILETYPE_JPX:
			imgext = ".jpx";
			break;
		case IMAGE_FILETYPE_JB2:
			imgext = ".jb2";
			break;
		case IMAGE_FILETYPE_XBM:
			imgext = ".xbm";
			break;
		case IMAGE_FILETYPE_ICO:
			imgext = ".ico";
			break;
		case IMAGE_FILETYPE_WEBP:
			imgext = ".webp";
			break;
	}

	if (imgext) {
		RETURN_STRING(&imgext[!inc_dot]);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ php_imagetype
   detect filetype from first bytes */
PHPAPI int php_getimagetype(php_stream * stream, char *filetype)
{
	char tmp[12];
    int twelve_bytes_read;

	if ( !filetype) filetype = tmp;
	if((php_stream_read(stream, filetype, 3)) != 3) {
		php_error_docref(NULL, E_NOTICE, "Read error!");
		return IMAGE_FILETYPE_UNKNOWN;
	}

/* BYTES READ: 3 */
	if (!memcmp(filetype, php_sig_gif, 3)) {
		return IMAGE_FILETYPE_GIF;
	} else if (!memcmp(filetype, php_sig_jpg, 3)) {
		return IMAGE_FILETYPE_JPEG;
	} else if (!memcmp(filetype, php_sig_png, 3)) {
		if (php_stream_read(stream, filetype+3, 5) != 5) {
			php_error_docref(NULL, E_NOTICE, "Read error!");
			return IMAGE_FILETYPE_UNKNOWN;
		}
		if (!memcmp(filetype, php_sig_png, 8)) {
			return IMAGE_FILETYPE_PNG;
		} else {
			php_error_docref(NULL, E_WARNING, "PNG file corrupted by ASCII conversion");
			return IMAGE_FILETYPE_UNKNOWN;
		}
	} else if (!memcmp(filetype, php_sig_swf, 3)) {
		return IMAGE_FILETYPE_SWF;
	} else if (!memcmp(filetype, php_sig_swc, 3)) {
		return IMAGE_FILETYPE_SWC;
	} else if (!memcmp(filetype, php_sig_psd, 3)) {
		return IMAGE_FILETYPE_PSD;
	} else if (!memcmp(filetype, php_sig_bmp, 2)) {
		return IMAGE_FILETYPE_BMP;
	} else if (!memcmp(filetype, php_sig_jpc, 3)) {
		return IMAGE_FILETYPE_JPC;
	} else if (!memcmp(filetype, php_sig_riff, 3)) {
		if (php_stream_read(stream, filetype+3, 9) != 9) {
			php_error_docref(NULL, E_NOTICE, "Read error!");
			return IMAGE_FILETYPE_UNKNOWN;
		}
		if (!memcmp(filetype+8, php_sig_webp, 4)) {
			return IMAGE_FILETYPE_WEBP;
		} else {
			return IMAGE_FILETYPE_UNKNOWN;
		}
	}

	if (php_stream_read(stream, filetype+3, 1) != 1) {
		php_error_docref(NULL, E_NOTICE, "Read error!");
		return IMAGE_FILETYPE_UNKNOWN;
	}
/* BYTES READ: 4 */
	if (!memcmp(filetype, php_sig_tif_ii, 4)) {
		return IMAGE_FILETYPE_TIFF_II;
	} else if (!memcmp(filetype, php_sig_tif_mm, 4)) {
		return IMAGE_FILETYPE_TIFF_MM;
	} else if (!memcmp(filetype, php_sig_iff, 4)) {
		return IMAGE_FILETYPE_IFF;
	} else if (!memcmp(filetype, php_sig_ico, 4)) {
		return IMAGE_FILETYPE_ICO;
	}

    /* WBMP may be smaller than 12 bytes, so delay error */
	twelve_bytes_read = (php_stream_read(stream, filetype+4, 8) == 8);

/* BYTES READ: 12 */
   	if (twelve_bytes_read && !memcmp(filetype, php_sig_jp2, 12)) {
		return IMAGE_FILETYPE_JP2;
	}

/* AFTER ALL ABOVE FAILED */
	if (php_get_wbmp(stream, NULL, 1)) {
		return IMAGE_FILETYPE_WBMP;
	}
    if (!twelve_bytes_read) {
		php_error_docref(NULL, E_NOTICE, "Read error!");
		return IMAGE_FILETYPE_UNKNOWN;
    }
	if (php_get_xbm(stream, NULL)) {
		return IMAGE_FILETYPE_XBM;
	}
	return IMAGE_FILETYPE_UNKNOWN;
}
/* }}} */

static void php_getimagesize_from_stream(php_stream *stream, zval *info, INTERNAL_FUNCTION_PARAMETERS) /* {{{ */
{
	int itype = 0;
	struct gfxinfo *result = NULL;

	if (!stream) {
		RETURN_FALSE;
	}

	itype = php_getimagetype(stream, NULL);
	switch( itype) {
		case IMAGE_FILETYPE_GIF:
			result = php_handle_gif(stream);
			break;
		case IMAGE_FILETYPE_JPEG:
			if (info) {
				result = php_handle_jpeg(stream, info);
			} else {
				result = php_handle_jpeg(stream, NULL);
			}
			break;
		case IMAGE_FILETYPE_PNG:
			result = php_handle_png(stream);
			break;
		case IMAGE_FILETYPE_SWF:
			result = php_handle_swf(stream);
			break;
		case IMAGE_FILETYPE_SWC:
#if HAVE_ZLIB && !defined(COMPILE_DL_ZLIB)
			result = php_handle_swc(stream);
#else
			php_error_docref(NULL, E_NOTICE, "The image is a compressed SWF file, but you do not have a static version of the zlib extension enabled");
#endif
			break;
		case IMAGE_FILETYPE_PSD:
			result = php_handle_psd(stream);
			break;
		case IMAGE_FILETYPE_BMP:
			result = php_handle_bmp(stream);
			break;
		case IMAGE_FILETYPE_TIFF_II:
			result = php_handle_tiff(stream, NULL, 0);
			break;
		case IMAGE_FILETYPE_TIFF_MM:
			result = php_handle_tiff(stream, NULL, 1);
			break;
		case IMAGE_FILETYPE_JPC:
			result = php_handle_jpc(stream);
			break;
		case IMAGE_FILETYPE_JP2:
			result = php_handle_jp2(stream);
			break;
		case IMAGE_FILETYPE_IFF:
			result = php_handle_iff(stream);
			break;
		case IMAGE_FILETYPE_WBMP:
			result = php_handle_wbmp(stream);
			break;
		case IMAGE_FILETYPE_XBM:
			result = php_handle_xbm(stream);
			break;
		case IMAGE_FILETYPE_ICO:
			result = php_handle_ico(stream);
			break;
		case IMAGE_FILETYPE_WEBP:
			result = php_handle_webp(stream);
			break;
		default:
		case IMAGE_FILETYPE_UNKNOWN:
			break;
	}

	if (result) {
		char temp[MAX_LENGTH_OF_LONG * 2 + sizeof("width=\"\" height=\"\"")];
		array_init(return_value);
		add_index_long(return_value, 0, result->width);
		add_index_long(return_value, 1, result->height);
		add_index_long(return_value, 2, itype);
		snprintf(temp, sizeof(temp), "width=\"%d\" height=\"%d\"", result->width, result->height);
		add_index_string(return_value, 3, temp);

		if (result->bits != 0) {
			add_assoc_long(return_value, "bits", result->bits);
		}
		if (result->channels != 0) {
			add_assoc_long(return_value, "channels", result->channels);
		}
		add_assoc_string(return_value, "mime", (char*)php_image_type_to_mime_type(itype));
		efree(result);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

#define FROM_DATA 0
#define FROM_PATH 1

static void php_getimagesize_from_any(INTERNAL_FUNCTION_PARAMETERS, int mode) {  /* {{{ */
	zval *info = NULL;
	php_stream *stream = NULL;
	char *input;
	size_t input_len;
	const int argc = ZEND_NUM_ARGS();

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(input, input_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(info)
	ZEND_PARSE_PARAMETERS_END();

	if (argc == 2) {
		info = zend_try_array_init(info);
		if (!info) {
			return;
		}
	}

	if (mode == FROM_PATH) {
		stream = php_stream_open_wrapper(input, "rb", STREAM_MUST_SEEK|REPORT_ERRORS|IGNORE_PATH, NULL);
	} else {
		stream = php_stream_memory_open(TEMP_STREAM_READONLY, input, input_len);
	}

	if (!stream) {
		   RETURN_FALSE;
	}

	php_getimagesize_from_stream(stream, info, INTERNAL_FUNCTION_PARAM_PASSTHRU);
	php_stream_close(stream);
}
/* }}} */

/* {{{ proto array getimagesize(string imagefile [, array info])
   Get the size of an image as 4-element array */
PHP_FUNCTION(getimagesize)
{
	php_getimagesize_from_any(INTERNAL_FUNCTION_PARAM_PASSTHRU, FROM_PATH);
}
/* }}} */

/* {{{ proto array getimagesizefromstring(string data [, array info])
   Get the size of an image as 4-element array */
PHP_FUNCTION(getimagesizefromstring)
{
	php_getimagesize_from_any(INTERNAL_FUNCTION_PARAM_PASSTHRU, FROM_DATA);
}
/* }}} */
