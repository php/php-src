/*
 * gd_jpeg.c: Read and write JPEG (JFIF) format image files using the
 * gd graphics library (http://www.boutell.com/gd/).
 *
 * This software is based in part on the work of the Independent JPEG
 * Group.  For more information on the IJG JPEG software (and JPEG
 * documentation, etc.), see ftp://ftp.uu.net/graphics/jpeg/.
 *
 * NOTE: IJG 12-bit JSAMPLE (BITS_IN_JSAMPLE == 12) mode is not
 * supported at all on read in gd 2.0, and is not supported on write
 * except for palette images, which is sort of pointless (TBB). Even that
 * has never been tested according to DB.
 *
 * Copyright 2000 Doug Becker, mailto:thebeckers@home.com
 *
 * Modification 4/18/00 TBB: JPEG_DEBUG rather than just DEBUG,
 * so VC++ builds don't spew to standard output, causing
 * major CGI brain damage
 *
 * 2.0.10: more efficient gdImageCreateFromJpegCtx, thanks to
 * Christian Aberger
 */

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <limits.h>
#include <string.h>

#include "gd.h"
/* TBB: move this up so include files are not brought in */
/* JCE: arrange HAVE_LIBJPEG so that it can be set in gd.h */
#ifdef HAVE_LIBJPEG
#include "gdhelpers.h"
#undef HAVE_STDLIB_H

/* 1.8.1: remove dependency on jinclude.h */
#include "jpeglib.h"
#include "jerror.h"

static const char *const GD_JPEG_VERSION = "1.0";

typedef struct _jmpbuf_wrapper
{
	jmp_buf jmpbuf;
	int ignore_warning;
} jmpbuf_wrapper;

static long php_jpeg_emit_message(j_common_ptr jpeg_info, int level)
{
	char message[JMSG_LENGTH_MAX];
	jmpbuf_wrapper *jmpbufw;
	int ignore_warning = 0;

	jmpbufw = (jmpbuf_wrapper *) jpeg_info->client_data;

	if (jmpbufw != 0) {
		ignore_warning = jmpbufw->ignore_warning;
	}

	(jpeg_info->err->format_message)(jpeg_info,message);

	/* It is a warning message */
	if (level < 0) {
		/* display only the 1st warning, as would do a default libjpeg
		 * unless strace_level >= 3
		 */
		if ((jpeg_info->err->num_warnings == 0) || (jpeg_info->err->trace_level >= 3)) {
			php_gd_error_ex(ignore_warning ? E_NOTICE : E_WARNING, "gd-jpeg, libjpeg: recoverable error: %s\n", message);
		}

		jpeg_info->err->num_warnings++;
	} else {
		/* strace msg, Show it if trace_level >= level. */
		if (jpeg_info->err->trace_level >= level) {
			php_gd_error_ex(E_NOTICE, "gd-jpeg, libjpeg: strace message: %s\n", message);
		}
	}
	return 1;
}



/* Called by the IJG JPEG library upon encountering a fatal error */
static void fatal_jpeg_error (j_common_ptr cinfo)
{
	jmpbuf_wrapper *jmpbufw;

	php_gd_error("gd-jpeg: JPEG library reports unrecoverable error: ");
	(*cinfo->err->output_message) (cinfo);

	jmpbufw = (jmpbuf_wrapper *) cinfo->client_data;
	jpeg_destroy (cinfo);

	if (jmpbufw != 0) {
		longjmp (jmpbufw->jmpbuf, 1);
		php_gd_error_ex(E_ERROR, "gd-jpeg: EXTREMELY fatal error: longjmp returned control; terminating");
	} else {
		php_gd_error_ex(E_ERROR, "gd-jpeg: EXTREMELY fatal error: jmpbuf unrecoverable; terminating");
	}

	exit (99);
}

int gdJpegGetVersionInt()
{
	return JPEG_LIB_VERSION;
}

const char * gdJpegGetVersionString()
{
	switch(JPEG_LIB_VERSION) {
		case 62:
			return "6b";
			break;

		case 70:
			return "7";
			break;

		case 80:
			return "8";
			break;

		case 90:
			return "9 compatible";
			break;

		default:
			return "unknown";
	}
}


