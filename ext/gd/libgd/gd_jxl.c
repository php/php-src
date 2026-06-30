/**
 * File: JPEG XL IO
 *
 * Read and write JPEG XL images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include "gdhelpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBJXL
#include <jxl/cms.h>
#include <jxl/color_encoding.h>
#include <jxl/decode.h>
#include <jxl/encode.h>

#define GD_JXL_ALLOC_STEP (4 * 1024)

/* ---- Internal helpers ---- */

/* Slurp gdIOCtx into dynamic buffer (pattern from gd_webp.c) */
static uint8_t *JxlReadCtxData(gdIOCtx *infile, size_t *size) {
	uint8_t *filedata = NULL, *temp, *read;
	ssize_t n;

	*size = 0;
	do {
		temp = gdRealloc(filedata, *size + GD_JXL_ALLOC_STEP);
		if (temp == NULL) {
			gdFree(filedata);
			gd_error("JXL decode: realloc failed");
			return NULL;
		}
		filedata = temp;
		read = temp + *size;
		n = gdGetBuf(read, GD_JXL_ALLOC_STEP, infile);
		if (n > 0 && n != EOF) {
			*size += n;
		}
	} while (n > 0 && n != EOF);

	if (*size == 0) {
		gdFree(filedata);
		return NULL;
	}

	return filedata;
}

/* JXL alpha (0-255) -> gd alpha (0-127, 0=opaque) */
static int JxlAlphaJxlToGd(uint8_t jxl_alpha) {
	if (jxl_alpha == 0) {
		return gdAlphaTransparent;
	}
	return gdAlphaMax - (jxl_alpha >> 1);
}

/* gd alpha (0-127) -> JXL alpha (0-255) */
static uint8_t JxlAlphaGdToJxl(int gd_alpha) {
	if (gd_alpha == gdAlphaTransparent) {
		return 0;
	}
	return (uint8_t)((gdAlphaMax - gd_alpha) << 1);
}

static int JxlImageHasAlpha(gdImagePtr im) {
	int x, y;

	if (im == NULL) {
		return 0;
	}

	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			if (gdTrueColorGetAlpha(im->tpixels[y][x]) != gdAlphaOpaque) {
				return 1;
			}
		}
	}

	return 0;
}

static int JxlDurationToMs(uint32_t duration,
						   const JxlAnimationHeader *animation) {
	if (animation == NULL || animation->tps_numerator == 0) {
		return 0;
	}

	return (int)((uint64_t)duration * 1000 * animation->tps_denominator /
				 animation->tps_numerator);
}

/* Build gdImagePtr from RGBA u8 buffer */
static gdImagePtr JxlImageFromRGBA(const uint8_t *rgba, int width, int height,
								   int has_alpha) {
	gdImagePtr im;
	const uint8_t *p;
	int x, y;

	if (rgba == NULL || width <= 0 || height <= 0) {
		return NULL;
	}
	im = gdImageCreateTrueColor(width, height);
	if (im == NULL) {
		return NULL;
	}
	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, has_alpha);
	for (y = 0, p = rgba; y < height; y++) {
		for (x = 0; x < width; x++) {
			uint8_t r = *(p++);
			uint8_t g = *(p++);
			uint8_t b = *(p++);
			uint8_t a = *(p++);
			im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, JxlAlphaJxlToGd(a));
		}
	}
	return im;
}

/* Extract RGBA u8 buffer from gdImagePtr */
static int JxlImageToRGBA(gdImagePtr im, uint8_t **rgba, int *has_alpha) {
	uint8_t *p;
	int x, y;
	int w, h;

	*rgba = NULL;
	*has_alpha = 0;

	if (im == NULL) {
		return 0;
	}

	/* Convert palette to truecolor if needed */
	if (!gdImageTrueColor(im)) {
		gdImagePaletteToTrueColor(im);
	}

	w = gdImageSX(im);
	h = gdImageSY(im);

	/* Check for alpha presence */
	if (gdImageGetTransparent(im) != -1 || JxlImageHasAlpha(im)) {
		*has_alpha = 1;
	}

	/* Overflow checks */
	if (overflow2(w, 4)) {
		return 0;
	}
	if (overflow2(w * 4, h)) {
		return 0;
	}

	*rgba = (uint8_t *)gdMalloc((size_t)w * 4 * (size_t)h);
	if (*rgba == NULL) {
		return 0;
	}

	p = *rgba;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			int c = im->tpixels[y][x];
			*(p++) = gdTrueColorGetRed(c);
			*(p++) = gdTrueColorGetGreen(c);
			*(p++) = gdTrueColorGetBlue(c);
			*(p++) = JxlAlphaGdToJxl(gdTrueColorGetAlpha(c));
		}
	}
	return 1;
}

static uint8_t *JxlRGBFromRGBA(const uint8_t *rgba, int width, int height) {
	uint8_t *rgb, *dst;
	const uint8_t *src;
	int x, y;

	if (overflow2(width, 3) || overflow2(width * 3, height)) {
		return NULL;
	}

	rgb = (uint8_t *)gdMalloc((size_t)width * 3 * (size_t)height);
	if (rgb == NULL) {
		return NULL;
	}

	src = rgba;
	dst = rgb;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			*(dst++) = *(src++);
			*(dst++) = *(src++);
			*(dst++) = *(src++);
			src++;
		}
	}

	return rgb;
}

