--TEST--
Phar::apiVersion
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
echo Phar::apiVersion();
?>
--EXPECT--
0.8.0