/*
 * $Id$
 * Copyright (C) 2007 Technorama Ltd. <oss-ruby@technorama.net>
 */
#include "ossl.h"

VALUE mPKCS5;
VALUE ePKCS5;

#ifdef HAVE_PKCS5_PBKDF2_HMAC
/*
 * call-seq:
 *    PKCS5.pbkdf2_hmac(pass, salt, iter, keylen, digest) => string
 *
 * === Parameters
 * * +pass+ - string
 * * +salt+ - string - should be at least 8 bytes long.
 * * +iter+ - integer - should be greater than 1000.  20000 is better.
 * * +keylen+ - integer
 * * +digest+ - a string or OpenSSL::Digest object.
 *
 * Available in OpenSSL 0.9.4.
 *
 * Digests other than SHA1 may not be supported by other cryptography libraries.
 */
static VALUE
ossl_pkcs5_pbkdf2_hmac(VALUE self, VALUE pass, VALUE salt, VALUE iter, VALUE keylen, VALUE digest)
{
    VALUE str;
    const EVP_MD *md;
    int len = NUM2INT(keylen);

    StringValue(pass);
    StringValue(salt);
    md = GetDigestPtr(digest);

    str = rb_str_new(0, len);

    if (PKCS5_PBKDF2_HMAC(RSTRING_PTR(pass), RSTRING_LEN(pass),
			  (unsigned char *)RSTRING_PTR(salt), RSTRING_LEN(salt),
			  NUM2INT(iter), md, len,
			  (unsigned char *)RSTRING_PTR(str)) != 1)
        ossl_raise(ePKCS5, "PKCS5_PBKDF2_HMAC");

    return str;
}
#else
#define ossl_pkcs5_pbkdf2_hmac rb_f_notimplement
#endif


#ifdef HAVE_PKCS5_PBKDF2_HMAC_SHA1
/*
 * call-seq:
 *    PKCS5.pbkdf2_hmac_sha1(pass, salt, iter, keylen) => string
 *
 * === Parameters
 * * +pass+ - string
 * * +salt+ - string - should be at least 8 bytes long.
 * * +iter+ - integer - should be greater than 1000.  20000 is better.
 * * +keylen+ - integer
 *
 * This method is available in almost any version of OpenSSL.
 *
 * Conforms to rfc2898.
 */
static VALUE
ossl_pkcs5_pbkdf2_hmac_sha1(VALUE self, VALUE pass, VALUE salt, VALUE iter, VALUE keylen)
{
    VALUE str;
    int len = NUM2INT(keylen);

    StringValue(pass);
    StringValue(salt);

    str = rb_str_new(0, len);

    if (PKCS5_PBKDF2_HMAC_SHA1(RSTRING_PTR(pass), RSTRING_LENINT(pass),
			       (const unsigned char *)RSTRING_PTR(salt), RSTRING_LENINT(salt), NUM2INT(iter),
			       len, (unsigned char *)RSTRING_PTR(str)) != 1)
        ossl_raise(ePKCS5, "PKCS5_PBKDF2_HMAC_SHA1");

    return str;
}
#else
#define ossl_pkcs5_pbkdf2_hmac_sha1 rb_f_notimplement
#endif

void
Init_ossl_pkcs5()
{
    /*
     * Password-based Encryption
     *
     */

    #if 0
	mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL */
    #endif

    /* Document-class: OpenSSL::PKCS5
     *
     * Provides password-based encryption functionality based on PKCS#5.
     * Typically used for securely deriving arbitrary length symmetric keys
     * to be used with an OpenSSL::Cipher from passwords. Another use case
     * is for storing passwords: Due to the ability to tweak the effort of
     * computation by increasing the iteration count, computation can be
     * slowed down artificially in order to render possible attacks infeasible.
     *
     * PKCS5 offers support for PBKDF2 with an OpenSSL::Digest::SHA1-based
     * HMAC, or an arbitrary Digest if the underlying version of OpenSSL
     * already supports it (>= 0.9.4).
     *
     * === Parameters
     * ==== Password
     * Typically an arbitrary String that represents the password to be used
     * for deriving a key.
     * ==== Salt
     * Prevents attacks based on dictionaries of common passwords. It is a
     * public value that can be safely stored along with the password (e.g.
     * if PBKDF2 is used for password storage). For maximum security, a fresh,
     * random salt should be generated for each stored password. According
     * to PKCS#5, a salt should be at least 8 bytes long.
     * ==== Iteration Count
     * Allows to tweak the length that the actual computation will take. The
     * larger the iteration count, the longer it will take.
     * ==== Key Length
     * Specifies the length in bytes of the output that will be generated.
     * Typically, the key length should be larger than or equal to the output
     * length of the underlying digest function, otherwise an attacker could
     * simply try to brute-force the key. According to PKCS#5, security is
     * limited by the output length of the underlying digest function, i.e.
     * security is not improved if a key length strictly larger than the
     * digest output length is chosen. Therefore, when using PKCS5 for
     * password storage, it suffices to store values equal to the digest
     * output length, nothing is gained by storing larger values.
     *
     * == Examples
     * === Generating a 128 bit key for a Cipher (e.g. AES)
     *   pass = "secret"
     *   salt = OpenSSL::Random.random_bytes(16)
     *   iter = 20000
     *   key_len = 16
     *   key = OpenSSL::PKCS5.pbkdf2_hmac_sha1(pass, salt, iter, key_len)
     *
     * === Storing Passwords
     *   pass = "secret"
     *   salt = OpenSSL::Random.random_bytes(16) #store this with the generated value
     *   iter = 20000
     *   digest = OpenSSL::Digest::SHA256.new
     *   len = digest.digest_length
     *   #the final value to be stored
     *   value = OpenSSL::PKCS5.pbkdf2_hmac(pass, salt, iter, len, digest)
     *
     * === Important Note on Checking Passwords
     * When comparing passwords provided by the user with previously stored
     * values, a common mistake made is comparing the two values using "==".
     * Typically, "==" short-circuits on evaluation, and is therefore
     * vulnerable to timing attacks. The proper way is to use a method that
     * always takes the same amount of time when comparing two values, thus
     * not leaking any information to potential attackers. To compare two
     * values, the following could be used:
     *   def eql_time_cmp(a, b)
     *     unless a.length == b.length
     *       return false
     *     end
     *     cmp = b.bytes.to_a
     *     result = 0
     *     a.bytes.each_with_index {|c,i|
     *       result |= c ^ cmp[i]
     *     }
     *     result == 0
     *   end
     * Please note that the premature return in case of differing lengths
     * typically does not leak valuable information - when using PKCS#5, the
     * length of the values to be compared is of fixed size.
     */

    mPKCS5 = rb_define_module_under(mOSSL, "PKCS5");
    /* Document-class: OpenSSL::PKCS5::PKCS5Error
     *
     * Generic Exception class that is raised if an error occurs during a
     * computation.
     */
    ePKCS5 = rb_define_class_under(mPKCS5, "PKCS5Error", eOSSLError);

    rb_define_module_function(mPKCS5, "pbkdf2_hmac", ossl_pkcs5_pbkdf2_hmac, 5);
    rb_define_module_function(mPKCS5, "pbkdf2_hmac_sha1", ossl_pkcs5_pbkdf2_hmac_sha1, 4);
}