/* ---- Still-image decode ---- */

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxl(FILE *inFile) {
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (!in) {
		return 0;
	}
	im = gdImageCreateFromJxlCtx(in);
	in->gd_free(in);
	return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlPtr(int size, void *data) {
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in) {
		return 0;
	}
	im = gdImageCreateFromJxlCtx(in);
	in->gd_free(in);
	return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlCtx(gdIOCtxPtr inCtx) {
	size_t buf_len = 0;
	uint8_t *buf = NULL;
	JxlDecoder *dec = NULL;
	JxlBasicInfo info = {0};
	uint8_t *pixels = NULL;
	gdImagePtr im = NULL;
	int has_alpha = 0;
	int have_basic_info = 0;

	if (inCtx == NULL) {
		return NULL;
	}

	buf = JxlReadCtxData(inCtx, &buf_len);
	if (buf == NULL) {
		return NULL;
	}

	dec = JxlDecoderCreate(NULL);
	if (dec == NULL) {
		gd_error("gdImageCreateFromJxl: JxlDecoderCreate failed");
		gdFree(buf);
		return NULL;
	}

	JxlDecoderSubscribeEvents(dec, JXL_DEC_BASIC_INFO | JXL_DEC_COLOR_ENCODING |
									   JXL_DEC_FULL_IMAGE);

	/* Attach CMS for color space conversion to sRGB */
	JxlDecoderSetCms(dec, *JxlGetDefaultCms());

	/* Set desired output color profile (sRGB) */
	JxlColorEncoding srgb_enc;
	JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
	JxlDecoderSetPreferredColorProfile(dec, &srgb_enc);

	JxlDecoderSetInput(dec, buf, buf_len);
	JxlDecoderCloseInput(dec);

	for (;;) {
		JxlDecoderStatus status = JxlDecoderProcessInput(dec);

		if (status == JXL_DEC_BASIC_INFO) {
			status = JxlDecoderGetBasicInfo(dec, &info);
			if (status != JXL_DEC_SUCCESS) {
				gd_error("gdImageCreateFromJxl: failed to get basic info");
				goto decode_fail;
			}
			have_basic_info = 1;
			has_alpha = (info.alpha_bits > 0);
		} else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
			JxlPixelFormat fmt = {.num_channels = 4, /* RGBA */
								  .data_type = JXL_TYPE_UINT8,
								  .endianness = JXL_NATIVE_ENDIAN,
								  .align = 0};
			size_t pixels_size;

			if (!have_basic_info) {
				gd_error("gdImageCreateFromJxl: missing basic info");
				goto decode_fail;
			}
			if (overflow2((int)info.xsize, (int)info.ysize) ||
				overflow2((int)info.xsize * (int)info.ysize, 4)) {
				gd_error("gdImageCreateFromJxl: image dimensions overflow");
				goto decode_fail;
			}

			pixels_size = (size_t)info.xsize * (size_t)info.ysize * 4;
			pixels = (uint8_t *)gdMalloc(pixels_size);
			if (pixels == NULL) {
				gd_error(
					"gdImageCreateFromJxl: pixel buffer allocation failed");
				goto decode_fail;
			}

			if (JxlDecoderSetImageOutBuffer(dec, &fmt, pixels,
											pixels_size) != JXL_DEC_SUCCESS) {
				gd_error("gdImageCreateFromJxl: failed to set output buffer");
				goto decode_fail;
			}
		} else if (status == JXL_DEC_FULL_IMAGE) {
			if (!have_basic_info || pixels == NULL) {
				gd_error("gdImageCreateFromJxl: incomplete decoded image");
				goto decode_fail;
			}
			break; /* pixels buffer is populated */
		} else if (status == JXL_DEC_SUCCESS || status == JXL_DEC_ERROR) {
			gd_error("gdImageCreateFromJxl: decoder error");
			goto decode_fail;
		}
	}

	/* Build gdImagePtr from RGBA buffer */
	im = JxlImageFromRGBA(pixels, (int)info.xsize, (int)info.ysize, has_alpha);
	if (!im) {
		gd_error("gdImageCreateFromJxl: JxlImageFromRGBA failed");
		goto decode_fail;
	}

	/* Success path - pixels consumed by im */
	gdFree(pixels);
	pixels = NULL;
	gdFree(buf);
	buf = NULL;
	JxlDecoderDestroy(dec);
	return im;

decode_fail:
	if (pixels)
		gdFree(pixels);
	if (buf)
		gdFree(buf);
	if (dec)
		JxlDecoderDestroy(dec);
	return NULL;
}

/* ---- Still-image encode ---- */

