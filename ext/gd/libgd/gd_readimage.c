#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_errors.h"
#include <stdio.h>
#include <string.h>

#define GD_READIMAGE_PROBE_SIZE 32

typedef gdImagePtr(BGD_STDCALL *ReadCtxFn)(gdIOCtxPtr);

struct FormatEntry {
	const char *name;
	size_t offset;
	const unsigned char *sig;
	size_t sig_len;
	ReadCtxFn reader;
	int enabled;
};

#ifdef HAVE_LIBPNG
#define GD_READIMAGE_PNG_READER gdImageCreateFromPngCtx
#define GD_READIMAGE_PNG_ENABLED 1
#else
#define GD_READIMAGE_PNG_READER NULL
#define GD_READIMAGE_PNG_ENABLED 0
#endif

#ifdef HAVE_LIBJXL
#define GD_READIMAGE_JXL_READER gdImageCreateFromJxlCtx
#define GD_READIMAGE_JXL_ENABLED 1
#else
#define GD_READIMAGE_JXL_READER NULL
#define GD_READIMAGE_JXL_ENABLED 0
#endif

#ifdef HAVE_LIBJPEG
#define GD_READIMAGE_JPEG_READER gdImageCreateFromJpegCtx
#define GD_READIMAGE_JPEG_ENABLED 1
#else
#define GD_READIMAGE_JPEG_READER NULL
#define GD_READIMAGE_JPEG_ENABLED 0
#endif

#ifdef HAVE_LIBTIFF
#define GD_READIMAGE_TIFF_READER gdImageCreateFromTiffCtx
#define GD_READIMAGE_TIFF_ENABLED 1
#else
#define GD_READIMAGE_TIFF_READER NULL
#define GD_READIMAGE_TIFF_ENABLED 0
#endif

#ifdef HAVE_LIBWEBP
#define GD_READIMAGE_WEBP_READER gdImageCreateFromWebpCtx
#define GD_READIMAGE_WEBP_ENABLED 1
#else
#define GD_READIMAGE_WEBP_READER NULL
#define GD_READIMAGE_WEBP_ENABLED 0
#endif

#ifdef HAVE_LIBAVIF
#define GD_READIMAGE_AVIF_READER gdImageCreateFromAvifCtx
#define GD_READIMAGE_AVIF_ENABLED 1
#else
#define GD_READIMAGE_AVIF_READER NULL
#define GD_READIMAGE_AVIF_ENABLED 0
#endif

#ifdef HAVE_LIBHEIF
#define GD_READIMAGE_HEIF_READER gdImageCreateFromHeifCtx
#define GD_READIMAGE_HEIF_ENABLED 1
#else
#define GD_READIMAGE_HEIF_READER NULL
#define GD_READIMAGE_HEIF_ENABLED 0
#endif

#if ENABLE_GD_FORMATS
#define GD_READIMAGE_GD_READER gdImageCreateFromGdCtx
#define GD_READIMAGE_GD_ENABLED 1
#else
#define GD_READIMAGE_GD_READER NULL
#define GD_READIMAGE_GD_ENABLED 0
#endif

#if ENABLE_GD_FORMATS && defined(HAVE_LIBZ)
#define GD_READIMAGE_GD2_READER gdImageCreateFromGd2Ctx
#define GD_READIMAGE_GD2_ENABLED 1
#else
#define GD_READIMAGE_GD2_READER NULL
#define GD_READIMAGE_GD2_ENABLED 0
#endif

#ifdef HAVE_LIBXPM
#define GD_READIMAGE_XPM_ENABLED 1
#else
#define GD_READIMAGE_XPM_ENABLED 0
#endif

#define ENTRY(n, off, reader_fn, is_enabled, ...) \
	{ (n), (off),                                                          \
	  (const unsigned char[]){__VA_ARGS__},                                \
	  sizeof((const unsigned char[]){__VA_ARGS__}) / sizeof(unsigned char), \
	  (reader_fn), (is_enabled) }

