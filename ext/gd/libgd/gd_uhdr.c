#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * File: UltraHDR IO
 *
 * Read and write UltraHDR images with gain map preservation.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gd.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include "gdhelpers.h"

#ifdef HAVE_LIBUHDR
#include <ultrahdr_api.h>
#endif

typedef enum {
	GD_UHDR_OP_RESIZE = 0,
	GD_UHDR_OP_CROP,
	GD_UHDR_OP_ROTATE,
	GD_UHDR_OP_MIRROR
} gdUhdrOpType;

typedef struct {
	gdUhdrOpType type;
	int p1;
	int p2;
	int p3;
	int p4;
} gdUhdrOp;

struct gdUhdrImageStruct {
	int format;
	int width;
	int height;
	int has_gain_map;
	void *blob;
	int blob_size;
	gdUhdrOp *ops;
	int op_count;
	int op_capacity;
};

static void gdUhdrSetError(gdUhdrErrorPtr err, int code, int provider_code,
						   const char *message) {
	if (!err) {
		return;
	}

	err->code = code;
	err->provider_code = provider_code;
	err->message[0] = '\0';
	if (message) {
		strncpy(err->message, message, sizeof(err->message) - 1);
		err->message[sizeof(err->message) - 1] = '\0';
	}
}

#ifdef HAVE_LIBUHDR

static int gdUhdrIsSupportedFormat(int format) {
	return format == GD_UHDR_FORMAT_JPEG;
}

static int gdUhdrIntAddOverflow(int a, int b) {
	if (a < 0 || b < 0) {
		return 1;
	}

	if (a > INT_MAX - b) {
		return 1;
	}

	return 0;
}

static int gdUhdrReadAllFromCtx(gdIOCtxPtr ctx, void **out_data,
								int *out_size) {
	const int step = 4096;
	unsigned char buf[4096];
	unsigned char *data = NULL;
	int size = 0;

	if (!ctx || !out_data || !out_size) {
		return GD_UHDR_E_INVALID;
	}

	for (;;) {
		int n = gdGetBuf(buf, step, ctx);
		int new_size;
		if (n <= 0) {
			break;
		}

		if (gdUhdrIntAddOverflow(size, n)) {
			gd_error("gd-uhdr read size overflow: size=%d n=%d\n", size, n);
			gdFree(data);
			return GD_UHDR_E_INVALID;
		}

		new_size = size + n;

		{
			unsigned char *tmp =
				(unsigned char *)gdRealloc(data, (size_t)new_size);
			if (!tmp) {
				gd_error("gd-uhdr realloc failed: requested=%d\n", new_size);
				gdFree(data);
				return GD_UHDR_E_DECODE;
			}
			data = tmp;
		}

		memcpy(data + size, buf, (size_t)n);
		size = new_size;
	}

	if (!data || size <= 0) {
		gdFree(data);
		return GD_UHDR_E_DECODE;
	}

	*out_data = data;
	*out_size = size;
	return GD_UHDR_SUCCESS;
}

static void gdUhdrInitCompressedImage(uhdr_compressed_image_t *image,
									  void *data, int size) {
	memset(image, 0, sizeof(*image));
	image->data = data;
	image->data_sz = (size_t)size;
	image->capacity = (size_t)size;
	image->cg = UHDR_CG_UNSPECIFIED;
	image->ct = UHDR_CT_UNSPECIFIED;
	image->range = UHDR_CR_FULL_RANGE;
}

static int gdUhdrCopyMetadataProfile(gdImageMetadata **dst,
									 const gdImageMetadata *src,
									 const char *key, gdUhdrErrorPtr err) {
	const unsigned char *profile;
	size_t profile_size;
	int status;

	profile = gdImageMetadataGetProfile(src, key, &profile_size);
	if (!profile) {
		return GD_UHDR_SUCCESS;
	}

	if (!*dst) {
		*dst = gdImageMetadataCreate();
		if (!*dst) {
			gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
						   "Out of memory copying JPEG metadata");
			return GD_UHDR_E_ENCODE;
		}
	}

	status = gdImageMetadataSetProfile(*dst, key, profile, profile_size);
	if (status != GD_META_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, status,
					   "Failed to copy JPEG metadata");
		return GD_UHDR_E_ENCODE;
	}

	return GD_UHDR_SUCCESS;
}

