/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1bb1eab5199d2e72e624f7e93f59dd5114ed7c86 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_export_to_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, no_text, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_export, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(1, output)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, no_text, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_x509_fingerprint, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, digest_algo, IS_STRING, 0, "\"sha1\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_check_private_key, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(0, private_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_verify, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(0, public_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_x509_parse, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, short_names, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_x509_checkpurpose, 0, 2, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, purpose, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ca_info, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, untrusted_certificates_file, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_openssl_x509_read, 0, 1, OpenSSLCertificate, MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_x509_free, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, certificate, OpenSSLCertificate, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkcs12_export_to_file, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO(0, passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkcs12_export, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(1, output)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO(0, passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkcs12_read, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pkcs12, IS_STRING, 0)
	ZEND_ARG_INFO(1, certificates)
	ZEND_ARG_TYPE_INFO(0, passphrase, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_csr_export_to_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, csr, OpenSSLCertificateSigningRequest, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, no_text, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_csr_export, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, csr, OpenSSLCertificateSigningRequest, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(1, output)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, no_text, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_openssl_csr_sign, 0, 4, OpenSSLCertificate, MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, csr, OpenSSLCertificateSigningRequest, MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, ca_certificate, OpenSSLCertificate, MAY_BE_STRING|MAY_BE_NULL, NULL)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO(0, days, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, serial, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, serial_hex, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_openssl_csr_new, 0, 2, OpenSSLCertificateSigningRequest, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, distinguished_names, IS_ARRAY, 0)
	ZEND_ARG_INFO(1, private_key)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, extra_attributes, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_csr_get_subject, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, csr, OpenSSLCertificateSigningRequest, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, short_names, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_openssl_csr_get_public_key, 0, 1, OpenSSLAsymmetricKey, MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, csr, OpenSSLCertificateSigningRequest, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, short_names, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_openssl_pkey_new, 0, 0, OpenSSLAsymmetricKey, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkey_export_to_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, passphrase, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkey_export, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(1, output)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, passphrase, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_openssl_pkey_get_public, 0, 1, OpenSSLAsymmetricKey, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, public_key)
ZEND_END_ARG_INFO()

#define arginfo_openssl_get_publickey arginfo_openssl_pkey_get_public

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkey_free, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, key, OpenSSLAsymmetricKey, 0)
ZEND_END_ARG_INFO()

