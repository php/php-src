
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
   SEEK_SET is defined to be zero by the standard. */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* SEEK_SET */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"

/* this is used for creating images in main memory */

typedef struct fileIOCtx
  {
    gdIOCtx ctx;
    FILE *f;
  }
fileIOCtx;

struct fileIOCtx *fileIOCtxPtr;

gdIOCtx *newFileCtx (FILE * f);

static int fileGetbuf (gdIOCtx *, void *, int);
static int filePutbuf (gdIOCtx *, const void *, int);
static void filePutchar (gdIOCtx *, int);
static int fileGetchar (gdIOCtx * ctx);

static int fileSeek (struct gdIOCtx *, const int);
static long fileTell (struct gdIOCtx *);
static void gdFreeFileCtx (gdIOCtx * ctx);

/* return data as a dynamic pointer */
gdIOCtx *
gdNewFileCtx (FILE * f)
{
  fileIOCtx *ctx;

  ctx = (fileIOCtx *) gdMalloc (sizeof (fileIOCtx));
  if (ctx == NULL)
    {
      return NULL;
    }

  ctx->f = f;

  ctx->ctx.getC = fileGetchar;
  ctx->ctx.putC = filePutchar;

  ctx->ctx.getBuf = fileGetbuf;
  ctx->ctx.putBuf = filePutbuf;

  ctx->ctx.tell = fileTell;
  ctx->ctx.seek = fileSeek;

  ctx->ctx.free = gdFreeFileCtx;

  return (gdIOCtx *) ctx;
}

static
void
gdFreeFileCtx (gdIOCtx * ctx)
{
  gdFree (ctx);
}


static int
filePutbuf (gdIOCtx * ctx, const void *buf, int size)
{
  fileIOCtx *fctx;
  fctx = (fileIOCtx *) ctx;

  return fwrite (buf, 1, size, fctx->f);

}

static int
fileGetbuf (gdIOCtx * ctx, void *buf, int size)
{
  fileIOCtx *fctx;
  fctx = (fileIOCtx *) ctx;

  return (fread (buf, 1, size, fctx->f));

}

static void
filePutchar (gdIOCtx * ctx, int a)
{
  unsigned char b;
  fileIOCtx *fctx;
  fctx = (fileIOCtx *) ctx;

  b = a;

  putc (b, fctx->f);
}

static int
fileGetchar (gdIOCtx * ctx)
{
  fileIOCtx *fctx;
  fctx = (fileIOCtx *) ctx;

  return getc (fctx->f);
}


static int
fileSeek (struct gdIOCtx *ctx, const int pos)
{
  fileIOCtx *fctx;
  fctx = (fileIOCtx *) ctx;

  return (fseek (fctx->f, pos, SEEK_SET) == 0);
}

static long
fileTell (struct gdIOCtx *ctx)
{
  fileIOCtx *fctx;
  fctx = (fileIOCtx *) ctx;

  return ftell (fctx->f);
}
