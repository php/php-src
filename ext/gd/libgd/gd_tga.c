/**
 * File: TGA Input
 *
 * Read TGA images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "gd_tga.h"
#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"

/*
	Function: gdImageCreateFromTga

	Creates a gdImage from a TGA file

	Parameters:

		infile - Pointer to TGA binary file
 */
gdImagePtr gdImageCreateFromTga(FILE *fp)
{
	gdImagePtr image;
	gdIOCtx* in = gdNewFileCtx(fp);
	if (in == NULL) return NULL;
	image = gdImageCreateFromTgaCtx(in);
	in->gd_free( in );
	return image;
}

/*
	Function: gdImageCreateFromTgaPtr
*/
gdImagePtr gdImageCreateFromTgaPtr(int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx (size, data, 0);
	if (in == NULL) return NULL;
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
gdImagePtr gdImageCreateFromTgaCtx(gdIOCtx* ctx)
{
	int bitmap_caret = 0;
	oTga *tga = NULL;
	/*	int pixel_block_size = 0;
		int image_block_size = 0; */
	volatile gdImagePtr image = NULL;
	int x = 0;
	int y = 0;

	tga = (oTga *) gdMalloc(sizeof(oTga));
	if (!tga) {
		return NULL;
	}

	tga->bitmap = NULL;
	tga->ident = NULL;

	if (read_header_tga(ctx, tga) < 0) {
		free_tga(tga);
		return NULL;
	}

	/*TODO: Will this be used?
		pixel_block_size = tga->bits / 8;
		image_block_size = (tga->width * tga->height) * pixel_block_size;
	*/

	if (read_image_tga(ctx, tga) < 0) {
		free_tga(tga);
		return NULL;
	}

	image = gdImageCreateTrueColor((int)tga->width, (int)tga->height );

	if (image == 0) {
		free_tga( tga );
		return NULL;
	}

	/*!	\brief Populate GD image object
	 *  Copy the pixel data from our tga bitmap buffer into the GD image
	 *  Disable blending and save the alpha channel per default
	 */
	if (tga->alphabits) {
		gdImageAlphaBlending(image, 0);
		gdImageSaveAlpha(image, 1);
	}

	/* TODO: use alphabits as soon as we support 24bit and other alpha bps (ie != 8bits) */
	for (y = 0; y < tga->height; y++) {
		register int *tpix = image->tpixels[y];
		for ( x = 0; x < tga->width; x++, tpix++) {
			if (tga->bits == TGA_BPP_24) {
				*tpix = gdTrueColor(tga->bitmap[bitmap_caret + 2], tga->bitmap[bitmap_caret + 1], tga->bitmap[bitmap_caret]);
				bitmap_caret += 3;
			} else if (tga->bits == TGA_BPP_32 && tga->alphabits) {
				register int a = tga->bitmap[bitmap_caret + 3];

				*tpix = gdTrueColorAlpha(tga->bitmap[bitmap_caret + 2], tga->bitmap[bitmap_caret + 1], tga->bitmap[bitmap_caret], gdAlphaMax - (a >> 1));
				bitmap_caret += 4;
			}
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
 *	Reads the header block from a binary TGA file populating the referenced TGA structure.
 *	\param ctx Pointer to TGA binary file
 *	\param tga Pointer to TGA structure
 *	\return int 1 on sucess, -1 on failure
 */
int read_header_tga(gdIOCtx *ctx, oTga *tga)
{

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

#if DEBUG
	printf("format bps: %i\n", tga->bits);
	printf("flip h/v: %i / %i\n", tga->fliph, tga->flipv);
	printf("alpha: %i\n", tga->alphabits);
	printf("wxh: %i %i\n", tga->width, tga->height);
#endif

	if (!((tga->bits == TGA_BPP_24 && tga->alphabits == 0)
		|| (tga->bits == TGA_BPP_32 && tga->alphabits == 8)))
	{
		gd_error_ex(GD_WARNING, "gd-tga: %u bits per pixel with %u alpha bits not supported\n",
			tga->bits, tga->alphabits);
		return -1;
	}

	tga->ident = NULL;

	if (tga->identsize > 0) {
		tga->ident = (char *) gdMalloc(tga->identsize * sizeof(char));
		if(tga->ident == NULL) {
			return -1;
		}

		gdGetBuf(tga->ident, tga->identsize, ctx);
	}

	return 1;
}

/*!	\brief Reads a TGA image data into buffer.
 *	Reads the image data block from a binary TGA file populating the referenced TGA structure.
 *	\param ctx Pointer to TGA binary file
 *	\param tga Pointer to TGA structure
 *	\return int 0 on sucess, -1 on failure
 */
int read_image_tga( gdIOCtx *ctx, oTga *tga )
{
	int pixel_block_size = (tga->bits / 8);
	int image_block_size;
	int* decompression_buffer = NULL;
	unsigned char* conversion_buffer = NULL;
	int buffer_caret = 0;
	int bitmap_caret = 0;
	int i = 0;
	int encoded_pixels;
	int rle_size;

	if(overflow2(tga->width, tga->height)) {
		return -1;
	}

	if(overflow2(tga->width * tga->height, pixel_block_size)) {
		return -1;
	}

	image_block_size = (tga->width * tga->height) * pixel_block_size;
	if(overflow2(image_block_size, sizeof(int))) {
		return -1;
	}

	/*! \todo Add more image type support.
	 */
	if (tga->imagetype != TGA_TYPE_RGB && tga->imagetype != TGA_TYPE_RGB_RLE)
		return -1;

	/*!	\brief Allocate memmory for image block
	 *  Allocate a chunk of memory for the image block to be passed into.
	 */
	tga->bitmap = (int *) gdMalloc(image_block_size * sizeof(int));
	if (tga->bitmap == NULL)
		return -1;

	switch (tga->imagetype) {
	case TGA_TYPE_RGB:
		/*! \brief Read in uncompressed RGB TGA
		 *  Chunk load the pixel data from an uncompressed RGB type TGA.
		 */
		conversion_buffer = (unsigned char *) gdMalloc(image_block_size * sizeof(unsigned char));
		if (conversion_buffer == NULL) {
			return -1;
		}

		if (gdGetBuf(conversion_buffer, image_block_size, ctx) != image_block_size) {
			gd_error("gd-tga: premature end of image data\n");
			gdFree(conversion_buffer);
			return -1;
		}

		while (buffer_caret < image_block_size) {
			tga->bitmap[buffer_caret] = (int) conversion_buffer[buffer_caret];
			buffer_caret++;
		}

		gdFree(conversion_buffer);
		break;

	case TGA_TYPE_RGB_RLE:
		/*! \brief Read in RLE compressed RGB TGA
		 *  Chunk load the pixel data from an RLE compressed RGB type TGA.
		 */
		decompression_buffer = (int*) gdMalloc(image_block_size * sizeof(int));
		if (decompression_buffer == NULL) {
			return -1;
		}
		conversion_buffer = (unsigned char *) gdMalloc(image_block_size * sizeof(unsigned char));
		if (conversion_buffer == NULL) {
			gd_error("gd-tga: premature end of image data\n");
			gdFree( decompression_buffer );
			return -1;
		}

		rle_size = gdGetBuf(conversion_buffer, image_block_size, ctx);
		if (rle_size <= 0) {
			gdFree(conversion_buffer);
			gdFree(decompression_buffer);
			return -1;
		}

		buffer_caret = 0;

		while( buffer_caret < rle_size) {
			decompression_buffer[buffer_caret] = (int)conversion_buffer[buffer_caret];
			buffer_caret++;
		}

		buffer_caret = 0;

		while( bitmap_caret < image_block_size ) {

			if (buffer_caret + pixel_block_size > rle_size) {
				gdFree( decompression_buffer );
				gdFree( conversion_buffer );
				return -1;
			}

			if ((decompression_buffer[buffer_caret] & TGA_RLE_FLAG) == TGA_RLE_FLAG) {
				encoded_pixels = ( ( decompression_buffer[ buffer_caret ] & ~TGA_RLE_FLAG ) + 1 );
				buffer_caret++;

				if ((bitmap_caret + (encoded_pixels * pixel_block_size)) > image_block_size
						|| buffer_caret + pixel_block_size > rle_size) {
					gdFree( decompression_buffer );
					gdFree( conversion_buffer );
					return -1;
				}

				for (i = 0; i < encoded_pixels; i++) {
					memcpy(tga->bitmap + bitmap_caret, decompression_buffer + buffer_caret, pixel_block_size * sizeof(int));
					bitmap_caret += pixel_block_size;
				}
				buffer_caret += pixel_block_size;

			} else {
				encoded_pixels = decompression_buffer[ buffer_caret ] + 1;
				buffer_caret++;

				if ((bitmap_caret + (encoded_pixels * pixel_block_size)) > image_block_size
						|| buffer_caret + (encoded_pixels * pixel_block_size) > rle_size) {
					gdFree( decompression_buffer );
					gdFree( conversion_buffer );
					return -1;
				}

				memcpy(tga->bitmap + bitmap_caret, decompression_buffer + buffer_caret, encoded_pixels * pixel_block_size * sizeof(int));
				bitmap_caret += (encoded_pixels * pixel_block_size);
				buffer_caret += (encoded_pixels * pixel_block_size);
			}
		}
		gdFree( decompression_buffer );
		gdFree( conversion_buffer );
		break;
	}

	return 1;
}

/*!	\brief Cleans up a TGA structure.
 *	Dereferences the bitmap referenced in a TGA structure, then the structure itself
 *	\param tga Pointer to TGA structure
 */
void free_tga(oTga * tga)
{
	if (tga) {
		if (tga->ident)
			gdFree(tga->ident);
		if (tga->bitmap)
			gdFree(tga->bitmap);
		gdFree(tga);
	}
}