static const struct FormatEntry format_table[] = {
	ENTRY("PNG", 0, GD_READIMAGE_PNG_READER, GD_READIMAGE_PNG_ENABLED,
		  0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A),
	ENTRY("JXL", 0, GD_READIMAGE_JXL_READER, GD_READIMAGE_JXL_ENABLED,
		  0x00, 0x00, 0x00, 0x0C, 0x4A, 0x58, 0x4C, 0x20,
		  0x0D, 0x0A, 0x87, 0x0A),
	ENTRY("JXL", 0, GD_READIMAGE_JXL_READER, GD_READIMAGE_JXL_ENABLED,
		  0xFF, 0x0A),
	ENTRY("JPEG", 0, GD_READIMAGE_JPEG_READER, GD_READIMAGE_JPEG_ENABLED,
		  0xFF, 0xD8, 0xFF),
	ENTRY("GIF", 0, gdImageCreateFromGifCtx, 1,
		  0x47, 0x49, 0x46, 0x38, 0x39, 0x61),
	ENTRY("GIF", 0, gdImageCreateFromGifCtx, 1,
		  0x47, 0x49, 0x46, 0x38, 0x37, 0x61),
	ENTRY("BMP", 0, gdImageCreateFromBmpCtx, 1,
		  0x42, 0x4D),
	ENTRY("TIFF", 0, GD_READIMAGE_TIFF_READER, GD_READIMAGE_TIFF_ENABLED,
		  0x4D, 0x4D, 0x00, 0x2A),
	ENTRY("TIFF", 0, GD_READIMAGE_TIFF_READER, GD_READIMAGE_TIFF_ENABLED,
		  0x49, 0x49, 0x2A, 0x00),
	ENTRY("WEBP", 8, GD_READIMAGE_WEBP_READER, GD_READIMAGE_WEBP_ENABLED,
		  0x57, 0x45, 0x42, 0x50),
	ENTRY("AVIF", 4, GD_READIMAGE_AVIF_READER, GD_READIMAGE_AVIF_ENABLED,
		  0x66, 0x74, 0x79, 0x70, 0x61, 0x76, 0x69, 0x66),
	ENTRY("HEIC", 4, GD_READIMAGE_HEIF_READER, GD_READIMAGE_HEIF_ENABLED,
		  0x66, 0x74, 0x79, 0x70, 0x68, 0x65, 0x69, 0x63),
	ENTRY("GD2", 0, GD_READIMAGE_GD2_READER, GD_READIMAGE_GD2_ENABLED,
		  0x67, 0x64, 0x32, 0x00),
	ENTRY("GD", 0, GD_READIMAGE_GD_READER, GD_READIMAGE_GD_ENABLED,
		  0xFF, 0xFE),
	ENTRY("GD", 0, GD_READIMAGE_GD_READER, GD_READIMAGE_GD_ENABLED,
		  0xFF, 0xFF),
	ENTRY("QOI", 0, gdImageCreateFromQoiCtx, 1,
		  0x71, 0x6F, 0x69, 0x66),
	ENTRY("XPM", 0, NULL, GD_READIMAGE_XPM_ENABLED,
		  0x2F, 0x2A, 0x20, 0x58, 0x50, 0x4D, 0x20, 0x2A),
	ENTRY("XBM", 0, NULL, 1,
		  0x23, 0x64, 0x65, 0x66, 0x69, 0x6E, 0x65, 0x20),
	{NULL, 0, NULL, 0, NULL, 0}
};

static int sig_match(const unsigned char *probe, size_t probe_len,
					 const struct FormatEntry *e) {
	if (e->offset + e->sig_len > probe_len)
		return 0;
	return memcmp(probe + e->offset, e->sig, e->sig_len) == 0;
}

static const struct FormatEntry *find_format(const unsigned char *probe,
											 size_t probe_len) {
	size_t i;

	for (i = 0; format_table[i].name != NULL; i++) {
		const struct FormatEntry *e = &format_table[i];
		if (sig_match(probe, probe_len, e))
			return e;
	}

	return NULL;
}

