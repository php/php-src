/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Thies C. Arntzen (thies@digicol.de)                         |
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
#include "php3_iptc.h"
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

static int php3_iptc_put1(FILE *fp,int spool,unsigned char c,unsigned char **spoolbuf)
{ 
	if (spool > 0)
		PUTC(c);

	if (spoolbuf) *(*spoolbuf)++ = c;

  	return c;
}

static int php3_iptc_get1(FILE *fp,int spool,unsigned char **spoolbuf)
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

static int php3_iptc_read_remaining(FILE *fp,int spool,unsigned char **spoolbuf)
{
 	int c;

  	while ((c = php3_iptc_get1(fp,spool,spoolbuf)) != EOF) continue;

	return M_EOI;
}

static int php3_iptc_skip_variable(FILE *fp,int spool,unsigned char **spoolbuf)
{ 
	unsigned int  length;
	int c1,c2;

    if ((c1 = php3_iptc_get1(fp,spool,spoolbuf)) == EOF) return M_EOI;

    if ((c2 = php3_iptc_get1(fp,spool,spoolbuf)) == EOF) return M_EOI;

	length = (((unsigned char) c1) << 8) + ((unsigned char) c2);

	length -= 2;

	while (length--)
		if (php3_iptc_get1(fp,spool,spoolbuf) == EOF) return M_EOI;

	return 0;
}

static int php3_iptc_nextmarker(FILE *fp,int spool,unsigned char **spoolbuf)
{
    int c;

    /* skip unimportant stuff */

    c = php3_iptc_get1(fp,spool,spoolbuf);

	if (c == EOF) return M_EOI;

    while (c != 0xff) {
        if ((c = php3_iptc_get1(fp,spool,spoolbuf)) == EOF)
            return M_EOI; /* we hit EOF */
    }

    /* get marker byte, swallowing possible padding */
    do {
        c = php3_iptc_get1(fp,0,0);
		if (c == EOF)
            return M_EOI;       /* we hit EOF */
		else
		if (c == 0xff)
			php3_iptc_put1(fp,spool,(unsigned char)c,spoolbuf);
    } while (c == 0xff);

    return (unsigned int) c;
}

static char psheader[] = "\xFF\xED\0\0Photoshop 3.0\08BIM\x04\x04\0\0\0\0";

/* {{{ proto array iptcembed(string iptcdata, string jpeg_file_name [ , int spool ])
   Embed binary IPTC data into a JPEG image. */
