#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gd_errors.h"

#include "php.h"

/* Used only when debugging GIF compression code */
/* #define DEBUGGING_ENVARS */

#ifdef DEBUGGING_ENVARS

static int verbose_set = 0;
static int verbose;
#define VERBOSE (verbose_set?verbose:set_verbose())

static int set_verbose(void)
{
	verbose = !!getenv("GIF_VERBOSE");
	verbose_set = 1;
	return(verbose);
}

#else

#define VERBOSE 0

#endif


#define        MAXCOLORMAPSIZE         256

#define        TRUE    1
#define        FALSE   0

#define CM_RED         0
#define CM_GREEN       1
#define CM_BLUE                2

#define        MAX_LWZ_BITS            12

#define INTERLACE              0x40
#define LOCALCOLORMAP  0x80
#define BitSet(byte, bit)      (((byte) & (bit)) == (bit))

#define        ReadOK(file,buffer,len) (gdGetBuf(buffer, len, file) > 0)

#define LM_to_uint(a,b)                        (((b)<<8)|(a))

/* We may eventually want to use this information, but def it out for now */
#if 0
static struct {
	unsigned int    Width;
	unsigned int    Height;
	unsigned char   ColorMap[3][MAXCOLORMAPSIZE];
	unsigned int    BitPixel;
	unsigned int    ColorResolution;
	unsigned int    Background;
	unsigned int    AspectRatio;
} GifScreen;
#endif

#if 0
static struct {
	int     transparent;
	int     delayTime;
	int     inputFlag;
	int     disposal;
} Gif89 = { -1, -1, -1, 0 };
#endif

#define STACK_SIZE ((1<<(MAX_LWZ_BITS))*2)

#define CSD_BUF_SIZE 280

typedef struct {
	unsigned char    buf[CSD_BUF_SIZE];
	int              curbit, lastbit, done, last_byte;
} CODE_STATIC_DATA;

typedef struct {
	int fresh;
	int code_size, set_code_size;
	int max_code, max_code_size;
	int firstcode, oldcode;
	int clear_code, end_code;
	int table[2][(1<< MAX_LWZ_BITS)];
	int stack[STACK_SIZE], *sp;
	CODE_STATIC_DATA scd;
} LZW_STATIC_DATA;

static int ReadColorMap (gdIOCtx *fd, int number, unsigned char (*buffer)[256]);
static int DoExtension (gdIOCtx *fd, int label, int *Transparent, int *ZeroDataBlockP);
static int GetDataBlock (gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP);
static int GetCode (gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag, int *ZeroDataBlockP);
static int LWZReadByte (gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size, int *ZeroDataBlockP);

static void ReadImage (gdImagePtr im, gdIOCtx *fd, int len, int height, unsigned char (*cmap)[256], int interlace, int *ZeroDataBlockP); /*1.4//, int ignore); */

gdImagePtr gdImageCreateFromGifSource(gdSourcePtr inSource) /* {{{ */
{
	gdIOCtx         *in = gdNewSSCtx(inSource, NULL);
	gdImagePtr      im;

	im = gdImageCreateFromGifCtx(in);

	in->gd_free(in);

	return im;
}
/* }}} */

gdImagePtr gdImageCreateFromGif(FILE *fdFile) /* {{{ */
{
	gdIOCtx		*fd = gdNewFileCtx(fdFile);
	gdImagePtr    	im = 0;

	im = gdImageCreateFromGifCtx(fd);

	fd->gd_free(fd);

	return im;
}
/* }}} */

