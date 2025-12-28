--TEST--
openssl_x509_export_to_file memory leak
--EXTENSIONS--
openssl
--FILE--
<?php

$path = "file://" . __DIR__ . "/sni_server_ca.pem";
var_dump(openssl_x509_export_to_file($path, str_repeat("a", 10000)));

?>
--EXPECTF--
Warning: openssl_x509_export_to_file(output_filename): must be a valid file path %s
bool(false)
