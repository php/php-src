

/*
 * io.c
 *
 * Implements the simple I/O 'helper' routines.
 *
 * Not really essential, but these routines were used extensively in GD,
 * so they were moved here. They also make IOCtx calls look better...
 *
 * Written (or, at least, moved) 1999, Philip Warner.
 */

#include "gd.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Use this for commenting out debug-print statements. */
/* Just use the first '#define' to allow all the prints... */
/*#define IO_DBG(s) (s) */
#define IO_DBG(s)

#define GD_IO_EOF_CHK(r)                                                                           \
    if (r == EOF) {                                                                                \
        return 0;                                                                                  \
    }

void gdPutC(const unsigned char c, gdIOCtx *ctx) { (ctx->putC)(ctx, c); }

void gdPutWord(int w, gdIOCtx *ctx)
{
    IO_DBG(printf("Putting word...\n"));
    (ctx->putC)(ctx, (unsigned char)(w >> 8));
    (ctx->putC)(ctx, (unsigned char)(w & 0xFF));
    IO_DBG(printf("put.\n"));
}

void gdPutInt(int w, gdIOCtx *ctx)
{
    IO_DBG(printf("Putting int...\n"));
    (ctx->putC)(ctx, (unsigned char)(w >> 24));
    (ctx->putC)(ctx, (unsigned char)((w >> 16) & 0xFF));
    (ctx->putC)(ctx, (unsigned char)((w >> 8) & 0xFF));
    (ctx->putC)(ctx, (unsigned char)(w & 0xFF));
    IO_DBG(printf("put.\n"));
}

int gdGetC(gdIOCtx *ctx) { return ((ctx->getC)(ctx)); }

int gdGetByte(int *result, gdIOCtx *ctx)
{
    int r;

    r = (ctx->getC)(ctx);
    if (r == EOF) {
        return 0;
    }

    *result = r;

    return 1;
}

int gdGetWord(int *result, gdIOCtx *ctx)
{
    int r;

    r = (ctx->getC)(ctx);
    if (r == EOF) {
        return 0;
    }

    *result = r << 8;

    r = (ctx->getC)(ctx);
    if (r == EOF) {
        return 0;
    }

    *result += r;

    return 1;
}

int gdGetWordLSB(signed short int *result, gdIOCtx *ctx)
{
    int high = 0, low = 0;
    low = (ctx->getC)(ctx);
    if (low == EOF) {
        return 0;
    }

    high = (ctx->getC)(ctx);
    if (high == EOF) {
        return 0;
    }

    if (result) {
        *result = (high << 8) | low;
    }

    return 1;
}

int gdGetInt(int *result, gdIOCtx *ctx)
{
    int r;
    uint32_t value;

    r = (ctx->getC)(ctx);
    if (r == EOF) {
        return 0;
    }

    value = (uint32_t)r << 24;

    r = (ctx->getC)(ctx);
    if (r == EOF) {
        return 0;
    }

    value |= (uint32_t)r << 16;

    r = (ctx->getC)(ctx);
    if (r == EOF) {
        return 0;
    }

    value |= (uint32_t)r << 8;

    r = (ctx->getC)(ctx);
    if (r == EOF) {
        return 0;
    }

    value |= (uint32_t)r;
    *result = (int32_t)value;

    return 1;
}

int gdGetIntLSB(signed int *result, gdIOCtx *ctx)
{
    int c;
    uint32_t r;

    c = (ctx->getC)(ctx);
    if (c == EOF) {
        return 0;
    }
    r = (uint32_t)c;

    c = (ctx->getC)(ctx);
    if (c == EOF) {
        return 0;
    }
    r |= (uint32_t)c << 8;

    c = (ctx->getC)(ctx);
    if (c == EOF) {
        return 0;
    }
    r |= (uint32_t)c << 16;

    c = (ctx->getC)(ctx);
    if (c == EOF) {
        return 0;
    }
    r |= (uint32_t)c << 24;

    if (result) {
        *result = (int32_t)r;
    }

    return 1;
}

int gdPutBuf(const void *buf, int size, gdIOCtx *ctx)
{
    IO_DBG(printf("Putting buf...\n"));
    return (ctx->putBuf)(ctx, buf, size);
    IO_DBG(printf("put.\n"));
}

int gdGetBuf(void *buf, int size, gdIOCtx *ctx) { return (ctx->getBuf)(ctx, buf, size); }

int gdSeek(gdIOCtx *ctx, const int pos)
{
    IO_DBG(printf("Seeking...\n"));
    return ((ctx->seek)(ctx, pos));
    IO_DBG(printf("Done.\n"));
}

long gdTell(gdIOCtx *ctx)
{
    IO_DBG(printf("Telling...\n"));
    return ((ctx->tell)(ctx));
    IO_DBG(printf("told.\n"));
}
