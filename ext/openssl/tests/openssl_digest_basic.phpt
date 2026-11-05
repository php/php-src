--TEST--
openssl_digest() basic test
--EXTENSIONS--
openssl
--FILE--
<?php
$data = "openssl_digest() basic test";
$method = "md5";
$method2 = "sha1";

var_dump(openssl_digest($data, $method));
var_dump(openssl_digest($data, $method2));

echo "Hex output\n";
var_dump(openssl_digest('abc', 'sha256'));
var_dump(openssl_digest('abc', 'sha256', false));

echo "Binary output\n";
$raw = openssl_digest('abc', 'sha256', true);
var_dump(strlen($raw));
var_dump(bin2hex($raw));

echo "Data with NUL bytes\n";
var_dump(openssl_digest("a\0b", 'sha256'));

echo "Unknown algorithm\n";
var_dump(openssl_digest('abc', 'unknown'));
?>
--EXPECTF--
string(32) "f0045b6c41d9ec835cb8948c7fec4955"
string(40) "aa6e750fef05c2414c18860ad31f2c35e79bf3dc"
Hex output
string(64) "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
string(64) "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
Binary output
int(32)
string(64) "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
Data with NUL bytes
string(64) "59b271ae1bbcb1d31d41929817f4b16fb439eb4f31520b5ad1d5ce98920a7138"
Unknown algorithm

Warning: openssl_digest(): Unknown digest algorithm in %s on line %d
bool(false)