PHP_FUNCTION(iptcembed)
{
    pval *iptcdata, *jpeg_file, *spool_flag;
    FILE *fp;
	unsigned int marker;
	unsigned int spool = 0, done = 0, inx, len;	
	unsigned char *spoolbuf=0,*poi=0;
	struct stat sb;

    switch(ARG_COUNT(ht)){
    case 3:
        if (getParameters(ht, 3, &iptcdata, &jpeg_file, &spool_flag) == FAILURE) {
            WRONG_PARAM_COUNT;
        }
        convert_to_string(iptcdata);
        convert_to_string(jpeg_file);
        convert_to_long(spool_flag);
		spool = spool_flag->value.lval;
        break;

    case 2:
        if (getParameters(ht, 2, &iptcdata, &jpeg_file) == FAILURE) {
            WRONG_PARAM_COUNT;
        }
        convert_to_string(iptcdata);
        convert_to_string(jpeg_file);
        break;

    default:
        WRONG_PARAM_COUNT;
        break;
    }

    if (_php3_check_open_basedir(jpeg_file->value.str.val)) 
		RETURN_FALSE;

    if ((fp = fopen(jpeg_file->value.str.val,"rb")) == 0) {
        php3_error(E_WARNING, "Unable to open %s", jpeg_file->value.str.val);
        RETURN_FALSE;
    }

	if (spool > 0) 
		if (!php3_header()){  /* we got a HEAD request. */
			if (spool == 2){ 
				RETURN_TRUE;	/* we only wanted to spool - report success. */
			} else 
			if (spool == 1) {
				spool = 0; 		/* we wanted the file to be spooled/returned, just return it */
			}
		}

	len = iptcdata->value.str.len;

	if (spool < 2) {
		fstat(fileno(fp),&sb);

		poi = spoolbuf = emalloc(len + 30 + sb.st_size);

		if (! spoolbuf) {
			fclose(fp);
			RETURN_FALSE;
		}
	} 

	if (php3_iptc_get1(fp,spool,poi?&poi:0) != 0xFF) {
		fclose(fp);
		RETURN_FALSE;
	}

	if (php3_iptc_get1(fp,spool,poi?&poi:0) != 0xD8) {
		fclose(fp);
		RETURN_FALSE;
	}

	while (!done) {
		marker = php3_iptc_nextmarker(fp,spool,poi?&poi:0);

		if (marker == M_EOI) { /* EOF */
			break;
		} else if (marker != M_APP13) { 
			php3_iptc_put1(fp,spool,(unsigned char)marker,poi?&poi:0);
		}

		switch (marker) {
			case M_APP13:
				/* we are going to write a new APP13 marker, so don't output the old one */
				php3_iptc_skip_variable(fp,0,0);    
				php3_iptc_read_remaining(fp,spool,poi?&poi:0);
				done = 1;
				break;

			case M_APP0:
				/* APP0 is in each and every JPEG, so when we hit APP0 we insert our new APP13! */
				php3_iptc_skip_variable(fp,spool,poi?&poi:0);

				if (len & 1) len++; /* make the length even */

				psheader[ 2 ] = (len+28)>>8;
				psheader[ 3 ] = (len+28)&0xff;

				for (inx = 0; inx < 28; inx++)
					php3_iptc_put1(fp,spool,psheader[inx],poi?&poi:0);

				php3_iptc_put1(fp,spool,(unsigned char)(len>>8),poi?&poi:0);
				php3_iptc_put1(fp,spool,(unsigned char)(len&0xff),poi?&poi:0);
					
				for (inx = 0; inx < len; inx++)
					php3_iptc_put1(fp,spool,iptcdata->value.str.val[inx],poi?&poi:0);
				break;

			case M_SOS:								
				/* we hit data, no more marker-inserting can be done! */
				php3_iptc_read_remaining(fp,spool,poi?&poi:0);
				done = 1;
				break;
			
			default:
				php3_iptc_skip_variable(fp,spool,poi?&poi:0);
				break;
		}
	}

	fclose(fp);

	if (spool < 2) {
		RETVAL_STRINGL(spoolbuf,poi - spoolbuf,0);
	} else {
		RETURN_TRUE;
	}
}

/* {{{ proto array iptcparse(string iptcdata)
   Parse binary IPTC-data into associative array.  */
PHP_FUNCTION(iptcparse)
{
	unsigned int length, inx, len, inheader, tagsfound;
	unsigned char *buffer;
	unsigned char recnum, dataset;
	unsigned char key[ 16 ];
	pval *values, *str, **element;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	inx = 0;
	length = str->value.str.len;
	buffer = str->value.str.val;

	inheader = 0; /* have we already found the IPTC-Header??? */
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

		sprintf(key,"%d#%03d",(unsigned int) dataset,(unsigned int) recnum);

		if ((inx + len) > length)
			break;

		if (tagsfound == 0) { /* found the 1st tag - initialize the return array */
			if (array_init(return_value) == FAILURE) {
				php3_error(E_ERROR, "Unable to initialize array");
				RETURN_FALSE;
	  		}
		}

		if (_php3_hash_find(return_value->value.ht,key,strlen(key) + 1,(void **) &element) == FAILURE) {
			values = emalloc(sizeof(pval));
			INIT_PZVAL(values);
			if (array_init(values) == FAILURE) {
				php3_error(E_ERROR, "Unable to initialize array");
				RETURN_FALSE;
			}
			
			_php3_hash_update(return_value->value.ht, key, strlen(key)+1, (void *) &values, sizeof(pval*), (void **) &element);
		} 
			
		add_next_index_stringl(*element,buffer+inx,len,1);

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
 */
