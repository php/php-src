#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include "gd_intern.h"

#ifdef HAVE_LIBAVIF
#include <avif/avif.h>

/*
	Define defaults for encoding images:
		CHROMA_SUBSAMPLING_DEFAULT: 4:2:0 is commonly used for Chroma subsampling.
		CHROMA_SUBAMPLING_HIGH_QUALITY: Use 4:4:4, or no subsampling, when a sufficient high quality is requested.
		SUBAMPLING_HIGH_QUALITY_THRESHOLD: At or above this value, use CHROMA_SUBAMPLING_HIGH_QUALITY
		QUANTIZER_DEFAULT:
			We need more testing to really know what quantizer settings are optimal,
			but teams at Google have been using maximum=30 as a starting point.
		QUALITY_DEFAULT: following gd conventions, -1 indicates the default.
		SPEED_DEFAULT:
			AVIF_SPEED_DEFAULT is simply the default encoding speed of the AV1 codec.
			This could be as slow as 0. So we use 6, which is currently considered to be a fine default.
*/

#define CHROMA_SUBSAMPLING_DEFAULT AVIF_PIXEL_FORMAT_YUV420
#define CHROMA_SUBAMPLING_HIGH_QUALITY AVIF_PIXEL_FORMAT_YUV444
#define HIGH_QUALITY_SUBSAMPLING_THRESHOLD 90
#define QUANTIZER_DEFAULT 30
#define QUALITY_DEFAULT -1
#define SPEED_DEFAULT 6

// This initial size for the gdIOCtx is standard among GD image conversion functions.
#define NEW_DYNAMIC_CTX_SIZE 2048

// Our quality param ranges from 0 to 100.
// To calculate quality, we convert from AVIF's quantizer scale, which runs from 63 to 0.
#define MAX_QUALITY 100

// These constants are for computing the number of tiles and threads to use during encoding.
// Maximum threads are from libavif/contrib/gkd-pixbuf/loader.c.
#define MIN_TILE_AREA (512 * 512)
#define MAX_TILES 8
#define MAX_THREADS 64

/*** Macros ***/

/*
	From gd_png.c:
		convert the 7-bit alpha channel to an 8-bit alpha channel.
		We do a little bit-flipping magic, repeating the MSB
		as the LSB, to ensure that 0 maps to 0 and
		127 maps to 255. We also have to invert to match
		PNG's convention in which 255 is opaque.
*/
#define alpha7BitTo8Bit(alpha7Bit) \
	(alpha7Bit == 127 ? \
				0 : \
				255 - ((alpha7Bit << 1) + (alpha7Bit >> 6)))

#define alpha8BitTo7Bit(alpha8Bit) (gdAlphaMax - (alpha8Bit >> 1))


/*** Helper functions ***/

/* Convert the quality param we expose to the quantity params used by libavif.
	 The *Quantizer* params values can range from 0 to 63, with 0 = highest quality and 63 = worst.
	 We make the scale 0-100, and we reverse this, so that 0 = worst quality and 100 = highest.

	 Values below 0 are set to 0, and values below MAX_QUALITY are set to MAX_QUALITY.
*/
static int quality2Quantizer(int quality) {
	int clampedQuality = CLAMP(quality, 0, MAX_QUALITY);

	float scaleFactor = (float) AVIF_QUANTIZER_WORST_QUALITY / (float) MAX_QUALITY;

	return round(scaleFactor * (MAX_QUALITY - clampedQuality));
}

/*
	 As of February 2021, this algorithm reflects the latest research on how many tiles
	 and threads to include for a given image size.
	 This is subject to change as research continues.

	 Returns false if there was an error, true if all was well.
 */
static avifBool setEncoderTilesAndThreads(avifEncoder *encoder, avifRGBImage *rgb) {
	int imageArea, tiles, tilesLog2, encoderTiles;

	// _gdImageAvifCtx(), the calling function, checks this operation for overflow
	imageArea = rgb->width * rgb->height;

	tiles = (int) ceil((double) imageArea / MIN_TILE_AREA);
	tiles = MIN(tiles, MAX_TILES);
	tiles = MIN(tiles, MAX_THREADS);

	// The number of tiles in any dimension will always be a power of 2. We can only specify log(2)tiles.

	tilesLog2 = floor(log2(tiles));

	// If the image's width is greater than the height, use more tile columns
	// than tile rows to make the tile size close to a square.

	if (rgb->width >= rgb->height) {
		encoder->tileRowsLog2 = tilesLog2 / 2;
		encoder->tileColsLog2 = tilesLog2 - encoder->tileRowsLog2;
	} else {
		encoder->tileColsLog2 = tilesLog2 / 2;
		encoder->tileRowsLog2 = tilesLog2 - encoder->tileColsLog2;
	}

	// It's good to have one thread per tile.
	encoderTiles = (1 << encoder->tileRowsLog2) * (1 << encoder->tileColsLog2);
	encoder->maxThreads = encoderTiles;

	return AVIF_TRUE;
}