gdImagePtr gdImageCreateFromGifCtx(gdIOCtxPtr fd) /* {{{ */
{
	int BitPixel;
#if 0
	int ColorResolution;
	int Background;
	int AspectRatio;
#endif
	int Transparent = (-1);
	unsigned char   buf[16];
	unsigned char   c;
	unsigned char   ColorMap[3][MAXCOLORMAPSIZE];
	unsigned char   localColorMap[3][MAXCOLORMAPSIZE];
	int             imw, imh, screen_width, screen_height;
	int             useGlobalColormap;
	int             bitPixel;
	int	       i;
	/*1.4//int             imageCount = 0; */

	int ZeroDataBlock = FALSE;
	int haveGlobalColormap;
	gdImagePtr im = 0;

	memset(ColorMap, 0, 3 * MAXCOLORMAPSIZE);
	memset(localColorMap, 0, 3 * MAXCOLORMAPSIZE);

	/*1.4//imageNumber = 1; */
	if (! ReadOK(fd,buf,6)) {
		return 0;
	}
	if (strncmp((char *)buf,"GIF",3) != 0) {
		return 0;
	}

	if (memcmp((char *)buf+3, "87a", 3) == 0) {
		/* GIF87a */
	} else if (memcmp((char *)buf+3, "89a", 3) == 0) {
		/* GIF89a */
	} else {
		return 0;
	}

	if (! ReadOK(fd,buf,7)) {
		return 0;
	}

	BitPixel        = 2<<(buf[4]&0x07);
#if 0
	ColorResolution = (int) (((buf[4]&0x70)>>3)+1);
	Background      = buf[5];
	AspectRatio     = buf[6];
#endif
	screen_width = imw = LM_to_uint(buf[0],buf[1]);
	screen_height = imh = LM_to_uint(buf[2],buf[3]);

	haveGlobalColormap = BitSet(buf[4], LOCALCOLORMAP);    /* Global Colormap */
	if (haveGlobalColormap) {
		if (ReadColorMap(fd, BitPixel, ColorMap)) {
			return 0;
		}
	}

	for (;;) {
		int top, left;
		int width, height;

		if (! ReadOK(fd,&c,1)) {
			return 0;
		}
		if (c == ';') {         /* GIF terminator */
			goto terminated;
		}

		if (c == '!') {         /* Extension */
			if (! ReadOK(fd,&c,1)) {
				return 0;
			}
			DoExtension(fd, c, &Transparent, &ZeroDataBlock);
			continue;
		}

		if (c != ',') {         /* Not a valid start character */
			continue;
		}

		/*1.4//++imageCount; */

		if (! ReadOK(fd,buf,9)) {
			return 0;
		}

		useGlobalColormap = ! BitSet(buf[8], LOCALCOLORMAP);

		bitPixel = 1<<((buf[8]&0x07)+1);
		left = LM_to_uint(buf[0], buf[1]);
		top = LM_to_uint(buf[2], buf[3]);
		width = LM_to_uint(buf[4], buf[5]);
		height = LM_to_uint(buf[6], buf[7]);

		if (left + width > screen_width || top + height > screen_height) {
			if (VERBOSE) {
				printf("Frame is not confined to screen dimension.\n");
			}
			return 0;
		}

		if (!(im = gdImageCreate(width, height))) {
			return 0;
		}
		im->interlace = BitSet(buf[8], INTERLACE);
		if (!useGlobalColormap) {
			if (ReadColorMap(fd, bitPixel, localColorMap)) {
				gdImageDestroy(im);
				return 0;
			}
			ReadImage(im, fd, width, height, localColorMap,
					BitSet(buf[8], INTERLACE), &ZeroDataBlock);
		} else {
			if (!haveGlobalColormap) {
				gdImageDestroy(im);
				return 0;
			}
			ReadImage(im, fd, width, height,
						ColorMap,
						BitSet(buf[8], INTERLACE), &ZeroDataBlock);
		}
		if (Transparent != (-1)) {
			gdImageColorTransparent(im, Transparent);
		}
		goto terminated;
	}

terminated:
	/* Terminator before any image was declared! */
	if (!im) {
		return 0;
	}
	/* Check for open colors at the end, so
	   we can reduce colorsTotal and ultimately
	   BitsPerPixel */
	for (i=((im->colorsTotal-1)); (i>=0); i--) {
		if (im->open[i]) {
			im->colorsTotal--;
		} else {
			break;
		}
	}
	if (!im->colorsTotal) {
		gdImageDestroy(im);
		return 0;
	}
	return im;
}
/* }}} */

static int ReadColorMap(gdIOCtx *fd, int number, unsigned char (*buffer)[256]) /* {{{ */
{
	int             i;
	unsigned char   rgb[3];


	for (i = 0; i < number; ++i) {
		if (! ReadOK(fd, rgb, sizeof(rgb))) {
			return TRUE;
		}
		buffer[CM_RED][i] = rgb[0] ;
		buffer[CM_GREEN][i] = rgb[1] ;
		buffer[CM_BLUE][i] = rgb[2] ;
	}


	return FALSE;
}
/* }}} */

static int
DoExtension(gdIOCtx *fd, int label, int *Transparent, int *ZeroDataBlockP)
{
	unsigned char buf[256];

	switch (label) {
		case 0xf9:              /* Graphic Control Extension */
			memset(buf, 0, 4); /* initialize a few bytes in the case the next function fails */
               (void) GetDataBlock(fd, (unsigned char*) buf, ZeroDataBlockP);
#if 0
			Gif89.disposal    = (buf[0] >> 2) & 0x7;
			Gif89.inputFlag   = (buf[0] >> 1) & 0x1;
			Gif89.delayTime   = LM_to_uint(buf[1],buf[2]);
#endif
			if ((buf[0] & 0x1) != 0)
				*Transparent = buf[3];

			while (GetDataBlock(fd, (unsigned char*) buf, ZeroDataBlockP) > 0);
			return FALSE;
		default:
			break;
	}
       while (GetDataBlock(fd, (unsigned char*) buf, ZeroDataBlockP) > 0)
		;

	return FALSE;
}
/* }}} */

