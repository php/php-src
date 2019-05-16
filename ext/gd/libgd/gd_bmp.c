/*
	gd_bmp.c

	Bitmap format support for libgd

	* Written 2007, Scott MacVicar
	---------------------------------------------------------------------------

	Todo:

	Bitfield encoding

	----------------------------------------------------------------------------
 */
/* $Id$ */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"
#include "bmp.h"

static int compress_row(unsigned char *uncompressed_row, int length);
static int build_rle_packet(unsigned char *row, int packet_type, int length, unsigned char *data);

static int bmp_read_header(gdIOCtxPtr infile, bmp_hdr_t *hdr);
static int bmp_read_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_windows_v3_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_os2_v1_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_os2_v2_info(gdIOCtxPtr infile, bmp_info_t *info);

static int bmp_read_direct(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_1bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_4bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_8bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_rle(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info);

#define BMP_DEBUG(s)

static int gdBMPPutWord(gdIOCtx *out, int w)
{
	/* Byte order is little-endian */
	gdPutC(w & 0xFF, out);
	gdPutC((w >> 8) & 0xFF, out);
	return 0;
}

static int gdBMPPutInt(gdIOCtx *out, int w)
{
	/* Byte order is little-endian */
	gdPutC(w & 0xFF, out);
	gdPutC((w >> 8) & 0xFF, out);
	gdPutC((w >> 16) & 0xFF, out);
	gdPutC((w >> 24) & 0xFF, out);
	return 0;
}

/*
	Function: gdImageBmpPtr
*/
void * gdImageBmpPtr(gdImagePtr im, int *size, int compression)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) return NULL;
	gdImageBmpCtx(im, out, compression);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}

/*
	Function: gdImageBmp
*/
void gdImageBmp(gdImagePtr im, FILE *outFile, int compression)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageBmpCtx(im, out, compression);
	out->gd_free(out);
}