/*
 * Write IM to OUTFILE as a JFIF-formatted JPEG image, using quality
 * QUALITY.  If QUALITY is in the range 0-100, increasing values
 * represent higher quality but also larger image size.  If QUALITY is
 * negative, the IJG JPEG library's default quality is used (which
 * should be near optimal for many applications).  See the IJG JPEG
 * library documentation for more details.
 */

void gdImageJpeg (gdImagePtr im, FILE * outFile, int quality)
{
	gdIOCtx *out = gdNewFileCtx (outFile);
	gdImageJpegCtx (im, out, quality);
	out->gd_free (out);
}

void *gdImageJpegPtr (gdImagePtr im, int *size, int quality)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
	gdImageJpegCtx (im, out, quality);
	rv = gdDPExtractData (out, size);
	out->gd_free (out);

	return rv;
}

void jpeg_gdIOCtx_dest (j_compress_ptr cinfo, gdIOCtx * outfile);

void gdImageJpegCtx (gdImagePtr im, gdIOCtx * outfile, int quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int i, j, jidx;
	/* volatile so we can gdFree it on return from longjmp */
	volatile JSAMPROW row = 0;
	JSAMPROW rowptr[1];
	jmpbuf_wrapper jmpbufw;
	JDIMENSION nlines;
	char comment[255];

	memset (&cinfo, 0, sizeof (cinfo));
	memset (&jerr, 0, sizeof (jerr));

	cinfo.err = jpeg_std_error (&jerr);
	cinfo.client_data = &jmpbufw;
	if (setjmp (jmpbufw.jmpbuf) != 0) {
		/* we're here courtesy of longjmp */
		if (row) {
			gdFree (row);
		}
		return;
	}

	cinfo.err->error_exit = fatal_jpeg_error;

	jpeg_create_compress (&cinfo);

	cinfo.image_width = im->sx;
	cinfo.image_height = im->sy;
	cinfo.input_components = 3;	/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;	/* colorspace of input image */
	jpeg_set_defaults (&cinfo);
	if (quality >= 0) {
		jpeg_set_quality (&cinfo, quality, TRUE);
	}

	/* If user requests interlace, translate that to progressive JPEG */
	if (gdImageGetInterlaced (im)) {
		jpeg_simple_progression (&cinfo);
	}

	jpeg_gdIOCtx_dest (&cinfo, outfile);

	row = (JSAMPROW) safe_emalloc(cinfo.image_width * cinfo.input_components, sizeof(JSAMPLE), 0);
	memset(row, 0, cinfo.image_width * cinfo.input_components * sizeof(JSAMPLE));
	rowptr[0] = row;

	jpeg_start_compress (&cinfo, TRUE);

	if (quality >= 0) {
		snprintf(comment, sizeof(comment)-1, "CREATOR: gd-jpeg v%s (using IJG JPEG v%d), quality = %d\n", GD_JPEG_VERSION, JPEG_LIB_VERSION, quality);
	} else {
		snprintf(comment, sizeof(comment)-1, "CREATOR: gd-jpeg v%s (using IJG JPEG v%d), default quality\n", GD_JPEG_VERSION, JPEG_LIB_VERSION);
	}
	jpeg_write_marker (&cinfo, JPEG_COM, (unsigned char *) comment, (unsigned int) strlen (comment));
	if (im->trueColor) {

#if BITS_IN_JSAMPLE == 12
		php_gd_error("gd-jpeg: error: jpeg library was compiled for 12-bit precision. This is mostly useless, because JPEGs on the web are 8-bit and such versions of the jpeg library won't read or write them. GD doesn't support these unusual images. Edit your jmorecfg.h file to specify the correct precision and completely 'make clean' and 'make install' libjpeg again. Sorry");
		goto error;
#endif /* BITS_IN_JSAMPLE == 12 */

		for (i = 0; i < im->sy; i++) {
			for (jidx = 0, j = 0; j < im->sx; j++) {
				int val = im->tpixels[i][j];

				row[jidx++] = gdTrueColorGetRed (val);
				row[jidx++] = gdTrueColorGetGreen (val);
				row[jidx++] = gdTrueColorGetBlue (val);
			}

			nlines = jpeg_write_scanlines (&cinfo, rowptr, 1);
			if (nlines != 1) {
				php_gd_error_ex(E_WARNING, "gd_jpeg: warning: jpeg_write_scanlines returns %u -- expected 1", nlines);
			}
		}
	} else {
		for (i = 0; i < im->sy; i++) {
			for (jidx = 0, j = 0; j < im->sx; j++) {
				int idx = im->pixels[i][j];

				/* NB: Although gd RGB values are ints, their max value is
				 * 255 (see the documentation for gdImageColorAllocate())
				 * -- perfect for 8-bit JPEG encoding (which is the norm)
				 */
#if BITS_IN_JSAMPLE == 8
				row[jidx++] = im->red[idx];
				row[jidx++] = im->green[idx];
				row[jidx++] = im->blue[idx];
#elif BITS_IN_JSAMPLE == 12
				row[jidx++] = im->red[idx] << 4;
				row[jidx++] = im->green[idx] << 4;
				row[jidx++] = im->blue[idx] << 4;
#else
#error IJG JPEG library BITS_IN_JSAMPLE value must be 8 or 12
#endif
			}

			nlines = jpeg_write_scanlines (&cinfo, rowptr, 1);
			if (nlines != 1) {
				php_gd_error_ex(E_WARNING, "gd_jpeg: warning: jpeg_write_scanlines returns %u -- expected 1", nlines);
			}
		}
	}

	jpeg_finish_compress (&cinfo);
	jpeg_destroy_compress (&cinfo);
	gdFree (row);
}

