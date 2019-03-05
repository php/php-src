--TEST--
Hash: hash_hmac() function : basic functionality
--FILE--
<?php
/*
* proto string hash_hmac ( string algo, string data, string key [, bool raw_output] )
* Function is implemented in ext/hash/hash.c
*/

echo "*** Testing hash_hmac() : error conditions ***\n";

$data = "This is a sample string used to test the hash_hmac function with various hashing algorithms";
$key = 'secret';

echo "\n-- Testing hash_hmac() function with invalid hash algorithm --\n";
var_dump(hash_hmac('foo', $data, $key));

echo "\n-- Testing hash_hmac() function with non-cryptographic hash algorithm --\n";
var_dump(hash_hmac('crc32', $data, $key));

?>
===Done===
--EXPECTF--
*** Testing hash_hmac() : error conditions ***

-- Testing hash_hmac() function with invalid hash algorithm --

Warning: hash_hmac(): Unknown hashing algorithm: foo in %s on line %d
bool(false)

-- Testing hash_hmac() function with non-cryptographic hash algorithm --

Warning: hash_hmac(): Non-cryptographic hashing algorithm: crc32 in %s on line %d
bool(false)
===Done===
