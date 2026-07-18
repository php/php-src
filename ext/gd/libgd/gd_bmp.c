/*
        gd_bmp.c

        Bitmap format support for libgd

        * Written 2007, Scott MacVicar
        ---------------------------------------------------------------------------

        Todo:

        Bitfield encoding

        ----------------------------------------------------------------------------
 */

/**
 * File: BMP IO
 *
 * Read and write BMP images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bmp.h"
#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__has_builtin) && __has_builtin(__builtin_assume)
#define GD_ASSUME(expr) __builtin_assume(expr)
#elif defined(__GNUC__)
#define GD_ASSUME(expr)                                                                            \
    do {                                                                                           \
        if (!(expr))                                                                               \
            __builtin_unreachable();                                                               \
    } while (0)
#else
#define GD_ASSUME(expr) ((void)(expr))
#endif

static int compress_row(unsigned char *uncompressed_row, int length);
static int build_rle_packet(unsigned char *row, int packet_type, int length, unsigned char *data);

static int bmp_read_header(gdIOCtxPtr infile, bmp_hdr_t *hdr);
static int bmp_read_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_windows_v3_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_os2_v1_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_os2_v2_info(gdIOCtxPtr infile, bmp_info_t *info);

static int bmp_read_direct(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_1bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_2bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_4bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_8bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_rle(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info);

typedef struct {
    int bpp;
    int compression;
    int header_ver;
    int row_stride;
    int bitmap_size;
    int info_size;
    int palette_size;
    int mask_size;
    unsigned int red_mask;
    unsigned int green_mask;
    unsigned int blue_mask;
    unsigned int alpha_mask;
} bmp_write_ctx_t;

static int _gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int bpp, int compression, int flags);
static int bmp_resolve_write_ctx(gdImagePtr im, int bpp_hint, int compression, int flags,
                                 bmp_write_ctx_t *ctx);
static int bmp_auto_bpp(gdImagePtr im);
static int bmp_has_alpha(gdImagePtr im);
static int bmp_prepare_write_image(gdImagePtr im, int bpp, int flags, gdImagePtr *write_im);
static void bmp_write_file_header(gdIOCtxPtr out, bmp_write_ctx_t *ctx);
static void bmp_write_info_header(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static void bmp_write_palette(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_pixels(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_pixels_1bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_pixels_4bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_pixels_8bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_pixels_16bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_pixels_24bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_pixels_32bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx);
static int bmp_write_rle4_row(gdIOCtxPtr out, gdImagePtr im, int row);
static int bmp_write_padding(gdIOCtxPtr out, int count);
static int bmp_write_tmpfile_to_ctx(gdIOCtxPtr out, gdIOCtxPtr out_original);

static int bmp_validate_info(bmp_info_t *info, bmp_hdr_t *hdr);
static int bmp_read_bitfield_masks(gdIOCtxPtr infile, bmp_info_t *info, int read_alpha);
static int bmp_skip_bytes(gdIOCtxPtr infile, int count);
static int bmp_check_palette_index(gdImagePtr im, int index);
static int bmp_row_padding(int width, int depth, int *padding);
static int bmp_image_size(int width, int height, int depth, int *size);
static int bmp_get_mask_shift(unsigned int mask);
static int bmp_get_mask_bits(unsigned int mask);
static int bmp_extract_mask(unsigned int pixel, unsigned int mask);
static unsigned int bmp_pack_mask(int channel_8bit, unsigned int mask);
static int bmp_alpha_to_gd(int alpha);
static int bmp_gd_to_alpha(int gd_alpha);

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

        Outputs the given image as BMP data, but using a <gdIOCtx> instead
        of a file. See <gdImageBmp>.

        This is the legacy BMP memory API. A zero <compression> value writes
        uncompressed BMP data. A nonzero <compression> value requests legacy
        RLE output when the automatically selected BMP bit depth supports RLE.
        For explicit bit depth, compression, and conversion control, use
        <gdImageBmpPtrEx>.

        Parameters:
                im			- the image to save.
                size 		- Output: size in bytes of the result.
                compression - whether to apply RLE or not.

        Returns:

                A pointer to memory containing the image data or NULL on error.
*/
BGD_DECLARE(void *) gdImageBmpPtr(gdImagePtr im, int *size, int compression)
{
    return gdImageBmpPtrEx(im, size, 0, compression ? -1 : GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE);
}

/*
        Function: gdImageBmpPtrEx

                <gdImageBmpPtrEx> outputs the given image as BMP data in memory.
                See <gdImageBmpEx> for the meaning of <bpp>, <compression>, and
                <flags>.

        Parameters:

                im          - The image to save.
                size        - Output: size in bytes of the returned data.
                bpp         - Requested output bit depth, or 0 for automatic selection.
                compression - Requested BMP compression mode.
                flags       - BMP writer option flags.

        Returns:

                A pointer to memory containing the image data, or NULL on error.
                The returned memory must be freed with <gdFree>.
*/
BGD_DECLARE(void *)
gdImageBmpPtrEx(gdImagePtr im, int *size, int bpp, int compression, int flags)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL)
        return NULL;
    if (!_gdImageBmpCtx(im, out, bpp, compression, flags))
        rv = gdDPExtractData(out, size);
    else
        rv = NULL;
    out->gd_free(out);
    return rv;
}

/*
        Function: gdImageBmp

        <gdImageBmp> outputs the specified image to the specified file in
        BMP format. The file must be open for writing. Under MSDOS and all
        versions of Windows, it is important to use "wb" as opposed to
        simply "w" as the mode when opening the file, and under Unix there
        is no penalty for doing so. <gdImageBmp> does not close the file;
        your code must do so.

        In addition, <gdImageBmp> allows to specify whether RLE compression
        should be applied.

        This is the legacy BMP file API. A zero <compression> value writes
        uncompressed BMP data. A nonzero <compression> value requests legacy
        RLE output when the automatically selected BMP bit depth supports RLE.
        For explicit bit depth, compression, and conversion control, use
        <gdImageBmpEx>.

        Variants:

                <gdImageBmpCtx> write via a <gdIOCtx> instead of a file handle.

                <gdImageBmpPtr> store the image file to memory.

        Parameters:

                im			- the image to save.
                outFile		- the output FILE* object.
                compression - whether to apply RLE or not.

        Returns:
                nothing
*/
BGD_DECLARE(void) gdImageBmp(gdImagePtr im, FILE *outFile, int compression)
{
    gdImageBmpEx(im, outFile, 0, compression ? -1 : GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE);
}