#define arginfo_openssl_free_key arginfo_openssl_pkey_free

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_openssl_pkey_get_private, 0, 1, OpenSSLAsymmetricKey, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, passphrase, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_openssl_get_privatekey arginfo_openssl_pkey_get_private

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_pkey_get_details, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, key, OpenSSLAsymmetricKey, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_pbkdf2, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, iterations, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, digest_algo, IS_STRING, 0, "\"sha1\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_pkcs7_verify, 0, 2, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, signers_certificates_filename, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ca_info, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, untrusted_certificates_filename, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, output_filename, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkcs7_encrypt, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, certificate)
	ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cipher_algo, IS_LONG, 0, "OPENSSL_CIPHER_AES_128_CBC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkcs7_sign, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PKCS7_DETACHED")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, untrusted_certificates_filename, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkcs7_decrypt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, certificate)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, private_key, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_pkcs7_read, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, certificates)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_cms_verify, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, certificates, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ca_info, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, untrusted_certificates_filename, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pk7, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sigfile, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_LONG, 0, "OPENSSL_ENCODING_SMIME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_cms_encrypt, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, certificate)
	ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_LONG, 0, "OPENSSL_ENCODING_SMIME")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cipher_algo, IS_LONG, 0, "OPENSSL_CIPHER_AES_128_CBC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_cms_sign, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, certificate, OpenSSLCertificate, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_LONG, 0, "OPENSSL_ENCODING_SMIME")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, untrusted_certificates_filename, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_cms_decrypt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, output_filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, certificate)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, private_key, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_LONG, 0, "OPENSSL_ENCODING_SMIME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_cms_read, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input_filename, IS_STRING, 0)
	ZEND_ARG_INFO(1, certificates)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_private_encrypt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, encrypted_data)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, padding, IS_LONG, 0, "OPENSSL_PKCS1_PADDING")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_private_decrypt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, decrypted_data)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, padding, IS_LONG, 0, "OPENSSL_PKCS1_PADDING")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_public_encrypt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, encrypted_data)
	ZEND_ARG_INFO(0, public_key)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, padding, IS_LONG, 0, "OPENSSL_PKCS1_PADDING")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_public_decrypt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, decrypted_data)
	ZEND_ARG_INFO(0, public_key)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, padding, IS_LONG, 0, "OPENSSL_PKCS1_PADDING")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_error_string, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_sign, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, signature)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_MASK(0, algorithm, MAY_BE_STRING|MAY_BE_LONG, "OPENSSL_ALGO_SHA1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_verify, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, signature, IS_STRING, 0)
	ZEND_ARG_INFO(0, public_key)
	ZEND_ARG_TYPE_MASK(0, algorithm, MAY_BE_STRING|MAY_BE_LONG, "OPENSSL_ALGO_SHA1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_seal, 0, 5, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, sealed_data)
	ZEND_ARG_INFO(1, encrypted_keys)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, cipher_algo, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, iv, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_open, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, output)
	ZEND_ARG_TYPE_INFO(0, encrypted_key, IS_STRING, 0)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO(0, cipher_algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iv, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_get_md_methods, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aliases, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_openssl_get_cipher_methods arginfo_openssl_get_md_methods

#if defined(HAVE_EVP_PKEY_EC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_get_curve_names, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_digest, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, digest_algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_encrypt, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cipher_algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iv, IS_STRING, 0, "\"\"")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, tag, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aad, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tag_length, IS_LONG, 0, "16")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_decrypt, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cipher_algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iv, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tag, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aad, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_cipher_iv_length, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, cipher_algo, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_openssl_cipher_key_length arginfo_openssl_cipher_iv_length

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_dh_compute_key, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, public_key, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, private_key, OpenSSLAsymmetricKey, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_pkey_derive, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, public_key)
	ZEND_ARG_INFO(0, private_key)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key_length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_random_pseudo_bytes, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, strong_result, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_spki_new, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, private_key, OpenSSLAsymmetricKey, 0)
	ZEND_ARG_TYPE_INFO(0, challenge, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, digest_algo, IS_LONG, 0, "OPENSSL_ALGO_MD5")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_spki_verify, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, spki, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_spki_export, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, spki, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_openssl_spki_export_challenge arginfo_openssl_spki_export

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_get_cert_locations, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(openssl_x509_export_to_file);
ZEND_FUNCTION(openssl_x509_export);
ZEND_FUNCTION(openssl_x509_fingerprint);
ZEND_FUNCTION(openssl_x509_check_private_key);
ZEND_FUNCTION(openssl_x509_verify);
ZEND_FUNCTION(openssl_x509_parse);
ZEND_FUNCTION(openssl_x509_checkpurpose);
ZEND_FUNCTION(openssl_x509_read);
ZEND_FUNCTION(openssl_x509_free);
ZEND_FUNCTION(openssl_pkcs12_export_to_file);
ZEND_FUNCTION(openssl_pkcs12_export);
ZEND_FUNCTION(openssl_pkcs12_read);
ZEND_FUNCTION(openssl_csr_export_to_file);
ZEND_FUNCTION(openssl_csr_export);
ZEND_FUNCTION(openssl_csr_sign);
ZEND_FUNCTION(openssl_csr_new);
ZEND_FUNCTION(openssl_csr_get_subject);
ZEND_FUNCTION(openssl_csr_get_public_key);
ZEND_FUNCTION(openssl_pkey_new);
ZEND_FUNCTION(openssl_pkey_export_to_file);
ZEND_FUNCTION(openssl_pkey_export);
ZEND_FUNCTION(openssl_pkey_get_public);
ZEND_FUNCTION(openssl_pkey_free);
ZEND_FUNCTION(openssl_pkey_get_private);
ZEND_FUNCTION(openssl_pkey_get_details);
ZEND_FUNCTION(openssl_pbkdf2);
ZEND_FUNCTION(openssl_pkcs7_verify);
ZEND_FUNCTION(openssl_pkcs7_encrypt);
ZEND_FUNCTION(openssl_pkcs7_sign);
ZEND_FUNCTION(openssl_pkcs7_decrypt);
ZEND_FUNCTION(openssl_pkcs7_read);
ZEND_FUNCTION(openssl_cms_verify);
ZEND_FUNCTION(openssl_cms_encrypt);
ZEND_FUNCTION(openssl_cms_sign);
ZEND_FUNCTION(openssl_cms_decrypt);
ZEND_FUNCTION(openssl_cms_read);
ZEND_FUNCTION(openssl_private_encrypt);
ZEND_FUNCTION(openssl_private_decrypt);
ZEND_FUNCTION(openssl_public_encrypt);
ZEND_FUNCTION(openssl_public_decrypt);
ZEND_FUNCTION(openssl_error_string);
ZEND_FUNCTION(openssl_sign);
ZEND_FUNCTION(openssl_verify);
ZEND_FUNCTION(openssl_seal);
ZEND_FUNCTION(openssl_open);
ZEND_FUNCTION(openssl_get_md_methods);
ZEND_FUNCTION(openssl_get_cipher_methods);
#if defined(HAVE_EVP_PKEY_EC)
ZEND_FUNCTION(openssl_get_curve_names);
#endif
ZEND_FUNCTION(openssl_digest);
ZEND_FUNCTION(openssl_encrypt);
ZEND_FUNCTION(openssl_decrypt);
ZEND_FUNCTION(openssl_cipher_iv_length);
ZEND_FUNCTION(openssl_cipher_key_length);
ZEND_FUNCTION(openssl_dh_compute_key);
ZEND_FUNCTION(openssl_pkey_derive);
ZEND_FUNCTION(openssl_random_pseudo_bytes);
ZEND_FUNCTION(openssl_spki_new);
ZEND_FUNCTION(openssl_spki_verify);
ZEND_FUNCTION(openssl_spki_export);
ZEND_FUNCTION(openssl_spki_export_challenge);
ZEND_FUNCTION(openssl_get_cert_locations);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(openssl_x509_export_to_file, arginfo_openssl_x509_export_to_file)
	ZEND_FE(openssl_x509_export, arginfo_openssl_x509_export)
	ZEND_FE(openssl_x509_fingerprint, arginfo_openssl_x509_fingerprint)
	ZEND_FE(openssl_x509_check_private_key, arginfo_openssl_x509_check_private_key)
	ZEND_FE(openssl_x509_verify, arginfo_openssl_x509_verify)
	ZEND_FE(openssl_x509_parse, arginfo_openssl_x509_parse)
	ZEND_FE(openssl_x509_checkpurpose, arginfo_openssl_x509_checkpurpose)
	ZEND_FE(openssl_x509_read, arginfo_openssl_x509_read)
	ZEND_DEP_FE(openssl_x509_free, arginfo_openssl_x509_free)
	ZEND_FE(openssl_pkcs12_export_to_file, arginfo_openssl_pkcs12_export_to_file)
	ZEND_FE(openssl_pkcs12_export, arginfo_openssl_pkcs12_export)
	ZEND_FE(openssl_pkcs12_read, arginfo_openssl_pkcs12_read)
	ZEND_FE(openssl_csr_export_to_file, arginfo_openssl_csr_export_to_file)
	ZEND_FE(openssl_csr_export, arginfo_openssl_csr_export)
	ZEND_FE(openssl_csr_sign, arginfo_openssl_csr_sign)
	ZEND_FE(openssl_csr_new, arginfo_openssl_csr_new)
	ZEND_FE(openssl_csr_get_subject, arginfo_openssl_csr_get_subject)
	ZEND_FE(openssl_csr_get_public_key, arginfo_openssl_csr_get_public_key)
	ZEND_FE(openssl_pkey_new, arginfo_openssl_pkey_new)
	ZEND_FE(openssl_pkey_export_to_file, arginfo_openssl_pkey_export_to_file)
	ZEND_FE(openssl_pkey_export, arginfo_openssl_pkey_export)
	ZEND_FE(openssl_pkey_get_public, arginfo_openssl_pkey_get_public)
	ZEND_FALIAS(openssl_get_publickey, openssl_pkey_get_public, arginfo_openssl_get_publickey)
	ZEND_DEP_FE(openssl_pkey_free, arginfo_openssl_pkey_free)
	ZEND_DEP_FALIAS(openssl_free_key, openssl_pkey_free, arginfo_openssl_free_key)
	ZEND_FE(openssl_pkey_get_private, arginfo_openssl_pkey_get_private)
	ZEND_FALIAS(openssl_get_privatekey, openssl_pkey_get_private, arginfo_openssl_get_privatekey)
	ZEND_FE(openssl_pkey_get_details, arginfo_openssl_pkey_get_details)
	ZEND_FE(openssl_pbkdf2, arginfo_openssl_pbkdf2)
	ZEND_FE(openssl_pkcs7_verify, arginfo_openssl_pkcs7_verify)
	ZEND_FE(openssl_pkcs7_encrypt, arginfo_openssl_pkcs7_encrypt)
	ZEND_FE(openssl_pkcs7_sign, arginfo_openssl_pkcs7_sign)
	ZEND_FE(openssl_pkcs7_decrypt, arginfo_openssl_pkcs7_decrypt)
	ZEND_FE(openssl_pkcs7_read, arginfo_openssl_pkcs7_read)
	ZEND_FE(openssl_cms_verify, arginfo_openssl_cms_verify)
	ZEND_FE(openssl_cms_encrypt, arginfo_openssl_cms_encrypt)
	ZEND_FE(openssl_cms_sign, arginfo_openssl_cms_sign)
	ZEND_FE(openssl_cms_decrypt, arginfo_openssl_cms_decrypt)
	ZEND_FE(openssl_cms_read, arginfo_openssl_cms_read)
	ZEND_FE(openssl_private_encrypt, arginfo_openssl_private_encrypt)
	ZEND_FE(openssl_private_decrypt, arginfo_openssl_private_decrypt)
	ZEND_FE(openssl_public_encrypt, arginfo_openssl_public_encrypt)
	ZEND_FE(openssl_public_decrypt, arginfo_openssl_public_decrypt)
	ZEND_FE(openssl_error_string, arginfo_openssl_error_string)
	ZEND_FE(openssl_sign, arginfo_openssl_sign)
	ZEND_FE(openssl_verify, arginfo_openssl_verify)
	ZEND_FE(openssl_seal, arginfo_openssl_seal)
	ZEND_FE(openssl_open, arginfo_openssl_open)
	ZEND_FE(openssl_get_md_methods, arginfo_openssl_get_md_methods)
	ZEND_FE(openssl_get_cipher_methods, arginfo_openssl_get_cipher_methods)