gdImagePtr gdImageCreateFromJpeg (FILE * inFile)
{
	return gdImageCreateFromJpegEx(inFile, 1);
}

gdImagePtr gdImageCreateFromJpegEx (FILE * inFile, int ignore_warning)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	im = gdImageCreateFromJpegCtxEx(in, ignore_warning);
	in->gd_free (in);

	return im;
}

gdImagePtr gdImageCreateFromJpegPtr (int size, void *data)
{
	return gdImageCreateFromJpegPtrEx(size, data, 1);
}

gdImagePtr gdImageCreateFromJpegPtrEx (int size, void *data, int ignore_warning)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	im = gdImageCreateFromJpegCtxEx(in, ignore_warning);
	in->gd_free(in);

	return im;
}

void jpeg_gdIOCtx_src (j_decompress_ptr cinfo, gdIOCtx * infile);

static int CMYKToRGB(int c, int m, int y, int k, int inverted);


/*
 * Create a gd-format image from the JPEG-format INFILE.  Returns the
 * image, or NULL upon error.
 */
gdImagePtr gdImageCreateFromJpegCtx (gdIOCtx * infile)
{
	return gdImageCreateFromJpegCtxEx(infile, 1);
}

gdImagePtr gdImageCreateFromJpegCtxEx (gdIOCtx * infile, int ignore_warning)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	jmpbuf_wrapper jmpbufw;
	/* volatile so we can gdFree them after longjmp */
	volatile JSAMPROW row = 0;
	volatile gdImagePtr im = 0;
	JSAMPROW rowptr[1];
	unsigned int i, j;
	int retval;
	JDIMENSION nrows;
	int channels = 3;
	int inverted = 0;

	memset (&cinfo, 0, sizeof (cinfo));
	memset (&jerr, 0, sizeof (jerr));

	jmpbufw.ignore_warning = ignore_warning;

	cinfo.err = jpeg_std_error (&jerr);
	cinfo.client_data = &jmpbufw;

	cinfo.err->emit_message = (void (*)(j_common_ptr,int)) php_jpeg_emit_message;

	if (setjmp (jmpbufw.jmpbuf) != 0) {
		/* we're here courtesy of longjmp */
		if (row) {
			gdFree (row);
		}
		if (im) {
			gdImageDestroy (im);
		}
		return 0;
	}

	cinfo.err->error_exit = fatal_jpeg_error;

	jpeg_create_decompress (&cinfo);

	jpeg_gdIOCtx_src (&cinfo, infile);

	/* 2.0.22: save the APP14 marker to check for Adobe Photoshop CMYK files with inverted components. */
	jpeg_save_markers(&cinfo, JPEG_APP0 + 14, 256);

	retval = jpeg_read_header (&cinfo, TRUE);
	if (retval != JPEG_HEADER_OK) {
		php_gd_error_ex(E_WARNING, "gd-jpeg: warning: jpeg_read_header returned %d, expected %d", retval, JPEG_HEADER_OK);
	}

	if (cinfo.image_height > INT_MAX) {
		php_gd_error_ex(E_WARNING, "gd-jpeg: warning: JPEG image height (%u) is greater than INT_MAX (%d) (and thus greater than gd can handle)", cinfo.image_height, INT_MAX);
	}

	if (cinfo.image_width > INT_MAX) {
		php_gd_error_ex(E_WARNING, "gd-jpeg: warning: JPEG image width (%u) is greater than INT_MAX (%d) (and thus greater than gd can handle)", cinfo.image_width, INT_MAX);
	}

	im = gdImageCreateTrueColor ((int) cinfo.image_width, (int) cinfo.image_height);
	if (im == 0) {
		php_gd_error("gd-jpeg error: cannot allocate gdImage struct");
		goto error;
	}

	/* 2.0.22: very basic support for reading CMYK colorspace files. Nice for
	 * thumbnails but there's no support for fussy adjustment of the
	 * assumed properties of inks and paper. */
	if ((cinfo.jpeg_color_space == JCS_CMYK) || (cinfo.jpeg_color_space == JCS_YCCK)) {
		cinfo.out_color_space = JCS_CMYK;
	} else {
		cinfo.out_color_space = JCS_RGB;
	}

	if (jpeg_start_decompress (&cinfo) != TRUE) {
		php_gd_error("gd-jpeg: warning: jpeg_start_decompress reports suspended data source");
	}

	/* REMOVED by TBB 2/12/01. This field of the structure is
	 * documented as private, and sure enough it's gone in the
	 * latest libjpeg, replaced by something else. Unfortunately
	 * there is still no right way to find out if the file was
	 * progressive or not; just declare your intent before you
	 * write one by calling gdImageInterlace(im, 1) yourself.
	 * After all, we're not really supposed to rework JPEGs and
	 * write them out again anyway. Lossy compression, remember?
	 */
