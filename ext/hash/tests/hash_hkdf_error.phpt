--TEST--
Hash: hash_hkdf() function: error conditions
--FILE--
<?php

error_reporting(E_ALL);

/* Prototype  : string hkdf  ( string $algo  , string $ikm  [, int $length  , string $info = '' , string $salt = ''  ] )
 * Description: HMAC-based Key Derivation Function
 * Source code: ext/hash/hash.c
*/

function trycatch_dump(...$tests) {
    foreach ($tests as $test) {
        try {
            var_dump($test());
        }
        catch (\Error $e) {
            echo '[' . get_class($e) . '] ' . $e->getMessage() . "\n";
        }
    }
}

$ikm = 'input key material';

echo "*** Testing hash_hkdf(): error conditions ***\n";

echo "\n-- Testing hash_hkdf() function with invalid hash algorithm --\n";
trycatch_dump(
    fn() => hash_hkdf('foo', $ikm)
);

echo "\n-- Testing hash_hkdf() function with non-cryptographic hash algorithm --\n";
trycatch_dump(
    fn() => hash_hkdf('adler32', $ikm),
    fn() => hash_hkdf('crc32', $ikm),
    fn() => hash_hkdf('crc32b', $ikm),
    fn() => hash_hkdf('fnv132', $ikm),
    fn() => hash_hkdf('fnv1a32', $ikm),
    fn() => hash_hkdf('fnv164', $ikm),
    fn() => hash_hkdf('fnv1a64', $ikm),
    fn() => hash_hkdf('joaat', $ikm)
);

echo "\n-- Testing hash_hkdf() function with invalid parameters --\n";
trycatch_dump(
    fn() => hash_hkdf('sha1', ''),
    fn() => hash_hkdf('sha1', $ikm, -1),
    fn() => hash_hkdf('sha1', $ikm, 20 * 255 + 1) // Length can't be more than 255 times the hash digest size
)
?>
--EXPECT--
*** Testing hash_hkdf(): error conditions ***

-- Testing hash_hkdf() function with invalid hash algorithm --
[Error] Unknown hashing algorithm: foo

-- Testing hash_hkdf() function with non-cryptographic hash algorithm --
[Error] Non-cryptographic hashing algorithm: adler32
[Error] Non-cryptographic hashing algorithm: crc32
[Error] Non-cryptographic hashing algorithm: crc32b
[Error] Non-cryptographic hashing algorithm: fnv132
[Error] Non-cryptographic hashing algorithm: fnv1a32
[Error] Non-cryptographic hashing algorithm: fnv164
[Error] Non-cryptographic hashing algorithm: fnv1a64
[Error] Non-cryptographic hashing algorithm: joaat

-- Testing hash_hkdf() function with invalid parameters --
[Error] Input keying material cannot be empty
[Error] Length must be greater than or equal to 0: -1
[Error] Length must be less than or equal to 5100: 5101
