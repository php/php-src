/* $Id$ */
#ifdef __cplusplus
extern "C" {
#endif

	/*
		gd_bmp.c

		Bitmap format support for libgd

		* Written 2007, Scott MacVicar
		---------------------------------------------------------------------------

		Todo:

		RLE4, RLE8 and Bitfield encoding
		Add full support for Windows v4 and Windows v5 header formats

		----------------------------------------------------------------------------
	 */

#ifndef BMP_H
#define BMP_H	1

#define BMP_PALETTE_3 1
#define BMP_PALETTE_4 2

#define BMP_WINDOWS_V3 40
#define BMP_OS2_V1 12
#define BMP_OS2_V2 64
#define BMP_WINDOWS_V4 108
#define BMP_WINDOWS_V5 124

#define BMP_BI_RGB 0
#define BMP_BI_RLE8 1
#define BMP_BI_RLE4 2
#define BMP_BI_BITFIELDS 3
#define BMP_BI_JPEG 4
#define BMP_BI_PNG 5

#define BMP_RLE_COMMAND 0
#define BMP_RLE_ENDOFLINE 0
#define BMP_RLE_ENDOFBITMAP 1
#define BMP_RLE_DELTA 2

#define BMP_RLE_TYPE_RAW 0
#define BMP_RLE_TYPE_RLE 1

	/* BMP header. */
	typedef struct {
		/* 16 bit - header identifying the type */
		signed short int magic;

		/* 32bit - size of the file */
		int size;

		/* 16bit - these two are in the spec but "reserved" */
		signed short int reserved1;
		signed short int reserved2;

		/* 32 bit - offset of the bitmap header from data in bytes */
		signed int off;

	} bmp_hdr_t;

	/* BMP info. */
	typedef struct {
		/* 16bit - Type, ie Windows or OS/2 for the palette info */
		signed short int type;
		/* 32bit - The length of the bitmap information header in bytes. */
		signed int len;

		/* 32bit - The width of the bitmap in pixels. */
		signed int width;

		/* 32bit - The height of the bitmap in pixels. */
		signed int height;

		/* 8 bit - The bitmap data is specified in top-down order. */
		signed char topdown;

		/* 16 bit - The number of planes.  This must be set to a value of one. */
		signed short int numplanes;

		/* 16 bit - The number of bits per pixel. */
		signed short int depth;

		/* 32bit - The type of compression used. */
		signed int enctype;

		/* 32bit - The size of the image in bytes. */
		signed int size;

		/* 32bit - The horizontal resolution in pixels/metre. */
		signed int hres;

		/* 32bit - The vertical resolution in pixels/metre. */
		signed int vres;

		/* 32bit - The number of color indices used by the bitmap. */
		signed int numcolors;

		/* 32bit - The number of color indices important for displaying the bitmap. */
		signed int mincolors;

	} bmp_info_t;

#endif

#ifdef __cplusplus
}
#endif
