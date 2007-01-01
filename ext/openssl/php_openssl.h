/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Venaas <venaas@php.net>                                |
   |          Wez Furlong <wez@thebrainroom.com                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_OPENSSL_H
#define PHP_OPENSSL_H

#ifdef PHP_WIN32
# undef PHP_OPENSSL_API
# ifdef OPENSSL_EXPORTS
#  define PHP_OPENSSL_API __declspec(dllexport)
# else
#  define PHP_OPENSSL_API __declspec(dllimport)
# endif
#else
# undef PHP_OPENSSL_API
# define PHP_OPENSSL_API /* nothing special */
#endif

/* HAVE_OPENSSL would include SSL MySQL stuff */
#if defined(HAVE_OPENSSL_EXT) || defined(HAVE_OPENSSL_SHARED_EXT)

extern zend_module_entry openssl_module_entry;
#define phpext_openssl_ptr &openssl_module_entry

PHP_MINIT_FUNCTION(openssl);
PHP_MSHUTDOWN_FUNCTION(openssl);
PHP_MINFO_FUNCTION(openssl);

PHP_FUNCTION(openssl_pkey_get_private);
PHP_FUNCTION(openssl_pkey_get_public);
PHP_FUNCTION(openssl_pkey_free);
PHP_FUNCTION(openssl_pkey_new);
PHP_FUNCTION(openssl_pkey_export);
PHP_FUNCTION(openssl_pkey_export_to_file);

PHP_FUNCTION(openssl_sign);
PHP_FUNCTION(openssl_verify);
PHP_FUNCTION(openssl_seal);
PHP_FUNCTION(openssl_open);
PHP_FUNCTION(openssl_private_encrypt);
PHP_FUNCTION(openssl_private_decrypt);
PHP_FUNCTION(openssl_public_encrypt);
PHP_FUNCTION(openssl_public_decrypt);

PHP_FUNCTION(openssl_pkcs7_verify);
PHP_FUNCTION(openssl_pkcs7_decrypt);
PHP_FUNCTION(openssl_pkcs7_sign);
PHP_FUNCTION(openssl_pkcs7_encrypt);

PHP_FUNCTION(openssl_error_string);

PHP_FUNCTION(openssl_x509_read);
PHP_FUNCTION(openssl_x509_free);
PHP_FUNCTION(openssl_x509_parse);
PHP_FUNCTION(openssl_x509_checkpurpose);
PHP_FUNCTION(openssl_x509_export);
PHP_FUNCTION(openssl_x509_export_to_file);
PHP_FUNCTION(openssl_x509_check_private_key);
PHP_FUNCTION(openssl_csr_new);
PHP_FUNCTION(openssl_csr_export);
PHP_FUNCTION(openssl_csr_export_to_file);
PHP_FUNCTION(openssl_csr_sign);

#include <openssl/ssl.h>
int php_openssl_apply_verification_policy(SSL *ssl, X509 *peer, php_stream *stream TSRMLS_DC);
SSL *php_SSL_new_from_context(SSL_CTX *ctx, php_stream *stream TSRMLS_DC);


#else

#define phpext_openssl_ptr NULL

#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
