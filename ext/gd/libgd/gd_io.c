

/*
   * io.c
   *
   * Implements the simple I/O 'helper' routines.
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


#define GD_IO_EOF_CHK(r)	\
	if (r == EOF) {		\
		return 0;	\
	}			\

/*
 * Write out a word to the I/O context pointer
 */
void Putword (int w, gdIOCtx * ctx)
{
	unsigned char buf[2];

	buf[0] = w & 0xff;
	buf[1] = (w / 256) & 0xff;
	(ctx->putBuf) (ctx, (char *) buf, 2);
}

void Putchar (int c, gdIOCtx * ctx)
{
	(ctx->putC) (ctx, c & 0xff);
}

void gdPutC (const unsigned char c, gdIOCtx * ctx)
{
	(ctx->putC) (ctx, c);
}

void gdPutWord (int w, gdIOCtx * ctx)
{
	IO_DBG (php_gd_error("Putting word..."));
	(ctx->putC) (ctx, (unsigned char) (w >> 8));
	(ctx->putC) (ctx, (unsigned char) (w & 0xFF));
	IO_DBG (php_gd_error("put."));
}

void gdPutInt (int w, gdIOCtx * ctx)
{
	IO_DBG (php_gd_error("Putting int..."));
	(ctx->putC) (ctx, (unsigned char) (w >> 24));
	(ctx->putC) (ctx, (unsigned char) ((w >> 16) & 0xFF));
	(ctx->putC) (ctx, (unsigned char) ((w >> 8) & 0xFF));
	(ctx->putC) (ctx, (unsigned char) (w & 0xFF));
	IO_DBG (php_gd_error("put."));
}

int gdGetC (gdIOCtx * ctx)
{
	return ((ctx->getC) (ctx));
}

int gdGetByte (int *result, gdIOCtx * ctx)
{
	int r;
	r = (ctx->getC) (ctx);
	GD_IO_EOF_CHK(r);
	*result = r;
	return 1;
}

int gdGetWord (int *result, gdIOCtx * ctx)
{
	int r;
	r = (ctx->getC) (ctx);
	GD_IO_EOF_CHK(r);
	*result = r << 8;
	r = (ctx->getC) (ctx);
	GD_IO_EOF_CHK(r);
	*result += r;
	return 1;
}


int gdGetInt (int *result, gdIOCtx * ctx)
{
	int r;
	r = (ctx->getC) (ctx);
	GD_IO_EOF_CHK(r);
	*result = r << 24;

	r = (ctx->getC) (ctx);
	GD_IO_EOF_CHK(r);
	*result += r << 16;

	r = (ctx->getC) (ctx);
	if (r == EOF) {
		return 0;
	}
	*result += r << 8;

	r = (ctx->getC) (ctx);
	GD_IO_EOF_CHK(r);
	*result += r;

	return 1;
}

int gdPutBuf (const void *buf, int size, gdIOCtx * ctx)
{
	IO_DBG (php_gd_error("Putting buf..."));
	return (ctx->putBuf) (ctx, buf, size);
	IO_DBG (php_gd_error("put."));
}

int gdGetBuf (void *buf, int size, gdIOCtx * ctx)
{
	return (ctx->getBuf) (ctx, buf, size);
}

int gdSeek (gdIOCtx * ctx, const int pos)
{
	IO_DBG (php_gd_error("Seeking..."));
	return ((ctx->seek) (ctx, pos));
	IO_DBG (php_gd_error("Done."));
}

long gdTell (gdIOCtx * ctx)
{
	IO_DBG (php_gd_error("Telling..."));
	return ((ctx->tell) (ctx));
	IO_DBG (php_gd_error ("told."));
}
