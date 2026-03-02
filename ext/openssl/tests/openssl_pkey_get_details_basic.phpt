--TEST--
openssl_pkey_get_details() with EC key
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip no EC available");
?>
--FILE--
<?php
$key = openssl_pkey_get_private("file://" . __DIR__ . "/private_ec.key");

print_r(openssl_pkey_get_details($key));
?>
--EXPECTF--
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
            [d] => %a
        )

    [type] => 3
)