static int _gdImageJxlCtxEx(gdImagePtr im, gdIOCtx *outfile, int lossless,
							float distance, int effort) {
	JxlEncoder *enc = NULL;
	JxlEncoderFrameSettings *frame_opts = NULL;
	uint8_t *pixels = NULL;
	int has_alpha = 0;
	uint8_t outbuf[65536];
	int w, h;
	int ret = 1;

	if (im == NULL) {
		return 1;
	}

	w = gdImageSX(im);
	h = gdImageSY(im);

	/* Extract RGBA pixels */
	if (!JxlImageToRGBA(im, &pixels, &has_alpha)) {
		gd_error("gdImageJxl: pixel extraction failed");
		return 1;
	}

	enc = JxlEncoderCreate(NULL);
	if (enc == NULL) {
		gd_error("gdImageJxl: JxlEncoderCreate failed");
		gdFree(pixels);
		return 1;
	}

	/* Set basic info */
	JxlBasicInfo info;
	JxlEncoderInitBasicInfo(&info);
	info.xsize = w;
	info.ysize = h;
	info.bits_per_sample = 8;
	info.exponent_bits_per_sample = 0;
	info.num_color_channels = 3;
	info.num_extra_channels = has_alpha ? 1 : 0;
	info.alpha_bits = has_alpha ? 8 : 0;
	info.alpha_exponent_bits = 0;
	info.alpha_premultiplied = JXL_FALSE;
	info.uses_original_profile = lossless ? JXL_TRUE : JXL_FALSE;

	if (JxlEncoderSetBasicInfo(enc, &info) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxl: JxlEncoderSetBasicInfo failed");
		goto encode_fail;
	}

	/* Set color encoding (sRGB) */
	JxlColorEncoding srgb_enc;
	JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
	if (JxlEncoderSetColorEncoding(enc, &srgb_enc) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxl: JxlEncoderSetColorEncoding failed");
		goto encode_fail;
	}

	/* Configure frame options */
	frame_opts = JxlEncoderFrameSettingsCreate(enc, NULL);
	if (frame_opts == NULL) {
		gd_error("gdImageJxl: JxlEncoderFrameSettingsCreate failed");
		goto encode_fail;
	}

	if (lossless) {
		if (JxlEncoderSetFrameLossless(frame_opts, JXL_TRUE) !=
			JXL_ENC_SUCCESS) {
			gd_error("gdImageJxl: JxlEncoderSetFrameLossless failed");
			goto encode_fail;
		}
	} else {
		if (JxlEncoderSetFrameDistance(frame_opts, distance) !=
			JXL_ENC_SUCCESS) {
			gd_error("gdImageJxl: JxlEncoderSetFrameDistance failed");
			goto encode_fail;
		}
	}

	if (JxlEncoderFrameSettingsSetOption(frame_opts,
										 JXL_ENC_FRAME_SETTING_EFFORT,
										 effort) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxl: JxlEncoderFrameSettingsSetOption effort failed");
		goto encode_fail;
	}

	/* Add image frame */
	uint8_t *frame_pixels = pixels;
	size_t frame_pixels_size = (size_t)w * (size_t)h * 4;
	JxlPixelFormat fmt = {.num_channels = has_alpha ? 4u : 3u,
						  .data_type = JXL_TYPE_UINT8,
						  .endianness = JXL_NATIVE_ENDIAN,
						  .align = 0};

	if (!has_alpha) {
		frame_pixels = JxlRGBFromRGBA(pixels, w, h);
		if (frame_pixels == NULL) {
			gd_error("gdImageJxl: RGB buffer allocation failed");
			goto encode_fail;
		}
		frame_pixels_size = (size_t)w * (size_t)h * 3;
	}

	if (JxlEncoderAddImageFrame(frame_opts, &fmt, frame_pixels,
								frame_pixels_size) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxl: JxlEncoderAddImageFrame failed");
		if (frame_pixels != pixels)
			gdFree(frame_pixels);
		goto encode_fail;
	}
	if (frame_pixels != pixels)
		gdFree(frame_pixels);

	JxlEncoderCloseInput(enc);
	for (;;) {
		uint8_t *next_out = outbuf;
		size_t avail = sizeof(outbuf);
		JxlEncoderStatus st = JxlEncoderProcessOutput(enc, &next_out, &avail);

		size_t written = sizeof(outbuf) - avail;
		if (written > 0) {
			if (gdPutBuf(outbuf, (int)written, outfile) != (int)written) {
				gd_error("gdImageJxl: write error");
				goto encode_fail;
			}
		}

		if (st == JXL_ENC_SUCCESS) {
			ret = 0;
			break;
		}
		if (st != JXL_ENC_NEED_MORE_OUTPUT) {
			gd_error("gdImageJxl: encoder error");
			goto encode_fail;
		}
	}

encode_fail:
	// frame_opts is owned by enc, so no separate destroy needed
	if (enc)
		JxlEncoderDestroy(enc);
	if (pixels)
		gdFree(pixels);
	return ret;
}

BGD_DECLARE(void) gdImageJxl(gdImagePtr im, FILE *outFile) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageJxlCtxEx(im, out, 0, 1.0f, 7);
	out->gd_free(out);
}

BGD_DECLARE(void)
gdImageJxlEx(gdImagePtr im, FILE *outFile, int lossless, float distance,
			 int effort) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageJxlCtxEx(im, out, lossless, distance, effort);
	out->gd_free(out);
}

BGD_DECLARE(void *) gdImageJxlPtr(gdImagePtr im, int *size) {
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageJxlCtxEx(im, out, 0, 1.0f, 7)) {
		rv = NULL;
	} else {
		rv = gdDPExtractData(out, size);
	}
	out->gd_free(out);
	return rv;
}

BGD_DECLARE(void *)
gdImageJxlPtrEx(gdImagePtr im, int *size, int lossless, float distance,
				int effort) {
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageJxlCtxEx(im, out, lossless, distance, effort)) {
		rv = NULL;
	} else {
		rv = gdDPExtractData(out, size);
	}
	out->gd_free(out);
	return rv;
}

BGD_DECLARE(void) gdImageJxlCtx(gdImagePtr im, gdIOCtxPtr outfile) {
	_gdImageJxlCtxEx(im, outfile, 0, 1.0f, 7);
}

BGD_DECLARE(void)
gdImageJxlCtxEx(gdImagePtr im, gdIOCtxPtr outfile, int lossless, float distance,
				int effort) {
	_gdImageJxlCtxEx(im, outfile, lossless, distance, effort);
}

/* ---- Animation structures ---- */

