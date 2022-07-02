--TEST--
Phar::getSupportedCompression() (none)
--EXTENSIONS--
phar
--SKIPIF--
<?php if (extension_loaded("bz2")) die("skip bz2 is available"); ?>
<?php if (extension_loaded("zlib")) die("skip zlib is available"); ?>
--FILE--
<?php
var_dump(Phar::getSupportedCompression());
?>
--EXPECT--
array(0) {
}
