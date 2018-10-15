--TEST--
Phar::getSupportedCompression() (bz2 and zlib)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not available"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
var_dump(Phar::getSupportedCompression());
?>
===DONE===
--EXPECT--
array(2) {
  [0]=>
  string(2) "GZ"
  [1]=>
  string(5) "BZIP2"
}
===DONE===
