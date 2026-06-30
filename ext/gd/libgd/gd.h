#ifndef GD_H
#define GD_H 1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_compat.h"

/* Bundled libgd has no separate symbol visibility requirements. */
#ifndef BGD_EXPORT_DATA_PROT
#define BGD_EXPORT_DATA_PROT
#endif
#ifndef BGD_STDCALL
#define BGD_STDCALL
#endif
#ifndef BGD_DECLARE
#define BGD_DECLARE(rt) rt
#endif
#ifndef ARG_NOT_USED
#define ARG_NOT_USED(arg) (void)(arg)
#endif

#define GD_MAJOR_VERSION 2
#define GD_MINOR_VERSION 0
#define GD_RELEASE_VERSION 35
#define GD_EXTRA_VERSION ""
#define GD_VERSION_STRING "2.0.35"

/* gd.h: declarations file for the graphic-draw module.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "AS IS." Thomas Boutell and
 * Boutell.Com, Inc. disclaim all warranties, either express or implied,
 * including but not limited to implied warranties of merchantability and
 * fitness for a particular purpose, with respect to this code and accompanying
 * documentation. */

/* stdio is needed for file I/O. */
#include <stdio.h>
#include "gd_io.h"

/* va_list needed in gdErrorMethod */
#include <stdarg.h>

/* The maximum number of palette entries in palette-based images.
	In the wonderful new world of gd 2.0, you can of course have
	many more colors when using truecolor mode. */

#define gdMaxColors 256

/* Image type. See functions below; you will not need to change
	the elements directly. Use the provided macros to
	access sx, sy, the color table, and colorsTotal for
	read-only purposes. */

/* If 'truecolor' is set true, the image is truecolor;
	pixels are represented by integers, which
	must be 32 bits wide or more.

	True colors are repsented as follows:

	ARGB

	Where 'A' (alpha channel) occupies only the
	LOWER 7 BITS of the MSB. This very small
	loss of alpha channel resolution allows gd 2.x
	to keep backwards compatibility by allowing
	signed integers to be used to represent colors,
	and negative numbers to represent special cases,
	just as in gd 1.x. */

#define gdAlphaMax 127
#define gdAlphaOpaque 0
#define gdAlphaTransparent 127
#define gdRedMax 255
#define gdGreenMax 255
#define gdBlueMax 255
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)
#define gdEffectReplace 0
#define gdEffectAlphaBlend 1
#define gdEffectNormal 2
#define gdEffectOverlay 3
#define gdEffectMultiply 4

#define GD_TRUE 1
#define GD_FALSE 0

#define GD_EPSILON 1e-6

/* This function accepts truecolor pixel values only. The
	source color is composited with the destination color
	based on the alpha channel value of the source color.
	The resulting color is opaque. */

int gdAlphaBlend(int dest, int src);
int gdLayerOverlay(int dst, int src);
int gdLayerMultiply(int dest, int src);

/**
 * Group: Transform
 *
 * Constants: gdInterpolationMethod

 *  GD_BELL				 - Bell
 *  GD_BESSEL			 - Bessel
 *  GD_BILINEAR_FIXED 	 - fixed point bilinear
 *  GD_BICUBIC 			 - Bicubic
 *  GD_BICUBIC_FIXED 	 - fixed point bicubic integer
 *  GD_BLACKMAN			 - Blackman
 *  GD_BOX				 - Box
 *  GD_BSPLINE			 - BSpline
 *  GD_CATMULLROM		 - Catmullrom
 *  GD_GAUSSIAN			 - Gaussian
 *  GD_GENERALIZED_CUBIC - Generalized cubic
 *  GD_HERMITE			 - Hermite
 *  GD_HAMMING			 - Hamming
 *  GD_HANNING			 - Hannig
 *  GD_MITCHELL			 - Mitchell
 *  GD_NEAREST_NEIGHBOUR - Nearest neighbour interpolation
 *  GD_POWER			 - Power
 *  GD_QUADRATIC		 - Quadratic
 *  GD_SINC				 - Sinc
 *  GD_TRIANGLE			 - Triangle
 *  GD_WEIGHTED4		 - 4 pixels weighted bilinear interpolation
 *
 * See also:
 *  <gdSetInterpolationMethod>
 **/
typedef enum {
	GD_DEFAULT          = 0,
	GD_BELL,
	GD_BESSEL,
	GD_BILINEAR_FIXED,
	GD_BICUBIC,
	GD_BICUBIC_FIXED,
	GD_BLACKMAN,
	GD_BOX,
	GD_BSPLINE,
	GD_CATMULLROM,
	GD_GAUSSIAN,
	GD_GENERALIZED_CUBIC,
	GD_HERMITE,
	GD_HAMMING,
	GD_HANNING,
	GD_MITCHELL,
	GD_NEAREST_NEIGHBOUR,
	GD_POWER,
	GD_QUADRATIC,
	GD_SINC,
	GD_TRIANGLE,
	GD_WEIGHTED4,
	GD_LINEAR,
	GD_LANCZOS3,
	GD_LANCZOS8,
	GD_BLACKMAN_BESSEL,
	GD_BLACKMAN_SINC,
	GD_QUADRATIC_BSPLINE,
	GD_CUBIC_SPLINE,
	GD_COSINE,
	GD_WELSH,
	GD_METHOD_COUNT = 30
} gdInterpolationMethod;

/**
 * Group: HEIF Coding Format
 *
 * Values that select the HEIF coding format.
 *
 * Constants: gdHeifCodec
 *
 *  GD_HEIF_CODEC_UNKNOWN
 *  GD_HEIF_CODEC_HEVC
 *  GD_HEIF_CODEC_AV1
 *
 * See also:
 *  - <gdImageHeif>
 */
typedef enum {
	GD_HEIF_CODEC_UNKNOWN = 0,
	GD_HEIF_CODEC_HEVC,
	GD_HEIF_CODEC_AV1 = 4,
} gdHeifCodec;

/**
 * Group: HEIF Chroma Subsampling
 *
 * Values that select the HEIF chroma subsampling.
 *
 * Constants: gdHeifCompression
 *
 *  GD_HEIF_CHROMA_420
 *  GD_HEIF_CHROMA_422
 *  GD_HEIF_CHROMA_444
 *
 * See also:
 *  - <gdImageHeif>
 */
typedef const char *gdHeifChroma;

#define GD_HEIF_CHROMA_420 "420"
#define GD_HEIF_CHROMA_422 "422"
#define GD_HEIF_CHROMA_444 "444"

/**
 * Group: UltraHDR
 *
 * UltraHDR (gain map) APIs are separate from <gdImage>. The UltraHDR handle
 * type is opaque and cannot be passed to existing <gdImage*> functions.
 */

/**
 * Constants: gdUhdrStatus
 *
 * Return status values used by UltraHDR APIs.
 *
 *  GD_UHDR_SUCCESS        - operation succeeded
 *  GD_UHDR_NOT_AVAILABLE  - libgd was built without UltraHDR support
 *  GD_UHDR_E_INVALID      - invalid argument or state
 *  GD_UHDR_E_UNSUPPORTED  - unsupported format or operation
 *  GD_UHDR_E_ENCODE       - encode failure
 *  GD_UHDR_E_DECODE       - decode failure
 */
#define GD_UHDR_SUCCESS 0
#define GD_UHDR_NOT_AVAILABLE -1
#define GD_UHDR_E_INVALID -2
#define GD_UHDR_E_UNSUPPORTED -3
#define GD_UHDR_E_ENCODE -4
#define GD_UHDR_E_DECODE -5