static int
GetDataBlock_(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP)
{
	unsigned char   count;

	if (! ReadOK(fd,&count,1)) {
		return -1;
	}

	*ZeroDataBlockP = count == 0;

	if ((count != 0) && (! ReadOK(fd, buf, count))) {
		return -1;
	}

	return count;
}
/* }}} */

static int
GetDataBlock(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP)
{
	int rv;
	int i;

	rv = GetDataBlock_(fd,buf, ZeroDataBlockP);
	if (VERBOSE) {
		char *tmp = NULL;
		if (rv > 0) {
			tmp = safe_emalloc(3 * rv, sizeof(char), 1);
			for (i=0;i<rv;i++) {
				sprintf(&tmp[3*sizeof(char)*i], " %02x", buf[i]);
			}
		} else {
			tmp = estrdup("");
		}
		gd_error_ex(GD_NOTICE, "[GetDataBlock returning %d: %s]", rv, tmp);
		efree(tmp);
	}
	return(rv);
}
/* }}} */

static int
GetCode_(gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag, int *ZeroDataBlockP)
{
	int           i, j, ret;
	int           count;

	if (flag) {
		scd->curbit = 0;
		scd->lastbit = 0;
		scd->last_byte = 2;
		scd->done = FALSE;
		return 0;
	}

	if ( (scd->curbit + code_size) >= scd->lastbit) {
		if (scd->done) {
			if (scd->curbit >= scd->lastbit) {
				/* Oh well */
			}
			return -1;
		}
		scd->buf[0] = scd->buf[scd->last_byte-2];
		scd->buf[1] = scd->buf[scd->last_byte-1];

               if ((count = GetDataBlock(fd, &scd->buf[2], ZeroDataBlockP)) <= 0)
			scd->done = TRUE;

		scd->last_byte = 2 + count;
		scd->curbit = (scd->curbit - scd->lastbit) + 16;
		scd->lastbit = (2+count)*8 ;
	}

	if ((scd->curbit + code_size - 1) >= (CSD_BUF_SIZE * 8)) {
		ret = -1;
	} else {
		ret = 0;
		for (i = scd->curbit, j = 0; j < code_size; ++i, ++j) {
			ret |= ((scd->buf[i / 8] & (1 << (i % 8))) != 0) << j;
		}
	}

	scd->curbit += code_size;
	return ret;
}

static int
GetCode(gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag, int *ZeroDataBlockP)
{
	int rv;

 rv = GetCode_(fd, scd, code_size,flag, ZeroDataBlockP);
 if (VERBOSE) printf("[GetCode(,%d,%d) returning %d]\n",code_size,flag,rv);
	return(rv);
}
/* }}} */