#if defined(HAVE_EVP_PKEY_EC)
	ZEND_FE(openssl_get_curve_names, arginfo_openssl_get_curve_names)
#endif
	ZEND_FE(openssl_digest, arginfo_openssl_digest)
	ZEND_FE(openssl_encrypt, arginfo_openssl_encrypt)
	ZEND_FE(openssl_decrypt, arginfo_openssl_decrypt)
	ZEND_FE(openssl_cipher_iv_length, arginfo_openssl_cipher_iv_length)
	ZEND_FE(openssl_cipher_key_length, arginfo_openssl_cipher_key_length)
	ZEND_FE(openssl_dh_compute_key, arginfo_openssl_dh_compute_key)
	ZEND_FE(openssl_pkey_derive, arginfo_openssl_pkey_derive)
	ZEND_FE(openssl_random_pseudo_bytes, arginfo_openssl_random_pseudo_bytes)
	ZEND_FE(openssl_spki_new, arginfo_openssl_spki_new)
	ZEND_FE(openssl_spki_verify, arginfo_openssl_spki_verify)
	ZEND_FE(openssl_spki_export, arginfo_openssl_spki_export)
	ZEND_FE(openssl_spki_export_challenge, arginfo_openssl_spki_export_challenge)
	ZEND_FE(openssl_get_cert_locations, arginfo_openssl_get_cert_locations)
	ZEND_FE_END
};


