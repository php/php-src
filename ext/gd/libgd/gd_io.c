

/*
   * io.c
   *
   * Implements the imple I/O 'helper' routines.
   *
   * Not really essential, but these routines were used extensively in GD,
   * so they were moved here. They also make IOCtx calls look better...
   *
   * Written (or, at least, moved) 1999, Philip Warner.
   *
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"

/* Use this for commenting out debug-print statements. */
/* Just use the first '#define' to allow all the prints... */
/*#define IO_DBG(s) (s) */
#define IO_DBG(s)


/*
 * Write out a word to the I/O context pointer
 */
void
Putword (int w, gdIOCtx * ctx)
{
  unsigned char buf[2];
  buf[0] = w & 0xff;
  buf[1] = (w / 256) & 0xff;
  (ctx->putBuf) (ctx, (char *) buf, 2);
}

void
Putchar (int c, gdIOCtx * ctx)
{
  (ctx->putC) (ctx, c & 0xff);
}

void
gdPutC (const unsigned char c, gdIOCtx * ctx)
{
  (ctx->putC) (ctx, c);
}

void
gdPutWord (int w, gdIOCtx * ctx)
{
  IO_DBG (printf ("Putting word...\n"));
  (ctx->putC) (ctx, (unsigned char) (w >> 8));
  (ctx->putC) (ctx, (unsigned char) (w & 0xFF));
  IO_DBG (printf ("put.\n"));
}

void
gdPutInt (int w, gdIOCtx * ctx)
{
  IO_DBG (printf ("Putting int...\n"));
  (ctx->putC) (ctx, (unsigned char) (w >> 24));
  (ctx->putC) (ctx, (unsigned char) ((w >> 16) & 0xFF));
  (ctx->putC) (ctx, (unsigned char) ((w >> 8) & 0xFF));
  (ctx->putC) (ctx, (unsigned char) (w & 0xFF));
  IO_DBG (printf ("put.\n"));
}

int
gdGetC (gdIOCtx * ctx)
{
  return ((ctx->getC) (ctx));
}



int
gdGetByte (int *result, gdIOCtx * ctx)
{
  int r;
  r = (ctx->getC) (ctx);
  if (r == EOF)
    {
      return 0;
    }
  *result = r;
  return 1;
}

int
gdGetWord (int *result, gdIOCtx * ctx)
{
  int r;
  r = (ctx->getC) (ctx);
  if (r == EOF)
    {
      return 0;
    }
  *result = r << 8;
  r = (ctx->getC) (ctx);
  if (r == EOF)
    {
      return 0;
    }
  *result += r;
  return 1;
}


int
gdGetInt (int *result, gdIOCtx * ctx)
{
  int r;
  r = (ctx->getC) (ctx);
  if (r == EOF)
    {
      return 0;
    }
  *result = r << 24;

  r = (ctx->getC) (ctx);
  if (r == EOF)
    {
      return 0;
    }
  *result += r << 16;

  r = (ctx->getC) (ctx);
  if (r == EOF)
    {
      return 0;
    }
  *result += r << 8;

  r = (ctx->getC) (ctx);
  if (r == EOF)
    {
      return 0;
    }
  *result += r;

  return 1;
}

int
gdPutBuf (const void *buf, int size, gdIOCtx * ctx)
{
  IO_DBG (printf ("Putting buf...\n"));
  return (ctx->putBuf) (ctx, buf, size);
  IO_DBG (printf ("put.\n"));
}

int
gdGetBuf (void *buf, int size, gdIOCtx * ctx)
{
  return (ctx->getBuf) (ctx, buf, size);
}


int
gdSeek (gdIOCtx * ctx, const int pos)
{
  IO_DBG (printf ("Seeking...\n"));
  return ((ctx->seek) (ctx, pos));
  IO_DBG (printf ("Done.\n"));
}

long
gdTell (gdIOCtx * ctx)
{
  IO_DBG (printf ("Telling...\n"));
  return ((ctx->tell) (ctx));
  IO_DBG (printf ("told.\n"));
}
