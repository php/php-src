--TEST--
Bug #70631 (Another Segfault in gc_remove_from_buffer())
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.protect_memory=1
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache') || !extension_loaded("Reflection")) die("skip"); ?>
--FILE--
<?php

function x($x = array("Okay")) {}

$func = new ReflectionFunction('x');
$params = $func->getParameters();
$value = $params[0]->getDefaultValue();
echo $value[0];
?>
--EXPECT--
Okay
