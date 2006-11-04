/*
   * gd_gd2.c
   *
   * Implements the I/O and support for the GD2 format.
   *
   * Changing the definition of GD2_DBG (below) will cause copious messages
   * to be displayed while it processes requests.
   *
   * Designed, Written & Copyright 1999, Philip Warner.
   *
 */

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"

#include <zlib.h>

#define TRUE 1
#define FALSE 0

/* 2.11: not part of the API, as the save routine can figure it out
 *	from im->trueColor, and the load routine doesn't need to tell
 *	the end user the saved format. NOTE: adding 2 is assumed
 *	to result in the correct format value for truecolor!
*/
#define GD2_FMT_TRUECOLOR_RAW 3
#define GD2_FMT_TRUECOLOR_COMPRESSED 4

#define gd2_compressed(fmt) (((fmt) == GD2_FMT_COMPRESSED) || ((fmt) == GD2_FMT_TRUECOLOR_COMPRESSED))
#define gd2_truecolor(fmt) (((fmt) == GD2_FMT_TRUECOLOR_RAW) || ((fmt) == GD2_FMT_TRUECOLOR_COMPRESSED))

/* Use this for commenting out debug-print statements. */
/* Just use the first '#define' to allow all the prints... */
/* #define GD2_DBG(s) (s) */
#define GD2_DBG(s)

typedef struct
{
	int offset;
	int size;
} t_chunk_info;

extern int _gdGetColors(gdIOCtx * in, gdImagePtr im, int gd2xFlag);
extern void _gdPutColors(gdImagePtr im, gdIOCtx * out);

/* */
/* Read the extra info in the gd2 header. */
/* */
static int _gd2GetHeader(gdIOCtxPtr in, int *sx, int *sy, int *cs, int *vers, int *fmt, int *ncx, int *ncy, t_chunk_info ** chunkIdx)
{
	int i;
	int ch;
	char id[5];
	t_chunk_info *cidx;
	int sidx;
	int nc;

	GD2_DBG(php_gd_error("Reading gd2 header info"));

	for (i = 0; i < 4; i++) {
		ch = gdGetC(in);
		if (ch == EOF) {
			goto fail1;
		}
		id[i] = ch;
	}
	id[4] = 0;

	GD2_DBG(php_gd_error("Got file code: %s", id));

	/* Equiv. of 'magick'.  */
	if (strcmp(id, GD2_ID) != 0) {
		GD2_DBG(php_gd_error("Not a valid gd2 file"));
		goto fail1;
	}

	/* Version */
	if (gdGetWord(vers, in) != 1) {
		goto fail1;
	}
	GD2_DBG(php_gd_error("Version: %d", *vers));

	if ((*vers != 1) && (*vers != 2)) {
		GD2_DBG(php_gd_error("Bad version: %d", *vers));
		goto fail1;
	}

	/* Image Size */
	if (!gdGetWord(sx, in)) {
		GD2_DBG(php_gd_error("Could not get x-size"));
		goto fail1;
	}
	if (!gdGetWord(sy, in)) {
		GD2_DBG(php_gd_error("Could not get y-size"));
		goto fail1;
	}
	GD2_DBG(php_gd_error("Image is %dx%d", *sx, *sy));

	/* Chunk Size (pixels, not bytes!) */
	if (gdGetWord(cs, in) != 1) {
		goto fail1;
	}
	GD2_DBG(php_gd_error("ChunkSize: %d", *cs));

	if ((*cs < GD2_CHUNKSIZE_MIN) || (*cs > GD2_CHUNKSIZE_MAX)) {
		GD2_DBG(php_gd_error("Bad chunk size: %d", *cs));
		goto fail1;
	}

	/* Data Format */
	if (gdGetWord(fmt, in) != 1) {
		goto fail1;
	}
	GD2_DBG(php_gd_error("Format: %d", *fmt));

	if ((*fmt != GD2_FMT_RAW) && (*fmt != GD2_FMT_COMPRESSED) && (*fmt != GD2_FMT_TRUECOLOR_RAW) && (*fmt != GD2_FMT_TRUECOLOR_COMPRESSED)) {
		GD2_DBG(php_gd_error("Bad data format: %d", *fmt));
		goto fail1;
	}

	/* # of chunks wide */
	if (gdGetWord(ncx, in) != 1) {
		goto fail1;
	}
	GD2_DBG(php_gd_error("%d Chunks Wide", *ncx));

	/* # of chunks high */
	if (gdGetWord(ncy, in) != 1) {
		goto fail1;
	}
	GD2_DBG(php_gd_error("%d Chunks vertically", *ncy));

	if (gd2_compressed(*fmt)) {
		nc = (*ncx) * (*ncy);
		GD2_DBG(php_gd_error("Reading %d chunk index entries", nc));
		sidx = sizeof(t_chunk_info) * nc;
		if (sidx <= 0) {
			goto fail1;
		}
		cidx = gdCalloc(sidx, 1);
		for (i = 0; i < nc; i++) {
			if (gdGetInt(&cidx[i].offset, in) != 1) {
				goto fail1;
			}
			if (gdGetInt(&cidx[i].size, in) != 1) {
				goto fail1;
			}
		}
		*chunkIdx = cidx;
	}

	GD2_DBG(php_gd_error("gd2 header complete"));

	return 1;

fail1:
	return 0;
}

