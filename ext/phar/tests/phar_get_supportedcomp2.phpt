--TEST--
Phar::getSupportedCompression() (bz2 only)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not available"); ?>
<?php if (extension_loaded("zlib")) die("skip zlib is available"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
var_dump(Phar::getSupportedCompression());
?>
===DONE===
--EXPECT--
array(1) {
  [0]=>
  string(5) "BZIP2"
}
===DONE===
