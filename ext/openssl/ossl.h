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
#if !defined(_OSSL_H_)
#define _OSSL_H_

#include RUBY_EXTCONF_H

#if defined(__cplusplus)
extern "C" {
#endif

#if 0
  mOSSL = rb_define_module("OpenSSL");
  mX509 = rb_define_module_under(mOSSL, "X509");
#endif

/*
* OpenSSL has defined RFILE and Ruby has defined RFILE - so undef it!
*/
#if defined(RFILE) /*&& !defined(OSSL_DEBUG)*/
#  undef RFILE
#endif
#include <ruby.h>
#include <ruby/io.h>

/*
 * Check the OpenSSL version
 * The only supported are:
 * 	OpenSSL >= 0.9.7
 */
#include <openssl/opensslv.h>

#ifdef HAVE_ASSERT_H
#  include <assert.h>
#else
#  define assert(condition)
#endif

#if defined(_WIN32)
#  include <openssl/e_os2.h>
#  define OSSL_NO_CONF_API 1
#  if !defined(OPENSSL_SYS_WIN32)
#    define OPENSSL_SYS_WIN32 1
#  endif
#  include <winsock2.h>
#endif
#include <errno.h>
#include <openssl/err.h>
#include <openssl/asn1_mac.h>
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/conf.h>
#include <openssl/conf_api.h>
#undef X509_NAME
#undef PKCS7_SIGNER_INFO
#if defined(HAVE_OPENSSL_ENGINE_H) && defined(HAVE_ST_ENGINE)
#  define OSSL_ENGINE_ENABLED
#  include <openssl/engine.h>
#endif
#if defined(HAVE_OPENSSL_OCSP_H)
#  define OSSL_OCSP_ENABLED
#  include <openssl/ocsp.h>
#endif

/*
 * Common Module
 */
extern VALUE mOSSL;

/*
 * Common Error Class
 */
extern VALUE eOSSLError;

/*
 * CheckTypes
 */
#define OSSL_Check_Kind(obj, klass) do {\
  if (!rb_obj_is_kind_of((obj), (klass))) {\
    ossl_raise(rb_eTypeError, "wrong argument (%s)! (Expected kind of %s)",\
               rb_obj_classname(obj), rb_class2name(klass));\
  }\
} while (0)

#define OSSL_Check_Instance(obj, klass) do {\
  if (!rb_obj_is_instance_of((obj), (klass))) {\
    ossl_raise(rb_eTypeError, "wrong argument (%s)! (Expected instance of %s)",\
               rb_obj_classname(obj), rb_class2name(klass));\
  }\
} while (0)

#define OSSL_Check_Same_Class(obj1, obj2) do {\
  if (!rb_obj_is_instance_of((obj1), rb_obj_class(obj2))) {\
    ossl_raise(rb_eTypeError, "wrong argument type");\
  }\
} while (0)

/*
 * Compatibility
 */
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
#define STACK _STACK
#endif

/*
 * String to HEXString conversion
 */
int string2hex(const unsigned char *, int, char **, int *);

/*
 * Data Conversion
 */
STACK_OF(X509) *ossl_x509_ary2sk0(VALUE);
STACK_OF(X509) *ossl_x509_ary2sk(VALUE);
STACK_OF(X509) *ossl_protect_x509_ary2sk(VALUE,int*);
VALUE ossl_x509_sk2ary(STACK_OF(X509) *certs);
VALUE ossl_x509crl_sk2ary(STACK_OF(X509_CRL) *crl);
VALUE ossl_x509name_sk2ary(STACK_OF(X509_NAME) *names);
VALUE ossl_buf2str(char *buf, int len);
#define ossl_str_adjust(str, p) \
do{\
    int len = RSTRING_LENINT(str);\
    int newlen = rb_long2int((p) - (unsigned char*)RSTRING_PTR(str));\
    assert(newlen <= len);\
    rb_str_set_len((str), newlen);\
}while(0)

/*
 * our default PEM callback
 */
int ossl_pem_passwd_cb(char *, int, int, void *);

/*
 * Clear BIO* with this in PEM/DER fallback scenarios to avoid decoding
 * errors piling up in OpenSSL::Errors
 */
#define OSSL_BIO_reset(bio)	(void)BIO_reset((bio)); \
				ERR_clear_error();

/*
 * ERRor messages
 */
#define OSSL_ErrMsg() ERR_reason_error_string(ERR_get_error())
NORETURN(void ossl_raise(VALUE, const char *, ...));
VALUE ossl_exc_new(VALUE, const char *, ...);

/*
 * Verify callback
 */
extern int ossl_verify_cb_idx;

struct ossl_verify_cb_args {
    VALUE proc;
    VALUE preverify_ok;
    VALUE store_ctx;
};

VALUE ossl_call_verify_cb_proc(struct ossl_verify_cb_args *);
int ossl_verify_cb(int, X509_STORE_CTX *);

/*
 * String to DER String
 */
extern ID ossl_s_to_der;
VALUE ossl_to_der(VALUE);
VALUE ossl_to_der_if_possible(VALUE);

/*
 * Debug
 */
extern VALUE dOSSL;

#if defined(HAVE_VA_ARGS_MACRO)
#define OSSL_Debug(...) do { \
  if (dOSSL == Qtrue) { \
    fprintf(stderr, "OSSL_DEBUG: "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, " [%s:%d]\n", __FILE__, __LINE__); \
  } \
} while (0)

#define OSSL_Warning(fmt, ...) do { \
  OSSL_Debug((fmt), ##__VA_ARGS__); \
  rb_warning((fmt), ##__VA_ARGS__); \
} while (0)

#define OSSL_Warn(fmt, ...) do { \
  OSSL_Debug((fmt), ##__VA_ARGS__); \
  rb_warn((fmt), ##__VA_ARGS__); \
} while (0)
#else
void ossl_debug(const char *, ...);
#define OSSL_Debug ossl_debug
#define OSSL_Warning rb_warning
#define OSSL_Warn rb_warn
#endif

/*
 * Include all parts
 */
#include "openssl_missing.h"
#include "ruby_missing.h"
#include "ossl_asn1.h"
#include "ossl_bio.h"
#include "ossl_bn.h"
#include "ossl_cipher.h"
#include "ossl_config.h"
#include "ossl_digest.h"
#include "ossl_hmac.h"
#include "ossl_ns_spki.h"
#include "ossl_ocsp.h"
#include "ossl_pkcs12.h"
#include "ossl_pkcs7.h"
#include "ossl_pkcs5.h"
#include "ossl_pkey.h"
#include "ossl_rand.h"
#include "ossl_ssl.h"
#include "ossl_version.h"
#include "ossl_x509.h"
#include "ossl_engine.h"

void Init_openssl(void);

#if defined(__cplusplus)
}
#endif

#endif /* _OSSL_H_ */

