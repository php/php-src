--TEST--
openssl_pkey_export_to_file memory leak
--EXTENSIONS--
openssl
--FILE--
<?php

$path = "file://" . __DIR__ . "/private_rsa_1024.key";
$key = [$path, ""];
var_dump(openssl_pkey_export_to_file($key, str_repeat("a", 10000), passphrase: ""));

?>
--EXPECTF--
Warning: openssl_pkey_export_to_file(output_filename): must be a valid file path %s
bool(false)
