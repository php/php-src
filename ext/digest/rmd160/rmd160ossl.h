/* $Id$ */

#ifndef RMD160OSSL_H_INCLUDED
#define RMD160OSSL_H_INCLUDED

#include <stddef.h>
#include <openssl/ripemd.h>

#define RMD160_CTX	RIPEMD160_CTX

#define RMD160_Init	RIPEMD160_Init
#define RMD160_Update	RIPEMD160_Update

#define RMD160_BLOCK_LENGTH		RIPEMD160_CBLOCK
#define RMD160_DIGEST_LENGTH		RIPEMD160_DIGEST_LENGTH

void RMD160_Finish(RMD160_CTX *ctx, char *buf);

#endif
