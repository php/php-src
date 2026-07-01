#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* JCE: Arrange HAVE_LIBPNG so that it can be set in gd.h */
#ifdef HAVE_LIBPNG

#include "gdhelpers.h"
#include "png.h" /* includes zlib.h and setjmp.h */
#include "zlib.h"

#define TRUE 1
#define FALSE 0

static const unsigned char gdPngSignature[8] = {137, 80, 78, 71,
												13,	 10, 26, 10};


BGD_DECLARE(const char *) gdPngGetVersionString(void)
{
	return PNG_LIBPNG_VER_STRING;
}

static unsigned int gdPngGetUint32(const unsigned char *data) {
	return ((unsigned int)data[0] << 24) | ((unsigned int)data[1] << 16) |
		   ((unsigned int)data[2] << 8) | (unsigned int)data[3];
}

static void gdPngPutUint32(unsigned char *data, size_t value) {
	data[0] = (unsigned char)((value >> 24) & 0xff);
	data[1] = (unsigned char)((value >> 16) & 0xff);
	data[2] = (unsigned char)((value >> 8) & 0xff);
	data[3] = (unsigned char)(value & 0xff);
}

static int gdPngChunkIs(const unsigned char *type, const char *name) {
	return memcmp(type, name, 4) == 0;
}

static int gdPngIsXmpItxt(const unsigned char *data, size_t size) {
	static const char keyword[] = "XML:com.adobe.xmp";
	size_t keyword_size = sizeof(keyword) - 1;

	return size > keyword_size && memcmp(data, keyword, keyword_size) == 0 &&
		   data[keyword_size] == 0;
}

static int gdPngIsRawProfile(const unsigned char *data, size_t size,
							 const char *profile_type) {
	static const char prefix[] = "Raw profile type ";
	size_t prefix_size = sizeof(prefix) - 1;
	size_t profile_type_size;

	if (data == NULL) {
		return FALSE;
	}

	profile_type_size = strlen(profile_type);
	return size > prefix_size + profile_type_size &&
		   memcmp(data, prefix, prefix_size) == 0 &&
		   memcmp(data + prefix_size, profile_type, profile_type_size) == 0 &&
		   data[prefix_size + profile_type_size] == 0;
}

static int gdPngSetTextProfile(gdImageMetadata *metadata,
							   const unsigned char *data, size_t size) {
	const unsigned char *nul;
	size_t keyword_size;
	char *key;
	int status;

	nul = (const unsigned char *)memchr(data, 0, size);
	if (nul == NULL || nul == data) {
		return GD_META_OK;
	}

	keyword_size = (size_t)(nul - data);
	key = (char *)gdMalloc(sizeof("png:text:") + keyword_size);
	if (key == NULL) {
		return GD_META_ERR_NOMEM;
	}
	memcpy(key, "png:text:", sizeof("png:text:") - 1);
	memcpy(key + sizeof("png:text:") - 1, data, keyword_size);
	key[sizeof("png:text:") - 1 + keyword_size] = '\0';

	status = gdImageMetadataSetProfile(metadata, key, data, size);
	gdFree(key);
	return status;
}

static int gdPngReadMetadataFromMemory(const unsigned char *png,
									   size_t png_size,
									   gdImageMetadata *metadata) {
	size_t pos = 8;

	if (metadata == NULL) {
		return GD_META_OK;
	}
	if (png == NULL || png_size < 8 || memcmp(png, gdPngSignature, 8) != 0) {
		return GD_META_ERR_FORMAT;
	}

	while (pos + 12 <= png_size) {
		unsigned int chunk_size = gdPngGetUint32(png + pos);
		const unsigned char *type = png + pos + 4;
		const unsigned char *chunk_data = png + pos + 8;
		int status = GD_META_OK;

		if ((size_t)chunk_size > png_size - pos - 12) {
			return GD_META_ERR_PARSE;
		}

		if (gdPngChunkIs(type, "eXIf")) {
			status = gdImageMetadataSetProfile(metadata, "exif", chunk_data,
											   chunk_size);
		} else if (gdPngChunkIs(type, "zTXt") &&
				   gdPngIsRawProfile(chunk_data, chunk_size, "exif")) {
			status = gdImageMetadataSetProfile(metadata, "exif", chunk_data,
											   chunk_size);
		} else if (gdPngChunkIs(type, "iCCP")) {
			status = gdImageMetadataSetProfile(metadata, "icc", chunk_data,
											   chunk_size);
		} else if (gdPngChunkIs(type, "iTXt") &&
				   gdPngIsXmpItxt(chunk_data, chunk_size)) {
			status = gdImageMetadataSetProfile(metadata, "xmp", chunk_data,
											   chunk_size);
		} else if (gdPngChunkIs(type, "zTXt") &&
				   gdPngIsRawProfile(chunk_data, chunk_size, "xmp")) {
			status = gdImageMetadataSetProfile(metadata, "xmp", chunk_data,
											   chunk_size);
		} else if (gdPngChunkIs(type, "tEXt")) {
			status = gdPngSetTextProfile(metadata, chunk_data, chunk_size);
		}
		if (status != GD_META_OK) {
			return status;
		}

		pos += (size_t)chunk_size + 12;
		if (gdPngChunkIs(type, "IEND")) {
			return GD_META_OK;
		}
	}

	return GD_META_ERR_PARSE;
}

static int gdPngAppendChunk(unsigned char *out, size_t *out_pos,
							const char *type, const unsigned char *data,
							size_t size) {
	uLong crc;

	if (data == NULL || size > UINT32_MAX) {
		return GD_META_ERR_INVALID;
	}

	gdPngPutUint32(out + *out_pos, size);
	memcpy(out + *out_pos + 4, type, 4);
	if (size != 0) {
		memcpy(out + *out_pos + 8, data, size);
	}
	crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, out + *out_pos + 4, (uInt)(size + 4));
	gdPngPutUint32(out + *out_pos + 8 + size, crc);
	*out_pos += size + 12;
	return GD_META_OK;
}

static int gdPngMetadataChunkSize(size_t *total, const unsigned char *data,
								  size_t size) {
	if (data == NULL) {
		return GD_META_OK;
	}
	if (size > UINT32_MAX || (size_t)-1 - *total < size + 12) {
		return GD_META_ERR_LIMIT;
	}
	*total += size + 12;
	return GD_META_OK;
}

static int gdPngShouldSkipChunk(const unsigned char *type,
								const unsigned char *data, size_t size,
								const gdImageMetadata *metadata) {
	return (gdImageMetadataGetProfile(metadata, "exif", NULL) != NULL &&
			(gdPngChunkIs(type, "eXIf") ||
			 (gdPngChunkIs(type, "zTXt") &&
			  gdPngIsRawProfile(data, size, "exif")))) ||
		   (gdImageMetadataGetProfile(metadata, "icc", NULL) != NULL &&
			gdPngChunkIs(type, "iCCP")) ||
		   (gdImageMetadataGetProfile(metadata, "xmp", NULL) != NULL &&
			((gdPngChunkIs(type, "iTXt") && gdPngIsXmpItxt(data, size)) ||
			 (gdPngChunkIs(type, "zTXt") &&
			  gdPngIsRawProfile(data, size, "xmp"))));
}

