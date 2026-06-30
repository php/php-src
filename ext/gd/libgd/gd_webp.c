/**
 * File: WebP IO
 *
 * Read and write WebP images.
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

#ifdef HAVE_LIBWEBP
#include "webp/decode.h"
#include "webp/demux.h"
#include "webp/encode.h"
#include "webp/mux.h"

#define GD_WEBP_ALLOC_STEP (4 * 1024)

struct gdWebpReadStruct {
	uint8_t *data;
	size_t size;
	WebPDemuxer *demux;
	WebPAnimDecoder *decoder;
	WebPIterator iter;
	int haveIter;
	int rawIndex;
	int imageIndex;
	int rawTimestamp;
	int imageTimestamp;
	gdImagePtr rawFrame;
	gdImagePtr canvas;
};

struct gdWebpWriteStruct {
	gdIOCtxPtr out;
	int ownsCtx;
	int memoryWriter;
	WebPAnimEncoder *encoder;
	gdWebpWriteOptions options;
	int canvasWidth;
	int canvasHeight;
	int timestamp;
	int frameCount;
	int finalized;
};

static uint8_t *WebpReadCtxData(gdIOCtx *infile, size_t *size) {
	uint8_t *filedata = NULL, *temp, *read;
	ssize_t n;

	*size = 0;
	do {
		temp = gdRealloc(filedata, *size + GD_WEBP_ALLOC_STEP);
		if (temp == NULL) {
			gdFree(filedata);
			gd_error("WebP decode: realloc failed");
			return NULL;
		}
		filedata = temp;
		read = temp + *size;
		n = gdGetBuf(read, GD_WEBP_ALLOC_STEP, infile);
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

static int WebpGdAlphaToWebp(int alpha) {
	if (alpha == gdAlphaTransparent) {
		return 0;
	}
	return 255 - ((alpha << 1) + (alpha >> 6));
}

static int WebpWebpAlphaToGd(uint8_t alpha) {
	return gdAlphaMax - (alpha >> 1);
}

static gdImagePtr WebpImageFromRGBA(const uint8_t *rgba, int width,
									int height) {
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
	gdImageSaveAlpha(im, 1);
	for (y = 0, p = rgba; y < height; y++) {
		for (x = 0; x < width; x++) {
			uint8_t r = *(p++);
			uint8_t g = *(p++);
			uint8_t b = *(p++);
			uint8_t a = *(p++);
			im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, WebpWebpAlphaToGd(a));
		}
	}
	return im;
}

static gdImagePtr WebpImageFromARGB(const uint8_t *argb, int width,
									int height) {
	gdImagePtr im;
	const uint8_t *p;
	int x, y;

	if (argb == NULL || width <= 0 || height <= 0) {
		return NULL;
	}
	im = gdImageCreateTrueColor(width, height);
	if (im == NULL) {
		return NULL;
	}
	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	for (y = 0, p = argb; y < height; y++) {
		for (x = 0; x < width; x++) {
			uint8_t a = *(p++);
			uint8_t r = *(p++);
			uint8_t g = *(p++);
			uint8_t b = *(p++);
			im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, WebpWebpAlphaToGd(a));
		}
	}
	return im;
}

static gdImagePtr WebpCloneImage(gdImagePtr src) {
	gdImagePtr dst;
	int x, y;

	if (src == NULL) {
		return NULL;
	}
	dst = gdImageCreateTrueColor(gdImageSX(src), gdImageSY(src));
	if (dst == NULL) {
		return NULL;
	}
	gdImageAlphaBlending(dst, 0);
	gdImageSaveAlpha(dst, src->saveAlphaFlag);
	for (y = 0; y < gdImageSY(src); y++) {
		for (x = 0; x < gdImageSX(src); x++) {
			dst->tpixels[y][x] = gdImageGetPixel(src, x, y);
		}
	}
	return dst;
}

static void WebpFillInfo(const WebPDemuxer *demux, gdWebpInfo *info) {
	if (info == NULL) {
		return;
	}
	info->width = (int)WebPDemuxGetI(demux, WEBP_FF_CANVAS_WIDTH);
	info->height = (int)WebPDemuxGetI(demux, WEBP_FF_CANVAS_HEIGHT);
	info->frameCount = (int)WebPDemuxGetI(demux, WEBP_FF_FRAME_COUNT);
	info->loopCount = (int)WebPDemuxGetI(demux, WEBP_FF_LOOP_COUNT);
	info->backgroundColor = (int)WebPDemuxGetI(demux, WEBP_FF_BACKGROUND_COLOR);
	info->formatFlags = (int)WebPDemuxGetI(demux, WEBP_FF_FORMAT_FLAGS);
}

static void WebpFillFrameInfo(const WebPIterator *iter, int frameIndex,
							  int timestamp, gdWebpFrameInfo *info) {
	if (info == NULL || iter == NULL) {
		return;
	}
	info->frameIndex = frameIndex;
	info->x = iter->x_offset;
	info->y = iter->y_offset;
	info->width = iter->width;
	info->height = iter->height;
	info->duration = iter->duration;
	info->timestamp = timestamp;
	info->dispose = iter->dispose_method;
	info->blend = iter->blend_method;
	info->hasAlpha = iter->has_alpha;
	info->complete = iter->complete;
}

static int WebpProbeData(const uint8_t *data, size_t size) {
	WebPData webpData;
	WebPDemuxer *demux;
	uint32_t flags;
	int frameCount;

	if (data == NULL || size == 0) {
		return -1;
	}
	webpData.bytes = data;
	webpData.size = size;
	demux = WebPDemux(&webpData);
	if (demux == NULL) {
		return -1;
	}
	flags = WebPDemuxGetI(demux, WEBP_FF_FORMAT_FLAGS);
	frameCount = (int)WebPDemuxGetI(demux, WEBP_FF_FRAME_COUNT);
	WebPDemuxDelete(demux);
	return ((flags & ANIMATION_FLAG) != 0 || frameCount > 1) ? 1 : 0;
}

static gdImagePtr WebpDecodeFirstImage(const uint8_t *filedata, size_t size) {
	WebPData webpData;
	WebPAnimDecoderOptions options;
	WebPAnimDecoder *decoder;
	uint8_t *rgba = NULL;
	int timestamp = 0;
	gdImagePtr im;

	if (!WebPAnimDecoderOptionsInit(&options)) {
		return NULL;
	}
	options.color_mode = MODE_RGBA;
	webpData.bytes = filedata;
	webpData.size = size;
	decoder = WebPAnimDecoderNew(&webpData, &options);
	if (decoder == NULL) {
		return NULL;
	}
	if (!WebPAnimDecoderGetNext(decoder, &rgba, &timestamp)) {
		WebPAnimDecoderDelete(decoder);
		return NULL;
	}
	{
		WebPAnimInfo info;
		if (!WebPAnimDecoderGetInfo(decoder, &info)) {
			WebPAnimDecoderDelete(decoder);
			return NULL;
		}
		im = WebpImageFromRGBA(rgba, (int)info.canvas_width,
							   (int)info.canvas_height);
	}
	WebPAnimDecoderDelete(decoder);
	return im;
}

/*
  Function: gdImageCreateFromWebp

	<gdImageCreateFromWebp> is called to load truecolor images from
	WebP format files. Invoke <gdImageCreateFromWebp> with an
	already opened pointer to a file containing the desired
	image. <gdImageCreateFromWebp> returns a <gdImagePtr> to the new
	truecolor image, or NULL if unable to load the image (most often
	because the file is corrupt or does not contain a WebP
	image). <gdImageCreateFromWebp> does not close the file.

	You can inspect the sx and sy members of the image to determine
	its size. The image must eventually be destroyed using
	<gdImageDestroy>.

	*The returned image is always a truecolor image.*

  Variants:

	<gdImageCreateFromWebpPtr> creates an image from WebP data
	already in memory.

	<gdImageCreateFromWebpCtx> reads its data via the function
	pointers in a <gdIOCtx> structure.

  Parameters:

	infile - The input FILE pointer.

  Returns:

	A pointer to the new *truecolor* image.  This will need to be
	destroyed with <gdImageDestroy> once it is no longer needed.

	On error, returns NULL.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp(FILE *inFile) {
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (!in) {
		return 0;
	}
	im = gdImageCreateFromWebpCtx(in);
	in->gd_free(in);

	return im;
}

/*
  Function: gdImageCreateFromWebpPtr

	See <gdImageCreateFromWebp>.

  Parameters:

	size            - size of WebP data in bytes.
	data            - pointer to WebP data.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr(int size, void *data) {
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in)
		return 0;
	im = gdImageCreateFromWebpCtx(in);
	in->gd_free(in);
	return im;
}

/*
  Function: gdImageCreateFromWebpCtx

	See <gdImageCreateFromWebp>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpCtx(gdIOCtx *infile) {
	int width, height;
	uint8_t *filedata = NULL;
	uint8_t *argb = NULL;
	size_t size = 0;
	gdImagePtr im;

	filedata = WebpReadCtxData(infile, &size);
	if (filedata == NULL) {
		gd_error("gd-webp cannot get webp info");
		return NULL;
	}

	if (WebPGetInfo(filedata, size, &width, &height) == 0) {
		im = WebpDecodeFirstImage(filedata, size);
		if (im == NULL) {
			gd_error("gd-webp cannot get webp info");
		}
		gdFree(filedata);
		return im;
	}
	argb = WebPDecodeARGB(filedata, size, &width, &height);
	if (!argb) {
		im = WebpDecodeFirstImage(filedata, size);
		if (im == NULL) {
			gd_error("gd-webp cannot allocate temporary buffer");
		}
		gdFree(filedata);
		return im;
	}
	im = WebpImageFromARGB(argb, width, height);
	/* do not use gdFree here, in case gdFree/alloc is mapped to something else
	 * than libc */
	WebPFree(argb);
	gdFree(filedata);
	return im;
}

