#ifndef GD_IO_H
#define GD_IO_H 1

#include <stdio.h>

#ifdef VMS
#define Putchar gdPutchar
#endif

typedef struct gdIOCtx {
	int	(*getC)(struct gdIOCtx*);
	int	(*getBuf)(struct gdIOCtx*, void*, int);

	void	(*putC)(struct gdIOCtx*, int);
	int	(*putBuf)(struct gdIOCtx*, const void*, int);

	int	(*seek)(struct gdIOCtx*, const int);
	long	(*tell)(struct gdIOCtx*);

	void	(*gd_free)(struct gdIOCtx*);

	void	*data;
} gdIOCtx;

typedef struct gdIOCtx	*gdIOCtxPtr;

void Putword(int w, gdIOCtx *ctx);
void Putchar(int c, gdIOCtx *ctx);

void gdPutC(const unsigned char c, gdIOCtx *ctx);
int gdPutBuf(const void *, int, gdIOCtx*);
void gdPutWord(int w, gdIOCtx *ctx);
void gdPutInt(int w, gdIOCtx *ctx);

int gdGetC(gdIOCtx *ctx);
int gdGetBuf(void *, int, gdIOCtx*);
int gdGetByte(int *result, gdIOCtx *ctx);
int gdGetWord(int *result, gdIOCtx *ctx);
int gdGetWordLSB(signed short int *result, gdIOCtx *ctx);
int gdGetInt(int *result, gdIOCtx *ctx);
int gdGetIntLSB(signed int *result, gdIOCtx *ctx);

int gdSeek(gdIOCtx *ctx, const int);
long gdTell(gdIOCtx *ctx);

#endif
