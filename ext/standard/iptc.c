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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

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
#include "php_iptc.h"
#include "ext/standard/head.h"

#include <sys/stat.h>


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

/* {{{ php_iptc_put1
 */
static int php_iptc_put1(FILE *fp, int spool, unsigned char c, unsigned char **spoolbuf TSRMLS_DC)
{ 
	if (spool > 0)
		PUTC(c);

	if (spoolbuf) *(*spoolbuf)++ = c;

  	return c;
}
/* }}} */

/* {{{ php_iptc_get1
 */
static int php_iptc_get1(FILE *fp, int spool, unsigned char **spoolbuf TSRMLS_DC)
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

/* {{{ php_iptc_read_remaining
 */
static int php_iptc_read_remaining(FILE *fp, int spool, unsigned char **spoolbuf TSRMLS_DC)
{
  	while (php_iptc_get1(fp, spool, spoolbuf TSRMLS_CC) != EOF) continue;

	return M_EOI;
}
/* }}} */

/* {{{ php_iptc_skip_variable
 */
static int php_iptc_skip_variable(FILE *fp, int spool, unsigned char **spoolbuf TSRMLS_DC)
{ 
	unsigned int  length;
	int c1, c2;

    if ((c1 = php_iptc_get1(fp, spool, spoolbuf TSRMLS_CC)) == EOF) return M_EOI;

    if ((c2 = php_iptc_get1(fp, spool, spoolbuf TSRMLS_CC)) == EOF) return M_EOI;

	length = (((unsigned char) c1) << 8) + ((unsigned char) c2);

	length -= 2;

	while (length--)
		if (php_iptc_get1(fp, spool, spoolbuf TSRMLS_CC) == EOF) return M_EOI;

	return 0;
}
/* }}} */

/* {{{ php_iptc_next_marker
 */
static int php_iptc_next_marker(FILE *fp, int spool, unsigned char **spoolbuf TSRMLS_DC)
{
    int c;

    /* skip unimportant stuff */

    c = php_iptc_get1(fp, spool, spoolbuf TSRMLS_CC);

	if (c == EOF) return M_EOI;

    while (c != 0xff) {
        if ((c = php_iptc_get1(fp, spool, spoolbuf TSRMLS_CC)) == EOF)
            return M_EOI; /* we hit EOF */
    }

    /* get marker byte, swallowing possible padding */
    do {
        c = php_iptc_get1(fp, 0, 0 TSRMLS_CC);
		if (c == EOF)
            return M_EOI;       /* we hit EOF */
		else
		if (c == 0xff)
			php_iptc_put1(fp, spool, (unsigned char)c, spoolbuf TSRMLS_CC);
    } while (c == 0xff);

    return (unsigned int) c;
}
/* }}} */

static char psheader[] = "\xFF\xED\0\0Photoshop 3.0\08BIM\x04\x04\0\0\0\0";

/* {{{ proto array iptcembed(string iptcdata, string jpeg_file_name [, int spool])
   Embed binary IPTC data into a JPEG image. */