/*
	Function: gdImageBmpCtx
*/
void gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression)
{
	int bitmap_size = 0, info_size, total_size, padding;
	int i, row, xpos, pixel;
	int error = 0;
	unsigned char *uncompressed_row = NULL, *uncompressed_row_start = NULL;
	FILE *tmpfile_for_compression = NULL;
	gdIOCtxPtr out_original = NULL;

	/* No compression if its true colour or we don't support seek */
	if (im->trueColor) {
		compression = 0;
	}

	if (compression == 1 && !out->seek) {
		/* Try to create a temp file where we can seek */
		if ((tmpfile_for_compression = tmpfile()) == NULL) {
			compression = 0;
		} else {
			out_original = out;
			if ((out = (gdIOCtxPtr)gdNewFileCtx(tmpfile_for_compression)) == NULL) {
				out = out_original;
				out_original = NULL;
				compression = 0;
			}
		}
	}

	bitmap_size = ((im->sx * (im->trueColor ? 24 : 8)) / 8) * im->sy;

	/* 40 byte Windows v3 header */
	info_size = BMP_WINDOWS_V3;

	/* data for the palette */
	if (!im->trueColor) {
		info_size += im->colorsTotal * 4;
		if (compression) {
			bitmap_size = 0;
		}
	}

	/* bitmap header + info header + data */
	total_size = 14 + info_size + bitmap_size;

	/* write bmp header info */
	gdPutBuf("BM", 2, out);
	gdBMPPutInt(out, total_size);
	gdBMPPutWord(out, 0);
	gdBMPPutWord(out, 0);
	gdBMPPutInt(out, 14 + info_size);

	/* write Windows v3 headers */
	gdBMPPutInt(out, BMP_WINDOWS_V3); /* header size */
	gdBMPPutInt(out, im->sx); /* width */
	gdBMPPutInt(out, im->sy); /* height */
	gdBMPPutWord(out, 1); /* colour planes */
	gdBMPPutWord(out, (im->trueColor ? 24 : 8)); /* bit count */
	gdBMPPutInt(out, (compression ? BMP_BI_RLE8 : BMP_BI_RGB)); /* compression */
	gdBMPPutInt(out, bitmap_size); /* image size */
	gdBMPPutInt(out, 0); /* H resolution */
	gdBMPPutInt(out, 0); /* V ressolution */
	gdBMPPutInt(out, im->colorsTotal); /* colours used */
	gdBMPPutInt(out, 0); /* important colours */

	/* The line must be divisible by 4, else its padded with NULLs */
	padding = ((int)(im->trueColor ? 3 : 1) * im->sx) % 4;
	if (padding) {
		padding = 4 - padding;
	}

	/* 8-bit colours */
	if (!im->trueColor) {
		for(i = 0; i< im->colorsTotal; ++i) {
			Putchar(gdImageBlue(im, i), out);
			Putchar(gdImageGreen(im, i), out);
			Putchar(gdImageRed(im, i), out);
			Putchar(0, out);
		}

		if (compression) {
			/* Can potentially change this to X + ((X / 128) * 3) */
			uncompressed_row = uncompressed_row_start = (unsigned char *) gdCalloc(gdImageSX(im) * 2, sizeof(char));
			if (!uncompressed_row) {
				/* malloc failed */
				goto cleanup;
			}
		}

		for (row = (im->sy - 1); row >= 0; row--) {
			if (compression) {
				memset (uncompressed_row, 0, gdImageSX(im));
			}

			for (xpos = 0; xpos < im->sx; xpos++) {
				if (compression) {
					*uncompressed_row++ = (unsigned char)gdImageGetPixel(im, xpos, row);
				} else {
					Putchar(gdImageGetPixel(im, xpos, row), out);
				}
			}

			if (!compression) {
				/* Add padding to make sure we have n mod 4 == 0 bytes per row */
				for (xpos = padding; xpos > 0; --xpos) {
					Putchar('\0', out);
				}
			} else {
				int compressed_size = 0;
				uncompressed_row = uncompressed_row_start;
				if ((compressed_size = compress_row(uncompressed_row, gdImageSX(im))) < 0) {
					error = 1;
					break;
				}
				bitmap_size += compressed_size;


				gdPutBuf(uncompressed_row, compressed_size, out);
				Putchar(BMP_RLE_COMMAND, out);
				Putchar(BMP_RLE_ENDOFLINE, out);
				bitmap_size += 2;
			}
		}

		if (compression && uncompressed_row) {
			gdFree(uncompressed_row);
			if (error != 0) {
				goto cleanup;
			}
			/* Update filesize based on new values and set compression flag */
			Putchar(BMP_RLE_COMMAND, out);
			Putchar(BMP_RLE_ENDOFBITMAP, out);
			bitmap_size += 2;

			/* Write new total bitmap size */
			gdSeek(out, 2);
			gdBMPPutInt(out, total_size + bitmap_size);

			/* Total length of image data */
			gdSeek(out, 34);
			gdBMPPutInt(out, bitmap_size);
		}

	} else {
		for (row = (im->sy - 1); row >= 0; row--) {
			for (xpos = 0; xpos < im->sx; xpos++) {
				pixel = gdImageGetPixel(im, xpos, row);

				Putchar(gdTrueColorGetBlue(pixel), out);
				Putchar(gdTrueColorGetGreen(pixel), out);
				Putchar(gdTrueColorGetRed(pixel), out);
			}

			/* Add padding to make sure we have n mod 4 == 0 bytes per row */
			for (xpos = padding; xpos > 0; --xpos) {
				Putchar('\0', out);
			}
		}
	}


	/* If we needed a tmpfile for compression copy it over to out_original */
	if (tmpfile_for_compression) {
		unsigned char* copy_buffer = NULL;
		int buffer_size = 0;

		gdSeek(out, 0);
		copy_buffer = (unsigned char *) gdMalloc(1024 * sizeof(unsigned char));
		if (copy_buffer == NULL) {
			goto cleanup;
		}

		while ((buffer_size = gdGetBuf(copy_buffer, 1024, out)) != EOF) {
			if (buffer_size == 0) {
				break;
			}
			gdPutBuf(copy_buffer , buffer_size, out_original);
		}
		gdFree(copy_buffer);

		/* Replace the temp with the original which now has data */
		out->gd_free(out);
		out = out_original;
		out_original = NULL;
	}

cleanup:
	if (tmpfile_for_compression) {
#ifdef _WIN32
		_rmtmp();
#else
		fclose(tmpfile_for_compression);
#endif
		tmpfile_for_compression = NULL;
	}

	if (out_original) {
		out_original->gd_free(out_original);
	}
	return;
}

