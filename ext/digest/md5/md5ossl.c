/* $Id$ */

#include "md5ossl.h"

void
MD5_Finish(MD5_CTX *pctx, unsigned char *digest)
{
    MD5_Final(digest, pctx);
}