static void *gdPngReadCtxToMemory(gdIOCtx *infile, int *size) {
	enum { GD_PNG_ALLOC_STEP = 8192 };
	unsigned char *data = NULL;
	int logical_size = 0;
	int real_size = 0;

	if (size != NULL) {
		*size = 0;
	}
	if (infile == NULL || size == NULL) {
		return NULL;
	}

	for (;;) {
		int n;

		if (real_size - logical_size < GD_PNG_ALLOC_STEP) {
			unsigned char *temp;
			int new_size;

			if (real_size > INT_MAX - GD_PNG_ALLOC_STEP) {
				gdFree(data);
				return NULL;
			}
			new_size = real_size + GD_PNG_ALLOC_STEP;
			temp = (unsigned char *)gdRealloc(data, new_size);
			if (temp == NULL) {
				gdFree(data);
				return NULL;
			}
			data = temp;
			real_size = new_size;
		}

		n = gdGetBuf(data + logical_size, GD_PNG_ALLOC_STEP, infile);
		if (n <= 0) {
			break;
		}
		logical_size += n;
	}

	*size = logical_size;
	return data;
}

/*---------------------------------------------------------------------------

  gd_png.c                 Copyright 1999 Greg Roelofs and Thomas Boutell

  The routines in this file, gdImagePng*() and gdImageCreateFromPng*(),
  are drop-in replacements for gdImageGif*() and gdImageCreateFromGif*(),
  except that these functions are noisier in the case of errors (comment
  out all fprintf() statements to disable that).

  GD 2.0 supports RGBA truecolor and will read and write truecolor PNGs.
  GD 2.0 supports 8 bits of color resolution per channel and
  7 bits of alpha channel resolution. Images with more than 8 bits
  per channel are reduced to 8 bits. Images with an alpha channel are
  only able to resolve down to '1/128th opaque' instead of '1/256th',
  and this conversion is also automatic. I very much doubt you can see it.
  Both tRNS and true alpha are supported.

  Gamma is ignored, and there is no support for text annotations.

  Last updated:  9 February 2001

  ---------------------------------------------------------------------------*/

/**
 * File: PNG IO
 *
 * Read and write PNG images.
 */

#ifdef PNG_SETJMP_SUPPORTED
typedef struct _jmpbuf_wrapper {
	jmp_buf jmpbuf;
} jmpbuf_wrapper;

static void gdPngErrorHandler(png_structp png_ptr, png_const_charp msg) {
	jmpbuf_wrapper *jmpbuf_ptr;

	/* This function, aside from the extra step of retrieving the "error
	 * pointer" (below) and the fact that it exists within the application
	 * rather than within libpng, is essentially identical to libpng's
	 * default error handler.  The second point is critical:  since both
	 * setjmp() and longjmp() are called from the same code, they are
	 * guaranteed to have compatible notions of how big a jmp_buf is,
	 * regardless of whether _BSD_SOURCE or anything else has (or has not)
	 * been defined. */

	gd_error_ex(GD_WARNING, "gd-png: fatal libpng error: %s\n", msg);

	jmpbuf_ptr = png_get_error_ptr(png_ptr);
	if (jmpbuf_ptr == NULL) { /* we are completely hosed now */
		gd_error_ex(GD_ERROR, "gd-png: EXTREMELY fatal error: jmpbuf "
							  "unrecoverable; terminating.\n");
		exit(99);
	}

	longjmp(jmpbuf_ptr->jmpbuf, 1);
}

static void gdPngWarningHandler(UNUSED_PARAM(png_structp png_ptr),
								png_const_charp msg) {
	gd_error_ex(GD_WARNING, "gd-png: libpng warning: %s", msg);
}
#endif

static void gdPngReadData(png_structp png_ptr, png_bytep data,
						  png_size_t length) {
	int check;
	check = gdGetBuf(data, length, (gdIOCtx *)png_get_io_ptr(png_ptr));
	if (check != (int)length) {
		png_error(png_ptr, "Read Error: truncated data");
	}
}

static void gdPngWriteData(png_structp png_ptr, png_bytep data,
						   png_size_t length) {
	gdPutBuf(data, length, (gdIOCtx *)png_get_io_ptr(png_ptr));
}

static void gdPngFlushData(png_structp png_ptr) { (void)png_ptr; }

/*
  Function: gdImageCreateFromPng

	<gdImageCreateFromPng> is called to load images from PNG format
	files. Invoke <gdImageCreateFromPng> with an already opened
	pointer to a FILE containing the desired
	image. <gdImageCreateFromPng> returns a <gdImagePtr> to the new
	image, or NULL if unable to load the image (most often because the
	file is corrupt or does not contain a PNG
	image). <gdImageCreateFromPng> does not close the file. You can
	inspect the sx and sy members of the image to determine its
	size. The image must eventually be destroyed using
	gdImageDestroy().

	If the PNG image being loaded is a truecolor image, the resulting
	gdImagePtr will refer to a truecolor image. If the PNG image being
	loaded is a palette or grayscale image, the resulting gdImagePtr
	will refer to a palette image. gd retains only 8 bits of
	resolution for each of the red, green and blue channels, and only
	7 bits of resolution for the alpha channel. The former restriction
	affects only a handful of very rare 48-bit color and 16-bit
	grayscale PNG images. The second restriction affects all
	semitransparent PNG images, but the difference is essentially
	invisible to the eye. 7 bits of alpha channel resolution is, in
	practice, quite a lot.

  Variants:

	<gdImageCreateFromPngPtr> creates an image from PNG data (i.e. the
	contents of a PNG file) already in memory.

	<gdImageCreateFromPngCtx> reads in an image using the functions in
	a <gdIOCtx> struct.

	<gdImageCreateFromPngSource> is similar to
	<gdImageCreateFromPngCtx> but uses the old <gdSource> interface.
	It is *obsolete*.

  Parameters:

	infile - The input FILE pointer.

  Returns:

	A pointer to the new image or NULL if an error occurred.

  Example:
	(start code)

	gdImagePtr im;
	... inside a function ...
	FILE *in;
	in = fopen("mypng.png", "rb");
	im = gdImageCreateFromPng(in);
	fclose(in);
	// ... Use the image ...
	gdImageDestroy(im);

	(end code)
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPng(FILE *inFile) {
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (in == NULL)
		return NULL;
	im = gdImageCreateFromPngCtx(in);
	in->gd_free(in);
	return im;
}

/*
  Function: gdImageCreateFromPngPtr

  See <gdImageCreateFromPng>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr(int size, void *data) {
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in)
		return 0;
	im = gdImageCreateFromPngCtx(in);
	in->gd_free(in);
	return im;
}

BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngPtrWithMetadata(int size, void *data,
									gdImageMetadata *metadata) {
	gdImagePtr im;
	gdIOCtx *in;

	if (metadata != NULL) {
		int status = gdPngReadMetadataFromMemory((const unsigned char *)data,
												 size, metadata);
		if (status != GD_META_OK) {
			return NULL;
		}
	}

	in = gdNewDynamicCtxEx(size, data, 0);
	if (!in)
		return 0;
	im = gdImageCreateFromPngCtx(in);
	in->gd_free(in);
	return im;
}

BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngCtxWithMetadata(gdIOCtx *infile,
									gdImageMetadata *metadata) {
	void *data;
	int size;
	gdImagePtr im;

	data = gdPngReadCtxToMemory(infile, &size);
	if (data == NULL) {
		return NULL;
	}

	im = gdImageCreateFromPngPtrWithMetadata(size, data, metadata);
	gdFree(data);
	return im;
}

/* This routine is based in part on the Chapter 13 demo code in
 * "PNG: The Definitive Guide" (http://www.libpng.org/pub/png/book/).
 */

