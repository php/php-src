/**
 * File: GIF Input
 *
 * Read GIF images.
 */
#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "php.h"

/* Used only when debugging GIF compression code */
/* #define DEBUGGING_ENVARS */

#ifdef DEBUGGING_ENVARS

static int verbose_set = 0;
static int verbose;
#define VERBOSE (verbose_set ? verbose : set_verbose())

static int set_verbose(void)
{
    verbose = !!getenv("GIF_VERBOSE");
    verbose_set = 1;
    return (verbose);
}

#else

#define VERBOSE 0

#endif

#define MAXCOLORMAPSIZE 256

#define TRUE 1
#define FALSE 0

#define CM_RED 0
#define CM_GREEN 1
#define CM_BLUE 2

#define MAX_LWZ_BITS 12

#define INTERLACE 0x40
#define LOCALCOLORMAP 0x80

#define BitSet(byte, bit) (((byte) & (bit)) == (bit))

#define ReadOK(file, buffer, len) (gdGetBuf(buffer, len, file) == len)

#define LM_to_uint(a, b) (((b) << 8) | (a))

/* We may eventually want to use this information, but def it out for now */
#if 0
static struct {
	unsigned int    Width;
	unsigned int    Height;
	unsigned char   ColorMap[3][MAXCOLORMAPSIZE];
	unsigned int    BitPixel;
	unsigned int    ColorResolution;
	unsigned int    Background;
	unsigned int    AspectRatio;
} GifScreen;
#endif

#if 0
static struct {
	int     transparent;
	int     delayTime;
	int     inputFlag;
	int     disposal;
} Gif89 = { -1, -1, -1, 0 };
#endif

#define STACK_SIZE ((1 << (MAX_LWZ_BITS)) * 2)

#define CSD_BUF_SIZE 280

typedef struct {
    unsigned char buf[CSD_BUF_SIZE];
    int curbit;
    int lastbit;
    int done;
    int last_byte;
} CODE_STATIC_DATA;

typedef struct {
    int fresh;
    int code_size, set_code_size;
    int max_code, max_code_size;
    int firstcode, oldcode;
    int clear_code, end_code;
    int table[2][(1 << MAX_LWZ_BITS)];
    int stack[STACK_SIZE], *sp;
    CODE_STATIC_DATA scd;
} LZW_STATIC_DATA;

static int ReadColorMap(gdIOCtx *fd, int number, unsigned char (*buffer)[256]);
static int DoExtension(gdIOCtx *fd, int label, int *Transparent, int *ZeroDataBlockP);
static int GetDataBlock(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP);
static int GetCode(gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag,
                   int *ZeroDataBlockP);
static int LWZReadByte(gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size,
                       int *ZeroDataBlockP);

static int ReadImage(gdImagePtr im, gdIOCtx *fd, int len, int height, unsigned char (*cmap)[256],
                     int colorCount, int interlace, int *ZeroDataBlockP); /*1.4//, int ignore); */

typedef struct {
    int transparent;
    int delay;
    int disposal;
} GifGraphicControl;

typedef struct gdGifReadStruct {
    gdIOCtxPtr in;
    int ownsCtx;
    int done;
    int error;
    int pendingSeparator;
    int frameIndex;
    int screenWidth;
    int screenHeight;
    int backgroundIndex;
    int haveGlobalColormap;
    int globalColorCount;
    int loopCount;
    unsigned char globalColorMap[3][MAXCOLORMAPSIZE];
    unsigned char localColorMap[3][MAXCOLORMAPSIZE];
    GifGraphicControl gce;
    gdImagePtr rawFrame;
    gdImagePtr canvas;
    gdImagePtr previousCanvas;
    gdGifFrameInfo lastInfo;
} gdGifRead;

static void GifResetGraphicControl(GifGraphicControl *gce);
static void GifTrimColorTable(gdImagePtr im);
static int GifReadHeader(gdGifRead *gif);
static int GifPrimeFirstImage(gdGifRead *gif);
static int GifSkipSubBlocks(gdIOCtxPtr in, int *ZeroDataBlockP);
static int GifReadApplicationExtension(gdGifRead *gif, int *ZeroDataBlockP);
static int GifReadExtension(gdGifRead *gif, int label, int *ZeroDataBlockP);
static void GifFillFrameInfo(gdGifRead *gif, gdGifFrameInfo *info);
static int GifFrameToColor(gdImagePtr frame, int color);
static int GifBackgroundColor(gdGifRead *gif, int transparentIndex);
static int GifEnsureCanvas(gdGifRead *gif, int transparentIndex);
static gdImagePtr GifCloneImage(gdImagePtr src);
static void GifApplyPreviousDisposal(gdGifRead *gif);
static int GifCompositeFrame(gdGifRead *gif);
static int GifProbeIsAnimated(gdIOCtxPtr in);

static void GifResetGraphicControl(GifGraphicControl *gce)
{
    gce->transparent = -1;
    gce->delay = 0;
    gce->disposal = gdDisposalUnknown;
}

static void GifTrimColorTable(gdImagePtr im)
{
    int i;

    for (i = im->colorsTotal - 1; i >= 0; i--) {
        if (im->open[i]) {
            im->colorsTotal--;
        } else {
            break;
        }
    }
}

