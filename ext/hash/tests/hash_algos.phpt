--TEST--
Test hash_algos() function : basic functionality 
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
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
array(42) {
  [0]=>
  string(3) "md2"
  [1]=>
  string(3) "md4"
  [2]=>
  string(3) "md5"
  [3]=>
  string(4) "sha1"
  [4]=>
  string(6) "sha224"
  [5]=>
  string(6) "sha256"
  [6]=>
  string(6) "sha384"
  [7]=>
  string(6) "sha512"
  [8]=>
  string(9) "ripemd128"
  [9]=>
  string(9) "ripemd160"
  [10]=>
  string(9) "ripemd256"
  [11]=>
  string(9) "ripemd320"
  [12]=>
  string(9) "whirlpool"
  [13]=>
  string(10) "tiger128,3"
  [14]=>
  string(10) "tiger160,3"
  [15]=>
  string(10) "tiger192,3"
  [16]=>
  string(10) "tiger128,4"
  [17]=>
  string(10) "tiger160,4"
  [18]=>
  string(10) "tiger192,4"
  [19]=>
  string(6) "snefru"
  [20]=>
  string(9) "snefru256"
  [21]=>
  string(4) "gost"
  [22]=>
  string(7) "adler32"
  [23]=>
  string(5) "crc32"
  [24]=>
  string(6) "crc32b"
  [25]=>
  string(7) "salsa10"
  [26]=>
  string(7) "salsa20"
  [27]=>
  string(10) "haval128,3"
  [28]=>
  string(10) "haval160,3"
  [29]=>
  string(10) "haval192,3"
  [30]=>
  string(10) "haval224,3"
  [31]=>
  string(10) "haval256,3"
  [32]=>
  string(10) "haval128,4"
  [33]=>
  string(10) "haval160,4"
  [34]=>
  string(10) "haval192,4"
  [35]=>
  string(10) "haval224,4"
  [36]=>
  string(10) "haval256,4"
  [37]=>
  string(10) "haval128,5"
  [38]=>
  string(10) "haval160,5"
  [39]=>
  string(10) "haval192,5"
  [40]=>
  string(10) "haval224,5"
  [41]=>
  string(10) "haval256,5"
}
===Done===