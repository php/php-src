--TEST--
029: Name ambiguity (class name & import name)
--FILE--
<?php
use A\B as Foo;

class Foo {
}

new Foo();
?>
--EXPECTF--
Fatal error: Class Foo has already been declared in %s on line %d
