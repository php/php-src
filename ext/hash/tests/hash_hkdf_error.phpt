--TEST--
Hash: hash_hkdf() function: error conditions
--FILE--
<?php

require __DIR__ . '/exceptional.inc';


/* Prototype  : string hkdf  ( string $algo  , string $ikm  [, int $length  , string $info = '' , string $salt = ''  ] )
 * Description: HMAC-based Key Derivation Function
 * Source code: ext/hash/hash.c
*/

$ikm = 'input key material';

echo "*** Testing hash_hkdf(): error conditions ***\n";

echo "\n-- Testing hash_hkdf() function with invalid hash algorithm --\n";
trycatch_dump(fn() => hash_hkdf('foo', $ikm));

echo "\n-- Testing hash_hkdf() function with non-cryptographic hash algorithm --\n";
trycatch_dump(fn() => hash_hkdf('adler32', $ikm));
trycatch_dump(fn() => hash_hkdf('crc32', $ikm));
trycatch_dump(fn() => hash_hkdf('crc32b', $ikm));
trycatch_dump(fn() => hash_hkdf('fnv132', $ikm));
trycatch_dump(fn() => hash_hkdf('fnv1a32', $ikm));
trycatch_dump(fn() => hash_hkdf('fnv164', $ikm));
trycatch_dump(fn() => hash_hkdf('fnv1a64', $ikm));
trycatch_dump(fn() => hash_hkdf('joaat', $ikm));

echo "\n-- Testing hash_hkdf() function with invalid parameters --\n";
trycatch_dump(fn() => hash_hkdf('sha1', ''));
trycatch_dump(fn() => hash_hkdf('sha1', $ikm, -1));
trycatch_dump(fn() => hash_hkdf('sha1', $ikm, 20 * 255 + 1)); // Length can't be more than 255 times the hash digest size

?>
===Done===
--EXPECT--
*** Testing hash_hkdf(): error conditions ***

-- Testing hash_hkdf() function with invalid hash algorithm --
[ErrorException] Parameter 1 is invalid: Unknown hashing algorithm: foo

-- Testing hash_hkdf() function with non-cryptographic hash algorithm --
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: adler32
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: crc32
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: crc32b
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: fnv132
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: fnv1a32
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: fnv164
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: fnv1a64
[ErrorException] Parameter 1 is invalid: Non-cryptographic hashing algorithm: joaat

-- Testing hash_hkdf() function with invalid parameters --
[ErrorException] Parameter 2 is invalid: Input keying material cannot be empty
[ErrorException] Parameter 3 is invalid: Length must be greater than or equal to 0: -1
[ErrorException] Parameter 3 is invalid: Length must be less than or equal to 5100: 5101
===Done===