PHP_FUNCTION(iptcembed)
{
    zval **iptcdata, **jpeg_file, **spool_flag; 
    FILE *fp;
	unsigned int marker;
	unsigned int spool = 0, done = 0, inx, len;	
	unsigned char *spoolbuf=0, *poi=0;
	struct stat sb;

    switch(ZEND_NUM_ARGS()){
    case 3:
        if (zend_get_parameters_ex(3, &iptcdata, &jpeg_file, &spool_flag) == FAILURE) {
            WRONG_PARAM_COUNT;
        }
        convert_to_string_ex(iptcdata);
        convert_to_string_ex(jpeg_file);
        convert_to_long_ex(spool_flag);
		spool = Z_LVAL_PP(spool_flag);
        break;

    case 2:
        if (zend_get_parameters_ex(2, &iptcdata, &jpeg_file) == FAILURE) {
            WRONG_PARAM_COUNT;
        }
        convert_to_string_ex(iptcdata);
        convert_to_string_ex(jpeg_file);
        break;

    default:
        WRONG_PARAM_COUNT;
        break;
    }

    if (php_check_open_basedir(Z_STRVAL_PP(jpeg_file) TSRMLS_CC)) {
		RETURN_FALSE;
	}

    if ((fp = VCWD_FOPEN(Z_STRVAL_PP(jpeg_file), "rb")) == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open %s", Z_STRVAL_PP(jpeg_file));
        RETURN_FALSE;
    }

	len = Z_STRLEN_PP(iptcdata);

	if (spool < 2) {
		fstat(fileno(fp), &sb);

		poi = spoolbuf = emalloc(len + sizeof(psheader) + sb.st_size + 1024);

		if (! spoolbuf) {
			fclose(fp);
			RETURN_FALSE;
		}
	} 

	if (php_iptc_get1(fp, spool, poi?&poi:0 TSRMLS_CC) != 0xFF) {
		fclose(fp);
		RETURN_FALSE;
	}

	if (php_iptc_get1(fp, spool, poi?&poi:0 TSRMLS_CC) != 0xD8) {
		fclose(fp);
		RETURN_FALSE;
	}

	while (!done) {
		marker = php_iptc_next_marker(fp, spool, poi?&poi:0 TSRMLS_CC);

		if (marker == M_EOI) { /* EOF */
			break;
		} else if (marker != M_APP13) { 
			php_iptc_put1(fp, spool, (unsigned char)marker, poi?&poi:0 TSRMLS_CC);
		}

		switch (marker) {
			case M_APP13:
				/* we are going to write a new APP13 marker, so don't output the old one */
				php_iptc_skip_variable(fp, 0, 0 TSRMLS_CC);    
				php_iptc_read_remaining(fp, spool, poi?&poi:0 TSRMLS_CC);
				done = 1;
				break;

			case M_APP0:
				/* APP0 is in each and every JPEG, so when we hit APP0 we insert our new APP13! */
				php_iptc_skip_variable(fp, spool, poi?&poi:0 TSRMLS_CC);

				if (len & 1) len++; /* make the length even */

				psheader[ 2 ] = (len+28)>>8;
				psheader[ 3 ] = (len+28)&0xff;

				for (inx = 0; inx < 28; inx++)
					php_iptc_put1(fp, spool, psheader[inx], poi?&poi:0 TSRMLS_CC);

				php_iptc_put1(fp, spool, (unsigned char)(len>>8), poi?&poi:0 TSRMLS_CC);
				php_iptc_put1(fp, spool, (unsigned char)(len&0xff), poi?&poi:0 TSRMLS_CC);
					
				for (inx = 0; inx < len; inx++)
					php_iptc_put1(fp, spool, Z_STRVAL_PP(iptcdata)[inx], poi?&poi:0 TSRMLS_CC);
				break;

			case M_SOS:								
				/* we hit data, no more marker-inserting can be done! */
				php_iptc_read_remaining(fp, spool, poi?&poi:0 TSRMLS_CC);
				done = 1;
				break;
			
			default:
				php_iptc_skip_variable(fp, spool, poi?&poi:0 TSRMLS_CC);
				break;
		}
	}

	fclose(fp);

	if (spool < 2) {
		RETVAL_STRINGL(spoolbuf, poi - spoolbuf, 0);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto array iptcparse(string iptcdata)
   Parse binary IPTC-data into associative array */
PHP_FUNCTION(iptcparse)
{
	unsigned int length, inx, len, tagsfound;
	unsigned char *buffer;
	unsigned char recnum, dataset;
	unsigned char key[ 16 ];
	zval *values, **str, **element;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	inx = 0;
	length = Z_STRLEN_PP(str);
	buffer = Z_STRVAL_PP(str);

	tagsfound = 0; /* number of tags already found */

	while (inx < length) { /* find 1st tag */
		if ((buffer[inx] == 0x1c) && (buffer[inx+1] == 0x02)){ 
			break;
		} else {
			inx++;
		}
	}

	while (inx < length) {
		if (buffer[ inx++ ] != 0x1c) {
			break;   /* we ran against some data which does not conform to IPTC - stop parsing! */
		} 
		
		if ((inx + 4) >= length)
			break;

		dataset = buffer[ inx++ ];
		recnum = buffer[ inx++ ];

		if (buffer[ inx ] & (unsigned char) 0x80) { /* long tag */
			len = (((long) buffer[ inx + 2 ]) << 24) + (((long) buffer[ inx + 3 ]) << 16) + 
				  (((long) buffer[ inx + 4 ]) <<  8) + (((long) buffer[ inx + 5 ]));
			inx += 6;
		} else { /* short tag */
			len = (((unsigned short) buffer[ inx ])<<8) | (unsigned short)buffer[ inx+1 ];
			inx += 2;
		}

		sprintf(key, "%d#%03d", (unsigned int) dataset, (unsigned int) recnum);

		if ((len > length) || (inx + len) > length)
			break;

		if (tagsfound == 0) { /* found the 1st tag - initialize the return array */
			if (array_init(return_value) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to initialize array");
				RETURN_FALSE;
	  		}
		}

		if (zend_hash_find(Z_ARRVAL_P(return_value), key, strlen(key) + 1, (void **) &element) == FAILURE) {
			ALLOC_ZVAL(values);
			INIT_PZVAL(values);
			if (array_init(values) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to initialize array");
				RETURN_FALSE;
			}
			
			zend_hash_update(Z_ARRVAL_P(return_value), key, strlen(key)+1, (void *) &values, sizeof(pval*), (void **) &element);
		} 
			
		add_next_index_stringl(*element, buffer+inx, len, 1);

		inx += len;

		tagsfound++;
	}

	if (! tagsfound) {
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
