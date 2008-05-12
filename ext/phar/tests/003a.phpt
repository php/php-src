--TEST--
Phar::canCompress, specific
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
var_dump(Phar::canCompress(Phar::GZ) == extension_loaded("zlib"));
var_dump(Phar::canCompress(Phar::BZ2) == extension_loaded("bz2"));
?>
===DONE===
--EXPECT--
bool(true)
bool(true)
===DONE===
