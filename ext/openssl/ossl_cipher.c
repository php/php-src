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

#define WrapCipher(obj, klass, ctx) \
    (obj) = Data_Wrap_Struct((klass), 0, ossl_cipher_free, (ctx))
#define MakeCipher(obj, klass, ctx) \
    (obj) = Data_Make_Struct((klass), EVP_CIPHER_CTX, 0, ossl_cipher_free, (ctx))
#define AllocCipher(obj, ctx) \
    memset(DATA_PTR(obj) = (ctx) = ALLOC(EVP_CIPHER_CTX), 0, sizeof(EVP_CIPHER_CTX))
#define GetCipherInit(obj, ctx) do { \
    Data_Get_Struct((obj), EVP_CIPHER_CTX, (ctx)); \
} while (0)
#define GetCipher(obj, ctx) do { \
    GetCipherInit((obj), (ctx)); \
    if (!(ctx)) { \
	ossl_raise(rb_eRuntimeError, "Cipher not inititalized!"); \
    } \
} while (0)
#define SafeGetCipher(obj, ctx) do { \
    OSSL_Check_Kind((obj), cCipher); \
    GetCipher((obj), (ctx)); \
} while (0)

/*
 * Classes
 */
VALUE cCipher;
VALUE eCipherError;

static VALUE ossl_cipher_alloc(VALUE klass);

/*
 * PUBLIC
 */
const EVP_CIPHER *
GetCipherPtr(VALUE obj)
{
    EVP_CIPHER_CTX *ctx;

    SafeGetCipher(obj, ctx);

    return EVP_CIPHER_CTX_cipher(ctx);
}

VALUE
ossl_cipher_new(const EVP_CIPHER *cipher)
{
    VALUE ret;
    EVP_CIPHER_CTX *ctx;

    ret = ossl_cipher_alloc(cCipher);
    AllocCipher(ret, ctx);
    EVP_CIPHER_CTX_init(ctx);
    if (EVP_CipherInit_ex(ctx, cipher, NULL, NULL, NULL, -1) != 1)
	ossl_raise(eCipherError, NULL);

    return ret;
}

/*
 * PRIVATE
 */
static void
ossl_cipher_free(EVP_CIPHER_CTX *ctx)
{
    if (ctx) {
	EVP_CIPHER_CTX_cleanup(ctx);
	ruby_xfree(ctx);
    }
}

static VALUE
ossl_cipher_alloc(VALUE klass)
{
    VALUE obj;

    WrapCipher(obj, klass, 0);

    return obj;
}

/*
 *  call-seq:
 *     Cipher.new(string) -> cipher
 *
 *  The string must contain a valid cipher name like "AES-128-CBC" or "3DES".
 *
 *  A list of cipher names is available by calling OpenSSL::Cipher.ciphers.
 */
static VALUE
ossl_cipher_initialize(VALUE self, VALUE str)
{
    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER *cipher;
    char *name;
    unsigned char key[EVP_MAX_KEY_LENGTH];

    name = StringValuePtr(str);
    GetCipherInit(self, ctx);
    if (ctx) {
	ossl_raise(rb_eRuntimeError, "Cipher already inititalized!");
    }
    AllocCipher(self, ctx);
    EVP_CIPHER_CTX_init(ctx);
    if (!(cipher = EVP_get_cipherbyname(name))) {
	ossl_raise(rb_eRuntimeError, "unsupported cipher algorithm (%s)", name);
    }
    /*
     * The EVP which has EVP_CIPH_RAND_KEY flag (such as DES3) allows
     * uninitialized key, but other EVPs (such as AES) does not allow it.
     * Calling EVP_CipherUpdate() without initializing key causes SEGV so we
     * set the data filled with "\0" as the key by default.
     */
    memset(key, 0, EVP_MAX_KEY_LENGTH);
    if (EVP_CipherInit_ex(ctx, cipher, NULL, key, NULL, -1) != 1)
	ossl_raise(eCipherError, NULL);

    return self;
}

