--TEST--
Bug #68104 (Segfault while pre-evaluating a disabled function)
--INI--
opcache.enable=1
opcache.enable_cli=1
disable_functions=dl
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(is_callable("dl"));
--EXPECT--
bool(true)
