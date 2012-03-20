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
#include "ossl.h"

VALUE mX509;

#define DefX509Const(x) rb_define_const(mX509, #x,INT2FIX(X509_##x))
#define DefX509Default(x,i) \
  rb_define_const(mX509, "DEFAULT_" #x, rb_str_new2(X509_get_default_##i()))

void
Init_ossl_x509()
{
    mX509 = rb_define_module_under(mOSSL, "X509");

    Init_ossl_x509attr();
    Init_ossl_x509cert();
    Init_ossl_x509crl();
    Init_ossl_x509ext();
    Init_ossl_x509name();
    Init_ossl_x509req();
    Init_ossl_x509revoked();
    Init_ossl_x509store();

    DefX509Const(V_OK);
    DefX509Const(V_ERR_UNABLE_TO_GET_ISSUER_CERT);
    DefX509Const(V_ERR_UNABLE_TO_GET_CRL);
    DefX509Const(V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE);
    DefX509Const(V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE);
    DefX509Const(V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY);
    DefX509Const(V_ERR_CERT_SIGNATURE_FAILURE);
    DefX509Const(V_ERR_CRL_SIGNATURE_FAILURE);
    DefX509Const(V_ERR_CERT_NOT_YET_VALID);
    DefX509Const(V_ERR_CERT_HAS_EXPIRED);
    DefX509Const(V_ERR_CRL_NOT_YET_VALID);
    DefX509Const(V_ERR_CRL_HAS_EXPIRED);
    DefX509Const(V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD);
    DefX509Const(V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD);
    DefX509Const(V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD);
    DefX509Const(V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD);
    DefX509Const(V_ERR_OUT_OF_MEM);
    DefX509Const(V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT);
    DefX509Const(V_ERR_SELF_SIGNED_CERT_IN_CHAIN);
    DefX509Const(V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY);
    DefX509Const(V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE);
    DefX509Const(V_ERR_CERT_CHAIN_TOO_LONG);
    DefX509Const(V_ERR_CERT_REVOKED);
    DefX509Const(V_ERR_INVALID_CA);
    DefX509Const(V_ERR_PATH_LENGTH_EXCEEDED);
    DefX509Const(V_ERR_INVALID_PURPOSE);
    DefX509Const(V_ERR_CERT_UNTRUSTED);
    DefX509Const(V_ERR_CERT_REJECTED);
    DefX509Const(V_ERR_SUBJECT_ISSUER_MISMATCH);
    DefX509Const(V_ERR_AKID_SKID_MISMATCH);
    DefX509Const(V_ERR_AKID_ISSUER_SERIAL_MISMATCH);
    DefX509Const(V_ERR_KEYUSAGE_NO_CERTSIGN);
    DefX509Const(V_ERR_APPLICATION_VERIFICATION);

#if defined(X509_V_FLAG_CRL_CHECK)
    DefX509Const(V_FLAG_CRL_CHECK);
#endif
#if defined(X509_V_FLAG_CRL_CHECK_ALL)
    DefX509Const(V_FLAG_CRL_CHECK_ALL);
#endif

    DefX509Const(PURPOSE_SSL_CLIENT);
    DefX509Const(PURPOSE_SSL_SERVER);
    DefX509Const(PURPOSE_NS_SSL_SERVER);
    DefX509Const(PURPOSE_SMIME_SIGN);
    DefX509Const(PURPOSE_SMIME_ENCRYPT);
    DefX509Const(PURPOSE_CRL_SIGN);
    DefX509Const(PURPOSE_ANY);
#if defined(X509_PURPOSE_OCSP_HELPER)
    DefX509Const(PURPOSE_OCSP_HELPER);
#endif

    DefX509Const(TRUST_COMPAT);
    DefX509Const(TRUST_SSL_CLIENT);
    DefX509Const(TRUST_SSL_SERVER);
    DefX509Const(TRUST_EMAIL);
    DefX509Const(TRUST_OBJECT_SIGN);
#if defined(X509_TRUST_OCSP_SIGN)
    DefX509Const(TRUST_OCSP_SIGN);
#endif
#if defined(X509_TRUST_OCSP_REQUEST)
    DefX509Const(TRUST_OCSP_REQUEST);
#endif

    DefX509Default(CERT_AREA, cert_area);
    DefX509Default(CERT_DIR, cert_dir);
    DefX509Default(CERT_FILE, cert_file);
    DefX509Default(CERT_DIR_ENV, cert_dir_env);
    DefX509Default(CERT_FILE_ENV, cert_file_env);
    DefX509Default(PRIVATE_DIR, private_dir);
}