#if 0
  gdImageInterlace (im, cinfo.progressive_mode != 0);
#endif

	if (cinfo.out_color_space == JCS_RGB) {
		if (cinfo.output_components != 3) {
			php_gd_error_ex(E_WARNING, "gd-jpeg: error: JPEG color quantization request resulted in output_components == %d (expected 3 for RGB)", cinfo.output_components);
			goto error;
		}
		channels = 3;
	} else if (cinfo.out_color_space == JCS_CMYK) {
		jpeg_saved_marker_ptr marker;
		if (cinfo.output_components != 4)  {
			php_gd_error_ex(E_WARNING, "gd-jpeg: error: JPEG color quantization request resulted in output_components == %d (expected 4 for CMYK)", cinfo.output_components);
			goto error;
		}
		channels = 4;
		marker = cinfo.marker_list;
		while (marker) {
			if ((marker->marker == (JPEG_APP0 + 14)) && (marker->data_length >= 12) && (!strncmp((const char *) marker->data, "Adobe", 5))) {
				inverted = 1;
				break;
			}
			marker = marker->next;
		}
	} else {
		php_gd_error_ex(E_WARNING, "gd-jpeg: error: unexpected colorspace.");
		goto error;
	}

#if BITS_IN_JSAMPLE == 12
	php_gd_error("gd-jpeg: error: jpeg library was compiled for 12-bit precision. This is mostly useless, because JPEGs on the web are 8-bit and such versions of the jpeg library won't read or write them. GD doesn't support these unusual images. Edit your jmorecfg.h file to specify the correct precision and completely 'make clean' and 'make install' libjpeg again. Sorry.");
	goto error;