/*
  Function: gdImageCreateFromPngCtx

  See <gdImageCreateFromPng>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx(gdIOCtx *infile) {
	png_byte sig[8];
#ifdef PNG_SETJMP_SUPPORTED
	jmpbuf_wrapper jbw;
#endif
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height, rowbytes, w, h, res_x, res_y;
	int bit_depth, color_type, interlace_type, unit_type;
	int num_palette = 0, num_trans;
	png_colorp palette;
	png_color_16p trans_gray_rgb;
	png_color_16p trans_color_rgb;
	png_bytep trans;
	png_bytep image_data = NULL;
	png_bytepp row_pointers = NULL;
	gdImagePtr im = NULL;
	int i, j, *open = NULL;
	volatile int transparent = -1;
	volatile int palette_allocated = FALSE;

	/* Make sure the signature can't match by dumb luck -- TBB */
	/* GRR: isn't sizeof(infile) equal to the size of the pointer? */
	memset(sig, 0, sizeof(sig));

	/* first do a quick check that the file really is a PNG image; could
	 * have used slightly more general png_sig_cmp() function instead */
	if (gdGetBuf(sig, 8, infile) < 8) {
		return NULL;
	}

	if (png_sig_cmp(sig, 0, 8) != 0) { /* bad signature */
		return NULL;				   /* bad signature */
	}

#ifdef PNG_SETJMP_SUPPORTED
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, &jbw,
									 gdPngErrorHandler, gdPngWarningHandler);
#else
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#endif
	if (png_ptr == NULL) {
		gd_error("gd-png error: cannot allocate libpng main struct\n");
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		gd_error("gd-png error: cannot allocate libpng info struct\n");
		png_destroy_read_struct(&png_ptr, NULL, NULL);

		return NULL;
	}

	/* we could create a second info struct here (end_info), but it's only
	 * useful if we want to keep pre- and post-IDAT chunk info separated
	 * (mainly for PNG-aware image editors and converters)
	 */

	/* setjmp() must be called in every non-callback function that calls a
	 * PNG-reading libpng function.  We must reset it everytime we get a
	 * new allocation that we save in a stack variable.
	 */
#ifdef PNG_SETJMP_SUPPORTED
	if (setjmp(jbw.jmpbuf)) {
		gd_error("gd-png error: setjmp returns error condition 1\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		return NULL;
	}
#endif

	png_set_sig_bytes(png_ptr, 8); /* we already read the 8 signature bytes */

	png_set_read_fn(png_ptr, (void *)infile, gdPngReadData);
	png_set_user_limits(png_ptr, 0x7fffffffL, 0x7fffffffL);
	png_read_info(png_ptr, info_ptr); /* read all PNG info up to image data */

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
				 &interlace_type, NULL, NULL);
	if ((color_type == PNG_COLOR_TYPE_RGB) ||
		(color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		im = gdImageCreateTrueColor((int)width, (int)height);
	} else {
		im = gdImageCreate((int)width, (int)height);
	}
	if (im == NULL) {
		gd_error("gd-png error: cannot allocate gdImage struct\n");
		goto error;
	}

	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	} else if (bit_depth < 8) {
		png_set_packing(png_ptr); /* expand to 1 byte per pixel */
	}

	/* setjmp() must be called in every non-callback function that calls a
	 * PNG-reading libpng function.  We must reset it everytime we get a
	 * new allocation that we save in a stack variable.
	 */
#ifdef PNG_SETJMP_SUPPORTED
	if (setjmp(jbw.jmpbuf)) {
		gd_error("gd-png error: setjmp returns error condition 2\n");
		goto error;
	}
#endif

#ifdef PNG_pHYs_SUPPORTED
	/* check if the resolution is specified */
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs)) {
		if (png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type)) {
			switch (unit_type) {
			case PNG_RESOLUTION_METER:
				im->res_x = DPM2DPI(res_x);
				im->res_y = DPM2DPI(res_y);
				break;
			}
		}
	}
#endif

	switch (color_type) {
	case PNG_COLOR_TYPE_PALETTE:
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
#ifdef DEBUG
		gd_error("gd-png color_type is palette, colors: %d\n", num_palette);
#endif /* DEBUG */
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
			/* gd 2.0: we support this rather thoroughly now. Grab the
			 * first fully transparent entry, if any, as the value of
			 * the simple-transparency index, mostly for backwards
			 * binary compatibility. The alpha channel is where it's
			 * really at these days.
			 */
			int firstZero = 1;
			png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
			for (i = 0; i < num_trans; ++i) {
				im->alpha[i] = gdAlphaMax - (trans[i] >> 1);
				if ((trans[i] == 0) && (firstZero)) {
					/* 2.0.5: long-forgotten patch from Wez Furlong */
					transparent = i;
					firstZero = 0;
				}
			}
		}
		break;

	case PNG_COLOR_TYPE_GRAY:
		/* create a fake palette and check for single-shade transparency */
		if ((palette = (png_colorp)gdMalloc(256 * sizeof(png_color))) == NULL) {
			gd_error("gd-png error: cannot allocate gray palette\n");
			goto error;
		}
		palette_allocated = TRUE;
		if (bit_depth < 8) {
			num_palette = 1 << bit_depth;
			for (i = 0; i < 256; ++i) {
				j = (255 * i) / (num_palette - 1);
				palette[i].red = palette[i].green = palette[i].blue = j;
			}
		} else {
			num_palette = 256;
			for (i = 0; i < 256; ++i) {
				palette[i].red = palette[i].green = palette[i].blue = i;
			}
		}
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_gray_rgb);
			if (bit_depth == 16) { /* png_set_strip_16() not yet in effect */
				transparent = trans_gray_rgb->gray >> 8;
			} else {
				transparent = trans_gray_rgb->gray;
			}
			/* Note slight error in 16-bit case:  up to 256 16-bit shades
			 * may get mapped to a single 8-bit shade, and only one of them
			 * is supposed to be transparent.  IOW, both opaque pixels and
			 * transparent pixels will be mapped into the transparent entry.
			 * There is no particularly good way around this in the case
			 * that all 256 8-bit shades are used, but one could write some
			 * custom 16-bit code to handle the case where there are gdFree
			 * palette entries.  This error will be extremely rare in
			 * general, though.  (Quite possibly there is only one such
			 * image in existence.) */
		}
		break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
		png_set_gray_to_rgb(png_ptr);
		// fall through
		// Keep above comment, gcc recognizes it and silent its warning about
		// fall through case here
	case PNG_COLOR_TYPE_RGB:
	case PNG_COLOR_TYPE_RGB_ALPHA:
		/* gd 2.0: we now support truecolor. See the comment above
		   for a rare situation in which the transparent pixel may not
		   work properly with 16-bit channels. */
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_color_rgb);
			if (bit_depth == 16) { /* png_set_strip_16() not yet in effect */
				transparent = gdTrueColor(trans_color_rgb->red >> 8,
										  trans_color_rgb->green >> 8,
										  trans_color_rgb->blue >> 8);
			} else {
				transparent =
					gdTrueColor(trans_color_rgb->red, trans_color_rgb->green,
								trans_color_rgb->blue);
			}
		}
		break;
	default:
		gd_error("gd-png color_type is unknown: %d\n", color_type);
		goto error;
	}

	/* enable the interlace transform if supported */