static int gdUhdrCreateJpegMetadataFromBlock(uhdr_mem_block_t *block,
											 int copy_exif, int copy_icc,
											 gdImageMetadata **out,
											 gdUhdrErrorPtr err) {
	gdImageMetadata *src_metadata;
	gdImagePtr decoded;
	int status;

	*out = NULL;
	if (!block || !block->data || block->data_sz == 0) {
		return GD_UHDR_SUCCESS;
	}
	if (block->data_sz > (size_t)INT_MAX) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0,
					   "JPEG metadata block is too large");
		return GD_UHDR_E_INVALID;
	}

	src_metadata = gdImageMetadataCreate();
	if (!src_metadata) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
					   "Out of memory reading JPEG metadata");
		return GD_UHDR_E_ENCODE;
	}

	decoded = gdImageCreateFromJpegPtrWithMetadata((int)block->data_sz,
												   block->data, src_metadata);
	if (!decoded) {
		gdImageMetadataFree(src_metadata);
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   "Failed to read JPEG metadata");
		return GD_UHDR_E_DECODE;
	}
	gdImageDestroy(decoded);

	if (copy_exif) {
		status = gdUhdrCopyMetadataProfile(out, src_metadata, "exif", err);
		if (status != GD_UHDR_SUCCESS) {
			gdImageMetadataFree(src_metadata);
			return status;
		}
	}
	if (copy_icc) {
		status = gdUhdrCopyMetadataProfile(out, src_metadata, "icc", err);
		if (status != GD_UHDR_SUCCESS) {
			gdImageMetadataFree(src_metadata);
			return status;
		}
	}

	gdImageMetadataFree(src_metadata);
	return GD_UHDR_SUCCESS;
}

static gdImagePtr gdUhdrCreateGdImageFromJpegBlock(uhdr_mem_block_t *block,
												   const char *label,
												   gdUhdrErrorPtr err) {
	gdImagePtr image;

	if (!block || !block->data || block->data_sz == 0 ||
		block->data_sz > (size_t)INT_MAX) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   "Invalid compressed UltraHDR component");
		return NULL;
	}

	image = gdImageCreateFromJpegPtr((int)block->data_sz, block->data);
	if (!image) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   label && label[0]
						   ? label
						   : "Failed to decode UltraHDR JPEG component");
		return NULL;
	}

	return image;
}

static int gdUhdrScaleValue(int value, int from_extent, int to_extent,
							int *out) {
	long long scaled;

	if (!out || value < 0 || from_extent <= 0 || to_extent <= 0) {
		return 0;
	}

	scaled = ((long long)value * (long long)to_extent + from_extent / 2) /
			 from_extent;
	if (scaled < 0 || scaled > INT_MAX) {
		return 0;
	}

	*out = (int)scaled;
	return 1;
}

static int gdUhdrReplaceImage(gdImagePtr *image, gdImagePtr replacement,
							  gdUhdrErrorPtr err) {
	if (!replacement) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
					   "Failed to transform UltraHDR image component");
		return GD_UHDR_E_ENCODE;
	}

	gdImageDestroy(*image);
	*image = replacement;
	return GD_UHDR_SUCCESS;
}

static gdImagePtr gdUhdrScaleImage(gdImagePtr image, unsigned int width,
								   unsigned int height) {
	/* Mitchell uses GD's two-pass scaler and reduces downscale aliasing. */
	if (!gdImageSetInterpolationMethod(image, GD_MITCHELL)) {
		return NULL;
	}

	return gdImageScale(image, width, height);
}