static int GifReadHeader(gdGifRead *gif)
{
    unsigned char buf[16];
    int bitPixel;

    memset(gif->globalColorMap, 0, 3 * MAXCOLORMAPSIZE);
    memset(gif->localColorMap, 0, 3 * MAXCOLORMAPSIZE);
    GifResetGraphicControl(&gif->gce);
    gif->loopCount = 1;

    if (!ReadOK(gif->in, buf, 6)) {
        return 0;
    }
    if (strncmp((char *)buf, "GIF", 3) != 0) {
        return 0;
    }
    if (memcmp((char *)buf + 3, "87a", 3) != 0 && memcmp((char *)buf + 3, "89a", 3) != 0) {
        return 0;
    }
    if (!ReadOK(gif->in, buf, 7)) {
        return 0;
    }

    gif->screenWidth = LM_to_uint(buf[0], buf[1]);
    gif->screenHeight = LM_to_uint(buf[2], buf[3]);
    if (gif->screenWidth <= 0 || gif->screenHeight <= 0) {
        return 0;
    }

    gif->backgroundIndex = buf[5];
    bitPixel = 2 << (buf[4] & 0x07);
    gif->globalColorCount = bitPixel;
    gif->haveGlobalColormap = BitSet(buf[4], LOCALCOLORMAP);
    if (gif->haveGlobalColormap) {
        if (ReadColorMap(gif->in, bitPixel, gif->globalColorMap)) {
            return 0;
        }
    }

    return 1;
}

static int GifSkipSubBlocks(gdIOCtxPtr in, int *ZeroDataBlockP)
{
    unsigned char buf[256];
    int count;

    do {
        count = GetDataBlock(in, buf, ZeroDataBlockP);
        if (count < 0) {
            return 0;
        }
    } while (count > 0);

    return 1;
}

static int GifReadApplicationExtension(gdGifRead *gif, int *ZeroDataBlockP)
{
    unsigned char buf[256];
    int count;

    count = GetDataBlock(gif->in, buf, ZeroDataBlockP);
    if (count < 0) {
        return 0;
    }

    if (count == 11 && memcmp(buf, "NETSCAPE2.0", 11) == 0) {
        count = GetDataBlock(gif->in, buf, ZeroDataBlockP);
        if (count < 0) {
            return 0;
        }
        if (count >= 3 && buf[0] == 1) {
            gif->loopCount = LM_to_uint(buf[1], buf[2]);
        }
        while (count > 0) {
            count = GetDataBlock(gif->in, buf, ZeroDataBlockP);
            if (count < 0) {
                return 0;
            }
        }
        return 1;
    }

    while (count > 0) {
        count = GetDataBlock(gif->in, buf, ZeroDataBlockP);
        if (count < 0) {
            return 0;
        }
    }
    return 1;
}

static int GifReadExtension(gdGifRead *gif, int label, int *ZeroDataBlockP)
{
    unsigned char buf[256];
    int count;

    if (label == 0xf9) {
        count = GetDataBlock(gif->in, buf, ZeroDataBlockP);
        if (count < 0) {
            return 0;
        }
        if (count >= 4) {
            gif->gce.disposal = (buf[0] >> 2) & 0x7;
            if (gif->gce.disposal == 4) {
                gif->gce.disposal = gdDisposalRestorePrevious;
            }
            gif->gce.delay = LM_to_uint(buf[1], buf[2]);
            gif->gce.transparent = (buf[0] & 0x1) ? buf[3] : -1;
        }
        while (count > 0) {
            count = GetDataBlock(gif->in, buf, ZeroDataBlockP);
            if (count < 0) {
                return 0;
            }
        }
        return 1;
    }

    if (label == 0xff) {
        return GifReadApplicationExtension(gif, ZeroDataBlockP);
    }

    return GifSkipSubBlocks(gif->in, ZeroDataBlockP);
}

static int GifPrimeFirstImage(gdGifRead *gif)
{
    unsigned char c;
    int ZeroDataBlock = FALSE;

    for (;;) {
        if (!ReadOK(gif->in, &c, 1)) {
            return 0;
        }
        if (c == ';') {
            gif->done = 1;
            return 1;
        }
        if (c == ',') {
            gif->pendingSeparator = 1;
            return 1;
        }
        if (c == '!') {
            if (!ReadOK(gif->in, &c, 1) || !GifReadExtension(gif, c, &ZeroDataBlock)) {
                return 0;
            }
            continue;
        }
    }
}

static void GifFillFrameInfo(gdGifRead *gif, gdGifFrameInfo *info)
{
    if (info != NULL) {
        *info = gif->lastInfo;
    }
}

static int GifFrameToColor(gdImagePtr frame, int color)
{
    return gdTrueColorAlpha(frame->red[color], frame->green[color], frame->blue[color],
                            frame->alpha[color]);
}

static int GifBackgroundColor(gdGifRead *gif, int transparentIndex)
{
    int bg = gif->backgroundIndex;

    if (bg == transparentIndex) {
        return gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent);
    }
    if (gif->haveGlobalColormap && bg >= 0 && bg < MAXCOLORMAPSIZE) {
        return gdTrueColorAlpha(gif->globalColorMap[CM_RED][bg], gif->globalColorMap[CM_GREEN][bg],
                                gif->globalColorMap[CM_BLUE][bg], gdAlphaOpaque);
    }
    return gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent);
}

