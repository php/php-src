--TEST--
Bug #80747: Providing RSA key size < 512 generates key that crash PHP
--FILE--
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (OPENSSL_VERSION_NUMBER < 0x10100000) die("skip OpenSSL >= v1.1.0 required");
?>
--FILE--
<?php

$conf = array(
    'config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf',
    'private_key_bits' => 511,
);
var_dump(openssl_pkey_new($conf));
while ($e = openssl_error_string()) {
    echo $e, "\n";
}

?>
--EXPECTF--
bool(false)
error:%s:key size too small