static int gdUhdrApplyGdOps(gdImagePtr *base_image, gdImagePtr *gainmap_image,
							gdUhdrImagePtr im, gdUhdrErrorPtr err) {
	int i;

	if (!base_image || !*base_image || !gainmap_image || !*gainmap_image ||
		!im) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0,
					   "Invalid UltraHDR transform state");
		return GD_UHDR_E_INVALID;
	}

	for (i = 0; i < im->op_count; i++) {
		gdUhdrOp *op = &im->ops[i];
		int base_w = gdImageSX(*base_image);
		int base_h = gdImageSY(*base_image);
		int gain_w = gdImageSX(*gainmap_image);
		int gain_h = gdImageSY(*gainmap_image);
		int status;

		switch (op->type) {
		case GD_UHDR_OP_RESIZE: {
			int scaled_gain_w;
			int scaled_gain_h;

			if (!gdUhdrScaleValue(op->p1, base_w, gain_w, &scaled_gain_w) ||
				!gdUhdrScaleValue(op->p2, base_h, gain_h, &scaled_gain_h)) {
				gdUhdrSetError(err, GD_UHDR_E_INVALID, 0,
							   "Invalid UltraHDR resize operation");
				return GD_UHDR_E_INVALID;
			}
			if (scaled_gain_w <= 0) {
				scaled_gain_w = 1;
			}
			if (scaled_gain_h <= 0) {
				scaled_gain_h = 1;
			}

			status = gdUhdrReplaceImage(base_image,
										gdUhdrScaleImage(*base_image,
														 (unsigned int)op->p1,
														 (unsigned int)op->p2),
										err);
			if (status != GD_UHDR_SUCCESS) {
				return status;
			}
			status = gdUhdrReplaceImage(
				gainmap_image,
				gdUhdrScaleImage(*gainmap_image, (unsigned int)scaled_gain_w,
								 (unsigned int)scaled_gain_h),
				err);
			if (status != GD_UHDR_SUCCESS) {
				return status;
			}
			break;
		}
		case GD_UHDR_OP_CROP: {
			gdRect base_crop;
			gdRect gain_crop;
			int gain_left;
			int gain_right;
			int gain_top;
			int gain_bottom;

			if (op->p1 < 0 || op->p3 < 0 || op->p2 <= op->p1 ||
				op->p4 <= op->p3 || op->p2 > base_w || op->p4 > base_h) {
				gdUhdrSetError(err, GD_UHDR_E_INVALID, 0,
							   "Invalid UltraHDR crop operation");
				return GD_UHDR_E_INVALID;
			}
			if (!gdUhdrScaleValue(op->p1, base_w, gain_w, &gain_left) ||
				!gdUhdrScaleValue(op->p2, base_w, gain_w, &gain_right) ||
				!gdUhdrScaleValue(op->p3, base_h, gain_h, &gain_top) ||
				!gdUhdrScaleValue(op->p4, base_h, gain_h, &gain_bottom) ||
				gain_right <= gain_left || gain_bottom <= gain_top) {
				gdUhdrSetError(err, GD_UHDR_E_INVALID, 0,
							   "Invalid UltraHDR gain map crop operation");
				return GD_UHDR_E_INVALID;
			}

			base_crop.x = op->p1;
			base_crop.y = op->p3;
			base_crop.width = op->p2 - op->p1;
			base_crop.height = op->p4 - op->p3;
			gain_crop.x = gain_left;
			gain_crop.y = gain_top;
			gain_crop.width = gain_right - gain_left;
			gain_crop.height = gain_bottom - gain_top;

			status = gdUhdrReplaceImage(
				base_image, gdImageCrop(*base_image, &base_crop), err);
			if (status != GD_UHDR_SUCCESS) {
				return status;
			}
			status = gdUhdrReplaceImage(
				gainmap_image, gdImageCrop(*gainmap_image, &gain_crop), err);
			if (status != GD_UHDR_SUCCESS) {
				return status;
			}
			break;
		}
		case GD_UHDR_OP_ROTATE: {
			gdImagePtr rotated_base = NULL;
			gdImagePtr rotated_gainmap = NULL;

			if (op->p1 == 90) {
				rotated_base = gdImageRotate90(*base_image, 0);
				rotated_gainmap = gdImageRotate90(*gainmap_image, 0);
			} else if (op->p1 == 180) {
				rotated_base = gdImageRotate180(*base_image, 0);
				rotated_gainmap = gdImageRotate180(*gainmap_image, 0);
			} else if (op->p1 == 270) {
				rotated_base = gdImageRotate270(*base_image, 0);
				rotated_gainmap = gdImageRotate270(*gainmap_image, 0);
			}

			status = gdUhdrReplaceImage(base_image, rotated_base, err);
			if (status != GD_UHDR_SUCCESS) {
				if (rotated_gainmap) {
					gdImageDestroy(rotated_gainmap);
				}
				return status;
			}
			status = gdUhdrReplaceImage(gainmap_image, rotated_gainmap, err);
			if (status != GD_UHDR_SUCCESS) {
				return status;
			}
			break;
		}
		case GD_UHDR_OP_MIRROR:
			if (op->p1 == GD_UHDR_MIRROR_HORIZONTAL) {
				gdImageFlipHorizontal(*base_image);
				gdImageFlipHorizontal(*gainmap_image);
			} else {
				gdImageFlipVertical(*base_image);
				gdImageFlipVertical(*gainmap_image);
			}
			break;
		default:
			gdUhdrSetError(err, GD_UHDR_E_INVALID, 0,
						   "Unknown queued UltraHDR operation");
			return GD_UHDR_E_INVALID;
		}
	}

	return GD_UHDR_SUCCESS;
}

