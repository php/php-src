/* This is a generated file, edit the .stub.php files instead. */

static const func_info_t func_infos[] = {
    F1("zend_version", MAY_BE_STRING),
    FN("func_get_args", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_ANY),
    F1("get_class_methods", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("get_included_files", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    FN("set_error_handler", MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_OBJECT|MAY_BE_NULL),
    F0("restore_error_handler", MAY_BE_TRUE),
    FN("set_exception_handler", MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_OBJECT|MAY_BE_NULL),
    F0("restore_exception_handler", MAY_BE_TRUE),
    F1("get_declared_classes", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("get_declared_traits", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("get_declared_interfaces", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("get_defined_functions", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_ARRAY),
    F1("get_resource_type", MAY_BE_STRING),
    F1("get_loaded_extensions", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("get_defined_constants", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_ANY),
    F1("debug_backtrace", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_ARRAY),
    F1("get_extension_funcs", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_FALSE),
    F1("gc_status", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_LONG),
    F1("class_implements", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_STRING|MAY_BE_FALSE),
    F1("class_parents", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_STRING|MAY_BE_FALSE),
    F1("class_uses", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_STRING|MAY_BE_FALSE),
    F1("spl_classes", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_STRING),
    F1("spl_object_hash", MAY_BE_STRING),
    FN("socket_export_stream", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("dba_popen", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("dba_open", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("bzopen", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("gzopen", MAY_BE_RESOURCE|MAY_BE_FALSE),
#if HAVE_NANOSLEEP
    F1("time_nanosleep", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_LONG|MAY_BE_BOOL),
#endif
#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
    F1("dns_get_record", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_ARRAY|MAY_BE_FALSE),
#endif
    FN("opendir", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("popen", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("fopen", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("tmpfile", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("fsockopen", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("pfsockopen", MAY_BE_RESOURCE|MAY_BE_FALSE),
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
    F1("proc_open", MAY_BE_RESOURCE|MAY_BE_FALSE),
#endif
    F1("stream_context_create", MAY_BE_RESOURCE),
    FN("stream_context_get_default", MAY_BE_RESOURCE),
    FN("stream_context_set_default", MAY_BE_RESOURCE),
    FN("stream_filter_prepend", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("stream_filter_append", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("stream_socket_client", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("stream_socket_server", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("stream_socket_accept", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("zip_open", MAY_BE_RESOURCE|MAY_BE_LONG|MAY_BE_FALSE),
    FN("zip_read", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("json_encode", MAY_BE_STRING|MAY_BE_FALSE),
    F1("json_last_error_msg", MAY_BE_STRING),
    F1("finfo_open", MAY_BE_OBJECT|MAY_BE_FALSE),
    F1("finfo_file", MAY_BE_STRING|MAY_BE_FALSE),
    F1("finfo_buffer", MAY_BE_STRING|MAY_BE_FALSE),
    F1("mime_content_type", MAY_BE_STRING|MAY_BE_FALSE),
    FN("oci_new_connect", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("oci_connect", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("oci_pconnect", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("oci_parse", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("oci_get_implicit_resultset", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("oci_password_change", MAY_BE_RESOURCE|MAY_BE_BOOL),
    FN("oci_new_cursor", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_prepare", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_exec", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_connect", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_pconnect", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_tables", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_columns", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_gettypeinfo", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_primarykeys", MAY_BE_RESOURCE|MAY_BE_FALSE),
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
    FN("odbc_procedurecolumns", MAY_BE_RESOURCE|MAY_BE_FALSE),
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
    FN("odbc_procedures", MAY_BE_RESOURCE|MAY_BE_FALSE),
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
    FN("odbc_foreignkeys", MAY_BE_RESOURCE|MAY_BE_FALSE),
#endif
    FN("odbc_specialcolumns", MAY_BE_RESOURCE|MAY_BE_FALSE),
    FN("odbc_statistics", MAY_BE_RESOURCE|MAY_BE_FALSE),
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
    FN("odbc_tableprivileges", MAY_BE_RESOURCE|MAY_BE_FALSE),
#endif
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
    FN("odbc_columnprivileges", MAY_BE_RESOURCE|MAY_BE_FALSE),
#endif
    F1("textdomain", MAY_BE_STRING),
    F1("gettext", MAY_BE_STRING),
    F1("dgettext", MAY_BE_STRING),
    F1("dcgettext", MAY_BE_STRING),
    F1("bindtextdomain", MAY_BE_STRING|MAY_BE_FALSE),
#if defined(HAVE_NGETTEXT)
    F1("ngettext", MAY_BE_STRING),
#endif
#if defined(HAVE_DNGETTEXT)
    F1("dngettext", MAY_BE_STRING),
#endif
#if defined(HAVE_DCNGETTEXT)
    F1("dcngettext", MAY_BE_STRING),
#endif
#if defined(HAVE_BIND_TEXTDOMAIN_CODESET)
    F1("bind_textdomain_codeset", MAY_BE_STRING|MAY_BE_FALSE),
#endif
    F1("exif_tagname", MAY_BE_STRING|MAY_BE_FALSE),
    F1("exif_read_data", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_ANY|MAY_BE_FALSE),
    F1("exif_thumbnail", MAY_BE_STRING|MAY_BE_FALSE),
    F1("pg_socket", MAY_BE_RESOURCE|MAY_BE_FALSE),
#if defined(HAVE_AESGCM)
    F1("sodium_crypto_aead_aes256gcm_decrypt", MAY_BE_STRING|MAY_BE_FALSE),
#endif
#if defined(HAVE_AESGCM)
    F1("sodium_crypto_aead_aes256gcm_encrypt", MAY_BE_STRING),
#endif
#if defined(HAVE_AESGCM)
    F1("sodium_crypto_aead_aes256gcm_keygen", MAY_BE_STRING),
#endif
    F1("sodium_crypto_aead_chacha20poly1305_decrypt", MAY_BE_STRING|MAY_BE_FALSE),
    F1("sodium_crypto_aead_chacha20poly1305_encrypt", MAY_BE_STRING),
    F1("sodium_crypto_aead_chacha20poly1305_keygen", MAY_BE_STRING),
    F1("sodium_crypto_aead_chacha20poly1305_ietf_decrypt", MAY_BE_STRING|MAY_BE_FALSE),
    F1("sodium_crypto_aead_chacha20poly1305_ietf_encrypt", MAY_BE_STRING),
    F1("sodium_crypto_aead_chacha20poly1305_ietf_keygen", MAY_BE_STRING),
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
    F1("sodium_crypto_aead_xchacha20poly1305_ietf_decrypt", MAY_BE_STRING|MAY_BE_FALSE),
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
    F1("sodium_crypto_aead_xchacha20poly1305_ietf_keygen", MAY_BE_STRING),
#endif
#if defined(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES)
    F1("sodium_crypto_aead_xchacha20poly1305_ietf_encrypt", MAY_BE_STRING),
#endif
    F1("sodium_crypto_auth", MAY_BE_STRING),
    F1("sodium_crypto_auth_keygen", MAY_BE_STRING),
    F1("sodium_crypto_box", MAY_BE_STRING),
    F1("sodium_crypto_box_keypair", MAY_BE_STRING),
    F1("sodium_crypto_box_seed_keypair", MAY_BE_STRING),
    F1("sodium_crypto_box_keypair_from_secretkey_and_publickey", MAY_BE_STRING),
    F1("sodium_crypto_box_open", MAY_BE_STRING|MAY_BE_FALSE),
    F1("sodium_crypto_box_publickey", MAY_BE_STRING),
    F1("sodium_crypto_box_publickey_from_secretkey", MAY_BE_STRING),
    F1("sodium_crypto_box_seal", MAY_BE_STRING),
    F1("sodium_crypto_box_seal_open", MAY_BE_STRING|MAY_BE_FALSE),
    F1("sodium_crypto_box_secretkey", MAY_BE_STRING),
    F1("sodium_crypto_kx_keypair", MAY_BE_STRING),
    F1("sodium_crypto_kx_publickey", MAY_BE_STRING),
    F1("sodium_crypto_kx_secretkey", MAY_BE_STRING),
    F1("sodium_crypto_kx_seed_keypair", MAY_BE_STRING),
    F1("sodium_crypto_kx_client_session_keys", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("sodium_crypto_kx_server_session_keys", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("sodium_crypto_generichash", MAY_BE_STRING),
    F1("sodium_crypto_generichash_keygen", MAY_BE_STRING),
    F1("sodium_crypto_generichash_init", MAY_BE_STRING),
    F0("sodium_crypto_generichash_update", MAY_BE_TRUE),
    F1("sodium_crypto_generichash_final", MAY_BE_STRING),
    F1("sodium_crypto_kdf_derive_from_key", MAY_BE_STRING),
    F1("sodium_crypto_kdf_keygen", MAY_BE_STRING),
#if defined(crypto_pwhash_SALTBYTES)
    F1("sodium_crypto_pwhash", MAY_BE_STRING),
#endif
#if defined(crypto_pwhash_SALTBYTES)
    F1("sodium_crypto_pwhash_str", MAY_BE_STRING),
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
    F1("sodium_crypto_pwhash_scryptsalsa208sha256", MAY_BE_STRING),
#endif
#if defined(crypto_pwhash_scryptsalsa208sha256_SALTBYTES)
    F1("sodium_crypto_pwhash_scryptsalsa208sha256_str", MAY_BE_STRING),
#endif
    F1("sodium_crypto_scalarmult", MAY_BE_STRING),
    F1("sodium_crypto_secretbox", MAY_BE_STRING),
    F1("sodium_crypto_secretbox_keygen", MAY_BE_STRING),
    F1("sodium_crypto_secretbox_open", MAY_BE_STRING|MAY_BE_FALSE),
    F1("sodium_crypto_shorthash", MAY_BE_STRING),
    F1("sodium_crypto_shorthash_keygen", MAY_BE_STRING),
    F1("sodium_crypto_sign", MAY_BE_STRING),
    F1("sodium_crypto_sign_detached", MAY_BE_STRING),
    F1("sodium_crypto_sign_ed25519_pk_to_curve25519", MAY_BE_STRING),
    F1("sodium_crypto_sign_ed25519_sk_to_curve25519", MAY_BE_STRING),
    F1("sodium_crypto_sign_keypair", MAY_BE_STRING),
    F1("sodium_crypto_sign_keypair_from_secretkey_and_publickey", MAY_BE_STRING),
    F1("sodium_crypto_sign_open", MAY_BE_STRING|MAY_BE_FALSE),
    F1("sodium_crypto_sign_publickey", MAY_BE_STRING),
    F1("sodium_crypto_sign_secretkey", MAY_BE_STRING),
    F1("sodium_crypto_sign_publickey_from_secretkey", MAY_BE_STRING),
    F1("sodium_crypto_sign_seed_keypair", MAY_BE_STRING),
    F1("sodium_crypto_stream", MAY_BE_STRING),
    F1("sodium_crypto_stream_keygen", MAY_BE_STRING),
    F1("sodium_crypto_stream_xor", MAY_BE_STRING),
    F1("sodium_pad", MAY_BE_STRING),
    F1("sodium_unpad", MAY_BE_STRING),
    F1("sodium_bin2hex", MAY_BE_STRING),
    F1("sodium_hex2bin", MAY_BE_STRING),
    F1("filter_input_array", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_ANY|MAY_BE_FALSE|MAY_BE_NULL),
    F1("filter_list", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    FN("pcntl_signal_get_handler", MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_OBJECT|MAY_BE_LONG),
    F1("bcadd", MAY_BE_STRING),
    F1("bcsub", MAY_BE_STRING),
    F1("bcmul", MAY_BE_STRING),
    F1("bcdiv", MAY_BE_STRING),
    F1("bcmod", MAY_BE_STRING),
    F1("bcpowmod", MAY_BE_STRING),
    F1("bcpow", MAY_BE_STRING),
    F1("bcsqrt", MAY_BE_STRING),
    F1("session_name", MAY_BE_STRING|MAY_BE_FALSE),
    F1("session_module_name", MAY_BE_STRING|MAY_BE_FALSE),
    F1("session_save_path", MAY_BE_STRING|MAY_BE_FALSE),
    F1("session_create_id", MAY_BE_STRING|MAY_BE_FALSE),
    F1("session_encode", MAY_BE_STRING|MAY_BE_FALSE),
    F1("session_get_cookie_params", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_ANY),
    F1("session_cache_limiter", MAY_BE_STRING|MAY_BE_FALSE),
};
