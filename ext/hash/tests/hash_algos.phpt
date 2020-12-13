--TEST--
Hash: hash_algos() function : basic functionality
--FILE--
<?php

echo "*** Testing hash_algos() : basic functionality ***\n";
var_dump(hash_algos());

?>
--EXPECTF--
*** Testing hash_algos() : basic functionality ***
array(60) {
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
  string(10) "sha512/224"
  [8]=>
  string(10) "sha512/256"
  [9]=>
  string(6) "sha512"
  [10]=>
  string(8) "sha3-224"
  [11]=>
  string(8) "sha3-256"
  [12]=>
  string(8) "sha3-384"
  [13]=>
  string(8) "sha3-512"
  [14]=>
  string(9) "ripemd128"
  [15]=>
  string(9) "ripemd160"
  [16]=>
  string(9) "ripemd256"
  [17]=>
  string(9) "ripemd320"
  [18]=>
  string(9) "whirlpool"
  [19]=>
  string(10) "tiger128,3"
  [20]=>
  string(10) "tiger160,3"
  [21]=>
  string(10) "tiger192,3"
  [22]=>
  string(10) "tiger128,4"
  [23]=>
  string(10) "tiger160,4"
  [24]=>
  string(10) "tiger192,4"
  [25]=>
  string(6) "snefru"
  [26]=>
  string(9) "snefru256"
  [27]=>
  string(4) "gost"
  [28]=>
  string(11) "gost-crypto"
  [29]=>
  string(7) "adler32"
  [30]=>
  string(5) "crc32"
  [31]=>
  string(6) "crc32b"
  [32]=>
  string(6) "crc32c"
  [33]=>
  string(6) "fnv132"
  [34]=>
  string(7) "fnv1a32"
  [35]=>
  string(6) "fnv164"
  [36]=>
  string(7) "fnv1a64"
  [37]=>
  string(5) "joaat"
  [38]=>
  string(8) "murmur3a"
  [39]=>
  string(8) "murmur3c"
  [40]=>
  string(8) "murmur3f"
  [41]=>
  string(5) "xxh32"
  [42]=>
  string(5) "xxh64"
  [43]=>
  string(4) "xxh3"
  [44]=>
  string(6) "xxh128"
  [45]=>
  string(10) "haval128,3"
  [46]=>
  string(10) "haval160,3"
  [47]=>
  string(10) "haval192,3"
  [48]=>
  string(10) "haval224,3"
  [49]=>
  string(10) "haval256,3"
  [50]=>
  string(10) "haval128,4"
  [51]=>
  string(10) "haval160,4"
  [52]=>
  string(10) "haval192,4"
  [53]=>
  string(10) "haval224,4"
  [54]=>
  string(10) "haval256,4"
  [55]=>
  string(10) "haval128,5"
  [56]=>
  string(10) "haval160,5"
  [57]=>
  string(10) "haval192,5"
  [58]=>
  string(10) "haval224,5"
  [59]=>
  string(10) "haval256,5"
}