static VALUE
ossl_cipher_copy(VALUE self, VALUE other)
{
    EVP_CIPHER_CTX *ctx1, *ctx2;

    rb_check_frozen(self);
    if (self == other) return self;

    GetCipherInit(self, ctx1);
    if (!ctx1) {
	AllocCipher(self, ctx1);
    }
    SafeGetCipher(other, ctx2);
    if (EVP_CIPHER_CTX_copy(ctx1, ctx2) != 1)
	ossl_raise(eCipherError, NULL);

    return self;
}

#ifdef HAVE_OBJ_NAME_DO_ALL_SORTED
static void*
add_cipher_name_to_ary(const OBJ_NAME *name, VALUE ary)
{
    rb_ary_push(ary, rb_str_new2(name->name));
    return NULL;
}
#endif

#ifdef HAVE_OBJ_NAME_DO_ALL_SORTED
/*
 *  call-seq:
 *     Cipher.ciphers -> array[string...]
 *
 *  Returns the names of all available ciphers in an array.
 */
static VALUE
ossl_s_ciphers(VALUE self)
{
    VALUE ary;

    ary = rb_ary_new();
    OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH,
                    (void(*)(const OBJ_NAME*,void*))add_cipher_name_to_ary,
                    (void*)ary);

    return ary;
}
#else
#define ossl_s_ciphers rb_f_notimplement
#endif

/*
 *  call-seq:
 *     cipher.reset -> self
 *
 *  Fully resets the internal state of the Cipher. By using this, the same
 *  Cipher instance may be used several times for en- or decryption tasks.
 *
 *  Internally calls EVP_CipherInit_ex(ctx, NULL, NULL, NULL, NULL, -1).
 */
static VALUE
ossl_cipher_reset(VALUE self)
{
    EVP_CIPHER_CTX *ctx;

    GetCipher(self, ctx);
    if (EVP_CipherInit_ex(ctx, NULL, NULL, NULL, NULL, -1) != 1)
	ossl_raise(eCipherError, NULL);

    return self;
}

static VALUE
ossl_cipher_init(int argc, VALUE *argv, VALUE self, int mode)
{
    EVP_CIPHER_CTX *ctx;
    unsigned char key[EVP_MAX_KEY_LENGTH], *p_key = NULL;
    unsigned char iv[EVP_MAX_IV_LENGTH], *p_iv = NULL;
    VALUE pass, init_v;

    if(rb_scan_args(argc, argv, "02", &pass, &init_v) > 0){
	/*
	 * oops. this code mistakes salt for IV.
	 * We deprecated the arguments for this method, but we decided
	 * keeping this behaviour for backward compatibility.
	 */
	const char *cname  = rb_class2name(rb_obj_class(self));
	rb_warn("arguments for %s#encrypt and %s#decrypt were deprecated; "
                "use %s#pkcs5_keyivgen to derive key and IV",
                cname, cname, cname);
	StringValue(pass);
	GetCipher(self, ctx);
	if (NIL_P(init_v)) memcpy(iv, "OpenSSL for Ruby rulez!", sizeof(iv));
	else{
	    StringValue(init_v);
	    if (EVP_MAX_IV_LENGTH > RSTRING_LEN(init_v)) {
		memset(iv, 0, EVP_MAX_IV_LENGTH);
		memcpy(iv, RSTRING_PTR(init_v), RSTRING_LEN(init_v));
	    }
	    else memcpy(iv, RSTRING_PTR(init_v), sizeof(iv));
	}
	EVP_BytesToKey(EVP_CIPHER_CTX_cipher(ctx), EVP_md5(), iv,
		       (unsigned char *)RSTRING_PTR(pass), RSTRING_LENINT(pass), 1, key, NULL);
	p_key = key;
	p_iv = iv;
    }
    else {
	GetCipher(self, ctx);
    }
    if (EVP_CipherInit_ex(ctx, NULL, NULL, p_key, p_iv, mode) != 1) {
	ossl_raise(eCipherError, NULL);
    }

    return self;
}