typedef struct gdJxlAnim {
	JxlEncoder *enc;
	JxlEncoderFrameSettings *frame_opts;
	gdIOCtxPtr ctx;
	uint32_t width;
	uint32_t height;
	int has_alpha;
	int ownsCtx;
	int memoryWriter;
	int finalized;
	int timestamp;
} gdJxlAnim;

typedef struct gdJxlAnimReader {
	JxlDecoder *dec;
	uint8_t *buf;
	size_t buf_len;
	JxlBasicInfo info;
	int coalesced;
	int done;
	int last_frame_seen;
} gdJxlAnimReader;

/* ---- Animation write ---- */

static int JxlAnimDrainEncoder(gdJxlAnim *anim) {
	uint8_t outbuf[65536];
	for (;;) {
		uint8_t *next_out = outbuf;
		size_t avail = sizeof(outbuf);
		JxlEncoderStatus st =
			JxlEncoderProcessOutput(anim->enc, &next_out, &avail);

		size_t written = sizeof(outbuf) - avail;
		if (written > 0) {
			if (gdPutBuf(outbuf, (int)written, anim->ctx) != (int)written) {
				gd_error("gdImageJxlAnim: write error");
				return 0;
			}
		}

		if (st == JXL_ENC_SUCCESS) {
			return 1;
		}
		if (st != JXL_ENC_NEED_MORE_OUTPUT) {
			gd_error("gdImageJxlAnim: encoder error");
			return 0;
		}
	}
}

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBegin(FILE *outFile, int width, int height, int lossless,
					float distance, int effort) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return NULL;
	}
	gdJxlAnimPtr anim =
		gdImageJxlAnimBeginCtx(out, width, height, lossless, distance, effort);
	if (anim) {
		anim->ownsCtx = 1;
	} else {
		out->gd_free(out);
	}
	return anim;
}

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginCtx(gdIOCtxPtr outCtx, int width, int height, int lossless,
					   float distance, int effort) {
	gdJxlAnimPtr anim;
	JxlBasicInfo info;

	if (outCtx == NULL || width <= 0 || height <= 0) {
		return NULL;
	}

	anim = (gdJxlAnimPtr)gdCalloc(1, sizeof(struct gdJxlAnim));
	if (anim == NULL) {
		return NULL;
	}

	anim->enc = JxlEncoderCreate(NULL);
	if (anim->enc == NULL) {
		gd_error("gdImageJxlAnimBegin: JxlEncoderCreate failed");
		gdFree(anim);
		return NULL;
	}

	anim->ctx = outCtx;
	anim->width = width;
	anim->height = height;
	anim->ownsCtx = 0;
	anim->memoryWriter = 0;
	anim->finalized = 0;
	anim->timestamp = 0;

	/* Set basic info for animation */
	JxlEncoderInitBasicInfo(&info);
	info.xsize = width;
	info.ysize = height;
	info.bits_per_sample = 8;
	info.exponent_bits_per_sample = 0;
	info.num_color_channels = 3;
	info.num_extra_channels = 1;
	info.alpha_bits = 8;
	info.alpha_exponent_bits = 0;
	info.alpha_premultiplied = JXL_FALSE;
	info.uses_original_profile = lossless ? JXL_TRUE : JXL_FALSE;
	info.have_animation = JXL_TRUE;
	info.animation.tps_numerator = 1000; /* ms == ticks */
	info.animation.tps_denominator = 1;
	info.animation.num_loops = 0; /* loop forever */

	if (JxlEncoderSetBasicInfo(anim->enc, &info) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxlAnimBegin: JxlEncoderSetBasicInfo failed");
		JxlEncoderDestroy(anim->enc);
		gdFree(anim);
		return NULL;
	}

	/* Set color encoding (sRGB) */
	JxlColorEncoding srgb_enc;
	JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
	if (JxlEncoderSetColorEncoding(anim->enc, &srgb_enc) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxlAnimBegin: JxlEncoderSetColorEncoding failed");
		JxlEncoderDestroy(anim->enc);
		gdFree(anim);
		return NULL;
	}

	/* Create frame settings once, reuse for all frames */
	anim->frame_opts = JxlEncoderFrameSettingsCreate(anim->enc, NULL);
	if (anim->frame_opts == NULL) {
		gd_error("gdImageJxlAnimBegin: JxlEncoderFrameSettingsCreate failed");
		JxlEncoderDestroy(anim->enc);
		gdFree(anim);
		return NULL;
	}

	if (lossless) {
		if (JxlEncoderSetFrameLossless(anim->frame_opts, JXL_TRUE) !=
			JXL_ENC_SUCCESS) {
			gd_error("gdImageJxlAnimBegin: JxlEncoderSetFrameLossless failed");
			goto anim_begin_fail;
		}
	} else {
		if (JxlEncoderSetFrameDistance(anim->frame_opts, distance) !=
			JXL_ENC_SUCCESS) {
			gd_error("gdImageJxlAnimBegin: JxlEncoderSetFrameDistance failed");
			goto anim_begin_fail;
		}
	}

	if (JxlEncoderFrameSettingsSetOption(anim->frame_opts,
										 JXL_ENC_FRAME_SETTING_EFFORT,
										 effort) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxlAnimBegin: JxlEncoderFrameSettingsSetOption effort "
				 "failed");
		goto anim_begin_fail;
	}

	return anim;

anim_begin_fail:
	// anim->frame_opts is owned by anim->enc, so no separate destroy needed
	if (anim->enc)
		JxlEncoderDestroy(anim->enc);
	gdFree(anim);
	return NULL;
}

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginPtr(int width, int height, int lossless, float distance,
					   int effort) {
	gdIOCtx *out;
	gdJxlAnimPtr anim;

	out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}

	anim =
		gdImageJxlAnimBeginCtx(out, width, height, lossless, distance, effort);
	if (anim == NULL) {
		out->gd_free(out);
		return NULL;
	}

	anim->ownsCtx = 1;
	anim->memoryWriter = 1;
	return anim;
}

