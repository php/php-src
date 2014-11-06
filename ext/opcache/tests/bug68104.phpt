--TEST--
Bug #68104 (Segfault while pre-evaluating a disabled function)
--CREDITS--
manuel <manuel@mausz.at>
--INI--
opcache.enable=1
opcache.enable_cli=1
disable_functions=dl
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(is_callable("dl"));
dl("a.so");
?>
--EXPECTF--
bool(true)

Warning: dl() has been disabled for security reasons in %sbug68104.php on line %d
