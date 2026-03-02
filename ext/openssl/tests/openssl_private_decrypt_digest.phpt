--TEST--
openssl_private_decrypt() with digest algorithm tests
--EXTENSIONS--
openssl
--FILE--
<?php
$data = "Testing openssl_private_decrypt() with digest algorithms";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$pubkey = "file://" . __DIR__ . "/public.key";

openssl_public_encrypt($data, $encrypted_sha256, $pubkey, OPENSSL_PKCS1_OAEP_PADDING, "sha256");
var_dump(openssl_private_decrypt($encrypted_sha256, $output_sha256, $privkey, OPENSSL_PKCS1_OAEP_PADDING, "sha256"));
var_dump($output_sha256);

openssl_public_encrypt($data, $encrypted_default, $pubkey, OPENSSL_PKCS1_OAEP_PADDING);
var_dump(openssl_private_decrypt($encrypted_default, $output_default, $privkey, OPENSSL_PKCS1_OAEP_PADDING));
var_dump($output_default);

// Some distros (RHEL) explicitly disable SHA1 so this runs only if available
if (in_array('sha1', openssl_get_md_methods())) {
    openssl_public_encrypt($data, $encrypted_sha256, $pubkey, OPENSSL_PKCS1_OAEP_PADDING, "sha256");
    var_dump(openssl_private_decrypt($encrypted_sha256, $output_mismatch, $privkey, OPENSSL_PKCS1_OAEP_PADDING, "sha1"));
    var_dump($output_mismatch);
} else {
    var_dump(false);
    var_dump(NULL);
}

var_dump(openssl_private_decrypt($encrypted_sha256, $output_invalid, $privkey, OPENSSL_PKCS1_OAEP_PADDING, "invalid_hash"));
var_dump($output_invalid);

// Test that "p" is used instead of "s" (it is not a ZPP test but flag selection test so do not remove)
try {
    var_dump(openssl_private_decrypt($encrypted_sha256, $output_invalid, $privkey, OPENSSL_PKCS1_OAEP_PADDING, "sha256\0extra"));
    var_dump($output_invalid);
} catch (\ValueError $e) {
    var_dump($e->getMessage());
}

openssl_public_encrypt($data, $encrypted_pkcs1, $pubkey, OPENSSL_PKCS1_PADDING);
var_dump(openssl_private_decrypt($encrypted_pkcs1, $output_pkcs1, $privkey, OPENSSL_PKCS1_PADDING, "sha256"));
var_dump($output_pkcs1);
?>
--EXPECTF--
bool(true)
string(56) "Testing openssl_private_decrypt() with digest algorithms"
bool(true)
string(56) "Testing openssl_private_decrypt() with digest algorithms"
bool(false)
NULL

Warning: openssl_private_decrypt(): Unknown digest algorithm: invalid_hash in %s on line %d
bool(false)
NULL
string(85) "openssl_private_decrypt(): Argument #5 ($digest_algo) must not contain any null bytes"
bool(true)
string(56) "Testing openssl_private_decrypt() with digest algorithms"
