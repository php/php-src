--TEST--
Bug #70632 (Third one of segfault in gc_remove_from_buffer)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.protect_memory=1
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die("skip"); ?>
--FILE--
<?php

class A {
	public function test($a = array("okey")) {
	}
}


eval ("class B extends A { public function test() {} }");
?>
--EXPECTF--
Strict Standards: Declaration of B::test() should be compatible with A::test($a = Array) in %sbug70632.php(%d) : eval()'d code on line %d