static int compress_row(unsigned char *row, int length)
{
	int rle_type = 0;
	int compressed_length = 0;
	int pixel = 0, compressed_run = 0, rle_compression = 0;
	unsigned char *uncompressed_row = NULL, *uncompressed_rowp = NULL, *uncompressed_start = NULL;

	uncompressed_row = (unsigned char *) gdMalloc(length);
	if (!uncompressed_row) {
		return -1;
	}

	memcpy(uncompressed_row, row, length);
	uncompressed_start = uncompressed_rowp = uncompressed_row;

	for (pixel = 0; pixel < length; pixel++) {
		if (compressed_run == 0) {
			uncompressed_row = uncompressed_rowp;
			compressed_run++;
			uncompressed_rowp++;
			rle_type = BMP_RLE_TYPE_RAW;
			continue;
		}

		if (compressed_run == 1) {
			/* Compare next byte */
			if (memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) == 0) {
				rle_type = BMP_RLE_TYPE_RLE;
			}
		}

		if (rle_type == BMP_RLE_TYPE_RLE) {
			if (compressed_run >= 128 || memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) != 0) {
				/* more than what we can store in a single run or run is over due to non match, force write */
				rle_compression = build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
				row += rle_compression;
				compressed_length += rle_compression;
				compressed_run = 0;
				pixel--;
			} else {
				compressed_run++;
				uncompressed_rowp++;
			}
		} else {
			if (compressed_run >= 128 || memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) == 0) {
				/* more than what we can store in a single run or run is over due to match, force write */
				rle_compression = build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
				row += rle_compression;
				compressed_length += rle_compression;
				compressed_run = 0;
				pixel--;
			} else {
				/* add this pixel to the row */
				compressed_run++;
				uncompressed_rowp++;
			}

		}
	}

	if (compressed_run) {
		if (rle_type == BMP_RLE_TYPE_RLE) {
			compressed_length += build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
		}
	}

	gdFree(uncompressed_start);

	return compressed_length;
}

static int build_rle_packet(unsigned char *row, int packet_type, int length, unsigned char *data)
{
	int compressed_size = 0;
	if (length < 1 || length > 128) {
		return 0;
	}

	/* Bitmap specific cases is that we can't have uncompressed rows of length 1 or 2 */
	if (packet_type == BMP_RLE_TYPE_RAW && length < 3) {
		int i = 0;
		for (i = 0; i < length; i++) {
			compressed_size += 2;
			memset(row, 1, 1);
			row++;

			memcpy(row, data++, 1);
			row++;
		}
	} else if (packet_type == BMP_RLE_TYPE_RLE) {
		compressed_size = 2;
		memset(row, length, 1);
		row++;

		memcpy(row, data, 1);
		row++;
	} else {
		compressed_size = 2 + length;
		memset(row, BMP_RLE_COMMAND, 1);
		row++;

		memset(row, length, 1);
		row++;

		memcpy(row, data, length);
		row += length;

		/* Must be an even number for an uncompressed run */
		if (length % 2) {
			memset(row, 0, 1);
			row++;
			compressed_size++;
		}
	}
	return compressed_size;
}

/*
	Function: gdImageCreateFromBmp
*/
gdImagePtr gdImageCreateFromBmp(FILE * inFile)
{
	gdImagePtr im = 0;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (in == NULL) return NULL;
	im = gdImageCreateFromBmpCtx(in);
	in->gd_free(in);
	return im;
}

/*
	Function: gdImageCreateFromBmpPtr
*/
gdImagePtr gdImageCreateFromBmpPtr(int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (in == NULL) return NULL;
	im = gdImageCreateFromBmpCtx(in);
	in->gd_free(in);
	return im;
}