BGD_DECLARE(int) gdWebpIsAnimated(FILE *fdFile) {
	gdIOCtx *fd;
	uint8_t *data;
	size_t size;
	int result, pos;

	if (fdFile == NULL) {
		return -1;
	}
	fd = gdNewFileCtx(fdFile);
	if (fd == NULL) {
		return -1;
	}
	pos = (int)gdTell(fd);
	if (pos < 0) {
		fd->gd_free(fd);
		return -1;
	}
	data = WebpReadCtxData(fd, &size);
	result = WebpProbeData(data, size);
	gdFree(data);
	if (!gdSeek(fd, pos)) {
		result = -1;
	}
	fd->gd_free(fd);
	return result;
}

BGD_DECLARE(int) gdWebpIsAnimatedCtx(gdIOCtxPtr in) {
	uint8_t *data;
	size_t size;
	int result, pos;

	if (in == NULL || in->tell == NULL || in->seek == NULL) {
		return -1;
	}
	pos = (int)gdTell(in);
	if (pos < 0) {
		return -1;
	}
	data = WebpReadCtxData(in, &size);
	result = WebpProbeData(data, size);
	gdFree(data);
	if (!gdSeek(in, pos)) {
		return -1;
	}
	return result;
}

BGD_DECLARE(int) gdWebpIsAnimatedPtr(int size, void *data) {
	if (size <= 0 || data == NULL) {
		return -1;
	}
	return WebpProbeData((const uint8_t *)data, (size_t)size);
}

BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpen(FILE *fdFile) {
	gdIOCtx *fd;
	gdWebpReadPtr webp;

	if (fdFile == NULL) {
		return NULL;
	}
	fd = gdNewFileCtx(fdFile);
	if (fd == NULL) {
		return NULL;
	}
	webp = gdWebpReadOpenCtx(fd);
	fd->gd_free(fd);
	return webp;
}

BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenPtr(int size, void *data) {
	gdIOCtx *in;
	gdWebpReadPtr webp;

	if (size <= 0 || data == NULL) {
		return NULL;
	}
	in = gdNewDynamicCtxEx(size, data, 0);
	if (in == NULL) {
		return NULL;
	}
	webp = gdWebpReadOpenCtx(in);
	in->gd_free(in);
	return webp;
}

BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenCtx(gdIOCtxPtr in) {
	gdWebpReadPtr webp;
	WebPData webpData;
	WebPAnimDecoderOptions decOptions;

	if (in == NULL) {
		return NULL;
	}
	webp = (gdWebpReadPtr)gdCalloc(1, sizeof(struct gdWebpReadStruct));
	if (webp == NULL) {
		return NULL;
	}
	webp->data = WebpReadCtxData(in, &webp->size);
	if (webp->data == NULL) {
		gdFree(webp);
		return NULL;
	}
	webpData.bytes = webp->data;
	webpData.size = webp->size;
	webp->demux = WebPDemux(&webpData);
	if (webp->demux == NULL || !WebPAnimDecoderOptionsInit(&decOptions)) {
		gdWebpReadClose(webp);
		return NULL;
	}
	decOptions.color_mode = MODE_RGBA;
	webp->decoder = WebPAnimDecoderNew(&webpData, &decOptions);
	if (webp->decoder == NULL) {
		gdWebpReadClose(webp);
		return NULL;
	}
	return webp;
}

