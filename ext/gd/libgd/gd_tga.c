/**
 * File: TGA Input
 *
 * Read TGA images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gd.h"
#include "gd_errors.h"
#include "gd_io.h"
#include "gd_tga.h"
#include "gdhelpers.h"

static int tga_is_rle(uint8_t imagetype);
static int tga_pixel_size(uint8_t bits);
static int tga_read_color_map(gdIOCtx *ctx, oTga *tga);
static int tga_read_pixel(gdIOCtx *ctx, oTga *tga, int *pixel);
static void tga_apply_attribute_type(gdIOCtx *ctx, oTga *tga, int pixel_count);
static int tga_decode_color(const unsigned char *buf, int bits, int alpha_bits,
							int *has_alpha);
static int tga_decode_16(unsigned int value, int alpha_bits, int *has_alpha);
static int tga_scale_5_to_8(int c);
static int tga_alpha_8_to_gd(int a);
static void tga_strip_alpha(oTga *tga, int pixel_count);

/*
	Function: gdImageCreateFromTga

	Creates a gdImage from a TGA file

	Parameters:

		infile - Pointer to TGA binary file
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTga(FILE *fp) {
	gdImagePtr image;
	gdIOCtx *in = gdNewFileCtx(fp);
	if (in == NULL)
		return NULL;
	image = gdImageCreateFromTgaCtx(in);
	in->gd_free(in);
	return image;
}

/*
	Function: gdImageCreateFromTgaPtr
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaPtr(int size, void *data) {
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (in == NULL)
		return NULL;
	im = gdImageCreateFromTgaCtx(in);
	in->gd_free(in);
	return im;
}

/*
	Function: gdImageCreateFromTgaCtx

	Creates a gdImage from a gdIOCtx referencing a TGA binary file.

	Parameters:
		ctx - Pointer to a gdIOCtx structure
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaCtx(gdIOCtx *ctx) {
	int bitmap_caret = 0;
	oTga *tga = NULL;
	volatile gdImagePtr image = NULL;
	int x = 0;
	int y = 0;

	if (ctx == NULL) {
		return NULL;
	}

	tga = (oTga *)gdMalloc(sizeof(oTga));
	if (!tga) {
		return NULL;
	}

	tga->bitmap = NULL;
	tga->colormap = NULL;
	tga->ident = NULL;
	tga->has_alpha = 0;

	if (read_header_tga(ctx, tga) < 0) {
		free_tga(tga);
		return NULL;
	}

	if (read_image_tga(ctx, tga) < 0) {
		free_tga(tga);
		return NULL;
	}

	image = gdImageCreateTrueColor((int)tga->width, (int)tga->height);

	if (image == 0) {
		free_tga(tga);
		return NULL;
	}

	/*!	\brief Populate GD image object
	 *  Copy the pixel data from our tga bitmap buffer into the GD image
	 *  Disable blending and save the alpha channel per default
	 */
	if (tga->has_alpha) {
		gdImageAlphaBlending(image, 0);
		gdImageSaveAlpha(image, 1);
	}

	for (y = 0; y < tga->height; y++) {
		register int *tpix = image->tpixels[y];
		for (x = 0; x < tga->width; x++, tpix++) {
			*tpix = tga->bitmap[bitmap_caret++];
		}
	}

	if (tga->flipv && tga->fliph) {
		gdImageFlipBoth(image);
	} else if (tga->flipv) {
		gdImageFlipVertical(image);
	} else if (tga->fliph) {
		gdImageFlipHorizontal(image);
	}

	free_tga(tga);

	return image;
}

/*!	\brief Reads a TGA header.
 *	Reads the header block from a binary TGA file populating the referenced TGA
 *structure. \param ctx Pointer to TGA binary file \param tga Pointer to TGA
 *structure \return int 1 on sucess, -1 on failure
 */
