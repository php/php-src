/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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

#include "php.h"
#include <stdio.h>
#if defined(NETWARE) && !defined(NEW_LIBC)
#include <sys/socket.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include "fopen_wrappers.h"
#include "ext/standard/fsock.h"
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "php_image.h"

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
PHPAPI const char php_sig_jpc[3] = {(char)0xFF, (char)0x4F, (char)0xff};
PHPAPI const char php_sig_iff[4] = {'F','O','R','M'};

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
	return SUCCESS;
}
/* }}} */

/* {{{ php_handle_gif
 * routine to handle GIF files. If only everything were that easy... ;} */
static struct gfxinfo *php_handle_gif (php_stream * stream TSRMLS_DC)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[5];

	if (php_stream_seek(stream, 3, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, dim, sizeof(dim)) != sizeof(dim))
		return NULL;

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
	result->width    = (unsigned int)dim[0] | (((unsigned int)dim[1])<<8);
	result->height   = (unsigned int)dim[2] | (((unsigned int)dim[3])<<8);
	result->bits     = dim[4]&0x80 ? ((((unsigned int)dim[4])&0x07) + 1) : 0;
	result->channels = 3; /* allways */

	return result;
}
/* }}} */

/* {{{ php_handle_psd
 */
static struct gfxinfo *php_handle_psd (php_stream * stream TSRMLS_DC)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[8];

	if (php_stream_seek(stream, 11, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, dim, sizeof(dim)) != sizeof(dim))
		return NULL;

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
	result->height   =  (((unsigned int)dim[0]) << 24) + (((unsigned int)dim[1]) << 16) + (((unsigned int)dim[2]) << 8) + ((unsigned int)dim[3]);
	result->width    =  (((unsigned int)dim[4]) << 24) + (((unsigned int)dim[5]) << 16) + (((unsigned int)dim[6]) << 8) + ((unsigned int)dim[7]);

	return result;
}
/* }}} */

/* {{{ php_handle_bmp
 */
static struct gfxinfo *php_handle_bmp (php_stream * stream TSRMLS_DC)
{
	struct gfxinfo *result = NULL;
	unsigned char dim[12];

	if (php_stream_seek(stream, 15, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, dim, sizeof(dim)) != sizeof(dim))
		return NULL;

	result = (struct gfxinfo *) ecalloc (1, sizeof(struct gfxinfo));
	result->width    =  (((unsigned int)dim[ 3]) << 24) + (((unsigned int)dim[ 2]) << 16) + (((unsigned int)dim[ 1]) << 8) + ((unsigned int) dim[ 0]);
	result->height   =  (((unsigned int)dim[ 7]) << 24) + (((unsigned int)dim[ 6]) << 16) + (((unsigned int)dim[ 5]) << 8) + ((unsigned int) dim[ 4]);
	result->bits     =  (((unsigned int)dim[11]) <<  8) +  ((unsigned int)dim[10]);

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
static struct gfxinfo *php_handle_swc(php_stream * stream TSRMLS_DC)
{
	struct gfxinfo *result = NULL;

	long bits;
	unsigned char a[64];
	unsigned long len=64, szlength;
	int factor=1,maxfactor=16;
	int slength, status=0;
	char *b, *buf=NULL, *bufz=NULL;

	b = ecalloc (1, len + 1);

	if (php_stream_seek(stream, 5, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, a, sizeof(a)) != sizeof(a))
		return NULL;