/*
	Function: gdImageCreateFromBmpCtx
*/
gdImagePtr gdImageCreateFromBmpCtx(gdIOCtxPtr infile)
{
	bmp_hdr_t *hdr;
	bmp_info_t *info;
	gdImagePtr im = NULL;
	int error = 0;

	if (!(hdr= (bmp_hdr_t *)gdCalloc(1, sizeof(bmp_hdr_t)))) {
		return NULL;
	}

	if (bmp_read_header(infile, hdr)) {
		gdFree(hdr);
		return NULL;
	}

	if (hdr->magic != 0x4d42) {
		gdFree(hdr);
		return NULL;
	}

	if (!(info = (bmp_info_t *)gdCalloc(1, sizeof(bmp_info_t)))) {
		gdFree(hdr);
		return NULL;
	}

	if (bmp_read_info(infile, info)) {
		gdFree(hdr);
		gdFree(info);
		return NULL;
	}

	BMP_DEBUG(printf("Numcolours: %d\n", info->numcolors));
	BMP_DEBUG(printf("Width: %d\n", info->width));
	BMP_DEBUG(printf("Height: %d\n", info->height));
	BMP_DEBUG(printf("Planes: %d\n", info->numplanes));
	BMP_DEBUG(printf("Depth: %d\n", info->depth));
	BMP_DEBUG(printf("Offset: %d\n", hdr->off));

	if (info->depth >= 16) {
		im = gdImageCreateTrueColor(info->width, info->height);
	} else {
		im = gdImageCreate(info->width, info->height);
	}

	if (!im) {
		gdFree(hdr);
		gdFree(info);
		return NULL;
	}

	switch (info->depth) {
	case 1:
		BMP_DEBUG(printf("1-bit image\n"));
		error = bmp_read_1bit(im, infile, info, hdr);
		break;
	case 4:
		BMP_DEBUG(printf("4-bit image\n"));
		error = bmp_read_4bit(im, infile, info, hdr);
		break;
	case 8:
		BMP_DEBUG(printf("8-bit image\n"));
		error = bmp_read_8bit(im, infile, info, hdr);
		break;
	case 16:
	case 24:
	case 32:
		BMP_DEBUG(printf("Direct BMP image\n"));
		error = bmp_read_direct(im, infile, info, hdr);
		break;
	default:
		BMP_DEBUG(printf("Unknown bit count\n"));
		error = 1;
	}

	gdFree(hdr);
	gdFree(info);

	if (error) {
		gdImageDestroy(im);
		return NULL;
	}

	return im;
}

static int bmp_read_header(gdIOCtx *infile, bmp_hdr_t *hdr)
{
	if(
	    !gdGetWordLSB(&hdr->magic, infile) ||
	    !gdGetIntLSB(&hdr->size, infile) ||
	    !gdGetWordLSB(&hdr->reserved1, infile) ||
	    !gdGetWordLSB(&hdr->reserved2 , infile) ||
	    !gdGetIntLSB(&hdr->off , infile)
	) {
		return 1;
	}
	return 0;
}

static int bmp_read_info(gdIOCtx *infile, bmp_info_t *info)
{
	/* read BMP length so we can work out the version */
	if (!gdGetIntLSB(&info->len, infile)) {
		return 1;
	}

	switch (info->len) {
		/* For now treat Windows v4 + v5 as v3 */
	case BMP_WINDOWS_V3:
	case BMP_WINDOWS_V4:
	case BMP_WINDOWS_V5:
		BMP_DEBUG(printf("Reading Windows Header\n"));
		if (bmp_read_windows_v3_info(infile, info)) {
			return 1;
		}
		break;
	case BMP_OS2_V1:
		if (bmp_read_os2_v1_info(infile, info)) {
			return 1;
		}
		break;
	case BMP_OS2_V2:
		if (bmp_read_os2_v2_info(infile, info)) {
			return 1;
		}
		break;
	default:
		BMP_DEBUG(printf("Unhandled bitmap\n"));
		return 1;
	}
	return 0;
}

