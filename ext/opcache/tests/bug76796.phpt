--TEST--
Bug #76796: Compile-time evaluation of disabled function in opcache (SCCP) causes segfault
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
disable_functions=strpos
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

var_dump(strpos('foo', 'bar'));

?>
--EXPECTF--
Warning: strpos() has been disabled for security reasons in %s on line %d
NULL