/*
	 We can handle AVIF images whose color profile is sRGB, or whose color profile isn't set.
*/
static avifBool isAvifSrgbImage(avifImage *avifIm) {
	return
		(avifIm->colorPrimaries == AVIF_COLOR_PRIMARIES_BT709 ||
			avifIm->colorPrimaries == AVIF_COLOR_PRIMARIES_UNSPECIFIED) &&
		(avifIm->transferCharacteristics == AVIF_TRANSFER_CHARACTERISTICS_SRGB ||
			avifIm->transferCharacteristics == AVIF_TRANSFER_CHARACTERISTICS_UNSPECIFIED)
	;
}

/*
	 Check the result from an Avif function to see if it's an error.
	 If so, decode the error and output it, and return true.
	 Otherwise, return false.
*/
static avifBool isAvifError(avifResult result, const char *msg) {
	if (result != AVIF_RESULT_OK) {
		gd_error("avif error - %s: %s\n", msg, avifResultToString(result));
		return AVIF_TRUE;
	}

	return AVIF_FALSE;
}


typedef struct avifIOCtxReader {
	avifIO io; // this must be the first member for easy casting to avifIO*
	avifROData rodata;
} avifIOCtxReader;

/*
	<readfromCtx> implements the avifIOReadFunc interface by calling the relevant functions
	in the gdIOCtx. Our logic is inspired by avifIOMemoryReaderRead() and avifIOFileReaderRead().
	We don't know whether we're reading from a file or from memory. We don't have to know,
	since we rely on the helper functions in the gdIOCtx.
	We assume we've stashed the gdIOCtx in io->data, as we do in createAvifIOFromCtx().

	We ignore readFlags, just as the avifIO*ReaderRead() functions do.

	If there's a problem, this returns an avifResult error.
	If things go well, return AVIF_RESULT_OK.
	Of course these AVIF codes shouldn't be returned by any top-level GD function.
*/
static avifResult readFromCtx(avifIO *io, uint32_t readFlags, uint64_t offset, size_t size, avifROData *out)
{
	gdIOCtx *ctx = (gdIOCtx *) io->data;
	avifIOCtxReader *reader = (avifIOCtxReader *) io;

	// readFlags is unsupported
	if (readFlags != 0) {
		return AVIF_RESULT_IO_ERROR;
	}

	// TODO: if we set sizeHint, this will be more efficient.

	if (offset > INT_MAX || size > INT_MAX)
		return AVIF_RESULT_IO_ERROR;

	// Try to seek offset bytes forward. If we pass the end of the buffer, throw an error.
	if (!ctx->seek(ctx, (int) offset))
		return AVIF_RESULT_IO_ERROR;

	if (size > reader->rodata.size) {
		reader->rodata.data = gdRealloc((void *) reader->rodata.data, size);
		reader->rodata.size = size;
	}
	if (!reader->rodata.data) {
		gd_error("avif error - couldn't allocate memory");
		return AVIF_RESULT_UNKNOWN_ERROR;
	}

	// Read the number of bytes requested.
	// If getBuf() returns a negative value, that means there was an error.
	int charsRead = ctx->getBuf(ctx, (void *) reader->rodata.data, (int) size);
	if (charsRead < 0) {
		return AVIF_RESULT_IO_ERROR;
	}

	out->data = reader->rodata.data;
	out->size = charsRead;
	return AVIF_RESULT_OK;
}

// avif.h says this is optional, but it seemed easy to implement.
static void destroyAvifIO(struct avifIO *io) {
	avifIOCtxReader *reader = (avifIOCtxReader *) io;
	if (reader->rodata.data != NULL) {
		gdFree((void *) reader->rodata.data);
	}
	gdFree(reader);
}

/* Set up an avifIO object.
	 The functions in the gdIOCtx struct may point either to a file or a memory buffer.
	 To us, that's immaterial.
	 Our task is simply to assign avifIO functions to the proper functions from gdIOCtx.
	 The destroy function needs to destroy the avifIO object and anything else it uses.

	 Returns NULL if memory for the object can't be allocated.
*/