#ifdef PNG_READ_INTERLACING_SUPPORTED
	(void)png_set_interlace_handling(png_ptr);
#endif

	png_read_update_info(png_ptr, info_ptr);

	/* allocate space for the PNG image data */
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	if (overflow2(rowbytes, height))
		goto error;
	image_data = (png_bytep)gdMalloc(rowbytes * height);
	if (!image_data) {
		gd_error("gd-png error: cannot allocate image data\n");
		goto error;
	}
	if (overflow2(height, sizeof(png_bytep)))
		goto error;

	row_pointers = (png_bytepp)gdMalloc(height * sizeof(png_bytep));
	if (!row_pointers) {
		gd_error("gd-png error: cannot allocate row pointers\n");
		goto error;
	}

	/* setjmp() must be called in every non-callback function that calls a
	 * PNG-reading libpng function.  We must reset it everytime we get a
	 * new allocation that we save in a stack variable.
	 */
#ifdef PNG_SETJMP_SUPPORTED
	if (setjmp(jbw.jmpbuf)) {
		gd_error("gd-png error: setjmp returns error condition 3\n");
		goto error;
	}
#endif

	/* set the individual row_pointers to point at the correct offsets */
	for (h = 0; h < height; ++h) {
		row_pointers[h] = image_data + h * rowbytes;
	}

	png_read_image(png_ptr, row_pointers); /* read whole image... */
	png_read_end(png_ptr, NULL);		   /* ...done! */

	if (!im->trueColor) {
		im->colorsTotal = num_palette;
		/* load the palette and mark all entries "open" (unused) for now */
		open = im->open;
		for (i = 0; i < num_palette; ++i) {
			im->red[i] = palette[i].red;
			im->green[i] = palette[i].green;
			im->blue[i] = palette[i].blue;
			open[i] = 1;
		}
		for (i = num_palette; i < gdMaxColors; ++i) {
			open[i] = 1;
		}
	}
	/* 2.0.12: Slaven Rezic: palette images are not the only images
	   with a simple transparent color setting */
	im->transparent = transparent;
	im->interlace = (interlace_type == PNG_INTERLACE_ADAM7);

	/* can't nuke structs until done with palette */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	switch (color_type) {
	case PNG_COLOR_TYPE_RGB:
		for (h = 0; h < height; h++) {
			int boffset = 0;
			for (w = 0; w < width; w++) {
				register png_byte r = row_pointers[h][boffset++];
				register png_byte g = row_pointers[h][boffset++];
				register png_byte b = row_pointers[h][boffset++];
				im->tpixels[h][w] = gdTrueColor(r, g, b);
			}
		}
		break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
	case PNG_COLOR_TYPE_RGB_ALPHA:
		for (h = 0; h < height; h++) {
			int boffset = 0;
			for (w = 0; w < width; w++) {
				register png_byte r = row_pointers[h][boffset++];
				register png_byte g = row_pointers[h][boffset++];
				register png_byte b = row_pointers[h][boffset++];

				/* gd has only 7 bits of alpha channel resolution, and
				 * 127 is transparent, 0 opaque. A moment of convenience,
				 *  a lifetime of compatibility.
				 */

				register png_byte a =
					gdAlphaMax - (row_pointers[h][boffset++] >> 1);
				im->tpixels[h][w] = gdTrueColorAlpha(r, g, b, a);
			}
		}
		break;
	default:
		if (!im->trueColor) {
			/* Palette image, or something coerced to be one */
			for (h = 0; h < height; ++h) {
				for (w = 0; w < width; ++w) {
					register png_byte idx = row_pointers[h][w];
					im->pixels[h][w] = idx;
					open[idx] = 0;
				}
			}
		}
	}
#ifdef DEBUG
	if (!im->trueColor) {
		for (i = num_palette; i < gdMaxColors; ++i) {
			if (!open[i]) {
				fprintf(stderr,
						"gd-png warning: image data references out-of-range"
						" color index (%d)\n",
						i);
			}
		}
	}
#endif

done:
	if (palette_allocated) {
		gdFree(palette);
	}
	if (image_data)
		gdFree(image_data);
	if (row_pointers)
		gdFree(row_pointers);

	return im;

error:
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	if (im) {
		gdImageDestroy(im);
		im = NULL;
	}
	goto done;
}

/*
  Function: gdImagePngEx

	<gdImagePngEx> outputs the specified image to the specified file in
	PNG format. The file must be open for writing. Under MSDOS and all
	versions of Windows, it is important to use "wb" as opposed to
	simply "w" as the mode when opening the file, and under Unix there
	is no penalty for doing so. <gdImagePngEx> does not close the file;
	your code must do so.

	In addition, <gdImagePngEx> allows the level of compression to be
	specified. A compression level of 0 means "no compression." A
	compression level of 1 means "compressed, but as quickly as
	possible." A compression level of 9 means "compressed as much as
	possible to produce the smallest possible file." A compression level
	of -1 will use the default compression level at the time zlib was
	compiled on your system.

  Variants:

	<gdImagePng> is equivalent to calling <gdImagePngEx> with
	compression of -1.

	<gdImagePngCtx> and <gdImagePngCtxEx> write via a <gdIOCtx>
	instead of a file handle.

	<gdImagePngPtr> and <gdImagePngPtrEx> store the image file to
	memory.

  Parameters:

	im      - the image to write
	outFile - the output FILE* object.
	level   - compression level: 0 -> none, 1-9 -> level, -1 -> default

  Returns:

	Nothing.

  Example:
	(start code)

	gdImagePtr im;
	int black, white;
	FILE *out;

	im = gdImageCreate(100, 100);              // Create the image
	white = gdImageColorAllocate(im, 255, 255, 255); // Alloc background
	black = gdImageColorAllocate(im, 0, 0, 0); // Allocate drawing color
	gdImageRectangle(im, 0, 0, 99, 99, black); // Draw rectangle
	out = fopen("rect.png", "wb");             // Open output file (binary)
	gdImagePngEx(im, out, 9);                  // Write PNG, max compression
	fclose(out);                               // Close file
	gdImageDestroy(im);                        // Destroy image

	(end code)
*/
BGD_DECLARE(void) gdImagePngEx(gdImagePtr im, FILE *outFile, int level) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL)
		return;
	gdImagePngCtxEx(im, out, level);
	out->gd_free(out);
}

/*
  Function: gdImagePng

	Equivalent to calling <gdImagePngEx> with compression of -1.

  Parameters:

	im      - the image to save.
	outFile - the output FILE*.

  Returns:

	Nothing.
*/
BGD_DECLARE(void) gdImagePng(gdImagePtr im, FILE *outFile) {
	gdImagePngEx(im, outFile, -1);
}

static int _gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile,
									 const gdPngWriteOptions *options);

BGD_DECLARE(void) gdPngWriteOptionsInit(gdPngWriteOptions *options) {
	if (options == NULL)
		return;
	memset(options, 0, sizeof(*options));
	options->struct_size = sizeof(*options);
	options->compression_level = -1;
	options->filters = GD_PNG_FILTER_AUTO;
	options->compression_strategy = GD_PNG_COMPRESSION_STRATEGY_DEFAULT;
}