/*
        Function: gdImageBmpEx

                <gdImageBmpEx> outputs the specified image to the specified file in
                BMP format. The file must be open for writing. Under MSDOS and all
                versions of Windows, it is important to use "wb" as opposed to
                simply "w" as the mode when opening the file, and under Unix there
                is no penalty for doing so. <gdImageBmpEx> does not close the file;
                your code must do so.

                This extended BMP writer supports 1, 4, 8, 16, 24, and 32 bits per
                pixel. It writes Windows BMP headers only: BITMAPINFOHEADER for most
                outputs, BITMAPV4HEADER for 32 bpp alpha output or when
                <GD_BMP_FLAG_FORCE_V4HDR> is set. It does not write OS/2 BMP
                headers, 2 bpp BMPs, top-down BMPs, custom bit masks, V5 color
                profiles, embedded JPEG/PNG BMP data, or non-standard high-depth
                bitfield formats.

        Bit depth:

                Pass 0 for automatic bit depth selection. Automatic selection writes
                truecolor images with non-opaque pixels as 32 bpp, opaque truecolor
                images as 24 bpp, palette images with 2 or fewer colors as 1 bpp,
                palette images with 16 or fewer colors as 4 bpp, and other palette
                images as 8 bpp.

                Explicit <bpp> values must be one of 1, 4, 8, 16, 24, or 32.
                Indexed output at 1, 4, or 8 bpp requires a palette image with no
                more colors than the selected bit depth can store. If the source is
                truecolor, explicit indexed output is a lossy conversion and fails
                unless <GD_BMP_FLAG_QUANTIZE> is set. With that flag, the writer
                clones the image, converts the clone with <gdImageTrueColorToPalette>,
                writes the clone, and leaves the caller's image unchanged.

        Compression:

                <GD_BMP_COMPRESS_NONE> writes uncompressed BMP pixels.
                <GD_BMP_COMPRESS_RLE4> is valid only for 4 bpp output and writes
                BI_RLE4. <GD_BMP_COMPRESS_RLE8> is valid only for 8 bpp output and
                writes BI_RLE8. Any invalid bit depth/compression combination fails
                before producing a valid BMP.

                16 bpp and 32 bpp output use BI_BITFIELDS automatically, regardless
                of the requested compression. 16 bpp defaults to RGB565 masks; set
                <GD_BMP_FLAG_RGB555> to write RGB555 masks instead. 32 bpp writes
                red, green, blue, and alpha masks and stores alpha as BMP opacity
                (255 is opaque), converted from gd's alpha representation.

                Palette alpha is not preserved in 1, 4, or 8 bpp BMP output. The
                writer stores indexed palettes as B, G, R, reserved byte entries,
                and writes the reserved byte as zero. Truecolor images that need
                alpha preservation should be written as 32 bpp, either explicitly or
                by using automatic bit depth selection on an image with non-opaque
                pixels. If a truecolor image is quantized to indexed BMP with
                <GD_BMP_FLAG_QUANTIZE>, alpha is not preserved in the indexed BMP.

        Flags:

                <GD_BMP_FLAG_NONE> selects default behavior.
                <GD_BMP_FLAG_QUANTIZE> allows explicit lossy truecolor to indexed
                conversion for 1, 4, or 8 bpp output.
                <GD_BMP_FLAG_RGB555> selects RGB555 masks for 16 bpp output.
                <GD_BMP_FLAG_FORCE_V4HDR> writes a BITMAPV4HEADER even when it is
                not otherwise required.

        Variants:

                <gdImageBmpPtrEx> stores the image in RAM.

                <gdImageBmpCtxEx> writes using a <gdIOCtx> struct.

                <gdImageBmp>, <gdImageBmpPtr>, and <gdImageBmpCtx> are legacy
                wrappers using automatic bit depth selection and no flags.

        Parameters:

                im          - The image to save.
                outFile     - The FILE pointer to write to.
                bpp         - Requested bit depth, or 0 for automatic selection.
                compression - One of <GD_BMP_COMPRESS_NONE>,
                                          <GD_BMP_COMPRESS_RLE4>, or <GD_BMP_COMPRESS_RLE8>.
                flags       - A bitwise OR of <GD_BMP_FLAG_NONE>,
                                          <GD_BMP_FLAG_FORCE_V4HDR>, <GD_BMP_FLAG_QUANTIZE>,
                                          and <GD_BMP_FLAG_RGB555>.

        Returns:

                For <gdImageBmpEx> and <gdImageBmpCtxEx>, nothing.
                For <gdImageBmpPtrEx>, a pointer to the image in memory, or NULL
                on error.
*/
BGD_DECLARE(void)
gdImageBmpEx(gdImagePtr im, FILE *outFile, int bpp, int compression, int flags)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    if (out == NULL)
        return;
    gdImageBmpCtxEx(im, out, bpp, compression, flags);
    out->gd_free(out);
}

/*
        Function: gdImageBmpCtx

        Outputs the given image as BMP data, but using a <gdIOCtx> instead
        of a file. See <gdImageBmp>.

        This is the legacy BMP context API. A zero <compression> value writes
        uncompressed BMP data. A nonzero <compression> value requests legacy
        RLE output when the automatically selected BMP bit depth supports RLE.
        For explicit bit depth, compression, and conversion control, use
        <gdImageBmpCtxEx>.

        Parameters:
                im			- the image to save.
                out 		- the <gdIOCtx> to write to.
                compression - whether to apply RLE or not.
*/
BGD_DECLARE(void)
gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression)
{
    gdImageBmpCtxEx(im, out, 0, compression ? -1 : GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE);
}

/*
        Function: gdImageBmpCtxEx

                <gdImageBmpCtxEx> outputs the given image as BMP data using a
                <gdIOCtx> structure. See <gdImageBmpEx> for the meaning of <bpp>,
                <compression>, and <flags>, including automatic bit depth selection,
                RLE restrictions, quantization behavior, bitfield output, and
                unsupported BMP variants.

        Parameters:

                im          - The image to save.
                out         - The <gdIOCtx> to write to.
                bpp         - Requested output bit depth, or 0 for automatic selection.
                compression - Requested BMP compression mode.
                flags       - BMP writer option flags.
*/
BGD_DECLARE(void)
gdImageBmpCtxEx(gdImagePtr im, gdIOCtxPtr out, int bpp, int compression, int flags)
{
    _gdImageBmpCtx(im, out, bpp, compression, flags);
}

