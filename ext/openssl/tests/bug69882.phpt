--TEST--
Bug #69882: OpenSSL error "key values mismatch" after openssl_pkcs12_read with extra certs
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$p12 = file_get_contents(__DIR__.'/p12_with_extra_certs.p12');

$result = openssl_pkcs12_read($p12, $cert_data, 'qwerty');
var_dump($result);
var_dump(openssl_error_string());
?>
--EXPECTF--
bool(true)
bool(false)