static int gdPngWriteOptionsValid(const gdPngWriteOptions *options) {
	if (options->struct_size < sizeof(*options)) {
		gd_error("gd-png error: invalid options structure size\n");
		return FALSE;
	}
	if (options->compression_level < -1 || options->compression_level > 9) {
		gd_error("gd-png error: compression level must be -1 through 9\n");
		return FALSE;
	}
	if ((options->filters & ~GD_PNG_FILTER_ALL) != 0) {
		gd_error("gd-png error: invalid filter mask\n");
		return FALSE;
	}
	if (options->compression_strategy < GD_PNG_COMPRESSION_STRATEGY_DEFAULT ||
		options->compression_strategy > GD_PNG_COMPRESSION_STRATEGY_FIXED) {
		gd_error("gd-png error: invalid compression strategy\n");
		return FALSE;
	}
	return TRUE;
}

BGD_DECLARE(int)
gdImagePngWithOptions(gdImagePtr im, FILE *outFile,
					  const gdPngWriteOptions *options) {
	gdIOCtx *out;
	int status;
	if (im == NULL || outFile == NULL)
		return 1;
	out = gdNewFileCtx(outFile);
	if (out == NULL)
		return 1;
	status = gdImagePngCtxWithOptions(im, out, options);
	out->gd_free(out);
	return status;
}

/*
  Function: gdImagePngPtr

	Equivalent to calling <gdImagePngPtrEx> with compression of -1.

	See <gdImagePngEx> for more information.

  Parameters:

	im      - the image to save.
	size    - Output: size in bytes of the result.

  Returns:

	A pointer to memory containing the image data or NULL on error.

*/
BGD_DECLARE(void *) gdImagePngPtr(gdImagePtr im, int *size) {
	return gdImagePngPtrEx(im, size, -1);
}

/*
  Function: gdImagePngPtrEx

	Identical to <gdImagePngEx> except that it returns a pointer to a
	memory area with the PNG data. This memory must be freed by the
	caller when it is no longer needed. **The caller must invoke
	gdFree(), not free()**

	The 'size' parameter receives the total size of the block of
	memory.

	See <gdImagePngEx> for more information.

  Parameters:

	im      - the image to save.
	size    - Output: size in bytes of the result.
	level   - compression level: 0 -> none, 1-9 -> level, -1 -> default

  Returns:

	A pointer to memory containing the image data or NULL on error.

*/
BGD_DECLARE(void *) gdImagePngPtrEx(gdImagePtr im, int *size, int level) {
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	gdPngWriteOptions options;
	if (out == NULL)
		return NULL;
	gdPngWriteOptionsInit(&options);
	options.compression_level = level;
	if (!_gdImagePngCtxWithOptions(im, out, &options))
		rv = gdDPExtractData(out, size);
	else
		rv = NULL;
	out->gd_free(out);
	return rv;
}

BGD_DECLARE(void *)
gdImagePngPtrWithOptions(gdImagePtr im, int *size,
						 const gdPngWriteOptions *options) {
	gdPngWriteOptions defaults;
	gdIOCtx *out;
	void *rv = NULL;
	if (size != NULL)
		*size = 0;
	if (im == NULL || size == NULL)
		return NULL;
	if (options == NULL) {
		gdPngWriteOptionsInit(&defaults);
		options = &defaults;
	}
	if (!gdPngWriteOptionsValid(options))
		return NULL;
	out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL)
		return NULL;
	if (!_gdImagePngCtxWithOptions(im, out, options))
		rv = gdDPExtractData(out, size);
	out->gd_free(out);
	if (rv != NULL && options->metadata != NULL &&
		gdImageMetadataInjectPng(&rv, size, options->metadata) != GD_META_OK) {
		gdFree(rv);
		*size = 0;
		return NULL;
	}
	return rv;
}

BGD_DECLARE(void *)
gdImagePngPtrWithMetadata(gdImagePtr im, int *size,
						  const gdImageMetadata *metadata) {
	return gdImagePngPtrExWithMetadata(im, size, -1, metadata);
}

BGD_DECLARE(void *)
gdImagePngPtrExWithMetadata(gdImagePtr im, int *size, int level,
							const gdImageMetadata *metadata) {
	void *rv = gdImagePngPtrEx(im, size, level);
	if (rv == NULL)
		return NULL;
	if (gdImageMetadataInjectPng(&rv, size, metadata) != GD_META_OK) {
		gdFree(rv);
		if (size != NULL)
			*size = 0;
		return NULL;
	}
	return rv;
}

BGD_DECLARE(int)
gdImageMetadataInjectPng(void **data, int *size,
						 const gdImageMetadata *metadata) {
	const unsigned char *png;
	unsigned char *out;
	const unsigned char *profile;
	size_t profile_size;
	size_t png_size;
	size_t pos;
	size_t out_pos;
	size_t out_size;
	size_t extra_size = 0;
	size_t i;
	int wrote_after_ihdr = FALSE;

	if (data == NULL || size == NULL || *data == NULL || *size < 0) {
		return GD_META_ERR_INVALID;
	}
	if (metadata == NULL) {
		return GD_META_OK;
	}

	png = (const unsigned char *)*data;
	png_size = (size_t)*size;
	if (png_size < 8 || memcmp(png, gdPngSignature, 8) != 0) {
		return GD_META_ERR_FORMAT;
	}

	profile = gdImageMetadataGetProfile(metadata, "exif", &profile_size);
	if (gdPngMetadataChunkSize(&extra_size, profile, profile_size) !=
		GD_META_OK) {
		return GD_META_ERR_LIMIT;
	}
	profile = gdImageMetadataGetProfile(metadata, "icc", &profile_size);
	if (gdPngMetadataChunkSize(&extra_size, profile, profile_size) !=
		GD_META_OK) {
		return GD_META_ERR_LIMIT;
	}
	profile = gdImageMetadataGetProfile(metadata, "xmp", &profile_size);
	if (gdPngMetadataChunkSize(&extra_size, profile, profile_size) !=
		GD_META_OK) {
		return GD_META_ERR_LIMIT;
	}
	for (i = 0; i < gdImageMetadataGetProfileCount(metadata); i++) {
		const char *key = NULL;
		const unsigned char *text_data = NULL;
		size_t text_size = 0;
		if (gdImageMetadataGetProfileAt(metadata, i, &key, &text_data,
										&text_size) == GD_META_OK &&
			key != NULL && strncmp(key, "png:text:", 9) == 0) {
			if (gdPngMetadataChunkSize(&extra_size, text_data, text_size) !=
				GD_META_OK) {
				return GD_META_ERR_LIMIT;
			}
		}
	}

	if ((size_t)-1 - png_size < extra_size || png_size + extra_size > INT_MAX) {
		return GD_META_ERR_LIMIT;
	}
	out_size = png_size + extra_size;
	out = (unsigned char *)gdMalloc(out_size);
	if (out == NULL) {
		return GD_META_ERR_NOMEM;
	}

	memcpy(out, png, 8);
	pos = 8;
	out_pos = 8;
	while (pos + 12 <= png_size) {
		unsigned int chunk_size = gdPngGetUint32(png + pos);
		const unsigned char *type = png + pos + 4;
		const unsigned char *chunk_data = png + pos + 8;
		size_t chunk_total;

		if ((size_t)chunk_size > png_size - pos - 12) {
			gdFree(out);
			return GD_META_ERR_PARSE;
		}
		chunk_total = (size_t)chunk_size + 12;

		if (!gdPngShouldSkipChunk(type, chunk_data, chunk_size, metadata)) {
			memcpy(out + out_pos, png + pos, chunk_total);
			out_pos += chunk_total;
		}
		pos += chunk_total;

		if (gdPngChunkIs(type, "IHDR") && !wrote_after_ihdr) {
			int status;
			profile =
				gdImageMetadataGetProfile(metadata, "exif", &profile_size);
			if (profile != NULL &&
				(status = gdPngAppendChunk(
					 out, &out_pos,
					 gdPngIsRawProfile(profile, profile_size, "exif") ? "zTXt"
																	  : "eXIf",
					 profile, profile_size)) != GD_META_OK) {
				gdFree(out);
				return status;
			}
			profile = gdImageMetadataGetProfile(metadata, "icc", &profile_size);
			if (profile != NULL &&
				(status = gdPngAppendChunk(out, &out_pos, "iCCP", profile,
										   profile_size)) != GD_META_OK) {
				gdFree(out);
				return status;
			}
			profile = gdImageMetadataGetProfile(metadata, "xmp", &profile_size);
			if (profile != NULL &&
				(status = gdPngAppendChunk(
					 out, &out_pos,
					 gdPngIsRawProfile(profile, profile_size, "xmp") ? "zTXt"
																	 : "iTXt",
					 profile, profile_size)) != GD_META_OK) {
				gdFree(out);
				return status;
			}
			for (i = 0; i < gdImageMetadataGetProfileCount(metadata); i++) {
				const char *key = NULL;
				const unsigned char *text_data = NULL;
				size_t text_size = 0;
				if (gdImageMetadataGetProfileAt(metadata, i, &key, &text_data,
												&text_size) == GD_META_OK &&
					key != NULL && strncmp(key, "png:text:", 9) == 0) {
					status = gdPngAppendChunk(out, &out_pos, "tEXt", text_data,
											  text_size);
					if (status != GD_META_OK) {
						gdFree(out);
						return status;
					}
				}
			}
			wrote_after_ihdr = TRUE;
		}

		if (gdPngChunkIs(type, "IEND")) {
			gdFree(*data);
			*data = out;
			*size = (int)out_pos;
			return GD_META_OK;
		}
	}

	gdFree(out);
	return GD_META_ERR_PARSE;
}