int read_header_tga(gdIOCtx *ctx, oTga *tga) {

	unsigned char header[18];

	if (gdGetBuf(header, sizeof(header), ctx) < 18) {
		gd_error("fail to read header");
		return -1;
	}

	tga->identsize = header[0];
	tga->colormaptype = header[1];
	tga->imagetype = header[2];
	tga->colormapstart = header[3] + (header[4] << 8);
	tga->colormaplength = header[5] + (header[6] << 8);
	tga->colormapbits = header[7];
	tga->xstart = header[8] + (header[9] << 8);
	tga->ystart = header[10] + (header[11] << 8);
	tga->width = header[12] + (header[13] << 8);
	tga->height = header[14] + (header[15] << 8);
	tga->bits = header[16];
	tga->alphabits = header[17] & 0x0f;
	tga->fliph = (header[17] & 0x10) ? 1 : 0;
	tga->flipv = (header[17] & 0x20) ? 0 : 1;
	tga->has_alpha = 0;

#ifdef DEBUG
	printf("format bps: %i\n", tga->bits);
	printf("flip h/v: %i / %i\n", tga->fliph, tga->flipv);
	printf("alpha: %i\n", tga->alphabits);
	printf("wxh: %i %i\n", tga->width, tga->height);
#endif

	if (tga->width <= 0 || tga->height <= 0) {
		gd_error("gd-tga: invalid image dimensions\n");
		return -1;
	}

	if (tga->colormaptype > 1) {
		gd_error_ex(GD_WARNING, "gd-tga: unsupported color map type %u\n",
					tga->colormaptype);
		return -1;
	}

	switch (tga->imagetype) {
	case TGA_TYPE_INDEXED:
	case TGA_TYPE_INDEXED_RLE:
		if (tga->colormaptype != 1 || tga->bits != TGA_BPP_8 ||
			!(tga->colormapbits == 15 || tga->colormapbits == 16 ||
			  tga->colormapbits == 24 || tga->colormapbits == 32)) {
			gd_error_ex(GD_WARNING, "gd-tga: unsupported color mapped image\n");
			return -1;
		}
		break;

	case TGA_TYPE_RGB:
	case TGA_TYPE_RGB_RLE:
		if (!(tga->bits == TGA_BPP_16 || tga->bits == TGA_BPP_24 ||
			  (tga->bits == TGA_BPP_32 && tga->alphabits == 8))) {
			gd_error_ex(GD_WARNING, "gd-tga: unsupported truecolor depth %u\n",
						tga->bits);
			return -1;
		}
		break;

	case TGA_TYPE_GREYSCALE:
	case TGA_TYPE_GREYSCALE_RLE:
		if (tga->bits != TGA_BPP_8) {
			gd_error_ex(GD_WARNING, "gd-tga: unsupported grayscale depth %u\n",
						tga->bits);
			return -1;
		}
		break;

	default:
		gd_error_ex(GD_WARNING, "gd-tga: unsupported image type %u\n",
					tga->imagetype);
		return -1;
	}

	tga->ident = NULL;

	if (tga->identsize > 0) {
		tga->ident = (char *)gdMalloc(tga->identsize * sizeof(char));
		if (tga->ident == NULL) {
			return -1;
		}

		if (gdGetBuf(tga->ident, tga->identsize, ctx) != tga->identsize) {
			gd_error("fail to read header ident");
			return -1;
		}
	}

	return 1;
}

/*!	\brief Reads a TGA image data into buffer.
 *	Reads the image data block from a binary TGA file populating the referenced
 *TGA structure. \param ctx Pointer to TGA binary file \param tga Pointer to TGA
 *structure \return int 0 on sucess, -1 on failure
 */
