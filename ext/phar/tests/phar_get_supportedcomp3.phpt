--TEST--
Phar::getSupportedCompression() (zlib only)
--EXTENSIONS--
phar
zlib
--SKIPIF--
<?php if (extension_loaded("bz2")) die("skip bz2 is available"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
var_dump(Phar::getSupportedCompression());
?>
--EXPECT--
array(1) {
  [0]=>
  string(2) "GZ"
}