/*
  Function: gdImagePngCtx

	Equivalent to calling <gdImagePngCtxEx> with compression of -1.
	See <gdImagePngEx> for more information.

  Parameters:

	im      - the image to save.
	outfile - the <gdIOCtx> to write to.

  Returns:

	Nothing.

*/
BGD_DECLARE(void) gdImagePngCtx(gdImagePtr im, gdIOCtx *outfile) {
	/* 2.0.13: 'return' here was an error, thanks to Kevin Smith */
	gdImagePngCtxEx(im, outfile, -1);
}

BGD_DECLARE(void)
gdImagePngCtxWithMetadata(gdImagePtr im, gdIOCtx *outfile,
						  const gdImageMetadata *metadata) {
	gdImagePngCtxExWithMetadata(im, outfile, -1, metadata);
}

/*
  Function: gdImagePngCtxEx

	Outputs the given image as PNG data, but using a <gdIOCtx> instead
	of a file.  See <gdImagePngEx>.

  Parameters:

	im      - the image to save.
	outfile - the <gdIOCtx> to write to.
	level   - compression level: 0 -> none, 1-9 -> level, -1 -> default

  Returns:

	Nothing.

*/
BGD_DECLARE(void) gdImagePngCtxEx(gdImagePtr im, gdIOCtx *outfile, int level) {
	gdPngWriteOptions options;
	gdPngWriteOptionsInit(&options);
	options.compression_level = level;
	(void)_gdImagePngCtxWithOptions(im, outfile, &options);
}

BGD_DECLARE(void)
gdImagePngCtxExWithMetadata(gdImagePtr im, gdIOCtx *outfile, int level,
							const gdImageMetadata *metadata) {
	void *data;
	int size = 0;
	data = gdImagePngPtrExWithMetadata(im, &size, level, metadata);
	if (data == NULL)
		return;
	gdPutBuf(data, size, outfile);
	gdFree(data);
}

BGD_DECLARE(int)
gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile,
						 const gdPngWriteOptions *options) {
	gdPngWriteOptions defaults;
	void *data;
	int size = 0;

	if (im == NULL || outfile == NULL)
		return 1;
	if (options == NULL) {
		gdPngWriteOptionsInit(&defaults);
		options = &defaults;
	}
	if (!gdPngWriteOptionsValid(options))
		return 1;
	if (options->metadata == NULL)
		return _gdImagePngCtxWithOptions(im, outfile, options);
	data = gdImagePngPtrWithOptions(im, &size, options);
	if (data == NULL)
		return 1;
	if (gdPutBuf(data, size, outfile) != size) {
		gdFree(data);
		return 1;
	}
	gdFree(data);
	return 0;
}

/* This routine is based in part on code from Dale Lutz (Safe Software Inc.)
 *  and in part on demo code from Chapter 15 of "PNG: The Definitive Guide"
 *  (http://www.libpng.org/pub/png/book/).
 */
/* returns 0 on success, 1 on failure */
static int _gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile,
									 const gdPngWriteOptions *options) {
	int i, j, bit_depth = 0, interlace_type;
	int width = im->sx;
	int height = im->sy;
	int colors = im->colorsTotal;
	int *open = im->open;
	int mapping[gdMaxColors]; /* mapping[gd_index] == png_index */
	png_byte trans_values[256];
	png_color_16 trans_rgb_value;
	png_color palette[gdMaxColors];
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers = NULL;
	volatile int transparent = im->transparent;
	volatile int remap = FALSE;
#ifdef PNG_SETJMP_SUPPORTED
	jmpbuf_wrapper jbw;
#endif
	int ret = 0;

	/* width or height of value 0 is invalid in IHDR;
	   see http://www.w3.org/TR/PNG-Chunks.html */
	if (width == 0 || height == 0)
		return 1;

#ifdef PNG_SETJMP_SUPPORTED
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &jbw,
									  gdPngErrorHandler, gdPngWarningHandler);
#else
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#endif
	if (png_ptr == NULL) {
		gd_error("gd-png error: cannot allocate libpng main struct\n");
		return 1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		gd_error("gd-png error: cannot allocate libpng info struct\n");
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return 1;
	}

#ifdef PNG_SETJMP_SUPPORTED
	if (setjmp(jbw.jmpbuf)) {
		gd_error("gd-png error: setjmp returns error condition\n");
		png_destroy_write_struct(&png_ptr, &info_ptr);

		if (row_pointers) {
			for (i = 0; i < height; ++i)
				gdFree(row_pointers[i]);
			gdFree(row_pointers);
		}

		return 1;
	}