BGD_DECLARE(int)
gdImageJxlAnimAddFrame(gdJxlAnimPtr anim, gdImagePtr im, int delay_ms) {
	JxlFrameHeader fhdr;
	JxlPixelFormat fmt;
	uint8_t *pixels = NULL;
	int has_alpha = 0;
	int w, h;

	if (anim == NULL || im == NULL || delay_ms < 0 || anim->finalized) {
		return 0;
	}

	if (!gdImageTrueColor(im)) {
		gd_error("Palette image not supported by JXL animation");
		return 0;
	}

	w = gdImageSX(im);
	h = gdImageSY(im);

	if (w != (int)anim->width || h != (int)anim->height) {
		gd_error("gdImageJxlAnimAddFrame: frame size must match canvas size");
		return 0;
	}

	if (!JxlImageToRGBA(im, &pixels, &has_alpha)) {
		gd_error("gdImageJxlAnimAddFrame: pixel extraction failed");
		return 0;
	}

	/* Update alpha info for this frame if needed */
	if (has_alpha && anim->has_alpha == 0) {
		anim->has_alpha = 1;
	}

	/* Set frame header - duration only, REPLACE blend, full canvas */
	JxlEncoderInitFrameHeader(&fhdr);
	fhdr.duration = (uint32_t)delay_ms; /* ticks == ms */

	if (JxlEncoderSetFrameHeader(anim->frame_opts, &fhdr) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxlAnimAddFrame: JxlEncoderSetFrameHeader failed");
		gdFree(pixels);
		return 0;
	}

	fmt.num_channels = 4;
	fmt.data_type = JXL_TYPE_UINT8;
	fmt.endianness = JXL_NATIVE_ENDIAN;
	fmt.align = 0;

	if (JxlEncoderAddImageFrame(anim->frame_opts, &fmt, pixels,
								(size_t)w * (size_t)h * 4) != JXL_ENC_SUCCESS) {
		gd_error("gdImageJxlAnimAddFrame: JxlEncoderAddImageFrame failed");
		gdFree(pixels);
		return 0;
	}

	gdFree(pixels);
	pixels = NULL;

	anim->timestamp += delay_ms;
	return 1;
}

BGD_DECLARE(int) gdImageJxlAnimEnd(gdJxlAnimPtr anim) {
	int ret = 0;

	if (anim == NULL) {
		return 0;
	}

	if (!anim->finalized) {
		JxlEncoderCloseInput(anim->enc);

		if (!JxlAnimDrainEncoder(anim)) {
			goto anim_end_cleanup;
		}

		anim->finalized = 1;
	}

	ret = 1;

anim_end_cleanup:
	// anim->frame_opts is owned by anim->enc, so no separate destroy needed
	if (anim->enc)
		JxlEncoderDestroy(anim->enc);
	if (anim->ownsCtx && anim->ctx)
		anim->ctx->gd_free(anim->ctx);
	gdFree(anim);
	return ret;
}

BGD_DECLARE(void *) gdImageJxlAnimEndPtr(gdJxlAnimPtr anim, int *size) {
	void *rv = NULL;

	if (size != NULL) {
		*size = 0;
	}
	if (anim == NULL || !anim->memoryWriter) {
		if (anim != NULL) {
			gdImageJxlAnimEnd(anim);
		}
		return NULL;
	}

	if (!anim->finalized) {
		JxlEncoderCloseInput(anim->enc);

		if (!JxlAnimDrainEncoder(anim)) {
			goto anim_end_ptr_cleanup;
		}

		anim->finalized = 1;
	}

	rv = gdDPExtractData(anim->ctx, size);

anim_end_ptr_cleanup:
	if (anim->enc)
		JxlEncoderDestroy(anim->enc);
	if (anim->ctx)
		anim->ctx->gd_free(anim->ctx);
	gdFree(anim);
	return rv;
}

/* ---- Animation read (coalesced & raw) ---- */

