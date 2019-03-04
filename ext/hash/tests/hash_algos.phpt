--TEST--
Hash: hash_algos() function : basic functionality
--FILE--
<?php

/* Prototype  : array hash_algos (void)
 * Description: Return a list of registered hashing algorithms
 * Source code: ext/hash/hash.c
 * Alias to functions:
*/

echo "*** Testing hash_algos() : basic functionality ***\n";
var_dump(hash_algos());

?>
===Done===
--EXPECTF--
*** Testing hash_algos() : basic functionality ***
array(53) {
  [%d]=>
  string(3) "md2"
  [%d]=>
  string(3) "md4"
  [%d]=>
  string(3) "md5"
  [%d]=>
  string(4) "sha1"
  [%d]=>
  string(6) "sha224"
  [%d]=>
  string(6) "sha256"
  [%d]=>
  string(6) "sha384"
  [%d]=>
  string(10) "sha512/224"
  [%d]=>
  string(10) "sha512/256"
  [%d]=>
  string(6) "sha512"
  [%d]=>
  string(8) "sha3-224"
  [%d]=>
  string(8) "sha3-256"
  [%d]=>
  string(8) "sha3-384"
  [%d]=>
  string(8) "sha3-512"
  [%d]=>
  string(9) "ripemd128"
  [%d]=>
  string(9) "ripemd160"
  [%d]=>
  string(9) "ripemd256"
  [%d]=>
  string(9) "ripemd320"
  [%d]=>
  string(9) "whirlpool"
  [%d]=>
  string(10) "tiger128,3"
  [%d]=>
  string(10) "tiger160,3"
  [%d]=>
  string(10) "tiger192,3"
  [%d]=>
  string(10) "tiger128,4"
  [%d]=>
  string(10) "tiger160,4"
  [%d]=>
  string(10) "tiger192,4"
  [%d]=>
  string(6) "snefru"
  [%d]=>
  string(9) "snefru256"
  [%d]=>
  string(4) "gost"
  [%d]=>
  string(11) "gost-crypto"
  [%d]=>
  string(7) "adler32"
  [%d]=>
  string(5) "crc32"
  [%d]=>
  string(6) "crc32b"
  [%d]=>
  string(6) "crc32c"
  [%d]=>
  string(6) "fnv132"
  [%d]=>
  string(7) "fnv1a32"
  [%d]=>
  string(6) "fnv164"
  [%d]=>
  string(7) "fnv1a64"
  [%d]=>
  string(5) "joaat"
  [%d]=>
  string(10) "haval128,3"
  [%d]=>
  string(10) "haval160,3"
  [%d]=>
  string(10) "haval192,3"
  [%d]=>
  string(10) "haval224,3"
  [%d]=>
  string(10) "haval256,3"
  [%d]=>
  string(10) "haval128,4"
  [%d]=>
  string(10) "haval160,4"
  [%d]=>
  string(10) "haval192,4"
  [%d]=>
  string(10) "haval224,4"
  [%d]=>
  string(10) "haval256,4"
  [%d]=>
  string(10) "haval128,5"
  [%d]=>
  string(10) "haval160,5"
  [%d]=>
  string(10) "haval192,5"
  [%d]=>
  string(10) "haval224,5"
  [%d]=>
  string(10) "haval256,5"
}
===Done===