static int GifEnsureCanvas(gdGifRead *gif, int transparentIndex)
{
    int x, y, bg;

    if (gif->canvas != NULL) {
        return 1;
    }

    gif->canvas = gdImageCreateTrueColor(gif->screenWidth, gif->screenHeight);
    if (gif->canvas == NULL) {
        return 0;
    }
    gdImageAlphaBlending(gif->canvas, 0);
    gdImageSaveAlpha(gif->canvas, 1);

    bg = GifBackgroundColor(gif, transparentIndex);
    for (y = 0; y < gif->screenHeight; y++) {
        for (x = 0; x < gif->screenWidth; x++) {
            gdImageSetPixel(gif->canvas, x, y, bg);
        }
    }

    return 1;
}

static gdImagePtr GifCloneImage(gdImagePtr src)
{
    gdImagePtr dst;
    int x, y;

    if (src == NULL) {
        return NULL;
    }

    dst =
        src->trueColor ? gdImageCreateTrueColor(src->sx, src->sy) : gdImageCreate(src->sx, src->sy);
    if (dst == NULL) {
        return NULL;
    }

    if (src->trueColor) {
        gdImageAlphaBlending(dst, 0);
        gdImageSaveAlpha(dst, src->saveAlphaFlag);
        for (y = 0; y < src->sy; y++) {
            for (x = 0; x < src->sx; x++) {
                gdImageSetPixel(dst, x, y, gdImageGetPixel(src, x, y));
            }
        }
    } else {
        for (x = 0; x < gdMaxColors; x++) {
            dst->red[x] = src->red[x];
            dst->green[x] = src->green[x];
            dst->blue[x] = src->blue[x];
            dst->alpha[x] = src->alpha[x];
            dst->open[x] = src->open[x];
        }
        dst->colorsTotal = src->colorsTotal;
        dst->transparent = src->transparent;
        for (y = 0; y < src->sy; y++) {
            for (x = 0; x < src->sx; x++) {
                gdImageSetPixel(dst, x, y, gdImageGetPixel(src, x, y));
            }
        }
    }

    return dst;
}

static void GifApplyPreviousDisposal(gdGifRead *gif)
{
    gdGifFrameInfo *info = &gif->lastInfo;
    int x, y, bg;

    if (gif->canvas == NULL || gif->frameIndex <= 0) {
        return;
    }

    if (info->disposal == gdDisposalRestoreBackground) {
        bg = GifBackgroundColor(gif, info->transparentIndex);
        for (y = info->y; y < info->y + info->height; y++) {
            for (x = info->x; x < info->x + info->width; x++) {
                gdImageSetPixel(gif->canvas, x, y, bg);
            }
        }
    } else if (info->disposal == gdDisposalRestorePrevious && gif->previousCanvas != NULL) {
        for (y = info->y; y < info->y + info->height; y++) {
            for (x = info->x; x < info->x + info->width; x++) {
                gdImageSetPixel(gif->canvas, x, y, gdImageGetPixel(gif->previousCanvas, x, y));
            }
        }
    }

    if (gif->previousCanvas != NULL) {
        gdImageDestroy(gif->previousCanvas);
        gif->previousCanvas = NULL;
    }
}

static int GifCompositeFrame(gdGifRead *gif)
{
    gdGifFrameInfo *info = &gif->lastInfo;
    int x, y, c;

    if (!GifEnsureCanvas(gif, info->transparentIndex)) {
        return 0;
    }

    if (info->disposal == gdDisposalRestorePrevious) {
        gif->previousCanvas = GifCloneImage(gif->canvas);
        if (gif->previousCanvas == NULL) {
            return 0;
        }
    }

    for (y = 0; y < info->height; y++) {
        for (x = 0; x < info->width; x++) {
            c = gdImageGetPixel(gif->rawFrame, x, y);
            if (c == info->transparentIndex) {
                continue;
            }
            gdImageSetPixel(gif->canvas, info->x + x, info->y + y,
                            GifFrameToColor(gif->rawFrame, c));
        }
    }

    return 1;
}

static int GifProbeIsAnimated(gdIOCtxPtr in)
{
    unsigned char buf[16], c;
    int bitPixel, frameCount = 0, zero = 0;

    if (in == NULL || !ReadOK(in, buf, 6)) {
        return -1;
    }
    if (strncmp((char *)buf, "GIF", 3) != 0 ||
        (memcmp((char *)buf + 3, "87a", 3) != 0 && memcmp((char *)buf + 3, "89a", 3) != 0)) {
        return -1;
    }
    if (!ReadOK(in, buf, 7)) {
        return -1;
    }
    if (LM_to_uint(buf[0], buf[1]) <= 0 || LM_to_uint(buf[2], buf[3]) <= 0) {
        return -1;
    }
    bitPixel = 2 << (buf[4] & 0x07);
    if (BitSet(buf[4], LOCALCOLORMAP)) {
        while (bitPixel-- > 0) {
            if (!ReadOK(in, buf, 3)) {
                return -1;
            }
        }
    }

    for (;;) {
        if (!ReadOK(in, &c, 1)) {
            return -1;
        }
        if (c == ';') {
            return frameCount > 1 ? 1 : 0;
        }
        if (c == '!') {
            if (!ReadOK(in, &c, 1) || !GifSkipSubBlocks(in, &zero)) {
                return -1;
            }
            continue;
        }
        if (c == ',') {
            int localColorCount;
            if (!ReadOK(in, buf, 9)) {
                return -1;
            }
            localColorCount = BitSet(buf[8], LOCALCOLORMAP) ? (2 << (buf[8] & 0x07)) : 0;
            while (localColorCount-- > 0) {
                if (!ReadOK(in, buf, 3)) {
                    return -1;
                }
            }
            if (!ReadOK(in, &c, 1) || !GifSkipSubBlocks(in, &zero)) {
                return -1;
            }
            frameCount++;
            if (frameCount > 1) {
                return 1;
            }
            continue;
        }
        return -1;
    }
}

