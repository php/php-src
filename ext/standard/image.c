/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Rasmus Lerdorf                                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
/* 
 * Based on Daniel Schmitt's imageinfo.c which carried the following
 * Copyright notice.
 */

/*
 * imageinfo.c
 *
 * Simple routines to extract image width/height data from GIF/JPEG files.
 *
 * Copyright (c) 1997 Daniel Schmitt, opal online publishing, Bonn, Germany.
 *
 * Includes code snippets from rdjpgcom.c, 
 * Copyright (c) 1994-1995 Thomas G. Lane
 * from release 6a of the Independent JPEG Group's software.
 *
 * Legal status: see GNU General Public License version 2 or later.
 *
 */

#include "php.h"
#include <stdio.h>
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include "fopen-wrappers.h"
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "image.h"

/* file type markers */
const char php3_sig_gif[3] =
{'G', 'I', 'F'};
const char php3_sig_jpg[3] =
{(char) 0xff, (char) 0xd8, (char) 0xff};
const char php3_sig_png[8] =
{(char) 0x89, (char) 0x50, (char) 0x4e,
 (char) 0x47, (char) 0x0d, (char) 0x0a,
 (char) 0x1a, (char) 0x0a};

/* return info as a struct, to make expansion easier */

struct gfxinfo {
	unsigned int width;
	unsigned int height;
};

/* routine to handle GIF files. If only everything were that easy... ;} */
static struct gfxinfo *php3_handle_gif (FILE *fp)
{
	struct gfxinfo *result = NULL;
	unsigned char a[2];

	result = (struct gfxinfo *) emalloc(sizeof(struct gfxinfo));
	fseek(fp, 6L, SEEK_SET);
	fread(a,sizeof(a),1,fp);
	result->width = (unsigned short)a[0] | (((unsigned short)a[1])<<8);
	fread(a,sizeof(a),1,fp);
	result->height = (unsigned short)a[0] | (((unsigned short)a[1])<<8);
	return result;
}

static unsigned long php3_read4(FILE *fp)
{
	unsigned char a[ 4 ];

	/* just return 0 if we hit the end-of-file */
	if (fread(a,sizeof(a),1,fp) != 1) return 0;

	return (((unsigned long) a[ 0 ]) << 24) + (((unsigned long) a[ 1 ]) << 16) + (((unsigned long) a[ 2 ]) << 8) + ((unsigned long) a[ 3 ]);

}

/* routine to handle PNG files. - even easier */
static struct gfxinfo *php3_handle_png(FILE *fp)
{
	struct gfxinfo *result = NULL;
	unsigned long in_width, in_height;

	result = (struct gfxinfo *) emalloc(sizeof(struct gfxinfo));
	fseek(fp, 16L, SEEK_SET);
	in_width = php3_read4(fp);
	in_height = php3_read4(fp);
	result->width = (unsigned int) in_width;
	result->height = (unsigned int) in_height;
	return result;
}

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

static unsigned short php3_read2(FILE *fp)
{
	unsigned char a[ 2 ];

	/* just return 0 if we hit the end-of-file */
	if (fread(a,sizeof(a),1,fp) != 1) return 0;

	return (((unsigned short) a[ 0 ]) << 8) + ((unsigned short) a[ 1 ]);
}

static unsigned int php3_next_marker(FILE *fp)
	 /* get next marker byte from file */
{
	int c;

	/* skip unimportant stuff */

	c = getc(fp);

	while (c != 0xff) { 
		if ((c = getc(fp)) == EOF)
			return M_EOI; /* we hit EOF */
	}

	/* get marker byte, swallowing possible padding */
	do {
		if ((c = getc(fp)) == EOF)
			return M_EOI;		/* we hit EOF */
	} while (c == 0xff);

	return (unsigned int) c;
}

static void php3_skip_variable(FILE *fp)
	 /* skip over a variable-length block; assumes proper length marker */
{
	unsigned short length;

	length = php3_read2(fp);
	length -= 2;				/* length includes itself */
	fseek(fp, (long) length, SEEK_CUR);	/* skip the header */
}