// TODO: can we get sizeHint somehow?
static avifIO *createAvifIOFromCtx(gdIOCtx *ctx) {
	struct avifIOCtxReader *reader;

	reader = gdMalloc(sizeof(*reader));
	if (reader == NULL)
		return NULL;

	// TODO: setting persistent=FALSE is safe, but it's less efficient. Is it necessary?
	reader->io.persistent = AVIF_FALSE;
	reader->io.read = readFromCtx;
	reader->io.write = NULL; // this function is currently unused; see avif.h
	reader->io.destroy = destroyAvifIO;
	reader->io.sizeHint = 0; // sadly, we don't get this information from the gdIOCtx.
	reader->io.data = ctx;
	reader->rodata.data = NULL;
	reader->rodata.size = 0;

	return (avifIO *) reader;
}


/*** Decoding functions ***/

/*
	Function: gdImageCreateFromAvif

		<gdImageCreateFromAvif> is called to load truecolor images from
		AVIF format files. Invoke <gdImageCreateFromAvif> with an
		already opened pointer to a file containing the desired
		image. <gdImageCreateFromAvif> returns a <gdImagePtr> to the new
		truecolor image, or NULL if unable to load the image (most often
		because the file is corrupt or does not contain a AVIF
		image). <gdImageCreateFromAvif> does not close the file.

		This function creates a gdIOCtx struct from the file pointer it's passed.
		And then it relies on <gdImageCreateFromAvifCtx> to do the real decoding work.
		If the file contains an image sequence, we simply read the first one, discarding the rest.

	Variants:

		<gdImageCreateFromAvifPtr> creates an image from AVIF data
		already in memory.

		<gdImageCreateFromAvifCtx> reads data from the function
		pointers in a <gdIOCtx> structure.

	Parameters:

		infile - pointer to the input file

	Returns:

		A pointer to the new truecolor image.	This will need to be
		destroyed with <gdImageDestroy> once it is no longer needed.

		On error, returns 0.
*/
gdImagePtr gdImageCreateFromAvif(FILE *infile)
{
	gdImagePtr im;
	gdIOCtx *ctx = gdNewFileCtx(infile);

	if (!ctx)
		return NULL;

	im = gdImageCreateFromAvifCtx(ctx);
	ctx->gd_free(ctx);

	return im;
}

/*
	Function: gdImageCreateFromAvifPtr

		See <gdImageCreateFromAvif>.

	Parameters:

		size						- size of Avif data in bytes.
		data						- pointer to Avif data.
*/
gdImagePtr gdImageCreateFromAvifPtr(int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *ctx = gdNewDynamicCtxEx(size, data, 0);

	if (!ctx)
		return 0;

	im = gdImageCreateFromAvifCtx(ctx);
	ctx->gd_free(ctx);

	return im;
}

/*
	Function: gdImageCreateFromAvifCtx

		See <gdImageCreateFromAvif>.

		Additional details: the AVIF library comes with functions to create an IO object from
		a file and from a memory pointer. Of course, it doesn't have a way to create an IO object
		from a gdIOCtx. So, here, we use our own helper function, <createAvifIOfromCtx>.

		Otherwise, we create the image by calling AVIF library functions in order:
		* avifDecoderCreate(), to create the decoder
		* avifDecoderSetIO(), to tell libavif how to read from our data structure
		* avifDecoderParse(), to parse the image
		* avifDecoderNextImage(), to read the first image from the decoder
		* avifRGBImageSetDefaults(), to create the avifRGBImage
		* avifRGBImageAllocatePixels(), to allocate memory for the pixels
		* avifImageYUVToRGB(), to convert YUV to RGB

		Finally, we create a new gd image and copy over the pixel data.

	Parameters:

		ctx							- a gdIOCtx struct
*/
gdImagePtr gdImageCreateFromAvifCtx (gdIOCtx *ctx)
{
	uint32_t x, y;
	gdImage *im = NULL;
	avifResult result;
	avifIO *io;
	avifDecoder *decoder;
	avifRGBImage rgb;

	// this lets us know that memory hasn't been allocated yet for the pixels
	rgb.pixels = NULL;

	decoder = avifDecoderCreate();

	// Check if libavif version is >= 0.9.1
	// If so, allow the PixelInformationProperty ('pixi') to be missing in AV1 image
	// items. libheif v1.11.0 or older does not add the 'pixi' item property to
	// AV1 image items. (This issue has been corrected in libheif v1.12.0.)

#if AVIF_VERSION >= 90100
	decoder->strictFlags &= ~AVIF_STRICT_PIXI_REQUIRED;
#endif

	io = createAvifIOFromCtx(ctx);
	if (!io) {
		gd_error("avif error - Could not allocate memory");
		goto cleanup;
	}

	avifDecoderSetIO(decoder, io);

	result = avifDecoderParse(decoder);
	if (isAvifError(result, "Could not parse image"))
		goto cleanup;

	// Note again that, for an image sequence, we read only the first image, ignoring the rest.
	result = avifDecoderNextImage(decoder);
	if (isAvifError(result, "Could not decode image"))
		goto cleanup;

	if (!isAvifSrgbImage(decoder->image))
		gd_error_ex(GD_NOTICE, "Image's color profile is not sRGB");

	// Set up the avifRGBImage, and convert it from YUV to an 8-bit RGB image.
	// (While AVIF image pixel depth can be 8, 10, or 12 bits, GD truecolor images are 8-bit.)
	avifRGBImageSetDefaults(&rgb, decoder->image);
	rgb.depth = 8;
	avifRGBImageAllocatePixels(&rgb);

	result = avifImageYUVToRGB(decoder->image, &rgb);
	if (isAvifError(result, "Conversion from YUV to RGB failed"))
		goto cleanup;

	im = gdImageCreateTrueColor(decoder->image->width, decoder->image->height);
	if (!im) {
		gd_error("avif error - Could not create GD truecolor image");
		goto cleanup;
	}

	im->saveAlphaFlag = 1;

	// Read the pixels from the AVIF image and copy them into the GD image.

	uint8_t *p = rgb.pixels;

	for (y = 0; y < decoder->image->height; y++) {
		for (x = 0; x < decoder->image->width; x++) {
			uint8_t r = *(p++);
			uint8_t g = *(p++);
			uint8_t b = *(p++);
			uint8_t a = alpha8BitTo7Bit(*(p++));
			im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, a);
		}
	}

