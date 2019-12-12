--TEST--
Preload trait with static variables in method
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_trait_static.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$bar = new Bar;
$bar->test();
?>
--EXPECT--
NULL