BGD_DECLARE(void) gdWebpReadClose(gdWebpReadPtr webp) {
	if (webp == NULL) {
		return;
	}
	if (webp->haveIter) {
		WebPDemuxReleaseIterator(&webp->iter);
	}
	if (webp->decoder != NULL) {
		WebPAnimDecoderDelete(webp->decoder);
	}
	if (webp->demux != NULL) {
		WebPDemuxDelete(webp->demux);
	}
	if (webp->rawFrame != NULL) {
		gdImageDestroy(webp->rawFrame);
	}
	if (webp->canvas != NULL) {
		gdImageDestroy(webp->canvas);
	}
	gdFree(webp->data);
	gdFree(webp);
}

BGD_DECLARE(int) gdWebpReadGetInfo(gdWebpReadPtr webp, gdWebpInfo *info) {
	if (webp == NULL || info == NULL || webp->demux == NULL) {
		return 0;
	}
	WebpFillInfo(webp->demux, info);
	return 1;
}

BGD_DECLARE(int)
gdWebpReadNextFrame(gdWebpReadPtr webp, gdWebpFrameInfo *info,
					gdImagePtr *frame) {
	uint8_t *rgba;
	int width, height;

	if (frame != NULL) {
		*frame = NULL;
	}
	if (webp == NULL || webp->demux == NULL) {
		return -1;
	}
	if (webp->haveIter) {
		if (!WebPDemuxNextFrame(&webp->iter)) {
			return 0;
		}
	} else {
		if (!WebPDemuxGetFrame(webp->demux, 1, &webp->iter)) {
			return 0;
		}
		webp->haveIter = 1;
	}
	if (webp->rawFrame != NULL) {
		gdImageDestroy(webp->rawFrame);
		webp->rawFrame = NULL;
	}
	rgba = WebPDecodeRGBA(webp->iter.fragment.bytes, webp->iter.fragment.size,
						  &width, &height);
	if (rgba == NULL) {
		return -1;
	}
	webp->rawFrame = WebpImageFromRGBA(rgba, width, height);
	WebPFree(rgba);
	if (webp->rawFrame == NULL) {
		return -1;
	}
	WebpFillFrameInfo(&webp->iter, webp->rawIndex, webp->rawTimestamp, info);
	webp->rawTimestamp += webp->iter.duration;
	webp->rawIndex++;
	if (frame != NULL) {
		*frame = webp->rawFrame;
	}
	return 1;
}

BGD_DECLARE(int)
gdWebpReadNextImage(gdWebpReadPtr webp, gdWebpFrameInfo *info,
					gdImagePtr *image) {
	uint8_t *rgba;
	int timestamp, duration;
	WebPIterator iter;
	int haveFrameInfo = 0;
	WebPAnimInfo animInfo;

	if (image != NULL) {
		*image = NULL;
	}
	if (webp == NULL || webp->decoder == NULL) {
		return -1;
	}
	if (!WebPAnimDecoderGetNext(webp->decoder, &rgba, &timestamp)) {
		return 0;
	}
	if (!WebPAnimDecoderGetInfo(webp->decoder, &animInfo)) {
		return -1;
	}
	duration = timestamp - webp->imageTimestamp;
	if (duration < 0) {
		duration = 0;
	}
	if (WebPDemuxGetFrame(webp->demux, webp->imageIndex + 1, &iter)) {
		haveFrameInfo = 1;
		WebpFillFrameInfo(&iter, webp->imageIndex, webp->imageTimestamp, info);
		if (info != NULL) {
			info->duration = duration;
		}
		WebPDemuxReleaseIterator(&iter);
	} else if (info != NULL) {
		memset(info, 0, sizeof(*info));
		info->frameIndex = webp->imageIndex;
		info->width = (int)animInfo.canvas_width;
		info->height = (int)animInfo.canvas_height;
		info->duration = duration;
		info->timestamp = webp->imageTimestamp;
	}
	if (webp->canvas != NULL) {
		gdImageDestroy(webp->canvas);
		webp->canvas = NULL;
	}
	webp->canvas = WebpImageFromRGBA(rgba, (int)animInfo.canvas_width,
									 (int)animInfo.canvas_height);
	if (webp->canvas == NULL) {
		return -1;
	}
	if (!haveFrameInfo && info != NULL) {
		info->complete = 1;
	}
	webp->imageTimestamp = timestamp;
	webp->imageIndex++;
	if (image != NULL) {
		*image = webp->canvas;
	}
	return 1;
}