static int gdUhdrEncodeJpegComponent(gdImagePtr image, int quality,
									 const gdImageMetadata *metadata,
									 int no_subsampling, void **out_data,
									 int *out_size, gdUhdrErrorPtr err) {
	void *jpeg;
	int jpeg_size = 0;

	if (no_subsampling) {
		jpeg = gdImageJpegPtrWithMetadataNoSubsampling(image, &jpeg_size,
													   quality, metadata);
	} else {
		jpeg = gdImageJpegPtrWithMetadata(image, &jpeg_size, quality, metadata);
	}
	if (!jpeg || jpeg_size <= 0) {
		gdFree(jpeg);
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
					   "Failed to encode UltraHDR JPEG component");
		return GD_UHDR_E_ENCODE;
	}

	*out_data = jpeg;
	*out_size = jpeg_size;
	return GD_UHDR_SUCCESS;
}

static int gdUhdrQueueOp(gdUhdrImagePtr im, gdUhdrOpType type, int p1, int p2,
						 int p3, int p4) {
	if (im->op_count == im->op_capacity) {
		int new_cap = im->op_capacity == 0 ? 8 : im->op_capacity * 2;
		gdUhdrOp *tmp;

		if (overflow2(new_cap, (int)sizeof(gdUhdrOp))) {
			return GD_UHDR_E_INVALID;
		}

		tmp =
			(gdUhdrOp *)gdRealloc(im->ops, (size_t)new_cap * sizeof(gdUhdrOp));
		if (!tmp) {
			return GD_UHDR_E_INVALID;
		}

		im->ops = tmp;
		im->op_capacity = new_cap;
	}

	im->ops[im->op_count].type = type;
	im->ops[im->op_count].p1 = p1;
	im->ops[im->op_count].p2 = p2;
	im->ops[im->op_count].p3 = p3;
	im->ops[im->op_count].p4 = p4;
	im->op_count++;

	return GD_UHDR_SUCCESS;
}

static gdUhdrImagePtr gdUhdrImageCreateFromData(void *data, int size,
												int format,
												gdUhdrErrorPtr err) {
	gdUhdrImagePtr im;
	uhdr_codec_private_t *dec;
	uhdr_compressed_image_t input;
	uhdr_error_info_t rc;

	if (!data || size <= 0 || !gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid UltraHDR input");
		return NULL;
	}

	if (!is_uhdr_image(data, size)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0,
					   "Input is not a valid UltraHDR image");
		return NULL;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   "Failed to create UltraHDR decoder");
		return NULL;
	}

	input.data = data;
	input.data_sz = (size_t)size;
	input.capacity = (size_t)size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail ? rc.detail : "uhdr_dec_set_image failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail ? rc.detail : "uhdr_dec_probe failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	im = (gdUhdrImagePtr)gdCalloc(1, sizeof(*im));
	if (!im) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Out of memory");
		uhdr_release_decoder(dec);
		return NULL;
	}

	im->blob = gdMalloc((size_t)size);
	if (!im->blob) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Out of memory");
		gdFree(im);
		uhdr_release_decoder(dec);
		return NULL;
	}

	memcpy(im->blob, data, (size_t)size);
	im->blob_size = size;
	im->format = format;
	im->width = uhdr_dec_get_image_width(dec);
	im->height = uhdr_dec_get_image_height(dec);
	im->has_gain_map = (uhdr_dec_get_gainmap_width(dec) > 0 &&
						uhdr_dec_get_gainmap_height(dec) > 0)
						   ? 1
						   : 0;

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	uhdr_release_decoder(dec);
	return im;
}

#endif

#ifndef HAVE_LIBUHDR

static int gdUhdrUnavailableCode(void) { return GD_UHDR_NOT_AVAILABLE; }

static const char *gdUhdrUnavailableMessage(void) {
	return "UltraHDR support is not enabled in this build";
}

#endif

/*
	Function: gdUhdrIsAvailable

		Returns whether UltraHDR support is available in this libgd build.

	Returns:

		1 if available, 0 otherwise.
*/
BGD_DECLARE(int) gdUhdrIsAvailable(void) {
#ifdef HAVE_LIBUHDR
	return 1;
#else
	return 0;
#endif
}

