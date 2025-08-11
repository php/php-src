--TEST--
openssl_sign() tests
--EXTENSIONS--
openssl
--FILE--
<?php
$data = "Testing openssl_sign()";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$wrong = "wrong";

var_dump(openssl_sign($data, $sign1, $privkey, OPENSSL_ALGO_SHA256));
var_dump(bin2hex($sign1));
var_dump(openssl_sign($data, $sign2, $privkey, OPENSSL_ALGO_SHA256));
var_dump($sign1 === $sign2);
var_dump(openssl_sign($data, $sign1, $privkey, OPENSSL_ALGO_SHA256, OPENSSL_PKCS1_PSS_PADDING));
var_dump(strlen($sign1));
var_dump(openssl_sign($data, $sign2, $privkey, OPENSSL_ALGO_SHA256, OPENSSL_PKCS1_PSS_PADDING));
var_dump(strlen($sign2));
var_dump($sign1 === $sign2);
var_dump(openssl_sign($data, $sign, $wrong));
?>
--EXPECTF--
bool(true)
string(256) "5eff033d92208fcbf52edc9cbf6c9d4bd7c06b7b5a6a22c7d641d1494a09d6b0898d321c0a8fdb55c10b9bf25c2bb777c2b4660f867001f79879d089de7321a28df5f037cc02b68c47d1eb28d98a9199876961adb02524a489872a12fd3675db6a957d623ff04b9f715b565f516806cea247264c82a7569871dbd0b86cfe4689"
bool(true)
bool(true)
bool(true)
int(128)
bool(true)
int(128)
bool(false)

Warning: openssl_sign(): Supplied key param cannot be coerced into a private key in %s on line %d
bool(false)
