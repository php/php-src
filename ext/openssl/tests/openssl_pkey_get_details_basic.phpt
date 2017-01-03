--TEST--
openssl_pkey_get_details() with EC key
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip no EC available");
?>
--FILE--
<?php
$key = openssl_pkey_get_private("file://" . dirname(__FILE__) . "/private_ec.key");

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