BGD_DECLARE(int) gdGifIsAnimated(FILE *fdFile)
{
    gdIOCtx *fd;
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
    result = GifProbeIsAnimated(fd);
    if (!gdSeek(fd, pos)) {
        result = -1;
    }
    fd->gd_free(fd);
    return result;
}

BGD_DECLARE(int) gdGifIsAnimatedCtx(gdIOCtxPtr in)
{
    int result, pos;

    if (in == NULL || in->tell == NULL || in->seek == NULL) {
        return -1;
    }
    pos = (int)gdTell(in);
    if (pos < 0) {
        return -1;
    }
    result = GifProbeIsAnimated(in);
    if (!gdSeek(in, pos)) {
        return -1;
    }
    return result;
}

BGD_DECLARE(int) gdGifIsAnimatedPtr(int size, void *data)
{
    gdIOCtx *in;
    int result;

    if (size <= 0 || data == NULL) {
        return -1;
    }
    in = gdNewDynamicCtxEx(size, data, 0);
    if (in == NULL) {
        return -1;
    }
    result = GifProbeIsAnimated(in);
    in->gd_free(in);
    return result;
}

BGD_DECLARE(gdGifReadPtr) gdGifReadOpen(FILE *fdFile)
{
    gdIOCtx *fd;
    gdGifReadPtr gif;

    if (fdFile == NULL) {
        return NULL;
    }
    fd = gdNewFileCtx(fdFile);
    if (fd == NULL) {
        return NULL;
    }
    gif = gdGifReadOpenCtx(fd);
    if (gif == NULL) {
        fd->gd_free(fd);
        return NULL;
    }
    gif->ownsCtx = 1;
    return gif;
}

BGD_DECLARE(gdGifReadPtr) gdGifReadOpenPtr(int size, void *data)
{
    gdIOCtx *in;
    gdGifReadPtr gif;

    if (size <= 0 || data == NULL) {
        return NULL;
    }
    in = gdNewDynamicCtxEx(size, data, 0);
    if (in == NULL) {
        return NULL;
    }
    gif = gdGifReadOpenCtx(in);
    if (gif == NULL) {
        in->gd_free(in);
        return NULL;
    }
    gif->ownsCtx = 1;
    return gif;
}

BGD_DECLARE(gdGifReadPtr) gdGifReadOpenCtx(gdIOCtxPtr in)
{
    gdGifReadPtr gif;

    if (in == NULL) {
        return NULL;
    }

    gif = (gdGifReadPtr)gdCalloc(1, sizeof(gdGifRead));
    if (gif == NULL) {
        return NULL;
    }
    gif->in = in;
    gif->ownsCtx = 0;
    GifResetGraphicControl(&gif->gce);
    if (!GifReadHeader(gif) || !GifPrimeFirstImage(gif)) {
        gdFree(gif);
        return NULL;
    }

    return gif;
}

BGD_DECLARE(void) gdGifReadClose(gdGifReadPtr gif)
{
    if (gif == NULL) {
        return;
    }
    if (gif->rawFrame != NULL) {
        gdImageDestroy(gif->rawFrame);
    }
    if (gif->canvas != NULL) {
        gdImageDestroy(gif->canvas);
    }
    if (gif->previousCanvas != NULL) {
        gdImageDestroy(gif->previousCanvas);
    }
    if (gif->ownsCtx && gif->in != NULL) {
        gif->in->gd_free(gif->in);
    }
    gdFree(gif);
}

BGD_DECLARE(int) gdGifReadGetInfo(gdGifReadPtr gif, gdGifInfo *info)
{
    if (gif == NULL || info == NULL) {
        return 0;
    }
    info->width = gif->screenWidth;
    info->height = gif->screenHeight;
    info->backgroundIndex = gif->backgroundIndex;
    info->globalColorTable = gif->haveGlobalColormap;
    info->loopCount = gif->loopCount;
    return 1;
}

