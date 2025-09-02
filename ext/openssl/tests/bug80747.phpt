--TEST--
Bug #80747: Providing RSA key size < 512 generates key that crash PHP
--EXTENSIONS--
openssl
--FILE--
<?php

$conf = array(
    'config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf',
    'private_key_bits' => 511,
);
var_dump(openssl_pkey_new($conf));
var_dump(openssl_error_string() !== false);

?>
--EXPECT--
bool(false)
bool(true)