static int bmp_read_windows_v3_info(gdIOCtxPtr infile, bmp_info_t *info)
{
	if (
	    !gdGetIntLSB(&info->width, infile) ||
	    !gdGetIntLSB(&info->height, infile) ||
	    !gdGetWordLSB(&info->numplanes, infile) ||
	    !gdGetWordLSB(&info->depth, infile) ||
	    !gdGetIntLSB(&info->enctype, infile) ||
	    !gdGetIntLSB(&info->size, infile) ||
	    !gdGetIntLSB(&info->hres, infile) ||
	    !gdGetIntLSB(&info->vres, infile) ||
	    !gdGetIntLSB(&info->numcolors, infile) ||
	    !gdGetIntLSB(&info->mincolors, infile)
	) {
		return 1;
	}

	if (info->height < 0) {
		info->topdown = 1;
		info->height = -info->height;
	} else {
		info->topdown = 0;
	}

	info->type = BMP_PALETTE_4;

	if (info->width <= 0 || info->height <= 0 || info->numplanes <= 0 ||
	        info->depth <= 0  || info->numcolors < 0 || info->mincolors < 0) {
		return 1;
	}

	return 0;
}

static int bmp_read_os2_v1_info(gdIOCtxPtr infile, bmp_info_t *info)
{
	if (
	    !gdGetWordLSB((signed short int *)&info->width, infile) ||
	    !gdGetWordLSB((signed short int *)&info->height, infile) ||
	    !gdGetWordLSB(&info->numplanes, infile) ||
	    !gdGetWordLSB(&info->depth, infile)
	) {
		return 1;
	}

	/* OS2 v1 doesn't support topdown */
	info->topdown = 0;

	info->numcolors = 1 << info->depth;
	info->type = BMP_PALETTE_3;

	if (info->width <= 0 || info->height <= 0 || info->numplanes <= 0 ||
	        info->depth <= 0 || info->numcolors < 0) {
		return 1;
	}

	return 0;
}

static int bmp_read_os2_v2_info(gdIOCtxPtr infile, bmp_info_t *info)
{
	char useless_bytes[24];
	if (
	    !gdGetIntLSB(&info->width, infile) ||
	    !gdGetIntLSB(&info->height, infile) ||
	    !gdGetWordLSB(&info->numplanes, infile) ||
	    !gdGetWordLSB(&info->depth, infile) ||
	    !gdGetIntLSB(&info->enctype, infile) ||
	    !gdGetIntLSB(&info->size, infile) ||
	    !gdGetIntLSB(&info->hres, infile) ||
	    !gdGetIntLSB(&info->vres, infile) ||
	    !gdGetIntLSB(&info->numcolors, infile) ||
	    !gdGetIntLSB(&info->mincolors, infile)
	) {
		return 1;
	}

	/* Lets seek the next 24 pointless bytes, we don't care too much about it */
	if (!gdGetBuf(useless_bytes, 24, infile)) {
		return 1;
	}

	if (info->height < 0) {
		info->topdown = 1;
		info->height = -info->height;
	} else {
		info->topdown = 0;
	}

	info->type = BMP_PALETTE_4;

	if (info->width <= 0 || info->height <= 0 || info->numplanes <= 0 ||
	        info->depth <= 0  || info->numcolors < 0 || info->mincolors < 0) {
		return 1;
	}


	return 0;
}

static int bmp_read_direct(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0;
	int padding = 0, alpha = 0, red = 0, green = 0, blue = 0;
	signed short int data = 0;

	switch(info->enctype) {
	case BMP_BI_RGB:
		/* no-op */
		break;

	case BMP_BI_BITFIELDS:
		if (info->depth == 24) {
			BMP_DEBUG(printf("Bitfield compression isn't supported for 24-bit\n"));
			return 1;
		}
		BMP_DEBUG(printf("Currently no bitfield support\n"));
		return 1;
		break;

	case BMP_BI_RLE8:
		if (info->depth != 8) {
			BMP_DEBUG(printf("RLE is only valid for 8-bit images\n"));
			return 1;
		}
		break;
	case BMP_BI_RLE4:
		if (info->depth != 4) {
			BMP_DEBUG(printf("RLE is only valid for 4-bit images\n"));
			return 1;
		}
		break;
	case BMP_BI_JPEG:
	case BMP_BI_PNG:
	default:
		BMP_DEBUG(printf("Unsupported BMP compression format\n"));
		return 1;
	}

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	/* The line must be divisible by 4, else its padded with NULLs */
	padding = ((int)(info->depth / 8) * info->width) % 4;
	if (padding) {
		padding = 4 - padding;
	}


	for (ypos = 0; ypos < info->height; ++ypos) {
		if (info->topdown) {
			row = ypos;
		} else {
			row = info->height - ypos - 1;
		}

		for (xpos = 0; xpos < info->width; xpos++) {
			if (info->depth == 16) {
				if (!gdGetWordLSB(&data, infile)) {
					return 1;
				}
				BMP_DEBUG(printf("Data: %X\n", data));
				red = ((data & 0x7C00) >> 10) << 3;
				green = ((data & 0x3E0) >> 5) << 3;
				blue = (data & 0x1F) << 3;
				BMP_DEBUG(printf("R: %d, G: %d, B: %d\n", red, green, blue));
			} else if (info->depth == 24) {
				if (!gdGetByte(&blue, infile) || !gdGetByte(&green, infile) || !gdGetByte(&red, infile)) {
					return 1;
				}
			} else {
				if (!gdGetByte(&blue, infile) || !gdGetByte(&green, infile) || !gdGetByte(&red, infile) || !gdGetByte(&alpha, infile)) {
					return 1;
				}
			}
			/*alpha = gdAlphaMax - (alpha >> 1);*/
			gdImageSetPixel(im, xpos, row, gdTrueColor(red, green, blue));
		}
		for (xpos = padding; xpos > 0; --xpos) {
			if (!gdGetByte(&red, infile)) {
				return 1;
			}
		}
	}

	return 0;
}

