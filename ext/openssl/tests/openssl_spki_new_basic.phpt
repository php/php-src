--TEST--
openssl_spki_new() test for creating SPKI string
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php

/* array of private key sizes to test */
$key_sizes = array(1024, 2048, 4096);
$pkeys = array();
foreach ($key_sizes as $key_size) {
    $key_file = "file://" . dirname(__FILE__) . "/private_rsa_" . $key_size . ".key";
    $pkeys[] = openssl_pkey_get_private($key_file);
}


/* array of available hashings to test */
$algo = array(
    OPENSSL_ALGO_MD4,
    OPENSSL_ALGO_MD5,
    OPENSSL_ALGO_SHA1,
    OPENSSL_ALGO_SHA224,
    OPENSSL_ALGO_SHA256,
    OPENSSL_ALGO_SHA384,
    OPENSSL_ALGO_SHA512,
    OPENSSL_ALGO_RMD160
);

/* loop over key sizes for test */
foreach ($pkeys as $pkey) {

    /* loop to create and verify results */
    foreach ($algo as $value) {
        var_dump(openssl_spki_new($pkey, _uuid(), $value));
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
--EXPECTF--
string(478) "%s"
string(478) "%s"
string(478) "%s"
string(478) "%s"
string(478) "%s"
string(478) "%s"
string(478) "%s"
string(474) "%s"
string(830) "%s"
string(830) "%s"
string(830) "%s"
string(830) "%s"
string(830) "%s"
string(830) "%s"
string(830) "%s"
string(826) "%s"
string(1510) "%s"
string(1510) "%s"
string(1510) "%s"
string(1510) "%s"
string(1510) "%s"
string(1510) "%s"
string(1510) "%s"
string(1506) "%s"