/*
 *  call-seq:
 *     cipher.encrypt -> self
 *
 *  Initializes the Cipher for encryption.
 *
 *  Make sure to call Cipher#encrypt or Cipher#decrypt before using any of the
 *  following methods:
 *  * [key=, iv=, random_key, random_iv, pkcs5_keyivgen]
 *
 *  Internally calls EVP_CipherInit_ex(ctx, NULL, NULL, NULL, NULL, 1).
 */
static VALUE
ossl_cipher_encrypt(int argc, VALUE *argv, VALUE self)
{
    return ossl_cipher_init(argc, argv, self, 1);
}

/*
 *  call-seq:
 *     cipher.decrypt -> self
 *
 *  Initializes the Cipher for decryption.
 *
 *  Make sure to call Cipher#encrypt or Cipher#decrypt before using any of the
 *  following methods:
 *  * [key=, iv=, random_key, random_iv, pkcs5_keyivgen]
 *
 *  Internally calls EVP_CipherInit_ex(ctx, NULL, NULL, NULL, NULL, 0).
 */
static VALUE
ossl_cipher_decrypt(int argc, VALUE *argv, VALUE self)
{
    return ossl_cipher_init(argc, argv, self, 0);
}

/*
 *  call-seq:
 *     cipher.pkcs5_keyivgen(pass [, salt [, iterations [, digest]]] ) -> nil
 *
 *  Generates and sets the key/IV based on a password.
 *
 *  WARNING: This method is only PKCS5 v1.5 compliant when using RC2, RC4-40,
 *  or DES with MD5 or SHA1. Using anything else (like AES) will generate the
 *  key/iv using an OpenSSL specific method. This method is deprecated and
 *  should no longer be used. Use a PKCS5 v2 key generation method from
 *  OpenSSL::PKCS5 instead.
 *
 *  === Parameters
 *  +salt+ must be an 8 byte string if provided.
 *  +iterations+ is a integer with a default of 2048.
 *  +digest+ is a Digest object that defaults to 'MD5'
 *
 *  A minimum of 1000 iterations is recommended.
 *
 */
static VALUE
ossl_cipher_pkcs5_keyivgen(int argc, VALUE *argv, VALUE self)
{
    EVP_CIPHER_CTX *ctx;
    const EVP_MD *digest;
    VALUE vpass, vsalt, viter, vdigest;
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH], *salt = NULL;
    int iter;

    rb_scan_args(argc, argv, "13", &vpass, &vsalt, &viter, &vdigest);
    StringValue(vpass);
    if(!NIL_P(vsalt)){
	StringValue(vsalt);
	if(RSTRING_LEN(vsalt) != PKCS5_SALT_LEN)
	    ossl_raise(eCipherError, "salt must be an 8-octet string");
	salt = (unsigned char *)RSTRING_PTR(vsalt);
    }
    iter = NIL_P(viter) ? 2048 : NUM2INT(viter);
    digest = NIL_P(vdigest) ? EVP_md5() : GetDigestPtr(vdigest);
    GetCipher(self, ctx);
    EVP_BytesToKey(EVP_CIPHER_CTX_cipher(ctx), digest, salt,
		   (unsigned char *)RSTRING_PTR(vpass), RSTRING_LENINT(vpass), iter, key, iv);
    if (EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, -1) != 1)
	ossl_raise(eCipherError, NULL);
    OPENSSL_cleanse(key, sizeof key);
    OPENSSL_cleanse(iv, sizeof iv);

    return Qnil;
}


/*
 *  call-seq:
 *     cipher.update(data [, buffer]) -> string or buffer
 *
 *  Encrypts data in a streaming fashion. Hand consecutive blocks of data
 *  to the +update+ method in order to encrypt it. Returns the encrypted
 *  data chunk. When done, the output of Cipher#final should be additionally
 *  added to the result.
 *
 *  === Parameters
 *  +data+ is a nonempty string.
 *  +buffer+ is an optional string to store the result.
 */