BGD_DECLARE(gdImagePtr) gdWebpReadCloneImage(gdWebpReadPtr webp) {
	if (webp == NULL || webp->canvas == NULL) {
		return NULL;
	}
	return WebpCloneImage(webp->canvas);
}

/* returns 0 on success, 1 on failure */
static int _gdImageWebpCtx(gdImagePtr im, gdIOCtx *outfile, int quality) {
	uint8_t *argb;
	int x, y;
	uint8_t *p;
	uint8_t *out;
	size_t out_size;
	size_t ret = 0;

	if (im == NULL) {
		return 1;
	}

	if (!gdImageTrueColor(im)) {
		gd_error("Palette image not supported by webp");
		return 1;
	}

	if (quality == -1) {
		quality = 80;
	}

	if (overflow2(gdImageSX(im), 4)) {
		return 1;
	}

	if (overflow2(gdImageSX(im) * 4, gdImageSY(im))) {
		return 1;
	}

	argb = (uint8_t *)gdMalloc(gdImageSX(im) * 4 * gdImageSY(im));
	if (!argb) {
		return 1;
	}
	p = argb;
	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			register int c;
			register char a;
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
	}
	if (quality >= gdWebpLossless) {
		out_size = WebPEncodeLosslessRGBA(argb, gdImageSX(im), gdImageSY(im), gdImageSX(im) * 4, &out);
	} else {
		out_size = WebPEncodeRGBA(argb, gdImageSX(im), gdImageSY(im), gdImageSX(im) * 4, quality, &out);
	}
	if (out_size == 0) {
		gd_error("gd-webp encoding failed");
		ret = 1;
		goto freeargb;
	}

	int res = gdPutBuf(out, out_size, outfile);
	WebPFree(out);
	if (res < 0 || (size_t)res != out_size) {
		gd_error("gd-webp write error\n");
		ret = 1;
	}

freeargb:
	gdFree(argb);

	return ret;
}

/*
  Function: gdImageWebpCtx

	Write the image as WebP data via a <gdIOCtx>. See <gdImageWebpEx>
	for more details.

  Parameters:

	im      - The image to write.
	outfile - The output sink.
	quality - Image quality.

  Returns:

	Nothing.
*/
BGD_DECLARE(void) gdImageWebpCtx(gdImagePtr im, gdIOCtx *outfile, int quality) {
	_gdImageWebpCtx(im, outfile, quality);
}

/*
  Function: gdImageWebpEx

	<gdImageWebpEx> outputs the specified image to the specified file in
	WebP format. The file must be open for writing. Under MSDOS and
	all versions of Windows, it is important to use "wb" as opposed to
	simply "w" as the mode when opening the file, and under Unix there
	is no penalty for doing so. <gdImageWebpEx> does not close the file;
	your code must do so.

	If _quality_ is -1, a reasonable quality value (which should yield a
	good general quality / size tradeoff for most situations) is used. Otherwise
	_quality_ should be a value in the range 0-100, higher quality values
	usually implying both higher quality and larger image sizes.

	If _quality_ is greater than or equal to <gdWebpLossless> then the image
	will be written in the lossless WebP format.

  Variants:

	<gdImageWebpCtx> stores the image using a <gdIOCtx> struct.

	<gdImageWebpPtrEx> stores the image to RAM.

  Parameters:

	im      - The image to save.
	outFile - The FILE pointer to write to.
	quality - Compression quality (0-100).

  Returns:

	Nothing.
*/
BGD_DECLARE(void) gdImageWebpEx(gdImagePtr im, FILE *outFile, int quality) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageWebpCtx(im, out, quality);
	out->gd_free(out);
}

/*
  Function: gdImageWebp

	Variant of <gdImageWebpEx> which uses the default quality (-1).

  Parameters:

	im      - The image to save
	outFile - The FILE pointer to write to.

  Returns:

	Nothing.
*/
BGD_DECLARE(void) gdImageWebp(gdImagePtr im, FILE *outFile) {
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageWebpCtx(im, out, -1);
	out->gd_free(out);
}