/*
	Function: gdUhdrImageCreateFromFile

		Loads an UltraHDR image from a file.

	Variants:

		<gdUhdrImageCreateFromCtx> loads from a <gdIOCtx>.

		<gdUhdrImageCreateFromPtr> loads from memory.

	Parameters:

		filename - input file path.
		format   - input format (currently <GD_UHDR_FORMAT_JPEG>).
		err      - optional output error details.

	Returns:

		A new <gdUhdrImagePtr> on success, or NULL on error.
*/
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromFile(const char *filename, int format,
						  gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	FILE *fp;
	gdIOCtxPtr in;
	gdUhdrImagePtr im;

	if (!filename) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "filename must not be NULL");
		return NULL;
	}

	fp = fopen(filename, "rb");
	if (!fp) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to open input file");
		return NULL;
	}

	in = gdNewFileCtx(fp);
	if (!in) {
		fclose(fp);
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to create IO context");
		return NULL;
	}

	im = gdUhdrImageCreateFromCtx(in, format, err);
	in->gd_free(in);
	fclose(fp);
	return im;
#else
	ARG_NOT_USED(filename);
	ARG_NOT_USED(format);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageCreateFromCtx

		See <gdUhdrImageCreateFromFile>.
*/
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromCtx(gdIOCtxPtr ctx, int format, gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	void *data = NULL;
	int size = 0;
	int rc;
	gdUhdrImagePtr im;

	if (!gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0,
					   "Unsupported UltraHDR format");
		return NULL;
	}

	rc = gdUhdrReadAllFromCtx(ctx, &data, &size);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to read UltraHDR stream");
		return NULL;
	}

	im = gdUhdrImageCreateFromData(data, size, format, err);
	gdFree(data);
	return im;
#else
	ARG_NOT_USED(ctx);
	ARG_NOT_USED(format);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageCreateFromPtr

		See <gdUhdrImageCreateFromFile>.
*/
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromPtr(int size, void *data, int format, gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	if (!gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0,
					   "Unsupported UltraHDR format");
		return NULL;
	}

	return gdUhdrImageCreateFromData(data, size, format, err);
#else
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	ARG_NOT_USED(format);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageDestroy

		Releases an UltraHDR image created by <gdUhdrImageCreateFromFile> or its
   variants.
*/
BGD_DECLARE(void) gdUhdrImageDestroy(gdUhdrImagePtr im) {
	if (!im) {
		return;
	}

	gdFree(im->ops);
	gdFree(im->blob);
	gdFree(im);
}

/*
  Function: gdUhdrImageWidth

	Returns the image width in pixels.
*/
BGD_DECLARE(int) gdUhdrImageWidth(gdUhdrImagePtr im) {
	if (!im) {
		return 0;
	}
	return im->width;
}

/*
  Function: gdUhdrImageHeight

	Returns the image height in pixels.
*/
BGD_DECLARE(int) gdUhdrImageHeight(gdUhdrImagePtr im) {
	if (!im) {
		return 0;
	}
	return im->height;
}

/*
  Function: gdUhdrImageHasGainMap

	Returns nonzero when the loaded image contains a gain map.
*/
BGD_DECLARE(int) gdUhdrImageHasGainMap(gdUhdrImagePtr im) {
	if (!im) {
		return 0;
	}
	return im->has_gain_map;
}

/*
  Function: gdUhdrImageResize

	Queues a resize operation to be applied at save time.
*/
BGD_DECLARE(int)
gdUhdrImageResize(gdUhdrImagePtr im, int width, int height,
				  gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	int rc;
	if (!im || width <= 0 || height <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid resize arguments");
		return GD_UHDR_E_INVALID;
	}

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_RESIZE, width, height, 0, 0);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue resize effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(width);
	ARG_NOT_USED(height);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageCrop

		Queues a crop operation to be applied at save time.
*/
BGD_DECLARE(int)
gdUhdrImageCrop(gdUhdrImagePtr im, int left, int top, int width, int height,
				gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	int rc;
	int right;
	int bottom;

	if (!im || left < 0 || top < 0 || width <= 0 || height <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid crop arguments");
		return GD_UHDR_E_INVALID;
	}

	if (gdUhdrIntAddOverflow(left, width) ||
		gdUhdrIntAddOverflow(top, height)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid crop arguments");
		return GD_UHDR_E_INVALID;
	}

	right = left + width;
	bottom = top + height;

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_CROP, left, right, top, bottom);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue crop effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(left);
	ARG_NOT_USED(top);
	ARG_NOT_USED(width);
	ARG_NOT_USED(height);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageRotate

		Queues a clockwise rotation (90, 180, or 270 degrees) to be applied at
   save time.