/**
 * Constants: gdUhdrMirrorAxis
 *
 * Mirror axis values used by <gdUhdrImageMirror>.
 *
 *  GD_UHDR_MIRROR_HORIZONTAL
 *  GD_UHDR_MIRROR_VERTICAL
 */
#define GD_UHDR_MIRROR_HORIZONTAL 0
#define GD_UHDR_MIRROR_VERTICAL 1

/**
 * Enum: gdUhdrFormat
 *
 * UltraHDR container format selector.
 *
 *  GD_UHDR_FORMAT_JPEG - UltraHDR JPEG (currently supported)
 *  GD_UHDR_FORMAT_WEBP - reserved for future support
 *  GD_UHDR_FORMAT_HEIF - reserved for future support
 */
typedef enum {
	GD_UHDR_FORMAT_JPEG = 0,
	GD_UHDR_FORMAT_WEBP = 1,
	GD_UHDR_FORMAT_HEIF = 2
} gdUhdrFormat;

/**
 * Typedef: gdUhdrImage
 *
 * Opaque UltraHDR image handle.
 */
typedef struct gdUhdrImageStruct gdUhdrImage;

/**
 * Typedef: gdUhdrImagePtr
 *
 * Pointer to <gdUhdrImage>.
 */
typedef gdUhdrImage *gdUhdrImagePtr;

/**
 * Typedef: gdUhdrError
 *
 * Structured error details for UltraHDR APIs.
 *
 * Fields:
 *  code          - libgd UltraHDR status code (GD_UHDR_*)
 *  provider_code - underlying provider error code, if any
 *  message       - optional human-readable detail string
 */
typedef struct {
	int code;
	int provider_code;
	char message[128];
} gdUhdrError;

/**
 * Typedef: gdUhdrErrorPtr
 *
 * Pointer to <gdUhdrError>.
 */
typedef gdUhdrError *gdUhdrErrorPtr;

/* define struct with name and func ptr and add it to gdImageStruct
 * gdInterpolationMethod interpolation; */

/* Interpolation function ptr */
typedef double (*interpolation_method)(double, double);

/*
   Group: Types

   typedef: gdImage

   typedef: gdImagePtr

   The data structure in which gd stores images. <gdImageCreate>,
   <gdImageCreateTrueColor> and the various image file-loading functions
   return a pointer to this type, and the other functions expect to
   receive a pointer to this type as their first argument.

   *gdImagePtr* is a pointer to *gdImage*.

   See also:
	 <Accessor Macros>

   (Previous versions of this library encouraged directly manipulating
   the contents of the struct but we are attempting to move away from
   this practice so the fields are no longer documented here.  If you
   need to poke at the internals of this struct, feel free to look at
   *gd.h*.)
*/
typedef struct gdImageStruct {
	/* Palette-based image pixels */
	unsigned char ** pixels;
	int sx;
	int sy;
	/* These are valid in palette images only. See also
		'alpha', which appears later in the structure to
		preserve binary backwards compatibility */
	int colorsTotal;
	int red[gdMaxColors];
	int green[gdMaxColors];
	int blue[gdMaxColors];
	int open[gdMaxColors];
	/* For backwards compatibility, this is set to the
		first palette entry with 100% transparency,
		and is also set and reset by the
		gdImageColorTransparent function. Newer
		applications can allocate palette entries
		with any desired level of transparency; however,
		bear in mind that many viewers, notably
		many web browsers, fail to implement
		full alpha channel for PNG and provide
		support for full opacity or transparency only. */
	int transparent;
	int *polyInts;
	int polyAllocated;
	struct gdImageStruct *brush;
	struct gdImageStruct *tile;
	int brushColorMap[gdMaxColors];
	int tileColorMap[gdMaxColors];
	int styleLength;
	int stylePos;
	int *style;
	int interlace;
	/* New in 2.0: thickness of line. Initialized to 1. */
	int thick;
	/* New in 2.0: alpha channel for palettes. Note that only
		Macintosh Internet Explorer and (possibly) Netscape 6
		really support multiple levels of transparency in
		palettes, to my knowledge, as of 2/15/01. Most
		common browsers will display 100% opaque and
		100% transparent correctly, and do something
		unpredictable and/or undesirable for levels
		in between. TBB */
	int alpha[gdMaxColors];
	/* Truecolor flag and pixels. New 2.0 fields appear here at the
		end to minimize breakage of existing object code. */
	int trueColor;
	int ** tpixels;
	/* Should alpha channel be copied, or applied, each time a
		pixel is drawn? This applies to truecolor images only.
		No attempt is made to alpha-blend in palette images,
		even if semitransparent palette entries exist.
		To do that, build your image as a truecolor image,
		then quantize down to 8 bits. */
	int alphaBlendingFlag;
	/* Should the alpha channel of the image be saved? This affects
		PNG at the moment; other future formats may also
		have that capability. JPEG doesn't. */
	int saveAlphaFlag;

	/* There should NEVER BE ACCESSOR MACROS FOR ITEMS BELOW HERE, so this
	   part of the structure can be safely changed in new releases. */

	/* 2.0.12: anti-aliased globals. 2.0.26: just a few vestiges after
	  switching to the fast, memory-cheap implementation from PHP-gd. */
	int AA;
	int AA_color;
	int AA_dont_blend;

	/* 2.0.12: simple clipping rectangle. These values
	  must be checked for safety when set; please use
	  gdImageSetClip */
	int cx1;
	int cy1;
	int cx2;
	int cy2;

	/* 2.1.0: allows to specify resolution in dpi */
	unsigned int res_x;
	unsigned int res_y;

	/* Selects quantization method, see gdImageTrueColorToPaletteSetMethod() and
	 * gdPaletteQuantizationMethod enum. */
	int paletteQuantizationMethod;
	/* speed/quality trade-off. 1 = best quality, 10 = best speed. 0 =
	   method-specific default. Applicable to GD_QUANT_LIQ and
	   GD_QUANT_NEUQUANT. */
	int paletteQuantizationSpeed;
	/* Image will remain true-color if conversion to palette cannot achieve
	   given quality. Value from 1 to 100, 1 = ugly, 100 = perfect. Applicable
	   to GD_QUANT_LIQ.*/
	int paletteQuantizationMinQuality;
	/* Image will use minimum number of palette colors needed to achieve given
	   quality. Must be higher than paletteQuantizationMinQuality Value from 1
	   to 100, 1 = ugly, 100 = perfect. Applicable to GD_QUANT_LIQ.*/
	int paletteQuantizationMaxQuality;
	gdInterpolationMethod interpolation_id;
	interpolation_method interpolation;
} gdImage;

typedef gdImage * gdImagePtr;

typedef struct gdImageMetadata gdImageMetadata;

#define GD_META_OK 0
#define GD_META_ERR_FORMAT -1
#define GD_META_ERR_PARSE -2
#define GD_META_ERR_NOMEM -3
#define GD_META_ERR_LIMIT -4
#define GD_META_ERR_UNSUPPORTED -5
#define GD_META_ERR_INVALID -6

#define GD_METADATA_DEFAULT_MAX_PROFILE_SIZE ((size_t)64 * 1024 * 1024)
#define GD_METADATA_DEFAULT_MAX_TOTAL_SIZE ((size_t)256 * 1024 * 1024)

