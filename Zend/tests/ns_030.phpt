--TEST--
030: Name ambiguity (import name & class name)
--FILE--
<?php
class Foo {
}

use A\B as Foo;

new Foo();
?>
--EXPECTF--
Fatal error: Cannot import class A\B as Foo, Foo has already been declared in %s on line %d
