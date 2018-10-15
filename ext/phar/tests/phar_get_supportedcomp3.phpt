--TEST--
Phar::getSupportedCompression() (zlib only)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (extension_loaded("bz2")) die("skip bz2 is available"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
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
  string(2) "GZ"
}
===DONE===