BGD_DECLARE(int)
gdGifReadNextFrame(gdGifReadPtr gif, gdGifFrameInfo *info, gdImagePtr *frame)
{
    unsigned char buf[16], c;
    int ZeroDataBlock = FALSE;

    if (frame != NULL) {
        *frame = NULL;
    }
    if (gif == NULL || gif->error) {
        return -1;
    }
    if (gif->done) {
        return 0;
    }

    for (;;) {
        int top, left, width, height;
        int useGlobalColormap, bitPixel, interlace, hasLocal;

        if (gif->pendingSeparator) {
            c = ',';
            gif->pendingSeparator = 0;
        } else if (!ReadOK(gif->in, &c, 1)) {
            gif->error = 1;
            return -1;
        }
        if (c == ';') {
            gif->done = 1;
            return 0;
        }
        if (c == '!') {
            if (!ReadOK(gif->in, &c, 1) || !GifReadExtension(gif, c, &ZeroDataBlock)) {
                gif->error = 1;
                return -1;
            }
            continue;
        }
        if (c != ',') {
            continue;
        }

        if (!ReadOK(gif->in, buf, 9)) {
            gif->error = 1;
            return -1;
        }

        hasLocal = BitSet(buf[8], LOCALCOLORMAP);
        useGlobalColormap = !hasLocal;
        bitPixel = 1 << ((buf[8] & 0x07) + 1);
        left = LM_to_uint(buf[0], buf[1]);
        top = LM_to_uint(buf[2], buf[3]);
        width = LM_to_uint(buf[4], buf[5]);
        height = LM_to_uint(buf[6], buf[7]);
        interlace = BitSet(buf[8], INTERLACE);

        if (width <= 0 || height <= 0 || ((left + width) > gif->screenWidth) ||
            ((top + height) > gif->screenHeight)) {
            gif->error = 1;
            return -1;
        }
        if (useGlobalColormap && !gif->haveGlobalColormap) {
            gif->globalColorMap[CM_RED][1] = 0xff;
            gif->globalColorMap[CM_GREEN][1] = 0xff;
            gif->globalColorMap[CM_BLUE][1] = 0xff;
        }

        if (gif->rawFrame != NULL) {
            gdImageDestroy(gif->rawFrame);
            gif->rawFrame = NULL;
        }
        gif->rawFrame = gdImageCreate(width, height);
        if (gif->rawFrame == NULL) {
            gif->error = 1;
            return -1;
        }
        gif->rawFrame->interlace = interlace;

        if (hasLocal) {
            if (ReadColorMap(gif->in, bitPixel, gif->localColorMap) ||
                !ReadImage(gif->rawFrame, gif->in, width, height, gif->localColorMap, bitPixel,
                           interlace, &ZeroDataBlock)) {
                gif->error = 1;
                return -1;
            }
        } else {
            if (!ReadImage(gif->rawFrame, gif->in, width, height, gif->globalColorMap,
                           gif->globalColorCount, interlace, &ZeroDataBlock)) {
                gif->error = 1;
                return -1;
            }
        }

        if (gif->gce.transparent != -1) {
            gdImageColorTransparent(gif->rawFrame, gif->gce.transparent);
        }
        GifTrimColorTable(gif->rawFrame);
        if (!gif->rawFrame->colorsTotal) {
            gif->error = 1;
            return -1;
        }

        gif->lastInfo.frameIndex = gif->frameIndex;
        gif->lastInfo.x = left;
        gif->lastInfo.y = top;
        gif->lastInfo.width = width;
        gif->lastInfo.height = height;
        gif->lastInfo.delay = gif->gce.delay;
        gif->lastInfo.disposal = gif->gce.disposal;
        gif->lastInfo.transparentIndex = gif->gce.transparent;
        gif->lastInfo.localColorTable = hasLocal;
        gif->lastInfo.interlace = interlace;
        gif->frameIndex++;
        GifFillFrameInfo(gif, info);
        if (frame != NULL) {
            *frame = gif->rawFrame;
        }
        GifResetGraphicControl(&gif->gce);
        return 1;
    }
}

BGD_DECLARE(int)
gdGifReadNextImage(gdGifReadPtr gif, gdGifFrameInfo *info, gdImagePtr *image)
{
    int result;

    if (image != NULL) {
        *image = NULL;
    }
    if (gif == NULL) {
        return -1;
    }

    GifApplyPreviousDisposal(gif);
    result = gdGifReadNextFrame(gif, info, NULL);
    if (result <= 0) {
        return result;
    }
    if (!GifCompositeFrame(gif)) {
        gif->error = 1;
        return -1;
    }
    if (image != NULL) {
        *image = gif->canvas;
    }
    return 1;
}

BGD_DECLARE(gdImagePtr) gdGifReadCloneImage(gdGifReadPtr gif)
{
    if (gif == NULL) {
        return NULL;
    }
    return GifCloneImage(gif->canvas);
}

/*
  Function: gdImageCreateFromGif

        <gdImageCreateFromGif> is called to load images from GIF format
        files. Invoke <gdImageCreateFromGif> with an already opened
        pointer to a file containing the desired
        image.

        <gdImageCreateFromGif> returns a <gdImagePtr> to the new image, or
        NULL if unable to load the image (most often because the file is
        corrupt or does not contain a GIF image). <gdImageCreateFromGif>
        does not close the file. You can inspect the sx and sy members of
        the image to determine its size. The image must eventually be
        destroyed using <gdImageDestroy>.

  Variants:

        <gdImageCreateFromGifPtr> creates an image from GIF data (i.e. the
        contents of a GIF file) already in memory.

        <gdImageCreateFromGifCtx> reads in an image using the functions in
        a <gdIOCtx> struct.

  Parameters:

        infile - The input FILE pointer

  Returns:

        A pointer to the new image or NULL if an error occurred.

  Example:

        > gdImagePtr im;
        > ... inside a function ...
        > FILE *in;
        > in = fopen("mygif.gif", "rb");
        > im = gdImageCreateFromGif(in);
        > fclose(in);
        > // ... Use the image ...
        > gdImageDestroy(im);

*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromGif(FILE *fdFile)
{
    gdIOCtx *fd = gdNewFileCtx(fdFile);
    gdImagePtr im;

    if (fd == NULL)
        return NULL;
    im = gdImageCreateFromGifCtx(fd);

    fd->gd_free(fd);

    return im;
}

/*
  Function: gdImageCreateFromGifPtr

  Parameters:

        size - size of GIF data in bytes.
        data - GIF data (i.e. contents of a GIF file).

  See <gdImageCreateFromGif>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifPtr(int size, void *data)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
    if (!in) {
        return 0;
    }
    im = gdImageCreateFromGifCtx(in);
    in->gd_free(in);
    return im;
}

/*
  Function: gdImageCreateFromGifCtx

  See <gdImageCreateFromGif>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifCtx(gdIOCtxPtr fd)
{
    int BitPixel;
#if 0
	int ColorResolution;
	int Background;
	int AspectRatio;
#endif
    int Transparent = (-1);
    unsigned char buf[16];
    unsigned char c;
    unsigned char ColorMap[3][MAXCOLORMAPSIZE];
    unsigned char localColorMap[3][MAXCOLORMAPSIZE];
    int imw, imh, screen_width, screen_height;
    int useGlobalColormap;
    int bitPixel, i;
    /*1.4//int             imageCount = 0; */
    /* 2.0.28: threadsafe storage */
    int ZeroDataBlock = FALSE;
    int haveGlobalColormap;

    gdImagePtr im = 0;

    memset(ColorMap, 0, 3 * MAXCOLORMAPSIZE);
    memset(localColorMap, 0, 3 * MAXCOLORMAPSIZE);

    if (!ReadOK(fd, buf, 6)) {
        return 0;
    }

    if (strncmp((char *)buf, "GIF", 3) != 0) {
        return 0;
    }

    if (memcmp((char *)buf + 3, "87a", 3) == 0) {
        /* GIF87a */
    } else if (memcmp((char *)buf + 3, "89a", 3) == 0) {
        /* GIF89a */
    } else {
        return 0;
    }

    if (!ReadOK(fd, buf, 7)) {
        return 0;
    }

    BitPixel = 2 << (buf[4] & 0x07);