static VALUE
ossl_cipher_update(int argc, VALUE *argv, VALUE self)
{
    EVP_CIPHER_CTX *ctx;
    unsigned char *in;
    int in_len, out_len;
    VALUE data, str;

    rb_scan_args(argc, argv, "11", &data, &str);

    StringValue(data);
    in = (unsigned char *)RSTRING_PTR(data);
    if ((in_len = RSTRING_LENINT(data)) == 0)
        ossl_raise(rb_eArgError, "data must not be empty");
    GetCipher(self, ctx);
    out_len = in_len+EVP_CIPHER_CTX_block_size(ctx);

    if (NIL_P(str)) {
        str = rb_str_new(0, out_len);
    } else {
        StringValue(str);
        rb_str_resize(str, out_len);
    }

    if (!EVP_CipherUpdate(ctx, (unsigned char *)RSTRING_PTR(str), &out_len, in, in_len))
	ossl_raise(eCipherError, NULL);
    assert(out_len < RSTRING_LEN(str));
    rb_str_set_len(str, out_len);

    return str;
}

/*
 *  call-seq:
 *     cipher.final -> string
 *
 *  Returns the remaining data held in the cipher object.  Further calls to
 *  Cipher#update or Cipher#final will return garbage.
 *
 *  See EVP_CipherFinal_ex for further information.
 */
static VALUE
ossl_cipher_final(VALUE self)
{
    EVP_CIPHER_CTX *ctx;
    int out_len;
    VALUE str;

    GetCipher(self, ctx);
    str = rb_str_new(0, EVP_CIPHER_CTX_block_size(ctx));
    if (!EVP_CipherFinal_ex(ctx, (unsigned char *)RSTRING_PTR(str), &out_len))
	ossl_raise(eCipherError, NULL);
    assert(out_len <= RSTRING_LEN(str));
    rb_str_set_len(str, out_len);

    return str;
}

/*
 *  call-seq:
 *     cipher.name -> string
 *
 *  Returns the name of the cipher which may differ slightly from the original
 *  name provided.
 */
static VALUE
ossl_cipher_name(VALUE self)
{
    EVP_CIPHER_CTX *ctx;

    GetCipher(self, ctx);

    return rb_str_new2(EVP_CIPHER_name(EVP_CIPHER_CTX_cipher(ctx)));
}

/*
 *  call-seq:
 *     cipher.key = string -> string
 *
 *  Sets the cipher key. To generate a key, you should either use a secure
 *  random byte string or, if the key is to be derived from a password, you
 *  should rely on PBKDF2 functionality provided by OpenSSL::PKCS5. To
 *  generate a secure random-based key, Cipher#random_key may be used.
 *
 *  Only call this method after calling Cipher#encrypt or Cipher#decrypt.
 */
static VALUE
ossl_cipher_set_key(VALUE self, VALUE key)
{
    EVP_CIPHER_CTX *ctx;

    StringValue(key);
    GetCipher(self, ctx);

    if (RSTRING_LEN(key) < EVP_CIPHER_CTX_key_length(ctx))
        ossl_raise(eCipherError, "key length too short");

    if (EVP_CipherInit_ex(ctx, NULL, NULL, (unsigned char *)RSTRING_PTR(key), NULL, -1) != 1)
        ossl_raise(eCipherError, NULL);

    return key;
}

/*
 *  call-seq:
 *     cipher.iv = string -> string
 *
 *  Sets the cipher IV. Please note that since you should never be using ECB
 *  mode, an IV is always explicitly required and should be set prior to
 *  encryption. The IV itself can be safely transmitted in public, but it
 *  should be unpredictable to prevent certain kinds of attacks. You may use
 *  Cipher#random_iv to create a secure random IV.
 *
 *  Only call this method after calling Cipher#encrypt or Cipher#decrypt.
 *
 *  If not explicitly set, the OpenSSL default of an all-zeroes ("\\0") IV is
 *  used.
 */