static void php3_read_APP(FILE *fp,unsigned int marker,pval *info)
{
	unsigned short length;
	unsigned char *buffer;
	unsigned char markername[ 16 ];

	length = php3_read2(fp);
	length -= 2;				/* length includes itself */

    buffer = emalloc(length);

 	if (fread(buffer,length,1,fp) != 1) {
		return;
	}

	sprintf(markername,"APP%d",marker - M_APP0);

	add_assoc_stringl(info,markername,buffer,length,1);

	efree(buffer);
}

static struct gfxinfo *php3_handle_jpeg(FILE *fp,pval *info)
	 /* main loop to parse JPEG structure */
{
	struct gfxinfo *result = NULL;
	unsigned int marker;
	unsigned short in_width, in_height;

	fseek(fp, 0L, SEEK_SET);		/* position file pointer on SOF */

	if (getc(fp) != 0xFF)			/* JPEG header... */
		return NULL;
		
	if (getc(fp) != M_SOI)			/* JPEG header... */
		return NULL;

	for (;;) {
		marker = php3_next_marker(fp);
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
					fseek(fp, 3L, SEEK_CUR);	/* skip length and precision bytes */
					in_height = php3_read2(fp);
					in_width = php3_read2(fp);
					/* fill a gfxinfo struct to return the data */
					result = (struct gfxinfo *) emalloc(sizeof(struct gfxinfo));

					result->width = (unsigned int) in_width;
					result->height = (unsigned int) in_height;	

					if (! info) /* if we don't want an extanded info -> return */
						return result;
				} else {
					php3_skip_variable(fp);
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
					php3_read_APP(fp,marker,info); /* read all the app markes... */
				} else {
				    php3_skip_variable(fp);
				}
				break;

			case M_SOS:
			case M_EOI:
				return result;	/* we're about to hit image data, or are at EOF. stop processing. */
				break;

			default:
				php3_skip_variable(fp);		/* anything else isn't interesting */
				break;
		}
	}

	return NULL;
}

/* main function */
PHP_FUNCTION(getimagesize)
{
	pval *arg1,*info = 0;
	FILE *fp;
	int itype = 0;
	char filetype[3];
	char pngtype[8];
	char temp[64];
	struct gfxinfo *result = NULL;
	
	switch(ARG_COUNT(ht)){
	case 1:
		if (getParameters(ht, 1, &arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string(arg1);
		break;

	case 2:
		if (getParameters(ht, 2, &arg1, &info) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		if (!ParameterPassedByReference(ht, 2)) {
            php3_error(E_WARNING, "Array to be filled with values must be passed by reference.");
            RETURN_FALSE;
        }

		pval_destructor(info);
		if (array_init(info) == FAILURE) {
			return;
		}

		convert_to_string(arg1);
		break;

	default:
		WRONG_PARAM_COUNT;
		break;
	}
		
	/* Check open_basedir */
	if (_php3_check_open_basedir(arg1->value.str.val)) return;
	
	if ((fp = fopen(arg1->value.str.val,"rb")) == 0) {
		php3_error(E_WARNING, "Unable to open %s", arg1->value.str.val);
		return;
	}
	fread(filetype,sizeof(filetype),1,fp);
	if (!memcmp(filetype, php3_sig_gif, 3)) {
		result = php3_handle_gif (fp);
		itype = 1;
	} else if (!memcmp(filetype, php3_sig_jpg, 3)) {
		result = php3_handle_jpeg(fp,info);
		itype = 2;
	} else if (!memcmp(filetype, php3_sig_png, 3)) {
		fseek(fp, 0L, SEEK_SET);
		fread(pngtype, sizeof(pngtype), 1, fp);
		if (!memcmp(pngtype, php3_sig_png, 8)) {
			result = php3_handle_png(fp);
			itype = 3;
		} else {
			php3_error(E_WARNING, "PNG file corrupted by ASCII conversion");
		}
	}
	fclose(fp);
	if (result) {
		if (array_init(return_value) == FAILURE) {
			php3_error(E_ERROR, "Unable to initialize array");
			if (result) efree(result);
			return;
		}
		add_index_long(return_value, 0, result->width);
		add_index_long(return_value, 1, result->height);
		add_index_long(return_value, 2, itype);
		sprintf(temp, "width=\"%d\" height=\"%d\"", result->width, result->height); /* safe */
		add_index_string(return_value, 3, temp, 1);
		
		efree(result);
	}
}
