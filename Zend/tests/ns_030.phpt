--TEST--
030: Name ambiguity (import name & class name)
--FILE--
<?php
class Foo {
}

use A\B as Foo;

new Foo();
--EXPECTF--
Fatal error: Class 'A\B' not found in %s on line %d