int read_image_tga(gdIOCtx *ctx, oTga *tga) {
	int pixel_count;
	int bitmap_caret = 0;

	if (overflow2(tga->width, tga->height)) {
		return -1;
	}

	pixel_count = tga->width * tga->height;
	if (overflow2(pixel_count, sizeof(int))) {
		return -1;
	}

	if (tga_read_color_map(ctx, tga) < 0) {
		return -1;
	}

	/*!	\brief Allocate memory for image block
	 *  Allocate a chunk of memory for the image block to be passed into.
	 */
	tga->bitmap = (int *)gdMalloc(pixel_count * sizeof(int));
	if (tga->bitmap == NULL)
		return -1;

	if (tga_is_rle(tga->imagetype)) {
		while (bitmap_caret < pixel_count) {
			int packet = gdGetC(ctx);
			int encoded_pixels;
			int pixel;
			int i;

			if (packet == EOF) {
				gd_error("gd-tga: premature end of image data\n");
				return -1;
			}

			encoded_pixels = (packet & ~TGA_RLE_FLAG) + 1;
			if (encoded_pixels > pixel_count - bitmap_caret) {
				return -1;
			}

			if ((packet & TGA_RLE_FLAG) == TGA_RLE_FLAG) {
				if (tga_read_pixel(ctx, tga, &pixel) < 0) {
					return -1;
				}
				for (i = 0; i < encoded_pixels; i++) {
					tga->bitmap[bitmap_caret++] = pixel;
				}
			} else {
				for (i = 0; i < encoded_pixels; i++) {
					if (tga_read_pixel(ctx, tga, &pixel) < 0) {
						return -1;
					}
					tga->bitmap[bitmap_caret++] = pixel;
				}
			}
		}
	} else {
		while (bitmap_caret < pixel_count) {
			if (tga_read_pixel(ctx, tga, &tga->bitmap[bitmap_caret]) < 0) {
				return -1;
			}
			bitmap_caret++;
		}
	}

	tga_apply_attribute_type(ctx, tga, pixel_count);

	return 1;
}

static int tga_is_rle(uint8_t imagetype) {
	return imagetype == TGA_TYPE_INDEXED_RLE || imagetype == TGA_TYPE_RGB_RLE ||
		   imagetype == TGA_TYPE_GREYSCALE_RLE;
}

static int tga_pixel_size(uint8_t bits) { return (bits + 7) / 8; }

static int tga_read_color_map(gdIOCtx *ctx, oTga *tga) {
	int i;
	int has_alpha = 0;
	int entry_size;

	if (tga->colormaptype == 0) {
		return 1;
	}

	if (tga->colormaplength <= 0) {
		return -1;
	}

	if (overflow2(tga->colormaplength, sizeof(int))) {
		return -1;
	}

	entry_size = tga_pixel_size(tga->colormapbits);
	tga->colormap = (int *)gdMalloc(tga->colormaplength * sizeof(int));
	if (tga->colormap == NULL) {
		return -1;
	}

	for (i = 0; i < tga->colormaplength; i++) {
		unsigned char buf[4] = {0, 0, 0, 0};

		if (gdGetBuf(buf, entry_size, ctx) != entry_size) {
			gd_error("gd-tga: premature end of color map data\n");
			return -1;
		}

		tga->colormap[i] =
			tga_decode_color(buf, tga->colormapbits,
							 tga->colormapbits == 32 ? 8 : 0, &has_alpha);
	}

	if (has_alpha) {
		tga->has_alpha = 1;
	}

	return 1;
}

static int tga_read_pixel(gdIOCtx *ctx, oTga *tga, int *pixel) {
	unsigned char buf[4] = {0, 0, 0, 0};
	int size = tga_pixel_size(tga->bits);
	int has_alpha = 0;

	if (gdGetBuf(buf, size, ctx) != size) {
		gd_error("gd-tga: premature end of image data\n");
		return -1;
	}

	switch (tga->imagetype) {
	case TGA_TYPE_INDEXED:
	case TGA_TYPE_INDEXED_RLE: {
		int index = buf[0] - tga->colormapstart;

		if (index < 0 || index >= tga->colormaplength ||
			tga->colormap == NULL) {
			return -1;
		}
		*pixel = tga->colormap[index];
		break;
	}

	case TGA_TYPE_RGB:
	case TGA_TYPE_RGB_RLE:
		*pixel = tga_decode_color(buf, tga->bits, tga->alphabits, &has_alpha);
		if (has_alpha) {
			tga->has_alpha = 1;
		}
		break;

	case TGA_TYPE_GREYSCALE:
	case TGA_TYPE_GREYSCALE_RLE:
		*pixel = gdTrueColor(buf[0], buf[0], buf[0]);
		break;

	default:
		return -1;
	}

	return 1;
}

