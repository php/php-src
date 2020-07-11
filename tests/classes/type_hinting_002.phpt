--TEST--
ZE2 class type hinting non existing class
--FILE--
<?php

class Foo {
    function a(NonExisting $foo) {}
}

$o = new Foo;
$o->a($o);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Foo::a(): Argument #1 ($foo) must be of type NonExisting, Foo given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): Foo->a(Object(Foo))
#1 {main}
  thrown in %s on line %d