#if 0
	ColorResolution = (int) (((buf[4]&0x70)>>3)+1);
	Background      = buf[5];
	AspectRatio     = buf[6];
#endif
    screen_width = imw = LM_to_uint(buf[0], buf[1]);
    screen_height = imh = LM_to_uint(buf[2], buf[3]);

    haveGlobalColormap = BitSet(buf[4], LOCALCOLORMAP); /* Global Colormap */
    if (haveGlobalColormap) {
        if (ReadColorMap(fd, BitPixel, ColorMap)) {
            return 0;
        }
    }

    for (;;) {
        int top, left;
        int width, height;

        if (!ReadOK(fd, &c, 1)) {
            return 0;
        }

        if (c == ';') { /* GIF terminator */
            goto terminated;
        }

        if (c == '!') { /* Extension */
            if (!ReadOK(fd, &c, 1)) {
                return 0;
            }

            DoExtension(fd, c, &Transparent, &ZeroDataBlock);
            continue;
        }

        if (c != ',') { /* Not a valid start character */
            continue;
        }

        /*1.4//++imageCount; */

        if (!ReadOK(fd, buf, 9)) {
            return 0;
        }

        useGlobalColormap = !BitSet(buf[8], LOCALCOLORMAP);

        bitPixel = 1 << ((buf[8] & 0x07) + 1);
        left = LM_to_uint(buf[0], buf[1]);
        top = LM_to_uint(buf[2], buf[3]);
        width = LM_to_uint(buf[4], buf[5]);
        height = LM_to_uint(buf[6], buf[7]);

        if (((left + width) > screen_width) || ((top + height) > screen_height)) {
            if (VERBOSE) {
                printf("Frame is not confined to screen dimension.\n");
            }
            return 0;
        }

        if (!(im = gdImageCreate(width, height))) {
            return 0;
        }

        im->interlace = BitSet(buf[8], INTERLACE);
        if (!useGlobalColormap) {
            if (ReadColorMap(fd, bitPixel, localColorMap)) {
                gdImageDestroy(im);
                return 0;
            }

            /* Legacy gdImageCreateFromGif* is intentionally tolerant of
             * out-of-palette LZW results and maps them to color 0. The newer
             * iterator API passes the actual color table size and rejects those
             * malformed frames instead.
             */
            if (!ReadImage(im, fd, width, height, localColorMap, 0, BitSet(buf[8], INTERLACE),
                           &ZeroDataBlock)) {
                gdImageDestroy(im);
                return 0;
            }
        } else {
            if (!haveGlobalColormap) {
                // Still a valid gif, apply simple default palette as per spec
                ColorMap[CM_RED][1] = 0xff;
                ColorMap[CM_GREEN][1] = 0xff;
                ColorMap[CM_BLUE][1] = 0xff;
            }

            /* Keep legacy tolerance here as above; strict validation is used by
             * the animated GIF iterator.
             */
            if (!ReadImage(im, fd, width, height, ColorMap, 0, BitSet(buf[8], INTERLACE),
                           &ZeroDataBlock)) {
                gdImageDestroy(im);
                return 0;
            }
        }

        if (Transparent != (-1)) {
            gdImageColorTransparent(im, Transparent);
        }

        goto terminated;
    }

terminated:
    /* Terminator before any image was declared! */
    if (!im) {
        return 0;
    }

    /* Check for open colors at the end, so
     * we can reduce colorsTotal and ultimately
     * BitsPerPixel */
    for (i = im->colorsTotal - 1; i >= 0; i--) {
        if (im->open[i]) {
            im->colorsTotal--;
        } else {
            break;
        }
    }

    if (!im->colorsTotal) {
        gdImageDestroy(im);
        return 0;
    }

    return im;
}

