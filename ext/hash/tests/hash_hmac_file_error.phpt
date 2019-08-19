--TEST--
Hash: hash_hmac_file() function : basic functionality
--FILE--
<?php

require_once __DIR__ . '/exceptional.inc';

/* Prototype  : string hash_hmac_file ( string algo, string filename, string key [, bool raw_output] )
 * Description: Generate a keyed hash value using the HMAC method and the contents of a given file
 * Source code: ext/hash/hash.c
 * Alias to functions:
*/

echo "*** Testing hash() : error conditions ***\n";

$file = __DIR__ . "hash_file.txt";
$key = 'secret';

echo "\n-- Testing hash_hmac_file() function with invalid hash algorithm --\n";
trycatch_dump(fn() => hash_hmac_file('foo', $file, $key, TRUE));

echo "\n-- Testing hash_hmac_file() function with non-cryptographic hash algorithm --\n";
trycatch_dump(fn() => hash_hmac_file('crc32', $file, $key, TRUE));

echo "\n-- Testing hash_hmac_file() function with bad path --\n";
trycatch_dump(fn() => hash_hmac_file('md5', $file.chr(0).$file, $key, TRUE));

?>
===Done===
--EXPECT--
*** Testing hash() : error conditions ***

-- Testing hash_hmac_file() function with invalid hash algorithm --
[ErrorException] Parameter 1 is invalid: Unknown hashing algorithm: foo

-- Testing hash_hmac_file() function with non-cryptographic hash algorithm --
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: crc32

-- Testing hash_hmac_file() function with bad path --
[ErrorException] Parameter 2 is invalid: Invalid path
===Done===
