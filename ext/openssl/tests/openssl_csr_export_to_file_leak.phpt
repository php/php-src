--TEST--
openssl_csr_export_to_file memory leak
--EXTENSIONS--
openssl
--FILE--
<?php

$path = "file://" . __DIR__ . "/cert.csr";
var_dump(openssl_csr_export_to_file($path, str_repeat("a", 10000)));

?>
--EXPECTF--
Warning: openssl_csr_export_to_file(output_filename): must be a valid file path %s
bool(false)
