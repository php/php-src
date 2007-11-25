--TEST--
Phar::getSupportedCompression() (none)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
<?php if (!extension_loaded("spl")) print "skip SPL not available"; ?>
<?php if (extension_loaded("bz2")) print "skip bz2 is available"; ?>
<?php if (extension_loaded("zlib")) print "skip zlib is available"; ?>
--FILE--
<?php
var_dump(Phar::getSupportedCompression());
?>
===DONE===
--EXPECT--
array(0) {
}
===DONE===