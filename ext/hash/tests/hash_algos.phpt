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
  unicode(3) "md2"
  [1]=>
  unicode(3) "md4"
  [2]=>
  unicode(3) "md5"
  [3]=>
  unicode(4) "sha1"
  [4]=>
  unicode(6) "sha224"
  [5]=>
  unicode(6) "sha256"
  [6]=>
  unicode(6) "sha384"
  [7]=>
  unicode(6) "sha512"
  [8]=>
  unicode(9) "ripemd128"
  [9]=>
  unicode(9) "ripemd160"
  [10]=>
  unicode(9) "ripemd256"
  [11]=>
  unicode(9) "ripemd320"
  [12]=>
  unicode(9) "whirlpool"
  [13]=>
  unicode(10) "tiger128,3"
  [14]=>
  unicode(10) "tiger160,3"
  [15]=>
  unicode(10) "tiger192,3"
  [16]=>
  unicode(10) "tiger128,4"
  [17]=>
  unicode(10) "tiger160,4"
  [18]=>
  unicode(10) "tiger192,4"
  [19]=>
  unicode(6) "snefru"
  [20]=>
  unicode(9) "snefru256"
  [21]=>
  unicode(4) "gost"
  [22]=>
  unicode(7) "adler32"
  [23]=>
  unicode(5) "crc32"
  [24]=>
  unicode(6) "crc32b"
  [25]=>
  unicode(7) "salsa10"
  [26]=>
  unicode(7) "salsa20"
  [27]=>
  unicode(10) "haval128,3"
  [28]=>
  unicode(10) "haval160,3"
  [29]=>
  unicode(10) "haval192,3"
  [30]=>
  unicode(10) "haval224,3"
  [31]=>
  unicode(10) "haval256,3"
  [32]=>
  unicode(10) "haval128,4"
  [33]=>
  unicode(10) "haval160,4"
  [34]=>
  unicode(10) "haval192,4"
  [35]=>
  unicode(10) "haval224,4"
  [36]=>
  unicode(10) "haval256,4"
  [37]=>
  unicode(10) "haval128,5"
  [38]=>
  unicode(10) "haval160,5"
  [39]=>
  unicode(10) "haval192,5"
  [40]=>
  unicode(10) "haval224,5"
  [41]=>
  unicode(10) "haval256,5"
}
===Done===