static int _gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int bpp, int compression, int flags)
{
    bmp_write_ctx_t ctx;
    int error = 0;
    FILE *tmpfile_for_compression = NULL;
    gdIOCtxPtr out_original = NULL;
    int ret = 1;
    gdImagePtr write_im = im;

    if (im == NULL || out == NULL) {
        return 1;
    }

    if (gdImageSX(im) <= 0 || gdImageSY(im) <= 0) {
        gd_error("image dimensions must be greater than 0");
        return 1;
    }

    if (overflow2(gdImageSX(im), gdImageSY(im))) {
        gd_error("image dimensions are too large");
        return 1;
    }

    if (bmp_prepare_write_image(im, bpp, flags, &write_im)) {
        return 1;
    }

    if (bmp_resolve_write_ctx(write_im, bpp, compression, flags, &ctx)) {
        goto cleanup;
    }

    if ((ctx.compression == BMP_BI_RLE8 || ctx.compression == BMP_BI_RLE4) && !out->seek) {
        /* Try to create a temp file where we can seek */
        if ((tmpfile_for_compression = tmpfile()) == NULL) {
            ctx.compression = BMP_BI_RGB;
            if (ctx.bpp == 4 || ctx.bpp == 8) {
                ctx.bitmap_size = ctx.row_stride * write_im->sy;
            }
        } else {
            out_original = out;
            if ((out = (gdIOCtxPtr)gdNewFileCtx(tmpfile_for_compression)) == NULL) {
                out = out_original;
                out_original = NULL;
                ctx.compression = BMP_BI_RGB;
                if (ctx.bpp == 4 || ctx.bpp == 8) {
                    ctx.bitmap_size = ctx.row_stride * write_im->sy;
                }
            }
        }
    }

    bmp_write_file_header(out, &ctx);
    bmp_write_info_header(out, write_im, &ctx);
    if (ctx.palette_size) {
        bmp_write_palette(out, write_im, &ctx);
    }

    error = bmp_write_pixels(out, write_im, &ctx);

    if (!error && (ctx.compression == BMP_BI_RLE8 || ctx.compression == BMP_BI_RLE4)) {
        gdPutC(BMP_RLE_COMMAND, out);
        gdPutC(BMP_RLE_ENDOFBITMAP, out);
        ctx.bitmap_size += 2;

        gdSeek(out, 2);
        gdBMPPutInt(out, 14 + ctx.info_size + ctx.bitmap_size);

        gdSeek(out, 34);
        gdBMPPutInt(out, ctx.bitmap_size);
    }

    /* If we needed a tmpfile for compression copy it over to out_original */
    if (!error && tmpfile_for_compression && bmp_write_tmpfile_to_ctx(out, out_original)) {
        error = 1;
    }
    if (tmpfile_for_compression && out_original) {
        out->gd_free(out);
        out = out_original;
        out_original = NULL;
    }

    ret = error;
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
    if (write_im != im) {
        gdImageDestroy(write_im);
    }
    return ret;
}

static int bmp_prepare_write_image(gdImagePtr im, int bpp, int flags, gdImagePtr *write_im)
{
    gdImagePtr clone;

    *write_im = im;
    if ((bpp == 1 || bpp == 4 || bpp == 8) && im->trueColor) {
        if (!(flags & GD_BMP_FLAG_QUANTIZE)) {
            return 1;
        }
        clone = gdImageClone(im);
        if (clone == NULL) {
            return 1;
        }
        if (!gdImageTrueColorToPalette(clone, 0, 1 << bpp)) {
            gdImageDestroy(clone);
            return 1;
        }
        *write_im = clone;
    }
    return 0;
}

static int bmp_auto_bpp(gdImagePtr im)
{
    if (im->trueColor) {
        return bmp_has_alpha(im) ? 32 : 24;
    }
    if (im->colorsTotal <= 2) {
        return 1;
    }
    if (im->colorsTotal <= 16) {
        return 4;
    }
    return 8;
}

static int bmp_has_alpha(gdImagePtr im)
{
    int x, y;

    for (y = 0; y < im->sy; y++) {
        for (x = 0; x < im->sx; x++) {
            if (gdTrueColorGetAlpha(gdImageGetPixel(im, x, y)) != gdAlphaOpaque) {
                return 1;
            }
        }
    }
    return 0;
}

static int bmp_resolve_write_ctx(gdImagePtr im, int bpp_hint, int compression, int flags,
                                 bmp_write_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));

    ctx->bpp = (bpp_hint > 0) ? bpp_hint : bmp_auto_bpp(im);
    switch (ctx->bpp) {
    case 1:
    case 4:
    case 8:
    case 16:
    case 24:
    case 32:
        break;
    default:
        return 1;
    }

    if ((ctx->bpp == 1 || ctx->bpp == 4 || ctx->bpp == 8) && im->trueColor) {
        return 1;
    }
    if ((ctx->bpp == 1 || ctx->bpp == 4 || ctx->bpp == 8) && im->colorsTotal > (1 << ctx->bpp)) {
        return 1;
    }

    if (compression == -1) {
        if (ctx->bpp == 8) {
            ctx->compression = BMP_BI_RLE8;
        } else if (ctx->bpp == 4) {
            ctx->compression = BMP_BI_RLE4;
        } else {
            ctx->compression = BMP_BI_RGB;
        }
    } else {
        switch (compression) {
        case GD_BMP_COMPRESS_NONE:
            ctx->compression = BMP_BI_RGB;
            break;
        case GD_BMP_COMPRESS_RLE8:
            if (ctx->bpp != 8) {
                return 1;
            }
            ctx->compression = BMP_BI_RLE8;
            break;
        case GD_BMP_COMPRESS_RLE4:
            if (ctx->bpp != 4) {
                return 1;
            }
            ctx->compression = BMP_BI_RLE4;
            break;
        default:
            return 1;
        }
    }

    if (ctx->bpp == 16) {
        if (flags & GD_BMP_FLAG_RGB555) {
            ctx->red_mask = 0x7C00;
            ctx->green_mask = 0x03E0;
            ctx->blue_mask = 0x001F;
        } else {
            ctx->red_mask = 0xF800;
            ctx->green_mask = 0x07E0;
            ctx->blue_mask = 0x001F;
        }
        ctx->compression = BMP_BI_BITFIELDS;
    } else if (ctx->bpp == 32) {
        ctx->red_mask = 0x00FF0000;
        ctx->green_mask = 0x0000FF00;
        ctx->blue_mask = 0x000000FF;
        ctx->alpha_mask = 0xFF000000;
        ctx->compression = BMP_BI_BITFIELDS;
    }

    ctx->header_ver = ((ctx->bpp == 32 && ctx->alpha_mask) || (flags & GD_BMP_FLAG_FORCE_V4HDR))
                          ? BMP_WINDOWS_V4
                          : BMP_WINDOWS_V3;
    ctx->palette_size = (ctx->bpp <= 8) ? (1 << ctx->bpp) * 4 : 0;
    ctx->mask_size =
        (ctx->header_ver == BMP_WINDOWS_V3 && ctx->compression == BMP_BI_BITFIELDS) ? 12 : 0;
    ctx->info_size = ctx->header_ver + ctx->mask_size + ctx->palette_size;
    ctx->row_stride = (((ctx->bpp * im->sx) + 31) / 32) * 4;
    if (ctx->compression != BMP_BI_RLE8 && ctx->compression != BMP_BI_RLE4) {
        ctx->bitmap_size = ctx->row_stride * im->sy;
    }
    return 0;
}

static void bmp_write_file_header(gdIOCtxPtr out, bmp_write_ctx_t *ctx)
{
    gdPutBuf("BM", 2, out);
    gdBMPPutInt(out, 14 + ctx->info_size + ctx->bitmap_size);
    gdBMPPutWord(out, 0);
    gdBMPPutWord(out, 0);
    gdBMPPutInt(out, 14 + ctx->info_size);
}