BGD_DECLARE(gdImageMetadata *) gdImageMetadataCreate(void);
BGD_DECLARE(void) gdImageMetadataFree(gdImageMetadata *metadata);
BGD_DECLARE(void) gdImageMetadataReset(gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataSetLimits(gdImageMetadata *metadata, size_t max_profile_size,
						 size_t max_total_size);
BGD_DECLARE(void)
gdImageMetadataGetLimits(const gdImageMetadata *metadata,
						 size_t *max_profile_size, size_t *max_total_size);
BGD_DECLARE(int)
gdImageMetadataSetProfile(gdImageMetadata *metadata, const char *key,
						  const unsigned char *data, size_t size);
BGD_DECLARE(const unsigned char *)
gdImageMetadataGetProfile(const gdImageMetadata *metadata, const char *key,
						  size_t *size);
BGD_DECLARE(int)
gdImageMetadataRemoveProfile(gdImageMetadata *metadata, const char *key);
BGD_DECLARE(size_t)
gdImageMetadataGetProfileCount(const gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataGetProfileAt(const gdImageMetadata *metadata, size_t index,
							const char **key, const unsigned char **data,
							size_t *size);

/* Point type for use in polygon drawing. */

/**
 * Group: Types
 *
 * typedef: gdPointF
 *  Defines a point in a 2D coordinate system using floating point
 *  values.
 * x - Floating point position (increase from left to right)
 * y - Floating point Row position (increase from top to bottom)
 *
 * typedef: gdPointFPtr
 *  Pointer to a <gdPointF>
 *
 * See also:
 *  <gdImageCreate>, <gdImageCreateTrueColor>,
 **/
typedef struct
{
	double x, y;
}
gdPointF, *gdPointFPtr;

typedef struct {
	/* # of characters in font */
	int nchars;
	/* First character is numbered... (usually 32 = space) */
	int offset;
	/* Character width and height */
	int w;
	int h;
	/* Font data; array of characters, one row after another.
		Easily included in code, also easily loaded from
		data files. */
	char *data;
} gdFont;

/* Text functions take these. */
typedef gdFont *gdFontPtr;

typedef void(*gdErrorMethod)(int, const char *, va_list);

void gdSetErrorMethod(gdErrorMethod);
void gdClearErrorMethod(void);

/**
 * Group: Types
 *
 * typedef: gdRect
 *  Defines a rectilinear region.
 *
 *  x - left position
 *  y - right position
 *  width - Rectangle width
 *  height - Rectangle height
 *
 * typedef: gdRectPtr
 *  Pointer to a <gdRect>
 *
 * See also:
 *  <gdSetInterpolationMethod>
 **/
typedef struct
{
	int x, y;
	int width, height;
}
gdRect, *gdRectPtr;

/* For backwards compatibility only. Use gdImageSetStyle()
	for MUCH more flexible line drawing. Also see
	gdImageSetBrush(). */
#define gdDashSize 4

/* Special colors. */

#define gdStyled (-2)
#define gdBrushed (-3)
#define gdStyledBrushed (-4)
#define gdTiled (-5)

/* NOT the same as the transparent color index.
	This is used in line styles only. */
#define gdTransparent (-6)

#define gdAntiAliased (-7)

/* Functions to manipulate images. */

/* Creates a palette-based image (up to 256 colors). */
gdImagePtr gdImageCreate(int sx, int sy);

/* An alternate name for the above (2.0). */
#define gdImageCreatePalette gdImageCreate

/* Creates a truecolor image (millions of colors). */
gdImagePtr gdImageCreateTrueColor(int sx, int sy);

/* Creates an image from various file types. These functions
	return a palette or truecolor image based on the
	nature of the file being loaded. Truecolor PNG
	stays truecolor; palette PNG stays palette-based;
	JPEG is always truecolor. */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPng(FILE *fd);
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx(gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngCtxWithMetadata(gdIOCtxPtr in, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr(int size, void *data);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngPtrWithMetadata(int size, void *data, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoi(FILE *fd);
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiCtx(gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromQoiCtxWithMetadata(gdIOCtxPtr in, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiPtr(int size, void *data);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromQoiPtrWithMetadata(int size, void *data, gdImageMetadata *metadata);
gdImagePtr gdImageCreateFromWBMP(FILE *inFile);
gdImagePtr gdImageCreateFromWBMPCtx(gdIOCtx *infile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpeg(FILE *infile);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegEx(FILE *infile, int ignore_warning);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtx(gdIOCtxPtr infile);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxEx(gdIOCtxPtr infile, int ignore_warning);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxWithMetadata(gdIOCtxPtr infile,
									 gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxExWithMetadata(gdIOCtxPtr infile, int ignore_warning,
									   gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtr(int size, void *data);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrEx(int size, void *data, int ignore_warning);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrWithMetadata(int size, void *data, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtrExWithMetadata(int size, void *data, int ignore_warning, gdImageMetadata *metadata);
BGD_DECLARE(const char *) gdJpegGetVersionString();
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpCtx(gdIOCtxPtr infile);

typedef struct gdWebpReadStruct *gdWebpReadPtr;
typedef struct gdWebpWriteStruct *gdWebpWritePtr;

typedef struct {
	int width;
	int height;
	int frameCount;
	int loopCount;
	int backgroundColor;
	int formatFlags;
} gdWebpInfo;

typedef struct {
	int frameIndex;
	int x;
	int y;
	int width;
	int height;
	int duration;
	int timestamp;
	int dispose;
	int blend;
	int hasAlpha;
	int complete;
} gdWebpFrameInfo;

typedef struct {
	int canvasWidth;
	int canvasHeight;
	int loopCount;
	int backgroundColor;
	int quality;
	int lossless;
	int method;
	int minimizeSize;
	int kmin;
	int kmax;
	int allowMixed;
} gdWebpWriteOptions;

enum { gdWebpDisposeNone, gdWebpDisposeBackground };
enum { gdWebpBlendAlpha, gdWebpBlendNone };

BGD_DECLARE(int) gdWebpIsAnimated(FILE *fd);
BGD_DECLARE(int) gdWebpIsAnimatedCtx(gdIOCtxPtr in);
BGD_DECLARE(int) gdWebpIsAnimatedPtr(int size, void *data);
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpen(FILE *fd);
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenCtx(gdIOCtxPtr in);
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenPtr(int size, void *data);
BGD_DECLARE(void) gdWebpReadClose(gdWebpReadPtr webp);
BGD_DECLARE(int) gdWebpReadGetInfo(gdWebpReadPtr webp, gdWebpInfo *info);
BGD_DECLARE(int)
gdWebpReadNextFrame(gdWebpReadPtr webp, gdWebpFrameInfo *info,
					gdImagePtr *frame);
BGD_DECLARE(int)
gdWebpReadNextImage(gdWebpReadPtr webp, gdWebpFrameInfo *info,
					gdImagePtr *image);
BGD_DECLARE(gdImagePtr) gdWebpReadCloneImage(gdWebpReadPtr webp);
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpen(FILE *outFile, const gdWebpWriteOptions *options);
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenCtx(gdIOCtxPtr out, const gdWebpWriteOptions *options);
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenPtr(const gdWebpWriteOptions *options);
BGD_DECLARE(int)
gdWebpWriteAddImage(gdWebpWritePtr webp, gdImagePtr image, int durationMs);
BGD_DECLARE(void) gdWebpWriteClose(gdWebpWritePtr webp);
BGD_DECLARE(void *) gdWebpWritePtrFinish(gdWebpWritePtr webp, int *size);

BGD_DECLARE(gdImagePtr) gdImageCreateFromTga(FILE *fp);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaCtx(gdIOCtxPtr ctx);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaPtr(int size, void *data);

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxl(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlCtx(gdIOCtxPtr infile);

BGD_DECLARE(void) gdImageJxl(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void) gdImageJxlEx(gdImagePtr im, FILE *outFile,
                                int lossless, float distance, int effort);
BGD_DECLARE(void *) gdImageJxlPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *) gdImageJxlPtrEx(gdImagePtr im, int *size,
                                     int lossless, float distance, int effort);
BGD_DECLARE(void) gdImageJxlCtx(gdImagePtr im, gdIOCtxPtr outfile);
BGD_DECLARE(void) gdImageJxlCtxEx(gdImagePtr im, gdIOCtxPtr outfile,
                                   int lossless, float distance, int effort);

/* Animation API */
typedef struct gdJxlAnimReader *gdJxlAnimReaderPtr;
typedef struct gdJxlAnim      *gdJxlAnimPtr;

typedef struct {
    int delay_ms;
    int x_offset;
    int y_offset;
    int width;
    int height;
    int blend_mode;
    int is_last;
} gdJxlFrameInfo;

#define GD_JXL_BLEND_REPLACE  0
#define GD_JXL_BLEND_ADD      1
#define GD_JXL_BLEND_BLEND    2
#define GD_JXL_BLEND_MULADD   3
#define GD_JXL_BLEND_MUL      4

BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreate(FILE *inFile);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreatePtr(int size, void *data);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateCtx(gdIOCtxPtr inCtx);

BGD_DECLARE(gdImagePtr) gdJxlReadNextImage(
    gdJxlAnimReaderPtr reader,
    int *delay_ms);

BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRaw(FILE *inFile);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRawPtr(int size, void *data);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRawCtx(gdIOCtxPtr inCtx);

BGD_DECLARE(gdImagePtr) gdJxlReadNextFrame(
    gdJxlAnimReaderPtr reader,
    gdJxlFrameInfo *info);

BGD_DECLARE(void) gdImageJxlAnimReaderDestroy(gdJxlAnimReaderPtr reader);

BGD_DECLARE(gdJxlAnimPtr) gdImageJxlAnimBegin(
    FILE *outFile,
    int width, int height,
    int lossless, float distance, int effort);

BGD_DECLARE(gdJxlAnimPtr) gdImageJxlAnimBeginCtx(
    gdIOCtxPtr outCtx,
    int width, int height,
    int lossless, float distance, int effort);

BGD_DECLARE(gdJxlAnimPtr) gdImageJxlAnimBeginPtr(
    int width, int height,
    int lossless, float distance, int effort);

BGD_DECLARE(int) gdImageJxlAnimAddFrame(
    gdJxlAnimPtr anim,
    gdImagePtr im,
    int delay_ms);

BGD_DECLARE(int) gdImageJxlAnimEnd(gdJxlAnimPtr anim);
BGD_DECLARE(void *) gdImageJxlAnimEndPtr(gdJxlAnimPtr anim, int *size);

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeif(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifCtx(gdIOCtxPtr infile);

BGD_DECLARE(gdImagePtr) gdImageCreateFromAvif(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifCtx(gdIOCtxPtr infile);

gdImagePtr gdImageCreateFromBmp (FILE * inFile);
gdImagePtr gdImageCreateFromBmpPtr (int size, void *data);
gdImagePtr gdImageCreateFromBmpCtx (gdIOCtxPtr infile);

const char * gdPngGetVersionString(void);
/* UltraHDR load API */

BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromFile(const char *filename, int format, gdUhdrErrorPtr err);
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromCtx(gdIOCtxPtr ctx, int format, gdUhdrErrorPtr err);
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromPtr(int size, void *data, int format, gdUhdrErrorPtr err);
BGD_DECLARE(void) gdUhdrImageDestroy(gdUhdrImagePtr im);
/* A custom data source. */
/* The source function must return -1 on error, otherwise the number
        of bytes fetched. 0 is EOF, not an error! */
/* context will be passed to your source function. */

typedef struct {
        int (*source) (void *context, char *buffer, int len);
        void *context;
} gdSource, *gdSourcePtr;

gdImagePtr gdImageCreateFromPngSource(gdSourcePtr in);

gdImagePtr gdImageCreateFromGd(FILE *in);
gdImagePtr gdImageCreateFromGdCtx(gdIOCtxPtr in);

gdImagePtr gdImageCreateFromGd2(FILE *in);
gdImagePtr gdImageCreateFromGd2Ctx(gdIOCtxPtr in);

gdImagePtr gdImageCreateFromGd2Part(FILE *in, int srcx, int srcy, int w, int h);
gdImagePtr gdImageCreateFromGd2PartCtx(gdIOCtxPtr in, int srcx, int srcy, int w, int h);

gdImagePtr gdImageCreateFromXbm(FILE *fd);
void gdImageXbmCtx(gdImagePtr image, char* file_name, int fg, gdIOCtx * out);

/* NOTE: filename, not FILE */
BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm(char *filename);

void gdImageDestroy(gdImagePtr im);

/* Replaces or blends with the background depending on the
	most recent call to gdImageAlphaBlending and the
	alpha channel value of 'color'; default is to overwrite.
	Tiling and line styling are also implemented
	here. All other gd drawing functions pass through this call,
	allowing for many useful effects. */

void gdImageSetPixel(gdImagePtr im, int x, int y, int color);

int gdImageGetTrueColorPixel (gdImagePtr im, int x, int y);
int gdImageGetPixel(gdImagePtr im, int x, int y);

void gdImageAABlend(gdImagePtr im);

void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageAALine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/* For backwards compatibility only. Use gdImageSetStyle()
	for much more flexible line drawing. */
void gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Corners specified (not width and height). Upper left first, lower right
 	second. */
void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Solid bar. Upper left corner first, lower right corner second. */
void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageSetClip(gdImagePtr im, int x1, int y1, int x2, int y2);
void gdImageGetClip(gdImagePtr im, int *x1P, int *y1P, int *x2P, int *y2P);
void gdImageSetResolution(gdImagePtr im, const unsigned int res_x, const unsigned int res_y);
void gdImageChar(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);
void gdImageCharUp(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);
void gdImageString(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);
void gdImageStringUp(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);
void gdImageString16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);
void gdImageStringUp16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);

/*
 * The following functions are required to be called prior to the
 * use of any sort of threads in a module load / shutdown function
 * respectively.
 */
void gdFontCacheMutexSetup(void);
void gdFontCacheMutexShutdown(void);

/* 2.0.16: for thread-safe use of gdImageStringFT and friends,
 * call this before allowing any thread to call gdImageStringFT.
 * Otherwise it is invoked by the first thread to invoke
 * gdImageStringFT, with a very small but real risk of a race condition.
 * Return 0 on success, nonzero on failure to initialize freetype.
 */
int gdFontCacheSetup(void);

/* Optional: clean up after application is done using fonts in gdImageStringFT(). */
void gdFontCacheShutdown(void);

/* Calls gdImageStringFT. Provided for backwards compatibility only. */
char *gdImageStringTTF(gdImage *im, int *brect, int fg, char *fontlist,
                double ptsize, double angle, int x, int y, char *string);

/* FreeType 2 text output */
char *gdImageStringFT(gdImage *im, int *brect, int fg, char *fontlist,
                double ptsize, double angle, int x, int y, char *string);

typedef struct {
	double linespacing;	/* fine tune line spacing for '\n' */
	int flags;		/* Logical OR of gdFTEX_ values */
	int charmap;		/* TBB: 2.0.12: may be gdFTEX_Unicode,
				   gdFTEX_Shift_JIS, gdFTEX_Big5 or gdFTEX_MacRoman;
				   when not specified, maps are searched
				   for in the above order. */
	int hdpi;
	int vdpi;
}
 gdFTStringExtra, *gdFTStringExtraPtr;

#define gdFTEX_LINESPACE 1
#define gdFTEX_CHARMAP 2
#define gdFTEX_RESOLUTION 4

/* These are NOT flags; set one in 'charmap' if you set the gdFTEX_CHARMAP bit in 'flags'. */
#define gdFTEX_Unicode 0
#define gdFTEX_Shift_JIS 1
#define gdFTEX_Big5 2
#define gdFTEX_MacRoman 3

/* FreeType 2 text output with fine tuning */
char *
gdImageStringFTEx(gdImage * im, int *brect, int fg, char * fontlist,
		double ptsize, double angle, int x, int y, char * string,
		gdFTStringExtraPtr strex);


/* Point type for use in polygon drawing. */
typedef struct {
	int x, y;
} gdPoint, *gdPointPtr;

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c);
void gdImageOpenPolygon(gdImagePtr im, gdPointPtr p, int n, int c);
void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c);

/* These functions still work with truecolor images,
	for which they never return error. */
int gdImageColorAllocate(gdImagePtr im, int r, int g, int b);
/* gd 2.0: palette entries with non-opaque transparency are permitted. */
int gdImageColorAllocateAlpha(gdImagePtr im, int r, int g, int b, int a);
/* Assumes opaque is the preferred alpha channel value */
int gdImageColorClosest(gdImagePtr im, int r, int g, int b);
/* Closest match taking all four parameters into account.
	A slightly different color with the same transparency
	beats the exact same color with radically different
	transparency */
int gdImageColorClosestAlpha(gdImagePtr im, int r, int g, int b, int a);
/* An alternate method */
int gdImageColorClosestHWB(gdImagePtr im, int r, int g, int b);
/* Returns exact, 100% opaque matches only */
int gdImageColorExact(gdImagePtr im, int r, int g, int b);
/* Returns an exact match only, including alpha */
int gdImageColorExactAlpha(gdImagePtr im, int r, int g, int b, int a);
/* Opaque only */
int gdImageColorResolve(gdImagePtr im, int r, int g, int b);
/* Based on gdImageColorExactAlpha and gdImageColorClosestAlpha */
int gdImageColorResolveAlpha(gdImagePtr im, int r, int g, int b, int a);

/* A simpler way to obtain an opaque truecolor value for drawing on a
	truecolor image. Not for use with palette images! */

#define gdTrueColor(r, g, b) (((r) << 16) + \
	((g) << 8) + \
	(b))

/* Returns a truecolor value with an alpha channel component.
	gdAlphaMax (127, **NOT 255**) is transparent, 0 is completely
	opaque. */

#define gdTrueColorAlpha(r, g, b, a) (((a) << 24) + \
	((r) << 16) + \
	((g) << 8) + \
	(b))

void gdImageColorDeallocate(gdImagePtr im, int color);

/* Converts a truecolor image to a palette-based image,
	using a high-quality two-pass quantization routine
	which attempts to preserve alpha channel information
	as well as R/G/B color information when creating
	a palette. If ditherFlag is set, the image will be
	dithered to approximate colors better, at the expense
	of some obvious "speckling." colorsWanted can be
	anything up to 256. If the original source image
	includes photographic information or anything that
	came out of a JPEG, 256 is strongly recommended.

	Better yet, don't use this function -- write real
	truecolor PNGs and JPEGs. The disk space gain of
        conversion to palette is not great (for small images
        it can be negative) and the quality loss is ugly. */

gdImagePtr gdImageCreatePaletteFromTrueColor (gdImagePtr im, int ditherFlag, int colorsWanted);

int gdImageTrueColorToPalette(gdImagePtr im, int ditherFlag, int colorsWanted);
int gdImagePaletteToTrueColor(gdImagePtr src);

/* An attempt at getting the results of gdImageTrueColorToPalette
	to look a bit more like the original (im1 is the original
	and im2 is the palette version */
int gdImageColorMatch(gdImagePtr im1, gdImagePtr im2);

/* Specifies a color index (if a palette image) or an
	RGB color (if a truecolor image) which should be
	considered 100% transparent. FOR TRUECOLOR IMAGES,
	THIS IS IGNORED IF AN ALPHA CHANNEL IS BEING
	SAVED. Use gdImageSaveAlpha(im, 0); to
	turn off the saving of a full alpha channel in
	a truecolor image. Note that gdImageColorTransparent
	is usually compatible with older browsers that
	do not understand full alpha channels well. TBB */
void gdImageColorTransparent(gdImagePtr im, int color);

void gdImagePaletteCopy(gdImagePtr dst, gdImagePtr src);
BGD_DECLARE(void) gdImagePng(gdImagePtr im, FILE *out);
BGD_DECLARE(void) gdImagePngCtx(gdImagePtr im, gdIOCtx *out);

BGD_DECLARE(void)
gdImagePngCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out,
						  const gdImageMetadata *metadata);

#define GD_PNG_FILTER_AUTO 0U
#define GD_PNG_FILTER_NONE (1U << 0)
#define GD_PNG_FILTER_SUB (1U << 1)
#define GD_PNG_FILTER_UP (1U << 2)
#define GD_PNG_FILTER_AVERAGE (1U << 3)
#define GD_PNG_FILTER_PAETH (1U << 4)
#define GD_PNG_FILTER_ALL                                                    \
	(GD_PNG_FILTER_NONE | GD_PNG_FILTER_SUB | GD_PNG_FILTER_UP |             \
	 GD_PNG_FILTER_AVERAGE | GD_PNG_FILTER_PAETH)

enum {
	GD_PNG_COMPRESSION_STRATEGY_DEFAULT = 0,
	GD_PNG_COMPRESSION_STRATEGY_FILTERED,
	GD_PNG_COMPRESSION_STRATEGY_HUFFMAN_ONLY,
	GD_PNG_COMPRESSION_STRATEGY_RLE,
	GD_PNG_COMPRESSION_STRATEGY_FIXED
};

typedef struct {
	size_t struct_size;
	int compression_level;
	unsigned int filters;
	int compression_strategy;
	const gdImageMetadata *metadata;
} gdPngWriteOptions;

BGD_DECLARE(void) gdPngWriteOptionsInit(gdPngWriteOptions *options);
BGD_DECLARE(int) gdImagePngWithOptions(gdImagePtr im, FILE *out, const gdPngWriteOptions *options);
BGD_DECLARE(int) gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtxPtr out, const gdPngWriteOptions *options);
BGD_DECLARE(void *) gdImagePngPtrWithOptions(gdImagePtr im, int *size, const gdPngWriteOptions *options);
BGD_DECLARE(void) gdImageQoi(gdImagePtr im, FILE *out);
BGD_DECLARE(void) gdImageQoiCtx(gdImagePtr im, gdIOCtxPtr out);
BGD_DECLARE(void) gdImageQoiCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out, const gdImageMetadata *metadata);
BGD_DECLARE(void) gdImageGifCtx(gdImagePtr im, gdIOCtxPtr out);
BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size);
BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtxPtr out);

BGD_DECLARE(void *) gdImageBmpPtr(gdImagePtr im, int *size, int compression);
BGD_DECLARE(void) gdImageBmp(gdImagePtr im, FILE *outFile, int compression);
BGD_DECLARE(void) gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression);

#define GD_BMP_COMPRESS_NONE 0
#define GD_BMP_COMPRESS_RLE8 1
#define GD_BMP_COMPRESS_RLE4 2

#define GD_BMP_FLAG_NONE 0
#define GD_BMP_FLAG_FORCE_V4HDR (1 << 0)
#define GD_BMP_FLAG_QUANTIZE (1 << 1)
#define GD_BMP_FLAG_RGB555 (1 << 2)

BGD_DECLARE(void *)
gdImageBmpPtrEx(gdImagePtr im, int *size, int bpp, int compression, int flags);
BGD_DECLARE(void)
gdImageBmpEx(gdImagePtr im, FILE *outFile, int bpp, int compression, int flags);
BGD_DECLARE(void)
gdImageBmpCtxEx(gdImagePtr im, gdIOCtxPtr out, int bpp, int compression,
				int flags);

enum { GD_QOI_SRGB = 0, GD_QOI_LINEAR = 1 };

BGD_DECLARE(void) gdImageQoi(gdImagePtr im, FILE *out);
BGD_DECLARE(void) gdImageQoiCtx(gdImagePtr im, gdIOCtxPtr out);
BGD_DECLARE(void)
gdImageQoiCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out,
						  const gdImageMetadata *metadata);
