--TEST--
Test hash_hkdf() function: edge cases
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--FILE--
<?php

/* Prototype  : string hkdf  ( string $algo  , string $ikm  [, int $length  , string $info = '' , string $salt = ''  ] )
 * Description: HMAC-based Key Derivation Function
 * Source code: ext/hash/hash.c
*/

echo "*** Testing hash_hkdf(): edge cases ***\n";

$ikm = 'input key material';

echo 'Length < digestSize: ', bin2hex(hash_hkdf('md5', $ikm, 7)), "\n";
echo 'Length % digestSize != 0: ', bin2hex(hash_hkdf('md5', $ikm, 17)), "\n";

?>
--EXPECTF--
*** Testing hash_hkdf(): edge cases ***
Length < digestSize: 98b16391063ece
Length % digestSize != 0: 98b16391063ecee006a3ca8ee5776b1e5f
