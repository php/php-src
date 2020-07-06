--TEST--
029: Name ambiguity (class name & import name)
--FILE--
<?php
use A\B as Foo;

class Foo {
}

new Foo();
--EXPECTF--
Fatal error: Class Foo cannot be declared because the name is already in use in %s on line %d
