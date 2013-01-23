/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"

#include "php.h"

#define MAX_XBM_LINE_SIZE 255

/* {{{ gdImagePtr gdImageCreateFromXbm */
gdImagePtr gdImageCreateFromXbm(FILE * fd)
{
	char fline[MAX_XBM_LINE_SIZE];
	char iname[MAX_XBM_LINE_SIZE];
	char *type;
	int value;
	unsigned int width = 0, height = 0;
	int fail = 0;
	int max_bit = 0;

	gdImagePtr im;
	int bytes = 0, i;
	int bit, x = 0, y = 0;
	int ch;
	char h[8];
	unsigned int b;

	rewind(fd);
	while (fgets(fline, MAX_XBM_LINE_SIZE, fd)) {
		fline[MAX_XBM_LINE_SIZE-1] = '\0';
		if (strlen(fline) == MAX_XBM_LINE_SIZE-1) {
			return 0;
		}
		if (sscanf(fline, "#define %s %d", iname, &value) == 2) {
			if (!(type = strrchr(iname, '_'))) {
				type = iname;
			} else {
				type++;
			}

			if (!strcmp("width", type)) {
				width = (unsigned int) value;
			}
			if (!strcmp("height", type)) {
				height = (unsigned int) value;
			}
		} else {
			if ( sscanf(fline, "static unsigned char %s = {", iname) == 1
			  || sscanf(fline, "static char %s = {", iname) == 1)
			{
				max_bit = 128;
			} else if (sscanf(fline, "static unsigned short %s = {", iname) == 1
					|| sscanf(fline, "static short %s = {", iname) == 1)
			{
				max_bit = 32768;
			}
			if (max_bit) {
				bytes = (width * height / 8) + 1;
				if (!bytes) {
					return 0;
				}
				if (!(type = strrchr(iname, '_'))) {
					type = iname;
				} else {
					type++;
				}
				if (!strcmp("bits[]", type)) {
					break;
				}
			}
 		}
	}
	if (!bytes || !max_bit) {
		return 0;
	}

	if(!(im = gdImageCreate(width, height))) {
		return 0;
	}
	gdImageColorAllocate(im, 255, 255, 255);
	gdImageColorAllocate(im, 0, 0, 0);
	h[2] = '\0';
	h[4] = '\0';
	for (i = 0; i < bytes; i++) {
		while (1) {
			if ((ch=getc(fd)) == EOF) {
				fail = 1;
				break;
			}
			if (ch == 'x') {
				break;
			}
		}
		if (fail) {
			break;
		}
		/* Get hex value */
		if ((ch=getc(fd)) == EOF) {
			break;
		}
		h[0] = ch;
		if ((ch=getc(fd)) == EOF) {
			break;
		}
		h[1] = ch;
		if (max_bit == 32768) {
			if ((ch=getc(fd)) == EOF) {
				break;
			}
			h[2] = ch;
			if ((ch=getc(fd)) == EOF) {
				break;
			}
			h[3] = ch;
		}
		sscanf(h, "%x", &b);
		for (bit = 1; bit <= max_bit; bit = bit << 1) {
			gdImageSetPixel(im, x++, y, (b & bit) ? 1 : 0);
			if (x == im->sx) {
				x = 0;
				y++;
				if (y == im->sy) {
					return im;
				}
				break;
			}
		}
	}

	php_gd_error("EOF before image was complete");
	gdImageDestroy(im);
	return 0;
}
/* }}} */

/* {{{ gdCtxPrintf */
void gdCtxPrintf(gdIOCtx * out, const char *format, ...)
{
	char *buf;
	int len;
	va_list args;

	va_start(args, format);
	len = vspprintf(&buf, 0, format, args);
	va_end(args);
	out->putBuf(out, buf, len);
	efree(buf);
}
/* }}} */

/* {{{ gdImageXbmCtx */
void gdImageXbmCtx(gdImagePtr image, char* file_name, int fg, gdIOCtx * out)
{
	int x, y, c, b, sx, sy, p;
	char *name, *f;
	size_t i, l;

	name = file_name;
	if ((f = strrchr(name, '/')) != NULL) name = f+1;
	if ((f = strrchr(name, '\\')) != NULL) name = f+1;
	name = estrdup(name);
	if ((f = strrchr(name, '.')) != NULL && !strcasecmp(f, ".XBM")) *f = '\0';
	if ((l = strlen(name)) == 0) {
		efree(name);
		name = estrdup("image");
	} else {
		for (i=0; i<l; i++) {
			/* only in C-locale isalnum() would work */
			if (!isupper(name[i]) && !islower(name[i]) && !isdigit(name[i])) {
				name[i] = '_';
			}
		}
	}

	gdCtxPrintf(out, "#define %s_width %d\n", name, gdImageSX(image));
	gdCtxPrintf(out, "#define %s_height %d\n", name, gdImageSY(image));
	gdCtxPrintf(out, "static unsigned char %s_bits[] = {\n  ", name);

	efree(name);

	b = 1;
	p = 0;
	c = 0;
	sx = gdImageSX(image);
	sy = gdImageSY(image);
	for (y = 0; y < sy; y++) {
		for (x = 0; x < sx; x++) {
			if (gdImageGetPixel(image, x, y) == fg) {
				c |= b;
			}
			if ((b == 128) || (x == sx && y == sy)) {
				b = 1;
				if (p) {
					gdCtxPrintf(out, ", ");
					if (!(p%12)) {
						gdCtxPrintf(out, "\n  ");
						p = 12;
					}
				}
				p++;
				gdCtxPrintf(out, "0x%02X", c);
				c = 0;
			} else {
				b <<= 1;
			}
		}
	}
	gdCtxPrintf(out, "};\n");
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
