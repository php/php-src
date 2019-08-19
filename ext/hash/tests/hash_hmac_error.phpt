--TEST--
Hash: hash_hmac() function : basic functionality
--FILE--
<?php

require_once __DIR__ '/exceptional.inc';

/*
* proto string hash_hmac ( string algo, string data, string key [, bool raw_output] )
* Function is implemented in ext/hash/hash.c
*/

echo "*** Testing hash_hmac() : error conditions ***\n";

$data = "This is a sample string used to test the hash_hmac function with various hashing algorithms";
$key = 'secret';

echo "\n-- Testing hash_hmac() function with invalid hash algorithm --\n";
trycatch_dump(fn() => hash_hmac('foo', $data, $key));

echo "\n-- Testing hash_hmac() function with non-cryptographic hash algorithm --\n";
trycatch_dump(fn() => hash_hmac('crc32', $data, $key));

?>
===Done===
--EXPECTF--
Parse error: syntax error, unexpected ''/exceptional.inc'' (T_CONSTANT_ENCAPSED_STRING) in %s on line %d
