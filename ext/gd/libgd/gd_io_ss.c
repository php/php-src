
/*
   * io_ss.c
   *
   * Implements the Source/Sink interface.
   *
   * As will all I/O modules, most functions are for local use only (called
   * via function pointers in the I/O context).
   *
   * The Source/Sink model is the primary 'user' interface for alternate data
   * sources; the IOCtx interface is intended (at least in version 1.5) to be
   * used internally until it settles down a bit.
   *
   * This module just layers the Source/Sink interface on top of the IOCtx; no
   * support is provided for tell/seek, so GD2 writing is not possible, and 
   * retrieving parts of GD2 files is also not possible.
   *
   * A new SS context does not need to be created with both a Source and a Sink.
   *
   * Written/Modified 1999, Philip Warner.
   *
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"

/* this is used for creating images in main memory */

typedef struct ssIOCtx
  {
    gdIOCtx ctx;
    gdSourcePtr src;
    gdSinkPtr snk;
  }
ssIOCtx;

typedef struct ssIOCtx *ssIOCtxPtr;

gdIOCtx *gdNewSSCtx (gdSourcePtr src, gdSinkPtr snk);

static int sourceGetbuf (gdIOCtx *, void *, int);
static int sourceGetchar (gdIOCtx * ctx);
static int sinkPutbuf (gdIOCtx * ctx, const void *buf, int size);
static void sinkPutchar (gdIOCtx * ctx, int a);
static void gdFreeSsCtx (gdIOCtx * ctx);

/* return data as a dynamic pointer */
gdIOCtx *
gdNewSSCtx (gdSourcePtr src, gdSinkPtr snk)
{
  ssIOCtxPtr ctx;

  ctx = (ssIOCtxPtr) gdMalloc (sizeof (ssIOCtx));
  if (ctx == NULL)
    {
      return NULL;
    }

  ctx->src = src;
  ctx->snk = snk;

  ctx->ctx.getC = sourceGetchar;
  ctx->ctx.getBuf = sourceGetbuf;

  ctx->ctx.putC = sinkPutchar;
  ctx->ctx.putBuf = sinkPutbuf;

  ctx->ctx.tell = NULL;
  ctx->ctx.seek = NULL;

  ctx->ctx.free = gdFreeSsCtx;

  return (gdIOCtx *) ctx;
}

static
void
gdFreeSsCtx (gdIOCtx * ctx)
{
  gdFree (ctx);
}


static int
sourceGetbuf (gdIOCtx * ctx, void *buf, int size)
{
  ssIOCtx *lctx;
  int res;

  lctx = (ssIOCtx *) ctx;

  res = ((lctx->src->source) (lctx->src->context, buf, size));

/*
   ** Translate the return values from the Source object: 
   ** 0 is EOF, -1 is error
 */

  if (res == 0)
    {
      return EOF;
    }
  else if (res < 0)
    {
      return 0;
    }
  else
    {
      return res;
    };

}

static int
sourceGetchar (gdIOCtx * ctx)
{
  int res;
  unsigned char buf;

  res = sourceGetbuf (ctx, &buf, 1);

  if (res == 1)
    {
      return buf;
    }
  else
    {
      return EOF;
    };

}

static int
sinkPutbuf (gdIOCtx * ctx, const void *buf, int size)
{
  ssIOCtxPtr lctx;
  int res;

  lctx = (ssIOCtx *) ctx;

  res = (lctx->snk->sink) (lctx->snk->context, buf, size);

  if (res <= 0)
    {
      return 0;
    }
  else
    {
      return res;
    };

}

static void
sinkPutchar (gdIOCtx * ctx, int a)
{
  unsigned char b;

  b = a;
  sinkPutbuf (ctx, &b, 1);

}