static VALUE
ossl_cipher_set_iv(VALUE self, VALUE iv)
{
    EVP_CIPHER_CTX *ctx;

    StringValue(iv);
    GetCipher(self, ctx);

    if (RSTRING_LEN(iv) < EVP_CIPHER_CTX_iv_length(ctx))
        ossl_raise(eCipherError, "iv length too short");

    if (EVP_CipherInit_ex(ctx, NULL, NULL, NULL, (unsigned char *)RSTRING_PTR(iv), -1) != 1)
	ossl_raise(eCipherError, NULL);

    return iv;
}


/*
 *  call-seq:
 *     cipher.key_len = integer -> integer
 *
 *  Sets the key length of the cipher.  If the cipher is a fixed length cipher
 *  then attempting to set the key length to any value other than the fixed
 *  value is an error.
 *
 *  Under normal circumstances you do not need to call this method (and probably shouldn't).
 *
 *  See EVP_CIPHER_CTX_set_key_length for further information.
 */
static VALUE
ossl_cipher_set_key_length(VALUE self, VALUE key_length)
{
    int len = NUM2INT(key_length);
    EVP_CIPHER_CTX *ctx;

    GetCipher(self, ctx);
    if (EVP_CIPHER_CTX_set_key_length(ctx, len) != 1)
        ossl_raise(eCipherError, NULL);

    return key_length;
}

#if defined(HAVE_EVP_CIPHER_CTX_SET_PADDING)
/*
 *  call-seq:
 *     cipher.padding = integer -> integer
 *
 *  Enables or disables padding. By default encryption operations are padded using standard block padding and the
 *  padding is checked and removed when decrypting. If the pad parameter is zero then no padding is performed, the
 *  total amount of data encrypted or decrypted must then be a multiple of the block size or an error will occur.
 *
 *  See EVP_CIPHER_CTX_set_padding for further information.
 */
static VALUE
ossl_cipher_set_padding(VALUE self, VALUE padding)
{
    EVP_CIPHER_CTX *ctx;
    int pad = NUM2INT(padding);

    GetCipher(self, ctx);
    if (EVP_CIPHER_CTX_set_padding(ctx, pad) != 1)
	ossl_raise(eCipherError, NULL);
    return padding;
}
#else
#define ossl_cipher_set_padding rb_f_notimplement
#endif