cleanup:
	// if io has been allocated, this frees it
	avifDecoderDestroy(decoder);

	if (rgb.pixels)
		avifRGBImageFreePixels(&rgb);

	return im;
}


/*** Encoding functions ***/

/*
	Function: gdImageAvifEx

		<gdImageAvifEx> outputs the specified image to the specified file in
		AVIF format. The file must be open for writing. Under MSDOS and
		all versions of Windows, it is important to use "wb" as opposed to
		simply "w" as the mode when opening the file, and under Unix there
		is no penalty for doing so. <gdImageAvifEx> does not close the file;
		your code must do so.

	Variants:

		<gdImageAvifEx> writes the image to a file, encoding with the default quality and speed.

		<gdImageAvifPtrEx> stores the image in RAM.

		<gdImageAvifPtr> stores the image in RAM, encoding with the default quality and speed.

		<gdImageAvifCtx> stores the image using a <gdIOCtx> struct.

	Parameters:

		im			- The image to save.
		outFile - The FILE pointer to write to.
		quality - Compression quality (0-100). 0 is lowest-quality, 100 is highest.
		speed	  - The speed of compression (0-10). 0 is slowest, 10 is fastest.

	Notes on parameters:
		quality - If quality = -1, we use a default quality as defined in QUALITY_DEFAULT.
			For information on how we convert this quality to libavif's quantity param, see <quality2Quantizer>.

		speed - At slower speeds, encoding may be quite slow. Use judiciously.

		Qualities or speeds that are lower than the minimum value get clamped to the minimum value,
		and qualities or speeds that are lower than the maximum value get clamped to the maxmum value.
		Note that AVIF_SPEED_DEFAULT is -1. If we ever set SPEED_DEFAULT = AVIF_SPEED_DEFAULT,
		we'd want to add a conditional to ensure that value doesn't get clamped.


	Returns:

		* for <gdImageAvifEx>, <gdImageAvif>, and <gdImageAvifCtx>, nothing.
		* for <gdImageAvifPtrEx> and <gdImageAvifPtr>, a pointer to the image in memory.
*/

