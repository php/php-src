/*
   * io_file.c
   *
   * Implements the file interface.
   *
   * As will all I/O modules, most functions are for local use only (called
   * via function pointers in the I/O context).
   *
   * Most functions are just 'wrappers' for standard file functions.
   *
   * Written/Modified 1999, Philip Warner.
   *
 */

/* For platforms with incomplete ANSI defines. Fortunately,
 * SEEK_SET is defined to be zero by the standard. */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* SEEK_SET */

#include "gd.h"
#include "gdhelpers.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* this is used for creating images in main memory */

typedef struct fileIOCtx {
	gdIOCtx ctx;
	FILE *f;
} fileIOCtx;

gdIOCtxPtr newFileCtx(FILE *f);

static int fileGetbuf(gdIOCtxPtr, void *, int);
static int filePutbuf(gdIOCtxPtr, const void *, int);
static void filePutchar(gdIOCtxPtr, int);
static int fileGetchar(gdIOCtxPtr ctx);

static int fileSeek(gdIOCtxPtr, const int);
static long fileTell(gdIOCtxPtr);
static void gdFreeFileCtx(gdIOCtxPtr ctx);

/*
	Function: gdNewFileCtx

	Return data as a dynamic pointer.
*/
BGD_DECLARE(gdIOCtxPtr) gdNewFileCtx(FILE *f) {
	fileIOCtx *ctx;

	if (f == NULL)
		return NULL;
	ctx = (fileIOCtx *) gdMalloc(sizeof (fileIOCtx));
	if (ctx == NULL) {
		return NULL;
	}

	ctx->f = f;

	ctx->ctx.getC = fileGetchar;
	ctx->ctx.putC = filePutchar;

	ctx->ctx.getBuf = fileGetbuf;
	ctx->ctx.putBuf = filePutbuf;

	ctx->ctx.tell = fileTell;
	ctx->ctx.seek = fileSeek;

	ctx->ctx.gd_free = gdFreeFileCtx;

	return (gdIOCtxPtr)ctx;
}

static void gdFreeFileCtx(gdIOCtxPtr ctx) { gdFree(ctx); }

static int filePutbuf(gdIOCtxPtr ctx, const void *buf, int size) {
	fileIOCtx *fctx;
	fctx = (fileIOCtx *) ctx;

	return fwrite(buf, 1, size, fctx->f);
}

static int fileGetbuf(gdIOCtxPtr ctx, void *buf, int size) {
	fileIOCtx *fctx;
	fctx = (fileIOCtx *) ctx;

	return (fread(buf, 1, size, fctx->f));
}

static void filePutchar(gdIOCtxPtr ctx, int a) {
	unsigned char b;
	fileIOCtx *fctx;
	fctx = (fileIOCtx *) ctx;

	b = a;

	putc (b, fctx->f);
}

static int fileGetchar(gdIOCtxPtr ctx) {
	fileIOCtx *fctx;
	fctx = (fileIOCtx *) ctx;

	return getc (fctx->f);
}

static int fileSeek(gdIOCtxPtr ctx, const int pos) {
	fileIOCtx *fctx;
	fctx = (fileIOCtx *) ctx;
	return (fseek (fctx->f, pos, SEEK_SET) == 0);
}

static long fileTell(gdIOCtxPtr ctx) {
	fileIOCtx *fctx;
	fctx = (fileIOCtx *) ctx;

	return ftell (fctx->f);
}