#endif /* BITS_IN_JSAMPLE == 12 */

	row = safe_emalloc(cinfo.output_width * channels, sizeof(JSAMPLE), 0);
	memset(row, 0, cinfo.output_width * channels * sizeof(JSAMPLE));
	rowptr[0] = row;

	if (cinfo.out_color_space == JCS_CMYK) {
		for (i = 0; i < cinfo.output_height; i++) {
			register JSAMPROW currow = row;
			register int *tpix = im->tpixels[i];
			nrows = jpeg_read_scanlines (&cinfo, rowptr, 1);
			if (nrows != 1) {
				php_gd_error_ex(E_WARNING, "gd-jpeg: error: jpeg_read_scanlines returns %u, expected 1", nrows);
				goto error;
			}
			for (j = 0; j < cinfo.output_width; j++, currow += 4, tpix++) {
				*tpix = CMYKToRGB (currow[0], currow[1], currow[2], currow[3], inverted);
			}
		}
	} else {
		for (i = 0; i < cinfo.output_height; i++) {
			register JSAMPROW currow = row;
			register int *tpix = im->tpixels[i];
			nrows = jpeg_read_scanlines (&cinfo, rowptr, 1);
			if (nrows != 1) {
				php_gd_error_ex(E_WARNING, "gd-jpeg: error: jpeg_read_scanlines returns %u, expected 1", nrows);
				goto error;
			}
			for (j = 0; j < cinfo.output_width; j++, currow += 3, tpix++) {
				*tpix = gdTrueColor (currow[0], currow[1], currow[2]);
			}
		}
	}

	if (jpeg_finish_decompress (&cinfo) != TRUE) {
		php_gd_error("gd-jpeg: warning: jpeg_finish_decompress reports suspended data source");
	}
	if (!ignore_warning) {
		if (cinfo.err->num_warnings > 0) {
			goto error;
		}
	}

	jpeg_destroy_decompress (&cinfo);
	gdFree (row);

	return im;

error:
	jpeg_destroy_decompress (&cinfo);
	if (row) {
		gdFree (row);
	}
	if (im) {
		gdImageDestroy (im);
	}
	return 0;
}

/* A very basic conversion approach, TBB */
static int CMYKToRGB(int c, int m, int y, int k, int inverted)
{
	if (inverted) {
		c = 255 - c;
		m = 255 - m;
		y = 255 - y;
		k = 255 - k;
	}
	return gdTrueColor((255 - c) * (255 - k) / 255, (255 - m) * (255 - k) / 255, (255 - y) * (255 - k) / 255);
}

/*
 * gdIOCtx JPEG data sources and sinks, T. Boutell
 * almost a simple global replace from T. Lane's stdio versions.
 *
 */

/* Different versions of libjpeg use either 'jboolean' or 'boolean', and
   some platforms define 'boolean', and so forth. Deal with this
   madness by typedeffing 'safeboolean' to 'boolean' if HAVE_BOOLEAN
   is already set, because this is the test that libjpeg uses.
   Otherwise, typedef it to int, because that's what libjpeg does
   if HAVE_BOOLEAN is not defined. -TBB */

#ifdef HAVE_BOOLEAN
typedef boolean safeboolean;
#else
typedef int safeboolean;
#endif /* HAVE_BOOLEAN */

/* Expanded data source object for gdIOCtx input */