BGD_DECLARE(void) gdImageQoiEx(gdImagePtr im, FILE *out, int colorspace);
BGD_DECLARE(void) gdImageQoiCtxEx(gdImagePtr im, gdIOCtxPtr out, int colorspace);
BGD_DECLARE(void)
gdImageQoiCtxExWithMetadata(gdImagePtr im, gdIOCtxPtr out, int colorspace,
							const gdImageMetadata *metadata);


void gdImageGif(gdImagePtr im, FILE *out);
void gdImageGifCtx(gdImagePtr im, gdIOCtx *out);

void * gdImageBmpPtr(gdImagePtr im, int *size, int compression);
void gdImageBmp(gdImagePtr im, FILE *outFile, int compression);
void gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression);

/* 2.0.12: Compression level: 0-9 or -1, where 0 is NO COMPRESSION at all,
 * 1 is FASTEST but produces larger files, 9 provides the best
 * compression (smallest files) but takes a long time to compress, and
 * -1 selects the default compiled into the zlib library.
 */
void gdImagePngEx(gdImagePtr im, FILE *out, int level);
void gdImagePngCtxEx(gdImagePtr im, gdIOCtx *out, int level);

void gdImageWBMP(gdImagePtr image, int fg, FILE *out);
void gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtx *out);