static void bmp_write_info_header(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int i;

    gdBMPPutInt(out, ctx->header_ver);
    gdBMPPutInt(out, im->sx);
    gdBMPPutInt(out, im->sy);
    gdBMPPutWord(out, 1);
    gdBMPPutWord(out, ctx->bpp);
    gdBMPPutInt(out, ctx->compression);
    gdBMPPutInt(out, ctx->bitmap_size);
    gdBMPPutInt(out, 0);
    gdBMPPutInt(out, 0);
    gdBMPPutInt(out, ctx->palette_size ? (ctx->palette_size / 4) : 0);
    gdBMPPutInt(out, 0);

    if (ctx->header_ver == BMP_WINDOWS_V4) {
        gdBMPPutInt(out, ctx->red_mask);
        gdBMPPutInt(out, ctx->green_mask);
        gdBMPPutInt(out, ctx->blue_mask);
        gdBMPPutInt(out, ctx->alpha_mask);
        gdBMPPutInt(out, 0x73524742);
        for (i = 0; i < 9; i++) {
            gdBMPPutInt(out, 0);
        }
        gdBMPPutInt(out, 0);
        gdBMPPutInt(out, 0);
        gdBMPPutInt(out, 0);
    } else if (ctx->mask_size) {
        gdBMPPutInt(out, ctx->red_mask);
        gdBMPPutInt(out, ctx->green_mask);
        gdBMPPutInt(out, ctx->blue_mask);
    }
}

static void bmp_write_palette(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int i, count;

    count = 1 << ctx->bpp;
    for (i = 0; i < count; i++) {
        if (i < im->colorsTotal) {
            gdPutC(gdImageBlue(im, i), out);
            gdPutC(gdImageGreen(im, i), out);
            gdPutC(gdImageRed(im, i), out);
        } else {
            gdPutC(0, out);
            gdPutC(0, out);
            gdPutC(0, out);
        }
        gdPutC(0, out);
    }
}

static int bmp_write_pixels(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    switch (ctx->bpp) {
    case 1:
        return bmp_write_pixels_1bit(out, im, ctx);
    case 4:
        return bmp_write_pixels_4bit(out, im, ctx);
    case 8:
        return bmp_write_pixels_8bit(out, im, ctx);
    case 16:
        return bmp_write_pixels_16bit(out, im, ctx);
    case 24:
        return bmp_write_pixels_24bit(out, im, ctx);
    case 32:
        return bmp_write_pixels_32bit(out, im, ctx);
    default:
        return 1;
    }
}

static int bmp_write_padding(gdIOCtxPtr out, int count)
{
    while (count-- > 0) {
        gdPutC(0, out);
    }
    return 0;
}

static int bmp_write_pixels_1bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int row, xpos, bit, byte, bytes_written, index;

    for (row = im->sy - 1; row >= 0; row--) {
        bytes_written = 0;
        for (xpos = 0; xpos < im->sx; xpos += 8) {
            byte = 0;
            for (bit = 0; bit < 8 && xpos + bit < im->sx; bit++) {
                index = gdImageGetPixel(im, xpos + bit, row);
                if (index < 0 || index > 1) {
                    return 1;
                }
                byte |= (index & 0x01) << (7 - bit);
            }
            gdPutC(byte, out);
            bytes_written++;
        }
        bmp_write_padding(out, ctx->row_stride - bytes_written);
    }
    return 0;
}

static int bmp_write_pixels_4bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int row, xpos, high, low, bytes_written;

    if (ctx->compression == BMP_BI_RLE4) {
        for (row = im->sy - 1; row >= 0; row--) {
            int compressed_size = bmp_write_rle4_row(out, im, row);
            if (compressed_size < 0) {
                return 1;
            }
            ctx->bitmap_size += compressed_size;
            ctx->bitmap_size += 2;
            gdPutC(BMP_RLE_COMMAND, out);
            gdPutC(BMP_RLE_ENDOFLINE, out);
        }
        return 0;
    }

    for (row = im->sy - 1; row >= 0; row--) {
        bytes_written = 0;
        for (xpos = 0; xpos < im->sx; xpos += 2) {
            high = gdImageGetPixel(im, xpos, row);
            if (high < 0 || high > 15) {
                return 1;
            }
            if (xpos + 1 < im->sx) {
                low = gdImageGetPixel(im, xpos + 1, row);
                if (low < 0 || low > 15) {
                    return 1;
                }
            } else {
                low = 0;
            }
            gdPutC(((high & 0x0f) << 4) | (low & 0x0f), out);
            bytes_written++;
        }
        bmp_write_padding(out, ctx->row_stride - bytes_written);
    }
    return 0;
}

static int bmp_write_rle4_row(gdIOCtxPtr out, gdImagePtr im, int row)
{
    int xpos, chunk, i, value, next;
    int bytes;
    int total = 0;

    for (xpos = 0; xpos < im->sx;) {
        chunk = im->sx - xpos;
        if (chunk > 255) {
            chunk = 255;
        }
        if (chunk < 3) {
            value = gdImageGetPixel(im, xpos, row);
            if (value < 0 || value > 15) {
                return 1;
            }
            next = value;
            if (chunk == 2) {
                next = gdImageGetPixel(im, xpos + 1, row);
                if (next < 0 || next > 15) {
                    return 1;
                }
            }
            gdPutC(chunk, out);
            gdPutC(((value & 0x0f) << 4) | (next & 0x0f), out);
            total += 2;
            xpos += chunk;
            continue;
        }
        gdPutC(BMP_RLE_COMMAND, out);
        gdPutC(chunk, out);
        total += 2;
        bytes = (chunk + 1) / 2;
        for (i = 0; i < chunk; i += 2) {
            value = gdImageGetPixel(im, xpos + i, row);
            if (value < 0 || value > 15) {
                return 1;
            }
            if (i + 1 < chunk) {
                next = gdImageGetPixel(im, xpos + i + 1, row);
                if (next < 0 || next > 15) {
                    return 1;
                }
            } else {
                next = 0;
            }
            gdPutC(((value & 0x0f) << 4) | (next & 0x0f), out);
            total++;
        }
        if (bytes & 1) {
            gdPutC(0, out);
            total++;
        }
        xpos += chunk;
    }
    return total;
}

static int bmp_write_pixels_8bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int row, xpos;
    unsigned char *uncompressed_row = NULL;

    if (ctx->compression == BMP_BI_RLE8) {
        uncompressed_row = (unsigned char *)gdCalloc(gdImageSX(im) * 2, sizeof(char));
        if (!uncompressed_row) {
            return 1;
        }
        for (row = im->sy - 1; row >= 0; row--) {
            int compressed_size;
            for (xpos = 0; xpos < im->sx; xpos++) {
                uncompressed_row[xpos] = (unsigned char)gdImageGetPixel(im, xpos, row);
            }
            compressed_size = compress_row(uncompressed_row, gdImageSX(im));
            if (compressed_size < 0) {
                gdFree(uncompressed_row);
                return 1;
            }
            ctx->bitmap_size += compressed_size;
            if (gdPutBuf(uncompressed_row, compressed_size, out) != compressed_size) {
                gd_error("gd-bmp write error\n");
                gdFree(uncompressed_row);
                return 1;
            }
            gdPutC(BMP_RLE_COMMAND, out);
            gdPutC(BMP_RLE_ENDOFLINE, out);
            ctx->bitmap_size += 2;
        }
        gdFree(uncompressed_row);
        return 0;
    }

    for (row = im->sy - 1; row >= 0; row--) {
        for (xpos = 0; xpos < im->sx; xpos++) {
            gdPutC(gdImageGetPixel(im, xpos, row), out);
        }
        bmp_write_padding(out, ctx->row_stride - im->sx);
    }
    return 0;
}