static gdImagePtr _gd2CreateFromFile (gdIOCtxPtr in, int *sx, int *sy, int *cs, int *vers, int *fmt, int *ncx, int *ncy, t_chunk_info ** cidx)
{
	gdImagePtr im;

	if (_gd2GetHeader (in, sx, sy, cs, vers, fmt, ncx, ncy, cidx) != 1) {
		GD2_DBG(php_gd_error("Bad GD2 header"));
		goto fail1;
	}

	if (gd2_truecolor(*fmt)) {
		im = gdImageCreateTrueColor(*sx, *sy);
	} else {
		im = gdImageCreate(*sx, *sy);
	}
	if (im == NULL) {
		GD2_DBG(php_gd_error("Could not create gdImage"));
		goto fail1;
	}

	if (!_gdGetColors(in, im, (*vers) == 2)) {
		GD2_DBG(php_gd_error("Could not read color palette"));
		goto fail2;
	}
	GD2_DBG(php_gd_error("Image palette completed: %d colours", im->colorsTotal));

	return im;

fail2:
	gdImageDestroy(im);
	return 0;

fail1:
	return 0;
}

static int _gd2ReadChunk (int offset, char *compBuf, int compSize, char *chunkBuf, uLongf * chunkLen, gdIOCtx * in)
{
	int zerr;

	if (gdTell(in) != offset) {
		GD2_DBG(php_gd_error("Positioning in file to %d", offset));
		gdSeek(in, offset);
	} else {
		GD2_DBG(php_gd_error("Already Positioned in file to %d", offset));
	}

	/* Read and uncompress an entire chunk. */
	GD2_DBG(php_gd_error("Reading file"));
	if (gdGetBuf(compBuf, compSize, in) != compSize) {
		return FALSE;
	}
	GD2_DBG(php_gd_error("Got %d bytes. Uncompressing into buffer of %d bytes", compSize, (int)*chunkLen));
	zerr = uncompress((unsigned char *) chunkBuf, chunkLen, (unsigned char *) compBuf, compSize);
	if (zerr != Z_OK) {
		GD2_DBG(php_gd_error("Error %d from uncompress", zerr));
		return FALSE;
	}
	GD2_DBG(php_gd_error("Got chunk"));

	return TRUE;
}