BGD_DECLARE(int) gdUhdrIsAvailable(void);
BGD_DECLARE(int) gdUhdrImageWidth(gdUhdrImagePtr im);
BGD_DECLARE(int) gdUhdrImageHeight(gdUhdrImagePtr im);
BGD_DECLARE(int) gdUhdrImageHasGainMap(gdUhdrImagePtr im);
BGD_DECLARE(int)
gdUhdrImageResize(gdUhdrImagePtr im, int width, int height, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageCrop(gdUhdrImagePtr im, int left, int top, int width, int height,
				gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageRotate(gdUhdrImagePtr im, int degrees, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageMirror(gdUhdrImagePtr im, int axis, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageFile(gdUhdrImagePtr im, const char *filename, int format,
				int quality, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageCtx(gdUhdrImagePtr im, gdIOCtxPtr ctx, int format, int quality,
			   gdUhdrErrorPtr err);
BGD_DECLARE(void *)
gdUhdrImageWritePtr(gdUhdrImagePtr im, int *size, int format, int quality,
					gdUhdrErrorPtr err);
BGD_DECLARE(gdImagePtr)
gdUhdrImageGetSdr(gdUhdrImagePtr im, gdUhdrErrorPtr err);

/* Guaranteed to correctly free memory returned
	by the gdImage*Ptr functions */
void gdFree(void *m);

/* Best to free this memory with gdFree(), not free() */
void *gdImageWBMPPtr(gdImagePtr im, int *size, int fg);

/* 100 is highest quality (there is always a little loss with JPEG).
       0 is lowest. 10 is about the lowest useful setting. */
BGD_DECLARE(void) gdImageJpeg(gdImagePtr im, FILE *out, int quality);
BGD_DECLARE(void) gdImageJpegCtx(gdImagePtr im, gdIOCtxPtr out, int quality);
BGD_DECLARE(void)
gdImageJpegCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out, int quality,
						   const gdImageMetadata *metadata);

/**
 * Group: WebP
 *
 * Constant: gdWebpLossless
 *
 * Lossless quality threshold. When image quality is greater than or equal to
 * <gdWebpLossless>, the image will be written in the lossless WebP format.
 *
 * See also:
 *   - <gdImageWebpCtx>
 */
#define gdWebpLossless 101

void gdImageWebpCtx (gdImagePtr im, gdIOCtx * outfile, int quality);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageJpegPtr(gdImagePtr im, int *size, int quality);
BGD_DECLARE(void *)
gdImageJpegPtrWithMetadata(gdImagePtr im, int *size, int quality,
						   const gdImageMetadata *metadata);

gdImagePtr gdImageCreateFromGif(FILE *fd);
gdImagePtr gdImageCreateFromGifCtx(gdIOCtxPtr in);
gdImagePtr gdImageCreateFromGifSource(gdSourcePtr in);

void gdImageAvif(gdImagePtr im, FILE *outfile);
void gdImageAvifEx(gdImagePtr im, FILE *outfile, int quality, int speed);
void *gdImageAvifPtr(gdImagePtr im, int *size);
void *gdImageAvifPtrEx(gdImagePtr im, int *size, int quality, int speed);
void gdImageAvifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, int speed);

/* A custom data sink. For backwards compatibility. Use
	gdIOCtx instead. */
/* The sink function must return -1 on error, otherwise the number
        of bytes written, which must be equal to len. */
/* context will be passed to your sink function. */
typedef struct {
        int (*sink) (void *context, const char *buffer, int len);
        void *context;
} gdSink, *gdSinkPtr;

void gdImagePngToSink(gdImagePtr im, gdSinkPtr out);

void gdImageGd(gdImagePtr im, FILE *out);
void gdImageGd2(gdImagePtr im, FILE *out, int cs, int fmt);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImagePngPtr(gdImagePtr im, int *size);

BGD_DECLARE(void *) gdImagePngPtrEx(gdImagePtr im, int *size, int level);
BGD_DECLARE(void *)
gdImagePngPtrWithMetadata(gdImagePtr im, int *size,
						  const gdImageMetadata *metadata);
BGD_DECLARE(void *)
gdImagePngPtrExWithMetadata(gdImagePtr im, int *size, int level,
							const gdImageMetadata *metadata);
BGD_DECLARE(void)
gdImagePngCtxExWithMetadata(gdImagePtr im, gdIOCtxPtr out, int level,
							const gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataInjectPng(void **data, int *size,
						 const gdImageMetadata *metadata);
BGD_DECLARE(void *) gdImageQoiPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *) gdImageQoiPtrEx(gdImagePtr im, int *size, int colorspace);
BGD_DECLARE(void *) gdImageQoiPtrWithMetadata(gdImagePtr im, int *size, const gdImageMetadata *metadata);
BGD_DECLARE(void *) gdImageQoiPtrExWithMetadata(gdImagePtr im, int *size, int colorspace, const gdImageMetadata *metadata);
BGD_DECLARE(int) gdImageMetadataInjectQoi(void **data, int *size, const gdImageMetadata *metadata);
BGD_DECLARE(void) gdImageQoiToSink(gdImagePtr im, gdSinkPtr out);

/* Best to free this memory with gdFree(), not free() */
void* gdImageGdPtr(gdImagePtr im, int *size);

/* Best to free this memory with gdFree(), not free() */
void* gdImageGd2Ptr(gdImagePtr im, int cs, int fmt, int *size);

void gdImageEllipse(gdImagePtr im, int cx, int cy, int w, int h, int c);

/* Style is a bitwise OR ( | operator ) of these.
	gdArc and gdChord are mutually exclusive;
	gdChord just connects the starting and ending
	angles with a straight line, while gdArc produces
	a rounded edge. gdPie is a synonym for gdArc.
	gdNoFill indicates that the arc or chord should be
	outlined, not filled. gdEdged, used together with
	gdNoFill, indicates that the beginning and ending
	angles should be connected to the center; this is
	a good way to outline (rather than fill) a
	'pie slice'. */
#define gdArc   0
#define gdPie   gdArc
#define gdChord 1
#define gdNoFill 2
#define gdEdged 4

void gdImageFilledArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color, int style);
void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color);
void gdImageFilledEllipse(gdImagePtr im, int cx, int cy, int w, int h, int color);
void gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color);
void gdImageFill(gdImagePtr im, int x, int y, int color);
void gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h);
void gdImageCopyMerge(gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
			int srcX, int srcY, int w, int h, int pct);
