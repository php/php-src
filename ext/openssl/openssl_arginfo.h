/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 90fe9bfde63940e050f74dd9dccae7773fc57eff */

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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cipher_algo, IS_LONG, 0, "OPENSSL_CIPHER_RC2_40")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cipher_algo, IS_LONG, 0, "OPENSSL_CIPHER_RC2_40")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tag, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aad, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_openssl_cipher_iv_length, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, cipher_algo, IS_STRING, 0)
ZEND_END_ARG_INFO()

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
