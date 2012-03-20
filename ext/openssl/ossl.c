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
#include <stdarg.h> /* for ossl_raise */

/*
 * String to HEXString conversion
 */
int
string2hex(const unsigned char *buf, int buf_len, char **hexbuf, int *hexbuf_len)
{
    static const char hex[]="0123456789abcdef";
    int i, len = 2 * buf_len;

    if (buf_len < 0 || len < buf_len) { /* PARANOIA? */
	return -1;
    }
    if (!hexbuf) { /* if no buf, return calculated len */
	if (hexbuf_len) {
	    *hexbuf_len = len;
	}
	return len;
    }
    if (!(*hexbuf = OPENSSL_malloc(len + 1))) {
	return -1;
    }
    for (i = 0; i < buf_len; i++) {
	(*hexbuf)[2 * i] = hex[((unsigned char)buf[i]) >> 4];
	(*hexbuf)[2 * i + 1] = hex[buf[i] & 0x0f];
    }
    (*hexbuf)[2 * i] = '\0';

    if (hexbuf_len) {
	*hexbuf_len = len;
    }
    return len;
}

/*
 * Data Conversion
 */
#define OSSL_IMPL_ARY2SK(name, type, expected_class, dup)	\
STACK_OF(type) *						\
ossl_##name##_ary2sk0(VALUE ary)				\
{								\
    STACK_OF(type) *sk;						\
    VALUE val;							\
    type *x;							\
    int i;							\
    								\
    Check_Type(ary, T_ARRAY);					\
    sk = sk_##type##_new_null();				\
    if (!sk) ossl_raise(eOSSLError, NULL);			\
    								\
    for (i = 0; i < RARRAY_LEN(ary); i++) {			\
	val = rb_ary_entry(ary, i);				\
	if (!rb_obj_is_kind_of(val, expected_class)) {		\
	    sk_##type##_pop_free(sk, type##_free);		\
	    ossl_raise(eOSSLError, "object in array not"	\
		       " of class ##type##");			\
	}							\
	x = dup(val); /* NEED TO DUP */				\
	sk_##type##_push(sk, x);				\
    }								\
    return sk;							\
}								\
								\
STACK_OF(type) *						\
ossl_protect_##name##_ary2sk(VALUE ary, int *status)		\
{								\
    return (STACK_OF(type)*)rb_protect(				\
	    (VALUE(*)_((VALUE)))ossl_##name##_ary2sk0,		\
	    ary,						\
	    status);						\
}								\
								\
STACK_OF(type) *						\
ossl_##name##_ary2sk(VALUE ary)					\
{								\
    STACK_OF(type) *sk;						\
    int status = 0;						\
    								\
    sk = ossl_protect_##name##_ary2sk(ary, &status);		\
    if (status) rb_jump_tag(status);				\
								\
    return sk;							\
}
OSSL_IMPL_ARY2SK(x509, X509, cX509Cert, DupX509CertPtr)

