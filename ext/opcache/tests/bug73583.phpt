--TEST--
Bug #73583 (Segfaults when conditionally declared class and function have the same name)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x4ff
opcache.file_update_protection=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
if (true) {
	class A { }
	function A() { }
	function A() { }
}
?>
--EXPECTF--
Fatal error: Cannot redeclare A() (previously declared in %sbug73583.php:4) in %sbug73583.php on line 5