#define CIPHER_0ARG_INT(func)					\
    static VALUE						\
    ossl_cipher_##func(VALUE self)				\
    {								\
	EVP_CIPHER_CTX *ctx;					\
	GetCipher(self, ctx);					\
	return INT2NUM(EVP_CIPHER_##func(EVP_CIPHER_CTX_cipher(ctx)));	\
    }

/*
 *  call-seq:
 *     cipher.key_len -> integer
 *
 *  Returns the key length in bytes of the Cipher.
 */
CIPHER_0ARG_INT(key_length)
/*
 *  call-seq:
 *     cipher.iv_len -> integer
 *
 *  Returns the expected length in bytes for an IV for this Cipher.
 */
CIPHER_0ARG_INT(iv_length)
/*
 *  call-seq:
 *     cipher.block_size -> integer
 *
 *  Returns the size in bytes of the blocks on which this Cipher operates on.
 */
CIPHER_0ARG_INT(block_size)

/*
 * INIT
 */
void
Init_ossl_cipher(void)
{
#if 0
    mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL */
#endif

    /* Document-class: OpenSSL::Cipher
     *
     * Provides symmetric algorithms for encryption and decryption. The
     * algorithms that are available depend on the particular version
     * of OpenSSL that is installed.
     *
     * === Listing all supported algorithms
     *
     * A list of supported algorithms can be obtained by
     *
     *   puts OpenSSL::Cipher.ciphers
     *
     * === Instantiating a Cipher
     *
     * There are several ways to create a Cipher instance. Generally, a
     * Cipher algorithm is categorized by its name, the key length in bits
     * and the cipher mode to be used. The most generic way to create a
     * Cipher is the following
     *
     *   cipher = OpenSSL::Cipher.new('<name>-<key length>-<mode>')
     *
     * That is, a string consisting of the hyphenated concatenation of the
     * individual components name, key length and mode. Either all uppercase
     * or all lowercase strings may be used, for example:
     *
     *  cipher = OpenSSL::Cipher.new('AES-128-CBC')
     *
     * For each algorithm supported, there is a class defined under the
     * Cipher class that goes by the name of the cipher, e.g. to obtain an
     * instance of AES, you could also use
     *
     *   # these are equivalent
     *   cipher = OpenSSL::Cipher::AES.new(128, :CBC)
     *   cipher = OpenSSL::Cipher::AES.new(128, 'CBC')
     *   cipher = OpenSSL::Cipher::AES.new('128-CBC')
     *
     * Finally, due to its wide-spread use, there are also extra classes
     * defined for the different key sizes of AES
     *
     *   cipher = OpenSSL::Cipher::AES128.new(:CBC)
     *   cipher = OpenSSL::Cipher::AES192.new(:CBC)
     *   cipher = OpenSSL::Cipher::AES256.new(:CBC)
     *
     * === Choosing either encryption or decryption mode
     *
     * Encryption and decryption are often very similar operations for
     * symmetric algorithms, this is reflected by not having to choose
     * different classes for either operation, both can be done using the
     * same class. Still, after obtaining a Cipher instance, we need to
     * tell the instance what it is that we intend to do with it, so we
     * need to call either
     *
     *   cipher.encrypt
     *
     * or
     *
     *   cipher.decrypt
     *
     * on the Cipher instance. This should be the first call after creating
     * the instance, otherwise configuration that has already been set could
     * get lost in the process.
     *
     * === Choosing a key
     *
     * Symmetric encryption requires a key that is the same for the encrypting
     * and for the decrypting party and after initial key establishment should
     * be kept as private information. There are a lot of ways to create
     * insecure keys, the most notable is to simply take a password as the key
     * without processing the password further. A simple and secure way to
     * create a key for a particular Cipher is
     *
     *  cipher = OpenSSL::AES256.new(:CFB)
     *  cipher.encrypt
     *  key = cipher.random_key # also sets the generated key on the Cipher
     *
     * If you absolutely need to use passwords as encryption keys, you
     * should use Password-Based Key Derivation Function 2 (PBKDF2) by
     * generating the key with the help of the functionality provided by
     * OpenSSL::PKCS5.pbkdf2_hmac_sha1 or OpenSSL::PKCS5.pbkdf2_hmac.
     *
     * Although there is Cipher#pkcs5_keyivgen, its use is deprecated and
     * it should only be used in legacy applications because it does not use
     * the newer PKCS#5 v2 algorithms.
     *
     * === Choosing an IV
     *
     * The cipher modes CBC, CFB, OFB and CTR all need an "initialization
     * vector", or short, IV. ECB mode is the only mode that does not require
     * an IV, but there is almost no legitimate use case for this mode
     * because of the fact that it does not sufficiently hide plaintext
     * patterns. Therefore
     *
     * <b>You should never use ECB mode unless you are absolutely sure that
     * you absolutely need it</b>
     *
     * Because of this, you will end up with a mode that explicitly requires
     * an IV in any case. Note that for backwards compatibility reasons,
     * setting an IV is not explicitly mandated by the Cipher API. If not
     * set, OpenSSL itself defaults to an all-zeroes IV ("\\0", not the
     * character). Although the IV can be seen as public information, i.e.
     * it may be transmitted in public once generated, it should still stay
     * unpredictable to prevent certain kinds of attacks. Therefore, ideally
     *
     * <b>Always create a secure random IV for every encryption of your
     * Cipher</b>
     *
     * A new, random IV should be created for every encryption of data. Think
     * of the IV as a nonce (number used once) - it's public but random and
     * unpredictable. A secure random IV can be created as follows
     *
     *  cipher = ...
     *  cipher.encrypt
     *  key = cipher.random_key
     *  iv = cipher.random_iv # also sets the generated IV on the Cipher
     *
     *  Although the key is generally a random value, too, it is a bad choice
     *  as an IV. There are elaborate ways how an attacker can take advantage
     *  of such an IV. As a general rule of thumb, exposing the key directly
     *  or indirectly should be avoided at all cost and exceptions only be
     *  made with good reason.
     *
     * === Calling Cipher#final
     *
     * ECB (which should not be used) and CBC are both block-based modes.
     * This means that unlike for the other streaming-based modes, they
     * operate on fixed-size blocks of data, and therefore they require a
     * "finalization" step to produce or correctly decrypt the last block of
     * data by appropriately handling some form of padding. Therefore it is
     * essential to add the output of OpenSSL::Cipher#final to your
     * encryption/decryption buffer or you will end up with decryption errors
     * or truncated data.
     *
     * Although this is not really necessary for streaming-mode ciphers, it is
     * still recommended to apply the same pattern of adding the output of
     * Cipher#final there as well - it also enables you to switch between
     * modes more easily in the future.
     *
     * === Encrypting and decrypting some data
     *
     *   data = "Very, very confidential data"
     *
     *   cipher = OpenSSL::Cipher::AES.new(128, :CBC)
     *   cipher.encrypt
     *   key = cipher.random_key
     *   iv = cipher.random_iv
     *
     *   encrypted = cipher.update(data) + cipher.final
     *   ...
     *   decipher = OpenSSL::Cipher::AES.new(128, :CBC)
     *   decipher.decrypt
     *   decipher.key = key
     *   decipher.iv = iv
     *
     *   plain = decipher.update(encrypted) + decipher.final
     *
     *   puts data == plain #=> true
     *
     */
    cCipher = rb_define_class_under(mOSSL, "Cipher", rb_cObject);
    eCipherError = rb_define_class_under(cCipher, "CipherError", eOSSLError);

    rb_define_alloc_func(cCipher, ossl_cipher_alloc);
    rb_define_copy_func(cCipher, ossl_cipher_copy);
    rb_define_module_function(cCipher, "ciphers", ossl_s_ciphers, 0);
    rb_define_method(cCipher, "initialize", ossl_cipher_initialize, 1);
    rb_define_method(cCipher, "reset", ossl_cipher_reset, 0);
    rb_define_method(cCipher, "encrypt", ossl_cipher_encrypt, -1);
    rb_define_method(cCipher, "decrypt", ossl_cipher_decrypt, -1);
    rb_define_method(cCipher, "pkcs5_keyivgen", ossl_cipher_pkcs5_keyivgen, -1);
    rb_define_method(cCipher, "update", ossl_cipher_update, -1);
    rb_define_method(cCipher, "final", ossl_cipher_final, 0);
    rb_define_method(cCipher, "name", ossl_cipher_name, 0);
    rb_define_method(cCipher, "key=", ossl_cipher_set_key, 1);
    rb_define_method(cCipher, "key_len=", ossl_cipher_set_key_length, 1);
    rb_define_method(cCipher, "key_len", ossl_cipher_key_length, 0);
    rb_define_method(cCipher, "iv=", ossl_cipher_set_iv, 1);
    rb_define_method(cCipher, "iv_len", ossl_cipher_iv_length, 0);
    rb_define_method(cCipher, "block_size", ossl_cipher_block_size, 0);
    rb_define_method(cCipher, "padding=", ossl_cipher_set_padding, 1);
}