static int bmp_write_pixels_16bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int row, xpos, pixel, red, green, blue;
    unsigned int packed;

    for (row = im->sy - 1; row >= 0; row--) {
        for (xpos = 0; xpos < im->sx; xpos++) {
            pixel = gdImageGetPixel(im, xpos, row);
            if (im->trueColor) {
                red = gdTrueColorGetRed(pixel);
                green = gdTrueColorGetGreen(pixel);
                blue = gdTrueColorGetBlue(pixel);
            } else {
                red = gdImageRed(im, pixel);
                green = gdImageGreen(im, pixel);
                blue = gdImageBlue(im, pixel);
            }
            packed = bmp_pack_mask(red, ctx->red_mask) | bmp_pack_mask(green, ctx->green_mask) |
                     bmp_pack_mask(blue, ctx->blue_mask);
            gdBMPPutWord(out, (int)packed);
        }
        bmp_write_padding(out, ctx->row_stride - im->sx * 2);
    }
    return 0;
}

static int bmp_write_pixels_24bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int row, xpos, pixel;

    for (row = im->sy - 1; row >= 0; row--) {
        for (xpos = 0; xpos < im->sx; xpos++) {
            pixel = gdImageGetPixel(im, xpos, row);
            if (im->trueColor) {
                gdPutC(gdTrueColorGetBlue(pixel), out);
                gdPutC(gdTrueColorGetGreen(pixel), out);
                gdPutC(gdTrueColorGetRed(pixel), out);
            } else {
                gdPutC(gdImageBlue(im, pixel), out);
                gdPutC(gdImageGreen(im, pixel), out);
                gdPutC(gdImageRed(im, pixel), out);
            }
        }
        bmp_write_padding(out, ctx->row_stride - im->sx * 3);
    }
    return 0;
}

static int bmp_write_pixels_32bit(gdIOCtxPtr out, gdImagePtr im, bmp_write_ctx_t *ctx)
{
    int row, xpos, pixel, red, green, blue, alpha;
    unsigned int packed;

    for (row = im->sy - 1; row >= 0; row--) {
        for (xpos = 0; xpos < im->sx; xpos++) {
            pixel = gdImageGetPixel(im, xpos, row);
            if (im->trueColor) {
                red = gdTrueColorGetRed(pixel);
                green = gdTrueColorGetGreen(pixel);
                blue = gdTrueColorGetBlue(pixel);
                alpha = bmp_gd_to_alpha(gdTrueColorGetAlpha(pixel));
            } else {
                red = gdImageRed(im, pixel);
                green = gdImageGreen(im, pixel);
                blue = gdImageBlue(im, pixel);
                alpha = bmp_gd_to_alpha(im->alpha[pixel]);
            }
            packed = bmp_pack_mask(red, ctx->red_mask) | bmp_pack_mask(green, ctx->green_mask) |
                     bmp_pack_mask(blue, ctx->blue_mask) | bmp_pack_mask(alpha, ctx->alpha_mask);
            gdBMPPutInt(out, (int)packed);
        }
    }
    return 0;
}

static int bmp_write_tmpfile_to_ctx(gdIOCtxPtr out, gdIOCtxPtr out_original)
{
    unsigned char *copy_buffer = NULL;
    int buffer_size = 0;

    gdSeek(out, 0);
    copy_buffer = (unsigned char *)gdMalloc(1024 * sizeof(unsigned char));
    if (copy_buffer == NULL) {
        return 1;
    }

    while ((buffer_size = gdGetBuf(copy_buffer, 1024, out)) != EOF) {
        if (buffer_size == 0) {
            break;
        }
        if (gdPutBuf(copy_buffer, buffer_size, out_original) != buffer_size) {
            gd_error("gd-bmp write error\n");
            gdFree(copy_buffer);
            return 1;
        }
    }
    gdFree(copy_buffer);
    return 0;
}