	if (uncompress(b, &len, a, sizeof(a)) != Z_OK) {
		/* failed to decompress the file, will try reading the rest of the file */
		if (php_stream_seek(stream, 8, SEEK_SET))
			return NULL;

		slength = php_stream_copy_to_mem(stream, &bufz, PHP_STREAM_COPY_ALL, 0);
		
		/*
		 * zlib::uncompress() wants to know the output data length
		 * if none was given as a parameter
		 * we try from input length * 2 up to input length * 2^8
		 * doubling it whenever it wasn't big enough
		 * that should be eneugh for all real life cases
		*/
		
		do {
			szlength=slength*(1<<factor++);
			buf = (char *) erealloc(buf,szlength);
			if (!buf) {
				status = 1;
				break;
			} 
			status = uncompress(buf, &szlength, bufz, slength);
		} while ((status==Z_BUF_ERROR)&&(factor<maxfactor));
		
		if (bufz) {
			pefree(bufz, 0);
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
static struct gfxinfo *php_handle_swf (php_stream * stream TSRMLS_DC)
{
	struct gfxinfo *result = NULL;
	long bits;
	unsigned char a[32];

	if (php_stream_seek(stream, 5, SEEK_CUR))
		return NULL;

	if (php_stream_read(stream, a, sizeof(a)) != sizeof(a))
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
static struct gfxinfo *php_handle_png (php_stream * stream TSRMLS_DC)
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

	if((php_stream_read(stream, dim, sizeof(dim))) < sizeof(dim))
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
static unsigned short php_read2(php_stream * stream TSRMLS_DC)
{
	unsigned char a[2];

	/* just return 0 if we hit the end-of-file */
	if((php_stream_read(stream, a, sizeof(a))) <= 0) return 0;

	return (((unsigned short) a[ 0 ]) << 8) + ((unsigned short) a[ 1 ]);
}
/* }}} */

/* {{{ php_next_marker
 * get next marker byte from file */
static unsigned int php_next_marker(php_stream * stream, int last_marker, int comment_correction, int ff_read TSRMLS_DC)
{
	int a=0, marker;

	/* get marker byte, swallowing possible padding                           */
	if ( last_marker==M_COM && comment_correction) {
		/* some software does not count the length bytes of COM section           */
		/* one company doing so is very much envolved in JPEG... so we accept too */
		/* by the way: some of those companies changed their code now...          */
		comment_correction = 2;
	} else {
		last_marker = 0;
		comment_correction = 0;
	}
	if ( ff_read) {
		a = 1; /* already read 0xff in filetype detection */
	}
	do {
		if ((marker = php_stream_getc(stream)) == EOF)
		{
			return M_EOI;/* we hit EOF */
		}
		if ( last_marker==M_COM && comment_correction>0)
		{
			if ( marker != 0xFF)
			{
				marker = 0xff;
				comment_correction--;
			} else {
				last_marker = M_PSEUDO; /* stop skipping non 0xff for M_COM */
			}
		}
		if ( ++a > 10)
		{
			/* who knows the maxim amount of 0xff? though 7 */
			/* but found other implementations              */
			return M_EOI;
		}
	} while ( marker == 0xff);
	if ( a < 2)
	{
		return M_EOI; /* at least one 0xff is needed before marker code */
	}
	if ( last_marker==M_COM && comment_correction)
	{
		return M_EOI; /* ah illegal: char after COM section not 0xFF */
	}
	return (unsigned int)marker;
}
/* }}} */

/* {{{ php_skip_variable
 * skip over a variable-length block; assumes proper length marker */
static void php_skip_variable(php_stream * stream TSRMLS_DC)
{
	off_t length = ((unsigned int)php_read2(stream TSRMLS_CC));

	length = length-2;
	if (length)
	{
		php_stream_seek(stream, (long)length, SEEK_CUR);
	}
}
/* }}} */

/* {{{ php_read_APP
 */
static void php_read_APP(php_stream * stream, unsigned int marker, zval *info TSRMLS_DC)
{
	unsigned short length;
	unsigned char *buffer;
	unsigned char markername[ 16 ];
	zval *tmp;

	length = php_read2(stream TSRMLS_CC);
	length -= 2;				/* length includes itself */

	buffer = emalloc(length);
	if ( !buffer) return;

	if (php_stream_read(stream, buffer, (long) length) <= 0) {
		efree(buffer);
		return;
	}

	sprintf(markername, "APP%d", marker - M_APP0);

	if (zend_hash_find(Z_ARRVAL_P(info), markername, strlen(markername)+1, (void **) &tmp) == FAILURE) {
		/* XXX we onyl catch the 1st tag of it's kind! */
		add_assoc_stringl(info, markername, buffer, length, 1);
	}

	efree(buffer);
}
/* }}} */

/* {{{ php_handle_jpeg
   main loop to parse JPEG structure */
static struct gfxinfo *php_handle_jpeg (php_stream * stream, pval *info TSRMLS_DC) 
{
	struct gfxinfo *result = NULL;
	unsigned int marker = M_PSEUDO;
	unsigned short length, ff_read=1;

	for (;;) {
		marker = php_next_marker(stream, marker, 1, ff_read TSRMLS_CC);
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
					if ( !result)
						return NULL;
					length = php_read2(stream TSRMLS_CC);
					result->bits     = php_stream_getc(stream);
					result->height   = php_read2(stream TSRMLS_CC);
					result->width    = php_read2(stream TSRMLS_CC);
					result->channels = php_stream_getc(stream);
					if (!info || length<8) /* if we don't want an extanded info -> return */
						return result;
					if (php_stream_seek(stream, length-8, SEEK_CUR)) /* file error after info */
						return result;
				} else {
					php_skip_variable(stream TSRMLS_CC);
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
					php_read_APP(stream, marker, info TSRMLS_CC); /* read all the app markes... */
				} else {
					php_skip_variable(stream TSRMLS_CC);
				}
				break;

			case M_SOS:
			case M_EOI:
				return result;	/* we're about to hit image data, or are at EOF. stop processing. */

			default:
				php_skip_variable(stream TSRMLS_CC);		/* anything else isn't interesting */
				break;
		}
	}

	return result; /* perhaps image broken -> no info but size */
}
/* }}} */

/* {{{ jpeg2000 constants
  See ext/exif for more */
#define JC_SOC   0x4F		/* Start of codestream */
#define JC_SIZ   0x51		/* Image and tile size */
/* }}} */

/* {{{ php_read4
 */
static unsigned int php_read4(php_stream * stream TSRMLS_DC)
{
	unsigned char a[4];

	/* just return 0 if we hit the end-of-file */
	if ((php_stream_read(stream, a, sizeof(a))) != sizeof(a)) return 0;

	return (((unsigned int)a[0]) << 24)
	     + (((unsigned int)a[1]) << 16)
	     + (((unsigned int)a[2]) <<  8)
	     + (((unsigned int)a[3]));
}
/* }}} */

/* {{{ php_handle_tiff
   main loop to parse TIFF structure */
static struct gfxinfo *php_handle_jpc(php_stream * stream TSRMLS_DC)
{
	struct gfxinfo *result = NULL;
	unsigned int marker, dummy;
	unsigned short length, ff_read = 1;

	marker = php_next_marker(stream, 0, 0, ff_read TSRMLS_CC);
	ff_read = 0;
	if ( marker == JC_SIZ)
	{
		length           = php_read2(stream TSRMLS_CC); /* Lsiz: length of segment */
		if ( length<42 || length>49191) /* read the spec */
			return NULL;
		result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));
		if ( !result)
			return NULL;
		dummy            = php_read2(stream TSRMLS_CC); /* Rsiz: capabilities */
		result->height   = php_read4(stream TSRMLS_CC); /* Xsiz */
		result->width    = php_read4(stream TSRMLS_CC); /* Ysiz */
		dummy            = php_read4(stream TSRMLS_CC); /* X0siz */
		dummy            = php_read4(stream TSRMLS_CC); /* Y0siz */
		dummy            = php_read4(stream TSRMLS_CC); /* XTsiz */
		dummy            = php_read4(stream TSRMLS_CC); /* YTsiz */
		dummy            = php_read4(stream TSRMLS_CC); /* XT0siz */
		dummy            = php_read4(stream TSRMLS_CC); /* YT0siz */
		result->bits     = php_read2(stream TSRMLS_CC); /* Csiz: precision in bitss */
		result->channels = 0; /* don't know yet */
		return result;
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
static struct gfxinfo *php_handle_tiff (php_stream * stream, pval *info, int motorola_intel TSRMLS_DC)
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
		dir_entry 	 = ifd_data+2+i*12;
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
static struct gfxinfo *php_handle_iff(php_stream * stream TSRMLS_DC)
{
	struct gfxinfo *result = NULL;
	unsigned char a[10];
	int chunkId;
	int size;

	if (php_stream_read(stream, a, 8) != 8)
		return NULL;
	if (strncmp(a+4, "ILBM", 4) && strncmp(a+4, "PBM ", 4))
		return NULL;

	result = (struct gfxinfo *) ecalloc(1, sizeof(struct gfxinfo));

	/* loop chunks to find BMHD chunk */
	do {
		if (php_stream_read(stream, a, 8) != 8) {
			efree(result);
			return NULL;
		}
		chunkId = php_ifd_get32s(a+0, 1);
		size    = php_ifd_get32s(a+4, 1);
		if ((size & 1) == 1) {
			size++;
		}
		if (chunkId == 0x424d4844) { /* BMHD chunk */
			if (php_stream_read(stream, a, 9) != 9) {
				efree(result);
				return NULL;
			}
			result->width    = php_ifd_get16s(a+0, 1);
			result->height   = php_ifd_get16s(a+2, 1);
			result->bits     = a[8] & 0xff;
			result->channels = 0;
			if (result->width > 0 && result->height > 0 && result->bits > 0 && result->bits < 33)
				return result;
		} else {
			if (php_stream_seek(stream, size, SEEK_CUR)) {
				efree(result);
				return NULL;
			}
		}
	} while (1);
}
/* }}} */

/* {{{ php_image_type_to_mime_type
 * Convert internal image_type to mime type */
PHPAPI const char * php_image_type_to_mime_type(int image_type)
{
	switch( image_type) {
		case IMAGE_FILETYPE_GIF:
			return "image/gif";
		case IMAGE_FILETYPE_JPEG:
		case IMAGE_FILETYPE_JPC:
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
	zval **p_image_type;
	int arg_c = ZEND_NUM_ARGS();

	if ((arg_c!=1) || zend_get_parameters_ex(arg_c, &p_image_type) == FAILURE) {
		RETVAL_FALSE;
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(p_image_type);
	ZVAL_STRING(return_value, (char*)php_image_type_to_mime_type(Z_LVAL_PP(p_image_type)), 1);
}
/* }}} */

/* {{{ php_imagetype
   detect filetype from first bytes */
PHPAPI int php_getimagetype(php_stream * stream, char *filetype TSRMLS_DC)
{
	char tmp[8];

	if ( !filetype) filetype = tmp;
	if((php_stream_read(stream, filetype, 3)) <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Read error!");
		return IMAGE_FILETYPE_UNKNOWN;
	}

	if (!memcmp(filetype, php_sig_gif, 3)) {
		return IMAGE_FILETYPE_GIF;
	} else if (!memcmp(filetype, php_sig_jpg, 3)) {
		return IMAGE_FILETYPE_JPEG;
	} else if (!memcmp(filetype, php_sig_png, 3)) {
		php_stream_read(stream, filetype+3, 5);
		if (!memcmp(filetype, php_sig_png, 8)) {
			return IMAGE_FILETYPE_PNG;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "PNG file corrupted by ASCII conversion");
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
	}
	php_stream_read(stream, filetype+3, 1);
	if (!memcmp(filetype, php_sig_tif_ii, 4)) {
		return IMAGE_FILETYPE_TIFF_II;
	} else
	if (!memcmp(filetype, php_sig_tif_mm, 4)) {
		return IMAGE_FILETYPE_TIFF_MM;
	}
	if (!memcmp(filetype, php_sig_iff, 4)) {
		return IMAGE_FILETYPE_IFF;
	}
    
	return IMAGE_FILETYPE_UNKNOWN;
}
/* }}} */

/* {{{ proto array getimagesize(string imagefile [, array info])
   Get the size of an image as 4-element array */
PHP_FUNCTION(getimagesize)
{
	zval **arg1, **info = NULL;
	int itype = 0;
	char *temp;
	struct gfxinfo *result = NULL;
	php_stream * stream = NULL;

	switch(ZEND_NUM_ARGS()) {

	case 1:
		if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
			RETVAL_FALSE;
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg1);
		break;

	case 2:
		if (zend_get_parameters_ex(2, &arg1, &info) == FAILURE) {
			RETVAL_FALSE;
			WRONG_PARAM_COUNT;
		}
		zval_dtor(*info);

		array_init(*info);

		convert_to_string_ex(arg1);
		break;

	default:
		RETVAL_FALSE;
		WRONG_PARAM_COUNT;
	}

	stream = php_stream_open_wrapper(Z_STRVAL_PP(arg1), "rb", REPORT_ERRORS|IGNORE_PATH|ENFORCE_SAFE_MODE, NULL);

	if (!stream) {
		RETURN_FALSE;
	}

	itype = php_getimagetype(stream, NULL TSRMLS_CC);
	switch( itype) {
		case IMAGE_FILETYPE_GIF:
			result = php_handle_gif (stream TSRMLS_CC);
			break;
		case IMAGE_FILETYPE_JPEG:
			if (info) {
				result = php_handle_jpeg(stream, *info TSRMLS_CC);
			} else {
				result = php_handle_jpeg(stream, NULL TSRMLS_CC);
			}
			break;
		case IMAGE_FILETYPE_PNG:
			result = php_handle_png(stream TSRMLS_CC);
			break;
		case IMAGE_FILETYPE_SWF:
			result = php_handle_swf(stream TSRMLS_CC);
			break;
#if HAVE_ZLIB && !defined(COMPILE_DL_ZLIB)
		case IMAGE_FILETYPE_SWC:
			result = php_handle_swc(stream TSRMLS_CC);
			break;
#endif
		case IMAGE_FILETYPE_PSD:
			result = php_handle_psd(stream TSRMLS_CC);
			break;
		case IMAGE_FILETYPE_BMP:
			result = php_handle_bmp(stream TSRMLS_CC);
			break;
		case IMAGE_FILETYPE_TIFF_II:
			result = php_handle_tiff(stream, NULL, 0 TSRMLS_CC);
			break;
		case IMAGE_FILETYPE_TIFF_MM:
			result = php_handle_tiff(stream, NULL, 1 TSRMLS_CC);
			break;
		case IMAGE_FILETYPE_JPC:
			result = php_handle_jpc(stream TSRMLS_CC);
			break;
		case IMAGE_FILETYPE_IFF:
  			result = php_handle_iff(stream TSRMLS_CC);
		default:
		case IMAGE_FILETYPE_UNKNOWN:
			break;
	}

	php_stream_close(stream);

	if (result) {
		array_init(return_value);
		add_index_long(return_value, 0, result->width);
		add_index_long(return_value, 1, result->height);
		add_index_long(return_value, 2, itype);
		spprintf(&temp, 0, "width=\"%d\" height=\"%d\"", result->width, result->height);
		add_index_string(return_value, 3, temp, 0);

		if (result->bits != 0) {
			add_assoc_long(return_value, "bits", result->bits);
		}
		if (result->channels != 0) {
			add_assoc_long(return_value, "channels", result->channels);
		}
		add_assoc_string(return_value, "mime", (char*)php_image_type_to_mime_type(itype), 1);
		efree(result);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
