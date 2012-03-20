/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2002  Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(_OSSL_BN_H_)
#define _OSSL_BN_H_

extern VALUE cBN;
extern VALUE eBNError;

extern BN_CTX *ossl_bn_ctx;

VALUE ossl_bn_new(const BIGNUM *);
BIGNUM *GetBNPtr(VALUE);
void Init_ossl_bn(void);


#endif /* _OSS_BN_H_ */

