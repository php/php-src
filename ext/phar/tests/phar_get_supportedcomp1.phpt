--TEST--
Phar::getSupportedCompression() (bz2 and zlib)
--EXTENSIONS--
phar
bz2
zlib
--INI--
phar.require_hash=0
--FILE--
<?php
var_dump(Phar::getSupportedCompression());
?>
--EXPECT--
array(2) {
  [0]=>
  string(2) "GZ"
  [1]=>
  string(5) "BZIP2"
}
