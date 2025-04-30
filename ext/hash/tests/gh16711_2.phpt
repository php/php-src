--TEST--
GH-16711: Segmentation fault in mhash()
--SKIPIF--
<?php if(!function_exists('mhash')) { die('skip mhash compatibility layer not available'); } ?>
--FILE--
<?php

$re = new ReflectionExtension("hash");
var_dump($re->getConstants());

var_dump(mhash(4, 1086849124, 133));
?>
--EXPECTF--
array(40) {
  ["HASH_HMAC"]=>
  int(1)
  ["MHASH_CRC32"]=>
  int(0)
  ["MHASH_MD5"]=>
  int(1)
  ["MHASH_SHA1"]=>
  int(2)
  ["MHASH_HAVAL256"]=>
  int(3)
  ["MHASH_RIPEMD160"]=>
  int(5)
  ["MHASH_TIGER"]=>
  int(7)
  ["MHASH_GOST"]=>
  int(8)
  ["MHASH_CRC32B"]=>
  int(9)
  ["MHASH_HAVAL224"]=>
  int(10)
  ["MHASH_HAVAL192"]=>
  int(11)
  ["MHASH_HAVAL160"]=>
  int(12)
  ["MHASH_HAVAL128"]=>
  int(13)
  ["MHASH_TIGER128"]=>
  int(14)
  ["MHASH_TIGER160"]=>
  int(15)
  ["MHASH_MD4"]=>
  int(16)
  ["MHASH_SHA256"]=>
  int(17)
  ["MHASH_ADLER32"]=>
  int(18)
  ["MHASH_SHA224"]=>
  int(19)
  ["MHASH_SHA512"]=>
  int(20)
  ["MHASH_SHA384"]=>
  int(21)
  ["MHASH_WHIRLPOOL"]=>
  int(22)
  ["MHASH_RIPEMD128"]=>
  int(23)
  ["MHASH_RIPEMD256"]=>
  int(24)
  ["MHASH_RIPEMD320"]=>
  int(25)
  ["MHASH_SNEFRU256"]=>
  int(27)
  ["MHASH_MD2"]=>
  int(28)
  ["MHASH_FNV132"]=>
  int(29)
  ["MHASH_FNV1A32"]=>
  int(30)
  ["MHASH_FNV164"]=>
  int(31)
  ["MHASH_FNV1A64"]=>
  int(32)
  ["MHASH_JOAAT"]=>
  int(33)
  ["MHASH_CRC32C"]=>
  int(34)
  ["MHASH_MURMUR3A"]=>
  int(35)
  ["MHASH_MURMUR3C"]=>
  int(36)
  ["MHASH_MURMUR3F"]=>
  int(37)
  ["MHASH_XXH32"]=>
  int(38)
  ["MHASH_XXH64"]=>
  int(39)
  ["MHASH_XXH3"]=>
  int(40)
  ["MHASH_XXH128"]=>
  int(41)
}

Deprecated: Function mhash() is deprecated since 8.1 in %s on line %d
bool(false)