static gdJxlAnimReaderPtr _gdImageJxlAnimReaderCreateCtx(gdIOCtxPtr inCtx,
														 int coalesced) {
	size_t buf_len = 0;
	uint8_t *buf = NULL;
	JxlDecoder *dec = NULL;
	gdJxlAnimReaderPtr reader = NULL;

	if (inCtx == NULL) {
		return NULL;
	}

	buf = JxlReadCtxData(inCtx, &buf_len);
	if (buf == NULL) {
		return NULL;
	}

	dec = JxlDecoderCreate(NULL);
	if (dec == NULL) {
		gd_error("gdImageJxlAnimReaderCreate: JxlDecoderCreate failed");
		gdFree(buf);
		return NULL;
	}

	reader = (gdJxlAnimReaderPtr)gdCalloc(1, sizeof(struct gdJxlAnimReader));
	if (reader == NULL) {
		JxlDecoderDestroy(dec);
		gdFree(buf);
		return NULL;
	}

	if (coalesced) {
		JxlDecoderSubscribeEvents(dec, JXL_DEC_BASIC_INFO |
										   JXL_DEC_COLOR_ENCODING |
										   JXL_DEC_FRAME | JXL_DEC_FULL_IMAGE);
		JxlDecoderSetCoalescing(dec, JXL_TRUE);
	} else {
		JxlDecoderSubscribeEvents(dec, JXL_DEC_BASIC_INFO |
										   JXL_DEC_COLOR_ENCODING |
										   JXL_DEC_FRAME | JXL_DEC_FULL_IMAGE);
		JxlDecoderSetCoalescing(dec, JXL_FALSE);
	}

	/* Attach CMS for color space conversion to sRGB */
	JxlDecoderSetCms(dec, *JxlGetDefaultCms());

	/* Set desired output color profile (sRGB) */
	JxlColorEncoding srgb_enc;
	JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
	JxlDecoderSetPreferredColorProfile(dec, &srgb_enc);

	JxlDecoderSetInput(dec, buf, buf_len);
	JxlDecoderCloseInput(dec);

	/* Process until BASIC_INFO to get dimensions */
	for (;;) {
		JxlDecoderStatus status = JxlDecoderProcessInput(dec);
		if (status == JXL_DEC_BASIC_INFO) {
			if (JxlDecoderGetBasicInfo(dec, &reader->info) !=
				JXL_DEC_SUCCESS) {
				gd_error("gdImageJxlAnimReaderCreate: failed to get basic info");
				JxlDecoderDestroy(dec);
				gdFree(buf);
				gdFree(reader);
				return NULL;
			}
			break;
		}
		if (status == JXL_DEC_ERROR || status == JXL_DEC_SUCCESS) {
			gd_error("gdImageJxlAnimReaderCreate: failed to get basic info");
			JxlDecoderDestroy(dec);
			gdFree(buf);
			gdFree(reader);
			return NULL;
		}
	}

	reader->dec = dec;
	reader->buf = buf;
	reader->buf_len = buf_len;
	reader->coalesced = coalesced;
	reader->done = 0;
	reader->last_frame_seen = 0;

	return reader;
}

BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreate(FILE *inFile) {
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (!in) {
		return NULL;
	}
	gdJxlAnimReaderPtr reader = _gdImageJxlAnimReaderCreateCtx(in, JXL_TRUE);
	in->gd_free(in);
	return reader;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreatePtr(int size, void *data) {
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in) {
		return NULL;
	}
	gdJxlAnimReaderPtr reader = _gdImageJxlAnimReaderCreateCtx(in, JXL_TRUE);
	in->gd_free(in);
	return reader;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreateCtx(gdIOCtxPtr inCtx) {
	return _gdImageJxlAnimReaderCreateCtx(inCtx, JXL_TRUE);
}

BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRaw(FILE *inFile) {
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (!in) {
		return NULL;
	}
	gdJxlAnimReaderPtr reader = _gdImageJxlAnimReaderCreateCtx(in, JXL_FALSE);
	in->gd_free(in);
	return reader;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreateRawPtr(int size, void *data) {
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in) {
		return NULL;
	}
	gdJxlAnimReaderPtr reader = _gdImageJxlAnimReaderCreateCtx(in, JXL_FALSE);
	in->gd_free(in);
	return reader;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreateRawCtx(gdIOCtxPtr inCtx) {
	return _gdImageJxlAnimReaderCreateCtx(inCtx, JXL_FALSE);
}

/* Helper: build gdImagePtr from current decoder state */
static gdImagePtr JxlAnimReaderGetCurrentFrame(gdJxlAnimReaderPtr reader,
											   int *delay_ms) {
	JxlDecoder *dec = reader->dec;
	JxlBasicInfo info = reader->info;
	uint8_t *pixels = NULL;
	gdImagePtr im = NULL;
	int have_frame_header = 0;

	if (reader->done) {
		return NULL;
	}

	for (;;) {
		JxlDecoderStatus status = JxlDecoderProcessInput(dec);

		if (status == JXL_DEC_FRAME) {
			JxlFrameHeader fhdr = {0};
			status = JxlDecoderGetFrameHeader(dec, &fhdr);
			if (status != JXL_DEC_SUCCESS) {
				gd_error("gdJxlReadNextFrame: failed to get frame header");
				if (pixels)
					gdFree(pixels);
				return NULL;
			}
			have_frame_header = 1;
			reader->last_frame_seen = fhdr.is_last == JXL_TRUE;
			if (delay_ms) {
				*delay_ms = JxlDurationToMs(fhdr.duration, &info.animation);
			}
		} else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
			JxlPixelFormat fmt = {.num_channels = 4,
								  .data_type = JXL_TYPE_UINT8,
								  .endianness = JXL_NATIVE_ENDIAN,
								  .align = 0};
			size_t pixels_size;
			int w, h;

			if (reader->coalesced) {
				w = (int)info.xsize;
				h = (int)info.ysize;
			} else {
				/* For non-coalesced, we need to query the frame size */
				size_t buf_size;
				if (!have_frame_header) {
					gd_error("gdJxlReadNextFrame: missing frame header");
					return NULL;
				}
				if (JxlDecoderImageOutBufferSize(dec, &fmt, &buf_size) !=
					JXL_DEC_SUCCESS) {
					gd_error("gdJxlReadNextFrame: failed to get buffer size");
					return NULL;
				}
				w = (int)sqrt(
					buf_size /
					4); /* approximation - we'll get actual size from fmt */
				h = (int)(buf_size / 4 / w);
				/* Actually better: we know the frame size from layer_info but
				 * for simplicity with coalesced=false we allocate based on
				 * the buffer size reported */
			}

			if (overflow2(w, h) || overflow2(w * h, 4)) {
				gd_error("gdJxlReadNextFrame: frame dimensions overflow");
				return NULL;
			}

			pixels_size = (size_t)w * (size_t)h * 4;
			pixels = (uint8_t *)gdMalloc(pixels_size);
			if (pixels == NULL) {
				gd_error("gdJxlReadNextFrame: pixel buffer allocation failed");
				return NULL;
			}

			if (JxlDecoderSetImageOutBuffer(dec, &fmt, pixels,
											pixels_size) != JXL_DEC_SUCCESS) {
				gd_error("gdJxlReadNextFrame: failed to set output buffer");
				gdFree(pixels);
				return NULL;
			}
		} else if (status == JXL_DEC_FULL_IMAGE) {
			int has_alpha = (info.alpha_bits > 0);
			int w, h;

			if (reader->coalesced) {
				w = (int)info.xsize;
				h = (int)info.ysize;
			} else {
				/* For non-coalesced, we don't know exact dimensions here.
				 * We'll reconstruct from the buffer. This is a simplification.
				 */
				w = (int)info.xsize;
				h = (int)info.ysize;
			}

			im = JxlImageFromRGBA(pixels, w, h, has_alpha);
			if (pixels)
				gdFree(pixels);
			if (reader->last_frame_seen)
				reader->done = 1;
			return im;
		} else if (status == JXL_DEC_SUCCESS) {
			reader->done = 1;
			if (pixels)
				gdFree(pixels);
			return NULL;
		} else if (status == JXL_DEC_ERROR) {
			gd_error("gdJxlReadNextFrame: decoder error");
			if (pixels)
				gdFree(pixels);
			return NULL;
		}
	}
}

BGD_DECLARE(gdImagePtr)
gdJxlReadNextImage(gdJxlAnimReaderPtr reader, int *delay_ms) {
	if (reader == NULL || reader->coalesced == 0) {
		return NULL;
	}
	return JxlAnimReaderGetCurrentFrame(reader, delay_ms);
}

/* For non-coalesced reader, we need to extract frame info */
static void JxlFrameHeaderToInfo(const JxlFrameHeader *fhdr,
								 const JxlBasicInfo *info,
								 gdJxlFrameInfo *out) {
	out->delay_ms = JxlDurationToMs(fhdr->duration, &info->animation);
	out->x_offset = (int)fhdr->layer_info.crop_x0;
	out->y_offset = (int)fhdr->layer_info.crop_y0;
	out->width = (int)fhdr->layer_info.xsize;
	out->height = (int)fhdr->layer_info.ysize;
	out->blend_mode = (int)fhdr->layer_info.blend_info.blendmode;
	out->is_last = (int)fhdr->is_last;
}

static gdImagePtr JxlAnimReaderGetCurrentRawFrame(gdJxlAnimReaderPtr reader,
												  gdJxlFrameInfo *info) {
	JxlDecoder *dec = reader->dec;
	JxlBasicInfo basic = reader->info;
	JxlFrameHeader fhdr = {0};
	uint8_t *pixels = NULL;
	gdImagePtr im = NULL;
	int has_alpha = 0;
	int have_frame_header = 0;
	int w = 0, h = 0;

	if (reader->done) {
		return NULL;
	}

	for (;;) {
		JxlDecoderStatus status = JxlDecoderProcessInput(dec);

		if (status == JXL_DEC_FRAME) {
			status = JxlDecoderGetFrameHeader(dec, &fhdr);
			if (status != JXL_DEC_SUCCESS) {
				gd_error("gdJxlReadNextFrame: failed to get frame header");
				return NULL;
			}
			have_frame_header = 1;
			reader->last_frame_seen = fhdr.is_last == JXL_TRUE;
			JxlFrameHeaderToInfo(&fhdr, &basic, info);
			has_alpha = (basic.alpha_bits > 0);
		} else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
			JxlPixelFormat fmt = {.num_channels = 4,
								  .data_type = JXL_TYPE_UINT8,
								  .endianness = JXL_NATIVE_ENDIAN,
								  .align = 0};
			size_t pixels_size;

			/* Query the exact buffer size for this frame */
			if (!have_frame_header) {
				gd_error("gdJxlReadNextFrame: missing frame header");
				return NULL;
			}
			if (JxlDecoderImageOutBufferSize(dec, &fmt, &pixels_size) !=
				JXL_DEC_SUCCESS) {
				gd_error("gdJxlReadNextFrame: failed to get buffer size");
				return NULL;
			}
			w = (int)fhdr.layer_info.xsize;
			h = (int)fhdr.layer_info.ysize;

			if (overflow2(w, h) || overflow2(w * h, 4)) {
				gd_error("gdJxlReadNextFrame: frame dimensions overflow");
				return NULL;
			}

			/* Verify pixels_size matches expected */
			if (pixels_size != (size_t)w * (size_t)h * 4) {
				gd_error("gdJxlReadNextFrame: unexpected buffer size");
				return NULL;
			}

			pixels = (uint8_t *)gdMalloc(pixels_size);
			if (pixels == NULL) {
				gd_error("gdJxlReadNextFrame: pixel buffer allocation failed");
				return NULL;
			}

			if (JxlDecoderSetImageOutBuffer(dec, &fmt, pixels,
											pixels_size) != JXL_DEC_SUCCESS) {
				gd_error("gdJxlReadNextFrame: failed to set output buffer");
				gdFree(pixels);
				return NULL;
			}
		} else if (status == JXL_DEC_FULL_IMAGE) {
			if (pixels == NULL) {
				gd_error("gdJxlReadNextFrame: incomplete decoded frame");
				return NULL;
			}
			w = info->width;
			h = info->height;
			im = JxlImageFromRGBA(pixels, w, h, has_alpha);
			if (pixels)
				gdFree(pixels);
			if (reader->last_frame_seen)
				reader->done = 1;
			return im;
		} else if (status == JXL_DEC_SUCCESS) {
			reader->done = 1;
			if (pixels)
				gdFree(pixels);
			return NULL;
		} else if (status == JXL_DEC_ERROR) {
			gd_error("gdJxlReadNextFrame: decoder error");
			if (pixels)
				gdFree(pixels);
			return NULL;
		}
	}
}