static int compress_row(unsigned char *row, int length)
{
    int rle_type = 0;
    int compressed_length = 0;
    int pixel = 0, compressed_run = 0, rle_compression = 0;
    unsigned char *uncompressed_row = NULL, *uncompressed_rowp = NULL, *uncompressed_start = NULL;
    GD_ASSUME(length > 0);
    uncompressed_row = (unsigned char *)gdMalloc(length);
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
                /* more than what we can store in a single run or run is over
                 * due to non match, force write */
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
                /* more than what we can store in a single run or run is over
                 * due to match, force write */
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
        compressed_length += build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
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

    /* Bitmap specific cases is that we can't have uncompressed rows of length 1
     * or 2 */
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
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmp(FILE *inFile)
{
    gdImagePtr im = 0;
    gdIOCtx *in = gdNewFileCtx(inFile);
    if (in == NULL)
        return NULL;
    im = gdImageCreateFromBmpCtx(in);
    in->gd_free(in);
    return im;
}

/*
        Function: gdImageCreateFromBmpPtr
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpPtr(int size, void *data)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
    if (in == NULL)
        return NULL;
    im = gdImageCreateFromBmpCtx(in);
    in->gd_free(in);
    return im;
}

/*
        Function: gdImageCreateFromBmpCtx
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpCtx(gdIOCtxPtr infile)
{
    bmp_hdr_t *hdr;
    bmp_info_t *info;
    gdImagePtr im = NULL;
    int error = 0;

    if (!(hdr = (bmp_hdr_t *)gdCalloc(1, sizeof(bmp_hdr_t)))) {
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

    if (bmp_validate_info(info, hdr)) {
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
    case 2:
        BMP_DEBUG(printf("2-bit image\n"));
        error = bmp_read_2bit(im, infile, info, hdr);
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
    if (!gdGetWordLSB(&hdr->magic, infile) || !gdGetIntLSB(&hdr->size, infile) ||
        !gdGetWordLSB(&hdr->reserved1, infile) || !gdGetWordLSB(&hdr->reserved2, infile) ||
        !gdGetIntLSB(&hdr->off, infile)) {
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
    case BMP_WINDOWS_V2:
    case BMP_WINDOWS_V3_ALPHA:
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
    case BMP_OS2_V2_SHORT:
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

static int bmp_skip_bytes(gdIOCtxPtr infile, int count)
{
    int i, c;

    for (i = 0; i < count; i++) {
        if (!gdGetByte(&c, infile)) {
            return 1;
        }
    }
    return 0;
}

static int bmp_read_bitfield_masks(gdIOCtxPtr infile, bmp_info_t *info, int read_alpha)
{
    int red, green, blue, alpha;

    if (!gdGetIntLSB(&red, infile) || !gdGetIntLSB(&green, infile) || !gdGetIntLSB(&blue, infile)) {
        return 1;
    }

    info->red_mask = (unsigned int)red;
    info->green_mask = (unsigned int)green;
    info->blue_mask = (unsigned int)blue;

    if (read_alpha) {
        if (!gdGetIntLSB(&alpha, infile)) {
            return 1;
        }
        info->alpha_mask = (unsigned int)alpha;
    }

    return 0;
}

static int bmp_validate_info(bmp_info_t *info, bmp_hdr_t *hdr)
{
    int image_size;
    int min_size;

    if (hdr->off < 14 + info->len || hdr->size < 0 || hdr->off < 0) {
        return 1;
    }

    if (info->numplanes != 1) {
        return 1;
    }

    if (info->depth != 1 && info->depth != 2 && info->depth != 4 && info->depth != 8 &&
        info->depth != 16 && info->depth != 24 && info->depth != 32) {
        return 1;
    }

    if (info->topdown && (info->enctype == BMP_BI_RLE4 || info->enctype == BMP_BI_RLE8)) {
        return 1;
    }

    if (overflow2(info->width, info->height)) {
        return 1;
    }

    if (info->hres < 0 || info->vres < 0) {
        return 1;
    }
    if (info->hres > 0 && info->vres > 0 &&
        (info->hres / info->vres > 1000 || info->vres / info->hres > 1000)) {
        return 1;
    }

    if (info->enctype == BMP_BI_RGB || info->enctype == BMP_BI_BITFIELDS ||
        info->enctype == BMP_BI_ALPHABITFIELDS) {
        if (bmp_image_size(info->width, info->height, info->depth, &image_size)) {
            return 1;
        }
        if (info->size != 0 && info->size != image_size) {
            return 1;
        }
        if (hdr->size != 0) {
            min_size = hdr->off + image_size;
            if (min_size < hdr->off) {
                return 1;
            }
            if (hdr->size < min_size && hdr->size != 14 + info->len) {
                return 1;
            }
            if (hdr->size > min_size && hdr->size - min_size > 1024 * 1024) {
                return 1;
            }
        }
    }

    return 0;
}

static int bmp_read_windows_v3_info(gdIOCtxPtr infile, bmp_info_t *info)
{
    int extra = info->len - BMP_WINDOWS_V3;

    if (!gdGetIntLSB(&info->width, infile) || !gdGetIntLSB(&info->height, infile) ||
        !gdGetWordLSB(&info->numplanes, infile) || !gdGetWordLSB(&info->depth, infile) ||
        !gdGetIntLSB(&info->enctype, infile) || !gdGetIntLSB(&info->size, infile) ||
        !gdGetIntLSB(&info->hres, infile) || !gdGetIntLSB(&info->vres, infile) ||
        !gdGetIntLSB(&info->numcolors, infile) || !gdGetIntLSB(&info->mincolors, infile)) {
        return 1;
    }

    /* Unlikely, but possible -- largest signed value won't fit in unsigned. */
    if (info->height == 0 || info->height == INT_MIN)
        return 1;
    if (info->height < 0) {
        info->topdown = 1;
        info->height = -info->height;
    } else {
        info->topdown = 0;
    }

    info->type = BMP_PALETTE_4;

    if (extra > 0) {
        if (extra < 12 || bmp_read_bitfield_masks(infile, info, extra >= 16)) {
            return 1;
        }
        extra -= (extra >= 16) ? 16 : 12;
        if (bmp_skip_bytes(infile, extra)) {
            return 1;
        }
    }

    /* Height was checked above. */
    if (info->width <= 0 || info->numplanes <= 0 || info->depth <= 0 || info->numcolors < 0 ||
        info->mincolors < 0) {
        return 1;
    }

    return 0;
}

static int bmp_read_os2_v1_info(gdIOCtxPtr infile, bmp_info_t *info)
{
    if (!gdGetWordLSB((signed short int *)&info->width, infile) ||
        !gdGetWordLSB((signed short int *)&info->height, infile) ||
        !gdGetWordLSB(&info->numplanes, infile) || !gdGetWordLSB(&info->depth, infile)) {
        return 1;
    }

    /* OS2 v1 doesn't support topdown */
    info->topdown = 0;

    /* The spec says the depth can only be a few value values. */
    if (info->depth != 1 && info->depth != 4 && info->depth != 8 && info->depth != 16 &&
        info->depth != 24) {
        return 1;
    }

    info->numcolors = (info->depth <= 8) ? (1 << info->depth) : 0;
    info->type = BMP_PALETTE_3;

    if (info->width <= 0 || info->height <= 0 || info->numplanes <= 0) {
        return 1;
    }

    return 0;
}

static int bmp_read_os2_v2_info(gdIOCtxPtr infile, bmp_info_t *info)
{
    char useless_bytes[24];
    if (!gdGetIntLSB(&info->width, infile) || !gdGetIntLSB(&info->height, infile) ||
        !gdGetWordLSB(&info->numplanes, infile) || !gdGetWordLSB(&info->depth, infile)) {
        return 1;
    }

    if (info->len == BMP_OS2_V2_SHORT) {
        info->enctype = BMP_BI_RGB;
        info->size = 0;
        info->hres = 0;
        info->vres = 0;
        info->numcolors = (info->depth <= 8) ? (1 << info->depth) : 0;
        info->mincolors = 0;
        goto done;
    }

    if (!gdGetIntLSB(&info->enctype, infile) || !gdGetIntLSB(&info->size, infile) ||
        !gdGetIntLSB(&info->hres, infile) || !gdGetIntLSB(&info->vres, infile) ||
        !gdGetIntLSB(&info->numcolors, infile) || !gdGetIntLSB(&info->mincolors, infile)) {
        return 1;
    }

    /* Lets seek the next 24 pointless bytes, we don't care too much about it */
    if (!gdGetBuf(useless_bytes, 24, infile)) {
        return 1;
    }

done:
    /* Unlikely, but possible -- largest signed value won't fit in unsigned. */
    if (info->height == 0 || info->height == INT_MIN)
        return 1;
    if (info->height < 0) {
        info->topdown = 1;
        info->height = -info->height;
    } else {
        info->topdown = 0;
    }

    info->type = BMP_PALETTE_4;

    /* Height was checked above. */
    if (info->width <= 0 || info->numplanes <= 0 || info->depth <= 0 || info->numcolors < 0 ||
        info->mincolors < 0) {
        return 1;
    }

    return 0;
}