*/
BGD_DECLARE(int)
gdUhdrImageRotate(gdUhdrImagePtr im, int degrees, gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	int rc;
	if (!im || (degrees != 90 && degrees != 180 && degrees != 270)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid rotation angle");
		return GD_UHDR_E_INVALID;
	}

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_ROTATE, degrees, 0, 0, 0);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue rotation effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(degrees);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageMirror

		Queues a mirror operation to be applied at save time.
*/
BGD_DECLARE(int)
gdUhdrImageMirror(gdUhdrImagePtr im, int axis, gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	int rc;
	if (!im || (axis != GD_UHDR_MIRROR_HORIZONTAL &&
				axis != GD_UHDR_MIRROR_VERTICAL)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid mirror axis");
		return GD_UHDR_E_INVALID;
	}

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_MIRROR, axis, 0, 0, 0);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue mirror effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(axis);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageFile

		Saves an UltraHDR image to a file path.

	Variants:

		<gdUhdrImageCtx> writes via <gdIOCtx>.

		<gdUhdrImageWritePtr> writes to memory.
*/
BGD_DECLARE(int)
gdUhdrImageFile(gdUhdrImagePtr im, const char *filename, int format,
				int quality, gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	FILE *fp;
	gdIOCtxPtr out;
	int rc;

	if (!filename) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "filename must not be NULL");
		return GD_UHDR_E_INVALID;
	}

	fp = fopen(filename, "wb");
	if (!fp) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Failed to open output file");
		return GD_UHDR_E_ENCODE;
	}

	out = gdNewFileCtx(fp);
	if (!out) {
		fclose(fp);
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Failed to create IO context");
		return GD_UHDR_E_ENCODE;
	}

	rc = gdUhdrImageCtx(im, out, format, quality, err);
	out->gd_free(out);
	fclose(fp);
	return rc;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(filename);
	ARG_NOT_USED(format);
	ARG_NOT_USED(quality);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageCtx

		See <gdUhdrImageFile>.
