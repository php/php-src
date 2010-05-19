--TEST--
openssl_decrypt() error tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "openssl_decrypt() tests";
$method = "AES-128-CBC";
$password = "openssl";
$wrong = "wrong";
$iv = str_repeat("\0", openssl_cipher_iv_length($method));

$encrypted = openssl_encrypt($data, $method, $password);
var_dump($encrypted); /* Not passing $iv should be the same as all-NULL iv, but with a warning */
var_dump(openssl_encrypt($data, $method, $password, false, $iv)); 
var_dump(openssl_decrypt($encrypted, $method, $wrong));
var_dump(openssl_decrypt($encrypted, $wrong, $password));
var_dump(openssl_decrypt($wrong, $method, $password));
var_dump(openssl_decrypt($wrong, $wrong, $password));
var_dump(openssl_decrypt($encrypted, $wrong, $wrong));
var_dump(openssl_decrypt($wrong, $wrong, $wrong));
var_dump(openssl_decrypt(array(), $method, $password));
var_dump(openssl_decrypt($encrypted, array(), $password));
var_dump(openssl_decrypt($encrypted, $method, array()));
?>
--EXPECTF--

Warning: openssl_encrypt(): Using an empty Initialization Vector (iv) is potentially insecure and not recommended in %s on line %d
string(44) "yof6cPPH4mLee6TOc0YQSrh4dvywMqxGUyjp0lV6+aM="
string(44) "yof6cPPH4mLee6TOc0YQSrh4dvywMqxGUyjp0lV6+aM="
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt(): Unknown cipher algorithm in %s on line %d
bool(false)

Warning: openssl_decrypt() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: openssl_decrypt() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: openssl_decrypt() expects parameter 3 to be string, array given in %s on line %d
NULL