static const zend_function_entry class_OpenSSLCertificate_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_OpenSSLCertificateSigningRequest_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_OpenSSLAsymmetricKey_methods[] = {
	ZEND_FE_END
};

static void register_openssl_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("OPENSSL_VERSION_TEXT", OPENSSL_VERSION_TEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_VERSION_NUMBER", OPENSSL_VERSION_NUMBER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SSL_CLIENT", X509_PURPOSE_SSL_CLIENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SSL_SERVER", X509_PURPOSE_SSL_SERVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_NS_SSL_SERVER", X509_PURPOSE_NS_SSL_SERVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SMIME_SIGN", X509_PURPOSE_SMIME_SIGN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SMIME_ENCRYPT", X509_PURPOSE_SMIME_ENCRYPT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_CRL_SIGN", X509_PURPOSE_CRL_SIGN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_ANY", X509_PURPOSE_ANY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA1", OPENSSL_ALGO_SHA1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD5", OPENSSL_ALGO_MD5, CONST_PERSISTENT);
#if !defined(OPENSSL_NO_MD4)
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD4", OPENSSL_ALGO_MD4, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_MD2)
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD2", OPENSSL_ALGO_MD2, CONST_PERSISTENT);
#endif
#if PHP_OPENSSL_API_VERSION < 0x10100
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_DSS1", OPENSSL_ALGO_DSS1, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA224", OPENSSL_ALGO_SHA224, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA256", OPENSSL_ALGO_SHA256, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA384", OPENSSL_ALGO_SHA384, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA512", OPENSSL_ALGO_SHA512, CONST_PERSISTENT);
#if !defined(OPENSSL_NO_RMD160)
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_RMD160", OPENSSL_ALGO_RMD160, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PKCS7_DETACHED", PKCS7_DETACHED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_TEXT", PKCS7_TEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOINTERN", PKCS7_NOINTERN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOVERIFY", PKCS7_NOVERIFY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOCHAIN", PKCS7_NOCHAIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOCERTS", PKCS7_NOCERTS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOATTR", PKCS7_NOATTR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_BINARY", PKCS7_BINARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOSIGS", PKCS7_NOSIGS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOOLDMIMETYPE", PKCS7_NOOLDMIMETYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_DETACHED", CMS_DETACHED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_TEXT", CMS_TEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOINTERN", CMS_NOINTERN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOVERIFY", CMS_NOVERIFY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOCERTS", CMS_NOCERTS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOATTR", CMS_NOATTR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_BINARY", CMS_BINARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOSIGS", CMS_NOSIGS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_OLDMIMETYPE", CMS_NOOLDMIMETYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_PADDING", RSA_PKCS1_PADDING, CONST_PERSISTENT);
#if defined(RSA_SSLV23_PADDING)
	REGISTER_LONG_CONSTANT("OPENSSL_SSLV23_PADDING", RSA_SSLV23_PADDING, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_NO_PADDING", RSA_NO_PADDING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_OAEP_PADDING", RSA_PKCS1_OAEP_PADDING, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("OPENSSL_DEFAULT_STREAM_CIPHERS", OPENSSL_DEFAULT_STREAM_CIPHERS, CONST_PERSISTENT);
#if !defined(OPENSSL_NO_RC2)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_40", PHP_OPENSSL_CIPHER_RC2_40, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_RC2)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_128", PHP_OPENSSL_CIPHER_RC2_128, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_RC2)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_64", PHP_OPENSSL_CIPHER_RC2_64, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_DES)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_DES", PHP_OPENSSL_CIPHER_DES, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_DES)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_3DES", PHP_OPENSSL_CIPHER_3DES, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_AES)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_AES_128_CBC", PHP_OPENSSL_CIPHER_AES_128_CBC, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_AES)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_AES_192_CBC", PHP_OPENSSL_CIPHER_AES_192_CBC, CONST_PERSISTENT);
#endif
#if !defined(OPENSSL_NO_AES)
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_AES_256_CBC", PHP_OPENSSL_CIPHER_AES_256_CBC, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_RSA", OPENSSL_KEYTYPE_RSA, CONST_PERSISTENT);
#if !defined(OPENSSL_NO_DSA)
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_DSA", OPENSSL_KEYTYPE_DSA, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_DH", OPENSSL_KEYTYPE_DH, CONST_PERSISTENT);
#if defined(HAVE_EVP_PKEY_EC)
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_EC", OPENSSL_KEYTYPE_EC, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_RAW_DATA", OPENSSL_RAW_DATA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ZERO_PADDING", OPENSSL_ZERO_PADDING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_DONT_ZERO_PAD_KEY", OPENSSL_DONT_ZERO_PAD_KEY, CONST_PERSISTENT);
#if !defined(OPENSSL_NO_TLSEXT)
	REGISTER_LONG_CONSTANT("OPENSSL_TLSEXT_SERVER_NAME", 1, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_ENCODING_DER", ENCODING_DER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ENCODING_SMIME", ENCODING_SMIME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ENCODING_PEM", ENCODING_PEM, CONST_PERSISTENT);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_x509_check_private_key", sizeof("openssl_x509_check_private_key") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs12_export_to_file", sizeof("openssl_pkcs12_export_to_file") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs12_export_to_file", sizeof("openssl_pkcs12_export_to_file") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs12_export", sizeof("openssl_pkcs12_export") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs12_export", sizeof("openssl_pkcs12_export") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs12_read", sizeof("openssl_pkcs12_read") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_csr_sign", sizeof("openssl_csr_sign") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_csr_new", sizeof("openssl_csr_new") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkey_export_to_file", sizeof("openssl_pkey_export_to_file") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkey_export_to_file", sizeof("openssl_pkey_export_to_file") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkey_export", sizeof("openssl_pkey_export") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkey_export", sizeof("openssl_pkey_export") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkey_get_private", sizeof("openssl_pkey_get_private") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkey_get_private", sizeof("openssl_pkey_get_private") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_get_privatekey", sizeof("openssl_get_privatekey") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_get_privatekey", sizeof("openssl_get_privatekey") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pbkdf2", sizeof("openssl_pbkdf2") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs7_sign", sizeof("openssl_pkcs7_sign") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs7_decrypt", sizeof("openssl_pkcs7_decrypt") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkcs7_decrypt", sizeof("openssl_pkcs7_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_cms_sign", sizeof("openssl_cms_sign") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_cms_decrypt", sizeof("openssl_cms_decrypt") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_cms_decrypt", sizeof("openssl_cms_decrypt") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_private_encrypt", sizeof("openssl_private_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_private_encrypt", sizeof("openssl_private_encrypt") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_private_decrypt", sizeof("openssl_private_decrypt") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_private_decrypt", sizeof("openssl_private_decrypt") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_public_encrypt", sizeof("openssl_public_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_public_decrypt", sizeof("openssl_public_decrypt") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_sign", sizeof("openssl_sign") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_seal", sizeof("openssl_seal") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_open", sizeof("openssl_open") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_open", sizeof("openssl_open") - 1), 3, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_encrypt", sizeof("openssl_encrypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_encrypt", sizeof("openssl_encrypt") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_decrypt", sizeof("openssl_decrypt") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_dh_compute_key", sizeof("openssl_dh_compute_key") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_pkey_derive", sizeof("openssl_pkey_derive") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_spki_new", sizeof("openssl_spki_new") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}

static zend_class_entry *register_class_OpenSSLCertificate(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OpenSSLCertificate", class_OpenSSLCertificate_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_OpenSSLCertificateSigningRequest(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OpenSSLCertificateSigningRequest", class_OpenSSLCertificateSigningRequest_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_OpenSSLAsymmetricKey(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OpenSSLAsymmetricKey", class_OpenSSLAsymmetricKey_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