*/
BGD_DECLARE(int)
gdUhdrImageCtx(gdUhdrImagePtr im, gdIOCtxPtr ctx, int format, int quality,
			   gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	uhdr_codec_private_t *dec = NULL;
	uhdr_codec_private_t *enc = NULL;
	uhdr_compressed_image_t source;
	uhdr_compressed_image_t base_jpeg;
	uhdr_compressed_image_t gainmap_jpeg;
	uhdr_error_info_t rc;
	uhdr_compressed_image_t *encoded;
	uhdr_gainmap_metadata_t *source_metadata;
	uhdr_gainmap_metadata_t metadata;
	uhdr_mem_block_t *base_block = NULL;
	uhdr_mem_block_t *gainmap_block = NULL;
	gdImageMetadata *base_metadata = NULL;
	gdImageMetadata *gainmap_metadata = NULL;
	gdImagePtr base_image = NULL;
	gdImagePtr gainmap_image = NULL;
	void *base_jpeg_data = NULL;
	void *gainmap_jpeg_data = NULL;
	int base_jpeg_size = 0;
	int gainmap_jpeg_size = 0;
	int write_result;
	int status = GD_UHDR_SUCCESS;

	if (!im || !ctx || !im->blob || im->blob_size <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0,
					   "Invalid UltraHDR image or output context");
		return GD_UHDR_E_INVALID;
	}

	if (!gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0,
					   "Unsupported UltraHDR output format");
		return GD_UHDR_E_UNSUPPORTED;
	}

	if (quality < 1 || quality > 95) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "quality must be in [1..95]");
		return GD_UHDR_E_INVALID;
	}

	if (im->op_count == 0) {
		write_result = gdPutBuf(im->blob, im->blob_size, ctx);
		if (write_result != im->blob_size) {
			gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
						   "Failed to write UltraHDR output");
			return GD_UHDR_E_ENCODE;
		}

		gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
		return GD_UHDR_SUCCESS;
	}

	dec = uhdr_create_decoder();
	enc = uhdr_create_encoder();
	if (!dec || !enc) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
					   "Failed to create UltraHDR codec contexts");
		status = GD_UHDR_E_ENCODE;
		goto cleanup;
	}

	gdUhdrInitCompressedImage(&source, im->blob, im->blob_size);

	rc = uhdr_dec_set_image(dec, &source);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail ? rc.detail : "uhdr_dec_set_image failed");
		status = GD_UHDR_E_DECODE;
		goto cleanup;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail ? rc.detail : "uhdr_dec_probe failed");
		status = GD_UHDR_E_DECODE;
		goto cleanup;
	}

	source_metadata = uhdr_dec_get_gainmap_metadata(dec);
	if (!source_metadata) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   "UltraHDR gain map metadata is missing");
		status = GD_UHDR_E_DECODE;
		goto cleanup;
	}
	metadata = *source_metadata;

	base_block = uhdr_dec_get_base_image(dec);
	gainmap_block = uhdr_dec_get_gainmap_image(dec);

	/*
	 * Geometry transforms make source EXIF dimensions, orientation, and
	 * thumbnails stale. Preserve ICC, but copy EXIF only for pass-through
	 * writes unless GD learns to rewrite those tags.
	 */
	status = gdUhdrCreateJpegMetadataFromBlock(base_block, im->op_count == 0, 1,
											   &base_metadata, err);
	if (status != GD_UHDR_SUCCESS) {
		goto cleanup;
	}

	if (!metadata.use_base_cg) {
		status = gdUhdrCreateJpegMetadataFromBlock(gainmap_block, 0, 1,
												   &gainmap_metadata, err);
		if (status != GD_UHDR_SUCCESS) {
			goto cleanup;
		}
	}

	base_image = gdUhdrCreateGdImageFromJpegBlock(
		base_block, "Failed to decode UltraHDR base image", err);
	if (!base_image) {
		status = GD_UHDR_E_DECODE;
		goto cleanup;
	}

	gainmap_image = gdUhdrCreateGdImageFromJpegBlock(
		gainmap_block, "Failed to decode UltraHDR gain map image", err);
	if (!gainmap_image) {
		status = GD_UHDR_E_DECODE;
		goto cleanup;
	}

	status = gdUhdrApplyGdOps(&base_image, &gainmap_image, im, err);
	if (status != GD_UHDR_SUCCESS) {
		goto cleanup;
	}

	status = gdUhdrEncodeJpegComponent(base_image, quality, base_metadata, 0,
									   &base_jpeg_data, &base_jpeg_size, err);
	if (status != GD_UHDR_SUCCESS) {
		goto cleanup;
	}

	status =
		gdUhdrEncodeJpegComponent(gainmap_image, quality, gainmap_metadata, 1,
								  &gainmap_jpeg_data, &gainmap_jpeg_size, err);
	if (status != GD_UHDR_SUCCESS) {
		goto cleanup;
	}

	gdUhdrInitCompressedImage(&base_jpeg, base_jpeg_data, base_jpeg_size);
	gdUhdrInitCompressedImage(&gainmap_jpeg, gainmap_jpeg_data,
							  gainmap_jpeg_size);

	rc = uhdr_enc_set_output_format(enc, UHDR_CODEC_JPG);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code,
					   rc.has_detail ? rc.detail
									 : "uhdr_enc_set_output_format failed");
		status = GD_UHDR_E_ENCODE;
		goto cleanup;
	}

	rc = uhdr_enc_set_compressed_image(enc, &base_jpeg, UHDR_BASE_IMG);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code,
					   rc.has_detail
						   ? rc.detail
						   : "uhdr_enc_set_compressed_image(base) failed");
		status = GD_UHDR_E_ENCODE;
		goto cleanup;
	}

	rc = uhdr_enc_set_gainmap_image(enc, &gainmap_jpeg, &metadata);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code,
					   rc.has_detail ? rc.detail
									 : "uhdr_enc_set_gainmap_image failed");
		status = GD_UHDR_E_ENCODE;
		goto cleanup;
	}

	rc = uhdr_encode(enc);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code,
					   rc.has_detail ? rc.detail : "uhdr_encode failed");
		status = GD_UHDR_E_ENCODE;
		goto cleanup;
	}

	encoded = uhdr_get_encoded_stream(enc);
	if (!encoded || !encoded->data || encoded->data_sz == 0) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
					   "Encoded UltraHDR stream is empty");
		status = GD_UHDR_E_ENCODE;
		goto cleanup;
	}

	write_result = gdPutBuf(encoded->data, (int)encoded->data_sz, ctx);
	if (write_result != (int)encoded->data_sz) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
					   "Failed to write UltraHDR output");
		status = GD_UHDR_E_ENCODE;
		goto cleanup;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	status = GD_UHDR_SUCCESS;