static int ReadColorMap(gdIOCtx *fd, int number, unsigned char (*buffer)[256])
{
    int i;
    unsigned char rgb[3];

    for (i = 0; i < number; ++i) {
        if (!ReadOK(fd, rgb, sizeof(rgb))) {
            return TRUE;
        }

        buffer[CM_RED][i] = rgb[0];
        buffer[CM_GREEN][i] = rgb[1];
        buffer[CM_BLUE][i] = rgb[2];
    }

    return FALSE;
}

static int DoExtension(gdIOCtx *fd, int label, int *Transparent, int *ZeroDataBlockP)
{
    unsigned char buf[256];

    switch (label) {
    case 0xf9:             /* Graphic Control Extension */
        memset(buf, 0, 4); /* initialize a few bytes in the case the next function fails */
        (void)GetDataBlock(fd, (unsigned char *)buf, ZeroDataBlockP);
#if 0
			Gif89.disposal    = (buf[0] >> 2) & 0x7;
			Gif89.inputFlag   = (buf[0] >> 1) & 0x1;
			Gif89.delayTime   = LM_to_uint(buf[1],buf[2]);
#endif
        if ((buf[0] & 0x1) != 0) {
            *Transparent = buf[3];
        }

        while (GetDataBlock(fd, (unsigned char *)buf, ZeroDataBlockP) > 0)
            ;
        return FALSE;

    default:
        break;
    }

    while (GetDataBlock(fd, (unsigned char *)buf, ZeroDataBlockP) > 0)
        ;

    return FALSE;
}

static int GetDataBlock_(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP)
{
    unsigned char count;

    if (!ReadOK(fd, &count, 1)) {
        return -1;
    }

    *ZeroDataBlockP = count == 0;

    if ((count != 0) && (!ReadOK(fd, buf, count))) {
        return -1;
    }

    return count;
}

static int GetDataBlock(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP)
{
    int rv, i;

    rv = GetDataBlock_(fd, buf, ZeroDataBlockP);

    if (VERBOSE) {
        printf("[GetDataBlock returning %d", rv);
        if (rv > 0) {
            printf(":");
            for (i = 0; i < rv; i++) {
                printf(" %02x", buf[i]);
            }
        }
        printf("]\n");
    }

    return rv;
}

static int GetCode_(gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag,
                    int *ZeroDataBlockP)
{
    int i, j, ret;
    int count;

    if (flag) {
        scd->curbit = 0;
        scd->lastbit = 0;
        scd->last_byte = 2;
        scd->done = FALSE;
        return 0;
    }

    if ((scd->curbit + code_size) >= scd->lastbit) {
        if (scd->done) {
            if (scd->curbit >= scd->lastbit) {
                /* Oh well */
            }
            return -1;
        }

        scd->buf[0] = scd->buf[scd->last_byte - 2];
        scd->buf[1] = scd->buf[scd->last_byte - 1];
        scd->last_byte = 2;
        scd->curbit = (scd->curbit - scd->lastbit) + 16;
        scd->lastbit = 16;

        do {
            if (scd->last_byte > (CSD_BUF_SIZE - 255)) {
                return -1;
            }

            if ((count = GetDataBlock(fd, &scd->buf[scd->last_byte], ZeroDataBlockP)) <= 0) {
                scd->done = TRUE;
                break;
            }

            scd->last_byte += count;
            scd->lastbit += count * 8;
        } while ((scd->curbit + code_size) > scd->lastbit);
    }

    if ((scd->curbit + code_size) > scd->lastbit) {
        return -1;
    }

    if ((scd->curbit + code_size - 1) >= (CSD_BUF_SIZE * 8)) {
        ret = -1;
    } else {
        ret = 0;
        for (i = scd->curbit, j = 0; j < code_size; ++i, ++j) {
            ret |= ((scd->buf[i / 8] & (1 << (i % 8))) != 0) << j;
        }
    }

    scd->curbit += code_size;

    return ret;
}

static int GetCode(gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag, int *ZeroDataBlockP)
{
    int rv;

    rv = GetCode_(fd, scd, code_size, flag, ZeroDataBlockP);

    if (VERBOSE) {
        printf("[GetCode(,%d,%d) returning %d]\n", code_size, flag, rv);
    }

    return rv;
}