static int
LWZReadByte_(gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size, int *ZeroDataBlockP)
{
	int code, incode, i;

	if (flag) {
		sd->set_code_size = input_code_size;
		sd->code_size = sd->set_code_size+1;
		sd->clear_code = 1 << sd->set_code_size ;
		sd->end_code = sd->clear_code + 1;
		sd->max_code_size = 2*sd->clear_code;
		sd->max_code = sd->clear_code+2;

		GetCode(fd, &sd->scd, 0, TRUE, ZeroDataBlockP);

		sd->fresh = TRUE;

		for (i = 0; i < sd->clear_code; ++i) {
			sd->table[0][i] = 0;
			sd->table[1][i] = i;
		}
		for (; i < (1<<MAX_LWZ_BITS); ++i)
			sd->table[0][i] = sd->table[1][0] = 0;

		sd->sp = sd->stack;

		return 0;
	} else if (sd->fresh) {
		sd->fresh = FALSE;
		do {
			sd->firstcode = sd->oldcode =
			GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP);
		} while (sd->firstcode == sd->clear_code);
		return sd->firstcode;
	}

	if (sd->sp > sd->stack)
		return *--sd->sp;

	while ((code = GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP)) >= 0) {
		if (code == sd->clear_code) {
			for (i = 0; i < sd->clear_code; ++i) {
				sd->table[0][i] = 0;
				sd->table[1][i] = i;
			}
			for (; i < (1<<MAX_LWZ_BITS); ++i)
				sd->table[0][i] = sd->table[1][i] = 0;
			sd->code_size = sd->set_code_size+1;
			sd->max_code_size = 2*sd->clear_code;
			sd->max_code = sd->clear_code+2;
			sd->sp = sd->stack;
			sd->firstcode = sd->oldcode =
								GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP);
			return sd->firstcode;
		} else if (code == sd->end_code) {
			int             count;
			unsigned char   buf[260];

			if (*ZeroDataBlockP)
				return -2;

			while ((count = GetDataBlock(fd, buf, ZeroDataBlockP)) > 0)
				;

			if (count != 0)
				return -2;
		}

		incode = code;

		if (sd->sp == (sd->stack + STACK_SIZE)) {
			/* Bad compressed data stream */
			return -1;
		}

		if (code >= sd->max_code) {
			*sd->sp++ = sd->firstcode;
			code = sd->oldcode;
		}

		while (code >= sd->clear_code) {
			if (sd->sp == (sd->stack + STACK_SIZE)) {
				/* Bad compressed data stream */
				return -1;
			}
			*sd->sp++ = sd->table[1][code];
			if (code == sd->table[0][code]) {
				/* Oh well */
			}
			code = sd->table[0][code];
		}

		*sd->sp++ = sd->firstcode = sd->table[1][code];

		if ((code = sd->max_code) <(1<<MAX_LWZ_BITS)) {
			sd->table[0][code] = sd->oldcode;
			sd->table[1][code] = sd->firstcode;
			++sd->max_code;
			if ((sd->max_code >= sd->max_code_size) &&
					(sd->max_code_size < (1<<MAX_LWZ_BITS))) {
				sd->max_code_size *= 2;
				++sd->code_size;
			}
		}

		sd->oldcode = incode;

		if (sd->sp > sd->stack)
			return *--sd->sp;
	}
	return code;
}
/* }}} */

static int
LWZReadByte(gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size, int *ZeroDataBlockP)
{
	int rv;

 rv = LWZReadByte_(fd, sd, flag, input_code_size, ZeroDataBlockP);
 if (VERBOSE) printf("[LWZReadByte(,%d,%d) returning %d]\n",flag,input_code_size,rv);
	return(rv);
}
/* }}} */

static void
ReadImage(gdImagePtr im, gdIOCtx *fd, int len, int height, unsigned char (*cmap)[256], int interlace, int *ZeroDataBlockP) /*1.4//, int ignore) */
{
	unsigned char   c;
	int             v;
	int             xpos = 0, ypos = 0, pass = 0;
	int i;
	LZW_STATIC_DATA sd;


	/*
	 **  Initialize the Compression routines
	 */
	if (! ReadOK(fd,&c,1)) {
		return;
	}

	if (c > MAX_LWZ_BITS) {
		return;
	}

	/* Stash the color map into the image */
	for (i=0; (i<gdMaxColors); i++) {
		im->red[i] = cmap[CM_RED][i];
		im->green[i] = cmap[CM_GREEN][i];
		im->blue[i] = cmap[CM_BLUE][i];
		im->open[i] = 1;
	}
	/* Many (perhaps most) of these colors will remain marked open. */
	im->colorsTotal = gdMaxColors;
	if (LWZReadByte(fd, &sd, TRUE, c, ZeroDataBlockP) < 0) {
		return;
	}

	/*
	 **  If this is an "uninteresting picture" ignore it.
	 **  REMOVED For 1.4
	 */
	/*if (ignore) { */
	/*        while (LWZReadByte(fd, &sd, FALSE, c) >= 0) */
	/*                ; */
	/*        return; */
	/*} */

	while ((v = LWZReadByte(fd, &sd, FALSE, c, ZeroDataBlockP)) >= 0) {
		if (v >= gdMaxColors) {
			v = 0;
		}
		/* This how we recognize which colors are actually used. */
		if (im->open[v]) {
			im->open[v] = 0;
		}
		gdImageSetPixel(im, xpos, ypos, v);
		++xpos;
		if (xpos == len) {
			xpos = 0;
			if (interlace) {
				switch (pass) {
					case 0:
					case 1:
						ypos += 8; break;
					case 2:
						ypos += 4; break;
					case 3:
						ypos += 2; break;
				}

				if (ypos >= height) {
					++pass;
					switch (pass) {
						case 1:
							ypos = 4; break;
						case 2:
							ypos = 2; break;
						case 3:
							ypos = 1; break;
						default:
							goto fini;
					}
				}
			} else {
				++ypos;
			}
		}
		if (ypos >= height)
			break;
	}

fini:
	if (LWZReadByte(fd, &sd, FALSE, c, ZeroDataBlockP) >=0) {
		/* Ignore extra */
	}
}
/* }}} */
