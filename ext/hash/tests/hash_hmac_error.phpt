--TEST--
Test hash_hmac() function : basic functionality
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--FILE--
<?php
/*
* proto string hash_hmac ( string algo, string data, string key [, bool raw_output] )
* Function is implemented in ext/hash/hash.c
*/

echo "*** Testing hash_hmac() : error conditions ***\n";

$data = "This is a sample string used to test the hash_hmac function with various hashing algorithms";
$key = 'secret';

echo "\n-- Testing hash_hmac() function with less than expected no. of arguments --\n";
var_dump(hash_hmac());
var_dump(hash_hmac('md5'));
var_dump(hash_hmac('md5', $data));

echo "\n-- Testing hash_hmac() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump(hash_hmac('md5', $data, $key, TRUE, $extra_arg));

echo "\n-- Testing hash_hmac() function with invalid hash algorithm --\n";
var_dump(hash_hmac('foo', $data, $key));

echo "\n-- Testing hash_hmac() function with non-cryptographic hash algorithm --\n";
var_dump(hash_hmac('crc32', $data, $key));

?>
===Done===
--EXPECTF--
*** Testing hash_hmac() : error conditions ***

-- Testing hash_hmac() function with less than expected no. of arguments --

Warning: hash_hmac() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: hash_hmac() expects at least 3 parameters, 1 given in %s on line %d
NULL

Warning: hash_hmac() expects at least 3 parameters, 2 given in %s on line %d
NULL

-- Testing hash_hmac() function with more than expected no. of arguments --

Warning: hash_hmac() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing hash_hmac() function with invalid hash algorithm --

Warning: hash_hmac(): Unknown hashing algorithm: foo in %s on line %d
bool(false)

-- Testing hash_hmac() function with non-cryptographic hash algorithm --

Warning: hash_hmac(): Non-cryptographic hashing algorithm: crc32 in %s on line %d
bool(false)
===Done===