typedef struct
{
	struct jpeg_source_mgr pub;	/* public fields */

	gdIOCtx *infile;		/* source stream */
	unsigned char *buffer;	/* start of buffer */
	safeboolean start_of_file;	/* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr *my_src_ptr;

#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

void init_source (j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;

	/* We reset the empty-input-file flag for each image,
	 * but we don't clear the input buffer.
	 * This is correct behavior for reading a series of images from one source.
	 */
	src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

#define END_JPEG_SEQUENCE "\r\n[*]--:END JPEG:--[*]\r\n"

safeboolean fill_input_buffer (j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;
	/* 2.0.12: signed size. Thanks to Geert Jansen */
	ssize_t nbytes = 0;

	/* ssize_t got; */
	/* char *s; */
	memset(src->buffer, 0, INPUT_BUF_SIZE);

	while (nbytes < INPUT_BUF_SIZE) {
		int got = gdGetBuf(src->buffer + nbytes, INPUT_BUF_SIZE - nbytes, src->infile);

		if (got == EOF || got == 0) {
			/* EOF or error. If we got any data, don't worry about it. If we didn't, then this is unexpected. */
			if (!nbytes) {
				nbytes = -1;
			}
			break;
		}
		nbytes += got;
	}

	if (nbytes <= 0) {
		if (src->start_of_file)	{ /* Treat empty input file as fatal error */
			ERREXIT (cinfo, JERR_INPUT_EMPTY);
		}
		WARNMS (cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (unsigned char) 0xFF;
		src->buffer[1] = (unsigned char) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;

	/* Just a dumb implementation for now. Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	 */
	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer (cinfo);
			/* note we assume that fill_input_buffer will never return FALSE,
			 * so suspension need not be handled.
			 */
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}


/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

void term_source (j_decompress_ptr cinfo)
{
#if 0
	* never used */
	my_src_ptr src = (my_src_ptr) cinfo->src;
#endif
}


/*
 * Prepare for input from a gdIOCtx stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

void jpeg_gdIOCtx_src (j_decompress_ptr cinfo, gdIOCtx * infile)
{
	my_src_ptr src;

	/* The source object and input buffer are made permanent so that a series
	 * of JPEG images can be read from the same file by calling jpeg_gdIOCtx_src
	 * only before the first one.  (If we discarded the buffer at the end of
	 * one image, we'd likely lose the start of the next one.)
	 * This makes it unsafe to use this manager and a different source
	 * manager serially with the same JPEG object.  Caveat programmer.
	 */
	if (cinfo->src == NULL) { /* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof (my_source_mgr));
		src = (my_src_ptr) cinfo->src;
		src->buffer = (unsigned char *) (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * sizeof (unsigned char));

	}

	src = (my_src_ptr) cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart;	/* use default method */
	src->pub.term_source = term_source;
	src->infile = infile;
	src->pub.bytes_in_buffer = 0;	/* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL;	/* until buffer loaded */
}

/* Expanded data destination object for stdio output */

typedef struct
{
	struct jpeg_destination_mgr pub; /* public fields */
	gdIOCtx *outfile;		 /* target stream */
	unsigned char *buffer;		 /* start of buffer */
} my_destination_mgr;

typedef my_destination_mgr *my_dest_ptr;

#define OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */

/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */

void init_destination (j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

	/* Allocate the output buffer --- it will be released when done with image */
	dest->buffer = (unsigned char *) (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE, OUTPUT_BUF_SIZE * sizeof (unsigned char));

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */

safeboolean empty_output_buffer (j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

	if (gdPutBuf (dest->buffer, OUTPUT_BUF_SIZE, dest->outfile) != (size_t) OUTPUT_BUF_SIZE) {
		ERREXIT (cinfo, JERR_FILE_WRITE);
	}

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

void term_destination (j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

	/* Write any data remaining in the buffer */
	if (datacount > 0 && ((size_t)gdPutBuf (dest->buffer, datacount, dest->outfile) != datacount)) {
		ERREXIT (cinfo, JERR_FILE_WRITE);
	}
}


/*
 * Prepare for output to a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing compression.
 */

void jpeg_gdIOCtx_dest (j_compress_ptr cinfo, gdIOCtx * outfile)
{
	my_dest_ptr dest;

	/* The destination object is made permanent so that multiple JPEG images
	 * can be written to the same file without re-executing jpeg_stdio_dest.
	 * This makes it dangerous to use this manager and a different destination
	 * manager serially with the same JPEG object, because their private object
	 * sizes may be different.  Caveat programmer.
	 */
	if (cinfo->dest == NULL) { /* first time for this JPEG object? */
		cinfo->dest = (struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof (my_destination_mgr));
	}

	dest = (my_dest_ptr) cinfo->dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->outfile = outfile;
}

#endif /* HAVE_JPEG */