#define OSSL_IMPL_SK2ARY(name, type)	        \
VALUE						\
ossl_##name##_sk2ary(STACK_OF(type) *sk)	\
{						\
    type *t;					\
    int i, num;					\
    VALUE ary;					\
						\
    if (!sk) {					\
	OSSL_Debug("empty sk!");		\
	return Qnil;				\
    }						\
    num = sk_##type##_num(sk);			\
    if (num < 0) {				\
	OSSL_Debug("items in sk < -1???");	\
	return rb_ary_new();			\
    }						\
    ary = rb_ary_new2(num);			\
						\
    for (i=0; i<num; i++) {			\
	t = sk_##type##_value(sk, i);		\
	rb_ary_push(ary, ossl_##name##_new(t));	\
    }						\
    return ary;					\
}
OSSL_IMPL_SK2ARY(x509, X509)
OSSL_IMPL_SK2ARY(x509crl, X509_CRL)
OSSL_IMPL_SK2ARY(x509name, X509_NAME)

static VALUE
ossl_str_new(int size)
{
    return rb_str_new(0, size);
}

VALUE
ossl_buf2str(char *buf, int len)
{
    VALUE str;
    int status = 0;

    str = rb_protect((VALUE(*)_((VALUE)))ossl_str_new, len, &status);
    if(!NIL_P(str)) memcpy(RSTRING_PTR(str), buf, len);
    OPENSSL_free(buf);
    if(status) rb_jump_tag(status);

    return str;
}

/*
 * our default PEM callback
 */
static VALUE
ossl_pem_passwd_cb0(VALUE flag)
{
    VALUE pass;

    pass = rb_yield(flag);
    SafeStringValue(pass);

    return pass;
}

int
ossl_pem_passwd_cb(char *buf, int max_len, int flag, void *pwd)
{
    int len, status = 0;
    VALUE rflag, pass;

    if (pwd || !rb_block_given_p())
	return PEM_def_callback(buf, max_len, flag, pwd);

    while (1) {
	/*
	 * when the flag is nonzero, this passphrase
	 * will be used to perform encryption; otherwise it will
	 * be used to perform decryption.
	 */
	rflag = flag ? Qtrue : Qfalse;
	pass  = rb_protect(ossl_pem_passwd_cb0, rflag, &status);
	if (status) {
	    /* ignore an exception raised. */
	    rb_set_errinfo(Qnil);
	    return -1;
	}
	len = RSTRING_LENINT(pass);
	if (len < 4) { /* 4 is OpenSSL hardcoded limit */
	    rb_warning("password must be longer than 4 bytes");
	    continue;
	}
	if (len > max_len) {
	    rb_warning("password must be shorter then %d bytes", max_len-1);
	    continue;
	}
	memcpy(buf, RSTRING_PTR(pass), len);
	break;
    }
    return len;
}

/*
 * Verify callback
 */
int ossl_verify_cb_idx;

VALUE
ossl_call_verify_cb_proc(struct ossl_verify_cb_args *args)
{
    return rb_funcall(args->proc, rb_intern("call"), 2,
                      args->preverify_ok, args->store_ctx);
}

int
ossl_verify_cb(int ok, X509_STORE_CTX *ctx)
{
    VALUE proc, rctx, ret;
    struct ossl_verify_cb_args args;
    int state = 0;

    proc = (VALUE)X509_STORE_CTX_get_ex_data(ctx, ossl_verify_cb_idx);
    if ((void*)proc == 0)
	proc = (VALUE)X509_STORE_get_ex_data(ctx->ctx, ossl_verify_cb_idx);
    if ((void*)proc == 0)
	return ok;
    if (!NIL_P(proc)) {
	ret = Qfalse;
	rctx = rb_protect((VALUE(*)(VALUE))ossl_x509stctx_new,
			  (VALUE)ctx, &state);
	if (state) {
	    rb_set_errinfo(Qnil);
	    rb_warn("StoreContext initialization failure");
	}
	else {
	    args.proc = proc;
	    args.preverify_ok = ok ? Qtrue : Qfalse;
	    args.store_ctx = rctx;
	    ret = rb_protect((VALUE(*)(VALUE))ossl_call_verify_cb_proc, (VALUE)&args, &state);
	    if (state) {
		rb_set_errinfo(Qnil);
		rb_warn("exception in verify_callback is ignored");
	    }
	    ossl_x509stctx_clear_ptr(rctx);
	}
	if (ret == Qtrue) {
	    X509_STORE_CTX_set_error(ctx, X509_V_OK);
	    ok = 1;
	}
	else{
	    if (X509_STORE_CTX_get_error(ctx) == X509_V_OK) {
		X509_STORE_CTX_set_error(ctx, X509_V_ERR_CERT_REJECTED);
	    }
	    ok = 0;
	}
    }

    return ok;
}

/*
 * main module
 */
VALUE mOSSL;

/*
 * OpenSSLError < StandardError
 */
VALUE eOSSLError;

/*
 * Convert to DER string
 */
ID ossl_s_to_der;

VALUE
ossl_to_der(VALUE obj)
{
    VALUE tmp;

    tmp = rb_funcall(obj, ossl_s_to_der, 0);
    StringValue(tmp);

    return tmp;
}

VALUE
ossl_to_der_if_possible(VALUE obj)
{
    if(rb_respond_to(obj, ossl_s_to_der))
	return ossl_to_der(obj);
    return obj;
}

/*
 * Errors
 */
static VALUE
ossl_make_error(VALUE exc, const char *fmt, va_list args)
{
    char buf[BUFSIZ];
    const char *msg;
    long e;
    int len = 0;

#ifdef HAVE_ERR_PEEK_LAST_ERROR
    e = ERR_peek_last_error();
#else
    e = ERR_peek_error();
#endif
    if (fmt) {
	len = vsnprintf(buf, BUFSIZ, fmt, args);
    }
    if (len < BUFSIZ && e) {
	if (dOSSL == Qtrue) /* FULL INFO */
	    msg = ERR_error_string(e, NULL);
	else
	    msg = ERR_reason_error_string(e);
	len += snprintf(buf+len, BUFSIZ-len, "%s%s", (len ? ": " : ""), msg);
    }
    if (dOSSL == Qtrue){ /* show all errors on the stack */
	while ((e = ERR_get_error()) != 0){
	    rb_warn("error on stack: %s", ERR_error_string(e, NULL));
	}
    }
    ERR_clear_error();

    if(len > BUFSIZ) len = rb_long2int(strlen(buf));
    return rb_exc_new(exc, buf, len);
}

void
ossl_raise(VALUE exc, const char *fmt, ...)
{
    va_list args;
    VALUE err;
    va_start(args, fmt);
    err = ossl_make_error(exc, fmt, args);
    va_end(args);
    rb_exc_raise(err);
}

VALUE
ossl_exc_new(VALUE exc, const char *fmt, ...)
{
    va_list args;
    VALUE err;
    va_start(args, fmt);
    err = ossl_make_error(exc, fmt, args);
    va_end(args);
    return err;
}

/*
 * call-seq:
 *   OpenSSL.errors -> [String...]
 *
 * See any remaining errors held in queue.
 *
 * Any errors you see here are probably due to a bug in ruby's OpenSSL implementation.
 */
VALUE
ossl_get_errors()
{
    VALUE ary;
    long e;

    ary = rb_ary_new();
    while ((e = ERR_get_error()) != 0){
        rb_ary_push(ary, rb_str_new2(ERR_error_string(e, NULL)));
    }

    return ary;
}

/*
 * Debug
 */
VALUE dOSSL;

#if !defined(HAVE_VA_ARGS_MACRO)
void
ossl_debug(const char *fmt, ...)
{
    va_list args;

    if (dOSSL == Qtrue) {
	fprintf(stderr, "OSSL_DEBUG: ");
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, " [CONTEXT N/A]\n");
    }
}
#endif