void gdImageCopyMergeGray(gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
                        int srcX, int srcY, int w, int h, int pct);

/* Stretches or shrinks to fit, as needed. Does NOT attempt
	to average the entire set of source pixels that scale down onto the
	destination pixel. */
void gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH);

/* gd 2.0: stretches or shrinks to fit, as needed. When called with a
	truecolor destination image, this function averages the
	entire set of source pixels that scale down onto the
	destination pixel, taking into account what portion of the
	destination pixel each source pixel represents. This is a
	floating point operation, but this is not a performance issue
	on modern hardware, except for some embedded devices. If the
	destination is a palette image, gdImageCopyResized is
	substituted automatically. */
void gdImageCopyResampled(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH);

gdImagePtr gdImageClone(gdImagePtr src);

void gdImageSetBrush(gdImagePtr im, gdImagePtr brush);
void gdImageSetTile(gdImagePtr im, gdImagePtr tile);
void gdImageSetAntiAliased(gdImagePtr im, int c);
void gdImageSetAntiAliasedDontBlend(gdImagePtr im, int c, int dont_blend);
void gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels);
/* Line thickness (defaults to 1). Affects lines, ellipses,
	rectangles, polygons and so forth. */
void gdImageSetThickness(gdImagePtr im, int thickness);
/* On or off (1 or 0) for all three of these. */
void gdImageInterlace(gdImagePtr im, int interlaceArg);
void gdImageAlphaBlending(gdImagePtr im, int alphaBlendingArg);
void gdImageAntialias(gdImagePtr im, int antialias);
void gdImageSaveAlpha(gdImagePtr im, int saveAlphaArg);


