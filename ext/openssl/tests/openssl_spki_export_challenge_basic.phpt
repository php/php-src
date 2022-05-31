--TEST--
openssl_spki_export_challenge() tests for exporting challenge
--INI--
error_reporting=0
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php

/* array of private key sizes to test */
$key_sizes = array(1024, 2048, 4096);
$pkeys = array();
foreach ($key_sizes as $key_size) {
    $key_file = "file://" . __DIR__ . "/private_rsa_" . $key_size . ".key";
    $pkeys[] = openssl_pkey_get_private($key_file);
}


/* array of available hashings to test */
$algo = array(
    OPENSSL_ALGO_SHA224,
    OPENSSL_ALGO_SHA256,
    OPENSSL_ALGO_SHA384,
    OPENSSL_ALGO_SHA512,
);

/* loop over key sizes for test */
foreach ($pkeys as $pkey) {

    /* loop to create and verify results */
    foreach ($algo as $value) {
        $spkac = openssl_spki_new($pkey, _uuid(), $value);
        var_dump(openssl_spki_export_challenge(preg_replace('/SPKAC=/', '', $spkac)));
        var_dump(openssl_spki_export_challenge($spkac  . 'Make it fail'));
    }
}

/* generate a random challenge */
function _uuid() {
    return sprintf('%04x%04x-%04x-%04x-%04x-%04x%04x%04x', mt_rand(0, 0xffff),
        mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0x0fff) | 0x4000,
        mt_rand(0, 0x3fff) | 0x8000, mt_rand(0, 0xffff),
        mt_rand(0, 0xffff), mt_rand(0, 0xffff));
}


?>
--EXPECTREGEX--
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
