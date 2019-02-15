--TEST--
Handling of includes that were not executed
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_include.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo "Foobar";
?>
--EXPECTF--
Foobar
