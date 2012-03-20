#ifndef SHA2OSSL_H_INCLUDED
#define SHA2OSSL_H_INCLUDED

#include <stddef.h>
#include <openssl/sha.h>

#define SHA256_BLOCK_LENGTH	SHA256_CBLOCK
#define SHA384_BLOCK_LENGTH	SHA512_CBLOCK
#define SHA512_BLOCK_LENGTH	SHA512_CBLOCK

typedef SHA512_CTX SHA384_CTX;

void SHA256_Finish(SHA256_CTX *ctx, char *buf);
void SHA384_Finish(SHA384_CTX *ctx, char *buf);
void SHA512_Finish(SHA512_CTX *ctx, char *buf);

#endif