#endif

	png_set_write_fn(png_ptr, (void *)outfile, gdPngWriteData, gdPngFlushData);

	png_set_user_limits(png_ptr, 0x7fffffffL, 0x7fffffffL);

	/* This is best for palette images, and libpng defaults to it for
	   palette images anyway, so we don't need to do it explicitly.
	   What to ideally do for truecolor images depends, alas, on the image.
	   gd is intentionally imperfect and doesn't spend a lot of time
	   fussing with such things. */

	/* 2.0.12: this is finally a parameter */
	png_set_compression_level(png_ptr, options->compression_level);
	if (options->filters != GD_PNG_FILTER_AUTO) {
		int filters = 0;
		if (options->filters & GD_PNG_FILTER_NONE)
			filters |= PNG_FILTER_NONE;
		if (options->filters & GD_PNG_FILTER_SUB)
			filters |= PNG_FILTER_SUB;
		if (options->filters & GD_PNG_FILTER_UP)
			filters |= PNG_FILTER_UP;
		if (options->filters & GD_PNG_FILTER_AVERAGE)
			filters |= PNG_FILTER_AVG;
		if (options->filters & GD_PNG_FILTER_PAETH)
			filters |= PNG_FILTER_PAETH;
		png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, filters);
	}
	if (options->compression_strategy != GD_PNG_COMPRESSION_STRATEGY_DEFAULT) {
		int strategy = Z_DEFAULT_STRATEGY;
		switch (options->compression_strategy) {
		case GD_PNG_COMPRESSION_STRATEGY_FILTERED:
			strategy = Z_FILTERED;
			break;
		case GD_PNG_COMPRESSION_STRATEGY_HUFFMAN_ONLY:
			strategy = Z_HUFFMAN_ONLY;
			break;
		case GD_PNG_COMPRESSION_STRATEGY_RLE:
			strategy = Z_RLE;
			break;
		case GD_PNG_COMPRESSION_STRATEGY_FIXED:
			strategy = Z_FIXED;
			break;
		}
		png_set_compression_strategy(png_ptr, strategy);
	}

#ifdef PNG_pHYs_SUPPORTED
	/* 2.1.0: specify the resolution */
	png_set_pHYs(png_ptr, info_ptr, DPI2DPM(im->res_x), DPI2DPM(im->res_y),
				 PNG_RESOLUTION_METER);
#endif

	/* can set this to a smaller value without compromising compression if all
	 * image data is 16K or less; will save some decoder memory [min == 8] */
	/*  png_set_compression_window_bits(png_ptr, 15);  */

	if (!im->trueColor) {
		if (transparent >= im->colorsTotal ||
			(transparent >= 0 && open[transparent]))
			transparent = -1;
	}
	if (!im->trueColor) {
		for (i = 0; i < gdMaxColors; ++i)
			mapping[i] = -1;
	}
	if (!im->trueColor) {
		/* count actual number of colors used (colorsTotal == high-water mark)
		 */
		colors = 0;
		for (i = 0; i < im->colorsTotal; ++i) {
			if (!open[i]) {
				mapping[i] = colors;
				++colors;
			}
		}
		if (colors == 0) {
			gd_error("gd-png error: no colors in palette\n");
			ret = 1;
			goto bail;
		}
		if (colors < im->colorsTotal) {
			remap = TRUE;
		}
		if (colors <= 2)
			bit_depth = 1;
		else if (colors <= 4)
			bit_depth = 2;
		else if (colors <= 16)
			bit_depth = 4;
		else
			bit_depth = 8;
	}
	interlace_type = im->interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

	if (im->trueColor) {
		if (im->saveAlphaFlag) {
			png_set_IHDR(png_ptr, info_ptr, width, height, 8,
						 PNG_COLOR_TYPE_RGB_ALPHA, interlace_type,
						 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		} else {
			png_set_IHDR(png_ptr, info_ptr, width, height, 8,
						 PNG_COLOR_TYPE_RGB, interlace_type,
						 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		}
	} else {
		png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
					 PNG_COLOR_TYPE_PALETTE, interlace_type,
					 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	}
	if (im->trueColor && (!im->saveAlphaFlag) && (transparent >= 0)) {
		/* 2.0.9: fixed by Thomas Winzig */
		trans_rgb_value.red = gdTrueColorGetRed(im->transparent);
		trans_rgb_value.green = gdTrueColorGetGreen(im->transparent);
		trans_rgb_value.blue = gdTrueColorGetBlue(im->transparent);
		png_set_tRNS(png_ptr, info_ptr, 0, 0, &trans_rgb_value);
	}
	if (!im->trueColor) {
		/* Oy veh. Remap the PNG palette to put the
		   entries with interesting alpha channel
		   values first. This minimizes the size
		   of the tRNS chunk and thus the size
		   of the PNG file as a whole. */
		int tc = 0;
		int i;
		int j;
		int k;
		for (i = 0; (i < im->colorsTotal); i++) {
			if ((!im->open[i]) && (im->alpha[i] != gdAlphaOpaque)) {
				tc++;
			}
		}
		if (tc) {
#if 0
			for (i = 0; (i < im->colorsTotal); i++) {
				trans_values[i] = 255 -
				                  ((im->alpha[i] << 1) + (im->alpha[i] >> 6));
			}
			png_set_tRNS (png_ptr, info_ptr, trans_values, 256, NULL);
#endif
			if (!remap) {
				remap = TRUE;
			}
			/* (Semi-)transparent indexes come up from the bottom
			   of the list of real colors; opaque
			   indexes come down from the top */
			j = 0;
			k = colors - 1;
			for (i = 0; (i < im->colorsTotal); i++) {
				if (!im->open[i]) {
					if (im->alpha[i] != gdAlphaOpaque) {
						/* Andrew Hull: >> 6, not >> 7! (gd 2.0.5) */
						trans_values[j] =
							255 - ((im->alpha[i] << 1) + (im->alpha[i] >> 6));
						mapping[i] = j++;
					} else {
						mapping[i] = k--;
					}
				}
			}
			png_set_tRNS(png_ptr, info_ptr, trans_values, tc, NULL);
		}
	}

	/* convert palette to libpng layout */
	if (!im->trueColor) {
		if (remap)
			for (i = 0; i < im->colorsTotal; ++i) {
				if (mapping[i] < 0)
					continue;
				palette[mapping[i]].red = im->red[i];
				palette[mapping[i]].green = im->green[i];
				palette[mapping[i]].blue = im->blue[i];
			}
		else
			for (i = 0; i < colors; ++i) {
				palette[i].red = im->red[i];
				palette[i].green = im->green[i];
				palette[i].blue = im->blue[i];
			}
		png_set_PLTE(png_ptr, info_ptr, palette, colors);
	}

	/* write out the PNG header info (everything up to first IDAT) */
	png_write_info(png_ptr, info_ptr);

	/* make sure < 8-bit images are packed into pixels as tightly as possible */
	png_set_packing(png_ptr);

	/* This code allocates a set of row buffers and copies the gd image data
	 * into them only in the case that remapping is necessary; in gd 1.3 and
	 * later, the im->pixels array is laid out identically to libpng's row
	 * pointers and can be passed to png_write_image() function directly.
	 * The remapping case could be accomplished with less memory for non-
	 * interlaced images, but interlacing causes some serious complications. */
	if (im->trueColor) {
		/* performance optimizations by Phong Tran */
		int channels = im->saveAlphaFlag ? 4 : 3;
		/* Our little 7-bit alpha channel trick costs us a bit here. */
		unsigned char *pOutputRow;
		int **ptpixels = im->tpixels;
		int *pThisRow;
		unsigned char a;
		int thisPixel;
		png_bytep *prow_pointers;
		int saveAlphaFlag = im->saveAlphaFlag;
		if (overflow2(sizeof(png_bytep), height)) {
			ret = 1;
			goto bail;
		}
		/* Need to use calloc so we can clean it up sanely in the error handler.
		 */
		row_pointers = gdCalloc(height, sizeof(png_bytep));
		if (row_pointers == NULL) {
			gd_error("gd-png error: unable to allocate row_pointers\n");
			ret = 1;
			goto bail;
		}
		prow_pointers = row_pointers;
		for (j = 0; j < height; ++j) {
			if (overflow2(width, channels) ||
				((*prow_pointers = (png_bytep)gdMalloc(width * channels)) ==
				 NULL)) {
				gd_error("gd-png error: unable to allocate rows\n");
				for (i = 0; i < j; ++i)
					gdFree(row_pointers[i]);
				/* 2.0.29: memory leak TBB */
				gdFree(row_pointers);
				ret = 1;
				goto bail;
			}
			pOutputRow = *prow_pointers++;
			pThisRow = *ptpixels++;
			for (i = 0; i < width; ++i) {
				thisPixel = *pThisRow++;
				*pOutputRow++ = gdTrueColorGetRed(thisPixel);
				*pOutputRow++ = gdTrueColorGetGreen(thisPixel);
				*pOutputRow++ = gdTrueColorGetBlue(thisPixel);

				if (saveAlphaFlag) {
					/* convert the 7-bit alpha channel to an 8-bit alpha
					   channel. We do a little bit-flipping magic, repeating the
					   MSB as the LSB, to ensure that 0 maps to 0 and 127 maps
					   to 255. We also have to invert to match PNG's convention
					   in which 255 is opaque. */
					a = gdTrueColorGetAlpha(thisPixel);
					/* Andrew Hull: >> 6, not >> 7! (gd 2.0.5) */
					*pOutputRow++ = 255 - ((a << 1) + (a >> 6));
				}
			}
		}

		png_write_image(png_ptr, row_pointers);
		png_write_end(png_ptr, info_ptr);

		for (j = 0; j < height; ++j)
			gdFree(row_pointers[j]);
		gdFree(row_pointers);
	} else {
		if (remap) {
			png_bytep *row_pointers;
			if (overflow2(sizeof(png_bytep), height)) {
				ret = 1;
				goto bail;
			}
			row_pointers = gdMalloc(sizeof(png_bytep) * height);
			if (row_pointers == NULL) {
				gd_error("gd-png error: unable to allocate row_pointers\n");
				ret = 1;
				goto bail;
			}
			for (j = 0; j < height; ++j) {
				if ((row_pointers[j] = (png_bytep)gdMalloc(width)) == NULL) {
					gd_error("gd-png error: unable to allocate rows\n");
					for (i = 0; i < j; ++i)
						gdFree(row_pointers[i]);
					/* TBB: memory leak */
					gdFree(row_pointers);
					ret = 1;
					goto bail;
				}
				for (i = 0; i < width; ++i)
					row_pointers[j][i] = mapping[im->pixels[j][i]];
			}

			png_write_image(png_ptr, row_pointers);

			for (j = 0; j < height; ++j)
				gdFree(row_pointers[j]);
			gdFree(row_pointers);

			png_write_end(png_ptr, info_ptr);
		} else {
			png_write_image(png_ptr, im->pixels);
			png_write_end(png_ptr, info_ptr);
		}
	}
	/* 1.6.3: maybe we should give that memory BACK! TBB */
bail:
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return ret;
}

#else /* !HAVE_LIBPNG */

static void _noPngError(void) {
	gd_error("PNG image support has been disabled\n");
}

BGD_DECLARE(void) gdPngWriteOptionsInit(gdPngWriteOptions *options) {
	if (options == NULL)
		return;
	memset(options, 0, sizeof(*options));
	options->struct_size = sizeof(*options);
	options->compression_level = -1;
}

BGD_DECLARE(int)
gdImagePngWithOptions(gdImagePtr im, FILE *outFile,
					  const gdPngWriteOptions *options) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outFile);
	ARG_NOT_USED(options);
	_noPngError();
	return 1;
}