gdImagePtr gdImageCreateFromGd2 (FILE * inFile)
{
	gdIOCtx *in = gdNewFileCtx(inFile);
	gdImagePtr im;

	im = gdImageCreateFromGd2Ctx(in);

	in->gd_free(in);

	return im;
}

gdImagePtr gdImageCreateFromGd2Ptr (int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	im = gdImageCreateFromGd2Ctx(in);
	in->gd_free(in);

	return im;
}

gdImagePtr gdImageCreateFromGd2Ctx (gdIOCtxPtr in)
{
	int sx, sy;
	int i;
	int ncx, ncy, nc, cs, cx, cy;
	int x, y, ylo, yhi, xlo, xhi;
	int vers, fmt;
	t_chunk_info *chunkIdx = NULL;	/* So we can gdFree it with impunity. */
	unsigned char *chunkBuf = NULL;	/* So we can gdFree it with impunity. */
	int chunkNum = 0;
	int chunkMax = 0;
	uLongf chunkLen;
	int chunkPos = 0;
	int compMax = 0;
	int bytesPerPixel;
	char *compBuf = NULL;		/* So we can gdFree it with impunity. */

	gdImagePtr im;

	/* Get the header */
	if (!(im = _gd2CreateFromFile(in, &sx, &sy, &cs, &vers, &fmt, &ncx, &ncy, &chunkIdx))) {
		 return 0;
	}

	bytesPerPixel = im->trueColor ? 4 : 1;
	nc = ncx * ncy;

	if (gd2_compressed(fmt)) {
		/* Find the maximum compressed chunk size. */
		compMax = 0;
		for (i = 0; (i < nc); i++) {
			if (chunkIdx[i].size > compMax) {
				compMax = chunkIdx[i].size;
			}
		}
		compMax++;

		/* Allocate buffers */
		chunkMax = cs * bytesPerPixel * cs;
		if (chunkMax <= 0) {
			return 0;
		}
		chunkBuf = gdCalloc(chunkMax, 1);
		compBuf = gdCalloc(compMax, 1);

		GD2_DBG(php_gd_error("Largest compressed chunk is %d bytes", compMax));
	}

	/* Read the data... */
	for (cy = 0; (cy < ncy); cy++) {
		for (cx = 0; (cx < ncx); cx++) {
			ylo = cy * cs;
			yhi = ylo + cs;
			if (yhi > im->sy) {
				yhi = im->sy;
			}

			GD2_DBG(php_gd_error("Processing Chunk %d (%d, %d), y from %d to %d", chunkNum, cx, cy, ylo, yhi));

			if (gd2_compressed(fmt)) {
				chunkLen = chunkMax;

				if (!_gd2ReadChunk(chunkIdx[chunkNum].offset, compBuf, chunkIdx[chunkNum].size, (char *) chunkBuf, &chunkLen, in)) {
					GD2_DBG(php_gd_error("Error reading comproessed chunk"));
					goto fail2;
				}

				chunkPos = 0;
			}

			for (y = ylo; (y < yhi); y++) {
				xlo = cx * cs;
				xhi = xlo + cs;
				if (xhi > im->sx) {
					xhi = im->sx;
				}

				if (!gd2_compressed(fmt)) {
					for (x = xlo; x < xhi; x++) {
						if (im->trueColor) {
							if (!gdGetInt(&im->tpixels[y][x], in)) {
								im->tpixels[y][x] = 0;
							}
						} else {
							int ch;
							if (!gdGetByte(&ch, in)) {
								ch = 0;
							}
							im->pixels[y][x] = ch;
						}
					}
				} else {
					for (x = xlo; x < xhi; x++) {
						if (im->trueColor) {
							/* 2.0.1: work around a gcc bug by being verbose. TBB */
							int a = chunkBuf[chunkPos++] << 24;
							int r = chunkBuf[chunkPos++] << 16;
							int g = chunkBuf[chunkPos++] << 8;
							int b = chunkBuf[chunkPos++];
							im->tpixels[y][x] = a + r + g + b;
						} else {
							im->pixels[y][x] = chunkBuf[chunkPos++];
						}
					}
				}
			}
			chunkNum++;
		}
	}

	GD2_DBG(php_gd_error("Freeing memory"));

	if (chunkBuf) {
		gdFree(chunkBuf);
	}
	if (compBuf) {
		gdFree(compBuf);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}

	GD2_DBG(php_gd_error("Done"));

	return im;

fail2:
	gdImageDestroy(im);
	if (chunkBuf) {
		gdFree(chunkBuf);
	}
	if (compBuf) {
		gdFree(compBuf);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}

	return 0;
}

