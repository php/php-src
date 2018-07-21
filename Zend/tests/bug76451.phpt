--TEST--
Aliases during inheritance type checks affected by opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache') || php_sapi_name() != "cli") die("skip CLI only"); ?>
--FILE--
<?php
require __DIR__ . "/bug76451.inc";

class A {
  public function test(Foo $foo) {}
}
class B extends A {
  public function test(Bar $foo) {}
}
?>
--EXPECT--
