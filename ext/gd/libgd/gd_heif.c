/**
 * File: HEIF IO
 *
 * Read and write HEIF images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBHEIF
#include <libheif/heif.h>

#define GD_HEIF_ALLOC_STEP (4 * 1024)
#define GD_HEIF_HEADER 12

typedef enum gd_heif_brand {
	GD_HEIF_BRAND_AVIF = 1,
	GD_HEIF_BRAND_MIF1 = 2,
	GD_HEIF_BRAND_HEIC = 4,
	GD_HEIF_BRAND_HEIX = 8,
} gd_heif_brand;

/*
  Function: gdImageCreateFromHeif

	<gdImageCreateFromHeif> is called to load truecolor images from
	HEIF format files. Invoke <gdImageCreateFromHeif> with an
	already opened pointer to a file containing the desired
	image. <gdImageCreateFromHeif> returns a <gdImagePtr> to the new
	truecolor image, or NULL if unable to load the image (most often
	because the file is corrupt or does not contain a HEIF
	image). <gdImageCreateFromHeif> does not close the file.

	You can inspect the sx and sy members of the image to determine
	its size. The image must eventually be destroyed using
	<gdImageDestroy>.

	*The returned image is always a truecolor image.*

  Parameters:

	infile - The input FILE pointer.

  Returns:

	A pointer to the new *truecolor* image.  This will need to be
	destroyed with <gdImageDestroy> once it is no longer needed.

	On error, returns NULL.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeif(FILE *inFile) {
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);

	if (!in)
		return NULL;
	im = gdImageCreateFromHeifCtx(in);
	in->gd_free(in);

	return im;
}

/*
  Function: gdImageCreateFromHeifPtr

	See <gdImageCreateFromHeif>.

  Parameters:

	size            - size of HEIF data in bytes.
	data            - pointer to HEIF data.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifPtr(int size, void *data) {
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);

	if (!in)
		return NULL;
	im = gdImageCreateFromHeifCtx(in);
	in->gd_free(in);

	return im;
}

static int _gdHeifCheckBrand(unsigned char *magic,
							 gd_heif_brand expected_brand) {
	if (memcmp(magic + 4, "ftyp", 4) != 0)
		return GD_FALSE;
	if (memcmp(magic + 8, "avif", 4) == 0 &&
		expected_brand & GD_HEIF_BRAND_AVIF)
		return GD_TRUE;
	if (memcmp(magic + 8, "heic", 4) == 0 &&
		expected_brand & GD_HEIF_BRAND_HEIC)
		return GD_TRUE;
	if (memcmp(magic + 8, "heix", 4) == 0 &&
		expected_brand & GD_HEIF_BRAND_HEIX)
		return GD_TRUE;
	if (memcmp(magic + 8, "mif1", 4) == 0 &&
		expected_brand & GD_HEIF_BRAND_MIF1)
		return GD_TRUE;

	return GD_FALSE;
}

static gdImagePtr _gdImageCreateFromHeifCtx(gdIOCtx *infile,
											gd_heif_brand expected_brand) {
	struct heif_context *heif_ctx;
	struct heif_decoding_options *heif_dec_opts;
	struct heif_image_handle *heif_imhandle;
	struct heif_image *heif_im;
	struct heif_error err;
	int width, height;
	uint8_t *filedata = NULL;
	uint8_t *rgba = NULL;
	unsigned char *read, *temp, magic[GD_HEIF_HEADER];
	int magic_len;
	size_t size = 0, n = GD_HEIF_ALLOC_STEP;
	gdImagePtr im;
	int x, y;
	uint8_t *p, *row_start;
	int stride;

	magic_len = gdGetBuf(magic, GD_HEIF_HEADER, infile);
	if (magic_len != GD_HEIF_HEADER ||
		!_gdHeifCheckBrand(magic, expected_brand)) {
		gd_error("gd-heif incorrect type of file\n");
		return NULL;
	}
	gdSeek(infile, 0);

	while (n == GD_HEIF_ALLOC_STEP) {
		temp = gdRealloc(filedata, size + GD_HEIF_ALLOC_STEP);
		if (temp) {
			filedata = temp;
			read = temp + size;
		} else {
			gdFree(filedata);
			gd_error("gd-heif decode realloc failed\n");
			return NULL;
		}

		n = gdGetBuf(read, GD_HEIF_ALLOC_STEP, infile);
		if (n > 0) {
			size += n;
		}
	}

	heif_ctx = heif_context_alloc();
	if (heif_ctx == NULL) {
		gd_error("gd-heif could not allocate context\n");
		gdFree(filedata);
		return NULL;
	}
	err = heif_context_read_from_memory_without_copy(heif_ctx, filedata, size,
													 NULL);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif context creation failed\n");
		gdFree(filedata);
		heif_context_free(heif_ctx);
		return NULL;
	}

	heif_imhandle = NULL;
	err = heif_context_get_primary_image_handle(heif_ctx, &heif_imhandle);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif cannot retreive handle\n");
		gdFree(filedata);
		heif_context_free(heif_ctx);
		return NULL;
	}

	heif_im = NULL;
	heif_dec_opts = heif_decoding_options_alloc();
	if (heif_dec_opts == NULL) {
		gd_error("gd-heif could not allocate decode options\n");
		gdFree(filedata);
		heif_image_handle_release(heif_imhandle);
		heif_context_free(heif_ctx);
		return NULL;
	}

	heif_dec_opts->convert_hdr_to_8bit = GD_TRUE;
	heif_dec_opts->ignore_transformations = GD_TRUE;
	err = heif_decode_image(heif_imhandle, &heif_im, heif_colorspace_RGB,
							heif_chroma_interleaved_RGBA, heif_dec_opts);
	heif_decoding_options_free(heif_dec_opts);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif decoding failed\n");
		gdFree(filedata);
		heif_image_handle_release(heif_imhandle);
		heif_context_free(heif_ctx);
		return NULL;
	}

	width = heif_image_get_width(heif_im, heif_channel_interleaved);
	height = heif_image_get_height(heif_im, heif_channel_interleaved);

	im = gdImageCreateTrueColor(width, height);
	if (!im) {
		gdFree(filedata);
		heif_image_release(heif_im);
		heif_image_handle_release(heif_imhandle);
		heif_context_free(heif_ctx);
		return NULL;
	}
	rgba = (uint8_t *)heif_image_get_plane_readonly(
		heif_im, heif_channel_interleaved, &stride);
	if (!rgba) {
		gd_error("gd-heif cannot get image plane\n");
		gdFree(filedata);
		heif_image_release(heif_im);
		heif_image_handle_release(heif_imhandle);
		heif_context_free(heif_ctx);
		gdImageDestroy(im);
		return NULL;
	}
	row_start = rgba;
	for (y = 0, p = rgba; y < height; y++) {
		p = row_start;
		for (x = 0; x < width; x++) {
			uint8_t r = *(p++);
			uint8_t g = *(p++);
			uint8_t b = *(p++);
			uint8_t a = gdAlphaMax - (*(p++) >> 1);
			im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, a);
		}
		row_start += stride;
	}
	gdFree(filedata);
	heif_image_release(heif_im);
	heif_image_handle_release(heif_imhandle);
	heif_context_free(heif_ctx);

	return im;
}

/*
  Function: gdImageCreateFromHeifCtx

	See <gdImageCreateFromHeif>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifCtx(gdIOCtx *infile) {
	return _gdImageCreateFromHeifCtx(
		infile, GD_HEIF_BRAND_AVIF | GD_HEIF_BRAND_MIF1 | GD_HEIF_BRAND_HEIC |
					GD_HEIF_BRAND_HEIX);
}

static struct heif_error _gdImageWriteHeif(struct heif_context *heif_ctx,
										   const void *data, size_t size,
										   void *userdata) {
	ARG_NOT_USED(heif_ctx);
	gdIOCtx *outfile;
	struct heif_error err;
	int bytes_written;

	outfile = (gdIOCtx *)userdata;
	if (outfile == NULL || data == NULL || size > INT_MAX) {
		err.code = heif_error_Encoding_error;
		err.subcode = heif_suberror_Cannot_write_output_data;
		err.message = "gd-heif write callback received invalid arguments";
		return err;
	}

	bytes_written = gdPutBuf(data, (int)size, outfile);
	if (bytes_written != (int)size) {
		err.code = heif_error_Encoding_error;
		err.subcode = heif_suberror_Cannot_write_output_data;
		err.message = "gd-heif failed to write output data";
		return err;
	}

	err.code = heif_error_Ok;
	err.subcode = heif_suberror_Unspecified;
	err.message = "";

	return err;
}

/* returns GD_TRUE on success, GD_FALSE on failure */
static int _gdImageHeifCtx(gdImagePtr im, gdIOCtx *outfile, int quality,
						   gdHeifCodec codec, gdHeifChroma chroma) {
	struct heif_context *heif_ctx;
	struct heif_encoder *heif_enc;
	struct heif_image *heif_im;
	struct heif_writer heif_wr;
	struct heif_error err;
	uint8_t *rgba;
	int x, y;
	uint8_t *p;
	uint8_t *row_start;
	int stride;
	if (im == NULL) {
		return GD_FALSE;
	}

	if (codec != GD_HEIF_CODEC_HEVC && codec != GD_HEIF_CODEC_AV1) {
		gd_error("Unsupported format by heif");
		return GD_FALSE;
	}

	if (!gdImageTrueColor(im)) {
		gd_error("Palette image not supported by heif\n");
		return GD_FALSE;
	}

	if (overflow2(gdImageSX(im), 4)) {
		return GD_FALSE;
	}

	if (overflow2(gdImageSX(im) * 4, gdImageSY(im))) {
		return GD_FALSE;
	}

	heif_ctx = heif_context_alloc();
	if (heif_ctx == NULL) {
		gd_error("gd-heif could not allocate context\n");
		return GD_FALSE;
	}
	err = heif_context_get_encoder_for_format(
		heif_ctx, (enum heif_compression_format)codec, &heif_enc);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif encoder acquisition failed (missing codec support?: "
				 "code: %d, subcode: %d, message: %s)\n",
				 err.code, err.subcode, err.message);
		heif_context_free(heif_ctx);
		return GD_FALSE;
	}

	if (quality == 200) {
		err = heif_encoder_set_lossless(heif_enc, GD_TRUE);
	} else if (quality == -1) {
		err = heif_encoder_set_lossy_quality(heif_enc, 80);
	} else {
		err = heif_encoder_set_lossy_quality(heif_enc, quality);
	}
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif invalid quality number\n");
		heif_encoder_release(heif_enc);
		heif_context_free(heif_ctx);
		return GD_FALSE;
	}

	if (heif_get_version_number_major() >= 1 &&
		heif_get_version_number_minor() >= 9) {
		err = heif_encoder_set_parameter_string(heif_enc, "chroma", chroma);
		if (err.code != heif_error_Ok) {
			gd_error("gd-heif invalid chroma subsampling parameter\n");
			heif_encoder_release(heif_enc);
			heif_context_free(heif_ctx);
			return GD_FALSE;
		}
	}

	err = heif_image_create(gdImageSX(im), gdImageSY(im), heif_colorspace_RGB,
							heif_chroma_interleaved_RGBA, &heif_im);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif image creation failed");
		heif_encoder_release(heif_enc);
		heif_context_free(heif_ctx);
		return GD_FALSE;
	}

	err = heif_image_add_plane(heif_im, heif_channel_interleaved, gdImageSX(im),
							   gdImageSY(im), 32);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif cannot add image plane\n");
		heif_image_release(heif_im);
		heif_encoder_release(heif_enc);
		heif_context_free(heif_ctx);
		return GD_FALSE;
	}

	rgba = (uint8_t *)heif_image_get_plane_readonly(
		heif_im, heif_channel_interleaved, &stride);
	if (!rgba) {
		gd_error("gd-heif cannot get image plane\n");
		heif_image_release(heif_im);
		heif_encoder_release(heif_enc);
		heif_context_free(heif_ctx);
		return GD_FALSE;
	}
	row_start = rgba;
	for (y = 0; y < gdImageSY(im); y++) {
		p = row_start;
		for (x = 0; x < gdImageSX(im); x++) {
			int c;
			char a;
			c = im->tpixels[y][x];
			a = gdTrueColorGetAlpha(c);
			if (a == 127) {
				a = 0;
			} else {
				a = 255 - ((a << 1) + (a >> 6));
			}
			*(p++) = gdTrueColorGetRed(c);
			*(p++) = gdTrueColorGetGreen(c);
			*(p++) = gdTrueColorGetBlue(c);
			*(p++) = a;
		}
		row_start += stride;
	}
	err = heif_context_encode_image(heif_ctx, heif_im, heif_enc, NULL, NULL);
	heif_encoder_release(heif_enc);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif encoding failed\n");
		heif_image_release(heif_im);
		heif_context_free(heif_ctx);
		return GD_FALSE;
	}
	heif_wr.write = _gdImageWriteHeif;
	heif_wr.writer_api_version = 1;
	err = heif_context_write(heif_ctx, &heif_wr, (void *)outfile);

	heif_image_release(heif_im);
	heif_context_free(heif_ctx);
	if (err.code != heif_error_Ok) {
		gd_error("gd-heif write failed (code: %d, subcode: %d, message: %s)\n",
				 err.code, err.subcode, err.message);
		return GD_FALSE;
	}

	return GD_TRUE;
}