/*
	 If we're passed the QUALITY_DEFAULT of -1, set the quantizer params to QUANTIZER_DEFAULT.
*/
void gdImageAvifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, int speed)
{
	avifResult result;
	avifRGBImage rgb;
	avifRWData avifOutput = AVIF_DATA_EMPTY;
	avifBool lossless = quality == 100;
	avifEncoder *encoder = NULL;

	uint32_t val;
	uint8_t *p;
	uint32_t x, y;

	if (im == NULL)
		return;

	if (!gdImageTrueColor(im)) {
		gd_error("avif error - avif doesn't support palette images");
		return;
	}

	if (!gdImageSX(im) || !gdImageSY(im)) {
		gd_error("avif error - image dimensions must not be zero");
		return;
	}

	if (overflow2(gdImageSX(im), gdImageSY(im))) {
		gd_error("avif error - image dimensions are too large");
		return;
	}

	speed = CLAMP(speed, AVIF_SPEED_SLOWEST, AVIF_SPEED_FASTEST);

	avifPixelFormat subsampling = quality >= HIGH_QUALITY_SUBSAMPLING_THRESHOLD ?
		CHROMA_SUBAMPLING_HIGH_QUALITY : CHROMA_SUBSAMPLING_DEFAULT;

	// Create the AVIF image.
	// Set the ICC to sRGB, as that's what gd supports right now.
	// Note that MATRIX_COEFFICIENTS_IDENTITY enables lossless conversion from RGB to YUV.

	avifImage *avifIm = avifImageCreate(gdImageSX(im), gdImageSY(im), 8, subsampling);

	avifIm->colorPrimaries = AVIF_COLOR_PRIMARIES_BT709;
	avifIm->transferCharacteristics = AVIF_TRANSFER_CHARACTERISTICS_SRGB;
	avifIm->matrixCoefficients = lossless ? AVIF_MATRIX_COEFFICIENTS_IDENTITY : AVIF_MATRIX_COEFFICIENTS_BT709;

	avifRGBImageSetDefaults(&rgb, avifIm);
	// this allocates memory, and sets rgb.rowBytes and rgb.pixels.
	avifRGBImageAllocatePixels(&rgb);

	// Parse RGB data from the GD image, and copy it into the AVIF RGB image.
	// Convert 7-bit GD alpha channel values to 8-bit AVIF values.

	p = rgb.pixels;
	for (y = 0; y < rgb.height; y++) {
		for (x = 0; x < rgb.width; x++) {
			val = im->tpixels[y][x];

			*(p++) = gdTrueColorGetRed(val);
			*(p++) = gdTrueColorGetGreen(val);
			*(p++) = gdTrueColorGetBlue(val);
			*(p++) = alpha7BitTo8Bit(gdTrueColorGetAlpha(val));
		}
	}

	// Convert the RGB image to YUV.

	result = avifImageRGBToYUV(avifIm, &rgb);
	if (isAvifError(result, "Could not convert image to YUV"))
		goto cleanup;

	// Encode the image in AVIF format.

	encoder = avifEncoderCreate();
	int quantizerQuality = quality == QUALITY_DEFAULT ?
		QUANTIZER_DEFAULT : quality2Quantizer(quality);

	encoder->minQuantizer = quantizerQuality;
	encoder->maxQuantizer = quantizerQuality;
	encoder->minQuantizerAlpha = quantizerQuality;
	encoder->maxQuantizerAlpha = quantizerQuality;
	encoder->speed = speed;

	if (!setEncoderTilesAndThreads(encoder, &rgb))
		goto cleanup;

	//TODO: is there a reason to use timeSscales != 1?
	result = avifEncoderAddImage(encoder, avifIm, 1, AVIF_ADD_IMAGE_FLAG_SINGLE);
	if (isAvifError(result, "Could not encode image"))
		goto cleanup;

	result = avifEncoderFinish(encoder, &avifOutput);
	if (isAvifError(result, "Could not finish encoding"))
		goto cleanup;

	// Write the AVIF image bytes to the GD ctx.

	gdPutBuf(avifOutput.data, avifOutput.size, outfile);

cleanup:
	if (rgb.pixels)
		avifRGBImageFreePixels(&rgb);

	if (encoder)
		avifEncoderDestroy(encoder);

	if (avifOutput.data)
		avifRWDataFree(&avifOutput);

	if (avifIm)
		avifImageDestroy(avifIm);
}

void gdImageAvifEx(gdImagePtr im, FILE *outFile, int quality, int speed)
{
	gdIOCtx *out = gdNewFileCtx(outFile);

	if (out != NULL) {
		gdImageAvifCtx(im, out, quality, speed);
		out->gd_free(out);
	}
}

void gdImageAvif(gdImagePtr im, FILE *outFile)
{
	gdImageAvifEx(im, outFile, QUALITY_DEFAULT, SPEED_DEFAULT);
}

void * gdImageAvifPtrEx(gdImagePtr im, int *size, int quality, int speed)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(NEW_DYNAMIC_CTX_SIZE, NULL);

	if (out == NULL) {
		return NULL;
	}

	gdImageAvifCtx(im, out, quality, speed);
	rv = gdDPExtractData(out, size);

	out->gd_free(out);
	return rv;
}

void * gdImageAvifPtr(gdImagePtr im, int *size)
{
	return gdImageAvifPtrEx(im, size, QUALITY_DEFAULT, AVIF_SPEED_DEFAULT);
}

#endif /* HAVE_LIBAVIF */
