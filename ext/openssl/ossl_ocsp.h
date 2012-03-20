/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2003  Michal Rokos <m.rokos@sh.cvut.cz>
 * Copyright (C) 2003  GOTOU Yuuzou <gotoyuzo@notwork.org>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(_OSSL_OCSP_H_)
#define _OSSL_OCSP_H_

#if defined(OSSL_OCSP_ENABLED)
extern VALUE mOCSP;
extern VALUE cOPCSReq;
extern VALUE cOPCSRes;
extern VALUE cOPCSBasicRes;
#endif

void Init_ossl_ocsp(void);

#endif /* _OSSL_OCSP_H_ */
