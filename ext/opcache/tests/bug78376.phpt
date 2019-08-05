--TEST--
Bug #78376 (Incorrect preloading of constant static properties)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_bug78376.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(\A::$a);
?>
--EXPECT--
string(4) "aaaa"