/*
  Function: gdImageWebpPtr

	See <gdImageWebpEx>.
*/
BGD_DECLARE(void *) gdImageWebpPtr(gdImagePtr im, int *size) {
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageWebpCtx(im, out, -1)) {
		rv = NULL;
	} else {
		rv = gdDPExtractData(out, size);
	}
	out->gd_free(out);

	return rv;
}

/*
  Function: gdImageWebpPtrEx

	See <gdImageWebpEx>.
*/
BGD_DECLARE(void *) gdImageWebpPtrEx(gdImagePtr im, int *size, int quality) {
	void *rv;

	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageWebpCtx(im, out, quality)) {
		rv = NULL;
	} else {
		rv = gdDPExtractData(out, size);
	}
	out->gd_free(out);
	return rv;
}

static void WebpWriteFree(gdWebpWritePtr webp) {
	if (webp == NULL) {
		return;
	}
	if (webp->encoder != NULL) {
		WebPAnimEncoderDelete(webp->encoder);
	}
	if (webp->ownsCtx && webp->out != NULL) {
		webp->out->gd_free(webp->out);
	}
	gdFree(webp);
}

static int WebpWriteEnsureEncoder(gdWebpWritePtr webp, gdImagePtr image) {
	WebPAnimEncoderOptions encOptions;

	if (webp->encoder != NULL) {
		return 1;
	}
	if (image == NULL) {
		return 0;
	}
	webp->canvasWidth = webp->options.canvasWidth > 0
							? webp->options.canvasWidth
							: gdImageSX(image);
	webp->canvasHeight = webp->options.canvasHeight > 0
							 ? webp->options.canvasHeight
							 : gdImageSY(image);
	if (webp->canvasWidth <= 0 || webp->canvasHeight <= 0) {
		return 0;
	}
	if (!WebPAnimEncoderOptionsInit(&encOptions)) {
		return 0;
	}
	encOptions.anim_params.loop_count = webp->options.loopCount;
	encOptions.anim_params.bgcolor = (uint32_t)webp->options.backgroundColor;
	if (webp->options.minimizeSize) {
		encOptions.minimize_size = webp->options.minimizeSize;
	}
	if (webp->options.kmin || webp->options.kmax) {
		encOptions.kmin = webp->options.kmin;
		encOptions.kmax = webp->options.kmax;
	}
	if (webp->options.allowMixed) {
		encOptions.allow_mixed = webp->options.allowMixed;
	}
	webp->encoder =
		WebPAnimEncoderNew(webp->canvasWidth, webp->canvasHeight, &encOptions);
	return webp->encoder != NULL;
}

static int WebpImageToRGBA(gdImagePtr im, uint8_t **rgba) {
	uint8_t *p;
	int x, y;

	*rgba = NULL;
	if (im == NULL || !gdImageTrueColor(im)) {
		gd_error("Palette image not supported by webp");
		return 0;
	}
	if (overflow2(gdImageSX(im), 4) ||
		overflow2(gdImageSX(im) * 4, gdImageSY(im))) {
		return 0;
	}
	*rgba = (uint8_t *)gdMalloc(gdImageSX(im) * 4 * gdImageSY(im));
	if (*rgba == NULL) {
		return 0;
	}
	p = *rgba;
	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			int c = im->tpixels[y][x];
			*(p++) = gdTrueColorGetRed(c);
			*(p++) = gdTrueColorGetGreen(c);
			*(p++) = gdTrueColorGetBlue(c);
			*(p++) = WebpGdAlphaToWebp(gdTrueColorGetAlpha(c));
		}
	}
	return 1;
}

static int WebpWriteAssemble(gdWebpWritePtr webp) {
	WebPData webpData;
	int ok = 0;

	if (webp == NULL || webp->out == NULL || webp->finalized) {
		return 0;
	}
	if (webp->encoder == NULL || webp->frameCount == 0) {
		gd_error("gd-webp animation has no frames");
		return 0;
	}
	WebPDataInit(&webpData);
	if (!WebPAnimEncoderAdd(webp->encoder, NULL, webp->timestamp, NULL)) {
		gd_error("gd-webp animation flush failed: %s",
				 WebPAnimEncoderGetError(webp->encoder));
		goto done;
	}
	if (!WebPAnimEncoderAssemble(webp->encoder, &webpData)) {
		gd_error("gd-webp animation assembly failed: %s",
				 WebPAnimEncoderGetError(webp->encoder));
		goto done;
	}
	if ((size_t)gdPutBuf(webpData.bytes, webpData.size, webp->out) !=
		webpData.size) {
		gd_error("gd-webp animation write error");
		goto done;
	}
	webp->finalized = 1;
	ok = 1;
done:
	WebPDataClear(&webpData);
	return ok;
}

BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpen(FILE *outFile, const gdWebpWriteOptions *options) {
	gdIOCtx *out;
	gdWebpWritePtr webp;

	if (outFile == NULL) {
		return NULL;
	}
	out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return NULL;
	}
	webp = gdWebpWriteOpenCtx(out, options);
	if (webp == NULL) {
		out->gd_free(out);
		return NULL;
	}
	webp->ownsCtx = 1;
	return webp;
}

BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenCtx(gdIOCtxPtr out, const gdWebpWriteOptions *options) {
	gdWebpWritePtr webp;

	if (out == NULL) {
		return NULL;
	}
	webp = (gdWebpWritePtr)gdCalloc(1, sizeof(struct gdWebpWriteStruct));
	if (webp == NULL) {
		return NULL;
	}
	webp->out = out;
	webp->ownsCtx = 0;
	if (options != NULL) {
		webp->options = *options;
	}
	if (webp->options.quality == 0) {
		webp->options.quality = -1;
	}
	return webp;
}

BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenPtr(const gdWebpWriteOptions *options) {
	gdIOCtx *out;
	gdWebpWritePtr webp;

	out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	webp = gdWebpWriteOpenCtx(out, options);
	if (webp == NULL) {
		out->gd_free(out);
		return NULL;
	}
	webp->ownsCtx = 1;
	webp->memoryWriter = 1;
	return webp;
}

BGD_DECLARE(int)
gdWebpWriteAddImage(gdWebpWritePtr webp, gdImagePtr image, int durationMs) {
	WebPConfig config;
	WebPPicture picture;
	uint8_t *rgba = NULL;
	int ok = 0;

	if (webp == NULL || image == NULL || durationMs < 0 || webp->finalized) {
		return 0;
	}
	if (!WebpWriteEnsureEncoder(webp, image)) {
		return 0;
	}
	if (gdImageSX(image) != webp->canvasWidth ||
		gdImageSY(image) != webp->canvasHeight) {
		gd_error("gd-webp animation frames must match canvas size");
		return 0;
	}
	if (!WebpImageToRGBA(image, &rgba)) {
		return 0;
	}
	if (!WebPConfigInit(&config) || !WebPPictureInit(&picture)) {
		goto done;
	}
	config.quality =
		webp->options.quality == -1 ? 80.0f : (float)webp->options.quality;
	if (webp->options.lossless || webp->options.quality >= gdWebpLossless) {
		config.lossless = 1;
		if (config.quality > 100.0f) {
			config.quality = 100.0f;
		}
	}
	if (webp->options.method >= 0) {
		config.method = webp->options.method;
	}
	if (!WebPValidateConfig(&config)) {
		gd_error("gd-webp invalid animation encoder configuration");
		goto done;
	}
	picture.width = gdImageSX(image);
	picture.height = gdImageSY(image);
	picture.use_argb = 1;
	if (!WebPPictureImportRGBA(&picture, rgba, gdImageSX(image) * 4)) {
		goto free_picture;
	}
	if (!WebPAnimEncoderAdd(webp->encoder, &picture, webp->timestamp,
							&config)) {
		gd_error("gd-webp animation add frame failed: %s",
				 WebPAnimEncoderGetError(webp->encoder));
		goto free_picture;
	}
	webp->timestamp += durationMs;
	webp->frameCount++;
	ok = 1;
free_picture:
	WebPPictureFree(&picture);
done:
	gdFree(rgba);
	return ok;
}

BGD_DECLARE(void) gdWebpWriteClose(gdWebpWritePtr webp) {
	if (webp == NULL) {
		return;
	}
	if (!webp->memoryWriter) {
		WebpWriteAssemble(webp);
	}
	WebpWriteFree(webp);
}

BGD_DECLARE(void *) gdWebpWritePtrFinish(gdWebpWritePtr webp, int *size) {
	void *rv = NULL;

	if (size != NULL) {
		*size = 0;
	}
	if (webp == NULL || !webp->memoryWriter) {
		WebpWriteFree(webp);
		return NULL;
	}
	if (WebpWriteAssemble(webp)) {
		rv = gdDPExtractData(webp->out, size);
	}
	WebpWriteFree(webp);
	return rv;
}