/*
  Function: gdImageHeifCtx

	Write the image as HEIF data via a <gdIOCtx>. See <gdImageHeifEx>
	for more details.

  Parameters:

	im          - The image to write.
	outfile     - The output sink.
	quality     - Image quality.
	codec       - The output coding format.
	chroma      - The output chroma subsampling format.

  Returns:

	Nothing.
*/
BGD_DECLARE(void)
gdImageHeifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, gdHeifCodec codec,
			   gdHeifChroma chroma) {
	_gdImageHeifCtx(im, outfile, quality, codec, chroma);
}

/*
  Function: gdImageHeifEx

	<gdImageHeifEx> outputs the specified image to the specified file in
	HEIF format. The file must be open for writing. Under MSDOS and
	all versions of Windows, it is important to use "wb" as opposed to
	simply "w" as the mode when opening the file, and under Unix there
	is no penalty for doing so. <gdImageHeifEx> does not close the file;
	your code must do so.

	If _quality_ is -1, a reasonable quality value (which should yield a
	good general quality / size tradeoff for most situations) is used. Otherwise
	_quality_ should be a value in the range 0-100, higher quality values
	usually implying both higher quality and larger image sizes or 200, for
	lossless codec.

	For _codec_, the default and most widely supported option is
	GD_HEIF_CODEC_HEVC. GD_HEIF_CODEC_AV1 is a newer codec that may not be
	supported by all decoders but can offer better compression efficiency.
	They must be installed on the system and enabled at compile time to be used.

  Variants:

	<gdImageHeifCtx> stores the image using a <gdIOCtx> struct.

	<gdImageHeifPtrEx> stores the image to RAM.

  Parameters:

	im          - The image to save.
	outFile     - The FILE pointer to write to.
	quality     - Codec quality (0-100).
	codec       - The output coding format.
	chroma      - The output chroma subsampling format.

  Returns:

	Nothing.
*/
BGD_DECLARE(void)
gdImageHeifEx(gdImagePtr im, FILE *outFile, int quality, gdHeifCodec codec,
			  gdHeifChroma chroma) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageHeifCtx(im, out, quality, codec, chroma);
	out->gd_free(out);
}

