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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
 */

/*
 * Functions to parse & compse IPTC data.
 * PhotoShop >= 3.0 can read and write textual data to JPEG files.
 * ... more to come .....
 *
 * i know, parts of this is now duplicated in image.c
 * but in this case i think it's okay!
 */

/*
 * TODO:
 *  - add IPTC translation table
 */

#include "php.h"
#include "ext/standard/head.h"

#include <sys/stat.h>

#include <stdint.h>
#ifndef PHP_WIN32
# include <inttypes.h>
#endif

/* some defines for the different JPEG block types */
#define M_SOF0  0xC0            /* Start Of Frame N */
#define M_SOF1  0xC1            /* N indicates which compression process */
#define M_SOF2  0xC2            /* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5            /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8
#define M_EOI   0xD9            /* End Of Image (end of datastream) */
#define M_SOS   0xDA            /* Start Of Scan (begins compressed data) */
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

/* {{{ php_iptc_put1 */
static int php_iptc_put1(FILE *fp, int spool, unsigned char c, unsigned char **spoolbuf)
{
	if (spool > 0)
		PUTC(c);

	if (spoolbuf) *(*spoolbuf)++ = c;

	return c;
}
/* }}} */

/* {{{ php_iptc_get1 */
static int php_iptc_get1(FILE *fp, int spool, unsigned char **spoolbuf)
{
	int c;
	char cc;

	c = getc(fp);

	if (c == EOF) return EOF;

	if (spool > 0) {
		cc = c;
		PUTC(cc);
	}

	if (spoolbuf) *(*spoolbuf)++ = c;

	return c;
}
/* }}} */

/* {{{ php_iptc_read_remaining */
static int php_iptc_read_remaining(FILE *fp, int spool, unsigned char **spoolbuf)
{
	while (php_iptc_get1(fp, spool, spoolbuf) != EOF) continue;

	return M_EOI;
}
/* }}} */

/* {{{ php_iptc_skip_variable */
static int php_iptc_skip_variable(FILE *fp, int spool, unsigned char **spoolbuf)
{
	unsigned int  length;
	int c1, c2;

	if ((c1 = php_iptc_get1(fp, spool, spoolbuf)) == EOF) return M_EOI;

	if ((c2 = php_iptc_get1(fp, spool, spoolbuf)) == EOF) return M_EOI;

	length = (((unsigned char) c1) << 8) + ((unsigned char) c2);

	length -= 2;

	while (length--)
		if (php_iptc_get1(fp, spool, spoolbuf) == EOF) return M_EOI;

	return 0;
}
/* }}} */

/* {{{ php_iptc_next_marker */
static int php_iptc_next_marker(FILE *fp, int spool, unsigned char **spoolbuf)
{
	int c;

	/* skip unimportant stuff */

	c = php_iptc_get1(fp, spool, spoolbuf);

	if (c == EOF) return M_EOI;

	while (c != 0xff) {
		if ((c = php_iptc_get1(fp, spool, spoolbuf)) == EOF)
			return M_EOI; /* we hit EOF */
	}

	/* get marker byte, swallowing possible padding */
	do {
		c = php_iptc_get1(fp, 0, 0);
		if (c == EOF)
			return M_EOI;       /* we hit EOF */
		else
		if (c == 0xff)
			php_iptc_put1(fp, spool, (unsigned char)c, spoolbuf);
	} while (c == 0xff);

	return (unsigned int) c;
}
/* }}} */

static char psheader[] = "\xFF\xED\0\0Photoshop 3.0\08BIM\x04\x04\0\0\0\0";