BGD_DECLARE(int)
gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile,
						 const gdPngWriteOptions *options) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	ARG_NOT_USED(options);
	_noPngError();
	return 1;
}

BGD_DECLARE(void *)
gdImagePngPtrWithOptions(gdImagePtr im, int *size,
						 const gdPngWriteOptions *options) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(options);
	if (size != NULL)
		*size = 0;
	_noPngError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromPng(FILE *inFile) {
	ARG_NOT_USED(inFile);
	_noPngError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr(int size, void *data) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	return NULL;
}

BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngPtrWithMetadata(int size, void *data,
									gdImageMetadata *metadata) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	ARG_NOT_USED(metadata);
	return NULL;
}

BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngCtxWithMetadata(gdIOCtx *infile,
									gdImageMetadata *metadata) {
	ARG_NOT_USED(infile);
	ARG_NOT_USED(metadata);
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx(gdIOCtx *infile) {
	ARG_NOT_USED(infile);
	return NULL;
}

BGD_DECLARE(void) gdImagePngEx(gdImagePtr im, FILE *outFile, int level) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outFile);
	ARG_NOT_USED(level);
	_noPngError();
}

BGD_DECLARE(void) gdImagePng(gdImagePtr im, FILE *outFile) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outFile);
	_noPngError();
}

BGD_DECLARE(void *) gdImagePngPtr(gdImagePtr im, int *size) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	return NULL;
}

BGD_DECLARE(void *) gdImagePngPtrEx(gdImagePtr im, int *size, int level) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	ARG_NOT_USED(level);
	return NULL;
}

BGD_DECLARE(void *)
gdImagePngPtrWithMetadata(gdImagePtr im, int *size,
						  const gdImageMetadata *metadata) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	ARG_NOT_USED(metadata);
	return NULL;
}

BGD_DECLARE(void *)
gdImagePngPtrExWithMetadata(gdImagePtr im, int *size, int level,
							const gdImageMetadata *metadata) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	ARG_NOT_USED(level);
	ARG_NOT_USED(metadata);
	return NULL;
}

BGD_DECLARE(int)
gdImageMetadataInjectPng(void **data, int *size,
						 const gdImageMetadata *metadata) {
	ARG_NOT_USED(data);
	ARG_NOT_USED(size);
	ARG_NOT_USED(metadata);
	return GD_META_ERR_UNSUPPORTED;
}

BGD_DECLARE(void) gdImagePngCtx(gdImagePtr im, gdIOCtx *outfile) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	_noPngError();
}

BGD_DECLARE(void)
gdImagePngCtxWithMetadata(gdImagePtr im, gdIOCtx *outfile,
						  const gdImageMetadata *metadata) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	ARG_NOT_USED(metadata);
	_noPngError();
}

BGD_DECLARE(void) gdImagePngCtxEx(gdImagePtr im, gdIOCtx *outfile, int level) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	ARG_NOT_USED(level);
	_noPngError();
}

BGD_DECLARE(void)
gdImagePngCtxExWithMetadata(gdImagePtr im, gdIOCtx *outfile, int level,
							const gdImageMetadata *metadata) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	ARG_NOT_USED(level);
	ARG_NOT_USED(metadata);
	_noPngError();
}

#endif /* HAVE_LIBPNG */
