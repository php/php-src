--TEST--
Test hash_algos() function : basic functionality 
--SKIPIF--
<?php 
	if(!extension_loaded("hash")) print "skip"; 
?>
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
array(38) {
  [0]=>
  string(3) "md2"
  [1]=>
  string(3) "md4"
  [2]=>
  string(3) "md5"
  [3]=>
  string(4) "sha1"
  [4]=>
  string(6) "sha256"
  [5]=>
  string(6) "sha384"
  [6]=>
  string(6) "sha512"
  [7]=>
  string(9) "ripemd128"
  [8]=>
  string(9) "ripemd160"
  [9]=>
  string(9) "ripemd256"
  [10]=>
  string(9) "ripemd320"
  [11]=>
  string(9) "whirlpool"
  [12]=>
  string(10) "tiger128,3"
  [13]=>
  string(10) "tiger160,3"
  [14]=>
  string(10) "tiger192,3"
  [15]=>
  string(10) "tiger128,4"
  [16]=>
  string(10) "tiger160,4"
  [17]=>
  string(10) "tiger192,4"
  [18]=>
  string(6) "snefru"
  [19]=>
  string(4) "gost"
  [20]=>
  string(7) "adler32"
  [21]=>
  string(5) "crc32"
  [22]=>
  string(6) "crc32b"
  [23]=>
  string(10) "haval128,3"
  [24]=>
  string(10) "haval160,3"
  [25]=>
  string(10) "haval192,3"
  [26]=>
  string(10) "haval224,3"
  [27]=>
  string(10) "haval256,3"
  [28]=>
  string(10) "haval128,4"
  [29]=>
  string(10) "haval160,4"
  [30]=>
  string(10) "haval192,4"
  [31]=>
  string(10) "haval224,4"
  [32]=>
  string(10) "haval256,4"
  [33]=>
  string(10) "haval128,5"
  [34]=>
  string(10) "haval160,5"
  [35]=>
  string(10) "haval192,5"
  [36]=>
  string(10) "haval224,5"
  [37]=>
  string(10) "haval256,5"
}
===Done===