static int bmp_read_palette(gdImagePtr im, gdIOCtxPtr infile, int count, int read_four)
{
	int i;
	int r, g, b, z;

	for (i = 0; i < count; i++) {
		if (
		    !gdGetByte(&b, infile) ||
		    !gdGetByte(&g, infile) ||
		    !gdGetByte(&r, infile) ||
		    (read_four && !gdGetByte(&z, infile))
		) {
			return 1;
		}
		im->red[i] = r;
		im->green[i] = g;
		im->blue[i] = b;
		im->open[i] = 1;
	}
	return 0;
}

static int bmp_read_1bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0, current_byte = 0, bit = 0;

	if (info->enctype != BMP_BI_RGB) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 2;
	} else if (info->numcolors < 0 || info->numcolors > 2) {
		return 1;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	/* The line must be aligned on a 32 bits word, else it is padded with zeros */
	padding = (info->width + 7) / 8 % 4;
	if (padding) {
		padding = 4 - padding;
	}

	for (ypos = 0; ypos < info->height; ++ypos) {
		if (info->topdown) {
			row = ypos;
		} else {
			row = info->height - ypos - 1;
		}

		for (xpos = 0; xpos < info->width; xpos += 8) {
			/* Bitmaps are always aligned in bytes so we'll never overflow */
			if (!gdGetByte(&current_byte, infile)) {
				return 1;
			}

			for (bit = 0; bit < 8; bit++) {
				index = ((current_byte & (0x80 >> bit)) != 0 ? 0x01 : 0x00);
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos + bit, row, index);
				/* No need to read anything extra */
				if ((xpos + bit) >= info->width) {
					break;
				}
			}
		}

		for (xpos = padding; xpos > 0; --xpos) {
			if (!gdGetByte(&index, infile)) {
				return 1;
			}
		}
	}
	return 0;
}

static int bmp_read_4bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0, current_byte = 0;

	if (info->enctype != BMP_BI_RGB && info->enctype != BMP_BI_RLE4) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 16;
	} else if (info->numcolors < 0 || info->numcolors > 16) {
		return 1;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	/* The line must be divisible by 4, else its padded with NULLs */
	padding = ((int)ceil(0.5 * info->width)) % 4;
	if (padding) {
		padding = 4 - padding;
	}

	switch (info->enctype) {
	case BMP_BI_RGB:
		for (ypos = 0; ypos < info->height; ++ypos) {
			if (info->topdown) {
				row = ypos;
			} else {
				row = info->height - ypos - 1;
			}

			for (xpos = 0; xpos < info->width; xpos += 2) {
				if (!gdGetByte(&current_byte, infile)) {
					return 1;
				}

				index = (current_byte >> 4) & 0x0f;
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos, row, index);

				/* This condition may get called often, potential optimsations */
				if (xpos >= info->width) {
					break;
				}

				index = current_byte & 0x0f;
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos + 1, row, index);
			}

			for (xpos = padding; xpos > 0; --xpos) {
				if (!gdGetByte(&index, infile)) {
					return 1;
				}
			}
		}
		break;

	case BMP_BI_RLE4:
		if (bmp_read_rle(im, infile, info)) {
			return 1;
		}
		break;

	default:
		return 1;
	}
	return 0;
}

