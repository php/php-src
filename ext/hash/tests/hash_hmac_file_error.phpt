--TEST--
Hash: hash_hmac_file() function : basic functionality
--FILE--
<?php

/* Prototype  : string hash_hmac_file ( string algo, string filename, string key [, bool raw_output] )
 * Description: Generate a keyed hash value using the HMAC method and the contents of a given file
 * Source code: ext/hash/hash.c
 * Alias to functions:
*/

echo "*** Testing hash() : error conditions ***\n";

$file = __DIR__ . "hash_file.txt";
$key = 'secret';

echo "\n-- Testing hash_hmac_file() function with less than expected no. of arguments --\n";
var_dump(hash_hmac_file());
var_dump(hash_hmac_file('crc32'));
var_dump(hash_hmac_file('crc32', $file));

echo "\n-- Testing hash_hmac_file() function with more than expected no. of arguments --\n";
$extra_arg = 10;
hash_hmac_file('crc32', $file, $key, TRUE, $extra_arg);

echo "\n-- Testing hash_hmac_file() function with invalid hash algorithm --\n";
hash_hmac_file('foo', $file, $key, TRUE);

echo "\n-- Testing hash_hmac_file() function with non-cryptographic hash algorithm --\n";
hash_hmac_file('crc32', $file, $key, TRUE);

echo "\n-- Testing hash_hmac_file() function with bad path --\n";
hash_hmac_file('md5', $file.chr(0).$file, $key, TRUE);

?>
===Done===
--EXPECTF--
*** Testing hash() : error conditions ***

-- Testing hash_hmac_file() function with less than expected no. of arguments --

Warning: hash_hmac_file() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: hash_hmac_file() expects at least 3 parameters, 1 given in %s on line %d
NULL

Warning: hash_hmac_file() expects at least 3 parameters, 2 given in %s on line %d
NULL

-- Testing hash_hmac_file() function with more than expected no. of arguments --

Warning: hash_hmac_file() expects at most 4 parameters, 5 given in %s on line %d

-- Testing hash_hmac_file() function with invalid hash algorithm --

Warning: hash_hmac_file(): Unknown hashing algorithm: foo in %s on line %d

-- Testing hash_hmac_file() function with non-cryptographic hash algorithm --

Warning: hash_hmac_file(): Non-cryptographic hashing algorithm: crc32 in %s on line %d

-- Testing hash_hmac_file() function with bad path --

Warning: hash_hmac_file(): Invalid path in %s on line %d
===Done===