/*
 * call-seq:
 *   OpenSSL.debug -> true | false
 */
static VALUE
ossl_debug_get(VALUE self)
{
    return dOSSL;
}

/*
 * call-seq:
 *   OpenSSL.debug = boolean -> boolean
 *
 * Turns on or off CRYPTO_MEM_CHECK.
 * Also shows some debugging message on stderr.
 */
static VALUE
ossl_debug_set(VALUE self, VALUE val)
{
    VALUE old = dOSSL;
    dOSSL = val;

    if (old != dOSSL) {
	if (dOSSL == Qtrue) {
	    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
	    fprintf(stderr, "OSSL_DEBUG: IS NOW ON!\n");
	} else if (old == Qtrue) {
	    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_OFF);
	    fprintf(stderr, "OSSL_DEBUG: IS NOW OFF!\n");
	}
    }
    return val;
}

/*
 * OpenSSL provides SSL, TLS and general purpose cryptography.  It wraps the
 * OpenSSL[http://www.openssl.org/] library.
 *
 * = Examples
 *
 * All examples assume you have loaded OpenSSL with:
 *
 *   require 'openssl'
 *
 * These examples build atop each other.  For example the key created in the
 * next is used in throughout these examples.
 *
 * == Keys
 *
 * === Creating a Key
 *
 * This example creates a 2048 bit RSA keypair and writes it to the current
 * directory.
 *
 *   key = OpenSSL::PKey::RSA.new 2048
 *
 *   open 'private_key.pem', 'w' do |io| io.write key.to_pem end
 *   open 'public_key.pem', 'w' do |io| io.write key.public_key.to_pem end
 *
 * === Exporting a Key
 *
 * Keys saved to disk without encryption are not secure as anyone who gets
 * ahold of the key may use it unless it is encrypted.  In order to securely
 * export a key you may export it with a pass phrase.
 *
 *   cipher = OpenSSL::Cipher::Cipher.new 'AES-128-CBC'
 *   pass_phrase = 'my secure pass phrase goes here'
 *
 *   key_secure = key.export cipher, pass_phrase
 *
 *   open 'private.secure.pem', 'w' do |io|
 *     io.write key_secure
 *   end
 *
 * OpenSSL::Cipher.ciphers returns a list of available ciphers.
 *
 * === Loading a Key
 *
 * A key can also be loaded from a file.
 *
 *   key2 = OpenSSL::PKey::RSA.new File.read 'private_key.pem'
 *   key2.public? # => true
 *
 * or
 *
 *   key3 = OpenSSL::PKey::RSA.new File.read 'public_key.pem'
 *   key3.private? # => false
 *
 * === Loading an Encrypted Key
 *
 * OpenSSL will prompt you for your pass phrase when loading an encrypted key.
 * If you will not be able to type in the pass phrase you may provide it when
 * loading the key:
 *
 *   key4_pem = File.read 'private.secure.pem'
 *   key4 = OpenSSL::PKey::RSA.new key4_pem, pass_phrase
 *
 * == RSA Encryption
 *
 * RSA provides ecryption and decryption using the public and private keys.
 * You can use a variety of padding methods depending upon the intended use of
 * encrypted data.
 *
 * === Encryption
 *
 * Documents encrypted with the public key can only be decrypted with the
 * private key.
 *
 *   public_encrypted = key.public_encrypt 'top secret document'
 *
 * Documents encrypted with the private key can only be decrypted with the
 * public key.
 *
 *   private_encrypted = key.private_encrypt 'public release document'
 *
 * === Decryption
 *
 * Use the opposite key type do decrypt the document
 *
 *   top_secret = key.public_decrypt public_encrypted
 *
 *   public_release = key.private_decrypt private_encrypted
 *
 * == PKCS #5 Password-based Encryption
 *
 * PKCS #5 is a password-based encryption standard documented at
 * RFC2898[http://www.ietf.org/rfc/rfc2898.txt].  It allows a short password or
 * passphrase to be used to create a secure encryption key.
 *
 * PKCS #5 uses a Cipher, a pass phrase and a salt to generate an encryption
 * key.
 *
 *   pass_phrase = 'my secure pass phrase goes here'
 *   salt = '8 octets'
 *
 * === Encryption
 *
 * First set up the cipher for encryption
 *
 *   encrypter = OpenSSL::Cipher::Cipher.new 'AES-128-CBC'
 *   encrypter.encrypt
 *   encrypter.pkcs5_keyivgen pass_phrase, salt
 *
 * Then pass the data you want to encrypt through
 *
 *   encrypted = encrypter.update 'top secret document'
 *   encrypted << encrypter.final
 *
 * === Decryption
 *
 * Use a new Cipher instance set up for decryption
 *
 *   decrypter = OpenSSL::Cipher::Cipher.new 'AES-128-CBC'
 *   decrypter.decrypt
 *   decrypter.pkcs5_keyivgen pass_phrase, salt
 *
 * Then pass the data you want to decrypt through
 *
 *   plain = decrypter.update encrypted
 *   plain << decrypter.final
 *
 * == X509 Certificates
 *
 * === Creating a Certificate
 *
 * This example creates a self-signed certificate using an RSA key and a SHA1
 * signature.
 *
 *   name = OpenSSL::X509::Name.parse 'CN=nobody/DC=example'
 *
 *   cert = OpenSSL::X509::Certificate.new
 *   cert.version = 2
 *   cert.serial = 0
 *   cert.not_before = Time.now
 *   cert.not_after = Time.now + 3600
 *
 *   cert.public_key = key.public_key
 *   cert.subject = name
 *
 * === Certificate Extensions
 *
 * You can add extensions to the certificate with
 * OpenSSL::SSL::ExtensionFactory to indicate the purpose of the certificate.
 *
 *   extension_factory = OpenSSL::X509::ExtensionFactory.new nil, cert
 *
 *   extension_factory.create_extension 'basicConstraints', 'CA:FALSE'
 *   extension_factory.create_extension 'keyUsage',
 *     'keyEncipherment,dataEncipherment,digitalSignature'
 *   extension_factory.create_extension 'subjectKeyIdentifier', 'hash'
 *
 * === Signing a Certificate
 *
 * To sign a certificate set the issuer and use OpenSSL::X509::Certificate#sign
 * with a digest algorithm.  This creates a self-signed cert because we're using
 * the same name and key to sign the certificate as was used to create the
 * certificate.
 *
 *   cert.issuer = name
 *   cert.sign key, OpenSSL::Digest::SHA1.new
 *
 *   open 'certificate.pem', 'w' do |io| io.write cert.to_pem end
 *
 * === Loading a Certificate
 *
 * Like a key, a cert can also be loaded from a file.
 *
 *   cert2 = OpenSSL::X509::Certificate.new File.read 'certificate.pem'
 *
 * === Verifying a Certificate
 *
 * Certificate#verify will return true when a certificate was signed with the
 * given public key.
 *
 *   raise 'certificate can not be verified' unless cert2.verify key
 *
 * == Certificate Authority
 *
 * A certificate authority (CA) is a trusted third party that allows you to
 * verify the ownership of unknown certificates.  The CA issues key signatures
 * that indicate it trusts the user of that key.  A user encountering the key
 * can verify the signature by using the CA's public key.
 *
 * === CA Key
 *
 * CA keys are valuable, so we encrypt and save it to disk and make sure it is
 * not readable by other users.
 *
 *   ca_key = OpenSSL::PKey::RSA.new 2048
 *
 *   cipher = OpenSSL::Cipher::Cipher.new 'AES-128-CBC'
 *
 *   open 'ca_key.pem', 'w', 0400 do |io|
 *     io.write key.export(cipher, pass_phrase)
 *   end
 *
 * === CA Certificate
 *
 * A CA certificate is created the same way we created a certificate above, but
 * with different extensions.
 *
 *   ca_name = OpenSSL::X509::Name.parse 'CN=ca/DC=example'
 *
 *   ca_cert = OpenSSL::X509::Certificate.new
 *   ca_cert.serial = 0
 *   ca_cert.version = 2
 *   ca_cert.not_before = Time.now
 *   ca_cert.not_after = Time.now + 86400
 *
 *   ca_cert.public_key = ca_key.public_key
 *   ca_cert.subject = ca_name
 *   ca_cert.issuer = ca_name
 *
 *   extension_factory = OpenSSL::X509::ExtensionFactory.new
 *   extension_factory.subject_certificate = ca_cert
 *   extension_factory.issuer_certificate = ca_cert
 *
 *   extension_factory.create_extension 'subjectKeyIdentifier', 'hash'
 *
 * This extension indicates the CA's key may be used as a CA.
 *
 *   extension_factory.create_extension 'basicConstraints', 'CA:TRUE', true
 *
 * This extension indicates the CA's key may be used to verify signatures on
 * both certificates and certificate revocations.
 *
 *   extension_factory.create_extension 'keyUsage', 'cRLSign,keyCertSign', true
 *
 * Root CA certificates are self-signed.
 *
 *   ca_cert.sign ca_key, OpenSSL::Digest::SHA1.new
 *
 * The CA certificate is saved to disk so it may be distributed to all the
 * users of the keys this CA will sign.
 *
 *   open 'ca_cert.pem', 'w' do |io|
 *     io.write ca_cert.to_pem
 *   end
 *
 * === Certificate Signing Request
 *
 * The CA signs keys through a Certificate Signing Request (CSR).  The CSR
 * contains the information necessary to identify the key.
 *
 *   csr = OpenSSL::X509::Request.new
 *   csr.version = 0
 *   csr.subject = name
 *   csr.public_key = key.public_key
 *   csr.sign key, OpenSSL::Digest::SHA1.new
 *
 * A CSR is saved to disk and sent to the CA for signing.
 *
 *   open 'csr.pem', 'w' do |io|
 *     io.write csr.to_pem
 *   end
 *
 * === Creating a Certificate from a CSR
 *
 * Upon receiving a CSR the CA will verify it before signing it.  A minimal
 * verification would be to check the CSR's signature.
 *
 *   csr = OpenSSL::X509::Request.new File.read 'csr.pem'
 *
 *   raise 'CSR can not be verified' unless csr.verify csr.public_key
 *
 * After verification a certificate is created, marked for various usages,
 * signed with the CA key and returned to the requester.
 *
 *   csr_cert = OpenSSL::X509::Certificate.new
 *   csr_cert.serial = 0
 *   csr_cert.version = 2
 *   csr_cert.not_before = Time.now
 *   csr_cert.not_after = Time.now + 600
 *
 *   csr_cert.subject = csr.subject
 *   csr_cert.public_key = csr.public_key
 *   csr_cert.issuer = ca_cert.subject
 *
 *   extension_factory = OpenSSL::X509::ExtensionFactory.new
 *   extension_factory.subject_certificate = csr_cert
 *   extension_factory.issuer_certificate = ca_cert
 *
 *   extension_factory.create_extension 'basicConstraints', 'CA:FALSE'
 *   extension_factory.create_extension 'keyUsage',
 *     'keyEncipherment,dataEncipherment,digitalSignature'
 *   extension_factory.create_extension 'subjectKeyIdentifier', 'hash'
 *
 *   csr_cert.sign ca_key, OpenSSL::Digest::SHA1.new
 *
 *   open 'csr_cert.pem', 'w' do |io|
 *     io.write csr_cert.to_pem
 *   end
 *
 * == SSL and TLS Connections
 *
 * Using our created key and certificate we can create an SSL or TLS connection.
 * An SSLContext is used to set up an SSL session.
 *
 *   context = OpenSSL::SSL::SSLContext.new
 *
 * === SSL Server
 *
 * An SSL server requires the certificate and private key to communicate
 * securely with its clients:
 *
 *   context.cert = cert
 *   context.key = key
 *
 * Then create an SSLServer with a TCP server socket and the context.  Use the
 * SSLServer like an ordinary TCP server.
 *
 *   require 'socket'
 *
 *   tcp_server = TCPServer.new 5000
 *   ssl_server = OpenSSL::SSL::SSLServer.new tcp_server, context
 *
 *   loop do
 *     ssl_connection = ssl_server.accept
 *
 *     data = connection.gets
 *
 *     response = "I got #{data.dump}"
 *     puts response
 *
 *     connection.puts "I got #{data.dump}"
 *     connection.close
 *   end
 *
 * === SSL client
 *
 * An SSL client is created with a TCP socket and the context.
 * SSLSocket#connect must be called to initiate the SSL handshake and start
 * encryption.  A key and certificate are not required for the client socket.
 *
 *   require 'socket'
 *
 *   tcp_client = TCPSocket.new 'localhost', 5000
 *   ssl_client = OpenSSL::SSL::SSLSocket.new client_socket, context
 *   ssl_client.connect
 *
 *   ssl_client.puts "hello server!"
 *   puts ssl_client.gets
 *
 * === Peer Verification
 *
 * An unverified SSL connection does not provide much security.  For enhanced
 * security the client or server can verify the certificate of its peer.
 *
 * The client can be modified to verify the server's certificate against the
 * certificate authority's certificate:
 *
 *   context.ca_file = 'ca_cert.pem'
 *   context.verify_mode = OpenSSL::SSL::VERIFY_PEER
 *
 *   require 'socket'
 *
 *   tcp_client = TCPSocket.new 'localhost', 5000
 *   ssl_client = OpenSSL::SSL::SSLSocket.new client_socket, context
 *   ssl_client.connect
 *
 *   ssl_client.puts "hello server!"
 *   puts ssl_client.gets
 *
 * If the server certificate is invalid or <tt>context.ca_file</tt> is not set
 * when verifying peers an OpenSSL::SSL::SSLError will be raised.
 *
 */