/**
 * Group: Color Quantization
 *
 * Enum: gdPaletteQuantizationMethod
 *
 * Constants:
 *   GD_QUANT_DEFAULT  - GD_QUANT_LIQ if libimagequant is available,
 *                       GD_QUANT_JQUANT otherwise.
 *   GD_QUANT_JQUANT   - libjpeg's old median cut. Fast, but only uses 16-bit
 *                       color.
 *   GD_QUANT_NEUQUANT - NeuQuant - approximation using Kohonen neural network.
 *   GD_QUANT_LIQ      - A combination of algorithms used in libimagequant
 *                       aiming for the highest quality at cost of speed.
 *
 * Note that GD_QUANT_JQUANT does not retain the alpha channel, and
 * GD_QUANT_NEUQUANT does not support dithering.
 *
 * See also:
 *   - <gdImageTrueColorToPaletteSetMethod>
 */
enum gdPaletteQuantizationMethod {
	GD_QUANT_DEFAULT = 0,
	GD_QUANT_JQUANT = 1,
	GD_QUANT_NEUQUANT = 2,
	GD_QUANT_LIQ = 3
};
BGD_DECLARE(gdImagePtr)
gdImageNeuQuant(gdImagePtr im, const int max_color, int sample_factor);

BGD_DECLARE(int)
gdImageTrueColorToPaletteSetMethod(gdImagePtr im, int method, int speed);

BGD_DECLARE(void)
gdImageTrueColorToPaletteSetQuality(gdImagePtr im, int min_quality,
									int max_quality);

enum gdPixelateMode {
	GD_PIXELATE_UPPERLEFT,
	GD_PIXELATE_AVERAGE
};

int gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode);

typedef struct {
	int sub;
	int plus;
	unsigned int num_colors;
	int *colors;
	unsigned int seed;
} gdScatter, *gdScatterPtr;

int gdImageScatter(gdImagePtr im, int sub, int plus);
int gdImageScatterColor(gdImagePtr im, int sub, int plus, int colors[], unsigned int num_colors);
int gdImageScatterEx(gdImagePtr im, gdScatterPtr s);

/* Macros to access information about images. */

/* Returns nonzero if the image is a truecolor image,
	zero for a palette image. */

#define gdImageTrueColor(im) ((im)->trueColor)

#define gdImageSX(im) ((im)->sx)
#define gdImageSY(im) ((im)->sy)
#define gdImageColorsTotal(im) ((im)->colorsTotal)
#define gdImageRed(im, c) ((im)->trueColor ? gdTrueColorGetRed(c) : \
	(im)->red[(c)])
#define gdImageGreen(im, c) ((im)->trueColor ? gdTrueColorGetGreen(c) : \
	(im)->green[(c)])
#define gdImageBlue(im, c) ((im)->trueColor ? gdTrueColorGetBlue(c) : \
	(im)->blue[(c)])
#define gdImageAlpha(im, c) ((im)->trueColor ? gdTrueColorGetAlpha(c) : \
	(im)->alpha[(c)])
#define gdImageGetTransparent(im) ((im)->transparent)
#define gdImageGetInterlaced(im) ((im)->interlace)

/* These macros provide direct access to pixels in
	palette-based and truecolor images, respectively.
	If you use these macros, you must perform your own
	bounds checking. Use of the macro for the correct type
	of image is also your responsibility. */
#define gdImagePalettePixel(im, x, y) (im)->pixels[(y)][(x)]
#define gdImageTrueColorPixel(im, x, y) (im)->tpixels[(y)][(x)]
#define gdImageResolutionX(im) (im)->res_x
#define gdImageResolutionY(im) (im)->res_y

/* I/O Support routines. */

gdIOCtx* gdNewFileCtx(FILE*);
gdIOCtx* gdNewDynamicCtx(int, void*);
gdIOCtx *gdNewDynamicCtxEx(int size, void *data, int freeFlag);
gdIOCtx* gdNewSSCtx(gdSourcePtr in, gdSinkPtr out);
void* gdDPExtractData(struct gdIOCtx* ctx, int *size);

#define GD2_CHUNKSIZE           128
#define GD2_CHUNKSIZE_MIN	64
#define GD2_CHUNKSIZE_MAX       4096

#define GD2_VERS                2
#define GD2_ID                  "gd2"
#define GD2_FMT_RAW             1
#define GD2_FMT_COMPRESSED      2


