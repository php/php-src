#ifndef __TGA_H
#define __TGA_H	1

#include "gd.h"
#include "gdhelpers.h"

#include "gd_intern.h"

typedef struct oTga_ {
	uint8_t  identsize;	// size of ID field that follows 18 uint8_t header (0 usually)
	uint8_t  colormaptype;	// type of colour map 0=none, 1=has palette					[IGNORED] Adrian requested no support
	uint8_t  imagetype;	// type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	int colormapstart;	// first colour map entry in palette						[IGNORED] Adrian requested no support
	int colormaplength;	// number of colours in palette								[IGNORED] Adrian requested no support
	uint8_t  colormapbits;	// number of bits per palette entry 15,16,24,32				[IGNORED] Adrian requested no support

	int xstart;			// image x origin
	int ystart;			// image y origin
	int width;			// image width in pixels
	int height;			// image height in pixels
	uint8_t  bits;			// image bits per pixel 8,16,24,32
	uint8_t alphabits;		// alpha bits (low 4bits of header 17)
	uint8_t fliph;			// horizontal or vertical
	uint8_t flipv;			// flip
	char *ident;		// identifcation tag string
	int *bitmap;		// bitmap data

} oTga;

#define TGA_TYPE_NO_IMAGE						0
#define TGA_TYPE_INDEXED						1
#define TGA_TYPE_RGB							2
#define TGA_TYPE_GREYSCALE						3
#define TGA_TYPE_INDEXED_RLE					9
#define TGA_TYPE_RGB_RLE						10
#define TGA_TYPE_GREYSCALE_RLE					11
#define TGA_TYPE_INDEXED_HUFFMAN_DELTA_RLE		32
#define TGA_TYPE_RGB_HUFFMAN_DELTA_QUADTREE_RLE	33

#define TGA_BPP_8	8
#define TGA_BPP_16	16
#define TGA_BPP_24	24
#define TGA_BPP_32	32

#define TGA_RLE_FLAG	128

int read_header_tga(gdIOCtx *ctx, oTga *tga);
int read_image_tga(gdIOCtx *ctx, oTga *tga);
void free_tga(oTga *tga);

#endif //__TGA_H