void
Init_openssl()
{
    /*
     * Init timezone info
     */
#if 0
    tzset();
#endif

    /*
     * Init all digests, ciphers
     */
    /* CRYPTO_malloc_init(); */
    /* ENGINE_load_builtin_engines(); */
    OpenSSL_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    /*
     * FIXME:
     * On unload do:
     */
#if 0
    CONF_modules_unload(1);
    destroy_ui_method();
    EVP_cleanup();
    ENGINE_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_remove_state(0);
    ERR_free_strings();
#endif

    /*
     * Init main module
     */
    mOSSL = rb_define_module("OpenSSL");

    /*
     * OpenSSL ruby extension version
     */
    rb_define_const(mOSSL, "VERSION", rb_str_new2(OSSL_VERSION));

    /*
     * Version of OpenSSL the ruby OpenSSL extension was built with
     */
    rb_define_const(mOSSL, "OPENSSL_VERSION", rb_str_new2(OPENSSL_VERSION_TEXT));
    /*
     * Version number of OpenSSL the ruby OpenSSL extension was built with
     * (base 16)
     */
    rb_define_const(mOSSL, "OPENSSL_VERSION_NUMBER", INT2NUM(OPENSSL_VERSION_NUMBER));

    /*
     * Generic error,
     * common for all classes under OpenSSL module
     */
    eOSSLError = rb_define_class_under(mOSSL,"OpenSSLError",rb_eStandardError);

    /*
     * Verify callback Proc index for ext-data
     */
    if ((ossl_verify_cb_idx = X509_STORE_CTX_get_ex_new_index(0, (void *)"ossl_verify_cb_idx", 0, 0, 0)) < 0)
        ossl_raise(eOSSLError, "X509_STORE_CTX_get_ex_new_index");

    /*
     * Init debug core
     */
    dOSSL = Qfalse;
    rb_define_module_function(mOSSL, "debug", ossl_debug_get, 0);
    rb_define_module_function(mOSSL, "debug=", ossl_debug_set, 1);
    rb_define_module_function(mOSSL, "errors", ossl_get_errors, 0);

    /*
     * Get ID of to_der
     */
    ossl_s_to_der = rb_intern("to_der");

    /*
     * Init components
     */
    Init_ossl_bn();
    Init_ossl_cipher();
    Init_ossl_config();
    Init_ossl_digest();
    Init_ossl_hmac();
    Init_ossl_ns_spki();
    Init_ossl_pkcs12();
    Init_ossl_pkcs7();
    Init_ossl_pkcs5();
    Init_ossl_pkey();
    Init_ossl_rand();
    Init_ossl_ssl();
    Init_ossl_x509();
    Init_ossl_ocsp();
    Init_ossl_engine();
    Init_ossl_asn1();
}

#if defined(OSSL_DEBUG)
/*
 * Check if all symbols are OK with 'make LDSHARED=gcc all'
 */
int
main(int argc, char *argv[])
{
    return 0;
}
#endif /* OSSL_DEBUG */