BGD_DECLARE(gdImagePtr)
gdJxlReadNextFrame(gdJxlAnimReaderPtr reader, gdJxlFrameInfo *info) {
	if (reader == NULL || reader->coalesced != 0 || info == NULL) {
		return NULL;
	}
	return JxlAnimReaderGetCurrentRawFrame(reader, info);
}

BGD_DECLARE(void) gdImageJxlAnimReaderDestroy(gdJxlAnimReaderPtr reader) {
	if (reader == NULL) {
		return;
	}
	if (reader->dec)
		JxlDecoderDestroy(reader->dec);
	if (reader->buf)
		gdFree(reader->buf);
	gdFree(reader);
}

#else /* !HAVE_LIBJXL */

static void _noJxlError(void) {
	gd_error("JXL image support has been disabled\n");
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxl(FILE *inFile) {
	ARG_NOT_USED(inFile);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlPtr(int size, void *data) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlCtx(gdIOCtx *infile) {
	ARG_NOT_USED(infile);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(void) gdImageJxl(gdImagePtr im, FILE *outFile) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outFile);
	_noJxlError();
}

BGD_DECLARE(void)
gdImageJxlEx(gdImagePtr im, FILE *outFile, int lossless, float distance,
			 int effort) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outFile);
	ARG_NOT_USED(lossless);
	ARG_NOT_USED(distance);
	ARG_NOT_USED(effort);
	_noJxlError();
}

BGD_DECLARE(void *) gdImageJxlPtr(gdImagePtr im, int *size) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(void *)
gdImageJxlPtrEx(gdImagePtr im, int *size, int lossless, float distance,
				int effort) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	ARG_NOT_USED(lossless);
	ARG_NOT_USED(distance);
	ARG_NOT_USED(effort);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(void) gdImageJxlCtx(gdImagePtr im, gdIOCtxPtr outfile) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	_noJxlError();
}

BGD_DECLARE(void)
gdImageJxlCtxEx(gdImagePtr im, gdIOCtxPtr outfile, int lossless, float distance,
				int effort) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	ARG_NOT_USED(lossless);
	ARG_NOT_USED(distance);
	ARG_NOT_USED(effort);
	_noJxlError();
}

/* Animation stubs */
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreate(FILE *inFile) {
	ARG_NOT_USED(inFile);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreatePtr(int size, void *data) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreateCtx(gdIOCtxPtr inCtx) {
	ARG_NOT_USED(inCtx);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdImagePtr)
gdJxlReadNextImage(gdJxlAnimReaderPtr reader, int *delay_ms) {
	ARG_NOT_USED(reader);
	ARG_NOT_USED(delay_ms);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRaw(FILE *inFile) {
	ARG_NOT_USED(inFile);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreateRawPtr(int size, void *data) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdJxlAnimReaderPtr)
gdImageJxlAnimReaderCreateRawCtx(gdIOCtxPtr inCtx) {
	ARG_NOT_USED(inCtx);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdImagePtr)
gdJxlReadNextFrame(gdJxlAnimReaderPtr reader, gdJxlFrameInfo *info) {
	ARG_NOT_USED(reader);
	ARG_NOT_USED(info);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(void) gdImageJxlAnimReaderDestroy(gdJxlAnimReaderPtr reader) {
	ARG_NOT_USED(reader);
	_noJxlError();
}

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBegin(FILE *outFile, int width, int height, int lossless,
					float distance, int effort) {
	ARG_NOT_USED(outFile);
	ARG_NOT_USED(width);
	ARG_NOT_USED(height);
	ARG_NOT_USED(lossless);
	ARG_NOT_USED(distance);
	ARG_NOT_USED(effort);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginCtx(gdIOCtxPtr outCtx, int width, int height, int lossless,
					   float distance, int effort) {
	ARG_NOT_USED(outCtx);
	ARG_NOT_USED(width);
	ARG_NOT_USED(height);
	ARG_NOT_USED(lossless);
	ARG_NOT_USED(distance);
	ARG_NOT_USED(effort);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginPtr(int width, int height, int lossless, float distance,
					   int effort) {
	ARG_NOT_USED(width);
	ARG_NOT_USED(height);
	ARG_NOT_USED(lossless);
	ARG_NOT_USED(distance);
	ARG_NOT_USED(effort);
	_noJxlError();
	return NULL;
}

BGD_DECLARE(int)
gdImageJxlAnimAddFrame(gdJxlAnimPtr anim, gdImagePtr im, int delay_ms) {
	ARG_NOT_USED(anim);
	ARG_NOT_USED(im);
	ARG_NOT_USED(delay_ms);
	_noJxlError();
	return 0;
}

BGD_DECLARE(int) gdImageJxlAnimEnd(gdJxlAnimPtr anim) {
	ARG_NOT_USED(anim);
	_noJxlError();
	return 0;
}

BGD_DECLARE(void *) gdImageJxlAnimEndPtr(gdJxlAnimPtr anim, int *size) {
	ARG_NOT_USED(anim);
	if (size != NULL) {
		*size = 0;
	}
	_noJxlError();
	return NULL;
}

#endif /* HAVE_LIBJXL */