#else /* !HAVE_LIBWEBP */

static void _noWebpError(void) {
	gd_error("WEBP image support has been disabled\n");
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp(FILE *inFile) {
	ARG_NOT_USED(inFile);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr(int size, void *data) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpCtx(gdIOCtx *infile) {
	ARG_NOT_USED(infile);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(void) gdImageWebpCtx(gdImagePtr im, gdIOCtx *outfile, int quality) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outfile);
	ARG_NOT_USED(quality);
	_noWebpError();
}

BGD_DECLARE(void) gdImageWebpEx(gdImagePtr im, FILE *outFile, int quality) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outFile);
	ARG_NOT_USED(quality);
	_noWebpError();
}

BGD_DECLARE(void) gdImageWebp(gdImagePtr im, FILE *outFile) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(outFile);
	_noWebpError();
}

BGD_DECLARE(void *) gdImageWebpPtr(gdImagePtr im, int *size) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(void *) gdImageWebpPtrEx(gdImagePtr im, int *size, int quality) {
	ARG_NOT_USED(im);
	ARG_NOT_USED(size);
	ARG_NOT_USED(quality);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(int) gdWebpIsAnimated(FILE *fd) {
	ARG_NOT_USED(fd);
	_noWebpError();
	return -1;
}

BGD_DECLARE(int) gdWebpIsAnimatedCtx(gdIOCtxPtr in) {
	ARG_NOT_USED(in);
	_noWebpError();
	return -1;
}

BGD_DECLARE(int) gdWebpIsAnimatedPtr(int size, void *data) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	_noWebpError();
	return -1;
}

BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpen(FILE *fd) {
	ARG_NOT_USED(fd);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenCtx(gdIOCtxPtr in) {
	ARG_NOT_USED(in);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenPtr(int size, void *data) {
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(void) gdWebpReadClose(gdWebpReadPtr webp) {
	ARG_NOT_USED(webp);
	_noWebpError();
}

BGD_DECLARE(int) gdWebpReadGetInfo(gdWebpReadPtr webp, gdWebpInfo *info) {
	ARG_NOT_USED(webp);
	ARG_NOT_USED(info);
	_noWebpError();
	return 0;
}

BGD_DECLARE(int)
gdWebpReadNextFrame(gdWebpReadPtr webp, gdWebpFrameInfo *info,
					gdImagePtr *frame) {
	ARG_NOT_USED(webp);
	ARG_NOT_USED(info);
	ARG_NOT_USED(frame);
	_noWebpError();
	return -1;
}

BGD_DECLARE(int)
gdWebpReadNextImage(gdWebpReadPtr webp, gdWebpFrameInfo *info,
					gdImagePtr *image) {
	ARG_NOT_USED(webp);
	ARG_NOT_USED(info);
	ARG_NOT_USED(image);
	_noWebpError();
	return -1;
}

BGD_DECLARE(gdImagePtr) gdWebpReadCloneImage(gdWebpReadPtr webp) {
	ARG_NOT_USED(webp);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpen(FILE *outFile, const gdWebpWriteOptions *options) {
	ARG_NOT_USED(outFile);
	ARG_NOT_USED(options);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenCtx(gdIOCtxPtr out, const gdWebpWriteOptions *options) {
	ARG_NOT_USED(out);
	ARG_NOT_USED(options);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenPtr(const gdWebpWriteOptions *options) {
	ARG_NOT_USED(options);
	_noWebpError();
	return NULL;
}

BGD_DECLARE(int)
gdWebpWriteAddImage(gdWebpWritePtr webp, gdImagePtr image, int durationMs) {
	ARG_NOT_USED(webp);
	ARG_NOT_USED(image);
	ARG_NOT_USED(durationMs);
	_noWebpError();
	return 0;
}

BGD_DECLARE(void) gdWebpWriteClose(gdWebpWritePtr webp) {
	ARG_NOT_USED(webp);
	_noWebpError();
}

BGD_DECLARE(void *) gdWebpWritePtrFinish(gdWebpWritePtr webp, int *size) {
	ARG_NOT_USED(webp);
	ARG_NOT_USED(size);
	_noWebpError();
	return NULL;
}

#endif /* HAVE_LIBWEBP */
