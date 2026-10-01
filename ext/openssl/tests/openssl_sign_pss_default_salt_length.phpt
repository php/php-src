--TEST--
openssl_sign() with RSA-PSS uses the digest length as the default salt length (OpenSSL >= 3.1)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER < 0x30100000) die('skip For OpenSSL >= 3.1');
?>
--FILE--
<?php
$data = "Testing openssl_sign() with RSA-PSS";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$pubkey = "file://" . __DIR__ . "/public.key";

var_dump(openssl_sign($data, $sign, $privkey, OPENSSL_ALGO_SHA256, OPENSSL_PKCS1_PSS_PADDING));
var_dump(openssl_verify($data, $sign, $pubkey, OPENSSL_ALGO_SHA256, OPENSSL_PKCS1_PSS_PADDING, OPENSSL_RSA_PSS_SALTLEN_DIGEST));
?>
--EXPECT--
bool(true)
int(1)
