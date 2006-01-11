--TEST--
Phar::mapPhar zlib is loaded
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip zlib is present"; ?>
--FILE--
<?php
var_dump(Phar::canCompress());
?>
--EXPECTF--
bool(true)