static void report_unreadable_format(const struct FormatEntry *e,
									 const char *source) {
	if (e == NULL) {
		gd_error("gd-readimage: unknown image format");
		return;
	}

	if (!e->enabled) {
		gd_error("gd-readimage: %s image support has been disabled",
				 e->name);
		return;
	}

	gd_error("gd-readimage: %s image does not support reading from %s",
			 e->name, source);
}

/*
  Function: gdImageReadCtx

	Read an image from a <gdIOCtx>, auto-detecting the format by
	magic-byte signatures.

	<gdImageReadCtx> reads the first bytes from the ctx, determines
	the image format by matching known magic-byte signatures, and
	dispatches to the appropriate codec's Ctx reader. The ctx must
	support seeking so it can be rewound after probing.

	Formats without a Ctx reader (XPM, XBM) are not supported by
	this function. Use <gdImageReadFile> for those formats.

	NULL is returned on error or if the format is not recognized.

 Parameters:

	ctx - the input <gdIOCtx>

  Returns:

	A pointer to the new image or NULL if an error occurred.

*/
BGD_DECLARE(gdImagePtr) gdImageReadCtx(gdIOCtxPtr ctx) {
	unsigned char probe[GD_READIMAGE_PROBE_SIZE];
	size_t probe_len;
	const struct FormatEntry *e;

	if (ctx == NULL)
		return NULL;

	probe_len = (size_t)gdGetBuf(probe, GD_READIMAGE_PROBE_SIZE, ctx);
	gdSeek(ctx, 0);

	e = find_format(probe, probe_len);
	if (e != NULL && e->reader != NULL)
		return e->reader(ctx);

	report_unreadable_format(e, "gdIOCtx");
	return NULL;
}

/*
  Function: gdImageReadFile

	Read an image file, auto-detecting the format by magic-byte
	signatures rather than filename extension.

	<gdImageReadFile> opens the file, probes its header bytes to
	determine the format, and calls the appropriate
	_gdImageCreateFrom*Ctx_ function. If the format has no Ctx
	reader (XPM, XBM), it falls back to the filename-based or
	FILE*-based reader.

	NULL is returned on error or if the format is not recognized.

  Parameters:

	filename    - the input file name

  Returns:

	A pointer to the new image or NULL if an error occurred.

*/
BGD_DECLARE(gdImagePtr) gdImageReadFile(const char *filename) {
	FILE *fh;
	gdIOCtxPtr ctx;
	unsigned char probe[GD_READIMAGE_PROBE_SIZE];
	size_t probe_len;
	gdImagePtr im;
	const struct FormatEntry *e;

	if (filename == NULL)
		return NULL;

	fh = fopen(filename, "rb");
	if (!fh)
		return NULL;

	ctx = gdNewFileCtx(fh);
	if (!ctx) {
		fclose(fh);
		return NULL;
	}

	probe_len = (size_t)gdGetBuf(probe, GD_READIMAGE_PROBE_SIZE, ctx);
	gdSeek(ctx, 0);

	e = find_format(probe, probe_len);
	if (e != NULL && e->reader != NULL) {
		im = e->reader(ctx);
		ctx->gd_free(ctx);
		fclose(fh);
		return im;
	}

	ctx->gd_free(ctx);
	fclose(fh);

	if (e != NULL && e->enabled) {
#ifdef HAVE_LIBXPM
		if (strcmp(e->name, "XPM") == 0)
			return gdImageCreateFromXpm((char *)filename);
#endif
		if (strcmp(e->name, "XBM") == 0) {
			FILE *xbm_fh = fopen(filename, "rb");
			if (!xbm_fh)
				return NULL;
			im = gdImageCreateFromXbm(xbm_fh);
			fclose(xbm_fh);
			return im;
		}
	}

	report_unreadable_format(e, "file");
	return NULL;
}