/*
  Function: gdImageHeif

	Variant of <gdImageHeifEx> which uses the default quality (-1), the
	default codec (GD_HEIF_Codec_HEVC) and the default chroma
	subsampling (GD_HEIF_CHROMA_444).

  Parameters:

	im      - The image to save
	outFile - The FILE pointer to write to.

  Returns:

	Nothing.
*/
BGD_DECLARE(void) gdImageHeif(gdImagePtr im, FILE *outFile) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageHeifCtx(im, out, -1, GD_HEIF_CODEC_HEVC, GD_HEIF_CHROMA_444);
	out->gd_free(out);
}

/*
  Function: gdImageHeifPtr

	See <gdImageHeifEx>.
*/
BGD_DECLARE(void *) gdImageHeifPtr(gdImagePtr im, int *size) {
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageHeifCtx(im, out, -1, GD_HEIF_CODEC_HEVC, GD_HEIF_CHROMA_444)) {
		rv = gdDPExtractData(out, size);
	} else {
		rv = NULL;
	}
	out->gd_free(out);

	return rv;
}

/*
  Function: gdImageHeifPtrEx

	See <gdImageHeifEx>.
*/
BGD_DECLARE(void *)
gdImageHeifPtrEx(gdImagePtr im, int *size, int quality, gdHeifCodec codec,
				 gdHeifChroma chroma) {
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageHeifCtx(im, out, quality, codec, chroma)) {
		rv = gdDPExtractData(out, size);
	} else {
		rv = NULL;
	}
	out->gd_free(out);
	return rv;
}

#else /* HAVE_LIBHEIF */

static void _noHeifError(void) {
	gd_error("HEIF image support has been disabled\n");
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeif(FILE *inFile) {
	_noHeifError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifPtr(int size, void *data) {
	_noHeifError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifCtx(gdIOCtx *infile) {
	_noHeifError();
	return NULL;
}

BGD_DECLARE(void)
gdImageHeifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, gdHeifCodec codec,
			   gdHeifChroma chroma) {
	_noHeifError();
}

BGD_DECLARE(void)
gdImageHeifEx(gdImagePtr im, FILE *outFile, int quality, gdHeifCodec codec,
			  gdHeifChroma chroma) {
	_noHeifError();
}

BGD_DECLARE(void) gdImageHeif(gdImagePtr im, FILE *outFile) { _noHeifError(); }

BGD_DECLARE(void *) gdImageHeifPtr(gdImagePtr im, int *size) {
	_noHeifError();
	return NULL;
}

BGD_DECLARE(void *)
gdImageHeifPtrEx(gdImagePtr im, int *size, int quality, gdHeifCodec codec,
				 gdHeifChroma chroma) {
	_noHeifError();
	return NULL;
}

#endif /* HAVE_LIBHEIF */