static int bmp_read_direct(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
    int ypos = 0, xpos = 0, row = 0;
    int padding = 0, red = 0, green = 0, blue = 0;
    int alpha = gdAlphaOpaque;
    signed short int data16 = 0;
    int data32 = 0;
    unsigned int mask = 0;

    if (info->depth == 16 && info->enctype == BMP_BI_RGB) {
        info->red_mask = 0x7C00;
        info->green_mask = 0x03E0;
        info->blue_mask = 0x001F;
    } else if (info->depth == 32 && info->enctype == BMP_BI_RGB) {
        info->red_mask = 0x00FF0000;
        info->green_mask = 0x0000FF00;
        info->blue_mask = 0x000000FF;
    }

    switch (info->enctype) {
    case BMP_BI_RGB:
        /* no-op */
        break;

    case BMP_BI_BITFIELDS:
        if (info->depth == 24) {
            BMP_DEBUG(printf("Bitfield compression isn't supported for 24-bit\n"));
            return 1;
        }
        if (info->len == BMP_WINDOWS_V3 && bmp_read_bitfield_masks(infile, info, 0)) {
            return 1;
        }
        break;
    case BMP_BI_ALPHABITFIELDS:
        if (info->depth != 16 && info->depth != 32) {
            return 1;
        }
        if (info->len == BMP_WINDOWS_V3 && bmp_read_bitfield_masks(infile, info, 1)) {
            return 1;
        }
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

    if (info->depth == 16 || info->depth == 32) {
        mask = info->red_mask | info->green_mask | info->blue_mask;
        if (info->red_mask == 0 || info->green_mask == 0 || info->blue_mask == 0) {
            return 1;
        }
        if ((info->red_mask & info->green_mask) || (info->red_mask & info->blue_mask) ||
            (info->green_mask & info->blue_mask)) {
            return 1;
        }
        if (info->depth == 16 && (mask & 0xFFFF0000U)) {
            return 1;
        }
    }

    /* There is a chance the data isn't until later, would be weird but it is
     * possible */
    if (gdTell(infile) != header->off) {
        /* Should make sure we don't seek past the file size */
        if (!gdSeek(infile, header->off)) {
            return 1;
        }
    }

    /* The line must be divisible by 4, else its padded with NULLs */
    if (bmp_row_padding(info->width, info->depth, &padding)) {
        return 1;
    }

    for (ypos = 0; ypos < info->height; ++ypos) {
        if (info->topdown) {
            row = ypos;
        } else {
            row = info->height - ypos - 1;
        }

        for (xpos = 0; xpos < info->width; xpos++) {
            if (info->depth == 16) {
                if (!gdGetWordLSB(&data16, infile)) {
                    return 1;
                }
                BMP_DEBUG(printf("Data: %X\n", data16));
                red = bmp_extract_mask((unsigned short)data16, info->red_mask);
                green = bmp_extract_mask((unsigned short)data16, info->green_mask);
                blue = bmp_extract_mask((unsigned short)data16, info->blue_mask);
                alpha = info->alpha_mask ? bmp_alpha_to_gd(bmp_extract_mask((unsigned short)data16,
                                                                            info->alpha_mask))
                                         : gdAlphaOpaque;
                BMP_DEBUG(printf("R: %d, G: %d, B: %d\n", red, green, blue));
            } else if (info->depth == 24) {
                if (!gdGetByte(&blue, infile) || !gdGetByte(&green, infile) ||
                    !gdGetByte(&red, infile)) {
                    return 1;
                }
                alpha = gdAlphaOpaque;
            } else {
                if (!gdGetIntLSB(&data32, infile)) {
                    return 1;
                }
                red = bmp_extract_mask((unsigned int)data32, info->red_mask);
                green = bmp_extract_mask((unsigned int)data32, info->green_mask);
                blue = bmp_extract_mask((unsigned int)data32, info->blue_mask);
                alpha =
                    info->alpha_mask
                        ? bmp_alpha_to_gd(bmp_extract_mask((unsigned int)data32, info->alpha_mask))
                        : gdAlphaOpaque;
            }
            gdImageSetPixel(im, xpos, row, gdTrueColorAlpha(red, green, blue, alpha));
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
        if (!gdGetByte(&b, infile) || !gdGetByte(&g, infile) || !gdGetByte(&r, infile) ||
            (read_four && !gdGetByte(&z, infile))) {
            return 1;
        }
        im->red[i] = r;
        im->green[i] = g;
        im->blue[i] = b;
        im->open[i] = 1;
    }
    return 0;
}

static int bmp_check_palette_index(gdImagePtr im, int index)
{
    return index >= 0 && index < im->colorsTotal;
}

static int bmp_row_padding(int width, int depth, int *padding)
{
    int bits_per_row;
    int bytes_per_row;

    if (overflow2(width, depth)) {
        return 1;
    }
    bits_per_row = width * depth;
    if (bits_per_row > INT_MAX - 31) {
        return 1;
    }
    bytes_per_row = ((bits_per_row + 31) / 32) * 4;
    *padding = bytes_per_row - ((bits_per_row + 7) / 8);
    return 0;
}

static int bmp_image_size(int width, int height, int depth, int *size)
{
    int bits_per_row;
    int bytes_per_row;

    if (overflow2(width, depth)) {
        return 1;
    }
    bits_per_row = width * depth;
    if (bits_per_row > INT_MAX - 31) {
        return 1;
    }
    bytes_per_row = ((bits_per_row + 31) / 32) * 4;
    if (overflow2(bytes_per_row, height)) {
        return 1;
    }
    *size = bytes_per_row * height;
    return 0;
}

static int bmp_get_mask_shift(unsigned int mask)
{
    int shift = 0;

    if (mask == 0) {
        return 0;
    }
    while ((mask & 1U) == 0) {
        mask >>= 1;
        shift++;
    }
    return shift;
}

static int bmp_get_mask_bits(unsigned int mask)
{
    int bits = 0;

    if (mask == 0) {
        return 0;
    }
    mask >>= bmp_get_mask_shift(mask);
    while (mask & 1U) {
        bits++;
        mask >>= 1;
    }
    return bits;
}

static int bmp_extract_mask(unsigned int pixel, unsigned int mask)
{
    unsigned int value;
    int bits;

    if (mask == 0) {
        return 0;
    }
    value = (pixel & mask) >> bmp_get_mask_shift(mask);
    bits = bmp_get_mask_bits(mask);
    if (bits <= 0) {
        return 0;
    }
    if (bits >= 32) {
        return (int)(value >> 24);
    }
    if (bits >= 8) {
        return (int)((value * 255U) / ((1U << bits) - 1U));
    }
    return (int)((value * 255U + ((1U << bits) - 1U) / 2U) / ((1U << bits) - 1U));
}

static unsigned int bmp_pack_mask(int channel_8bit, unsigned int mask)
{
    int bits;
    int shift;
    unsigned int max_val;
    unsigned int value;

    if (mask == 0) {
        return 0;
    }
    bits = bmp_get_mask_bits(mask);
    shift = bmp_get_mask_shift(mask);
    if (bits <= 0) {
        return 0;
    }
    if (bits >= 32) {
        value = (unsigned int)channel_8bit << 24;
    } else {
        max_val = (1U << bits) - 1U;
        value = ((unsigned int)channel_8bit * max_val + 127U) / 255U;
    }
    return (value << shift) & mask;
}

static int bmp_alpha_to_gd(int alpha)
{
    if (alpha <= 0) {
        return gdAlphaTransparent;
    }
    if (alpha >= 255) {
        return gdAlphaOpaque;
    }
    return gdAlphaMax - (alpha * gdAlphaMax + 127) / 255;
}

static int bmp_gd_to_alpha(int gd_alpha)
{
    if (gd_alpha <= gdAlphaOpaque) {
        return 255;
    }
    if (gd_alpha >= gdAlphaTransparent) {
        return 0;
    }
    return (gdAlphaMax - gd_alpha) * 255 / gdAlphaMax;
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

    /* There is a chance the data isn't until later, would be weird but it is
     * possible */
    if (gdTell(infile) != header->off) {
        /* Should make sure we don't seek past the file size */
        if (!gdSeek(infile, header->off)) {
            return 1;
        }
    }

    if (bmp_row_padding(info->width, info->depth, &padding)) {
        return 1;
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
                /* No need to read anything extra */
                if ((xpos + bit) >= info->width) {
                    break;
                }
                if (!bmp_check_palette_index(im, index)) {
                    return 1;
                }
                if (im->open[index]) {
                    im->open[index] = 0;
                }
                gdImageSetPixel(im, xpos + bit, row, index);
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

static int bmp_read_2bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
    int ypos = 0, xpos = 0, row = 0, index = 0;
    int padding = 0, current_byte = 0, shift = 0;

    if (info->enctype != BMP_BI_RGB) {
        return 1;
    }

    if (!info->numcolors) {
        info->numcolors = 4;
    } else if (info->numcolors < 0 || info->numcolors > 4) {
        return 1;
    }

    if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
        return 1;
    }

    im->colorsTotal = info->numcolors;

    if (gdTell(infile) != header->off) {
        if (!gdSeek(infile, header->off)) {
            return 1;
        }
    }

    if (bmp_row_padding(info->width, info->depth, &padding)) {
        return 1;
    }

    for (ypos = 0; ypos < info->height; ++ypos) {
        if (info->topdown) {
            row = ypos;
        } else {
            row = info->height - ypos - 1;
        }

        for (xpos = 0; xpos < info->width; xpos += 4) {
            if (!gdGetByte(&current_byte, infile)) {
                return 1;
            }

            for (shift = 6; shift >= 0; shift -= 2) {
                if (xpos + ((6 - shift) / 2) >= info->width) {
                    break;
                }
                index = (current_byte >> shift) & 0x03;
                if (!bmp_check_palette_index(im, index)) {
                    return 1;
                }
                if (im->open[index]) {
                    im->open[index] = 0;
                }
                gdImageSetPixel(im, xpos + ((6 - shift) / 2), row, index);
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

    /* There is a chance the data isn't until later, would be weird but it is
     * possible */
    if (gdTell(infile) != header->off) {
        /* Should make sure we don't seek past the file size */
        if (!gdSeek(infile, header->off)) {
            return 1;
        }
    }

    if (bmp_row_padding(info->width, info->depth, &padding)) {
        return 1;
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
                if (!bmp_check_palette_index(im, index)) {
                    return 1;
                }
                if (im->open[index]) {
                    im->open[index] = 0;
                }
                gdImageSetPixel(im, xpos, row, index);

                /* This condition may get called often, potential optimsations
                 */
                if (xpos + 1 >= info->width) {
                    break;
                }

                index = current_byte & 0x0f;
                if (!bmp_check_palette_index(im, index)) {
                    return 1;
                }
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
    int palette_entries = 0;
    int palette_entry_size = (info->type == BMP_PALETTE_4) ? 4 : 3;

    if (info->enctype != BMP_BI_RGB && info->enctype != BMP_BI_RLE8) {
        return 1;
    }

    if (!info->numcolors) {
        info->numcolors = 256;
    } else if (info->numcolors < 0 || info->numcolors > 1024) {
        return 1;
    }

    palette_entries = (header->off - (14 + info->len)) / palette_entry_size;
    if (palette_entries <= 0) {
        return 1;
    }
    if (info->numcolors > palette_entries) {
        info->numcolors = palette_entries;
    }
    if (info->numcolors > gdMaxColors) {
        info->numcolors = gdMaxColors;
    }

    if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
        return 1;
    }

    im->colorsTotal = info->numcolors;

    /* There is a chance the data isn't until later, would be weird but it is
     * possible */
    if (gdTell(infile) != header->off) {
        /* Should make sure we don't seek past the file size */
        if (!gdSeek(infile, header->off)) {
            return 1;
        }
    }

    if (bmp_row_padding(info->width, info->depth, &padding)) {
        return 1;
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

                if (!bmp_check_palette_index(im, index)) {
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

    if (info->topdown) {
        return 1;
    }

    for (ypos = 0; ypos < info->height && xpos <= info->width;) {
        if (!gdGetByte(&rle_length, infile) || !gdGetByte(&rle_data, infile)) {
            return 1;
        }
        row = info->height - ypos - 1;
        if (row < 0 || row >= info->height) {
            return 1;
        }

        if (rle_length != BMP_RLE_COMMAND) {
            for (i = 0; i < rle_length;) {
                for (j = 1; (j <= pixels_per_byte) && (i < rle_length); j++, xpos++, i++) {
                    index = (rle_data & (((1 << info->depth) - 1) << (8 - (j * info->depth)))) >>
                            (8 - (j * info->depth));
                    if (xpos >= info->width) {
                        return 1;
                    }
                    if (!bmp_check_palette_index(im, index)) {
                        return 1;
                    }
                    if (im->open[index]) {
                        im->open[index] = 0;
                    }
                    gdImageSetPixel(im, xpos, row, index);
                }
            }
        } else if (rle_length == BMP_RLE_COMMAND && rle_data > 2) {
            /* Uncompressed RLE needs to be even */
            padding = 0;
            for (i = 0; i < rle_data; i += pixels_per_byte) {
                int max_pixels = pixels_per_byte;

                if (!gdGetByte(&index, infile)) {
                    return 1;
                }
                padding++;

                if (rle_data - i < max_pixels) {
                    max_pixels = rle_data - i;
                }

                for (j = 1; j <= max_pixels; j++) {
                    int temp = (index >> (8 - (j * info->depth))) & ((1 << info->depth) - 1);
                    if (xpos >= info->width) {
                        return 1;
                    }
                    if (!bmp_check_palette_index(im, temp)) {
                        return 1;
                    }
                    if (im->open[temp]) {
                        im->open[temp] = 0;
                    }
                    gdImageSetPixel(im, xpos, row, temp);
                    xpos++;
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
            if (xpos + rle_length > info->width || ypos + rle_data >= info->height) {
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
