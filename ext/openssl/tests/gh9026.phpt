--TEST--
Bug GH-9026: openssl_encrypt() silently truncates passphrase
--EXTENSIONS--
openssl
--FILE--
<?php

$cipher = "aes-128-ctr";
$data = "test";
$passphrase = "KeyLengthIs16_12";
$iv = str_repeat("\x00", openssl_cipher_iv_length($cipher));
$flags = 0;

$m1 = openssl_encrypt(
    $data,
    $cipher,
    $passphrase,
    $flags,
    $iv
);

$passphrase .= "3";
$m2 = openssl_encrypt(
    $data,
    $cipher,
    $passphrase,
    $flags,
    $iv
);

var_dump($m1 === $m2);

?>
--EXPECTF--
Deprecated: openssl_encrypt(): Passphrase is too long and will be truncated to 16 characters in %s on line %d
bool(true)