gdImagePtr gdImageCreateFromGd2PartPtr (int size, void *data, int srcx, int srcy, int w, int h)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	im = gdImageCreateFromGd2PartCtx(in, srcx, srcy, w, h);
	in->gd_free(in);

	return im;
}

gdImagePtr gdImageCreateFromGd2Part (FILE * inFile, int srcx, int srcy, int w, int h)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);

	im = gdImageCreateFromGd2PartCtx(in, srcx, srcy, w, h);

	in->gd_free(in);

	return im;
}

gdImagePtr gdImageCreateFromGd2PartCtx (gdIOCtx * in, int srcx, int srcy, int w, int h)
{
	int scx, scy, ecx, ecy, fsx, fsy;
	int nc, ncx, ncy, cs, cx, cy;
	int x, y, ylo, yhi, xlo, xhi;
	int dstart, dpos;
	int i;
	/* 2.0.12: unsigned is correct; fixes problems with color munging. Thanks to Steven Brown. */
	unsigned int ch;
	int vers, fmt;
	t_chunk_info *chunkIdx = NULL;
	unsigned char *chunkBuf = NULL;
	int chunkNum;
	int chunkMax = 0;
	uLongf chunkLen;
	int chunkPos = 0;
	int compMax;
	char *compBuf = NULL;

	gdImagePtr im;

	if (w<1 || h <1) {
		return 0;
	}

	/* The next few lines are basically copied from gd2CreateFromFile
	 * we change the file size, so don't want to use the code directly.
	 * but we do need to know the file size.
	 */
	if (_gd2GetHeader(in, &fsx, &fsy, &cs, &vers, &fmt, &ncx, &ncy, &chunkIdx) != 1) {
		goto fail1;
	}

	GD2_DBG(php_gd_error("File size is %dx%d", fsx, fsy));

	/* This is the difference - make a file based on size of chunks. */
	if (gd2_truecolor(fmt)) {
		im = gdImageCreateTrueColor(w, h);
	} else {
		im = gdImageCreate(w, h);
	}
	if (im == NULL) {
		goto fail1;
	}

	if (!_gdGetColors(in, im, vers == 2)) {
		goto fail2;
	}
	GD2_DBG(php_gd_error("Image palette completed: %d colours", im->colorsTotal));

	/* Process the header info */
	nc = ncx * ncy;

	if (gd2_compressed(fmt)) {
		/* Find the maximum compressed chunk size. */
		compMax = 0;
		for (i = 0; (i < nc); i++) {
			if (chunkIdx[i].size > compMax) {
				compMax = chunkIdx[i].size;
			}
		}
		compMax++;

		if (im->trueColor) {
			chunkMax = cs * cs * 4;
		} else {
			chunkMax = cs * cs;
		}
		if (chunkMax <= 0) {
			goto fail2;
		}

		chunkBuf = gdCalloc(chunkMax, 1);
		compBuf = gdCalloc(compMax, 1);
	}

	/* Work out start/end chunks */
	scx = srcx / cs;
	scy = srcy / cs;
	if (scx < 0) {
		scx = 0;
	}
	if (scy < 0) {
		scy = 0;
	}

	ecx = (srcx + w) / cs;
	ecy = (srcy + h) / cs;
	if (ecx >= ncx) {
		ecx = ncx - 1;
	}
	if (ecy >= ncy) {
		ecy = ncy - 1;
	}

	/* Remember file position of image data. */
	dstart = gdTell(in);
	GD2_DBG(php_gd_error("Data starts at %d", dstart));

	/* Loop through the chunks. */
	for (cy = scy; (cy <= ecy); cy++) {
		ylo = cy * cs;
		yhi = ylo + cs;
		if (yhi > fsy) {
			yhi = fsy;
		}

		for (cx = scx; cx <= ecx; cx++) {

			xlo = cx * cs;
			xhi = xlo + cs;
			if (xhi > fsx) {
				xhi = fsx;
			}

			GD2_DBG(php_gd_error("Processing Chunk (%d, %d), from %d to %d", cx, cy, ylo, yhi));

			if (!gd2_compressed(fmt)) {
				GD2_DBG(php_gd_error("Using raw format data"));
				if (im->trueColor) {
					dpos = (cy * (cs * fsx) * 4 + cx * cs * (yhi - ylo) * 4) + dstart;
				} else {
					dpos = cy * (cs * fsx) + cx * cs * (yhi - ylo) + dstart;
				}

				/* gd 2.0.11: gdSeek returns TRUE on success, not 0. Longstanding bug. 01/16/03 */
				if (!gdSeek(in, dpos)) {
					php_gd_error_ex(E_WARNING, "Error from seek: %d", errno);
					goto fail2;
				}
				GD2_DBG(php_gd_error("Reading (%d, %d) from position %d", cx, cy, dpos - dstart));
			} else {
				chunkNum = cx + cy * ncx;

				chunkLen = chunkMax;
				if (!_gd2ReadChunk (chunkIdx[chunkNum].offset, compBuf, chunkIdx[chunkNum].size, (char *)chunkBuf, &chunkLen, in)) {
					php_gd_error("Error reading comproessed chunk");
					goto fail2;
				}
				chunkPos = 0;
				GD2_DBG(php_gd_error("Reading (%d, %d) from chunk %d", cx, cy, chunkNum));
			}

			GD2_DBG(php_gd_error("   into (%d, %d) - (%d, %d)", xlo, ylo, xhi, yhi));

			for (y = ylo; (y < yhi); y++) {
				for (x = xlo; x < xhi; x++) {
					if (!gd2_compressed(fmt)) {
						if (im->trueColor) {
							if (!gdGetInt((int *)&ch, in)) {
								ch = 0;
							}
						} else {
							ch = gdGetC(in);
							if ((int)ch == EOF) {
								ch = 0;
							}
						}
					} else {
						if (im->trueColor) {
							ch = chunkBuf[chunkPos++];
							ch = (ch << 8) + chunkBuf[chunkPos++];
							ch = (ch << 8) + chunkBuf[chunkPos++];
							ch = (ch << 8) + chunkBuf[chunkPos++];
						} else {
							ch = chunkBuf[chunkPos++];
						}
					}

					/* Only use a point that is in the image. */
					if ((x >= srcx) && (x < (srcx + w)) && (x < fsx) && (x >= 0) && (y >= srcy) && (y < (srcy + h)) && (y < fsy) && (y >= 0)) {
						if (im->trueColor) {
							im->tpixels[y - srcy][x - srcx] = ch;
						} else {
							im->pixels[y - srcy][x - srcx] = ch;
						}
					}
				}
			}
		}
	}

	if (chunkBuf) {
		gdFree(chunkBuf);
	}
	if (compBuf) {
		gdFree(compBuf);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}

	return im;

fail2:
	gdImageDestroy(im);
fail1:
	if (chunkBuf) {
		gdFree(chunkBuf);
	}
	if (compBuf) {
		gdFree(compBuf);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}

	return 0;
}