cleanup:
	gdFree(gainmap_jpeg_data);
	gdFree(base_jpeg_data);
	if (gainmap_image) {
		gdImageDestroy(gainmap_image);
	}
	if (base_image) {
		gdImageDestroy(base_image);
	}
	if (gainmap_metadata) {
		gdImageMetadataFree(gainmap_metadata);
	}
	if (base_metadata) {
		gdImageMetadataFree(base_metadata);
	}
	if (enc) {
		uhdr_release_encoder(enc);
	}
	if (dec) {
		uhdr_release_decoder(dec);
	}
	return status;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(ctx);
	ARG_NOT_USED(format);
	ARG_NOT_USED(quality);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageWritePtr

		See <gdUhdrImageFile>.
*/
BGD_DECLARE(void *)
gdUhdrImageWritePtr(gdUhdrImagePtr im, int *size, int format, int quality,
					gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	gdIOCtxPtr out;
	void *rv;

	if (size) {
		*size = 0;
	}

	out = gdNewDynamicCtx(2048, NULL);
	if (!out) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0,
					   "Failed to create dynamic output context");
		return NULL;
	}

	if (gdUhdrImageCtx(im, out, format, quality, err) != GD_UHDR_SUCCESS) {
		out->gd_free(out);
		return NULL;
	}

	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return rv;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(format);
	ARG_NOT_USED(quality);
	if (size) {
		*size = 0;
	}
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageGetSdr

		Decodes and returns the SDR base image as a <gdImagePtr>.
*/
BGD_DECLARE(gdImagePtr)
gdUhdrImageGetSdr(gdUhdrImagePtr im, gdUhdrErrorPtr err) {
#ifdef HAVE_LIBUHDR
	uhdr_codec_private_t *dec;
	uhdr_compressed_image_t input;
	uhdr_error_info_t rc;
	uhdr_raw_image_t *raw;
	gdImagePtr out;
	int x, y;
	unsigned int stride;
	unsigned char *row;

	if (!im || !im->blob || im->blob_size <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid UltraHDR image");
		return NULL;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   "Failed to create UltraHDR decoder");
		return NULL;
	}

	input.data = im->blob;
	input.data_sz = (size_t)im->blob_size;
	input.capacity = (size_t)im->blob_size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail ? rc.detail : "uhdr_dec_set_image failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_dec_set_out_img_format(dec, UHDR_IMG_FMT_32bppRGBA8888);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail ? rc.detail
									 : "uhdr_dec_set_out_img_format failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_dec_set_out_color_transfer(dec, UHDR_CT_SRGB);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail
						   ? rc.detail
						   : "uhdr_dec_set_out_color_transfer failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_decode(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code,
					   rc.has_detail ? rc.detail : "uhdr_decode failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	raw = uhdr_get_decoded_image(dec);
	if (!raw || !raw->planes[UHDR_PLANE_PACKED] || raw->w == 0 || raw->h == 0) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   "Decoded SDR image unavailable");
		uhdr_release_decoder(dec);
		return NULL;
	}

	out = gdImageCreateTrueColor((int)raw->w, (int)raw->h);
	if (!out) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0,
					   "Failed to allocate SDR gdImage");
		uhdr_release_decoder(dec);
		return NULL;
	}

	stride = raw->stride[UHDR_PLANE_PACKED] ? raw->stride[UHDR_PLANE_PACKED]
											: raw->w;
	for (y = 0; y < (int)raw->h; y++) {
		row = (unsigned char *)raw->planes[UHDR_PLANE_PACKED] +
			  ((size_t)y * stride * 4);
		for (x = 0; x < (int)raw->w; x++) {
			unsigned char r = row[(size_t)x * 4 + 0];
			unsigned char g = row[(size_t)x * 4 + 1];
			unsigned char b = row[(size_t)x * 4 + 2];
			unsigned char a8 = row[(size_t)x * 4 + 3];
			int a7 = gdAlphaMax - (a8 >> 1);
			out->tpixels[y][x] = gdTrueColorAlpha(r, g, b, a7);
		}
	}
	out->saveAlphaFlag = 1;

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	uhdr_release_decoder(dec);
	return out;
#else
	ARG_NOT_USED(im);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}
