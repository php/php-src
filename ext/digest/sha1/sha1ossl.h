/* $Id$ */

#ifndef SHA1OSSL_H_INCLUDED
#define SHA1OSSL_H_INCLUDED

#include <stddef.h>
#include <openssl/sha.h>

#define SHA1_CTX	SHA_CTX

#ifdef SHA_BLOCK_LENGTH
#define SHA1_BLOCK_LENGTH	SHA_BLOCK_LENGTH
#else
#define SHA1_BLOCK_LENGTH	SHA_CBLOCK
#endif
#define SHA1_DIGEST_LENGTH	SHA_DIGEST_LENGTH

void SHA1_Finish(SHA1_CTX *ctx, char *buf);

#endif