static void _gd2PutHeader (gdImagePtr im, gdIOCtx * out, int cs, int fmt, int cx, int cy)
{
	int i;

	/* Send the gd2 id, to verify file format. */
	for (i = 0; i < 4; i++) {
		gdPutC((unsigned char) (GD2_ID[i]), out);
	}

	/* We put the version info first, so future versions can easily change header info. */

	gdPutWord(GD2_VERS, out);
	gdPutWord(im->sx, out);
	gdPutWord(im->sy, out);
	gdPutWord(cs, out);
	gdPutWord(fmt, out);
	gdPutWord(cx, out);
	gdPutWord(cy, out);
}

static void _gdImageGd2 (gdImagePtr im, gdIOCtx * out, int cs, int fmt)
{
	int ncx, ncy, cx, cy;
	int x, y, ylo, yhi, xlo, xhi;
	int chunkLen;
	int chunkNum = 0;
	char *chunkData = NULL;	/* So we can gdFree it with impunity. */
	char *compData = NULL;	/* So we can gdFree it with impunity. */
	uLongf compLen;
	int idxPos = 0;
	int idxSize;
	t_chunk_info *chunkIdx = NULL; /* So we can gdFree it with impunity. */
	int posSave;
	int bytesPerPixel = im->trueColor ? 4 : 1;
	int compMax = 0;

	/* Force fmt to a valid value since we don't return anything. */
	if ((fmt != GD2_FMT_RAW) && (fmt != GD2_FMT_COMPRESSED)) {
		fmt = im->trueColor ? GD2_FMT_TRUECOLOR_COMPRESSED : GD2_FMT_COMPRESSED;
	}
	if (im->trueColor) {
		fmt += 2;
	}
	/* Make sure chunk size is valid. These are arbitrary values; 64 because it seems
	 * a little silly to expect performance improvements on a 64x64 bit scale, and
	 * 4096 because we buffer one chunk, and a 16MB buffer seems a little large - it may be
	 * OK for one user, but for another to read it, they require the buffer.
	 */
	if (cs == 0) {
		cs = GD2_CHUNKSIZE;
	} else if (cs < GD2_CHUNKSIZE_MIN) {
		cs = GD2_CHUNKSIZE_MIN;
	} else if (cs > GD2_CHUNKSIZE_MAX) {
		cs = GD2_CHUNKSIZE_MAX;
	}

	/* Work out number of chunks. */
	ncx = im->sx / cs + 1;
	ncy = im->sy / cs + 1;

	/* Write the standard header. */
	_gd2PutHeader (im, out, cs, fmt, ncx, ncy);

	if (gd2_compressed(fmt)) {
		/* Work out size of buffer for compressed data, If CHUNKSIZE is large,
	 	 * then these will be large!
	 	 */

		/* The zlib notes say output buffer size should be (input size) * 1.01 * 12
		 * - we'll use 1.02 to be paranoid.
		 */
		compMax = (int)(cs * bytesPerPixel * cs * 1.02f) + 12;

		/* Allocate the buffers.  */
		chunkData = safe_emalloc(cs * bytesPerPixel, cs, 0);
		memset(chunkData, 0, cs * bytesPerPixel * cs);
		if (compMax <= 0) {
			goto fail;
		}
		compData = gdCalloc(compMax, 1);

		/* Save the file position of chunk index, and allocate enough space for
		 * each chunk_info block .
		 */
		idxPos = gdTell(out);
		idxSize = ncx * ncy * sizeof(t_chunk_info);
		GD2_DBG(php_gd_error("Index size is %d", idxSize));
		gdSeek(out, idxPos + idxSize);

		chunkIdx = safe_emalloc(idxSize, sizeof(t_chunk_info), 0);
		memset(chunkIdx, 0, idxSize * sizeof(t_chunk_info));
	}

	_gdPutColors (im, out);

	GD2_DBG(php_gd_error("Size: %dx%d", im->sx, im->sy));
	GD2_DBG(php_gd_error("Chunks: %dx%d", ncx, ncy));

	for (cy = 0; (cy < ncy); cy++) {
		for (cx = 0; (cx < ncx); cx++) {
			ylo = cy * cs;
			yhi = ylo + cs;
			if (yhi > im->sy) {
				yhi = im->sy;
			}

			GD2_DBG(php_gd_error("Processing Chunk (%dx%d), y from %d to %d", cx, cy, ylo, yhi));
			chunkLen = 0;
			for (y = ylo; (y < yhi); y++) {
				GD2_DBG(php_gd_error("y=%d: ",y));
				xlo = cx * cs;
				xhi = xlo + cs;
				if (xhi > im->sx) {
					xhi = im->sx;
				}

				if (gd2_compressed(fmt)) {
					for (x = xlo; x < xhi; x++) {
						GD2_DBG(php_gd_error("%d...",x));
						if (im->trueColor) {
							int p = im->tpixels[y][x];
							chunkData[chunkLen++] = gdTrueColorGetAlpha(p);
							chunkData[chunkLen++] = gdTrueColorGetRed(p);
							chunkData[chunkLen++] = gdTrueColorGetGreen(p);
							chunkData[chunkLen++] = gdTrueColorGetBlue(p);
						} else {
							chunkData[chunkLen++] = im->pixels[y][x];
						}
					}
				} else {
					for (x = xlo; x < xhi; x++) {
						GD2_DBG(php_gd_error("%d, ",x));

						if (im->trueColor) {
							gdPutInt(im->tpixels[y][x], out);
						} else {
							gdPutC((unsigned char) im->pixels[y][x], out);
						}
					}
				}
				GD2_DBG(php_gd_error("y=%d done.",y));
			}

			if (gd2_compressed(fmt)) {
				compLen = compMax;
				if (compress((unsigned char *) &compData[0], &compLen, (unsigned char *) &chunkData[0], chunkLen) != Z_OK) {
					php_gd_error("Error from compressing");
				} else {
					chunkIdx[chunkNum].offset = gdTell(out);
					chunkIdx[chunkNum++].size = compLen;
					GD2_DBG(php_gd_error("Chunk %d size %d offset %d", chunkNum, chunkIdx[chunkNum - 1].size, chunkIdx[chunkNum - 1].offset));

					if (gdPutBuf (compData, compLen, out) <= 0) {
						/* Any alternate suggestions for handling this? */
						php_gd_error_ex(E_WARNING, "Error %d on write", errno);
					}
				}
			}
		}
    	}

	if (gd2_compressed(fmt)) {
		/* Save the position, write the index, restore position (paranoia). */
		GD2_DBG(php_gd_error("Seeking %d to write index", idxPos));
		posSave = gdTell(out);
		gdSeek(out, idxPos);
		GD2_DBG(php_gd_error("Writing index"));
		for (x = 0; x < chunkNum; x++) {
			GD2_DBG(php_gd_error("Chunk %d size %d offset %d", x, chunkIdx[x].size, chunkIdx[x].offset));
			gdPutInt(chunkIdx[x].offset, out);
			gdPutInt(chunkIdx[x].size, out);
		}
		gdSeek(out, posSave);
	}
fail:
	GD2_DBG(php_gd_error("Freeing memory"));
	if (chunkData) {
		gdFree(chunkData);
	}
	if (compData) {
		gdFree(compData);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}
	GD2_DBG(php_gd_error("Done"));
}

void gdImageGd2 (gdImagePtr im, FILE * outFile, int cs, int fmt)
{
	gdIOCtx *out = gdNewFileCtx(outFile);

	_gdImageGd2(im, out, cs, fmt);

	out->gd_free(out);
}

void *gdImageGd2Ptr (gdImagePtr im, int cs, int fmt, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);

	_gdImageGd2(im, out, cs, fmt);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);

	return rv;
}
