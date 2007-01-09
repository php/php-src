--TEST--
Phar::apiVersion
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
var_dump(Phar::apiVersion());
?>
===DONE===
--EXPECT--
string(5) "0.9.0"
===DONE===
