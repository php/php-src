--TEST--
Phar::getSupportedCompression() (bz2 only)
--EXTENSIONS--
phar
bz2
--SKIPIF--
<?php if (extension_loaded("zlib")) die("skip zlib is available"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
var_dump(Phar::getSupportedCompression());
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "BZIP2"
}