static void tga_apply_attribute_type(gdIOCtx *ctx, oTga *tga, int pixel_count) {
	static const unsigned char signature[18] = {'T', 'R', 'U', 'E', 'V', 'I',
												'S', 'I', 'O', 'N', '-', 'X',
												'F', 'I', 'L', 'E', '.', '\0'};
	unsigned char footer_ring[26];
	unsigned char footer[26];
	size_t trailing_size = 0;
	size_t i;
	int c;
	long start;
	uint32_t extension_offset;
	uint32_t attr_type_offset;
	uint32_t end_offset;
	int attr_type;

	if (!tga->has_alpha) {
		return;
	}

	if (ctx->tell == NULL || ctx->seek == NULL) {
		return;
	}

	start = gdTell(ctx);
	if (start < 0 || start > INT_MAX) {
		return;
	}

	while ((c = gdGetC(ctx)) != EOF) {
		footer_ring[trailing_size % sizeof(footer_ring)] = (unsigned char)c;
		if (trailing_size == SIZE_MAX) {
			return;
		}
		trailing_size++;
	}

	if (trailing_size < sizeof(footer)) {
		return;
	}

	for (i = 0; i < sizeof(footer); i++) {
		footer[i] = footer_ring[(trailing_size + i) % sizeof(footer_ring)];
	}
	if (memcmp(footer + 8, signature, sizeof(signature)) != 0) {
		return;
	}

	extension_offset = (uint32_t)footer[0] | ((uint32_t)footer[1] << 8) |
					   ((uint32_t)footer[2] << 16) |
					   ((uint32_t)footer[3] << 24);
	if (extension_offset == 0 || extension_offset < (uint32_t)start ||
		extension_offset > (uint32_t)INT_MAX - 494) {
		return;
	}

	attr_type_offset = extension_offset + 494;
	if (trailing_size > (size_t)INT_MAX - (size_t)start ||
		attr_type_offset >= (uint32_t)start + (uint32_t)trailing_size -
										 sizeof(footer)) {
		return;
	}
	end_offset = (uint32_t)start + (uint32_t)trailing_size;

	if (!gdSeek(ctx, (int)attr_type_offset)) {
		return;
	}
	attr_type = gdGetC(ctx);
	gdSeek(ctx, (int)end_offset);
	if (attr_type == EOF) {
		return;
	}
	if (attr_type != 3 && attr_type != 4) {
		tga_strip_alpha(tga, pixel_count);
	}
}

static int tga_decode_color(const unsigned char *buf, int bits, int alpha_bits,
							int *has_alpha) {
	switch (bits) {
	case 15:
	case 16:
		return tga_decode_16((unsigned int)buf[0] | ((unsigned int)buf[1] << 8),
							 alpha_bits, has_alpha);

	case 24:
		return gdTrueColor(buf[2], buf[1], buf[0]);

	case 32:
		if (has_alpha) {
			*has_alpha = 1;
		}
		return gdTrueColorAlpha(buf[2], buf[1], buf[0],
								tga_alpha_8_to_gd(buf[3]));
	}

	return 0;
}

static int tga_decode_16(unsigned int value, int alpha_bits, int *has_alpha) {
	int b = tga_scale_5_to_8(value & 0x1f);
	int g = tga_scale_5_to_8((value >> 5) & 0x1f);
	int r = tga_scale_5_to_8((value >> 10) & 0x1f);

	if (alpha_bits > 0) {
		int a = (value & 0x8000) ? gdAlphaOpaque : gdAlphaTransparent;

		if (has_alpha) {
			*has_alpha = 1;
		}
		return gdTrueColorAlpha(r, g, b, a);
	}

	return gdTrueColor(r, g, b);
}

static int tga_scale_5_to_8(int c) { return (c * 255) / 31; }

static int tga_alpha_8_to_gd(int a) { return gdAlphaMax - (a >> 1); }

static void tga_strip_alpha(oTga *tga, int pixel_count) {
	int i;

	for (i = 0; i < pixel_count; i++) {
		int pixel = tga->bitmap[i];

		tga->bitmap[i] =
			gdTrueColor(gdTrueColorGetRed(pixel), gdTrueColorGetGreen(pixel),
						gdTrueColorGetBlue(pixel));
	}
	tga->has_alpha = 0;
}

/*!	\brief Cleans up a TGA structure.
 *	Dereferences the bitmap referenced in a TGA structure, then the structure
 *itself \param tga Pointer to TGA structure
 */
void free_tga(oTga *tga) {
	if (tga) {
		if (tga->ident)
			gdFree(tga->ident);
		if (tga->bitmap)
			gdFree(tga->bitmap);
		if (tga->colormap)
			gdFree(tga->colormap);
		gdFree(tga);
	}
}