/* {{{ Embed binary IPTC data into a JPEG image. */
PHP_FUNCTION(iptcembed)
{
	char *iptcdata, *jpeg_file;
	size_t iptcdata_len, jpeg_file_len;
	zend_long spool = 0;
	FILE *fp;
	unsigned int marker, done = 0;
	size_t inx;
	zend_string *spoolbuf = NULL;
	unsigned char *poi = NULL;
	zend_stat_t sb = {0};
	bool written = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(iptcdata, iptcdata_len)
		Z_PARAM_PATH(jpeg_file, jpeg_file_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(spool)
	ZEND_PARSE_PARAMETERS_END();

	if (php_check_open_basedir(jpeg_file)) {
		RETURN_FALSE;
	}

	if (iptcdata_len >= SIZE_MAX - sizeof(psheader) - 1025) {
		zend_argument_value_error(1, "is too large");
		RETURN_THROWS();
	}

	if ((fp = VCWD_FOPEN(jpeg_file, "rb")) == 0) {
		php_error_docref(NULL, E_WARNING, "Unable to open %s", jpeg_file);
		RETURN_FALSE;
	}

	if (spool < 2) {
		if (zend_fstat(fileno(fp), &sb) != 0) {
			RETURN_FALSE;
		}

		spoolbuf = zend_string_safe_alloc(1, iptcdata_len + sizeof(psheader) + 1024 + 1, sb.st_size, 0);
		poi = (unsigned char*)ZSTR_VAL(spoolbuf);
		memset(poi, 0, iptcdata_len + sizeof(psheader) + sb.st_size + 1024 + 1);
	}

	if (php_iptc_get1(fp, spool, poi?&poi:0) != 0xFF) {
		fclose(fp);
		if (spoolbuf) {
			zend_string_efree(spoolbuf);
		}
		RETURN_FALSE;
	}

	if (php_iptc_get1(fp, spool, poi?&poi:0) != 0xD8) {
		fclose(fp);
		if (spoolbuf) {
			zend_string_efree(spoolbuf);
		}
		RETURN_FALSE;
	}

	while (!done) {
		marker = php_iptc_next_marker(fp, spool, poi?&poi:0);

		if (marker == M_EOI) { /* EOF */
			break;
		} else if (marker != M_APP13) {
			php_iptc_put1(fp, spool, (unsigned char)marker, poi?&poi:0);
		}

		switch (marker) {
			case M_APP13:
				/* we are going to write a new APP13 marker, so don't output the old one */
				php_iptc_skip_variable(fp, 0, 0);
				fgetc(fp); /* skip already copied 0xFF byte */
				php_iptc_read_remaining(fp, spool, poi?&poi:0);
				done = 1;
				break;

			case M_APP0:
				/* APP0 is in each and every JPEG, so when we hit APP0 we insert our new APP13! */
			case M_APP1:
				if (written) {
					/* don't try to write the data twice */
					break;
				}
				written = 1;

				php_iptc_skip_variable(fp, spool, poi?&poi:0);

				if (iptcdata_len & 1) {
					iptcdata_len++; /* make the length even */
				}

				psheader[ 2 ] = (char) ((iptcdata_len+28)>>8);
				psheader[ 3 ] = (iptcdata_len+28)&0xff;

				for (inx = 0; inx < 28; inx++) {
					php_iptc_put1(fp, spool, psheader[inx], poi?&poi:0);
				}

				php_iptc_put1(fp, spool, (unsigned char)(iptcdata_len>>8), poi?&poi:0);
				php_iptc_put1(fp, spool, (unsigned char)(iptcdata_len&0xff), poi?&poi:0);

				for (inx = 0; inx < iptcdata_len; inx++) {
					php_iptc_put1(fp, spool, iptcdata[inx], poi?&poi:0);
				}
				break;

			case M_SOS:
				/* we hit data, no more marker-inserting can be done! */
				php_iptc_read_remaining(fp, spool, poi?&poi:0);
				done = 1;
				break;

			default:
				php_iptc_skip_variable(fp, spool, poi?&poi:0);
				break;
		}
	}

	fclose(fp);

	if (spool < 2) {
		spoolbuf = zend_string_truncate(spoolbuf, poi - (unsigned char*)ZSTR_VAL(spoolbuf), 0);
		RETURN_NEW_STR(spoolbuf);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Parse binary IPTC-data into associative array */
PHP_FUNCTION(iptcparse)
{
	size_t inx = 0, len;
	unsigned int tagsfound = 0;
	unsigned char *buffer, recnum, dataset;
	char *str, key[16];
	size_t str_len;
	zval values, *element;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(str, str_len)
	ZEND_PARSE_PARAMETERS_END();

	buffer = (unsigned char *)str;

	while (inx < str_len) { /* find 1st tag */
		if ((buffer[inx] == 0x1c) && ((buffer[inx+1] == 0x01) || (buffer[inx+1] == 0x02))){
			break;
		} else {
			inx++;
		}
	}

	while (inx < str_len) {
		if (buffer[ inx++ ] != 0x1c) {
			break;   /* we ran against some data which does not conform to IPTC - stop parsing! */
		}

		if ((inx + 4) >= str_len)
			break;

		dataset = buffer[ inx++ ];
		recnum = buffer[ inx++ ];

		if (buffer[ inx ] & (unsigned char) 0x80) { /* long tag */
			if((inx+6) >= str_len) {
				break;
			}
			len = (((zend_long) buffer[ inx + 2 ]) << 24) + (((zend_long) buffer[ inx + 3 ]) << 16) +
				  (((zend_long) buffer[ inx + 4 ]) <<  8) + (((zend_long) buffer[ inx + 5 ]));
			inx += 6;
		} else { /* short tag */
			len = (((unsigned short) buffer[ inx ])<<8) | (unsigned short)buffer[ inx+1 ];
			inx += 2;
		}

		if ((len > str_len) || (inx + len) > str_len) {
			break;
		}

		snprintf(key, sizeof(key), "%d#%03d", (unsigned int) dataset, (unsigned int) recnum);

		if (tagsfound == 0) { /* found the 1st tag - initialize the return array */
			array_init(return_value);
		}

		if ((element = zend_hash_str_find(Z_ARRVAL_P(return_value), key, strlen(key))) == NULL) {
			array_init(&values);

			element = zend_hash_str_update(Z_ARRVAL_P(return_value), key, strlen(key), &values);
		}

		add_next_index_stringl(element, (char *) buffer+inx, len);
		inx += len;
		tagsfound++;
	}

	if (! tagsfound) {
		RETURN_FALSE;
	}
}
/* }}} */
