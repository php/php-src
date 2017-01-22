--TEST--
Test hash_hkdf() function: error conditions
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--FILE--
<?php

/* Prototype  : string hkdf  ( string $algo  , string $ikm  [, int $length  , string $info = '' , string $salt = ''  ] )
 * Description: HMAC-based Key Derivation Function
 * Source code: ext/hash/hash.c
*/

$ikm = 'input key material';

echo "*** Testing hash_hkdf(): error conditions ***\n";

echo "\n-- Testing hash_hkdf() function with less than expected no. of arguments --\n";
var_dump(hash_hkdf());
var_dump(hash_hkdf('sha1'));

echo "\n-- Testing hash_hkdf() function with more than expected no. of arguments --\n";
var_dump(hash_hkdf('sha1', $ikm, 20, '', '', 'extra parameter'));

echo "\n-- Testing hash_hkdf() function with invalid hash algorithm --\n";
var_dump(hash_hkdf('foo', $ikm));

echo "\n-- Testing hash_hkdf() function with non-cryptographic hash algorithm --\n";
var_dump(hash_hkdf('adler32', $ikm));
var_dump(hash_hkdf('crc32', $ikm));
var_dump(hash_hkdf('crc32b', $ikm));
var_dump(hash_hkdf('fnv132', $ikm));
var_dump(hash_hkdf('fnv1a32', $ikm));
var_dump(hash_hkdf('fnv164', $ikm));
var_dump(hash_hkdf('fnv1a64', $ikm));
var_dump(hash_hkdf('joaat', $ikm));

echo "\n-- Testing hash_hkdf() function with invalid parameters --\n";
var_dump(hash_hkdf('sha1', ''));
var_dump(hash_hkdf('sha1', $ikm, -1));
var_dump(hash_hkdf('sha1', $ikm, 20 * 255 + 1)); // Length can't be more than 255 times the hash digest size
?>
===Done===
--EXPECTF--
*** Testing hash_hkdf(): error conditions ***

-- Testing hash_hkdf() function with less than expected no. of arguments --

Warning: hash_hkdf() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: hash_hkdf() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing hash_hkdf() function with more than expected no. of arguments --

Warning: hash_hkdf() expects at most 5 parameters, 6 given in %s on line %d
NULL

-- Testing hash_hkdf() function with invalid hash algorithm --

Warning: hash_hkdf(): Unknown hashing algorithm: foo in %s on line %d
bool(false)

-- Testing hash_hkdf() function with non-cryptographic hash algorithm --

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: adler32 in %s on line %d
bool(false)

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: crc32 in %s on line %d
bool(false)

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: crc32b in %s on line %d
bool(false)

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: fnv132 in %s on line %d
bool(false)

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: fnv1a32 in %s on line %d
bool(false)

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: fnv164 in %s on line %d
bool(false)

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: fnv1a64 in %s on line %d
bool(false)

Warning: hash_hkdf(): Non-cryptographic hashing algorithm: joaat in %s on line %d
bool(false)

-- Testing hash_hkdf() function with invalid parameters --

Warning: hash_hkdf(): Input keying material cannot be empty in %s on line %d
bool(false)

Warning: hash_hkdf(): Length must be greater than or equal to 0: -1 in %s on line %d
bool(false)

Warning: hash_hkdf(): Length must be less than or equal to 5100: 5101 in %s on line %d
bool(false)
===Done===
