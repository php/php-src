--TEST--
Bug #76451: Aliases during inheritance type checks affected by opcache
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
===DONE===
--EXPECT--
===DONE===
