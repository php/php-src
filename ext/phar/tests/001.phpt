--TEST--
PHP_Archive::apiVersion
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
echo PHP_Archive::apiVersion();
?>
--EXPECT--
0.7.1