static int LWZReadByte_(gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size,
                        int *ZeroDataBlockP)
{
    int code, incode, i;

    if (flag) {
        sd->set_code_size = input_code_size;
        sd->code_size = sd->set_code_size + 1;
        sd->clear_code = 1 << sd->set_code_size;
        sd->end_code = sd->clear_code + 1;
        sd->max_code_size = 2 * sd->clear_code;
        sd->max_code = sd->clear_code + 2;

        GetCode(fd, &sd->scd, 0, TRUE, ZeroDataBlockP);

        sd->fresh = TRUE;

        for (i = 0; i < sd->clear_code; ++i) {
            sd->table[0][i] = 0;
            sd->table[1][i] = i;
        }

        for (; i < (1 << MAX_LWZ_BITS); ++i) {
            sd->table[0][i] = sd->table[1][0] = 0;
        }

        sd->sp = sd->stack;

        return 0;

    } else if (sd->fresh) {
        sd->fresh = FALSE;

        do {
            sd->firstcode = sd->oldcode =
                GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP);
        } while (sd->firstcode == sd->clear_code);

        return sd->firstcode;
    }

    if (sd->sp > sd->stack) {
        return *--sd->sp;
    }

    while ((code = GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP)) >= 0) {
        if (code == sd->clear_code) {
            for (i = 0; i < sd->clear_code; ++i) {
                sd->table[0][i] = 0;
                sd->table[1][i] = i;
            }

            for (; i < (1 << MAX_LWZ_BITS); ++i) {
                sd->table[0][i] = sd->table[1][i] = 0;
            }

            sd->code_size = sd->set_code_size + 1;
            sd->max_code_size = 2 * sd->clear_code;
            sd->max_code = sd->clear_code + 2;
            sd->sp = sd->stack;
            sd->firstcode = sd->oldcode =
                GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP);

            return sd->firstcode;
        } else if (code == sd->end_code) {
            int count;
            unsigned char buf[260];

            if (*ZeroDataBlockP) {
                return -2;
            }

            while ((count = GetDataBlock(fd, buf, ZeroDataBlockP)) > 0)
                ;

            if (count != 0) {
                return -2;
            }
        }

        incode = code;

        if (sd->sp == (sd->stack + STACK_SIZE)) {
            /* Bad compressed data stream */
            return -1;
        }

        if (code >= sd->max_code) {
            *sd->sp++ = sd->firstcode;
            code = sd->oldcode;
        }

        while (code >= sd->clear_code) {
            if (sd->sp == (sd->stack + STACK_SIZE)) {
                /* Bad compressed data stream */
                return -1;
            }
            if (code >= (1 << MAX_LWZ_BITS)) {
                /* Corrupted code */
                return -1;
            }

            *sd->sp++ = sd->table[1][code];

            if (code == sd->table[0][code]) {
                /* Oh well */
            }

            code = sd->table[0][code];
        }
        if (code >= (1 << MAX_LWZ_BITS)) {
            /* Corrupted code */
            return -1;
        }

        *sd->sp++ = sd->firstcode = sd->table[1][code];

        if ((code = sd->max_code) < (1 << MAX_LWZ_BITS)) {
            sd->table[0][code] = sd->oldcode;
            sd->table[1][code] = sd->firstcode;
            ++sd->max_code;

            if ((sd->max_code >= sd->max_code_size) && (sd->max_code_size < (1 << MAX_LWZ_BITS))) {
                sd->max_code_size *= 2;
                ++sd->code_size;
            }
        }

        sd->oldcode = incode;

        if (sd->sp > sd->stack) {
            return *--sd->sp;
        }
    }

    return code;
}

static int LWZReadByte(gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size,
                       int *ZeroDataBlockP)
{
    int rv;

    rv = LWZReadByte_(fd, sd, flag, input_code_size, ZeroDataBlockP);

    if (VERBOSE) {
        printf("[LWZReadByte(,%d,%d) returning %d]\n", flag, input_code_size, rv);
    }

    return rv;
}

static int ReadImage(gdImagePtr im, gdIOCtx *fd, int len, int height, unsigned char (*cmap)[256],
                     int colorCount, int interlace, int *ZeroDataBlockP) /*1.4//, int ignore) */
{
    unsigned char c;
    int xpos = 0, ypos = 0, pass = 0;
    int v, i;
    LZW_STATIC_DATA sd;

    /* Initialize the Compression routines */
    if (!ReadOK(fd, &c, 1)) {
        return 0;
    }

    if (c > MAX_LWZ_BITS) {
        return 0;
    }

    /* Stash the color map into the image */
    for (i = 0; (i < gdMaxColors); i++) {
        im->red[i] = cmap[CM_RED][i];
        im->green[i] = cmap[CM_GREEN][i];
        im->blue[i] = cmap[CM_BLUE][i];
        im->open[i] = 1;
    }

    /* Many (perhaps most) of these colors will remain marked open. */
    im->colorsTotal = gdMaxColors;
    if (LWZReadByte(fd, &sd, TRUE, c, ZeroDataBlockP) < 0) {
        return 0;
    }

    /*
     **  If this is an "uninteresting picture" ignore it.
     **  REMOVED For 1.4
     */
    /*if (ignore) { */
    /*        while (LWZReadByte(fd, &sd, FALSE, c) >= 0) */
    /*                ; */
    /*        return; */
    /*} */

    while ((v = LWZReadByte(fd, &sd, FALSE, c, ZeroDataBlockP)) >= 0) {
        if (colorCount > 0 && v >= colorCount) {
            return 0;
        }
        if (v >= gdMaxColors) {
            v = 0;
        }

        /* This how we recognize which colors are actually used. */
        if (im->open[v]) {
            im->open[v] = 0;
        }

        gdImageSetPixel(im, xpos, ypos, v);

        ++xpos;
        if (xpos == len) {
            xpos = 0;
            if (interlace) {
                switch (pass) {
                case 0:
                case 1:
                    ypos += 8;
                    break;
                case 2:
                    ypos += 4;
                    break;
                case 3:
                    ypos += 2;
                    break;
                }

                if (ypos >= height) {
                    ++pass;
                    switch (pass) {
                    case 1:
                        ypos = 4;
                        break;
                    case 2:
                        ypos = 2;
                        break;
                    case 3:
                        ypos = 1;
                        break;
                    default:
                        goto fini;
                    }
                }
            } else {
                ++ypos;
            }
        }

        if (ypos >= height) {
            break;
        }
    }

fini:
    if (LWZReadByte(fd, &sd, FALSE, c, ZeroDataBlockP) >= 0) {
        /* Ignore extra */
    }
    return 1;
}
/* }}} */