/* filters section
 *
 * Negate the imag src, white becomes black,
 * The red, green, and blue intensities of an image are negated.
 * White becomes black, yellow becomes blue, etc.
 */
int gdImageNegate(gdImagePtr src);

/* Convert the image src to a grayscale image */
int gdImageGrayScale(gdImagePtr src);

/* Set the brightness level <brightness> for the image src */
int gdImageBrightness(gdImagePtr src, int brightness);

/* Set the contrast level <contrast> for the image <src> */
int gdImageContrast(gdImagePtr src, double contrast);

/* Simply adds or subtracts respectively red, green or blue to a pixel */
int gdImageColor(gdImagePtr src, const int red, const int green, const int blue, const int alpha);

/* Image convolution by a 3x3 custom matrix */
int gdImageConvolution(gdImagePtr src, float ft[3][3], float filter_div, float offset);

int gdImageEdgeDetectQuick(gdImagePtr src);

int gdImageGaussianBlur(gdImagePtr im);

int gdImageSelectiveBlur( gdImagePtr src);

int gdImageEmboss(gdImagePtr im);

int gdImageMeanRemoval(gdImagePtr im);

int gdImageSmooth(gdImagePtr im, float weight);

/* Image comparison definitions */
int gdImageCompare(gdImagePtr im1, gdImagePtr im2);

void gdImageFlipHorizontal(gdImagePtr im);
void gdImageFlipVertical(gdImagePtr im);
void gdImageFlipBoth(gdImagePtr im);

#define GD_FLIP_HORIZONTAL 1
#define GD_FLIP_VERTICAL 2
#define GD_FLIP_BOTH 3

/**
 * Group: Crop
 *
 * Constants: gdCropMode
 *  GD_CROP_DEFAULT - Default crop mode (4 corners or background)
 *  GD_CROP_TRANSPARENT - Crop using the transparent color
 *  GD_CROP_BLACK - Crop black borders
 *  GD_CROP_WHITE - Crop white borders
 *  GD_CROP_SIDES - Crop using colors of the 4 corners
 *
 * See also:
 *  <gdImageAutoCrop>
 **/
enum gdCropMode {
	GD_CROP_DEFAULT = 0,
	GD_CROP_TRANSPARENT,
	GD_CROP_BLACK,
	GD_CROP_WHITE,
	GD_CROP_SIDES,
	GD_CROP_THRESHOLD
};

gdImagePtr gdImageCrop(gdImagePtr src, const gdRectPtr crop);
gdImagePtr gdImageCropAuto(gdImagePtr im, const unsigned int mode);
gdImagePtr gdImageCropThreshold(gdImagePtr im, const unsigned int color, const float threshold);

BGD_DECLARE(int)
gdImageSetInterpolationMethod(gdImagePtr im, gdInterpolationMethod id);
BGD_DECLARE(gdInterpolationMethod) gdImageGetInterpolationMethod(gdImagePtr im);

BGD_DECLARE(gdImagePtr)
gdImageScale(const gdImagePtr src, const unsigned int new_width,
			 const unsigned int new_height);

BGD_DECLARE(gdImagePtr)
gdImageRotateInterpolated(const gdImagePtr src, const float angle, int bgcolor);

typedef enum {
	GD_AFFINE_TRANSLATE = 0,
	GD_AFFINE_SCALE,
	GD_AFFINE_ROTATE,
	GD_AFFINE_SHEAR_HORIZONTAL,
	GD_AFFINE_SHEAR_VERTICAL
} gdAffineStandardMatrix;

BGD_DECLARE(int)
gdAffineApplyToPointF(gdPointFPtr dst, const gdPointFPtr src,
					  const double affine[6]);
BGD_DECLARE(int) gdAffineInvert(double dst[6], const double src[6]);
BGD_DECLARE(int)
gdAffineFlip(double dst_affine[6], const double src_affine[6], const int flip_h,
			 const int flip_v);
BGD_DECLARE(int)
gdAffineConcat(double dst[6], const double m1[6], const double m2[6]);

BGD_DECLARE(int) gdAffineIdentity(double dst[6]);
BGD_DECLARE(int)
gdAffineScale(double dst[6], const double scale_x, const double scale_y);
BGD_DECLARE(int) gdAffineRotate(double dst[6], const double angle);
BGD_DECLARE(int) gdAffineShearHorizontal(double dst[6], const double angle);
BGD_DECLARE(int) gdAffineShearVertical(double dst[6], const double angle);
BGD_DECLARE(int)
gdAffineTranslate(double dst[6], const double offset_x, const double offset_y);
BGD_DECLARE(double) gdAffineExpansion(const double src[6]);
BGD_DECLARE(int) gdAffineRectilinear(const double src[6]);
BGD_DECLARE(int)
gdAffineEqual(const double matrix1[6], const double matrix2[6]);
BGD_DECLARE(int)
gdTransformAffineGetImage(gdImagePtr *dst, const gdImagePtr src,
						  gdRectPtr src_area, const double affine[6]);
BGD_DECLARE(int)
gdTransformAffineCopy(gdImagePtr dst, int dst_x, int dst_y,
					  const gdImagePtr src, gdRectPtr src_region,
					  const double affine[6]);
/*
gdTransformAffineCopy(gdImagePtr dst, int x0, int y0, int x1, int y1,
			const gdImagePtr src, int src_width, int src_height,
			const double affine[6]);
*/
BGD_DECLARE(int)
gdTransformAffineBoundingBox(gdRectPtr src, const double affine[6],
							 gdRectPtr bbox);

/**
 * Group: Image Comparison
 *
 * Constants:
 *   GD_CMP_IMAGE       - Actual image IS different
 *   GD_CMP_NUM_COLORS  - Number of colors in pallette differ
 *   GD_CMP_COLOR       - Image colors differ
 *   GD_CMP_SIZE_X      - Image width differs
 *   GD_CMP_SIZE_Y      - Image heights differ
 *   GD_CMP_TRANSPARENT - Transparent color differs
 *   GD_CMP_BACKGROUND  - Background color differs
 *   GD_CMP_INTERLACE   - Interlaced setting differs
 *   GD_CMP_TRUECOLOR   - Truecolor vs palette differs
 *
 * See also:
 *   - <gdImageCompare>
 */
#define GD_CMP_IMAGE 1
#define GD_CMP_NUM_COLORS 2
#define GD_CMP_COLOR 4
#define GD_CMP_SIZE_X 8
#define GD_CMP_SIZE_Y 16
#define GD_CMP_TRANSPARENT 32
#define GD_CMP_BACKGROUND 64
#define GD_CMP_INTERLACE 128
#define GD_CMP_TRUECOLOR 256

/* resolution affects ttf font rendering, particularly hinting */
#define GD_RESOLUTION           96      /* pixels per inch */

/* Version information functions */
BGD_DECLARE(int) gdMajorVersion(void);
BGD_DECLARE(int) gdMinorVersion(void);
BGD_DECLARE(int) gdReleaseVersion(void);
BGD_DECLARE(const char *) gdExtraVersion(void);
BGD_DECLARE(const char *) gdVersionString(void);

/* newfangled special effects */
#include "gdfx.h"

#ifdef __cplusplus
}
#endif

/* 2.0.12: this now checks the clipping rectangle */
#define gdImageBoundsSafe(im, x, y) (!((((y) < (im)->cy1) || ((y) > (im)->cy2)) || (((x) < (im)->cx1) || ((x) > (im)->cx2))))

#endif /* GD_H */
