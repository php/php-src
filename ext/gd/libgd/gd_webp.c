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

#define GD_WEBP_ALLOC_STEP (4*1024)

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
	int    width, height;
	uint8_t   *filedata = NULL;
	uint8_t    *argb = NULL;
	size_t size = 0;
	gdImagePtr im;

	filedata = WebpReadCtxData(infile, &size);
	if (filedata == NULL) {
			return NULL;
		}

	if (WebPGetInfo(filedata,size, &width, &height) == 0) {
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

	if (im == NULL) {
		return;
	}

	if (!gdImageTrueColor(im)) {
		gd_error("Palette image not supported by webp");
		return;
	}

	if (quality == -1) {
		quality = 80;
	}

	if (overflow2(gdImageSX(im), 4)) {
		return;
	}

	if (overflow2(gdImageSX(im) * 4, gdImageSY(im))) {
		return;
	}

	argb = (uint8_t *)gdMalloc(gdImageSX(im) * 4 * gdImageSY(im));
	if (!argb) {
		return;
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
		goto freeargb;
	}
	gdPutBuf(out, out_size, outfile);
	free(out);

freeargb:
	gdFree(argb);
}

void gdImageWebpEx (gdImagePtr im, FILE * outFile, int quality)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	gdImageWebpCtx(im, out, quality);
	out->gd_free(out);
}

void gdImageWebp (gdImagePtr im, FILE * outFile)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	gdImageWebpCtx(im, out, -1);
	out->gd_free(out);
}

void * gdImageWebpPtr (gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	gdImageWebpCtx(im, out, -1);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);

	return rv;
}

void * gdImageWebpPtrEx (gdImagePtr im, int *size, int quality)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	gdImageWebpCtx(im, out, quality);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}
#endif /* HAVE_LIBWEBP */