static int bmp_read_8bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0;

	if (info->enctype != BMP_BI_RGB && info->enctype != BMP_BI_RLE8) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 256;
	} else if (info->numcolors < 0 || info->numcolors > 256) {
		return 1;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	/* The line must be divisible by 4, else its padded with NULLs */
	padding = (1 * info->width) % 4;
	if (padding) {
		padding = 4 - padding;
	}

	switch (info->enctype) {
	case BMP_BI_RGB:
		for (ypos = 0; ypos < info->height; ++ypos) {
			if (info->topdown) {
				row = ypos;
			} else {
				row = info->height - ypos - 1;
			}

			for (xpos = 0; xpos < info->width; ++xpos) {
				if (!gdGetByte(&index, infile)) {
					return 1;
				}

				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos, row, index);
			}
			/* Could create a new variable, but it isn't really worth it */
			for (xpos = padding; xpos > 0; --xpos) {
				if (!gdGetByte(&index, infile)) {
					return 1;
				}
			}
		}
		break;

	case BMP_BI_RLE8:
		if (bmp_read_rle(im, infile, info)) {
			return 1;
		}
		break;

	default:
		return 1;
	}
	return 0;
}

static int bmp_read_rle(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int rle_length = 0, rle_data = 0;
	int padding = 0;
	int i = 0, j = 0;
	int pixels_per_byte = 8 / info->depth;

	for (ypos = 0; ypos < info->height && xpos <= info->width;) {
		if (!gdGetByte(&rle_length, infile) || !gdGetByte(&rle_data, infile)) {
			return 1;
		}
		row = info->height - ypos - 1;

		if (rle_length != BMP_RLE_COMMAND) {
			if (im->open[rle_data]) {
				im->open[rle_data] = 0;
			}

			for (i = 0; (i < rle_length) && (xpos < info->width);) {
				for (j = 1; (j <= pixels_per_byte) && (xpos < info->width) && (i < rle_length); j++, xpos++, i++) {
					index = (rle_data & (((1 << info->depth) - 1) << (8 - (j * info->depth)))) >> (8 - (j * info->depth));
					if (im->open[index]) {
						im->open[index] = 0;
					}
					gdImageSetPixel(im, xpos, row, index);
				}
			}
		} else if (rle_length == BMP_RLE_COMMAND && rle_data > 2) {
			/* Uncompressed RLE needs to be even */
			padding = 0;
			for (i = 0; (i < rle_data) && (xpos < info->width); i += pixels_per_byte) {
				int max_pixels = pixels_per_byte;

				if (!gdGetByte(&index, infile)) {
					return 1;
				}
				padding++;

				if (rle_data - i < max_pixels) {
					max_pixels = rle_data - i;
				}

				for (j = 1; (j <= max_pixels)  && (xpos < info->width); j++, xpos++) {
					int temp = (index >> (8 - (j * info->depth))) & ((1 << info->depth) - 1);
					if (im->open[temp]) {
						im->open[temp] = 0;
					}
					gdImageSetPixel(im, xpos, row, temp);
				}
			}

			/* Make sure the bytes read are even */
			if (padding % 2 && !gdGetByte(&index, infile)) {
				return 1;
			}
		} else if (rle_length == BMP_RLE_COMMAND && rle_data == BMP_RLE_ENDOFLINE) {
			/* Next Line */
			xpos = 0;
			ypos++;
		} else if (rle_length == BMP_RLE_COMMAND && rle_data == BMP_RLE_DELTA) {
			/* Delta Record, used for bmp files that contain other data*/
			if (!gdGetByte(&rle_length, infile) || !gdGetByte(&rle_data, infile)) {
				return 1;
			}
			xpos += rle_length;
			ypos += rle_data;
		} else if (rle_length == BMP_RLE_COMMAND && rle_data == BMP_RLE_ENDOFBITMAP) {
			/* End of bitmap */
			break;
		}
	}
	return 0;
}
